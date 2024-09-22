# Systemintegration_Gruppuppgift

### MySQL Database Scheme

## 1. `iot_devices` Table

- The primary key here is the `mac` address (that is unique to the hardware).

| mac            | name        |
| -------------- | ----------- |
| A1:B2:C3:D4:E5 | Simon-ESP32 |

## 2. `sensor_data` Table

- The `mac` column links the sensor data to the `iot_devices` table.

| id  | mac            | temperature | humidity | timestamp           | latitude | longitude |
| --- | -------------- | ----------- | -------- | ------------------- | -------- | --------- |
| 1   | A1:B2:C3:D4:E5 | 28.1        | 95.0     | 2024-09-19 12:55:55 | 59.3293  | 18.0686   |
