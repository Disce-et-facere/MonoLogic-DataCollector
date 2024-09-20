#!/bin/bash
USB_PORT="$1"
START_DIR=""

command -v idf.py > /dev/null 2>&1 || { 
  echo "idf.py not found, adding to export"
  if [ -d ~/IOT23/esp/esp-idf/ ]; then
    START_DIR=$PWD
    cd ~/IOT23/esp/esp-idf/
    source export.sh
    echo "START DIR IS $START_DIR"
    cd $START_DIR
  else
    echo "Could not find idf dir, Exiting"
    exit 1;
  fi
}

echo "Starting build"

cd esp32
BUILD_RESULT=$(idf.py build)
BUILD_SUCCESS=$(echo "$BUILD_RESULT" | rg -i "project build complete")
if [ -z "$BUILD_SUCCESS" ]; then
  echo "Build failed"
  echo "$BUILD_RESULT"
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

