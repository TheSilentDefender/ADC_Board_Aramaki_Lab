#include "ADS795X.h"

#define HIGH_BYTE                                   (0xFF00)
#define LOW_BYTE                                    (0x00FF)


#define CMD_AUTO1_PROGRAMMING_SEQUENCE              (1 << 15)
#define CMD_AUTO2_PROGRAMMING_SEQUENCE              ((1 << 15) | (1 << 12))

ADS795X::ADS795X()
{
    // Constructor
    auto1_active_channels = 0x00;
    auto2_last_channel_address = 0x00;
    slave_select_pin = -1;
}

void ADS795X::enterProgramSequenceAuto1(void)
{
    _spiPort->beginTransaction(SPISettings(_spiClock, _spiBitOrder, _spiDataMode));

    digitalWrite(slave_select_pin, LOW);

    _spiPort->transfer((uint8_t) (CMD_AUTO1_PROGRAMMING_SEQUENCE & HIGH_BYTE));

    _spiPort->transfer((uint8_t) (CMD_AUTO1_PROGRAMMING_SEQUENCE & LOW_BYTE));

    _spiPort->transfer((uint8_t) (auto1_active_channels & HIGH_BYTE));
    
    _spiPort->transfer((uint8_t) (auto1_active_channels & LOW_BYTE));

    digitalWrite(slave_select_pin, HIGH);

    _spiPort->endTransaction();
}

void ADS795X::enterProgramSequenceAuto2(void)
{
    uint32_t cmd = (CMD_AUTO2_PROGRAMMING_SEQUENCE | (auto2_last_channel_address << 6));
    _spiPort->beginTransaction(SPISettings(_spiClock, _spiBitOrder, _spiDataMode));

    digitalWrite(slave_select_pin, LOW);

    _spiPort->transfer((uint8_t) (cmd & HIGH_BYTE));

    _spiPort->transfer((uint8_t) (cmd & LOW_BYTE));

    digitalWrite(slave_select_pin, HIGH);

    _spiPort->endTransaction();
}

boolean ADS795X::initCommunication(SPIClass &spiPort, uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
{
    boolean statusCode = false;

    if(slave_select_pin == -1)
    {
        goto exit;
    }

    operationMode = MANUAL_MODE;

    _spiPort = &spiPort;
    _spiClock = clock;
    _spiBitOrder = bitOrder;
    _spiDataMode = dataMode;
    
    digitalWrite(slave_select_pin, HIGH);

    _spiPort->begin();

    if(auto1_active_channels != 0x00)
    {
        enterProgramSequenceAuto1();
    }

    if(auto2_last_channel_address != 0x00)
    {
        enterProgramSequenceAuto2();
    }

    statusCode = true;

exit:

    return statusCode;
}


void ADS795X::setFunctionalMode(controlRegisterMode regValue)
{
    _spiPort->beginTransaction(SPISettings(_spiClock, _spiBitOrder, _spiDataMode));
    
    digitalWrite(slave_select_pin, LOW);

    _spiPort->transfer((uint8_t) (regValue.cmd & HIGH_BYTE));

    _spiPort->transfer((uint8_t) (regValue.cmd & LOW_BYTE));

    digitalWrite(slave_select_pin, HIGH);

    _spiPort->endTransaction();

operationMode = static_cast<channelSequencingMode>((regValue.cmd >> 12) & 0x0F);
}

uint16_t ADS795X::readData(uint8_t channel)
{
    uint16_t data_rx;
    controlRegisterMode regValue;

    regValue.cmd = operationMode << 12;

    if(operationMode == MANUAL_MODE)
    {
        regValue.bit_access_manual.enable_programming = 0x00;
        regValue.bit_access_manual.next_channel = channel & 0x0F;
    }
    else if((operationMode == AUTO1_MODE) || (operationMode == AUTO2_MODE))
    {
        regValue.bit_access_auto.enable_programming = 0x00;
        regValue.bit_access_auto.reset_counter = 0x00;
    }
    else
    {
        regValue.cmd = (uint16_t) 0x00;
    }

    _spiPort->beginTransaction(SPISettings(_spiClock, _spiBitOrder, _spiDataMode));
    
    digitalWrite(slave_select_pin, LOW);

    data_rx = _spiPort->transfer((uint8_t) (regValue.cmd & HIGH_BYTE));

    data_rx |= _spiPort->transfer((uint8_t) (regValue.cmd & LOW_BYTE)) << 8;

    digitalWrite(slave_select_pin, HIGH);

    _spiPort->endTransaction();

    return data_rx;
}