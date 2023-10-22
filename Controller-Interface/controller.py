# LEGACY -- DO NOT USE
# USB Joystick Interface for 2023 Red Robot Hackathon
# Reads button and axis input from Logitech Gamepad F310
# Packs data to be sent over serial to NRF24L01+ USB adapter

from time import time
import serial
from pyjoystick.sdl2 import Key, Joystick, run_event_loop
import pyjoystick
import random

radio = serial.Serial("COM13", baudrate=115200)
last_radio_time = time()

controller_state = (
    [0]*10, # Button state (0 or 1 for each of 10 buttons)
    [0.0]*6, # Axis state ([-1.0,1.0] for each of 6 axes)
    [255]) # Hat state (-1,0,1,2,3,4,5,6,7 clockwise quarter-turns from up)
controller_state[1][2] = -1.0
controller_state[1][5] = -1.0

# Dictionary to convert hat inputs to angles (not complete, some hat inputs are invalid)
hat_angles = {0: 255, 1: 0, 3: 1, 2: 2, 6: 3, 4: 4, 12: 5, 8: 6, 9: 7}

# Handle a keypress
def key_received(key):
    global controller_state, radio, last_radio_time
    if key.keytype == "Button" and key.number >= 0 and key.number <= 9:
        controller_state[0][key.number] = key.value 
    elif key.keytype == "Axis" and key.number >= 0 and key.number <=5:
        if key.number in [0,3]:
            controller_state[1][key.number] = key.value
        elif key.number in [1,4]:
            controller_state[1][key.number] = -key.value
        else: # key.number in [2,5]
            controller_state[1][key.number] = (2.0*key.value)-1.0
    elif key.keytype == "Hat" and key.number == 0:
        if key.value in hat_angles.keys():
            controller_state[2][0] = hat_angles[key.value]
    else:
        print("BAD INPUT: {} | {} | {}".format(key.keytype, key.number, key.value))

#run_event_loop(print_add, print_remove, key_received)
repeat = pyjoystick.Repeater(first_repeat_timeout=0.1, repeat_timeout=0.1, check_timeout=0.01)
evtloop = pyjoystick.ThreadEventManager(event_loop=run_event_loop,
                                        handle_key_event=key_received,
                                        button_repeater=repeat)
evtloop.start()

while True:
    t = time()
    if t - last_radio_time > 0.02:
        last_radio_time = t
        packed_controller_state = [int("".join(map(str,controller_state[0])),2) // 256,
                                   int("".join(map(str,controller_state[0])),2) % 256] + \
            list(map(lambda x: int(((x+1.0)/2.0)*255), controller_state[1])) + \
            controller_state[2]
        rand_bytes = random.randbytes(4)
        packed_controller_bytes = rand_bytes + bytes(packed_controller_state) + rand_bytes
        #print(len(packed_controller_bytes), packed_controller_bytes)
        radio.write(packed_controller_bytes)
