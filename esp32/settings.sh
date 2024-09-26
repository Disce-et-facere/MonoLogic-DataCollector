#!/bin/bash
USB_PORT="$1"
SSID="$2"
PASSWORD="$3"
NAME="$4"

echo -n "s$SSID" > $USB_PORT
echo "Sent SSID: $SSID"
sleep 2

echo -n "p$PASSWORD" > $USB_PORT
echo "Sent password: $PASSWORD"
sleep 2

echo -n "n$NAME" > $USB_PORT
echo "Sent name: $NAME"
