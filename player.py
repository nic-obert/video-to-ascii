#!/usr/bin/env python3
from io import BufferedReader
from sys import argv

from c_converter import fast_print


class FileInfo:
    """A class that holds metadata about the file."""
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
    """Reads the header of the file and returns it in a friendly format."""
    width = int.from_bytes(file.read(2), 'big')         # First 2 bytes: width
    height = int.from_bytes(file.read(2), 'big')        # Second 2 bytes: height
    frame_rate = int.from_bytes(file.read(1), 'big')    # Third byte: frame rate
    frame_count = int.from_bytes(file.read(8), 'big')   # Last 8 bytes: frame count
    return FileInfo(width, height, frame_rate, frame_count)


def main() -> None:
    # Get the file name from the command line arguments.
    input_file = argv[1]

    # Open the ASCII art file.
    with open(input_file, 'rb') as file:
        # Read the header to extract the metadata.
        file_info = read_header(file)

        # Time every frame should take to be printed in nanoseconds.
        frame_duration = int(1 / file_info.frame_rate * 1e9)

        # Iterate over every frame in the file.
        for _ in range(file_info.frame_count):
            # Read the frame and print it to the console.
            fast_print(file.read(file_info.frame_buffer_size).decode('ascii'), frame_duration)


# Python's kind-of main function.
if __name__ == '__main__':
    main()

