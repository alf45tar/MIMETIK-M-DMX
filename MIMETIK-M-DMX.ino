/*
===============================================================================
  SMOKE CONTROLLER (Arduino + DMX)
===============================================================================

LEDs:
- Pin 13 → READY indicator (follows smoke machine warm-up/ready signal)
- Pin 10 → Smoke activity LED (follows actual smoke ON/OFF pulses)
- Pin 9  → Received DMX value is the PWM duty cycle

-------------------------------------------------------------------------------
STATE MACHINE
-------------------------------------------------------------------------------
States:
  IDLE     → OFF, waiting for next pulse
  PULSING  → ON, active for pulseDuration
  WAITING  → OFF, gap until next pulse
  FULL_ON  → ON, continuous (DMX=255)

-------------------------------------------------------------------------------
TIMING BEHAVIOR (examples)
-------------------------------------------------------------------------------

DMX = 0   → OFF, no pulses (IDLE)
DMX = 50  → pulseDuration ~250 ms, period ~8000 ms
             [ON--250ms][OFF-----------------7750ms-----------------][ON...]

DMX = 128 → pulseDuration ~500 ms, period ~5500 ms
             [ON--500ms][OFF----------------5000ms----------------][ON...]

DMX = 254 → pulseDuration ~900 ms, period ~1000 ms
             [ON--900ms][OFF-100ms][ON--900ms][OFF-100ms]...

DMX = 255 → FULL_ON (continuous ON, no pulses)
             [ON--------------------------------------------------------------]

-------------------------------------------------------------------------------
FAILSAFE
-------------------------------------------------------------------------------
- If smoke machine is warming up (READY=LOW) → forced OFF
- If DMX signal lost for >3s → forced OFF
- DMX=0 → forced OFF
===============================================================================
*/

#include <Arduino.h>
#include <LXUSARTDMX.h>

// --- Config ---
const int dmxChannel  = 503;  // DMX channel
const int readyPin    = 12;   // READY input from smoke machine
const int ledPin      = 13;   // onboard LED = READY indicator
const int smokePin    = 8;    // smoke control (tri-state)
const int pulseLedPin = 10;   // LED follows smoke ON/OFF
const int feedbackPin = 9;    // feedback PWM

// --- Timing ---
uint16_t pulseDuration = 200;   // ms
uint16_t period        = 5000;  // ms

unsigned long pulseStartTime = 0;
unsigned long nextPulseTime  = 0;
unsigned long lastDMXMillis  = 0;
const unsigned long dmxTimeout = 3000; // ms

// --- State ---
enum SmokeState { IDLE, PULSING, WAITING, FULL_ON };
SmokeState state = IDLE;

byte lastDMXValue = 0;
bool prevReadyState = false;

// --- Helpers ---
void smokeOn() {
  digitalWrite(smokePin, HIGH);
  digitalWrite(pulseLedPin, HIGH); // LED follows smoke ON
  pulseStartTime = millis();
  state = PULSING;
}

void smokeFullOn() {
  digitalWrite(smokePin, HIGH);    // permanently ON
  digitalWrite(pulseLedPin, HIGH); // LED ON solid
  state = FULL_ON;
}

void smokeOff() {
  digitalWrite(smokePin, LOW);
  digitalWrite(pulseLedPin, LOW);  // LED follows smoke OFF
  state = IDLE;
}

// --- DMX callback ---
void gotDMXCallback(int slots) {
  lastDMXMillis = millis(); // refresh "alive" watchdog
}

// --- Update smoke logic ---
void updateSmoke() {
    unsigned long now = millis();
    bool readyState = digitalRead(readyPin);

    // READY indicator LED mirrors machine status
    digitalWrite(ledPin, readyState);

    // --- Warm-up / Ready detection ---
    if (readyState != prevReadyState) {
        prevReadyState = readyState;
        if (readyState) {
            nextPulseTime = now;  // can start immediately
        } else {
            smokeOff();
        }
    }

    if (!readyState) {
        smokeOff();
        return;
    }

    // --- DMX input ---
    byte dmxValue = LXSerialDMX.getSlot(dmxChannel);

    // --- DMX failsafe ---
    if (millis() - lastDMXMillis > dmxTimeout) {
        analogWrite(feedbackPin, 0); // force LOW = DMX lost
        smokeOff();
        return;
    } else {
        analogWrite(feedbackPin, dmxValue); // show live DMX value
    }

    // --- IDLE handling ---
    if (dmxValue == 0) {
        smokeOff();
        return;
    }

    // --- FULL_ON handling ---
    if (dmxValue == 255) {
        if (state != FULL_ON) {
            smokeFullOn();
        }
        return; // skip pulsing logic
    } 
    else if (state == FULL_ON) {
        // Exit FULL_ON when DMX < 255
        smokeOff(); 
    }

    // --- Calculate pulseDuration and period for 1–254 DMX ---
    uint16_t newPulseDuration = map(dmxValue, 1, 254, 100, 900);
    uint16_t newPeriod = map(dmxValue, 1, 254, 10000, 1000);

    // --- Adjust timings mid-pulse to avoid glitches ---
    if (state == PULSING) {
        unsigned long elapsed = now - pulseStartTime;
        if (elapsed >= newPulseDuration) {
            smokeOff();
            nextPulseTime = now + (newPeriod - newPulseDuration);
            state = WAITING;
        } else {
            pulseDuration = newPulseDuration;
            period = newPeriod;
        }
    } 
    else if (state == WAITING) {
        unsigned long remaining = (nextPulseTime > now) ? nextPulseTime - now : 0;
        if (remaining > newPeriod - newPulseDuration) {
            nextPulseTime = now + (newPeriod - newPulseDuration);
        }
        pulseDuration = newPulseDuration;
        period = newPeriod;

        if (now >= nextPulseTime) {
            smokeOn();
        }
    } 
    else if (state == IDLE) {
        pulseDuration = newPulseDuration;
        period = newPeriod;
        if (now >= nextPulseTime) {
            smokeOn();
        }
    }
}

// --- Setup ---
void setup() {
    pinMode(readyPin, INPUT);
    pinMode(ledPin, OUTPUT);  
    pinMode(pulseLedPin, OUTPUT);
    pinMode(feedbackPin, OUTPUT);
    pinMode(smokePin, OUTPUT);
    smokeOff();

    LXSerialDMX.setDirectionPin(2);
    LXSerialDMX.setDataReceivedCallback(&gotDMXCallback);
    LXSerialDMX.startInput();
    lastDMXMillis = millis();
}

// --- Main loop ---
void loop() {
    updateSmoke();
}