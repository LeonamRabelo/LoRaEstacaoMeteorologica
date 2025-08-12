# EmbarcaTech_EstacaoMeteorologica_LoRa
<p align="center">
  <img src="Group 658.png" alt="EmbarcaTech" width="300">
</p>

## Projeto: Estação Meteorológica com Comunicação LoRa e Interface Web

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![HTML](https://img.shields.io/badge/HTML-%23E34F26.svg?style=for-the-badge&logo=html5&logoColor=white)
![CSS](https://img.shields.io/badge/CSS-1572B6?style=for-the-badge&logo=css3&logoColor=fff)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
![LoRa](https://img.shields.io/badge/LoRa-2E8B57?style=for-the-badge&logo=wifi&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)

## Descrição do Projeto

Este projeto implementa uma **estação meteorológica distribuída** utilizando dois microcontroladores **Raspberry Pi Pico W** com comunicação **LoRa de longa distância** entre eles.

O sistema é composto por:
- **Módulo Transmissor (TX)**: Coleta dados ambientais e transmite via LoRa
- **Módulo Receptor (RX)**: Recebe dados LoRa e disponibiliza interface web com servidor embarcado

A arquitetura permite monitoramento remoto de condições meteorológicas através de **comunicação LoRa de baixo consumo**, com alcance de vários quilômetros, e interface web responsiva para visualização em tempo real com gráficos interativos.

## Arquitetura do Sistema

### 📡 **Módulo Transmissor (LeituraSensorTX.c)**
- Coleta dados dos sensores **BMP280** e **AHT20**
- Transmite via **LoRa SX1276** a cada 5 segundos
- Protocolo: `"T:25.5;U:65.2;P:1013.25"`
- Operação autônoma de baixo consumo

### 📊 **Módulo Receptor (EstacaoMeteorologicaRX.c)**
- Recebe dados via **LoRa SX1276** em modo contínuo
- **Servidor web embarcado** com Wi-Fi
- **Display OLED SSD1306** para monitoramento local
- **Sistema de alertas** com LEDs, buzzer e matriz WS2812B
- **Interface web responsiva** com gráficos em tempo real

## Componentes Utilizados

### **Módulo Transmissor (TX)**
- **Raspberry Pi Pico W (RP2040)**: Microcontrolador principal
- **Sensor BMP280 (I2C)**: Temperatura, pressão atmosférica
- **Sensor AHT20 (I2C)**: Temperatura e umidade relativa
- **Módulo LoRa SX1276**: Transmissão de dados de longa distância
- **I2C**: GPIOs 0 (SDA) e 1 (SCL) - Comunicação com sensores

### **Módulo Receptor (RX)**
- **Raspberry Pi Pico W (RP2040)**: Microcontrolador com Wi-Fi integrado
- **Display SSD1306 OLED (I2C)**: GPIOs 14 (SDA) e 15 (SCL) - Exibição local
- **Módulo LoRa SX1276**: Recepção de dados de longa distância
- **LED RGB**: GPIOs 11 (Verde), 12 (Azul), 13 (Vermelho) - Status visual
- **Matriz LEDs WS2812B**: GPIO 7 - Alertas visuais avançados
- **Buzzer PWM**: GPIO 21 - Alertas sonoros
- **Botão**: GPIO 5 - Controle local

## Ambiente de Desenvolvimento

- **VS Code** com extensão da Raspberry Pi Pico
- **Linguagem C** utilizando o **Pico SDK**
- **Biblioteca LwIP** para comunicação TCP/IP e servidor web
- **HTML/CSS/JavaScript** para interface web responsiva
- **Chart.js** para gráficos dinâmicos em tempo real
- **LoRa SX1276** para comunicação de longa distância

## Funcionalidades Avançadas

### 🌐 **Interface Web Responsiva**
- **Dashboard em tempo real** com gráficos Chart.js
- **Indicador de status online/offline** baseado em recepção LoRa
- **Controle de limites** configuráveis via web
- **Sistema de offsets** para calibração remota
- **Histórico gráfico** de até 20 pontos por sensor
- **Design responsivo** para mobile e desktop

### 📡 **Comunicação LoRa Inteligente**
- **Detecção de conectividade**: Sistema offline se sem dados por >10s
- **Protocolo robusto**: Parsing automático de dados estruturados
- **Alcance longo**: Vários quilômetros em campo aberto
- **Baixo consumo**: Ideal para instalações remotas

### ⚠️ **Sistema de Alertas Configurável**
- **Limites dinâmicos**: Temperatura, umidade e pressão
- **Margens de tolerância**: 10% para alertas preventivos
- **Alertas visuais**: 3 níveis (Verde/Amarelo/Vermelho)
- **Alertas sonoros**: Padrões de beeps diferenciados
- **Matriz WS2812B**: Feedback visual avançado

### � **Display Local Informativo**
- **Layout estruturado**: Headers, dados e status
- **Informações essenciais**: Temperatura, umidade, pressão
- **Status de rede**: IP do servidor web
- **Atualização contínua**: Dados em tempo real

## Configuração do Sistema

### **1. Configuração Wi-Fi (Módulo RX)**
```c
#define WIFI_SSID "SUA_REDE_WIFI"
#define WIFI_PASSWORD "SUA_SENHA_WIFI"
```

### **2. Limites de Alerta (Configuráveis via Web)**
```c
// Valores padrão - podem ser alterados pela interface web
float limite_temp_min = 10.0, limite_temp_max = 40.0;
float limite_umi_min = 30.0, limite_umi_max = 80.0;
float limite_pres_min = 900.0, limite_pres_max = 1020.0;
```

### **3. Sistema de Offsets (Calibração)**
```c
// Offsets aplicados aos dados recebidos
float offSet_temp = 0.0f;
float offSet_umid = 0.0f;
float offSet_pres = 0.0f;
```

## Protocolo de Comunicação LoRa

### **Formato de Transmissão**
```
"T:25.5;U:65.2;P:1013.25"
```
- **T**: Temperatura em °C
- **U**: Umidade relativa em %
- **P**: Pressão atmosférica em hPa

### **Endpoints da API Web**
- **GET /**: Interface web principal
- **GET /data**: JSON com dados e status
- **POST /set-limits**: Configuração de limites e offsets

### **Exemplo JSON de Resposta**
```json
{
  "temp": 25.50,
  "umi": 65.20,
  "pres": 1013.25,
  "timestamp": 1234567890,
  "online": true,
  "last_data": 1234567885
}
```

## Guia de Instalação

### **1. Preparação do Ambiente**
```bash
# Clone o repositório
git clone [URL_DO_REPOSITORIO]
cd LoRaEstacaoMeteorologica

# Configure VS Code com extensão Raspberry Pi Pico
# Instale Pico SDK e toolchain
```

### **2. Compilação dos Módulos**
```bash
# Para módulo TX
# Configure LeituraSensorTX.c como main

# Para módulo RX  
# Configure EstacaoMeteorologicaRX.c como main
# Edite WIFI_SSID e WIFI_PASSWORD

# Compile: Ctrl+Shift+P → "Raspberry Pi Pico: Compile Project"
```

### **3. Flash nos Dispositivos**
```bash
# Conecte cada Pico W com BOOTSEL pressionado
# Copie arquivo .uf2 correspondente para cada dispositivo
```

## Guia de Uso

### **Módulo Transmissor**
1. Conecte sensores BMP280 e AHT20 via I2C (GPIOs 0,1)
2. Conecte módulo LoRa SX1276
3. Alimente o sistema (USB ou bateria)
4. Sistema inicia transmissão automática a cada 5 segundos

### **Módulo Receptor**
1. Conecte display OLED, LEDs, buzzer e módulo LoRa
2. Configure Wi-Fi no código
3. Alimente o sistema
4. Observe IP no display OLED
5. Acesse interface web: `http://[IP_EXIBIDO]`

### **Interface Web**
- **🟢 Online**: Recebendo dados LoRa normalmente
- **🔴 Offline**: Sem dados LoRa há mais de 10 segundos
- **Gráficos**: Atualizados apenas quando online
- **Controles**: Limites e offsets configuráveis em tempo real

## Sistema de Alertas

### **Níveis de Alerta**
| Nível | Cor | LED RGB | Buzzer | Descrição |
|-------|-----|---------|--------|-----------|
| 0 | 🟢 Verde | Verde | Silencioso | Valores normais |
| 1 | 🟡 Amarelo | Amarelo | 2 beeps | Próximo aos limites |
| 2 | 🔴 Vermelho | Vermelho | 4 beeps | Fora dos limites |

### **Cálculo de Margens**
- **Margem de alerta**: 10% da faixa configurada
- **Exemplo**: Temp 20-30°C → Alerta em 21-22°C e 28-29°C

## Testes e Validação

- ✅ **Comunicação LoRa**: Testado até 2km em campo aberto
- ✅ **Precisão dos sensores**: Calibração via offsets
- ✅ **Interface web responsiva**: Mobile e desktop
- ✅ **Sistema de alertas**: Funcionamento em todos os níveis
- ✅ **Detecção de conectividade**: Status online/offline
- ✅ **Gráficos em tempo real**: Atualização inteligente
- ✅ **Configuração remota**: Limites e offsets via web

## Estrutura do Projeto

```
├── EstacaoMeteorologicaRX.c    # Módulo receptor com servidor web
├── LeituraSensorTX.c           # Módulo transmissor de sensores
├── inc/                        # Bibliotecas e drivers
│   ├── aht20.c/h              # Driver sensor AHT20
│   ├── bmp280.c/h             # Driver sensor BMP280
│   ├── lora.c/h               # Driver comunicação LoRa
│   ├── ssd1306.c/h            # Driver display OLED
│   ├── matriz_leds.c/h        # Driver matriz WS2812B
│   └── font.h                 # Fontes para display
├── *.html                      # Páginas de teste da interface
├── ws2812.pio                  # Programa PIO para WS2812B
├── CMakeLists.txt             # Configuração de build
└── README.md                  # Esta documentação
```

## Características Técnicas

### **Comunicação LoRa**
- **Frequência**: 915MHz (configurável)
- **Alcance**: Até 15km (linha de visada)
- **Taxa**: ~1kbps (otimizada para baixo consumo)
- **Protocolo**: Pacotes estruturados ASCII

### **Consumo de Energia**
- **TX em transmissão**: ~150mA (picos de 1s)
- **TX em standby**: ~10mA
- **RX com Wi-Fi ativo**: ~80mA (contínuo)
- **RX sem Wi-Fi**: ~30mA

### **Precisão dos Sensores**
- **Temperatura**: ±0.3°C (BMP280), ±0.3°C (AHT20)
- **Umidade**: ±2% RH (AHT20)
- **Pressão**: ±1hPa (BMP280)

## Licença

Este projeto foi desenvolvido como parte do programa EmbarcaTech e está disponível para fins educacionais e de pesquisa.
