#include <Python.h>
#include <pyfunc.h>
#include <iostream>

void callPythonFunc(const char* filename, const char* funcname) {
  Py_Initialize();

  PyObject* ModuleString = PyUnicode_FromString( filename );
  PyObject* Module = PyImport_Import( ModuleString );
  if (Module == NULL) {
    std::cout << "import failed" << std::endl;
    return;
  }
  PyObject* Dict = PyModule_GetDict( Module );
  PyObject* Func = PyDict_GetItemString( Dict, funcname );
  PyObject* Result = PyObject_CallObject( Func, NULL );

  Py_Finalize();
}
