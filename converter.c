#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <time.h>


#define CHARACTERS_NUMBER 8
// The ASCII characters to be printed
const char CHARACTERS[CHARACTERS_NUMBER] = {' ', '.', '-', '*', 'o', 'O', '#', '@'};

// Maximum value of the RGB channels
const unsigned char MAX_CHANNEL_INTENSITY = 255;
const unsigned short MAX_CHANNEL_VALUES = MAX_CHANNEL_INTENSITY * 3; // 3 is the number of channels of a Pixel (red, green, blue)

// Some useful type definitions

// Pixel is an array of bytes (unsigned char) representing the RGB values of a pixel
typedef unsigned char* Pixel;

// Frame is an array of pixels
typedef Pixel* Frame;
    

// Associates a pixel intensity to an ASCII character
char mapIntensityToCharacter(float intensity) {
    return CHARACTERS[(int) roundf(intensity * (CHARACTERS_NUMBER - 1))];
}


// Converts a pixel to a floating point intensity
float getPixelIntensity(Pixel pixel) {
    return (pixel[0] + pixel[1] + pixel[2]) / MAX_CHANNEL_VALUES;
}


PyObject* fast_print(PyObject* self, PyObject* args) 
{
    // Get the starting time in nanoseconds
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    // Declare the arguments from Python
    const char* frame;
    unsigned int base_delay;

    // Parse the arguments from Python
    PyArg_ParseTuple(args, "sI", &frame, &base_delay);

    // Clear screen
    printf("\033[H\033[J");
    // Print the frame to the console
    printf("%s", frame);

    // Get the end time in nanoseconds
    timespec_get(&end, TIME_UTC);
    
    // Calculate the difference between the two times
    long int diff = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // Calculate the delay to sleep
    long int delay = base_delay - diff;

    // Sleep for the specified amount of time to achieve the desired frame rate
    if (delay > 0) {
        nanosleep((const struct timespec[]){{0, delay}}, NULL);
    }

    // Return a None object to Python, since the function doesn't return anything
    Py_RETURN_NONE;
}


PyObject* fast_convert_frame(PyObject* self, PyObject* args)
{
    // Declare the arguments that will be passed by Python
    PyBytesObject* frameBytes;
    unsigned short frameWidth;
    unsigned short frameHeight;
    
    // Parse the arguments
    if (!PyArg_ParseTuple(args, "SHH", &frameBytes, &frameWidth, &frameHeight)) {
        return NULL;
    }

    // Get the frame data
    Py_buffer pyBuffer;
    if (PyObject_GetBuffer((PyObject*) frameBytes, &pyBuffer, PyBUF_READ) < 0) {
        printf("Error getting buffer from frame\n");
        return NULL;
    }

    const unsigned char* frameBuffer = pyBuffer.buf;
    const unsigned short STRING_WIDTH = frameWidth * 2;
    const unsigned short BUFFER_WIDTH = frameWidth * 3;

    // Create the string to store the frame that will be returned
    char string[frameHeight * STRING_WIDTH];

    // Iterate over the rows of the frame and convert them to ASCII
    unsigned int yString = 0;
    for (unsigned short y = 0; y < frameHeight; y++, yString += STRING_WIDTH) {

        for (unsigned short xBuffer = 0, xString = 0; xString < STRING_WIDTH; xBuffer+=3, xString+=2) {
            // Calculate the position of the pixel in the frame
            const unsigned int framePosition = y * BUFFER_WIDTH + xBuffer;
            
            // Calculate the intensity of the pixel by its RGB values
            const float intensity = 
                (float) (frameBuffer[framePosition] + frameBuffer[framePosition + 1] + frameBuffer[framePosition + 2])
                / MAX_CHANNEL_VALUES;
            
            // Convert the intensity to a character using the CHARAACTERS lookup table
            const char character = CHARACTERS[(int) roundf(intensity * (CHARACTERS_NUMBER - 1))];
            
            // Store the character in the string
            string[yString + xString] = character;
            // Add a space after the character to fix the console aspect ratio
            string[yString + xString + 1] = ' ';
        }
        // Add a new line after each row
        string[yString + STRING_WIDTH - 1] = '\n';
    }
    // Add the null termination character at the end of the string
    string[frameHeight * STRING_WIDTH - 1] = '\0';

    // Release the previously allocated buffer to avoid memory leaks
    PyBuffer_Release(&pyBuffer);

    // return a Python string representing the frame
    return Py_BuildValue("s", string);
}


// Define the functions that will be exported to Python
PyMethodDef module_methods[] = 
{
    {"fast_print", fast_print, METH_VARARGS, "Print a string"},
    {"fast_convert_frame", fast_convert_frame, METH_VARARGS, "Convert a frame into a string"},
    {NULL} // this struct signals the end of the array
};


// Struct representing the module
struct PyModuleDef c_module =
{
    PyModuleDef_HEAD_INIT, // PyModuleDef_HEAD_INIT should always be the first element of the struct
    "c_converter", // Module name
    "Rapidly convert and print a frame to the console", // module description
    -1, // Module size (https://docs.python.org/3/extending/extending.html)
    module_methods // Methods associated with the module
};


// The initialization function that will be called when the module is loaded
PyMODINIT_FUNC PyInit_c_converter()
{
    return PyModule_Create(&c_module);
}

