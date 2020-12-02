#include <Python.h>
#include <pyfunc.h>
#include <iostream>

void callPythonFunc(const std::string& filename, const std::string& funcname) {
  Py_Initialize();

  PyObject* ModuleString = PyUnicode_FromString( filename.c_str() );
  PyObject* Module = PyImport_Import( ModuleString );
  if (Module == NULL) {
    std::cout << "import failed" << std::endl;
    return;
  }
  PyObject* Dict = PyModule_GetDict( Module );
  PyObject* Func = PyDict_GetItemString( Dict, funcname.c_str() );
  PyObject* Result = PyObject_CallObject( Func, NULL );

  Py_Finalize();
}
