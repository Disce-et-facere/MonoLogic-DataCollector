package com.systemintegration.backend.controller;

import com.systemintegration.backend.model.SensorData;
import com.systemintegration.backend.service.SensorDataService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;

@RestController
@RequestMapping("/api/sensor-data")
public class SensorDataController {

    private static final Logger logger = LoggerFactory.getLogger(SensorDataController.class);

    @Autowired
    private SensorDataService sensorDataService;

    @GetMapping
    public List<SensorData> getAllSensorData() {
        return sensorDataService.getAllSensorData();
    }

    @PostMapping
    public SensorData saveSensorData(@RequestBody SensorData sensorData) {
        // Log a message when a POST request is received
        logger.info("Received POST request with data: {}", sensorData.toString());

        return sensorDataService.saveSensorData(sensorData);
    }
}
