#ifndef _GARAGE_BLUETOOTH_H_
#define _GARAGE_BLUETOOTH_H_

// #include <SPI.h>
#include <BLEPeripheral.h>

class GarageBluetooth
{
  public:
    GarageBluetooth(const char* deviceName, const char* localName);
    const char* deviceName() const;
    const char* localName() const;
    
    void initialize(bool clearBond);
    void begin(unsigned char door, unsigned char rangeCar, unsigned char rangeDoor);

    void poll();

    bool connected();

    void setDoorEventHandler(BLECharacteristicEventHandler handler);

    void setRangeDoor(unsigned char range);
    void setRangeCar(unsigned char range);

    void clearPaired();

  private:
    const char* _deviceName;
    const char* _localName;

    const char* _serviceUuid = "321ccaca-29a6-4d46-b2db-9b5639948751";
    const char* _doorUuid = "a3ab3644-1832-4eb2-87f6-777b9845f5ea";
    const char* _rangeDoorUuid = "a204907d-4d03-46ac-97ef-0758911fe3f4";
    const char* _rangeCarUuid = "6cccfcb2-4e84-4c13-9c03-020b9c25c5f4";
  
    BLEBondStore* _bleBondStore;
    BLEPeripheral* _blePeriph;
    BLEService* _bleServ;

    BLECharCharacteristic* _doorChar;
    BLECharCharacteristic* _rangeDoorChar;
    BLECharCharacteristic* _rangeCarChar;

    static void blePeripheralCentralConnectedEventHandler(BLECentral& central);
    static void blePeripheralCentralDisconnectedEventHandler(BLECentral& central);
    
    static void blePeripheralBondedHandler(BLECentral& central);
};

#endif