#!/usr/bin/env python3
import cv2
import os
import pathlib
import math
import time
from sys import argv
from io import BufferedWriter

from c_converter import fast_convert_frame


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
    The byte order is big endian.
    """
    output_file.write(
        int.to_bytes(width, 2, 'big') +     # First 2 bytes: width
        int.to_bytes(height, 2, 'big') +    # Second 2 bytes: height
        normalize_frame_rate(frame_rate) +  # Third byte: frame rate
        int.to_bytes(frame_count, 8, 'big') # Last 8 bytes: frame count
    )


def convert_video(file_name: pathlib.Path, output_name: pathlib.Path) -> bool:
    
    print(f'Loading video file {file_name}')
    # Open the video file.
    cap = cv2.VideoCapture(file_name.name)

    # Check if the video file was opened successfully.
    if not cap.isOpened():
        print('Could not open video file')
        return False

    try:
        # Get the video file info.
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        frame_rate = float(cap.get(5))

        print(F'Frame rate: {frame_rate}')
        print(f'Size: {width}, {height}')
        print(f'Frame count: {frame_count}')

        # Create the output file.
        with open(output_name.name, 'wb') as output_file:

            # Write the header to the file with some useful metadata.
            write_file_header(output_file, width, height, frame_rate, frame_count)

            # Convert every frame of the video to ASCII art.
            counter = 0
            while cap.isOpened():
                # Read the next frame.
                ret, frame = cap.read()
                if not ret:
                    break

                counter += 1
                print(f'\r{counter}/{frame_count} frames converted', end='')
                
                # Convert the frame to ASCII art and write it to the file.
                output_file.write(fast_convert_frame(bytes(frame), width, height).encode('ascii'))
    
    # If an error occurs, print the error and return False.
    except Exception as e:
        print(f'\nError: {e}')
        return False

    # Close the video file in any case to avoid leaks.
    finally:
        cap.release()

    # Return True if the conversion was successful.
    return True


def main() -> None:
    # Load the file name from the command line arguments.
    file_name = pathlib.Path(argv[1])
    # Generate the output file name.
    output_name = pathlib.Path(file_name.stem + '.ascii')

    # Take the starting time to calculate the total time taken to convert the video.
    start = time.time()

    # Convert the video.
    success = convert_video(file_name, output_name)
    
    if success:
        print(f'\nConversion successful.\nTime: {time.time() - start}')
        print(f'\nOutput file saved as "{output_name}"')
    else:
        print('\nConversion failed')
        # If the conversion failed, delete the output file, if it was created.
        if os.path.exists(output_name.name):
            os.remove(output_name.name)


# Python's kind-of main function.
if __name__ == '__main__':
    main()

