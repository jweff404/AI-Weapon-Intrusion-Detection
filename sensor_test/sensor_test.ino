/*
 * sensor_test.ino
 * ESP32-CAM — PIR & Sound Sensor Test Sketch
 *
 * Wiring:
 *   PIR sensor   → GPIO 13
 *   Sound sensor → GPIO 14
 *
 * Upload steps:
 *   1. Bridge IO0 → GND, press RESET, then upload.
 *   2. Remove IO0 → GND wire, press RESET.
 *   3. Open Serial Monitor at 115200 baud.
 *
 * PIR notes:
 *   Allow 30–60 seconds warm-up after power-up before testing.
 *   Output goes HIGH (1) when motion is detected.
 *
 * Sound sensor (HW-484) notes:
 *   Output flips state on a clap / loud noise.
 *   If output is unstable at rest, adjust the blue trimmer
 *   potentiometer on the sensor board.
 */

const int PIR_PIN   = 13;
const int SOUND_PIN = 14;

// Track previous states to detect changes
int lastPir   = -1;
int lastSound = -1;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN,   INPUT);
  pinMode(SOUND_PIN, INPUT);

  Serial.println("=== Sensor Test Ready ===");
  Serial.println("PIR   → GPIO 13");
  Serial.println("Sound → GPIO 14");
  Serial.println("Waiting for PIR warm-up (30-60 s)...");
  Serial.println("-------------------------");
}

void loop() {
  int pirVal   = digitalRead(PIR_PIN);
  int soundVal = digitalRead(SOUND_PIN);

  // Print raw values every cycle
  Serial.printf("PIR=%d  SOUND=%d", pirVal, soundVal);

  // Print event labels when state changes
  if (pirVal != lastPir) {
    if (pirVal == HIGH) Serial.print("  >> MOTION DETECTED");
    else                Serial.print("  >> Motion cleared");
    lastPir = pirVal;
  }

  if (soundVal != lastSound) {
    if (soundVal == HIGH) Serial.print("  >> SOUND TRIGGERED");
    else                  Serial.print("  >> Sound cleared");
    lastSound = soundVal;
  }

  Serial.println();
  delay(200);
}
