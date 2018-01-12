// Import libraries (BLEPeripheral depends on SPI)
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>
#include <GarageBluetooth.h>
#include "config.h"

GarageBluetooth garageBluetooth("James' Garage", "James' Garage");
bool toggleDoor = false;
bool wasConnected = false;

CommandMessage command;

const char *key = "0123456789abcdef";
uint8_t authCode[SHA256HMAC_SIZE];
SHA256 hasher;

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

        //also grab the bytes
        const int8_t msgSize = 20;
        uint8_t msgBytes[msgSize];
        memcpy(msgBytes, characteristic.value(), sizeof(uint8_t) * msgSize);

        Serial.println("Printing given msg bytes");
        for (int i = 0; i < msgSize; i++) {
          Serial.print(msgBytes[i], DEC);
          Serial.print(", ");
        }
        Serial.println();

        // calculate the HMAC
        hasher.resetHMAC(key, strlen(key));
        hasher.update(msgBytes, 9); //msgSize);
        // Finish the HMAC calculation and return the authentication code
        hasher.finalizeHMAC(key, strlen(key), authCode, sizeof(authCode));
        // authCode now contains our SHA256HMAC_SIZE byte authentication code
        Serial.println("Printing Calculated HMAC");
        for (int i = 0; i < 11; i++) {
          if(authCode[i] == msgBytes[9+i]) {
            Serial.print(authCode[i], DEC);
            Serial.print(", ");
          } else {
            Serial.print("Misatch: ");
            Serial.print(authCode[i], DEC);
            Serial.print(" != ");
            Serial.print(msgBytes[9+i], DEC);
            Serial.print(", ");
          }
        }
        Serial.println();
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
