#include <SPI.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3MDL.h>
#include <LSM6DSOSensor.h>
#include <Adafruit_SHT4x.h>
// Define SPI pins
#define CS_PIN PA_4
#define RANGE_PIN PB_12

#define SCK_PIN_ADS7953_1 PA_5
#define MISO_PIN_ADS7953_1 PA_6
#define MOSI_PIN_ADS7953_1 PA_7

#define SCK_PIN_ADS7953_2 PB_10
#define MISO_PIN_ADS7953_2 PB_14
#define MOSI_PIN_ADS7953_2 PB_15

// Define I2C bus
#define I2C_SDA PB_7
#define I2C_SCL PB_6

SPIClass ADS7953_1;
SPIClass ADS7953_2;
// TwoWire dev_i2c(I2C_SDA, I2C_SCL);
// Adafruit_SHT4x sht4 = Adafruit_SHT4x();
// Adafruit_LIS3MDL lis = Adafruit_LIS3MDL();    // Use I2C      
// LSM6DSOSensor lis3mdl = LSM6DSOSensor(&dev_i2c);

#define CHANNELS_TO_PRINT 16

int sensorData1[16] = {0};
int sensorData2[16] = {0};

void setupADS7953(SPIClass ADS7953, uint32_t MOSI, uint32_t MISO, uint32_t SCK);
void readADS7953(SPIClass ADS7953, int adc, int sensorData[]);
void readADS7953_teleplot(SPIClass ADS7953, int adc, int sensorData[]);
void printADC(int sensorData[], int sensorData2[]);
void setupSensors();
void readSensors();

void setup()
{
  // Initialize serial communication for debugging
  Serial.begin(9600);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  // dev_i2c.begin();

  setupADS7953(ADS7953_1, MOSI_PIN_ADS7953_1, MISO_PIN_ADS7953_1, SCK_PIN_ADS7953_1);
  setupADS7953(ADS7953_2, MOSI_PIN_ADS7953_2, MISO_PIN_ADS7953_2, SCK_PIN_ADS7953_2);
  setupSensors();
}

void loop()
{
  readADS7953(ADS7953_1, 0, sensorData1);
  readADS7953(ADS7953_2, 1, sensorData2);
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'read')
    {
      printADC(sensorData1, sensorData2);
      readSensors();
    }
  }
}

void setupADS7953(SPIClass ADS7953, uint32_t MOSI, uint32_t MISO, uint32_t SCK)
{
  ADS7953.begin();
  ADS7953.setMOSI(MOSI);
  ADS7953.setMISO(MISO);
  ADS7953.setSCLK(SCK);
  ADS7953.setClockDivider(SPI_CLOCK_DIV16);
  ADS7953.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
  ADS7953.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first

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

void setupSensors()
{
  // Initialize LIS3MDL sensor
  // if (!lis3mdl.begin_I2C())
  // {
  //   Serial.println("Could not find a valid LIS3MDL sensor, check wiring!");
  //   while (1);
  // }
  // Serial.println("LIS3MDL sensor found!");


  // SNT4x sensor
  // if (!sht4.begin())
  // {
  //   Serial.println("Couldn't find SHT4x");
  //   while (1)
  //     delay(1);
  // }
  // Serial.println("Found SHT4x sensor");
  // Serial.print("Serial number 0x");
  // Serial.println(sht4.readSerial(), HEX);
  // sht4.setPrecision(SHT4X_HIGH_PRECISION);
  // sht4.setHeater(SHT4X_NO_HEATER);
}

void readSensors()
{
  // TODO: Read sensors and print data
}



void readADS7953(SPIClass ADS7953, int adc, int sensorData[])
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

void printADC(int sensorData[], int sensorData2[])
{
  for (int i = 0; i < CHANNELS_TO_PRINT; i++)
  {
    char buffer[4];
    Serial.print("A");
    Serial.print(i);
    Serial.print("_");
    sprintf(buffer, "%04d", sensorData[i]);
    Serial.print(buffer);
    Serial.print("_");
    sprintf(buffer, "%04d", sensorData2[i]);
    Serial.print(buffer);
    if (i < CHANNELS_TO_PRINT - 1)
    {
      Serial.print("_");
    }
    else
    {
      Serial.println();
    }
  }
}