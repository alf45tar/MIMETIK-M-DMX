# DMX Smoke Machine Controller

DMX smoke machine controller for SDJ/Sagitter/Proel [MIMETIK-M](https://www.sdjlighting.com/en/prodotto/sg-mimetikm-smoke-machine-mimetik-900w/) smoke machine.

This is a complete DIY project to add **DMX512 control** to the **SDJ Mimetik-M smoke machine** (and other machines with the same timer/remote port).

The project includes:  
- Arduino sketch for DMX-based smoke pulse control  
- DMX interface  
- 3D-printed **enclosure module** that mounts directly to the machine (left or right side) using the existing screws  
- Wiring diagram and assembly instructions  

## 💡 Compatibility

- ✅ **Tested with:** SDJ Mimetik-M only  
- ⚠️ May also work with machines that have the **same timer/remote port pinout**  
- ❌ Not universal — always check wiring before use  

## ✨ Features
- DMX512 input
- READY/WARM-UP detection (machine will not fire until ready)  
- Pulse control (duration and period scale with DMX value)  
- Feedback PWM output LED for monitoring DMX channel receipt  
- Compact 3D-printed enclosure mounts directly to the machine  

## 🛠 Bill of Materials (BOM)

| Item | Quantity | Notes |
|------|----------|-------|
| Arduino Uno R3 | 1 | Tested reference board |
| Protoshield v.5 | 1 | For clean soldering and module integration |
| MAX485 **or** SN75176 module | 1 | RS485 DMX transceiver |

## 📌 Timer Port Pinout
![Pinout](Timer%20Pinout.jpg)
    

| Label | Direction | Notes                                      |
| ----- | --------- | ------------------------------------------ |
| GND   | Common    | Ground reference                           |
| +5V   | Output    | Provides +5V to remote controller          |
| SMOKE | Input     | Active LOW → pulling to GND triggers smoke |
| READY | Output    | HIGH when warmed up, LOW during warm-up    |
| NC    | –         | Not connected                              |


| Arduino Pin | Function | Notes |
|------|----------|-------|
| `12` | `readyPin`        | Input from machine’s **READY** output |
| `13` | `ledPin`          | Built-in LED mirrors READY state |
| `8`  | `smokePin`        | Smoke trigger                   |
| `9`  | `feedbackPin`     | PWM output showing DMX level |
| `2`  | DMX direction pin | Required by LXUSARTDMX |

## ⚙️ Software Behavior

- **READY low → WARM-UP**: smoke disabled until machine signals ready  
- **DMX mapping**:  
  - `0` = smoke off  
  - `1–255` = pulse duration `0.1–1 s`, period `10 s → 1 s`  
- **Pulse cycle**:  
  - Smoke ON for mapped duration  
  - Smoke OFF until period expires  
  - Adapts instantly if DMX changes mid-cycle  

## ▶️ Usage
1. Print the enclosure and assemble electronics inside.  
2. Mount the enclosure to the **left or right side** of the smoke machine using the existing screws.  
3. Connect wiring to the timer/remote port.  
4. Upload the Arduino sketch (tested on Uno R3).  
5. Send DMX values on the configured channel.  

## 📸 Photos

![Wiring Diagram](./wiring-diagram.png)  
![3D Enclosure](./enclosure.png)  

## ⚠️ Safety Notes
- Smoke machines run on mains voltage — keep low-voltage electronics isolated.  
- Double-check wiring before powering on.  
- Only use with machines confirmed to share the SDJ Mimetik-M timer port wiring.  

## 📜 License
MIT License – use, adapt, and improve freely.  
