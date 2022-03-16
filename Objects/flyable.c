#include "Python.h"
#include "flyable.h"

FlyableImpl* FlyableImpls = NULL;
int FlyableImplsCount = 0;


void flyable_add_impl(char* name, void* tp, void* vec)
{
    if (FlyableImplsCount == 0)
    {
        FlyableImpls = (FlyableImpl*) malloc(sizeof(FlyableImpl));
        FlyableImplsCount = 1;
        printf("Impl added\n");
    }
    else
    {
        ++FlyableImplsCount;
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
    /*PyObject* repr = PyObject_Repr(object);
    PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
    const char* bytes = PyBytes_AS_STRING(str);

    printf("% s\n", bytes);

    Py_XDECREF(repr);
    Py_XDECREF(str);*/

    for (int i = 0; i < FlyableImplsCount; ++i)
    {
        FlyableImpl* currentImpl = &FlyableImpls[i];
        if (PyFunction_Check(object))
        {
            PyFunctionObject* funcObj = (PyFunctionObject*)object;
            PyTypeObject* callType = object->ob_type;
            PyTypeObject* callTypeType = object->ob_type;
            if (PyUnicode_CompareWithASCIIString(funcObj->func_qualname, currentImpl->name) == 0)
            {
                printf("Ptr changed\n");
                //Flyable objects can be called with vector calls, so we set the flag
                callType->tp_flags = callType->tp_flags | _Py_TPFLAGS_HAVE_VECTORCALL;
                callType->tp_vectorcall = (vectorcallfunc)currentImpl->vec_call;
                callType->tp_vectorcall_offset = (char*) & funcObj->vectorcall - (char*) funcObj;

                //Set the tp call directly
                callType->tp_call = (ternaryfunc)currentImpl->tp_call;
                
                //Set the vector call using the offset
                char* offset = (char*) callType;
                offset += callTypeType->tp_vectorcall_offset;
                *offset = (char*) currentImpl->vec_call;         
            }
        }
        else if (PyMethod_Check(object))
        {

            PyMethodObject* methodObj = (PyMethodObject*)object;
            PyObject* classObj = methodObj->im_self;
            if (PyType_Check(classObj))
            {
                printf("Ptr changed for meth\n");
                PyTypeObject* obj = (PyTypeObject*)classObj;
                if (strcmp(obj->tp_name, currentImpl->name) == 0)
                {
                    obj->tp_call = (ternaryfunc)currentImpl->tp_call;
                    obj->tp_vectorcall = (vectorcallfunc)currentImpl->vec_call;
                    methodObj->vectorcall = (vectorcallfunc)currentImpl->vec_call;
                }
            }

        }
    }
}
