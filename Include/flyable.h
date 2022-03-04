#ifndef FLYABLE_H
#define FLYABLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "python.h"
#include "cpython/funcobject.h"

//Represents the implementation of a flyable object
typedef struct FlyableImpl{
    PyObject* name;
    void* tp_call;
    void* vec_call;
}FlyableImpl;


FlyableImpl* FlyableImpls = NULL;
int FlyableImplsCount = 0;

void flyable_add_impl(PyObject* name,void* tp,void* vec)
{
    if (FlyableImpls == NULL)
    {
        FlyableImpls = (FlyableImpl*) malloc(sizeof(FlyableImpl));
        FlyableImplsCount = 1;
    }
    else
    {
        FlyableImplsCount++;
        FlyableImpls = (FlyableImpl*) realloc(FlyableImpls, sizeof(FlyableImpl) * FlyableImplsCount);
    }

    FlyableImpl* newImpl = &FlyableImpls[FlyableImplsCount - 1];
    newImpl->name = name;
    newImpl->tp_call = tp;
    newImpl->vec_call = vec;
}

//Get an object and try to match it to replace the given pointers
void flyable_set_implementation(PyObject* object)
{
    if (PyFunction_Check(object))
    {
        PyFunctionObject* funcObj = (PyFunctionObject*) object;
    }
    else if (PyMethod_Check(object))
    {
        PyMethodObject* methodObj = (PyMethodObject*) object;
    }
}

#ifdef __cplusplus
}
#endif
#endif 
