#include <SPI.h>
#include <SoftSPI.h>

// Define SPI pins
#define SCK_PIN_ADS7953_1 PA_5
#define MISO_PIN_ADS7953_1 PA_6
#define MOSI_PIN_ADS7953_1 PA_7
#define CS_PIN_ADS7953_1 PA_4

#define SCK_PIN_ADS7953_2 PB_13
#define MISO_PIN_ADS7953_2 PB_14
#define MOSI_PIN_ADS7953_2 PB_15
#define CS_PIN_ADS7953_2 PB_12

#define SCK_PIN_ADS7953_3 PB_0
#define MISO_PIN_ADS7953_3 PB_1
#define MOSI_PIN_ADS7953_3 PB_2
#define CS_PIN_ADS7953_3 PB_10

SPIClass ADS7953_1;
SPIClass ADS7953_2;
SoftSPI ADS7953_3(MOSI_PIN_ADS7953_3, MISO_PIN_ADS7953_3, SCK_PIN_ADS7953_3);

#define CHANNELS_TO_PRINT 48

int sensorData[CHANNELS_TO_PRINT] = {0};
int order[CHANNELS_TO_PRINT] = {8, 9, 10, 11, 12, 13, 14, 15,
                                0, 1, 2, 3, 4, 5, 6, 7,
                                24, 25, 26, 27, 28, 29, 30, 31,
                                16, 17, 18, 19, 20, 21, 22, 23,
                                40, 41, 42, 43, 44, 45, 46, 47,
                                32, 33, 34, 35, 36, 37, 38, 39};

void setupADS7953(SPIClass ADS7953, int cs);
void setupSoftADS7953(SoftSPI ADS7953, int cs);
void readADS7953(SPIClass ADS7953, int adc, int cs);
void readSoftADS7953(SoftSPI ADS7953, int adc, int cs);
void printADC();
void JumpToBootloader();
void reset();

void setup()
{

    // Initialize serial communication for debugging
    Serial.begin(9600);

    pinMode(CS_PIN_ADS7953_1, OUTPUT);
    pinMode(CS_PIN_ADS7953_2, OUTPUT);
    pinMode(CS_PIN_ADS7953_3, OUTPUT);
    digitalWrite(CS_PIN_ADS7953_1, HIGH);
    digitalWrite(CS_PIN_ADS7953_2, HIGH);
    digitalWrite(CS_PIN_ADS7953_3, HIGH);

    ADS7953_1.begin();
    ADS7953_1.setMOSI(MOSI_PIN_ADS7953_1);
    ADS7953_1.setMISO(MISO_PIN_ADS7953_1);
    ADS7953_1.setSCLK(SCK_PIN_ADS7953_1);
    ADS7953_1.setClockDivider(SPI_CLOCK_DIV16);
    ADS7953_1.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
    ADS7953_1.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first
    setupADS7953(ADS7953_1, CS_PIN_ADS7953_1);

    ADS7953_2.begin();
    ADS7953_2.setMOSI(MOSI_PIN_ADS7953_2);
    ADS7953_2.setMISO(MISO_PIN_ADS7953_2);
    ADS7953_2.setSCLK(SCK_PIN_ADS7953_2);
    ADS7953_2.setClockDivider(SPI_CLOCK_DIV16);
    ADS7953_2.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
    ADS7953_2.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first
    setupADS7953(ADS7953_2, CS_PIN_ADS7953_2);

    ADS7953_3.begin();
    ADS7953_3.setDataMode(SPI_MODE0); // ADS7953 uses SPI Mode 0
    ADS7953_3.setBitOrder(MSBFIRST);  // ADS7953 expects MSB first
    setupSoftADS7953(ADS7953_3, CS_PIN_ADS7953_3);
}

