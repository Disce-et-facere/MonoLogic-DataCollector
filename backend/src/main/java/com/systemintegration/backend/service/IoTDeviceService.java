package com.systemintegration.backend.service;

import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.repository.IoTDeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.http.HttpStatus;
import org.springframework.web.server.ResponseStatusException;

import java.util.Optional;
import java.util.List;

@Service
public class IoTDeviceService {

    @Autowired
    private IoTDeviceRepository deviceRepository;

    public IoTDevice authenticateDevice(String mac) {
        return deviceRepository.findByMac(mac)
                .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Device not found"));
    }

    public boolean validateMAC(String mac) {
        boolean isValid = deviceRepository.findByMac(mac).isPresent();
        return isValid;
    }

    public String getDeviceNameByMac(String mac) {
        Optional<IoTDevice> deviceOptional = deviceRepository.findByMac(mac);
        return deviceOptional.map(IoTDevice::getName).orElse("Unknown Device");
    }

    public List<IoTDevice> getAllDevices() {
        return deviceRepository.findAll();
    }

    public void addDevice(String name, String mac) {
        IoTDevice device = new IoTDevice();
        device.setName(name);
        device.setMac(mac);
        deviceRepository.save(device);
    }

    public boolean removeDevice(String mac) {
        if (deviceRepository.existsById(mac)) {
            deviceRepository.deleteById(mac);
            return true;
        } else {
            return false;
        }
    }
}