#include <Python.h>
#include "pythread.h"
#include "hook.h"
#include "frameobject.h"

PyObject * hooked_PyEval_EvalFrameEx(PyFrameObject *f, int throwflag);

static PyObject *
sys_setrwatch(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"targets", 0};
    PyObject *targets = NULL;
	PyObject *dict;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:setrwatch",
                                     kwlist, &targets))
		return NULL;

    if (!targets)
		targets = Py_None;
	Py_INCREF(targets);

	dict = PyThreadState_GetDict();
	PyDict_SetItemString(dict, "watchtargets", targets);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(setrwatch_doc,
"setrwatch(targets)\n\
\n\
Set read-watches for the specified objects.\n\
The targets should be a mapping of object ids to functions."
);

static PyObject *
sys_getrwatch(PyObject *self, PyObject *args)
{
    PyObject *dict = PyThreadState_GetDict();
    PyObject *targets = PyDict_GetItemString(dict, "watchtargets");

    if (targets == NULL)
        targets = Py_None;

	Py_INCREF(targets);
    return targets;
}

PyDoc_STRVAR(getrwatch_doc,
"getrwatch()\n\
\n\
Return the targets set with sys.setrwatch."
);

PyDoc_STRVAR(module_doc,
"This module implements a \"read watches\" in Python for debugging.");

static PyMethodDef module_methods[] = {
    {"setrwatch",       (PyCFunction)sys_setrwatch,
 	 METH_VARARGS | METH_KEYWORDS, setrwatch_doc},
    {"getrwatch",       sys_getrwatch,  METH_NOARGS, getrwatch_doc},
	{NULL} /* Sentinel */
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "rwatch",
    module_doc,
    -1,
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_rwatch(void) {
	__asm__("");

    PyObject *m = PyModule_Create(&moduledef);
    if (!m) goto finally;

	if(hook_function(PyEval_EvalFrameEx, hooked_PyEval_EvalFrameEx))
		fprintf(stderr, "Function hooking failed.\n");

	PyObject* sys_str = PyUnicode_FromString("sys");
	PyObject* sys_mod = PyImport_Import(sys_str);
	if(0 != PyModule_AddFunctions(sys_mod, module_methods))
		fprintf(stderr, "Couldn't add methods to sys module.\n");

	/* PyThreadState_GetDict is the right approach, but I would like to note
 	 *   that it would have been much more fun to have had:
 	 * typedef struct {
 	 *		PyThreadState ts;
 	 *		PyObject *c_watchtargets;
 	 *	} PyThreadState2;
 	 *
	 * PyInterpreterState *is = PyThreadState_GET()->interp;
	 * PyThreadState *head = PyInterpreterState_ThreadHead(is);
	 * PyThreadState *ts;
	 * for (ts = head; ts; ts = PyThreadState_Next(ts)) {
	 *   // (:
	 * }
	 */

	Py_DECREF(sys_str);
	Py_DECREF(sys_mod);

finally:
    return m;
}
