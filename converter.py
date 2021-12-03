#!/usr/bin/env python3
from typing import List, NewType, Tuple
import cv2
import numpy as np
import time

from c_converter import fast_print, fast_convert_frame

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



def convert_video(file: str) -> List[str]:
    print(f'Loading video file {file}')
    cap = cv2.VideoCapture(file)

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_count = cap.get(cv2.CAP_PROP_FRAME_COUNT)
    frame_rate = cap.get(5)

    print(F'Frame rate: {frame_rate}')
    print(f'Size: {width}, {height}')

    frames: List[str] = []

    counter = 0
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        counter += 1
        print(f'\r{counter}/{frame_count} frames converted', end='')
        
        frames.append(fast_convert_frame(bytes(frame), width, height))
        #frames.append(convert_frame_optimized(frame, width, height))
    
    cap.release()

    return frames, frame_rate
    

def print_frames(frames: List[str], frame_rate: float) -> None:

    base_delay = 1 / frame_rate

    for frame in frames:
        time.sleep(base_delay)
        fast_print(frame)


def main() -> None:
    file = 'video.mp4'    

    frames, frame_rate = convert_video(file)

    input('\nPress Enter to show video...')

    #print_frames(frames, frame_rate)

    print("len frames", len(frames))
    for frame in frames:
        print(len(frame))


if __name__ == '__main__':
    main()

