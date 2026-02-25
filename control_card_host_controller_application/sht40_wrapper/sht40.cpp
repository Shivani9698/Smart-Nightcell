#include "sht40.h"
#include "main.h"
#include <cmath>

#include "usart_class.h"
extern usart debug;

/* SHT40 commands */
#define SHT40_CMD_MEASURE_HIGH     0xFD
#define SHT40_MEAS_DELAY_MS        15

SHT40::SHT40(i2c *i2c_bus, uint16_t i2c_address)
    : _i2c(i2c_bus), _addr(i2c_address)
{
}

uint8_t SHT40::crc8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0xFF;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
    }
    return crc;
}

SHT40::Status SHT40::read(int16_t &temperature_x10, uint8_t &humidity_percent)
{
    uint8_t cmd = SHT40_CMD_MEASURE_HIGH;
    uint8_t rx[6] = {0};

    _i2c->write(_addr, &cmd, 1);
    osDelay(SHT40_MEAS_DELAY_MS);
    _i2c->read(_addr, rx, 6);

    /* DEBUG: print raw bytes */
    debug.printf(
        "RAW: %02X %02X %02X %02X %02X %02X\r\n",
        rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]
    );

    if (crc8(&rx[0], 2) != rx[2]) return Status::CRC_ERROR;
    if (crc8(&rx[3], 2) != rx[5]) return Status::CRC_ERROR;

    uint16_t rawT  = (rx[0] << 8) | rx[1];
    uint16_t rawRH = (rx[3] << 8) | rx[4];

    float temperature = -45.0f + 175.0f * (rawT / 65535.0f);
    float humidity    = -6.0f  + 125.0f * (rawRH / 65535.0f);

    if (humidity < 0)   humidity = 0;
    if (humidity > 100) humidity = 100;

    temperature_x10  = (int16_t)(temperature * 10.0f);
    humidity_percent = (uint8_t)humidity;

    return Status::OK;
}
