package com.systemintegration.backend.repository;

import com.systemintegration.backend.model.SensorData;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface SensorDataRepository extends JpaRepository<SensorData, Long> {
    // Custom query method to find sensor data by MAC address
    List<SensorData> findByMac(String mac);
}
