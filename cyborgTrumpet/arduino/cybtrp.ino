#include <PulseSensorPlayground.h>
#include <Servo.h>

/* ---------- Pins and Parameters ---------- */
const byte  PULSE_PIN       = A0;   // PulseSensor purple wire → A0
const byte  SERVO_PULSE_PIN = 10;  // Servo driven by heart rate
const byte  SERVO_SWEEP_PIN = 9;   // Regular sweeping servo
const byte  LED_PIN         = LED_BUILTIN;
int   Threshold             = 550;  // Adjust based on actual signal quality

/* ---------- Library Objects ---------- */
PulseSensorPlayground pulseSensor;
Servo servoPulse;   // D10: Servo driven by pulse
Servo servoSweep;   // D9: Regular sweeping servo

/* ---------- Heart Rate Servo Variables ---------- */
int bpm      = 0;
int step     = 1;
int posPulse = 0;
bool fwdPulse = true;

/* ---------- Regular Servo Variables ---------- */
int posSweep = 0;
bool fwdSweep = true;

/* ---------- BPM to Step Mapping ---------- */
int stepFromBPM(int bpmVal) {
  if (bpmVal <= 50)        return 0;
  else if (bpmVal <= 75)   return 1;
  else if (bpmVal <= 90)   return 2;
  else if (bpmVal <= 105)  return 3;
  else if (bpmVal <= 120)  return 4;
  else                     return 6;   // 121+
}

/* ---------- Initialization ---------- */
void setup() {
  Serial.begin(115200);

  pulseSensor.analogInput(PULSE_PIN);
  pulseSensor.blinkOnPulse(LED_PIN);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.begin();

  servoPulse.attach(SERVO_PULSE_PIN);
  servoPulse.write(posPulse);

  servoSweep.attach(SERVO_SWEEP_PIN);
  servoSweep.write(posSweep);

  Serial.println("Dual‑Servo Pulse & Sweep demo ready");
}

/* ---------- Main Loop ---------- */
void loop() {

  /* — 1. Detect new heartbeat and update step size — */
  if (pulseSensor.sawStartOfBeat()) {
    bpm  = pulseSensor.getBeatsPerMinute();
    step = stepFromBPM(bpm);

    Serial.print("♥  BPM = ");
    Serial.print(bpm);
    Serial.print("  →  step = ");
    Serial.println(step);
  }

  /* — 2. Update heart rate-driven servo position — */
  if (fwdPulse) {
    posPulse += step;
    if (posPulse >= 180) {
      posPulse = 180;
      fwdPulse = false;
    }
  } else {
    posPulse -= step;
    if (posPulse <= 0) {
      posPulse = 0;
      fwdPulse = true;
    }
  }
  servoPulse.write(posPulse);

  /* — 3. Update regular sweeping servo position — */
  if (fwdSweep) {
    posSweep += 1;
    if (posSweep >= 180) {
      posSweep = 180;
      fwdSweep = false;
    }
  } else {
    posSweep -= 1;
    if (posSweep <= 0) {
      posSweep = 0;
      fwdSweep = true;
    }
  }
  servoSweep.write(posSweep);

  /* — 4. Shared delay — */
  delay(15);   // Shared timing for both servos
}
