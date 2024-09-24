package com.systemintegration.backend.controller;

import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.service.IoTDeviceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;
import org.springframework.http.HttpStatus;

import java.util.List;

@RestController
@RequestMapping("/api/iot-device")
public class IoTDeviceController {

    @Autowired
    private IoTDeviceService deviceService;

    // GET all devices
    @GetMapping
    public ResponseEntity<List<IoTDevice>> getAllDevices() {
        List<IoTDevice> devices = deviceService.getAllDevices();
        return ResponseEntity.ok(devices);
    }

    // POST to add a new device
    @PostMapping
    public ResponseEntity<String> addDevice(@RequestParam String name, @RequestParam String mac) {
        deviceService.addDevice(name, mac);
        return ResponseEntity.status(HttpStatus.CREATED).body("Device added successfully");
    }

    // DELETE to remove a device by MAC
    @DeleteMapping
    public ResponseEntity<String> removeDevice(@RequestParam String mac) {
        boolean isDeleted = deviceService.removeDevice(mac);
        if (isDeleted) {
            return ResponseEntity.ok("Device removed successfully");
        } else {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body("Device not found");
        }
    }

    // POST to authenticate device
    @PostMapping("/authenticate")
    public ResponseEntity<String> authenticateDevice(@RequestParam String mac) {
        deviceService.authenticateDevice(mac);
        return ResponseEntity.ok("Device authenticated");
    }
}