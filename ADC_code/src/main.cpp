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

#define CHANNELS_TO_PRINT 32

int sensorData[32] = {0};

void setupADS7953(SPIClass ADS7953);
void readADS7953(SPIClass ADS7953, int adc);
void readADS7953_teleplot(SPIClass ADS7953, int adc);
void printADC();
void printDummySensor();

void setup()
{

  // Initialize serial communication for debugging
  Serial.begin(9600);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(RANGE_PIN, OUTPUT);
  digitalWrite(RANGE_PIN, HIGH);

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
  if (Serial.available() > 0)
  {
    char command = Serial.read();
    if (command == 'a')
    {
      readADS7953(ADS7953_1, 0);
      readADS7953(ADS7953_2, 1);
      printADC();
    }
    else if (command == 's')
    {
      printDummySensor();
    }
  }
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

void printADC()
{
  for (int i = 0; i < CHANNELS_TO_PRINT; i++)
  {
    char buffer[4];
    Serial.print("A");
    // print the channel number with leading zeros
    sprintf(buffer, "%02d", i);
    Serial.print(buffer);
    Serial.print("_");
    sprintf(buffer, "%04d", sensorData[i]);
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

void printDummySensor() {
    // Sensors: 1 Temp, 3 Mag, 3 Accel, 3 Gyro, 1 Humidity
    // Realistic test data
    float sensorData[] = {
        22.5,   // Temperature in degrees Celsius
        0.1,    // Magnetic field X in microteslas
        -0.2,   // Magnetic field Y in microteslas
        0.5,    // Magnetic field Z in microteslas
        0.01,   // Acceleration X in g's
        0.02,   // Acceleration Y in g's
        -0.03,  // Acceleration Z in g's
        0.1,    // Gyro rate X in degrees per second
        0.2,    // Gyro rate Y in degrees per second
        -0.1,   // Gyro rate Z in degrees per second
        45.0    // Humidity percentage
    };

    // Array of sensor labels
    const char* labels[] = {
        "T_", "Mx_", "My_", "Mz_", 
        "Ax_", "Ay_", "Az_", 
        "Gx_", "Gy_", "Gz_", 
        "H_"
    };

    // Buffer for formatted output
    char buffer[10];
    
    // Printing data in the format with a sign
    for (int i = 0; i < 11; i++) {
        Serial.print(labels[i]);
        
        sprintf(buffer, "%+06.2f", sensorData[i]);
        Serial.print(buffer);
        
        if (i < 10) { 
            Serial.print("_");
        }
    }
    
    Serial.println();
}
