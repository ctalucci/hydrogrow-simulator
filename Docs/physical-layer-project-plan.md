# Physical Layer Project Plan - HydroGrow Sensors

## Project Overview
Build a complete hydroponic sensor system using ESP32 that can monitor pH, EC, temperature, and other critical parameters for integration with the HydroGrow Simulator game.

## Timeline: 4-6 Weeks

### Week 1-2: Planning & Procurement
### Week 3-4: Assembly & Testing
### Week 5-6: Integration & Refinement

---

## Phase 1: Hardware Selection & Procurement (Week 1-2)

### Core Components Shopping List

#### 1. **Microcontroller**
| Item | Model | Quantity | Price | Source | Notes |
|------|-------|----------|-------|--------|-------|
| ESP32 Dev Board | ESP32-WROOM-32 | 1 | $10-15 | Amazon/AliExpress | WiFi + Bluetooth built-in |
| Alternative | ESP32-S3 | 1 | $15-20 | Adafruit | Better performance |

#### 2. **Essential Sensors**
| Sensor | Model | Range | Price | Interface | Notes |
|--------|-------|-------|-------|-----------|-------|
| pH Sensor | Atlas Scientific EZO-pH | 0-14 pH | $80-100 | I2C/UART | Lab-grade accuracy |
| pH Sensor (Budget) | DFRobot SEN0161 | 0-14 pH | $30-40 | Analog | Good for beginners |
| EC Sensor | Atlas Scientific EZO-EC | 0.07-500,000 μS/cm | $90-110 | I2C/UART | Auto-ranging |
| EC Sensor (Budget) | DFRobot DFR0300 | 0-20 mS/cm | $40-50 | Analog | Requires calibration |
| Water Temp | DS18B20 Waterproof | -55 to 125°C | $5-10 | OneWire | Very reliable |
| Air Temp/Humidity | DHT22/AM2302 | -40 to 80°C | $10-15 | Digital | Good accuracy |

#### 3. **Optional Advanced Sensors**
| Sensor | Model | Price | Purpose |
|--------|-------|-------|---------|
| Dissolved Oxygen | Atlas Scientific EZO-DO | $200-250 | Advanced monitoring |
| CO2 | MH-Z19B | $20-30 | Growth optimization |
| Water Level | HC-SR04 Ultrasonic | $5-10 | Reservoir monitoring |
| Light Intensity | BH1750 | $5-10 | PAR approximation |

#### 4. **Supporting Hardware**
| Component | Specification | Quantity | Price | Purpose |
|-----------|--------------|----------|-------|---------|
| Breadboard | 830 points | 1 | $5-10 | Prototyping |
| Jumper Wires | M-M, M-F, F-F set | 1 set | $10 | Connections |
| Resistors | 4.7kΩ, 10kΩ | 10 each | $5 | Pull-ups |
| Probe Storage | Test tubes/holders | 1 set | $10-20 | Sensor care |
| Power Supply | 5V 2A USB | 1 | $10 | ESP32 power |
| Enclosure | IP65 rated box | 1 | $15-25 | Waterproofing |
| Terminal Blocks | Screw terminals | 10 | $5 | Secure connections |

#### 5. **Calibration Solutions**
| Solution | Purpose | Price |
|----------|---------|-------|
| pH 4.0 Buffer | pH sensor calibration | $10 |
| pH 7.0 Buffer | pH sensor calibration | $10 |
| pH 10.0 Buffer | pH sensor calibration | $10 |
| EC 1413 μS/cm | EC sensor calibration | $10 |
| EC 12880 μS/cm | EC sensor calibration | $10 |

### Total Budget Estimates
- **Basic Setup** (pH, EC, Temp): $150-200
- **Standard Setup** (+ DO, CO2): $400-500
- **Advanced Setup** (All sensors): $600-800

---

## Phase 2: Assembly Plan (Week 3)

### Day 1-2: Basic ESP32 Setup

#### Wiring Diagram - Basic Sensors
```
ESP32 Pin Connections:
├── pH Sensor (Analog)
│   ├── VCC → 3.3V
│   ├── GND → GND
│   └── Signal → GPIO34 (ADC)
│
├── EC Sensor (Analog)
│   ├── VCC → 3.3V
│   ├── GND → GND
│   └── Signal → GPIO35 (ADC)
│
├── DS18B20 (Water Temp)
│   ├── VCC → 3.3V
│   ├── GND → GND
│   └── Data → GPIO4 (with 4.7kΩ pull-up)
│
└── DHT22 (Air Temp/Humidity)
    ├── VCC → 3.3V
    ├── GND → GND
    └── Data → GPIO15 (with 10kΩ pull-up)
```

