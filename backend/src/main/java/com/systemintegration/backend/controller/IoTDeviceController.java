package com.systemintegration.backend.controller;

// import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.service.IoTDeviceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;
import org.springframework.http.HttpStatus;

import java.util.UUID;

@RestController
@RequestMapping("/api")
public class IoTDeviceController {
    @Autowired
    private IoTDeviceService deviceService;

    @PostMapping("/add-iot-device")
    public ResponseEntity<UUID> addDevice(@RequestParam String name, @RequestParam String mac) {
        UUID deviceId = deviceService.addDevice(name, mac);
        return ResponseEntity.ok(deviceId);
    }

    @GetMapping("/authenticate-iot-device")
    public ResponseEntity<UUID> authenticateDevice(@RequestParam String mac) {
        UUID deviceId = deviceService.authenticateDevice(mac);
        return ResponseEntity.ok(deviceId);
    }

    @PostMapping("/sensor-data")
    public ResponseEntity<String> postSensorData(@RequestParam UUID deviceId) {
        boolean isValid = deviceService.validateUUID(deviceId);
        if (isValid) {
            return ResponseEntity.ok("Success");
        } else {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("Fail");
        }
    }
}
