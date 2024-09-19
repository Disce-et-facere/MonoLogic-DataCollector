# System Integration IoT Backend

## Overview

**System Integration IoT Backend** is a Spring Boot application designed to receive data from IoT devices via a REST API, store the data in a MySQL database using JPA/Hibernate, and provide endpoints for a frontend to retrieve the stored data. This backend is built using Kotlin, leveraging Spring Boot's powerful features like web services, security, and developer tools to deliver a robust and scalable IoT data management system.

## Project Setup

To quickly generate the boilerplate code for this backend, you can use the Spring Initializr. Spring Initializr allows you to set up a Spring Boot project with a specific set of dependencies and configurations.

### Generate the Project

Click on the link below to generate the initial setup of the project with the required dependencies and configuration:

[Generate System Integration IoT Backend Project](https://start.spring.io/#!type=gradle-project-kotlin&language=java&platformVersion=3.3.3&packaging=jar&jvmVersion=22&groupId=com.systemintegration&artifactId=backend&name=System%20Integration%20IoT%20Backend&description=A%20Spring%20Boot%20REST%20API%20for%20collecting%20and%20serving%20IoT%20device%20data%2C%20integrated%20with%20a%20MySQL%20database&packageName=com.systemintegration.backend&dependencies=web,devtools,data-jpa,security,actuator,mysql)

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

## How to Run

1. Make sure you have grade installed.

- On Mac use homebrew: `brew install gradle`

2. Clone or download the generated project.
3. Navigate to the project directory and run:
   ```bash
   ./gradlew bootRun
   ```
4. The application will start at http://localhost:8080.
