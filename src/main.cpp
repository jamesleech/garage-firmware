// Import libraries (BLEPeripheral depends on SPI)
#include <Crypto.h>
#include <SHA3.h>
#include <string.h>
#include <GarageBluetooth.h>
#include "config.h"

GarageBluetooth garageBluetooth("James' Garage", "James' Garage");
bool toggleDoor = false;
bool wasConnected = false;

CommandMessage command;

uint8_t key[KEY_LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
uint8_t hmac[HMAC_SIZE];

SHA3_256 sha3_256;

static void newDoorWrittenEventHandler(BLECentral& central, BLECharacteristic& characteristic) {
    Serial.print("newDoorWrittenEventHandler, central: ");
    Serial.println(central.address());

    if (characteristic.written()) {
        // copy the value written
        memcpy(&command, characteristic.value(), sizeof(command));

        garageBluetooth.setSerialNumber(command.serialNumber);
        Serial.print("SerialNumber: ");
        Serial.println(command.serialNumber);

        garageBluetooth.setCounter(command.counter);
        Serial.print("Counter: ");
        Serial.println(command.counter);

        garageBluetooth.setCommand(command.command);
        Serial.print("Command: ");
        Serial.println(command.command);

        garageBluetooth.setMAC(command.MAC);
        Serial.print("MAC: ");
        for (int i = 0; i < HMAC_SIZE; i++) {
          Serial.print(command.MAC[i]);
        }
        Serial.println("");

        sha3_256.resetHMAC(key, sizeof(key));
        sha3_256.update(&command.serialNumber, sizeof(command.serialNumber));
        sha3_256.update(&command.counter, sizeof(command.counter));
        sha3_256.update(&command.command, sizeof(command.command));
        sha3_256.finalizeHMAC(key, sizeof(key), hmac, sizeof(hmac));

        Serial.print("Calculated MAC: ");
        for (int i = 0; i < HMAC_SIZE; i++) {
          Serial.print(hmac[i]);
          Serial.print(",");
        }
        Serial.println("");

        int hmacCheck = memcmp (hmac, &(command.MAC), sizeof(HMAC_SIZE) );
        if(hmacCheck == 0) {
          Serial.println("MAC's are equal");
        }
    }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE);

  Serial.begin(9600);

  garageBluetooth.initialize(true);
  garageBluetooth.setNewDoorEventHandler(newDoorWrittenEventHandler);
  garageBluetooth.begin(!LED_ACTIVE, 255, 255);

  Serial.println("Setup Complete");
  
  digitalWrite(LED_PIN, !LED_ACTIVE);
}

void loop() {
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
}
