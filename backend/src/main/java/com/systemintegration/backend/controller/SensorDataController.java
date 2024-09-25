package com.systemintegration.backend.controller;

import com.systemintegration.backend.dto.SensorDataResponseDTO;
import com.systemintegration.backend.model.SensorData;
import com.systemintegration.backend.service.SensorDataService;
import com.systemintegration.backend.service.IoTDeviceService;
import com.systemintegration.backend.service.IpDataService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import jakarta.servlet.http.HttpServletRequest;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/sensor-data")
public class SensorDataController {

    @Autowired
    private SensorDataService sensorDataService;

    @Autowired
    private IoTDeviceService deviceService;

    @Autowired
    private IpDataService ipDataService;

    @GetMapping
    public ResponseEntity<List<SensorDataResponseDTO>> getAllSensorData() {
        List<SensorData> sensorDataList = sensorDataService.getAllSensorData();

        // Map sensor data to DTOs and fetch device names
        List<SensorDataResponseDTO> responseDTOs = sensorDataList.stream().map(sensorData -> {
            String deviceName = deviceService.getDeviceNameByMac(sensorData.getMac());
            SensorDataResponseDTO dto = new SensorDataResponseDTO();
            dto.setId(sensorData.getId());
            dto.setMac(sensorData.getMac());
            dto.setName(deviceName); // Set the device name
            dto.setTemperature(sensorData.getTemperature());
            dto.setHumidity(sensorData.getHumidity());
            dto.setTimestamp(sensorData.getTimestamp());
            dto.setLatitude(sensorData.getLatitude());
            dto.setLongitude(sensorData.getLongitude());
            return dto;
        }).toList();

        return ResponseEntity.ok(responseDTOs);
    }

    @GetMapping("/{mac}")
    public ResponseEntity<List<SensorDataResponseDTO>> getSensorDataByMac(@PathVariable String mac) {
        boolean isValid = deviceService.validateMAC(mac);

        if (isValid) {
            // Get all sensor data for the provided MAC address
            List<SensorData> sensorDataList = sensorDataService.getSensorDataByMac(mac);
            String deviceName = deviceService.getDeviceNameByMac(mac);

            // Map sensor data to DTOs
            List<SensorDataResponseDTO> responseDTOs = sensorDataList.stream().map(sensorData -> {
                SensorDataResponseDTO dto = new SensorDataResponseDTO();
                dto.setId(sensorData.getId());
                dto.setMac(mac);
                dto.setName(deviceName);
                dto.setTemperature(sensorData.getTemperature());
                dto.setHumidity(sensorData.getHumidity());
                dto.setTimestamp(sensorData.getTimestamp());
                dto.setLatitude(sensorData.getLatitude());
                dto.setLongitude(sensorData.getLongitude());
                return dto;
            }).toList();

            return ResponseEntity.ok(responseDTOs);
        } else {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(null);
        }
    }

    @PostMapping("/{mac}")
    public ResponseEntity<String> saveSensorData(@PathVariable String mac,
            @RequestBody SensorData sensorData,
            HttpServletRequest request) {
        boolean isValid = deviceService.validateMAC(mac);

        if (isValid) {
            // Perform IP lookup to get the location data
            String clientIp = request.getHeader("X-Forwarded-For");
            Map<String, Object> ipLocation = ipDataService.getIpLocation(clientIp);
            System.out.println("IP Location data: " + ipLocation);

            // Extract latitude and longitude from the IP lookup response
            Double latitude = (Double) ipLocation.get("latitude");
            Double longitude = (Double) ipLocation.get("longitude");

            // Save sensor data to the database
            sensorData.setMac(mac);
            sensorData.setLatitude(latitude); // Set the latitude
            sensorData.setLongitude(longitude); // Set the longitude

            sensorDataService.saveSensorData(sensorData);
            return ResponseEntity.ok("Sensor data saved successfully");
        } else {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("Device not authorized");
        }
    }
}
