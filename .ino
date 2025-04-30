#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configurações do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD, largura 16 e altura 2

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST";
const char* default_PASSWORD = "";
const char* default_BROKER_MQTT = "52.237.23.203";
const int default_BROKER_PORT = 1883;
const char* default_message = "/TEF/mednotify001/attrs";
const char* default_button1 = "/TEF/mednotify001/cmd/b1"; // Tópico MQTT de Botão 1
const char* default_button2 = "/TEF/mednotify001/cmd/b2"; // Tópico MQTT de Botão 1
const char* default_button3 = "/TEF/mednotify001/cmd/b3"; // Tópico MQTT de Botão 1
const char* default_button4 = "/TEF/mednotify001/cmd/b4"; // Tópico MQTT de Botão 1
const char* default_button5 = "/TEF/mednotify001/cmd/b5"; // Tópico MQTT de Botão 1
const char* default_ID_MQTT = "mednotify001_teste";

// Variáveis de rede e MQTT
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* MESSAGE_PUBLISH = const_cast<char*>(default_message);
char* BUTTON1_SUBSCRIBE = const_cast<char*>(default_button1);
char* BUTTON2_SUBSCRIBE = const_cast<char*>(default_button2);
char* BUTTON3_SUBSCRIBE = const_cast<char*>(default_button3);
char* BUTTON4_SUBSCRIBE = const_cast<char*>(default_button4);
char* BUTTON5_SUBSCRIBE = const_cast<char*>(default_button5);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);

// Pinos dos botões
const int button1 = 13;  // Pino do botão 1
const int button2 = 12; // Pino do botão 2
const int button3 = 14;  // Pino do botão 1
const int button4 = 27; // Pino do botão 2
const int button5 = 26;  // Pino do botão 1

String button1_message = "emergencia";
String button2_message = "banana";
String button3_message = "emergencia";
String button4_message = "cozinha";
String button5_message = "agua";



const int D4 = 2; // Pino do LED onboard

// Variáveis para controle
String message = "Button Pressed"; // Mensagem a ser publicada quando o botão for pressionado
WiFiClient espClient;
PubSubClient MQTT(espClient);


void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Conteúdo: ");
  Serial.println(msg);

    lcd.clear();
    lcd.print(msg);
  if (String(topic).endsWith("b1")) {
    button1_message = msg;
  } else if (String(topic).endsWith("b2")) {
    button2_message = msg;
  }else if (String(topic).endsWith("b3")) {
    button3_message = msg;
  }else if (String(topic).endsWith("b4")) {
    button4_message = msg;
  }else{
    button2_message = msg;
    lcd.clear();
    lcd.print("CMD5: " + msg);
    }
}
// Função para inicialização do Wi-Fi
void initWiFi() {
    delay(10);
    Serial.println("Conectando ao Wi-Fi...");
    WiFi.begin(default_SSID, default_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nConectado ao Wi-Fi!");
}

// Função para inicializar o MQTT
void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(BUTTON1_SUBSCRIBE);
            MQTT.subscribe(BUTTON2_SUBSCRIBE);
            MQTT.subscribe(BUTTON3_SUBSCRIBE);
            MQTT.subscribe(BUTTON4_SUBSCRIBE);
            MQTT.subscribe(BUTTON5_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

// Função para exibir mensagens no LCD
void displayLCDMessage(String msg) {
    lcd.clear();
    lcd.print(msg); // Exibe a mensagem no LCD
}

// Função para publicar a mensagem no MQTT
void pressButton(int BUTTON_TYPE) {
    String payload;
    switch(BUTTON_TYPE){
      case 1:
        payload = "m|" + button1_message;  // Formato UltraLight
        MQTT.publish(MESSAGE_PUBLISH, payload.c_str());
        lcd.clear();
        lcd.print(button1_message);
        delay(500);
        break;
      case 2:
        payload = "m|" + button2_message;
        MQTT.publish(MESSAGE_PUBLISH, payload.c_str());
        lcd.clear();
        lcd.print(button2_message);
        delay(500);
        break;
      case 3:
        payload = "m|" + button3_message;
        MQTT.publish(MESSAGE_PUBLISH, payload.c_str());
        lcd.clear();
        lcd.print(button3_message);
        delay(500);
        break;
      case 4:
        payload = "m|" + button4_message;
        MQTT.publish(MESSAGE_PUBLISH, payload.c_str());
        lcd.clear();
        lcd.print(button4_message);
        delay(500);
        break;
      case 5:
        payload = "m|" + button5_message;  // Mantido consistente com outros botões
        MQTT.publish(MESSAGE_PUBLISH, payload.c_str());
        lcd.clear();
        lcd.print(button5_message);
        delay(500);
        break;   
    }
    Serial.print("Mensagem enviada: ");
    Serial.println(payload);
}

// Função para verificar os botões
void handleButtons() {
    if (digitalRead(button1) == LOW) { // Se o botão 1 for pressionado
      pressButton(1); // Publica a mensagem no MQTT
      delay(500); // Delay para evitar debounce
  }
    if (digitalRead(button2) == LOW) { // Se o botão de depuração for pressionado
      pressButton(2); // Publica a mensagem no MQTT
      delay(500); // Delay para evitar debounce
    }
      if (digitalRead(button3) == LOW) { // Se o botão de depuração for pressionado
      pressButton(3); // Publica a mensagem no MQTT
      delay(500); // Delay para evitar debounce
    }
      if (digitalRead(button4) == LOW) { // Se o botão de depuração for pressionado
      pressButton(4); // Publica a mensagem no MQTT
      delay(500); // Delay para evitar debounce
    }
      if (digitalRead(button5) == LOW) { // Se o botão de depuração for pressionado
      pressButton(5); // Publica a mensagem no MQTT
      delay(500); // Delay para evitar debounce
    }
}

// Função para verificar conexões Wi-Fi e MQTT
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected()) {
        reconnectMQTT();
    }
    if (WiFi.status() != WL_CONNECTED) {
        reconectWiFi();
    }
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nConectado ao Wi-Fi!");
}

void setup() {
    Serial.begin(115200);

    // Inicializa o LCD
    initWiFi();
    initMQTT();
    lcd.init();           // Inicializa o LCD
    lcd.backlight();      // Liga a luz de fundo
    lcd.clear();          // Limpa o display
    MQTT.setCallback(callback);
    pinMode(button1, INPUT_PULLUP); // Configura o pino do botão 1
    pinMode(button2, INPUT_PULLUP); // Configura o pino do botão de depuração
    pinMode(button3, INPUT_PULLUP); // Configura o pino do botão 1
    pinMode(button4, INPUT_PULLUP); // Configura o pino do botão de depuração
    pinMode(button5, INPUT_PULLUP); // Configura o pino do botão 1
    

    // Mensagem inicial no LCD
    lcd.print("Sistema Pronto!");
    delay(2000);
    lcd.clear();
}

void loop() {
    VerificaConexoesWiFIEMQTT(); // Verifica as conexões
    handleButtons(); // Verifica os botões
    MQTT.loop();     // Mantém a conexão MQTT ativa
}