#!/usr/bin/env python3
from typing import List, NewType, Tuple
import cv2


Pixel = NewType('Pixel', Tuple[int, int, int])
Frame = NewType('Frame', Tuple[List, List, Pixel])

CHARACTERS = (' ', '.', '°', '*', 'o', 'O', '#', '@')
MAX_CHANNEL_INTENSITY = 255
MAX_CHANNEL_VALUES = MAX_CHANNEL_INTENSITY * 3 # 3 is the number of channels of a Pixel


def map_intensity_to_character(intensity: float) -> CHARACTERS:
    return CHARACTERS[round(intensity * len(CHARACTERS) - 1)]


def get_pixel_intensity(pixel: Pixel) -> float:
    return sum(pixel) / MAX_CHANNEL_VALUES


def print_frame(frame: Frame, width: int, height: int) -> None:
    for y in range(height):
        for x in range(width):
            pixel = Pixel((frame[y,x,0], frame[y,x,1], frame[y,x,2]))
            intensity = get_pixel_intensity(pixel)
            character = map_intensity_to_character(intensity)
            print(character, end='')
        print()


cap = cv2.VideoCapture('video.mp4')

print(cap.get(5))
print(cap.get(cv2.CAP_PROP_FRAME_WIDTH))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    height, width, channels = frame.shape

    print_frame(frame, width, height)
    


cap.release()
cv2.destroyAllWindows()

