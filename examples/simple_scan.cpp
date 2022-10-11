/*
    Scan and print MikrotTik beacons
*/

#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>

#include "MikroTikBT5.h"

BLEScan *pBLEScan;

String seconds2time(uint32_t value) {
    uint8_t ss = value % 60;
    value /= 60;

    uint8_t mm = value % 60;
    value /= 60;

    uint8_t hh = value % 60;
    value -= hh;

    uint8_t dd = value / 24;

    char buf[16];
    sprintf(buf, "%ud %02d:%02d:%02d", dd, hh, mm, ss);

    return String(buf);
}

void printBeacon(MikroTikBeacon beacon, BLEAdvertisedDevice *advertisedDevice) {
    static int beacon_count = 0;

    char mac[18];
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
        advertisedDevice->getAddress().getNative()[5],
        advertisedDevice->getAddress().getNative()[4],
        advertisedDevice->getAddress().getNative()[3],
        advertisedDevice->getAddress().getNative()[2],
        advertisedDevice->getAddress().getNative()[1],
        advertisedDevice->getAddress().getNative()[0]
    );

    char flag = ' ';
    switch (beacon.flags) {
        case BT5_FLAG_REED_SWITCH: flag = 'R'; break;
        case BT5_FLAG_ACCEL_TILT: flag = 'T'; break;
        case BT5_FLAG_ACCEL_FREE_FALL: flag = 'F'; break;
        case BT5_FLAG_IMPACT_X: flag = 'X'; break;
        case BT5_FLAG_IMPACT_Y: flag = 'Y'; break;
        case BT5_FLAG_IMPACT_Z: flag = 'Z'; break;
    };

    char bat_str[6];
    sprintf(bat_str, "%u %%", beacon.battery);

    char temperature_str[8] = "   -   ";
    if (beacon.hasTemperature()) {
        sprintf(temperature_str, "%.1f C", beacon.temperature);
    }

    if (beacon_count % 15 == 0) {
        // Print header after every 15 beacons
        Serial.println("+-------------------+------+---+-------+---------+---------------+---------------------+");
        Serial.println("| MAC               | RSSI |   | Bat   | Temp.   | Uptime        | Accelerometer XYZ   |");
        Serial.println("+-------------------+------+---+-------+---------+---------------+---------------------+");
    }

    beacon_count++;

    Serial.printf("| %s | %4d | %c | %5s | %7s | %13s | %5.2f  %5.2f  %5.2f |\n",
        mac,
        advertisedDevice->getRSSI(),
        flag,
        bat_str,
        temperature_str,
        seconds2time(beacon.uptime).c_str(),
        beacon.acceleration.x,
        beacon.acceleration.y,
        beacon.acceleration.z
    );
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
        if (advertisedDevice->haveManufacturerData() == true) {
            std::string strManufacturerData = advertisedDevice->getManufacturerData();

            uint8_t cManufacturerData[100];
            int cLength = strManufacturerData.length();
            
            strManufacturerData.copy((char *)cManufacturerData, cLength, 0);

            uint16_t manufacturerId = 0;
            memcpy(&manufacturerId, (void*) cManufacturerData, sizeof(manufacturerId));

            if (manufacturerId == MIKROTIK_MANUFACTURER_ID) {
                MikroTikBeacon beacon;
                beacon.unpack(cManufacturerData, cLength);
                if (!beacon.isValid()) return;
                printBeacon(beacon, advertisedDevice);
            }
        }
        return;
      }
};

void setup() {
    Serial.begin(115200);

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(false); // BT tags doesn't need active scan
    pBLEScan->setDuplicateFilter(false);

    pBLEScan->setInterval(10); // knot defaults
    pBLEScan->setWindow(10);   // knot defaults
}

void loop() {
    BLEScanResults foundDevices = pBLEScan->start(10, false);
    pBLEScan->clearResults();
}