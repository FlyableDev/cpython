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

    for (int i = 0; i < FlyableImplsCount; ++i)
    {
        FlyableImpl* currentImpl = &FlyableImpls[i];
        PyTypeObject* implType = &FlyableImpls[i].type;
         
        memcpy((void*) implType, (void*) & PyFunction_Type, sizeof(PyFunction_Type));

        if (PyFunction_Check(object))
        {
            PyFunctionObject* funcObj = (PyFunctionObject*)object;
         
            PyTypeObject* callType = object->ob_type;
            if (PyUnicode_CompareWithASCIIString(funcObj->func_qualname, currentImpl->name) == 0)
            {
                //Change the function type so it refers to a flyable type
                funcObj->ob_base.ob_type = implType;
                Py_INCREF(implType);

                //Change the type funcs so it refers to flyable calls
                implType->tp_vectorcall = currentImpl->vec_call;
                implType->tp_call = currentImpl->tp_call;

                //Make sure the types indicates that it supportes vec call
                implType->tp_flags = implType->tp_flags & _Py_TPFLAGS_HAVE_VECTORCALL;

                //Set the type vector offset
                currentImpl->type.tp_vectorcall_offset = (char*) &funcObj->vectorcall -  (char*) funcObj;

                //Set the vector call using the offset
                char** offset = (char**)funcObj;
                offset += callType->tp_vectorcall_offset;
                *offset = (char*) currentImpl->vec_call;

                PyObject* repr = PyObject_Repr(object);
                PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
                const char* bytes = PyBytes_AS_STRING(str);
                printf("% s\n", bytes);
                Py_XDECREF(repr);
                Py_XDECREF(str);

            }
        }
        else if (PyMethod_Check(object))
        {

            PyMethodObject* methodObj = (PyMethodObject*)object;
            PyObject* classObj = methodObj->im_self;
            if (PyType_Check(classObj))
            {
                PyTypeObject* obj = (PyTypeObject*)classObj;
                if (strcmp(obj->tp_name, currentImpl->name) == 0)
                {
                    obj->tp_call = (ternaryfunc)currentImpl->tp_call;
                    obj->tp_vectorcall = (vectorcallfunc)currentImpl->vec_call;
                    methodObj->vectorcall = (vectorcallfunc)currentImpl->vec_call;

                    PyObject* repr = PyObject_Repr(object);
                    PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
                    const char* bytes = PyBytes_AS_STRING(str);

                    printf("% s\n", bytes);

                    Py_XDECREF(repr);
                    Py_XDECREF(str);

                }
            }

        }
    }
}
