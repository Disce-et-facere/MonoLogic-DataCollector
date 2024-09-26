#!/bin/bash
USB_PORT="$1"
SSID="$2"
PASSWORD="$3"
NAME="$4"


function readData() {
  echo -n "$1" > $USB_PORT
  GREPRESULT=""
   while true;
   do
    read LINE < $USB_PORT
    GREPRESULT=$(echo $LINE | rg -i "$2")
    if [ ! -z "$GREPRESULT" ]; then
      echo "$GREPRESULT"
      return 
    fi
   done
}


echo "Sending SSID: $SSID"
readData "s$SSID" "SSID OK"

echo "Sending password: $PASSWORD"
readData "p$PASSWORD" "PW OK"

echo "Sending name: $NAME"
readData "n$NAME" "NAME OK"
