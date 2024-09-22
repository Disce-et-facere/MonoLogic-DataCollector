package com.systemintegration.backend.service;

import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.repository.IoTDeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.http.HttpStatus;
import org.springframework.web.server.ResponseStatusException;

@Service
public class IoTDeviceService {

    @Autowired
    private IoTDeviceRepository deviceRepository;

    public void addDevice(String name, String mac) {
        IoTDevice device = new IoTDevice();
        device.setName(name);
        device.setMac(mac);
        deviceRepository.save(device);
    }

    public IoTDevice authenticateDevice(String mac) {
        return deviceRepository.findByMac(mac)
                .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Device not found"));
    }

    public boolean validateMAC(String mac) {
        boolean isValid = deviceRepository.findByMac(mac).isPresent();
        return isValid;
    }
}