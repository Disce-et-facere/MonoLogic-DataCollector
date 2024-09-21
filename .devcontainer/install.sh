#!/bin/bash

# Gradle version to install
GRADLE_VERSION=8.10.1

# Update the package list and install dependencies
echo "Updating package list and installing dependencies..."
sudo apt-get update -y
sudo apt-get install -y wget unzip openjdk-22-jdk

# Download Gradle binary distribution
echo "Downloading Gradle version $GRADLE_VERSION..."
wget https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip -P /tmp

# Unzip the Gradle package
echo "Unzipping Gradle package..."
sudo unzip -d /opt/gradle /tmp/gradle-${GRADLE_VERSION}-bin.zip

# Create a symlink to the gradle binary
echo "Creating symlink for Gradle..."
sudo ln -s /opt/gradle/gradle-${GRADLE_VERSION}/bin/gradle /usr/bin/gradle

# Verify Gradle installation
echo "Verifying Gradle installation..."
gradle -v

echo "Gradle $GRADLE_VERSION installed successfully."

cd ..
docker-compose up --build