#### Assembly Steps
1. **Prepare Workspace**
   - [ ] ESD-safe mat
   - [ ] Good lighting
   - [ ] Multimeter ready
   - [ ] Soldering station (if needed)

2. **Initial Connections**
   - [ ] Mount ESP32 on breadboard
   - [ ] Connect power rails
   - [ ] Test ESP32 with blink sketch
   - [ ] Verify WiFi connectivity

3. **Sensor Integration** (One at a time)
   - [ ] Connect sensor power
   - [ ] Add pull-up resistors
   - [ ] Connect data lines
   - [ ] Test with example code
   - [ ] Document working configuration

### Day 3-4: Advanced Integration

#### I2C Sensor Setup (Atlas Scientific)
```
I2C Bus Configuration:
├── SDA → GPIO21
├── SCL → GPIO22
│
├── pH EZO Circuit
│   └── Address: 0x63
├── EC EZO Circuit
│   └── Address: 0x64
└── DO EZO Circuit
    └── Address: 0x61
```

#### Power Management
- Use separate 5V supply for sensors
- Common ground with ESP32
- Add capacitors for stability
- Consider isolated DC-DC converter

### Day 5: Enclosure & Waterproofing

#### Enclosure Layout
```
┌─────────────────────────┐
│  Cable Glands for       │
│  Sensor Probes         │
│  ┌───┐ ┌───┐ ┌───┐    │
│  │ pH│ │EC │ │DO │    │
│  └───┘ └───┘ └───┘    │
│                        │
│  ┌─────────────────┐  │
│  │    ESP32 Board  │  │
│  └─────────────────┘  │
│                        │
│  ┌─────────────────┐  │
│  │  Sensor Boards  │  │
│  └─────────────────┘  │
│                        │
│  Power Input  WiFi Ant │
└─────────────────────────┘
```

#### Waterproofing Steps
1. [ ] Drill holes for cable glands
2. [ ] Install waterproof connectors
3. [ ] Apply silicone sealant
4. [ ] Test enclosure seal
5. [ ] Add desiccant packets

---

## Phase 3: Firmware Development (Week 4)

### Basic Sensor Reading Code

```cpp
// config.h
#define WIFI_SSID "your-ssid"
#define WIFI_PASS "your-password"
#define MQTT_SERVER "192.168.1.100"
#define DEVICE_ID "hydro_sensor_01"

// Pin definitions
#define PH_PIN 34
#define EC_PIN 35
#define TEMP_PIN 4
#define DHT_PIN 15

// Reading intervals (ms)
#define SENSOR_INTERVAL 5000
#define UPLOAD_INTERVAL 30000
```

### Core Firmware Structure
```cpp
// main.ino
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Global objects
WiFiClient espClient;
PubSubClient mqtt(espClient);
OneWire oneWire(TEMP_PIN);
DallasTemperature waterTemp(&oneWire);
DHT dht(DHT_PIN, DHT22);

// Sensor data structure
struct SensorData {
  float ph;
  float ec;
  float waterTemp;
  float airTemp;
  float humidity;
  unsigned long timestamp;
};

void setup() {
  Serial.begin(115200);
  initSensors();
  connectWiFi();
  mqtt.setServer(MQTT_SERVER, 1883);
}

void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();
  
  static unsigned long lastRead = 0;
  if (millis() - lastRead > SENSOR_INTERVAL) {
    SensorData data = readSensors();
    publishData(data);
    lastRead = millis();
  }
}
```

### Calibration Procedures

#### pH Calibration
```cpp
void calibratePH() {
  Serial.println("=== pH Calibration ===");
  Serial.println("1. Place probe in pH 7.0 buffer");
  Serial.println("2. Wait for stable reading");
  Serial.println("3. Press 'n' for neutral point");
  
  // Wait for user input
  while (Serial.read() != 'n');
  float neutral = readRawPH();
  
  Serial.println("4. Rinse probe");
  Serial.println("5. Place in pH 4.0 buffer");
  Serial.println("6. Press 'a' for acid point");
  
  while (Serial.read() != 'a');
  float acid = readRawPH();
  
  // Calculate calibration values
  float slope = 3.0 / (neutral - acid);
  float intercept = 7.0 - (slope * neutral);
  
  // Save to EEPROM
  saveCalibration(slope, intercept);
}
```

---

## Phase 4: Testing & Validation (Week 5)

### Test Protocol

