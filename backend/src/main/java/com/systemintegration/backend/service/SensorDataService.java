package com.systemintegration.backend.service;

import com.systemintegration.backend.model.IoTDevice;
import com.systemintegration.backend.model.SensorData;
import com.systemintegration.backend.repository.SensorDataRepository;
import com.systemintegration.backend.repository.IoTDeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SensorDataService {

    @Autowired
    private SensorDataRepository sensorDataRepository;

    @Autowired
    private IoTDeviceRepository deviceRepository;

    public List<SensorData> getAllSensorData() {
        return sensorDataRepository.findAll();
    }

    public List<SensorData> getSensorDataByMac(String mac) {
        return sensorDataRepository.findByMac(mac);
    }

    public String getDeviceNameByMac(String mac) {
        return deviceRepository.findByMac(mac)
                .map(IoTDevice::getName)
                .orElse("Unknown Device");
    }

    public SensorData saveSensorData(SensorData sensorData) {
        return sensorDataRepository.save(sensorData);
    }
}
