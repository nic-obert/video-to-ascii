#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <time.h>


#define CHARACTERS_NUMBER 8
const char CHARACTERS[CHARACTERS_NUMBER] = {' ', '.', '-', '*', 'o', 'O', '#', '@'};

const unsigned char MAX_CHANNEL_INTENSITY = 255;
const unsigned short MAX_CHANNEL_VALUES = MAX_CHANNEL_INTENSITY * 3; // 3 is the number of channels of a Pixel


typedef unsigned char* Pixel;

typedef Pixel* Frame;
    

char mapIntensityToCharacter(float intensity) {
    return CHARACTERS[(int) roundf(intensity * (CHARACTERS_NUMBER - 1))];
}


float getPixelIntensity(Pixel pixel) {
    return (pixel[0] + pixel[1] + pixel[2]) / MAX_CHANNEL_VALUES;
}


PyObject* fast_print(PyObject* self, PyObject* args) 
{
    // Get the starting time in nanoseconds
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    const char* frame;
    unsigned int base_delay;

    PyArg_ParseTuple(args, "sI", &frame, &base_delay);

    // Clear screen
    printf("\033[H\033[J");
    printf("%s", frame);

    // Get the end time in nanoseconds
    timespec_get(&end, TIME_UTC);
    
    // Calculate the difference between the two times
    long int diff = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // Calculate the delay to sleep
    long int delay = base_delay - diff;

    // Sleep
    if (delay > 0) {
        nanosleep((const struct timespec[]){{0, delay}}, NULL);
    }

    Py_RETURN_NONE;
}


PyObject* fast_convert_frame(PyObject* self, PyObject* args)
{
    PyBytesObject* frame;
    unsigned short frameWidth;
    unsigned short frameHeight;
    
    if (!PyArg_ParseTuple(args, "SHH", &frame, &frameWidth, &frameHeight)) {
        return NULL;
    }

    Py_buffer pyBuffer;
    if (PyObject_GetBuffer((PyObject*) frame, &pyBuffer, PyBUF_READ) < 0) {
        printf("Error getting buffer from frame\n");
        return NULL;
    }

    const unsigned char* buffer = pyBuffer.buf;
    const unsigned short STRING_WIDTH = frameWidth * 2;
    const unsigned short BUFFER_WIDTH = frameWidth * 3;

    char string[frameHeight * STRING_WIDTH];

    unsigned int yString = 0;
    for (unsigned short y = 0; y < frameHeight; y++, yString += STRING_WIDTH) {

        for (unsigned short xBuffer = 0, xString = 0; xString < STRING_WIDTH; xBuffer+=3, xString+=2) {
            const unsigned int bufferPosition = y * BUFFER_WIDTH + xBuffer;
            
            const float intensity = (float) (buffer[bufferPosition] + buffer[bufferPosition + 1] + buffer[bufferPosition + 2]) / MAX_CHANNEL_VALUES;
            const char character = CHARACTERS[(int) roundf(intensity * (CHARACTERS_NUMBER - 1))];
            
            string[yString + xString] = character;
            string[yString + xString + 1] = ' ';
            //printf("Space position: %d\n", yString + xString + 1);
        }
        //printf("Newline position: %d\n", yString + STRING_WIDTH - 1);
        string[yString + STRING_WIDTH - 1] = '\n';
    }

    string[frameHeight * STRING_WIDTH - 1] = '\0';
    //printf("Zero termination position: %d\n", frameHeight * STRING_WIDTH - 1);

    //getchar();

    PyBuffer_Release(&pyBuffer);

    // return a Python string
    return Py_BuildValue("s", string);
}


PyMethodDef module_methods[] = 
{
    {"fast_print", fast_print, METH_VARARGS, "Print a string"},
    {"fast_convert_frame", fast_convert_frame, METH_VARARGS, "Convert a frame into a string"},
    {NULL} // this struct signals the end of the array
};


// struct representing the module
struct PyModuleDef c_module =
{
    PyModuleDef_HEAD_INIT, // Always initialize this member to PyModuleDef_HEAD_INIT
    "c_converter", // module name
    "Rapidly convert and print a frame to the console", // module description
    -1, // module size (more on this later)
    module_methods // methods associated with the module
};


PyMODINIT_FUNC PyInit_c_converter()
{
    return PyModule_Create(&c_module);
}