#### 1. **Dry Testing** (No water)
- [ ] Verify all connections
- [ ] Test each sensor individually
- [ ] Check WiFi connectivity
- [ ] Verify MQTT publishing
- [ ] Test error handling

#### 2. **Wet Testing** (With test solutions)
- [ ] Test with tap water
- [ ] Verify pH with buffer solutions
- [ ] Test EC with calibration solution
- [ ] Check temperature accuracy
- [ ] Long-term stability test (24h)

#### 3. **Integration Testing**
- [ ] Connect to local gateway
- [ ] Verify data in UE5 game
- [ ] Test alert systems
- [ ] Simulate sensor failures
- [ ] Test recovery mechanisms

### Validation Checklist
| Test | Expected Result | Pass/Fail | Notes |
|------|----------------|-----------|-------|
| pH accuracy | ±0.1 pH | | |
| EC accuracy | ±5% | | |
| Temperature | ±0.5°C | | |
| Update rate | 5 sec | | |
| WiFi stability | 99% uptime | | |
| Data format | Valid JSON | | |

---

## Phase 5: Production Deployment (Week 6)

### Installation Guide

#### 1. **Probe Placement**
```
Nutrient Reservoir Setup:
├── pH Probe
│   └── 2" from pump intake
├── EC Probe
│   └── Opposite side from pH
├── Temperature Probe
│   └── Center of reservoir
└── Water Level Sensor
    └── Above max fill line
```

#### 2. **Best Practices**
- Keep probes submerged
- Clean probes weekly
- Recalibrate monthly
- Store pH probe in KCl solution
- Avoid probe cross-contamination

### Maintenance Schedule
| Task | Frequency | Time Required |
|------|-----------|---------------|
| Visual inspection | Daily | 2 min |
| Data verification | Daily | 5 min |
| Probe cleaning | Weekly | 15 min |
| Calibration check | Bi-weekly | 20 min |
| Full calibration | Monthly | 30 min |
| Probe replacement | Yearly | 30 min |

---

## Troubleshooting Guide

### Common Issues & Solutions

#### 1. **Erratic pH Readings**
- **Cause**: Air bubbles on probe
- **Solution**: Tap probe gently
- **Prevention**: Angle probe 15°

#### 2. **EC Drift**
- **Cause**: Salt buildup
- **Solution**: Clean with distilled water
- **Prevention**: Regular cleaning

#### 3. **WiFi Disconnections**
- **Cause**: Weak signal
- **Solution**: Add external antenna
- **Prevention**: Use 2.4GHz only

#### 4. **Power Issues**
- **Cause**: Inadequate supply
- **Solution**: Use 5V 2A minimum
- **Prevention**: Quality power adapter

---

## Budget-Friendly Alternatives

### DIY pH Probe Circuit
- Use TL082 op-amp
- BNC connector
- Total cost: ~$15
- Accuracy: ±0.2 pH

### EC Probe Alternative
- Use stainless steel probes
- 555 timer circuit
- Total cost: ~$10
- Accuracy: ±10%

### Student Kit Version
| Component | Standard | Budget | Savings |
|-----------|----------|--------|---------|
| pH Sensor | Atlas $80 | DFRobot $30 | $50 |
| EC Sensor | Atlas $90 | DIY $15 | $75 |
| Enclosure | IP65 $25 | Tupperware $5 | $20 |
| **Total** | **$195** | **$50** | **$145** |

---

## Educational Extensions

### 1. **Classroom Kit** (30 students)
- 6 sensor stations (5 students each)
- Shared calibration solutions
- Central data dashboard
- Total cost: ~$1,200

### 2. **Science Fair Project**
- Compare sensor accuracy
- Long-term data logging
- Growth correlation analysis
- Cost analysis vs. commercial

### 3. **Advanced Projects**
- Machine learning predictions
- Multi-node mesh network
- Solar powered stations
- LoRaWAN integration

---

## Resources & References

### Supplier Links
- **Adafruit**: ESP32, sensors, tutorials
- **Atlas Scientific**: Professional sensors
- **DFRobot**: Budget sensors
- **Amazon**: General components
- **AliExpress**: Bulk orders

### Documentation
- [ESP32 Pinout Reference](https://docs.espressif.com)
- [Sensor Datasheets](link-to-folder)
- [Calibration Guides](link-to-guides)
- [Safety Data Sheets](link-to-sds)

### Community Support
- HydroGrow Discord
- r/hydro subreddit
- ESP32 forums
- Local maker spaces

---

*This project plan provides a structured approach to building the physical sensor layer for HydroGrow Simulator, balancing professional quality with educational accessibility.*