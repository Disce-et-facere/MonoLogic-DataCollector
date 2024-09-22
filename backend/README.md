# Java Spring Boot Backend

## Overview

**System Integration IoT Backend** is a Spring Boot application designed to receive data from IoT devices via a REST API, store the data in a MySQL database using JPA/Hibernate, and provide endpoints for a frontend to retrieve the stored data. This backend is built using Kotlin, leveraging Spring Boot's powerful features like web services, security, and developer tools to deliver a robust and scalable IoT data management system.

## Project Setup

To quickly generate the boilerplate code for this backend, you can use the Spring Initializr. Spring Initializr allows you to set up a Spring Boot project with a specific set of dependencies and configurations.

### Generate the Project

Click on the link below to generate the initial setup of the project with the required dependencies and configuration:

[Generate System Integration IoT Backend Project](https://start.spring.io/#!type=gradle-project-kotlin&language=java&platformVersion=3.3.3&packaging=jar&jvmVersion=22&groupId=com.systemintegration&artifactId=backend&name=System%20Integration%20IoT%20Backend&description=A%20Spring%20Boot%20REST%20API%20for%20collecting%20and%20serving%20IoT%20device%20data%2C%20integrated%20with%20a%20MySQL%20database&packageName=com.systemintegration.backend&dependencies=web,devtools,data-jpa,security,actuator,mysql)

_Note: In this project we decided to remove Spring Security as we only needed HTTPS support that the nginx reverse proxy will handle /ST 2024-09-22_

This link will generate a Spring Boot project with the following configurations:

- **Type**: Gradle (Kotlin)
- **Language**: Java
- **Spring Boot Version**: 3.3.3
- **Packaging**: Jar
- **Java Version**: 22
- **Group**: `com.systemintegration`
- **Artifact**: `backend`
- **Name**: `System Integration IoT Backend`
- **Description**: `A Spring Boot REST API for collecting and serving IoT device data, integrated with a MySQL database`
- **Package Name**: `com.systemintegration.backend`

### Included Dependencies:

- **Spring Web**: For building the REST API.
- **Spring Boot DevTools**: For hot reloading during development.
- **Spring Data JPA**: For integrating with MySQL using JPA/Hibernate.
- **Spring Security**: For securing the REST API.
- **Spring Boot Actuator**: For monitoring and managing the application.
- **MySQL Driver**: For connecting to a MySQL database.

## API Endpoint Structure

### IoT Device Endpoints

#### 1. **Get All IoT Devices**

- **Endpoint**: `GET /api/iot-device`
- **Description**: Retrieve all registered IoT devices from MySQL database.
- **Response**:
  ```json
  [
    {
      "mac": "A1:B2:C3:D4:E5",
      "name": "Device1"
    },
    {
      "mac": "B2:C3:D4:E6:F7",
      "name": "Device2"
    }
  ]
  ```

#### 2. **Add a New IoT Device**

- **Endpoint**: `POST /api/iot-device`
- **Description**: Register a new IoT device with a `mac` and `name`.
- **Parameters**:
  - `name` (String): Name of the device.
  - `mac` (String): Unique MAC address of the device.
- **Response**:
  ```json
  {
    "message": "Device added successfully"
  }
  ```

#### 3. **Authenticate IoT Device**

- **Endpoint**: `POST /api/iot-device/authenticate`
- **Description**: Authenticate a device by its MAC address.
- **Parameters**:
  - `mac` (String): MAC address of the device.
- **Response**:
  ```json
  {
    "message": "Device authenticated"
  }
  ```

#### 4. **Delete an IoT Device**

- **Endpoint**: `DELETE /api/iot-device/{mac}`
- **Description**: Remove a device from the system using its MAC address.
- **Parameters**:
  - `mac` (String): MAC address of the device to be deleted.
- **Response**:
  ```json
  {
    "message": "Device removed successfully"
  }
  ```

### Sensor Data Endpoints

#### 1. **Get All Sensor Data**

- **Endpoint**: `GET /api/sensor-data`
- **Description**: Retrieve all stored sensor data from all devices.
- **Response**:
  ```json
  [
    {
      "id": 1,
      "mac": "A1:B2:C3:D4:E5",
      "name": "Test-ESP32-Device",
      "temperature": 21.1,
      "humidity": 55.0,
      "timestamp": "2024-09-22T16:01:00",
      "latitude": 59.3293,
      "longitude": 18.0686
    }
  ]
  ```

#### 2. **Get Sensor Data by MAC**

- **Endpoint**: `GET /api/sensor-data/{mac}`
- **Description**: Retrieve sensor data for a specific device using its MAC address.
- **Parameters**:
  - `mac` (String): MAC address of the device.
- **Response**:
  ```json
  [
    {
      "id": 1,
      "mac": "A1:B2:C3:D4:E5",
      "name": "Test-ESP32-Device",
      "temperature": 21.1,
      "humidity": 55.0,
      "timestamp": "2024-09-22T16:01:00",
      "latitude": 59.3293,
      "longitude": 18.0686
    }
  ]
  ```

#### 3. **Submit Sensor Data**

- **Endpoint**: `POST /api/sensor-data/{mac}`
- **Description**: Submit new sensor data for a specific IoT device.
- **Parameters**:
  - `mac` (String): MAC address of the device.
  - **Body**:
    ```json
    {
      "temperature": 29.1,
      "humidity": 99.0,
      "timestamp": "2024-09-19T12:55:55",
      "latitude": 59.3293, // Optional!
      "longitude": 18.0686 // Optional!
    }
    ```
- **Response**:
  ```json
  {
    "message": "Sensor data saved successfully"
  }
  ```

# Optional Information

## How to Run without Docker

1. Make sure you have grade installed.

- On Mac use homebrew: `brew install gradle`

2. Clone or download the generated project.
3. Navigate to the project directory and run:
   ```bash
   ./gradlew bootRun
   ```
4. The application will start at http://localhost:5000.

## API Endpoint Testing Examples

### 1. Add a New IoT Device

Test the `POST /api/iot-devices` endpoint using `curl` to add a new IoT device:

```bash
curl -i -k -X POST https://localhost/api/iot-device \
-H "Content-Type: application/x-www-form-urlencoded" \
-d 'name=Device1&mac=A1:B2:C3:D4:E5'
```

Test the `POST /api/sensor-data/{mac}` endpoint using `curl` to add a new sensor data:

```bash
curl -i -k -X POST https://localhost/api/sensor-data/A1:B2:C3:D4:E5 \
-H "Content-Type: application/json" \
-d '{
      "temperature": 22.5,
      "humidity": 55.0,
      "timestamp": "2024-09-22T12:00:00"
    }'
```

Test the `GET /api/sensor-data` endpoint using `curl` to get all sensor data:

```bash
curl -i -k -X GET https://localhost/api/sensor-data
```

Check directly in SQL database:

```bash
docker exec -it db-mysql mysql -u root -p
# Enter DB_ROOT_PASSWORD password mentioned in .env file
USE system_integration_db;
SELECT * FROM sensor_data;
```

## Reset mySQL database

_TODO: Move to separate README_

```bash
docker-compose down
docker volume ls # Optional: to find your volume
docker volume rm <your_project_name>_mysql-data
```

To list all users created at init:

```bash
docker exec -it db-mysql mysql -u root -p

### Enter root password on the command line ###

SELECT user, host FROM mysql.user;

### Make sure that the user specified in .env file was created ###
```
