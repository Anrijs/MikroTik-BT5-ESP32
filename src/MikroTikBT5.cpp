#include "MikroTikBT5.h"

uint16_t MikroTikBeacon::_getU16(uint8_t *data) {
    uint16_t u16;
    memcpy(&u16, (void*) data, sizeof(u16));
    return u16;
}

uint32_t MikroTikBeacon::_getU32(uint8_t *data) {
    uint32_t u32;
    memcpy(&u32, (void*) data, sizeof(u32));
    return u32;
}

float MikroTikBeacon::_getFP88(uint8_t *data) {
    int16_t u88 = (int16_t) _getU16(data);
    return u88 / 256.0;
}

void MikroTikBeacon::unpack(uint8_t* data, size_t len) {
    if (len < 20) return; // too short

    version = data[2];

    if (version == 1) {
        userdata = data[3];
        salt = _getU16(&data[4]);
        acceleration.x = _getFP88(&data[6]);
        acceleration.y = _getFP88(&data[8]);
        acceleration.z = _getFP88(&data[10]);
        temperature = _getFP88(&data[12]);
        uptime = _getU32(&data[14]);
        flags = data[18];
        battery = data[19];
    } else if (version == 0) {
        userdata = data[3];
        salt = _getU16(&data[4]);
        acceleration.x = _getFP88(&data[6]);
        acceleration.y = _getFP88(&data[8]);
        acceleration.z = _getFP88(&data[10]);
        temperature = data[12];
        uptime = _getU32(&data[13]);
        flags = data[17];
        battery = data[18];
    } else {
        version = 0xFF;
    }
}

bool MikroTikBeacon::isValid() {
    return version != 0xFF;
}

bool MikroTikBeacon::hasTemperature() {
    return temperature != -128.0;
}
