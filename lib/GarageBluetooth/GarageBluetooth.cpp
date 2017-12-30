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

void GarageBluetooth::clearPaired() {
  _bleBondStore->clearData();
}

void GarageBluetooth::initialize(bool clearBond) {
  _blePeriph = new BLEPeripheral();
  _bleServ = new BLEService(_serviceUuid);

  _doorChar = new BLEUnsignedCharCharacteristic(_doorUuid, BLERead | BLEWrite);
  _rangeCarChar = new BLEUnsignedCharCharacteristic(_rangeCarUuid, BLERead); // TODO: Notify
  _rangeDoorChar = new BLEUnsignedCharCharacteristic(_rangeDoorUuid, BLERead); // TODO: Notify

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
  _blePeriph->addAttribute(*_bleServ);

  // Add characteristics
  _blePeriph->addAttribute(*_doorChar);
  _blePeriph->addAttribute(*_rangeCarChar);
  _blePeriph->addAttribute(*_rangeDoorChar);
  
  _blePeriph->setEventHandler(BLEConnected, blePeripheralCentralConnectedEventHandler);
  _blePeriph->setEventHandler(BLEDisconnected, blePeripheralCentralConnectedEventHandler);
  _blePeriph->setEventHandler(BLEBonded, blePeripheralBondedHandler);
}

void GarageBluetooth::begin(unsigned char door, unsigned char rangeCar, unsigned char rangeDoor) {
  // Now that device, service, characteristic are set up,
  // initialize BLE:
  _blePeriph->begin();

  // Set characteristic to default values
  _doorChar->setValue(door);
  this->setRangeCar(rangeCar);
  this->setRangeDoor(rangeDoor);
}

void GarageBluetooth::poll() {
  _blePeriph->poll();
}

bool GarageBluetooth::connected() {
  return _blePeriph->connected();
}

void GarageBluetooth::setDoorEventHandler(BLECharacteristicEventHandler handler) {
  _doorChar->setEventHandler(BLEWritten, handler);
}

void GarageBluetooth::blePeripheralCentralConnectedEventHandler(BLECentral& central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
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