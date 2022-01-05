#!/usr/bin/env python3
import time
from io import BufferedReader
from sys import argv

from c_converter import fast_print


class FileInfo:
    """A class that holds information about the file."""
    def __init__(self, width: int, height: int, frame_rate: int, frame_count: int):
        self.width = width
        self.height = height
        self.frame_rate = frame_rate
        self.frame_count = frame_count
        self.frame_buffer_size = (self.width * 2) * self.height
    
    def __str__(self):
        return f'FileInfo: {self.width}x{self.height} | {self.frame_rate} fps | {self.frame_count} frames | {self.frame_buffer_size} bytes'

    def __repr__(self) -> str:
        return self.__str__()


def read_header(file: BufferedReader) -> FileInfo:
    """Reads the header of the file and prints it to the console."""
    width = int.from_bytes(file.read(2), 'big')
    height = int.from_bytes(file.read(2), 'big')
    frame_rate = int.from_bytes(file.read(1), 'big')
    frame_count = int.from_bytes(file.read(8), 'big')
    return FileInfo(width, height, frame_rate, frame_count)


def main() -> None:
    input_file = argv[1]

    with open(input_file, 'rb') as file:
        file_info = read_header(file)

        # Time every frame should take to be printed in nanoseconds.
        base_delay = int(1 / file_info.frame_rate * 1e9)

        for _ in range(file_info.frame_count):
            fast_print(file.read(file_info.frame_buffer_size).decode('ascii'), base_delay)


if __name__ == '__main__':
    main()

