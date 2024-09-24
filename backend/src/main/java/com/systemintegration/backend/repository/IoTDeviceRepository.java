package com.systemintegration.backend.repository;

import com.systemintegration.backend.model.IoTDevice;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface IoTDeviceRepository extends JpaRepository<IoTDevice, String> {
    Optional<IoTDevice> findByMac(String mac);
}
