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
        self.frame_buffer_size = (self.width + 1) * self.height


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

        base_delay = 1 / file_info.frame_rate

        for _ in range(file_info.frame_count):
            start = time.time()
            frame = file.read(file_info.frame_buffer_size).decode('ascii')
            fast_print(frame)
            end = time.time()
            time.sleep(abs(base_delay - (end - start)))


if __name__ == '__main__':
    main()