void loop()
{
    if (Serial.available() > 0)
    {
        char command = Serial.read();
        if (command == 'a')
        {
            readADS7953(ADS7953_1, 0, CS_PIN_ADS7953_1);
            readADS7953(ADS7953_2, 2, CS_PIN_ADS7953_2);
            readSoftADS7953(ADS7953_3, 1, CS_PIN_ADS7953_3);
            printADC();
        }
        else if (command == 'd')
        {
            JumpToBootloader();
        } else if (command == 'r')
        {
            reset();
        }
    }
}

void setupADS7953(SPIClass ADS7953, int cs)
{
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);

    // Reset the device
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x4200);
    digitalWrite(cs, HIGH);

    // Set the device to Auto mode 2
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x3C40);
    digitalWrite(cs, HIGH);

    // Set last channel to be read to 15
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x93C0);
    digitalWrite(cs, HIGH);
}

void setupSoftADS7953(SoftSPI ADS7953, int cs)
{
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);

    // Reset the device
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x4200);
    digitalWrite(cs, HIGH);

    // Set the device to Auto mode 2
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x3C40);
    digitalWrite(cs, HIGH);

    // Set last channel to be read to 15
    digitalWrite(cs, LOW);
    ADS7953.transfer16(0x93C0);
    digitalWrite(cs, HIGH);
}

void readADS7953(SPIClass ADS7953, int adc, int cs)
{
    for (int i = 0; i < 16; i++)
    {
        digitalWrite(cs, LOW);
        uint16_t rawData = ADS7953.transfer16(0x0000);
        digitalWrite(cs, HIGH);

        uint16_t channel = (rawData & 0xF000) >> 12;
        channel = (channel + adc * 16);
        uint16_t adcValue = rawData & 0x0FFF;
        sensorData[channel] = adcValue;
    }
}

void readSoftADS7953(SoftSPI ADS7953, int adc, int cs)
{
    for (int i = 0; i < 16; i++)
    {
        digitalWrite(cs, LOW);
        uint16_t rawData = ADS7953.transfer16(0x0000);
        digitalWrite(cs, HIGH);

        uint16_t channel = (rawData & 0xF000) >> 12;
        channel = (channel + adc * 16);
        uint16_t adcValue = rawData & 0x0FFF;
        sensorData[channel] = adcValue;
    }
}

void printADC()
{
    for (int row = 0; row < 8; row++) // Columns 1-6
    {
        for (int col = 0; col < 6; col++) // Rows A-H
        {
            int index = col * 6 + row; // Corrected the indexing logic
            char buffer[4];
            Serial.print((char)('A' + row)); // Print row letter
            Serial.print(col + 1);          // Print column number
            Serial.print("_");
            sprintf(buffer, "%04d", sensorData[order[index]]);
            Serial.print(buffer);
            if (index < CHANNELS_TO_PRINT - 1)
            {
                Serial.print("_");
            }
            else
            {
                Serial.println();
            }
        }
    }
    // for (int i = 0; i < CHANNELS_TO_PRINT; i++)
    // {
    //     char buffer[4];
    //     Serial.print("A");
    //     Serial.print(i);
    //     Serial.print("_");
    //     sprintf(buffer, "%04d", sensorData[order[i]]);
    //     Serial.print(buffer);
    //     if (i < CHANNELS_TO_PRINT - 1)
    //     {
    //         Serial.print("_");
    //     }
    //     else
    //     {
    //         Serial.println();
    //     }
    // }
}

void reset()
{
    NVIC_SystemReset();
}

#define BOOT_ADDR 0x1FFFC800

struct boot_vectable_
{
    uint32_t Initial_SP;
    void (*Reset_Handler)(void);
};

#define BOOTVTAB ((struct boot_vectable_ *)BOOT_ADDR)

void JumpToBootloader(void)
{
    __disable_irq();

    USB->CNTR = 0x0003;

    SysTick->CTRL = 0;

    HAL_RCC_DeInit();

    for (int i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    __enable_irq();

    __set_MSP(BOOTVTAB->Initial_SP);

    BOOTVTAB->Reset_Handler();
}
