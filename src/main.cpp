#include <Arduino.h>
#include "config.h"
#include "emg_sensor.h"
#include "state_machine.h"

namespace
{
EMGSensor emgSensor(EMG_PIN);
CommandClassifier classifier(EMG_THRESHOLDS);
HandStateMachine hand;
ServoController servos(SERVO_MOTION);
SafetyManager safety(SAFETY_CONFIG);
uint32_t lastSample = 0;
uint32_t lastTelemetry = 0;
int rawValue = 0;
uint32_t milliVolts = 0;
EMGProcessed processed = {0.0f, false};
HandCommand command = HandCommand::NONE;
}

void setup()
{
    Serial.begin(SERIAL_BAUD);
    emgSensor.begin();
    if (!servos.begin(millis()) || SAFETY_CONFIG.maxSampleGapMs <= EMG_SAMPLE_PERIOD_MS)
        hand.update(HandCommand::NONE, false, FaultCode::INVALID_CONFIGURATION);

    Serial.println("Wrist/thumb target preview only; no servo PWM output.");
    Serial.println("Set measured calibration, thresholds, timing and poses in config.h.");
}

void loop()
{
    uint32_t now = millis();
    if (uint32_t(now - lastSample) >= EMG_SAMPLE_PERIOD_MS)
    {
        rawValue = emgSensor.readRaw();
        milliVolts = emgSensor.readMilliVolts(); // Separate conversion, telemetry only.
        now = millis();
        lastSample = now;
        processed = processEMG(float(rawValue), EMG_CALIBRATION);
        // Reject either ADC rail before a clipped value can become a command.
        processed.valid = processed.valid && rawValue > 0 && rawValue < EMG_ADC_MAX;
        command = classifier.update(processed, now);
        safety.observe(processed, command, now);
    }

    hand.update(command, servos.targetReached(), safety.check(now));
    const ServoTargets targets = servos.update(hand.target(), now, hand.enabled());
    if (uint32_t(now - lastTelemetry) >= TELEMETRY_PERIOD_MS)
    {
        lastTelemetry = now;
        Serial.printf("raw=%d mv=%lu activation=%.3f cmd=%d state=%d fault=%d "
                      "targets_valid=%d wrist_us=%.1f thumb_us=%.1f\n",
                      rawValue, static_cast<unsigned long>(milliVolts), processed.activation,
                      int(command), int(hand.state()), int(hand.fault()),
                      int(targets.valid), targets.wristPulseUs, targets.thumbPulseUs);
    }
}
