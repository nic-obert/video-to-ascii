#!/usr/bin/env python3
from io import BufferedWriter
import cv2
import os
import pathlib
import math
from sys import argv
import numpy as np
from typing import List, NewType, Tuple

from c_converter import fast_convert_frame


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


def normalize_frame_rate(frame_rate: float) -> bytes:
    """
    Normalizes the frame rate to an unsigned byte value between 1 and 255.
    """
    frame_rate = math.ceil(frame_rate)
    if frame_rate > 255:
        frame_rate = 255
    elif frame_rate < 1:
        frame_rate = 1
    
    return bytes([frame_rate])


def write_file_header(output_file: BufferedWriter, width: int, height: int, frame_rate: float, frame_count: int) -> None:
    """
    Writes the header of the file.
    The header takes up the first 13 bytes of the file.
    """
    output_file.write(
        int.to_bytes(width, 2, 'big') +
        int.to_bytes(height, 2, 'big') +
        normalize_frame_rate(frame_rate) + 
        int.to_bytes(frame_count, 8, 'big')
    )


def convert_video(file_name: pathlib.Path, output_name: pathlib.Path) -> bool:
    
    print(f'Loading video file {file_name}')
    cap = cv2.VideoCapture(file_name.name)

    if not cap.isOpened():
        print('Could not open video file')
        return False

    try:
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        frame_rate = float(cap.get(5))

        print(F'Frame rate: {frame_rate}')
        print(f'Size: {width}, {height}')
        print(f'Frame count: {frame_count}')

        with open(output_name.name, 'wb') as output_file:

            write_file_header(output_file, width, height, frame_rate, frame_count)

            counter = 0
            while cap.isOpened():
                ret, frame = cap.read()
                if not ret:
                    break

                counter += 1
                print(f'\r{counter}/{frame_count} frames converted', end='')
                
                output_file.write(fast_convert_frame(bytes(frame), width, height).encode('ascii'))
    
    except Exception as e:
        print(e)
        return False

    finally:
        cap.release()

    return True


def main() -> None:
    file_name = pathlib.Path(argv[1])
    output_name = pathlib.Path(file_name.stem + '.ascii')

    success = convert_video(file_name, output_name)

    if success:
        print(f'\nConverted file saved as "{output_name}"')
    else:
        print('\nConversion failed')
        if os.path.exists(output_name.name):
            os.remove(output_name.name)


if __name__ == '__main__':
    main()

