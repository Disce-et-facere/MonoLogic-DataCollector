#!/bin/bash
USB_PORT="$1"

echo "Starting build"

cd esp32
BUILD_RESULT=$(idf.py build)
BUILD_SUCCESS=$(echo "$BUILD_RESULT" | rg -i "project build complete")
if [ -z "$BUILD_SUCCESS" ]; then
  echo "Build failed"
  exit 1;
else
  echo "Build success"
fi

echo "Starting flash to $USB_PORT, can take a minute"
FLASH_RESULT=$(idf.py -p $USB_PORT flash)
FLASH_SUCCESS=$(echo "$FLASH_RESULT" | rg -i "done")
if [ -z "$FLASH_SUCCESS" ]; then
  echo "Flash failed"
  exit 1;
else
  echo "Flash Success"
fi

echo "Entering monitor"
idf.py -p $USB_PORT monitor

