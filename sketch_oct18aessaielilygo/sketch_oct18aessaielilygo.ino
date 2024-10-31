#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define I2C_SDA 21 // I2C pins for this board
#define I2C_SCL 22

#define LORA_SCK  5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS   18
#define LORA_RST  14
#define LORA_IRQ  26

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize OLED display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);  // Pause for 2 seconds
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("LoRa Receiver");
  display.display();

  // set LoRa module pins
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(868E6)) { // Adjust frequency to your region's settings
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Receiver");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Received: ");
    
    String receivedData = "";
    // read packet
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      Serial.print(c);
      receivedData += c;
    }

    display.print(receivedData);
    display.display();

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    
    display.setCursor(0, 30);
    display.print("RSSI: ");
    display.print(LoRa.packetRssi());
    display.display();
  }
}
