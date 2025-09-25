# esphome_adc_rms
This is a dirty Esphome component to measure RMS values from AD1115 or a similar sensor.
It is based on https://esphome.io/components/sensor/ct_clamp.html.

I am license-agnostic, so wish you.

```yaml
i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true
  frequency: 200kHz

ads1115:
  - address: 0x48
    continuous_mode: True

external_components:
  - source: github://flicker581/esphome_adc_rms
    components: [adc_rms]

sensor:
  - platform: ads1115
    multiplexer: 'A0_GND'
    gain: 4.096
    id: ads_v1
    update_interval: never
  - platform: ads1115
    multiplexer: 'A1_GND'
    gain: 4.096
    id: ads_v2
    update_interval: never
  - platform: ads1115
    multiplexer: 'A3_GND'
    gain: 4.096
    id: ads_v3
    update_interval: never
  - platform: ads1115
    multiplexer: 'A0_A1'
    gain: 4.096
    id: ads_v1_2
    update_interval: never
  - platform: ads1115
    multiplexer: 'A0_A3'
    gain: 4.096
    id: ads_v1_3
    update_interval: never
  - platform: ads1115
    multiplexer: 'A1_A3'
    gain: 4.096
    id: ads_v2_3
    update_interval: never

  - platform: adc_rms
    sensor: ads_v1
    name: "Phase 1 RMS voltage"
    update_interval: 10s
    sample_duration: 1s
    filters:
      - multiply: 500.0
  - platform: adc_rms
    sensor: ads_v2
    name: "Phase 2 RMS voltage"
    sample_duration: 1s
    update_interval: 10s
    filters:
      - multiply: 500.0
  - platform: adc_rms
    sensor: ads_v3
    name: "Phase 3 RMS voltage"
    update_interval: 10s
    sample_duration: 1s
    filters:
      - multiply: 500.0
  - platform: adc_rms
    sensor: ads_v1_2
    name: "Phase 1-2 RMS voltage"
    update_interval: 10s
    sample_duration: 1s
    filters:
      - multiply: 500.0
  - platform: adc_rms
    sensor: ads_v1_3
    name: "Phase 1-3 RMS voltage"
    update_interval: 10s
    sample_duration: 1s
    filters:
      - multiply: 500.0
  - platform: adc_rms
    sensor: ads_v2_3
    name: "Phase 2-3 RMS voltage"
    update_interval: 10s
    sample_duration: 1s
    filters:
      - multiply: 500.0
  - platform: uptime
    name: "Uptime Sensor"
  - platform: wifi_signal
    name: "WiFi Signal Sensor"
    update_interval: 60s
    filters:    
      - or:
        - throttle: 10min
        - delta: 0.1
    entity_category: "diagnostic"

```
