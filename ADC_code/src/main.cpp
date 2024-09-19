#include <SPI.h>
#include <CRC.h>

// Define SPI pins
#define CS_PIN PA_4
#define RANGE_PIN PB_12

#define SCK_PIN_ADS7953_1 PA_5
#define MISO_PIN_ADS7953_1 PA_6
#define MOSI_PIN_ADS7953_1 PA_7

#define SCK_PIN_ADS7953_2 PB_10
#define MISO_PIN_ADS7953_2 PB_14
#define MOSI_PIN_ADS7953_2 PB_15

SPIClass ADS7953_1;
SPIClass ADS7953_2;


#define CHANNELS_TO_PRINT 32

uint16_t sensorData[32] = {0};

void setupADS7953(SPIClass ADS7953);
void readADS7953(SPIClass ADS7953, int adc);
void readADS7953_teleplot(SPIClass ADS7953, int adc);
float ADCToTemp(int adc);
void printData();

void setup()
{

  // Initialize serial communication for debugging
  Serial.begin(9600);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);


  ADS7953_1.begin();
  ADS7953_1.setMOSI(MOSI_PIN_ADS7953_1);
  ADS7953_1.setMISO(MISO_PIN_ADS7953_1);
  ADS7953_1.setSCLK(SCK_PIN_ADS7953_1);
  ADS7953_1.setClockDivider(SPI_CLOCK_DIV16);
  ADS7953_1.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
  ADS7953_1.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first
  setupADS7953(ADS7953_1);

  ADS7953_2.begin();
  ADS7953_2.setMOSI(MOSI_PIN_ADS7953_2);
  ADS7953_2.setMISO(MISO_PIN_ADS7953_2);
  ADS7953_2.setSCLK(SCK_PIN_ADS7953_2);
  ADS7953_2.setClockDivider(SPI_CLOCK_DIV16);
  ADS7953_2.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
  ADS7953_2.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first
  setupADS7953(ADS7953_2);
}

void loop()
{
 if(Serial.available() > 0)
  {
    char command = Serial.read();
    if(command == 0x11)
    {
      readADS7953(ADS7953_1, 0);
      readADS7953(ADS7953_2, 1);
      printData();
    }
  }
  delay(1000);
}

void setupADS7953(SPIClass ADS7953)
{
  digitalWrite(CS_PIN, LOW);
  ADS7953.transfer16(0x4200); // Reset the device
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7953.transfer16(0x3C40); // Set the device to Auto mode 2
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  ADS7953.transfer16(0x93C0); // Set last channel to be read to 15
  digitalWrite(CS_PIN, HIGH);
}

void readADS7953(SPIClass ADS7953, int adc)
{
    for (int i = 0; i < 16; i++)
    {
      digitalWrite(CS_PIN, LOW);
      uint16_t rawData = ADS7953.transfer16(0x0000);
      digitalWrite(CS_PIN, HIGH);

      uint16_t channel = (rawData & 0xF000) >> 12;
      channel = (channel + adc * 16);
      printf("Channel: %d\n", channel);
      uint16_t adcValue = rawData & 0x0FFF;
      sensorData[channel] = adcValue;
    }
}

void printData()
{
  // Define header, footer, and CRC variables
  uint8_t header = 0xFF;
  uint8_t footer = 0xFF;
  uint8_t crc = 0;

  // Start with the header
  Serial.write(header);

  // Send the channel data (4 bits for channel + 12 bits for data) in binary format
  for (int i = 0; i < CHANNELS_TO_PRINT; i++)
  {
    uint16_t channel = (i & 0x0F) << 12;        // Channel number in the upper 4 bits
    uint16_t data = sensorData[i] & 0x0FFF;     // ADC data in the lower 12 bits
    uint16_t packet = channel | data;           // Combine the channel and data

    // Send the 16-bit packet as two bytes (MSB first)
    Serial.write((packet >> 8) & 0xFF);         // Send high byte
    Serial.write(packet & 0xFF);                // Send low byte

    // Update CRC with the 16-bit data (split into two 8-bit values)
    crc = CRC8(reinterpret_cast<uint8_t*>(&packet), 2, crc);
  }

  // Send the CRC
  Serial.write(crc);

  // Send the footer
  Serial.write(footer);
}