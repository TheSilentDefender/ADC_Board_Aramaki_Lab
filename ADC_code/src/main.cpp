#include <SPI.h>
#include "DFRobot_OxygenSensor.h"

// Define SPI pins
#define CS_PIN PA_4
#define RANGE_PIN PB_12

#define SCK_PIN_ADS7951_1 PA_5
#define MISO_PIN_ADS7951_1 PA_6
#define MOSI_PIN_ADS7951_1 PA_7

#define SCK_PIN_ADS7951_2 PB_10
#define MISO_PIN_ADS7951_2 PB_14
#define MOSI_PIN_ADS7951_2 PB_15

SPIClass ADS7951_1;
SPIClass ADS7951_2;

void setup() {
  
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // Set up ADS7951_1
  ADS7951_1.begin();
  ADS7951_1.setMOSI(MOSI_PIN_ADS7951_1);
  ADS7951_1.setMISO(MISO_PIN_ADS7951_1);
  ADS7951_1.setSCLK(SCK_PIN_ADS7951_1);
  ADS7951_1.setClockDivider(SPI_CLOCK_DIV16); 
  ADS7951_1.setDataMode(SPI_MODE0);           // ADS7953 uses SPI Mode 0
  ADS7951_1.setBitOrder(MSBFIRST);            // ADS7953 expects MSB first

  digitalWrite(CS_PIN, LOW);
  ADS7951_1.transfer16(0x4200); // Reset the device
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7951_1.transfer16(0x3C00); // Set the device to Auto mode 2
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7951_1.transfer16(0x93C0); // Set last channel to be read to 15
  digitalWrite(CS_PIN, HIGH);

  // Set up ADS7951_2
  ADS7951_2.begin();
  ADS7951_2.setMOSI(MOSI_PIN_ADS7951_2);
  ADS7951_2.setMISO(MISO_PIN_ADS7951_2);
  ADS7951_2.setSCLK(SCK_PIN_ADS7951_2);
  ADS7951_2.setClockDivider(SPI_CLOCK_DIV16); 
  ADS7951_2.setDataMode(SPI_MODE0);           // ADS7953 uses SPI Mode 0
  ADS7951_2.setBitOrder(MSBFIRST);            // ADS7953 expects MSB first

  digitalWrite(CS_PIN, LOW);
  ADS7951_2.transfer16(0x4200); // Reset the device
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7951_2.transfer16(0x3C40); // Set the device to Auto mode 2
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7951_2.transfer16(0x93C0); // Set last channel to be read to 15
  digitalWrite(CS_PIN, HIGH);


  pinMode(RANGE_PIN, INPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readString();
    if (input == "reset") {
      NVIC_SystemReset();
    }
  }
  
  // read from ADS7951_1
  digitalWrite(CS_PIN, LOW);
  uint16_t rawData1 = ADS7951_1.transfer16(0x0000);
  digitalWrite(CS_PIN, HIGH);
  
  uint16_t channel1 = (rawData1 & 0xF000) >> 12;
  uint16_t adcValue1 = rawData1 & 0x0FFF;

  float voltage1 = (adcValue1 / 4095.0) * 5;

  // Print the received data
  Serial.print("Channel1: ");
  Serial.print(channel1);
  Serial.print(" | ADS7951_1 Value: ");
  Serial.print(adcValue1);
  Serial.print(" | ");
  Serial.print(voltage1, 3);
  Serial.println(" V");


  // read from ADS7951_2
  digitalWrite(CS_PIN, LOW);
  uint16_t rawData = ADS7951_2.transfer16(0x0000);
  digitalWrite(CS_PIN, HIGH);

  uint16_t channel2 = (rawData & 0xF000) >> 12;
  uint16_t adcValue2 = rawData & 0x0FFF;


  float voltage2 = (adcValue2 / 4095.0) * 5;

  // Print the received data
  Serial.print("Channel2: ");
  Serial.print(channel2);
  Serial.print(" | ADS7951_2 Value: ");
  Serial.print(adcValue2);
  Serial.print(" | ");
  Serial.print(voltage2, 3); 
  Serial.println(" V");
  Serial.println(digitalRead(RANGE_PIN));

  delay(1000);
}
  

