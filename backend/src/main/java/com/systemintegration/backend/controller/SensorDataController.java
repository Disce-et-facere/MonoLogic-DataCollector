package com.systemintegration.backend.controller;

import java.nio.charset.StandardCharsets;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.systemintegration.backend.dto.SensorDataResponseDTO;
import com.systemintegration.backend.model.SensorData;
import com.systemintegration.backend.service.IoTDeviceService;
import com.systemintegration.backend.service.SensorDataService;
import com.systemintegration.backend.service.IpDataService;

import jakarta.servlet.http.HttpServletRequest;
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
            // Check if latitude and longitude are provided in the request body
            if (sensorData.getLatitude() != null && sensorData.getLongitude() != null) {
                // Use the provided latitude and longitude from the request body
                System.out.println("Using provided latitude and longitude.");
            } else {
                // Perform IP lookup to get the location data if latitude and longitude are not
                // provided
                String clientIp = request.getHeader("X-Forwarded-For");

                if (clientIp != null) {
                    // Decode the IP string (in case it's encoded)
                    clientIp = java.net.URLDecoder.decode(clientIp, StandardCharsets.UTF_8);

                    // Split the IP string if it contains multiple IP addresses and take the first
                    // one
                    String[] ipAddresses = clientIp.split(",");
                    clientIp = ipAddresses[0].trim(); // Use the first IP address
                } else {
                    // Fallback to remote address if 'X-Forwarded-For' is not present
                    clientIp = request.getRemoteAddr();
                }

                Map<String, Object> ipLocation = ipDataService.getIpLocation(clientIp);

                // Extract latitude and longitude from the IP lookup response
                Double latitude = (Double) ipLocation.get("latitude");
                Double longitude = (Double) ipLocation.get("longitude");

                // Set latitude and longitude from the IP lookup result
                sensorData.setLatitude(latitude);
                sensorData.setLongitude(longitude);
            }

            // Save sensor data to the database
            sensorData.setMac(mac);
            sensorDataService.saveSensorData(sensorData);

            return ResponseEntity.ok("Sensor data saved successfully");
        } else {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("Device not authorized");
        }
    }

}
