#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>


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
    const char* frame;

    PyArg_ParseTuple(args, "s", &frame);

    // Clear screen
    printf("\033[H\033[J");
    printf("%s", frame);

    return Py_None;
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

    unsigned const char* buffer = pyBuffer.buf;
    const unsigned short stringWidth = frameWidth + 1;
    const unsigned short bufferWidth = frameWidth * 3;

    char string[frameHeight * stringWidth + 1];

    for (unsigned short y = 0; y < frameHeight; y++) {
        unsigned int yBufferPosition = y * bufferWidth;
        unsigned int yStringPosition = y * stringWidth;
        for (unsigned short xBuffer = 0, xString = 0; xString < frameWidth; xBuffer+=3, xString++) {
            unsigned int bufferPosition = yBufferPosition + xBuffer;
            float intensity = (float) (buffer[bufferPosition] + buffer[bufferPosition + 1] + buffer[bufferPosition + 2]) / MAX_CHANNEL_VALUES;
            char character = CHARACTERS[(int) roundf(intensity * (CHARACTERS_NUMBER - 1))];
            string[yStringPosition + xString] = character;
        }
        string[yStringPosition + frameWidth] = '\n';
    }
    string[frameHeight * stringWidth] = '\0';

    PyBuffer_Release(&pyBuffer);

    // return a Python string
    return Py_BuildValue("s", string);
}


PyMethodDef module_methods[] = 
{
    {"fast_print", fast_print, METH_VARARGS, "Method description"},
    {"fast_convert_frame", fast_convert_frame, METH_VARARGS, "Method description"},
    {NULL} // this struct signals the end of the array
};


// struct representing the module
struct PyModuleDef c_module =
{
    PyModuleDef_HEAD_INIT, // Always initialize this member to PyModuleDef_HEAD_INIT
    "c_converter", // module name
    "Module description", // module description
    -1, // module size (more on this later)
    module_methods // methods associated with the module
};


PyMODINIT_FUNC PyInit_c_converter()
{
    return PyModule_Create(&c_module);
}

