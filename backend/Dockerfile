# Use the official Gradle image with JDK 22 and Alpine as the base image
FROM gradle:8.10.1-jdk22-alpine AS build

# Set the working directory inside the container
WORKDIR /app

# Copy the Gradle wrapper and build files first for better caching
COPY gradlew* build.gradle.kts settings.gradle.kts ./
COPY gradle gradle

# Download dependencies before copying the project files
RUN ./gradlew build --no-daemon || return 0

# Copy the rest of the project
COPY . .

# Expose the port the application runs on
EXPOSE 5000

# Run the Spring Boot application with Gradle
CMD ["./gradlew", "bootRun"]