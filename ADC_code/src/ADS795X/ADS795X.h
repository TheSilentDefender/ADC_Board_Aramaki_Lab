
#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>

typedef enum {
    MANUAL_MODE = 0b0001,
    AUTO1_MODE  = 0b0010,
    AUTO2_MODE  = 0b0011,
} channelSequencingMode;

typedef struct
{
    uint8_t reserved: 4;
    uint8_t output_address_mode: 1;
    uint8_t operation_mode: 1;
    uint8_t input_range: 1;
    uint8_t next_channel: 4;
    uint8_t enable_programming: 1;
    uint8_t mode: 4;
} bit_cmd_manual;


typedef struct
{
    uint8_t reserved_1: 4;
    uint8_t output_address_mode: 1;
    uint8_t operation_mode: 1;
    uint8_t input_range: 1;
    uint8_t reserved_2: 3;
    uint8_t reset_counter: 1;
    uint8_t enable_programming: 1;
    uint8_t mode: 4;
} bit_cmd_auto;

typedef union
{
    uint16_t cmd;
    bit_cmd_manual bit_access_manual;
    bit_cmd_auto bit_access_auto;
} controlRegisterMode;


class ADS795X {

    public:
        ADS795X(void);
        int8_t slave_select_pin;
        uint16_t auto1_active_channels;
        uint16_t auto2_last_channel_address;

        boolean initCommunication(SPIClass &spiPort = SPI, uint32_t clock = SPI_CLOCK_DIV4, uint8_t bitOrder = MSBFIRST, uint8_t dataMode = SPI_MODE0);
        void setFunctionalMode(controlRegisterMode regValue);
        uint16_t readData(uint8_t channel);
        
    private:

        SPIClass *_spiPort;
        channelSequencingMode operationMode;
        uint32_t _spiClock;
        uint8_t _spiBitOrder;
        uint8_t _spiDataMode;

        void enterProgramSequenceAuto1(void);
        void enterProgramSequenceAuto2(void);
};