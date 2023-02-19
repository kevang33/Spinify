#!/usr/bin/env python

import RPi.GPIO as GPIO
from mfrc522 import MFRC522
from mfrc522 import SimpleMFRC522
from time import sleep
from datetime import datetime

reader = MFRC522()

try:
    while True:
        status, _ = reader.MFRC522_Request(reader.PICC_REQIDL)
        if status != reader.MI_OK:
            sleep(0.1)
            continue
        status, backData = reader.MFRC522_Anticoll()
        buf = reader.MFRC522_Read(0)
        reader.MFRC522_Request(reader.PICC_HALT)
        if buf:
            print(datetime.now().isoformat(), ':'.join([hex(x) for x in buf]))
finally:
        GPIO.cleanup()
