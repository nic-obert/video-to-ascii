#define PY_SSIZE_T_CLEAN
#include <Python.h>


#define CHARACTERS_NUMBER 8
const short CHARACTERS[CHARACTERS_NUMBER] = {' ', '.', '-', '*', 'o', 'O', '#', '@'};

const unsigned char MAX_CHANNEL_INTENSITY = 255;
const unsigned short MAX_CHANNEL_VALUES = MAX_CHANNEL_INTENSITY * 3; // 3 is the number of channels of a Pixel


typedef unsigned char Pixel[3];

typedef Pixel* Frame;
    


char mapIntensityToCharacter(float intensity) {
    return CHARACTERS[(int) roundf(intensity * CHARACTERS_NUMBER - 1)];
}


float getPixelIntensity(Pixel pixel) {
    unsigned short intensity = 0;
    intensity += pixel[0] + pixel[1] + pixel[2];
    return intensity / MAX_CHANNEL_VALUES;
}


void printFrame(Frame frame, unsigned short width, unsigned short height) {
    char string[height * width + height];
    for (unsigned short y = 0; y < height; y++) {
        unsigned int basePosition = y * width;
        for (unsigned short x = 0; x < width; x++) {
            string[basePosition + x] = mapIntensityToCharacter(getPixelIntensity(frame[basePosition + x]));
        }
        string[basePosition + width] = '\n';
    }
    printf(string);
}

PyObject* convert_and_print(PyObject* self, PyObject* args)
{
    PyBytesObject* frame;
    unsigned short width;
    unsigned short height;
    PyArg_ParseTuple(args, "SHH", &frame, &width, &height);

    printf("\033[H\033[J");
    printFrame(frame, width, height);

    return Py_None;
}


PyMethodDef module_methods[] = 
{
    {"convert_and_print", convert_and_print, METH_VARARGS, "Method description"},
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

