#include <SPI.h>

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


#define CHANNELS_TO_PRINT 16

int sensorData[16] = {0};

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
  readADS7953(ADS7953_2, 0);
  printData();
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
      uint16_t adcValue = rawData & 0x0FFF;
      sensorData[channel] = adcValue;
    }
}

void printData()
{
  sensorData[1] = sensorData[6];
  for (int i = 0; i < CHANNELS_TO_PRINT; i++)
  {
    char buffer[4];
    Serial.print("A");
    Serial.print(i);
    Serial.print("_");
    sprintf(buffer, "%04d", sensorData[i]);
    Serial.print(buffer);
    if (i < CHANNELS_TO_PRINT - 1)
    {
      Serial.print("_");
    } else {
      Serial.println();
    }
  }
}