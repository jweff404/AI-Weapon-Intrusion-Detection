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

// ── Configuration ────────────────────────────────────────
#define VERSION             "1.1.0"  // Firmware version
#define PIR_PIN              13      // PIR sensor GPIO
#define SOUND_PIN            14      // Sound sensor GPIO
#define LED_PIN               4      // Onboard flash LED GPIO
#define SERIAL_BAUD      115200      // Serial Monitor baud rate
#define LOOP_DELAY_MS       200      // Polling interval (ms)
#define SOUND_COOLDOWN_MS   500      // Min ms between sound events
#define SUMMARY_INTERVAL_MS 10000   // Status print interval (ms)
#define PIR_WARMUP_MS       60000   // PIR lockout after boot (ms)
// ─────────────────────────────────────────────────────────

// Track previous states to detect changes
int lastPir   = -1;
int lastSound = -1;

// Alert counters since boot
int motionCount = 0;
int soundCount  = 0;

// Sound debounce: ignore re-triggers within SOUND_COOLDOWN_MS
unsigned long lastSoundTime = 0;

// Periodic status summary
unsigned long lastSummaryTime = 0;

// Last event tracking
unsigned long lastEventTime = 0;
const char*   lastEventName = "none";

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(PIR_PIN,   INPUT);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("=== Sensor Test Ready ===");
  Serial.printf( "    Firmware v%s\n", VERSION);
  Serial.printf( "    Built: %s %s\n", __DATE__, __TIME__);
  Serial.println("PIR   → GPIO 13");
  Serial.println("Sound → GPIO 14");
  Serial.println("Waiting for PIR warm-up (30-60 s)...");
  Serial.println("Commands: 's' = status now  |  'r' = reset counters  |  'h' = help");
  Serial.println("-------------------------");
}

void printStatus(unsigned long ms) {
  Serial.println("--- STATUS ---");
  Serial.printf("  Uptime     : %lu s\n", ms / 1000);
  Serial.printf("  Motion     : %d event(s)\n", motionCount);
  Serial.printf("  Sound      : %d event(s)\n", soundCount);
  if (lastEventTime > 0)
    Serial.printf("  Last event : %s (%lu s ago)\n", lastEventName, (ms - lastEventTime) / 1000);
  else
    Serial.println("  Last event : none");
  Serial.println("--------------");
}

void handleSerialInput(unsigned long ms) {
  if (!Serial.available()) return;
  char cmd = Serial.read();
  if (cmd == 's' || cmd == 'S') {
    Serial.println("[CMD] Status requested:");
    printStatus(ms);
  } else if (cmd == 'r' || cmd == 'R') {
    motionCount  = 0;
    soundCount   = 0;
    lastEventTime = 0;
    lastEventName = "none";
    Serial.println("[CMD] Counters reset.");
  } else if (cmd == 'h' || cmd == 'H' || cmd == '?') {
    Serial.println("── Commands ──────────────────");
    Serial.println("  s  Print status snapshot");
    Serial.println("  r  Reset event counters");
    Serial.println("  h  Show this help");
    Serial.println("──────────────────────────────");
  }
}

void loop() {
  int pirVal   = digitalRead(PIR_PIN);
  int soundVal = digitalRead(SOUND_PIN);

  // Print timestamp + raw values every cycle
  unsigned long ms = millis();
  Serial.printf("[%lu ms] PIR=%d  SOUND=%d", ms, pirVal, soundVal);

  // Print event labels when state changes
  if (pirVal != lastPir) {
    if (pirVal == HIGH) {
      if (ms < PIR_WARMUP_MS) {
        Serial.printf("  >> PIR warming up (%lu s left)", (PIR_WARMUP_MS - ms) / 1000);
      } else {
        motionCount++;
        lastEventTime = ms;
        lastEventName = "MOTION";
        Serial.printf("  >> MOTION DETECTED (#%d)", motionCount);
      }
    } else {
      Serial.print("  >> Motion cleared");
    }
    lastPir = pirVal;
  }

  if (soundVal != lastSound) {
    unsigned long now = millis();
    if (soundVal == HIGH && (now - lastSoundTime) > SOUND_COOLDOWN_MS) {
      soundCount++;
      lastSoundTime = now;
      lastEventTime = now;
      lastEventName = "SOUND";
      Serial.printf("  >> SOUND TRIGGERED (#%d)", soundCount);
    } else if (soundVal == LOW) {
      Serial.print("  >> Sound cleared");
    }
    lastSound = soundVal;
  }

  // Flash onboard LED when either sensor triggers
  digitalWrite(LED_PIN, (pirVal == HIGH || soundVal == HIGH) ? HIGH : LOW);

  // Handle Serial commands ('s' = status, 'r' = reset)
  handleSerialInput(ms);

  // Print periodic status summary
  if (ms - lastSummaryTime >= SUMMARY_INTERVAL_MS) {
    lastSummaryTime = ms;
    printStatus(ms);
  }

  Serial.println();
  delay(LOOP_DELAY_MS);
}
