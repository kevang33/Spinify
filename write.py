#!/usr/bin/env python

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
import time

rfid = SimpleMFRC522()

try:
#        print("Waiting for you to scan an RFID sticker/card")
	rfid.write("Back in black")
	print("Written")
finally:
	GPIO.cleanup()
