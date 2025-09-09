# DMX Smoke Machine Controller

DMX512 controller add-on for the **SDJ / Sagitter / Proel** [**MIMETIK-M**](https://www.sdjlighting.com/en/prodotto/sg-mimetikm-smoke-machine-mimetik-900w/)  smoke machine.
Provides pulse-based DMX smoke control with ready-state detection and a neat 3D-printed enclosure.  

## 💡 Compatibility
- ✅ Tested with **SDJ Mimetik-M**  
- ⚠️ May also work with machines sharing the **same timer/remote port pinout**  
- ❌ Not universal — always check wiring before use  

## ✨ Features
- DMX512 input via RS485 transceiver  
- READY/WARM-UP detection (machine won’t fire until warmed up)  
- Pulse control: DMX value maps to **pulse duration** and **pulse period**  
- Feedback PWM LED (DMX level, pin 9)  
- Smoke pulse LED (mirrors smoke firing, pin 10)  
- READY LED (built-in pin 13)  
- Compact 3D-printed enclosure that mounts directly using factory screws  

## 🛠 Bill of Materials
| Item | Quantity | Notes |
|------|----------|-------|
| Arduino Uno R3 | 1 | Reference/tested board (⚠️ must be powered separately — not from timer port) |
| Protoshield v.5 | 1 | For soldering & integration |
| MAX485 **or** SN75176 module | 1 | RS485 transceiver |
| PN2222 transistor | 1 | Smoke trigger driver |
| LEDs + resistors | optional | Status & feedback indicators |

## 📌 Timer Port Pinout
![Pinout](Timer%20Pinout.jpg)

| Label | Direction | Description |
|-------|-----------|-------------|
| GND   | Common    | Ground reference |
| +5V   | Output    | **Low-current reference only** — cannot power Arduino |
| SMOKE | Input     | Active LOW → pulling to GND triggers smoke |
| READY | Output    | HIGH when warmed up, LOW during warm-up |
| NC    | –         | Not connected |

⚠️ **Important:** The +5 V pin from the timer/remote port is **not able to supply enough current to run an Arduino**.  
Use a separate Arduino power source (USB, barrel jack, VIN, or stable +5 V).

---

## 🔌 Arduino Connections
| Arduino Pin | Function | Notes |
|-------------|----------|-------|
| `2`  | Direction pin | Required by **LXUSARTDMX** |
| `8`  | `smokePin` | Smoke trigger |
| `9`  | `feedbackPin` | PWM output showing DMX level |
| `10` | `smokeLedPin` | LED mirrors smoke pulses |
| `12` | `readyPin` | Input from machine READY |
| `13` | `ledPin` | Built-in LED mirrors READY |

# Schematic

## DMX connector wiring
```
Arduino                                               DMX connector

    +5V  --------------------------------------+
                                               |                                 
                          +---------------+    |
     RX  -----------------| R         VCC |----+
                          |               |
                     +----| RE/         B |----------  Data - (XLR pin 2)
                     |    |    SN75176    |
      2  ------------+----| DE          A |----------  Data + (XLR pin 3)
                          |               |
     TX  -----------------| D         GND |---+------  Ground (XLR pin 1)
                          +---------------+   |
                                              |
    GND  -------------------------------------+                                       
```

## Timer port wiring
```
Arduino                                  Timer Port

                           +------------  SMOKE
                           |
                           |
                           |  
                         | /  C
               560Ω      |/     
      8  -----\/\/\/\----|        PN22222
                       B |\
                         | v  E
                           |
                           |
                           |
    GND  ------------------+------------  GND                    

     12  -------------------------------  READY

```

## Led wiring (optional)
```
Arduino
                     220Ω
      9  -----|>|---\/\/\/\-----+  DMX feedback LED
                                |
                     220Ω       |
     10  -----|>|---\/\/\/\-----+  Smoke pulse LED
                                |
                                |
    GND  -----------------------+            
```

## 🔋 Powering the Arduino

The smoke machine’s **timer port +5 V cannot power the Arduino** (insufficient current).  
Use one of the standard Arduino Uno power options instead:

| Method | Pin/Port | Notes |
|--------|----------|-------|
| **USB** | USB-B port | Easiest option, 5 V regulated by PC/adapter |
| **Barrel jack (VIN)** | 2.1 mm center-positive | Accepts 7–12 V (recommended 9 V); onboard regulator steps it down |
| **VIN pin** | Header pin labeled `VIN` | Same as barrel jack, feed 7–12 V into this pin |
| **+5 V pin** | Header pin labeled `5V` | Direct regulated 5 V input (⚠️ bypasses onboard regulator, only safe if supply is stable) |

⚠️ Never connect power simultaneously to multiple inputs (e.g. USB and VIN) unless you know exactly how Arduino’s auto-select circuitry works.  

## ⚙️ Software Behavior
- **READY = LOW** → warm-up → smoke disabled  
- **DMX mapping**  
  - `0` = smoke OFF  
  - `1` = short pulse (**0.1 s ON / 9.9 s OFF**)  
  - `255` = continuous smoke (**1 s ON every 1 s cycle → always ON**)  
  - Intermediate DMX values scale linearly between these extremes  
- **Pulse cycle**  
  - For each cycle:  
    - Smoke ON for mapped duration  
    - Smoke OFF until the mapped period completes  
  - At DMX 255, duration = period → machine runs continuously  
  - Responds instantly to DMX changes mid-cycle  
- **LED indicators**  
  - Pin 9: PWM LED shows DMX level  
  - Pin 10: LED mirrors smoke pulse (ON when machine is triggered)  
  - Pin 13: Built-in LED follows READY state  

## 📊 DMX Pulse Example


## ▶️ Usage
1. Print the enclosure and assemble the electronics.  
2. Mount to left or right side of the machine with existing screws.  
3. Connect to the **timer/remote port**.  
4. **Power the Arduino** via USB, barrel jack/VIN, or +5 V pin from a regulated supply.  
   ⚠️ Do **not** use the timer port’s +5 V — it cannot supply enough current.  
5. Upload the Arduino sketch (tested on Uno R3).  
6. Send DMX values to the configured channel.  

## ⚠️ Safety Notes
- Smoke machines run on **mains voltage** — isolate low-voltage electronics.  
- Double-check wiring before powering on.  
- Only use with machines confirmed to share the SDJ Mimetik-M pinout.  
- ⚠️ The **+5 V timer port output cannot power the Arduino** (insufficient current).  
  Use USB or the barrel jack with an external supply.  

## 📜 License
MIT License – free to use, adapt, and improve.  
