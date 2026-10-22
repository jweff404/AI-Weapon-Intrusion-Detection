# ESP32-CAM Security System

An ESP32-CAM based security camera project with PIR motion detection and sound sensing.

## Hardware

| Component | Pin |
|---|---|
| ESP32-CAM (AI Thinker) | — |
| PIR Motion Sensor | GPIO 13 |
| Sound Sensor (HW-484) | GPIO 14 |
| FTDI USB-to-Serial Adapter | TX/RX/5V/GND |

## Testing Stages

### Stage 1 — Flash CameraWebServer
1. Install **esp32 by Espressif Systems** from Arduino IDE Boards Manager.
2. Set board to **AI Thinker ESP32-CAM**, select the FTDI COM port.
3. Open `File → Examples → ESP32 → Camera → CameraWebServer`.
4. Edit the sketch:
   ```cpp
   // #define CAMERA_MODEL_WROVER_KIT   ← comment this out
   #define CAMERA_MODEL_AI_THINKER      ← uncomment this
   ```
5. Enter your **2.4 GHz** WiFi SSID and password.
6. Set `Tools → Partition Scheme → Huge APP (3MB No OTA)`.
7. Bridge **IO0 → GND**, press RESET, click Upload.
8. When done, remove IO0 → GND and press RESET.

### Stage 2 — Verify Camera Stream
1. Open Serial Monitor at **115200 baud**, press RESET.
2. Note the IP address printed, e.g. `http://192.168.x.x`.
3. Open that IP in a browser on the same WiFi.
4. Click **Start Stream** — live video confirms success.

### Stage 3 — Test Sensors
Upload `sensor_test/sensor_test.ino`. Open Serial Monitor at 115200 baud.

- **PIR**: Wait 30–60 s warm-up, then wave — output should show `PIR=1`.
- **Sound**: Clap near the mic — `SOUND` should flip. Adjust the blue trimmer if unstable.

## Troubleshooting

| Symptom | Fix |
|---|---|
| Upload timeout / "Connecting…" | Swap TX ↔ RX; confirm IO0 → GND and RESET was pressed |
| Garbage in Serial Monitor | Set baud to **115200** |
| Brownout / random resets | Short USB cable, direct to laptop (no hub); add 100–470 µF cap across 5V/GND |
| Camera init failed | Re-seat ribbon cable; confirm AI Thinker model selected |
| No IP printed | Check WiFi credentials; must be **2.4 GHz** |
