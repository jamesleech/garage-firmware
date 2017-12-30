// Import libraries (BLEPeripheral depends on SPI)
#include <GarageBluetooth.h>

//////////////
// Hardware //
//////////////
#define LED_PIN LED_BUILTIN // LED on pin 7
#define LED_ACTIVE HIGH // Pin 7 LED is active low
#define LED_DEFAULT LOW

GarageBluetooth garageBluetooth("James' Garage", "James' Garage");

static void doorWrittenEventHandler(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print("doorWrittenEventHandler, central: ");
  Serial.println(central.address());

  // Serial.print("Door Value Written: ");
  // Serial.println(characteristic.value());

  unsigned int value;
  memcpy(&value, (unsigned char*)characteristic.value(), characteristic.valueSize());

  if (value == 0xFF) {
    digitalWrite(LED_PIN, LED_ACTIVE);
  }
  else {
    digitalWrite(LED_PIN, !LED_ACTIVE);
  }

  unsigned int rangeCar = value + 2;
  unsigned int rangeDoor = value + 8;

  garageBluetooth.setRangeCar(rangeCar);
  garageBluetooth.setRangeDoor(rangeDoor);
}

void setup() {
  Serial.begin(9600); // Set up serial

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE);

  garageBluetooth.initialize(true);
  garageBluetooth.setDoorEventHandler(doorWrittenEventHandler);
  garageBluetooth.begin(!LED_ACTIVE, 255, 255);

  delay(100);
  digitalWrite(LED_PIN, !LED_ACTIVE);

  Serial.println("Setup Complete");
}

bool wasConnected = false;

void loop() 
{
  // Serial.println("Loop ");
  garageBluetooth.poll();

  if(garageBluetooth.connected()) {
    if(!wasConnected) {
      wasConnected = true;
      Serial.println("Loop: New connection");
    }
  } else {
    if(wasConnected) {
      Serial.println("Loop: Disconnected");
      wasConnected = false;
    }
  }

  // TODO: low power mode for x ms
}
