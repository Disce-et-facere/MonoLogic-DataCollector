package com.systemintegration.backend.controller;

import com.systemintegration.backend.model.SensorData;
import com.systemintegration.backend.service.SensorDataService;
import com.systemintegration.backend.service.IoTDeviceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/sensor-data")
public class SensorDataController {

    @Autowired
    private SensorDataService sensorDataService;

    @Autowired
    private IoTDeviceService deviceService;

    @GetMapping
    public List<SensorData> getAllSensorData() {
        return sensorDataService.getAllSensorData();
    }

    @GetMapping("/{mac}")
    public ResponseEntity<List<SensorData>> getSensorDataByMac(@PathVariable String mac) {
        // Validate that the MAC address exists in the IoT devices
        boolean isValid = deviceService.validateMAC(mac);

        if (isValid) {
            // Get all sensor data for the provided MAC address
            List<SensorData> sensorDataList = sensorDataService.getSensorDataByMac(mac);
            return ResponseEntity.ok(sensorDataList);
        } else {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(null); // Return 404 if the MAC is not found
        }
    }

    @PostMapping("/{mac}")
    public ResponseEntity<String> saveSensorData(@PathVariable String mac, @RequestBody SensorData sensorData) {
        boolean isValid = deviceService.validateMAC(mac);

        if (isValid) {
            sensorData.setMac(mac);
            sensorDataService.saveSensorData(sensorData);
            return ResponseEntity.ok("Sensor data saved successfully");
        } else {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("Device not authorized");
        }
    }
}
