#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configurações do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configurações WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqtt_broker = "52.237.23.203";
const int mqtt_port = 1883;
const char* mqtt_topic_pub = "/TEF/mednotify001/attrs";
const char* mqtt_topic_sub = "/TEF/mednotify001/cmd/#";
const char* mqtt_client_id = "mednotify001_teste";

// Configurações FIWARE Orion
const String ORION_SERVER = "http://52.237.23.203:1026";
const String ENTITY_ID = "urn:ngsi-ld:mednotify:001";
const String FIWARE_SERVICE = "smart";
const String FIWARE_SERVICEPATH = "/";

// Pinos dos botões
const int button1 = 13;
const int button2 = 12;
const int button3 = 14;
const int button4 = 27;
const int button5 = 26;

// Variáveis de estado
String button_messages[5] = {"", "", "", "", ""}; // Inicializa vazio

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(115200);
  
  // Inicializa LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Conectando WiFi...");
  
  // Conecta WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  lcd.clear();
  lcd.print("Conectado!");
  delay(2000);
  
  // Configura MQTT
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  // Configura botões
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);

  // Verifica atributos inicialmente
  checkOrionAttributes();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("MQTT - Mensagem recebida: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(msg);

  // Atualiza o LCD com a mensagem recebida
  lcd.clear();
  lcd.print(msg);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("Conectado!");
      mqttClient.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

String getOrionAttribute(String attribute) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado");
    return "";
  }

  HTTPClient http;
  String url = ORION_SERVER + "/v2/entities/" + ENTITY_ID + "/attrs/" + attribute + "/value";
  
  http.begin(url);
  http.addHeader("Fiware-Service", FIWARE_SERVICE);
  http.addHeader("Fiware-ServicePath", FIWARE_SERVICEPATH);
  
  int httpCode = http.GET();
  String payload = "";
  
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    payload.trim();
    
    // Remove aspas se existirem
    if (payload.startsWith("\"") && payload.endsWith("\"")) {
      payload = payload.substring(1, payload.length()-1);
    }
    
    Serial.print("HTTP - " + attribute + " obtido: ");
    Serial.println(payload);
  } else {
    Serial.printf("HTTP - Falha ao obter %s. Código: %d\n", attribute.c_str(), httpCode);
    Serial.printf("Resposta: %s\n", http.getString().c_str());
  }
  
  http.end();
  return payload;
}

void checkOrionAttributes() {
  bool updated = false;
  
  for (int i = 0; i < 5; i++) {
    String attribute = "b" + String(i+1);
    String new_value = getOrionAttribute(attribute);
    
    if (new_value != "" && new_value != button_messages[i]) {
      button_messages[i] = new_value;
      updated = true;
      
      Serial.printf("%s atualizado para: %s\n", attribute.c_str(), new_value.c_str());
    }
  }
  
  if (updated) {
    lcd.clear();
    lcd.print("Atributos atualizados");
    delay(1000);
  }
}

void publishMQTT(int button_index) {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  String payload = "m|" + button_messages[button_index];
  mqttClient.publish(mqtt_topic_pub, payload.c_str());
  
  lcd.clear();
  lcd.print("Enviado b" + String(button_index+1) + ":");
  lcd.setCursor(0, 1);
  lcd.print(button_messages[button_index]);
  delay(500);
}

void loop() {
  static unsigned long lastCheck = 0;
  
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Verifica atributos no Orion a cada 10 segundos
  if (millis() - lastCheck >= 10000) {
    checkOrionAttributes();
    lastCheck = millis();
  }

  // Verifica botões
  if (digitalRead(button1) == LOW) {
    publishMQTT(0);
    delay(500);
  }
  if (digitalRead(button2) == LOW) {
    publishMQTT(1);
    delay(500);
  }
  if (digitalRead(button3) == LOW) {
    publishMQTT(2);
    delay(500);
  }
  if (digitalRead(button4) == LOW) {
    publishMQTT(3);
    delay(500);
  }
  if (digitalRead(button5) == LOW) {
    publishMQTT(4);
    delay(500);
  }
  
  delay(100);
}