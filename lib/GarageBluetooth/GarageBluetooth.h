#ifndef _GARAGE_BLUETOOTH_H_
#define _GARAGE_BLUETOOTH_H_

#include <SPI.h>
#include <BLEPeripheral.h>


// From the discussion above, the following message format is suggested: 
// 4 bytes for the serial number, 
// 1 command byte, 
// 4 bytes for the counter value and 
// 4 bytes for the MAC. 
// That is a total of 13 bytes or 104 bits.
// This format will be used in the rest of this application note, but is easily customized.

#define KEY_BITS 128 //!< Use AES128 //#define KEY_BITS 192 //!< Use AES192 //#define KEY_BITS 256 //!< Use AES256

#define MAC_SIZE 4

//! Maximum number of transmitters associated with one receiver.
#define MAX_TRANSMITTERS 5

struct CommandMessage {
  uint32_t serialNumber;
  uint32_t counter;
  uint8_t command;
  char MAC[MAC_SIZE];
};

class GarageBluetooth {
  public:
    GarageBluetooth(const char* deviceName, const char* localName);
    const char* deviceName() const;
    const char* localName() const;
    
    void initialize(bool clearBond);
    void begin(unsigned char door, unsigned char rangeCar, unsigned char rangeDoor);

    void poll();

    bool connected();

    void setNewDoorEventHandler(BLECharacteristicEventHandler handler);

    void setRangeDoor(unsigned char range);
    void setRangeCar(unsigned char range);
    
    void setSerialNumber(uint32_t newValue);
    void setCounter(uint32_t newValue);
    void setCommand(uint8_t newValue);
    void setMAC(char *newValue);

    void clearPaired();

  private:
    const char* _deviceName;
    const char* _localName;

    const char* _serviceUuid = "321ccaca-29a6-4d46-b2db-9b5639948751";

    const char* _newDoorUuid = "D7C7B570-EEDA-11E7-BD5D-FB4762172F1A";
    const char* _rangeDoorUuid = "a204907d-4d03-46ac-97ef-0758911fe3f4";
    const char* _rangeCarUuid = "6cccfcb2-4e84-4c13-9c03-020b9c25c5f4";

    //DEBUG: characteristics
    const char* _serialNumberUuid = "9be37802-11ae-4c38-baf2-019e8d0fc0d8";
    const char* _counterUuid = "13434cac-3c78-4846-b132-d0c97ed4d8eb";
    const char* _commandUuid = "29be5904-d24e-4536-8df2-958c2b8beb5c";
    const char *_macUuid = "9f220908-3433-4fcd-bac0-5dbd4c38c6a5";

    BLEBondStore* _bleBondStore;
    BLEPeripheral* _blePeriph;
    BLEService* _bleService;

    BLECharCharacteristic* _rangeDoorChar;
    BLECharCharacteristic* _rangeCarChar;
    BLEFixedLengthCharacteristic* _newDoorChar;

    BLEUnsignedIntCharacteristic* _serialNumberChar;
    BLEUnsignedIntCharacteristic* _counterChar;
    BLECharCharacteristic* _commandChar;
    BLEFixedLengthCharacteristic *_macChar;

    static void blePeripheralCentralConnectedEventHandler(BLECentral& central);
    static void blePeripheralCentralDisconnectedEventHandler(BLECentral& central);
    
    static void blePeripheralBondedHandler(BLECentral& central);
};

#endif