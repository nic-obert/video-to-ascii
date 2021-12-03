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
    return CHARACTERS[(int) roundf(intensity * CHARACTERS_NUMBER - 1)];
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
    fflush(stdout);

    return Py_None;
}


PyObject* fast_convert_frame(PyObject* self, PyObject* args)
{
    PyBytesObject* frame;
    unsigned short width;
    unsigned short height;
    
    if (!PyArg_ParseTuple(args, "SHH", &frame, &width, &height)) {
        return NULL;
    }

    Py_buffer pyBuffer;
    if (!PyObject_GetBuffer((PyObject*) frame, &pyBuffer, PyBUF_SIMPLE)) {
        printf("Error getting buffer from frame\n");
        return NULL;
    }

    unsigned char* buffer = pyBuffer.buf;
    char string[height * width + height + 1];

    for (unsigned short y = 0; y < height; y++) {
        unsigned int basePosition = y * width;
        for (unsigned short x = 0; x < width; x++) {
            unsigned int position = basePosition + x;
            float intensity = (float) (buffer[position] + buffer[position + 1] + buffer[position + 2]) / MAX_CHANNEL_VALUES;
            char character = CHARACTERS[(int) roundf(intensity * CHARACTERS_NUMBER - 1)];
            string[basePosition + x] = character;   
        }
        string[basePosition + width] = '\n';
    }
    string[height * width + height] = '\0';

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

