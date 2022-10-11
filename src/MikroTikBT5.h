/*
 *  Name:       MikroTikBT5.h
 *  Created:    2022-10-11
 *  Author:     Anrijs Jargans <anrijs@anrijs.lv>
 *  Url:        https://github.com/Anrijs/MikroTik-BT5-ESP32
 */

#ifndef __MIKROTIKBT5_H
#define __MIKROTIKBT5_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MIKROTIK_MANUFACTURER_ID  0x094F

#define BT5_FLAG_REED_SWITCH      0x01
#define BT5_FLAG_ACCEL_TILT       0x02
#define BT5_FLAG_ACCEL_FREE_FALL  0x04
#define BT5_FLAG_IMPACT_X         0x08
#define BT5_FLAG_IMPACT_Y         0x10
#define BT5_FLAG_IMPACT_Z         0x20
#define BT5_FLAG_IMPACT           BT5_FLAG_IMPACT_Y | BT5_FLAG_IMPACT_Y | BT5_FLAG_IMPACT_Z

class MikroTikBeacon {
public:
    uint8_t version;
    uint8_t userdata;
    uint16_t salt;
    struct {
        float x;
        float y;
        float z;
    } acceleration;
    float temperature;
    uint32_t uptime;
    uint8_t flags;
    uint8_t battery;

    void unpack(uint8_t* data, size_t len);
    bool isValid();
    bool hasTemperature();

private:
    // datatype processing
    uint16_t _getU16(uint8_t *data);
    uint32_t _getU32(uint8_t *data);
    float _getFP88(uint8_t *data);
};

#endif