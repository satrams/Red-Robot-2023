# USB Joystick Interface for 2023 Red Robot Hackathon
# Reads button and axis input from Logitech Gamepad F310
# Packs data to be sent over serial to NRF24L01+ USB adapter

import sys
import hid
import time
import serial
import random

radio = serial.Serial("COM13", baudrate=115200)

joystick_id = None
for device in hid.enumerate():
    if device["product_string"] == "Controller (Gamepad F310)":
        joystick_id = (device["vendor_id"], device["product_id"])
    #print(device["product_string"], device["vendor_id"], device["product_id"])
if joystick_id is None:
    print("Couldn't find joystick")
    sys.exit()
print("Found controller with id", joystick_id)
    
joystick = hid.device()
joystick.open(joystick_id[0], joystick_id[1])
joystick.set_nonblocking(True)

joystick_state = [127]*5 + [0]*2
lastT = time.time()
while True:
    report = joystick.read(64)
    t = time.time()
    if report:
        joystick_state = [report[0]] + [255-report[2]] + [report[4]] + [255-report[6]] + [report[9]] + [report[10], report[11]]
    if t - lastT >= 0.1:
        lastT = t
        rand_bytes = random.randbytes(4)
        packed_bytes = rand_bytes + bytes(joystick_state) + rand_bytes
        packed_bytes += bytes([sum(packed_bytes) % 256])
        radio.write(packed_bytes)
