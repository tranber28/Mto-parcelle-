#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK  5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS   18
#define LORA_RST  12
#define LORA_IRQ  26

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LoRa module pins
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(868E6)) { // Adjust frequency to your region's settings
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Transmitter");
}

void loop() {
  Serial.print("Sending packet: ");

  // send packet
  LoRa.beginPacket();
  LoRa.print("Hello LoRa!");
  LoRa.endPacket();

  Serial.println("Hello LoRa!");

  delay(5000); // Send a packet every 5 seconds
}

