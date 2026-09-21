#include <Arduino.h>
#include "config.h"
#include "emg_sensor.h"

// create myoware sensor object
EMGSensor emgSensor(EMG_PIN);

void setup() {
    Serial.begin(SERIAL_BAUD);

    emgSensor.begin();

    Serial.println("Prosthetic hand controller initialized.");
    Serial.println("Reading MyoWare ENV signal...");

    // initialize systems
}

void loop() {

int rawValue = emgSensor.readRaw();
    uint32_t milliVolts = emgSensor.readMilliVolts();

    Serial.print("Raw ADC: ");
    Serial.print(rawValue);

    Serial.print(" | Voltage: ");
    Serial.print(milliVolts);

    Serial.println(" mV");

    delay(100);

}