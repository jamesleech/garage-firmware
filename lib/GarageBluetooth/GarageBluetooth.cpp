#include "GarageBluetooth.h"

GarageBluetooth::GarageBluetooth(const char* deviceName, const char* localName) :
  _deviceName(deviceName),
  _localName(localName) {
}

const char* GarageBluetooth::deviceName() const {
  return this->_deviceName;
}

const char* GarageBluetooth::localName() const {
  return this->_localName;
}

void GarageBluetooth::setRangeCar(unsigned char range) {
  _rangeCarChar->setValue(range);
}

void GarageBluetooth::setRangeDoor(unsigned char range) {
  _rangeDoorChar->setValue(range);
}

void GarageBluetooth::setSerialNumber(uint32_t newValue) {
  _serialNumberChar->setValue(newValue);
}

void GarageBluetooth::setCounter(uint32_t newValue) {
  _counterChar->setValue(newValue);
}

void GarageBluetooth::setCommand(uint8_t newValue) {
  _commandChar->setValue(newValue);
}

void GarageBluetooth::setMAC(char* newValue) {
  _macChar->setValue(newValue);
}

void GarageBluetooth::clearPaired() {
  _bleBondStore->clearData();
}

void GarageBluetooth::initialize(bool clearBond) {
  _blePeriph = new BLEPeripheral();
  _bleService = new BLEService(_serviceUuid);

  _newDoorChar = new BLEFixedLengthCharacteristic(_newDoorUuid, BLERead | BLEWrite, sizeof(CommandMessage));
  _rangeCarChar = new BLECharCharacteristic(_rangeCarUuid, BLERead); // TODO: Notify
  _rangeDoorChar = new BLECharCharacteristic(_rangeDoorUuid, BLERead); // TODO: Notify

  _serialNumberChar = new BLEUnsignedIntCharacteristic(_serialNumberUuid, BLERead);
  _counterChar = new BLEUnsignedIntCharacteristic(_counterUuid, BLERead);
  _commandChar = new BLECharCharacteristic(_commandUuid, BLERead);
  _macChar = new BLEFixedLengthCharacteristic(_macUuid, BLERead, sizeof(char) * HMAC_SIZE);

  // setup bond store for secure pairing
  // _bleBondStore = new BLEBondStore();
  // if(clearBond) {
  //   this->clearPaired(); //TODO: hook up to button press
  // }
  // _blePeriph->setBondStore(*_bleBondStore);

  // Advertise name and service:
  _blePeriph->setDeviceName(_deviceName);
  _blePeriph->setLocalName(_localName);
  _blePeriph->setAdvertisedServiceUuid(_serviceUuid);

  // Add service
  _blePeriph->addAttribute(*_bleService);

  // Add characteristics
  _blePeriph->addAttribute(*_newDoorChar);
  _blePeriph->addAttribute(*_rangeCarChar);
  _blePeriph->addAttribute(*_rangeDoorChar);

  // Add debug characteristics
  _blePeriph->addAttribute(*_serialNumberChar);
  _blePeriph->addAttribute(*_counterChar);
  _blePeriph->addAttribute(*_commandChar);
  _blePeriph->addAttribute(*_macChar);

  _blePeriph->setEventHandler(BLEConnected, blePeripheralCentralConnectedEventHandler);
  _blePeriph->setEventHandler(BLEDisconnected, blePeripheralCentralConnectedEventHandler);
  _blePeriph->setEventHandler(BLEBonded, blePeripheralBondedHandler);
}

void GarageBluetooth::begin(unsigned char door, unsigned char rangeCar, unsigned char rangeDoor) {
  // Now that device, service, characteristic are set up,
  // initialize BLE:
  _blePeriph->begin();

  // Set characteristic to default values
  CommandMessage commandMessage = {};
  _newDoorChar->setValue((unsigned char *)&commandMessage, sizeof(commandMessage));
  this->setRangeCar(rangeCar);
  this->setRangeDoor(rangeDoor);
}

void GarageBluetooth::poll() {
  _blePeriph->poll();
}

bool GarageBluetooth::connected() {
  return _blePeriph->connected();
}

void GarageBluetooth::setNewDoorEventHandler(BLECharacteristicEventHandler handler) {
  _newDoorChar->setEventHandler(BLEWritten, handler);
}

void GarageBluetooth::blePeripheralCentralConnectedEventHandler(BLECentral& central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());

  if (central) {
    Serial.print("Connected event, central is valid ");
  } else {
    Serial.print("Connected event, central is invalid ");
  }
}

void GarageBluetooth::blePeripheralCentralDisconnectedEventHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());  
}

void GarageBluetooth::blePeripheralBondedHandler(BLECentral& central) {
  // central bonded event handler
  Serial.print(F("Remote bonded event, central: "));
  Serial.println(central.address());
}