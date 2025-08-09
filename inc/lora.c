#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lora.h"

//Definições de pinos e porta SPI
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20

//Funções internas
static inline void cs_select()   { gpio_put(PIN_CS, 0); }   //Coloca o CS em 0, ou seja, ativo
static inline void cs_deselect() { gpio_put(PIN_CS, 1); }   //Coloca o CS em 1, ou seja, inativo (encerra a transmissão)

//Escreve no registrador passado por parametro
static void lora_writeRegister(uint8_t reg, uint8_t data){
    uint8_t buf[2] = { reg | 0x80, data };  //1000 do 0x80 para indicar escrita, bit 7 como 1 (escrita)
    cs_select();
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect();
    sleep_ms(1);
}

//Le o registrador passado por parametro
static uint8_t lora_readRegister(uint8_t addr){
    uint8_t data;
    addr &= 0x7F;   //garante o bit mais alto como 0 (leitura)
    cs_select();
    spi_write_blocking(SPI_PORT, &addr, 1);
    spi_read_blocking(SPI_PORT, 0, &data, 1);
    cs_deselect();
    sleep_ms(1);
    return data;
}

//Reseta o modulo (No LoRa, 0 significa reset, logo devemos deixar em 1 para utilizar o modulo)
static void lora_reset(void){
    gpio_put(PIN_RST, 0);
    sleep_ms(10);
    gpio_put(PIN_RST, 1);
    sleep_ms(10);
}

//Configura a frequência, escrevendo os registradores correspondentes para ajustar o radio para a frequência desejada
static void lora_setFrequency(double freqMHz){
    unsigned long freqReg = (unsigned long)(freqMHz * 7110656 / 434);
    lora_writeRegister(REG_FRF_MSB, (freqReg >> 16) & 0xFF);
    lora_writeRegister(REG_FRF_MID, (freqReg >> 8) & 0xFF);
    lora_writeRegister(REG_FRF_LSB, freqReg & 0xFF);
}

//Funções públicas
void lora_init(void){   //Função de inicialização do LoRa
    //Inicializa SPI e GPIOs
    spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    lora_reset();

    //Modo Sleep
    lora_writeRegister(REG_OPMODE, RF95_MODE_SLEEP);
    //Habilita LoRa
    lora_writeRegister(REG_OPMODE, 0x80);

    //Frequência
    lora_setFrequency(915.0);

    //Configurações de Modem
    lora_writeRegister(REG_MODEM_CONFIG,   BANDWIDTH_125K | ERROR_CODING_4_5 | EXPLICIT_MODE);
    lora_writeRegister(REG_MODEM_CONFIG2,  SPREADING_7 | CRC_ON);
    lora_writeRegister(REG_MODEM_CONFIG3,  0x04); // LDRO off, AGC on

    //Preâmbulo
    lora_writeRegister(REG_PREAMBLE_MSB, 0x00);
    lora_writeRegister(REG_PREAMBLE_LSB, 0x08);

    //Payload máximo
    lora_writeRegister(REG_PAYLOAD_LENGTH, 255);

    //FIFO TX base
    lora_writeRegister(REG_FIFO_TX_BASE_AD, 0x00);

    printf("LoRa inicializado em 915 MHz.\n");
}

//Função de envio
void lora_send(const char *msg){
    uint8_t len = strlen(msg);
    if (len > 255) len = 255; //Tamanho máximo permitido

    //Ponteiro FIFO para base TX
    lora_writeRegister(REG_FIFO_ADDR_PTR, 0x00);

    //Escreve no FIFO
    for (int i = 0; i < len; i++) {
        lora_writeRegister(REG_FIFO, msg[i]);
    }

    //Define tamanho payload
    lora_writeRegister(REG_PAYLOAD_LENGTH, len);

    //Entra em modo TX
    lora_writeRegister(REG_OPMODE, RF95_MODE_TX);

    //Aguarda envio
    while ((lora_readRegister(REG_IRQ_FLAGS) & 0x08) == 0);
    lora_writeRegister(REG_IRQ_FLAGS, 0x08); // limpa flag

    printf("LoRa TX -> %s\n", msg);
}

//Função que coloca o SX1276 em modo RX contínuo
void lora_setModeRx(void){
    lora_writeRegister(REG_OPMODE, RF95_MODE_RX_CONTINUOUS);
}

//Função de recepção para o RX
bool lora_receive(char *buf, int maxlen){
    //Verifica se há dado recebido (bit RX_DONE no REG_IRQ_FLAGS)
    if(lora_readRegister(REG_IRQ_FLAGS) & 0x40){
        //Ponteiro FIFO para onde o pacote está
        uint8_t fifo_addr = lora_readRegister(REG_FIFO_RX_CURRENT_ADDR);
        lora_writeRegister(REG_FIFO_ADDR_PTR, fifo_addr);

        //Quantos bytes chegaram
        uint8_t length = lora_readRegister(REG_RX_NB_BYTES);
        if(length > maxlen) length = maxlen;

        //Lê byte a byte
        for(int i = 0; i < length; i++){
            buf[i] = lora_readRegister(REG_FIFO);
        }
        buf[length] = '\0'; //Finaliza string

        //Limpa flag RX_DONE
        lora_writeRegister(REG_IRQ_FLAGS, 0x40);

        return true; //Recebimento com sucesso
    }
    return false; //Nenhum dado
}