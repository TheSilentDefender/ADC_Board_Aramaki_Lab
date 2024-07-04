// #include <SPI.h>

// // Define SPI pins
// #define CS_PIN PA_4
// #define SCK_PIN PA_5
// #define MISO_PIN PA_6
// #define MOSI_PIN PA_7

// void setup() {
//   // Initialize serial communication for debugging
//   Serial.begin(9600);

//   // Set up SPI
//   pinMode(CS_PIN, OUTPUT);
//   SPI.begin();
//   SPI.setClockDivider(SPI_CLOCK_DIV16); // Adjust as necessary for your application
//   SPI.setDataMode(SPI_MODE0);           // ADS7953 uses SPI Mode 0
//   SPI.setBitOrder(MSBFIRST);            // ADS7953 expects MSB first

//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer16(0x8000);  // Write x8000 to enter program mode 1
//   digitalWrite(CS_PIN, HIGH);
//   delay(200);

//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer16(0xFFFF);  // Write xFFFF to read all channels in mode 1
//   digitalWrite(CS_PIN, HIGH);
//   delay(200);

//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer16(0x2800);  // Write x2800 to enter mode 1
//   digitalWrite(CS_PIN, HIGH);
//   delay(200);
// }

// int adcPin = 0;
// void loop() {
//   if(adcPin >= 16) {
//     adcPin = 0;
//   }
//   digitalWrite(CS_PIN, LOW);
//   uint16_t rawData = SPI.transfer16(0x0000);
//   digitalWrite(CS_PIN, HIGH);

//   uint16_t adcValue = rawData & 0x0FFF;

//   // Convert the ADC value to a voltage
//   float voltage = (adcValue / 4095.0) * 5;

//   // Print the received data
//   Serial.print("ADC" + String(adcPin) + " Value: ");
//   Serial.print(adcValue);
//   Serial.print(" | Voltage: ");
//   Serial.print(voltage, 3);  // Print voltage with 3 decimal places
//   Serial.println(" V");
//   adcPin++;
//   delay(1000);
// }
