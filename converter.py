#!/usr/bin/env python3
from typing import List, NewType, Tuple
import cv2
import numpy as np


Frame = NewType('Frame', Tuple[List, List, Tuple[int, int, int]])

CHARACTERS = np.array([' ', '.', '°', '*', 'o', 'O', '#', '@'])


MAX_CHANNEL_INTENSITY = 255
MAX_CHANNEL_VALUES = MAX_CHANNEL_INTENSITY * 3 # 3 is the number of channels of a Pixel


def convert_frame_optimized(frame: Frame, width: int, height: int) -> str:
    string = ''
    for y in range(height):
        for x in range(width):
            string += CHARACTERS[round(np.sum(frame[y,x]) / 95.625 - 1)]
        string += '\n'
    return string


cap = cv2.VideoCapture('video.mp4')

print(F'Frame rate: {cap.get(5)}')
print(f'Size: {cap.get(cv2.CAP_PROP_FRAME_WIDTH)}, {cap.get(cv2.CAP_PROP_FRAME_HEIGHT)}')
frame_count = cap.get(cv2.CAP_PROP_FRAME_COUNT)

frames: List[str] = []

counter = 0
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    counter += 1
    print(f'\r{counter}/{frame_count} frames converted', end='')

    height, width, channels = frame.shape
    
    frames.append(convert_frame_optimized(frame, width, height))
    

cap.release()

input('Press Enter to show video...')

for frame in frames:
    # clear screen
    print('\033[H\033[J', end='')
    print(frame)

