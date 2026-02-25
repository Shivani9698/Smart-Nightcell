#ifndef SHT40_H
#define SHT40_H

#include <stdint.h>
#include "i2c.h"


class SHT40
{
public:
    enum class Status
    {
        OK,
        CRC_ERROR,
        I2C_ERROR
    };

    /**
     * @param i2c_bus Pointer to your i2c wrapper object
     * @param i2c_address 8-bit I2C address (example: 0x44 << 1)
     */
    SHT40(i2c *i2c_bus, uint16_t i2c_address);
    Status read(int16_t &temperature_x10, uint8_t &humidity_percent);

private:
    i2c *_i2c;
    uint16_t _addr;

    uint8_t crc8(const uint8_t *data, uint16_t len);
};

#endif /* SHT40_H */
