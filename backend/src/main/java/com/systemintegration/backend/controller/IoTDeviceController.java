package com.systemintegration.backend.controller;

import com.systemintegration.backend.service.IoTDeviceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;
import org.springframework.http.HttpStatus;

@RestController
@RequestMapping("/api")
public class IoTDeviceController {

    @Autowired
    private IoTDeviceService deviceService;

    @PostMapping("/add-iot-device")
    public ResponseEntity<String> addDevice(@RequestParam String name, @RequestParam String mac) {
        deviceService.addDevice(name, mac);
        return ResponseEntity.status(HttpStatus.CREATED).body("Device added successfully");
    }

    @GetMapping("/authenticate-iot-device")
    public ResponseEntity<String> authenticateDevice(@RequestParam String mac) {
        deviceService.authenticateDevice(mac);
        return ResponseEntity.ok("Device authenticated");
    }
}