package com.systemintegration.backend.service;

import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.repository.IoTDeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.http.HttpStatus;
import org.springframework.web.server.ResponseStatusException;

import java.util.UUID;

@Service
public class IoTDeviceService {
    @Autowired
    private IoTDeviceRepository deviceRepository;

    public UUID addDevice(String name, String mac) {
        IoTDevice device = new IoTDevice();
        device.setName(name);
        device.setMac(mac);
        device = deviceRepository.save(device);
        return device.getId();
    }

    public UUID authenticateDevice(String mac) {
        return deviceRepository.findByMac(mac)
                .map(IoTDevice::getId)
                .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Device not found"));
    }

    public boolean validateUUID(UUID deviceId) {
        return deviceRepository.existsById(deviceId);
    }
}