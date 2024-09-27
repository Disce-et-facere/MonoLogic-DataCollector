#!/bin/bash
USB_PORT="$1"
SSID="$2"
PASSWORD="$3"
NAME="$4"
declare -i NUMOK=0

function readData() {
  #echo -n "$1" #> $USB_PORT
  GREPRESULT=""
   while true;
   do
    read LINE < $USB_PORT
    GREPRESULT=$(echo $LINE | rg -i "$1")
    if [ ! -z "$GREPRESULT" ]; then
      echo "$GREPRESULT"
      NUMOK=$((NUMOK + 1))
      return 
    else
      echo $LINE
    fi
   done
}

echo "Sending SSID: $SSID"
echo -n "s$SSID" > $USB_PORT
readData "SSID OK"

echo "Sending password: $PASSWORD"
echo -n "p$PASSWORD" > $USB_PORT
readData "PW OK"

echo "Sending name: $NAME"
echo -n "n$NAME" > $USB_PORT
readData "NAME OK"

if [[ "$NUMOK" -eq 3 ]]; then
  while true; do
    read -p "Commit to NVS? (y/n) " commit 
    case $commit in
      [yY] ) 
        echo "Sending commit"
        echo -n "c" > $USB_PORT
        readData "All"
        break;;
      [nN] ) 
        break;;
    esac
  done
fi
