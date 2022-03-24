#ifndef FLYABLE_H
#define FLYABLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "Python.h"
#include "cpython/funcobject.h"

//Represents the implementation of a flyable object
typedef struct FlyableImpl{
    PyTypeObject type;
    char* name;
    void* tp_call;
    void* vec_call;
}FlyableImpl;


PyAPI_FUNC(void) flyable_add_impl(char* name, void* tp, void* vec);

PyAPI_FUNC(void) flyable_set_implementation(PyObject* object);

PyAPI_FUNC(void) flyable_debug_print_int64(long long value);

PyAPI_FUNC(void) flyable_debug_print_cstr(char* debug);

PyAPI_FUNC(void) flyable_debug_print_ptr(char* ptr);

#ifdef __cplusplus
}
#endif
#endif 
