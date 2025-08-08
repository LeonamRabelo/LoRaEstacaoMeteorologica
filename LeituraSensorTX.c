#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "ws2812.pio.h"
#include "inc/matriz_leds.h"
#include "aht20.h"
#include "bmp280.h"
#include "lora.h"

//Definição de pinagem
#define I2C_PORT_SENSOR i2c0
#define I2C_PORT_DISPLAY i2c1
#define I2C_SDA_SENSOR 0    // Pino SDA - Dados
#define I2C_SCL_SENSOR 1    // Pino SCL - Clock
#define IS_RGBW false // Maquina PIO para RGBW

float temperatura = 0, umidade = 0, pressao = 0;            //Variáveis para armazenar os valores de temperatura, umidade e pressão

AHT20_Data dados_aht;   //Estrutura para armazenar os dados do sensor
struct bmp280_calib_param bmp_params;   //Estrutura para armazenar os parâmetros do sensor


// Função para modularizar a inicialização do hardware
void inicializar_componentes(){
    stdio_init_all();

    //Configura o I2C na porta i2c0 para os sensores
    i2c_init(I2C_PORT_SENSOR, 400 * 1000);
    gpio_set_function(I2C_SDA_SENSOR, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_SENSOR, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_SENSOR); gpio_pull_up(I2C_SCL_SENSOR);
    //bmp280
    bmp280_init(I2C_PORT_SENSOR);
    bmp280_get_calib_params(I2C_PORT_SENSOR, &bmp_params);
    //aht20
    aht20_reset(I2C_PORT_SENSOR);
    aht20_init(I2C_PORT_SENSOR);

    //LoRa
    lora_init();
}

int main(){
    inicializar_componentes(); //Inicia os componentes

    //Estrutura para armazenar os dados do sensor
    AHT20_Data data;
    int32_t raw_temp_bmp;
    int32_t raw_pressure;

    while(true){
        //Leitura do BMP280
        bmp280_read_raw(I2C_PORT_SENSOR, &raw_temp_bmp, &raw_pressure);
        float temperatura_bmp = bmp280_convert_temp(raw_temp_bmp, &bmp_params);
        pressao = (bmp280_convert_pressure(raw_pressure, raw_temp_bmp, &bmp_params) / 100.0f);
        //Leitura do AHT20
        if(aht20_read(I2C_PORT_SENSOR, &data)){
            printf("Temperatura AHT: %.2f C\n", data.temperature);
            printf("Umidade: %.2f %%\n\n\n", data.humidity);
            umidade = data.humidity;
        }
        else{
            printf("Erro na leitura do AHT20!\n\n\n");
        }

        //Para enviar para o html, utilizei a media da soma dos dois sensores e jogar no grafico
        temperatura = (((temperatura_bmp / 100.0f) + data.temperature) / 2.0f); 

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "T:%.2f;U:%.2f;P:%.2f", temperatura, umidade, pressao);
        lora_send(buffer);
        printf("TX -> %s\n", buffer);
        sleep_ms(5000); //Envia a cada 5 segundos
    }
    return 0;
}