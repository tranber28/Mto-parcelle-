#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    868E6

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
  display.print("LoRa Receiver 1");
  display.display();

  // set LoRa module pins
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  

  if (!LoRa.begin(868E6)) { // Adjust frequency to your region's settings
    Serial.println("Starting LoRa failed!");
    display.setCursor(0, 30);
    display.print("LoRa Failed");
    display.display();
    while (1);
  }
  Serial.println("LoRa Receiver ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");
    
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
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
    
    display.setCursor(0, 30);
    display.print("RSSI: ");
    display.print(LoRa.packetRssi());
    display.display();
  }
}
