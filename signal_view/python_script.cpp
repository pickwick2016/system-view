#include <boost/format.hpp>

#ifdef slots
#undef slots
#endif //slots
#include <Python.h>

// pyconfig.h need comment the Macro Py_DEBUG


#include "python_script.h"

Python::Python(const std::string & dir) : m_initialized(false), m_dir(dir)
{
	Py_Initialize();
	m_initialized = Py_IsInitialized();
}

Python::~Python()
{
	if (m_initialized) {
		Py_Finalize();
	}
}

bool Python::run(const std::string & moduleName, const std::string & functionName)
{
	if (!m_initialized) {
		return false;
	}

	try {
		if (!m_dir.empty()) {
			PyRun_SimpleString("import sys");
			PyRun_SimpleString(str(boost::format("sys.path.append('%s')") % m_dir).c_str());
		}

		PyObject* module = PyImport_ImportModule(moduleName.c_str());
		if (!module) {
			return false;
		}

		PyObject* func = PyObject_GetAttrString(module, functionName.c_str());
		if (!func) {
			return false;
		}

		int ret = 0;

		PyObject * args = PyTuple_New(1);
		PyObject * params = PyDict_New();
		ret = PyDict_SetItemString(params, "name", Py_BuildValue("s", "liuyang"));
		ret = PyDict_SetItemString(params, "value", Py_BuildValue("i", 3));

		ret = PyTuple_SetItem(args, 0, params);
		PyObject * result = PyObject_CallObject(func, args);

		char * out;
		ret = PyArg_Parse(result, "s", &out);

		//PyObject * args = Py_BuildValue("(ii)", 123, 456);
		//PyObject * result = PyObject_CallObject(func, args);
		//int value = 0;
		//int ok = PyArg_Parse(result, "i", &value);
		//int ok = PyArg_ParseTuple(result, "(i)", &value);
		
		//PyObject * ret = PyObject_CallFunction(func, nullptr);
		
		//char * ss = nullptr;
		//if (!PyArg_Parse(ret, "s", &ss)) {
		//	return false;
		//}

		Py_XDECREF(params);
		Py_XDECREF(args);
		Py_XDECREF(result);
		Py_XDECREF(func);
		Py_XDECREF(module);

		return true;
	}
	catch (...) {
		return false;
	}
}