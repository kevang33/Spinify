#!/usr/bin/env python

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
import time

reader = SimpleMFRC522()

try:
	while True:
#        	print("Waiting for you to scan an RFID sticker/card")
		id,text = reader.read()
		print("ID:", id)
		print("Text:", text)
		time.sleep(0.5)
finally:
	GPIO.cleanup()
