#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configuration de l'écran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuration LoRa
#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    868E6

// Configuration WiFi
const char* ssid = "maison2"; // Changez ici avec votre SSID WiFi
const char* password = "berber15"; // Changez ici avec votre mot de passe WiFi

// Configuration MQTT
const char* mqtt_server = "192.168.1.148"; // Vous pouvez changer cela par votre serveur MQTT
const char* mqtt_topic = "lora/received/";
const char* mqtt_client_id = "LoRaClient"; // ID du client MQTT
const int mqtt_port = 1883; // Port du serveur MQTT
const char* mqtt_user = "mqtt"; // Votre utilisateur MQTT
const char* mqtt_password = "berber15"; // Votre mot de passe MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialiser l'écran OLED avec l'adresse I2C 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Échec de l'allocation SSD1306"));
    for (;;);
  }
  display.display();
  delay(2000);  // Pause de 2 secondes
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Récepteur LoRa");
  display.display();

  // set LoRa module pins
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0); 
  
  if (!LoRa.begin(868E6)) { // Ajustez la fréquence selon votre région
    Serial.println("Démarrage de LoRa échoué!");
    display.setCursor(0, 30);
    display.print("Échec de LoRa");
    display.display();
    while (1);
  }
  Serial.println("Récepteur LoRa prêt");

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Paquet reçu : ");
    
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Reçu : ");
    
    String receivedData = "";
    // lire le paquet
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      Serial.print(c);
      receivedData += c;
    }

    display.print(receivedData);
    display.display();

    // afficher RSSI du paquet
    Serial.print(" avec RSSI ");
    Serial.println(LoRa.packetRssi());
    
    display.setCursor(0, 30);
    display.print("RSSI : ");
    display.print(LoRa.packetRssi());
    display.display();

    // Publier sur MQTT
    client.publish(mqtt_topic, receivedData.c_str());
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion à ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("connecté");
      client.subscribe(mqtt_topic); // Abonnement au topic MQTT
    } else {
      Serial.print("échec, rc=");
      Serial.print(client.state());
      Serial.println(" nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}
