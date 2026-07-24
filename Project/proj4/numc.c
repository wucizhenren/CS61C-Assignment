#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
  matrix *new_mat;
  int alloc_failed = allocate_matrix(&new_mat, rows, cols);
  if (alloc_failed)
    return alloc_failed;
  rand_matrix(new_mat, seed, low, high);
  ((Matrix61c *)self)->mat = new_mat;
  ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
  return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
  matrix *new_mat;
  int alloc_failed = allocate_matrix(&new_mat, rows, cols);
  if (alloc_failed)
    return alloc_failed;
  else {
    fill_matrix(new_mat, val);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
  }
  return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with
 * 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
  if (rows * cols != PyList_Size(lst)) {
    PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
    return -1;
  }
  matrix *new_mat;
  int alloc_failed = allocate_matrix(&new_mat, rows, cols);
  if (alloc_failed)
    return alloc_failed;
  int count = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
      count++;
    }
  }
  ((Matrix61c *)self)->mat = new_mat;
  ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
  return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
  int rows = PyList_Size(lst);
  if (rows == 0) {
    PyErr_SetString(PyExc_ValueError,
                    "Cannot initialize numc.Matrix with an empty list");
    return -1;
  }
  int cols;
  if (!PyList_Check(PyList_GetItem(lst, 0))) {
    PyErr_SetString(PyExc_ValueError, "List values not valid");
    return -1;
  } else {
    cols = PyList_Size(PyList_GetItem(lst, 0));
  }
  for (int i = 0; i < rows; i++) {
    if (!PyList_Check(PyList_GetItem(lst, i)) ||
        PyList_Size(PyList_GetItem(lst, i)) != cols) {
      PyErr_SetString(PyExc_ValueError, "List values not valid");
      return -1;
    }
  }
  matrix *new_mat;
  int alloc_failed = allocate_matrix(&new_mat, rows, cols);
  if (alloc_failed)
    return alloc_failed;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      set(new_mat, i, j,
          PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
    }
  }
  ((Matrix61c *)self)->mat = new_mat;
  ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
  return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
  deallocate_matrix(self->mat);
  Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
  Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
  return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success
 * otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
  /* Generate random matrices */
  if (kwds != NULL) {
    PyObject *rand = PyDict_GetItemString(kwds, "rand");
    if (!rand) {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }
    if (!PyBool_Check(rand)) {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }
    if (rand != Py_True) {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }

    PyObject *low = PyDict_GetItemString(kwds, "low");
    PyObject *high = PyDict_GetItemString(kwds, "high");
    PyObject *seed = PyDict_GetItemString(kwds, "seed");
    double double_low = 0;
    double double_high = 1;
    unsigned int unsigned_seed = 0;

    if (low) {
      if (PyFloat_Check(low)) {
        double_low = PyFloat_AsDouble(low);
      } else if (PyLong_Check(low)) {
        double_low = PyLong_AsLong(low);
      }
    }

    if (high) {
      if (PyFloat_Check(high)) {
        double_high = PyFloat_AsDouble(high);
      } else if (PyLong_Check(high)) {
        double_high = PyLong_AsLong(high);
      }
    }

    if (double_low >= double_high) {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }

    // Set seed if argument exists
    if (seed) {
      if (PyLong_Check(seed)) {
        unsigned_seed = PyLong_AsUnsignedLong(seed);
      }
    }

    PyObject *rows = NULL;
    PyObject *cols = NULL;
    if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
      if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
        return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols),
                         unsigned_seed, double_low, double_high);
      }
    } else {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }
  }
  PyObject *arg1 = NULL;
  PyObject *arg2 = NULL;
  PyObject *arg3 = NULL;
  if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
    /* arguments are (rows, cols, val) */
    if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) &&
        (PyLong_Check(arg3) || PyFloat_Check(arg3))) {
      if (PyLong_Check(arg3)) {
        return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2),
                         PyLong_AsLong(arg3));
      } else
        return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2),
                         PyFloat_AsDouble(arg3));
    } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) &&
               PyLong_Check(arg2) && PyList_Check(arg3)) {
      /* Matrix(rows, cols, 1D list) */
      return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
    } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
      /* Matrix(rows, cols, 1D list) */
      return init_2d(self, arg1);
    } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) &&
               arg3 == NULL) {
      /* Matrix(rows, cols, 1D list) */
      return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
    } else {
      PyErr_SetString(PyExc_TypeError, "Invalid arguments");
      return -1;
    }
  } else {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
    return -1;
  }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
  int rows = self->mat->rows;
  int cols = self->mat->cols;
  PyObject *py_lst = NULL;
  if (self->mat->is_1d) { // If 1D matrix, print as a single list
    py_lst = PyList_New(rows * cols);
    int count = 0;
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
        count++;
      }
    }
  } else { // if 2D, print as nested list
    py_lst = PyList_New(rows);
    for (int i = 0; i < rows; i++) {
      PyList_SetItem(py_lst, i, PyList_New(cols));
      PyObject *curr_row = PyList_GetItem(py_lst, i);
      for (int j = 0; j < cols; j++) {
        PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
      }
    }
  }
  return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
  PyObject *mat = NULL;
  if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
    if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
      PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
      return NULL;
    }
    Matrix61c *mat61c = (Matrix61c *)mat;
    return Matrix61c_to_list(mat61c);
  } else {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
    return NULL;
  }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS,
     "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
  PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
  return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first
 * operand is self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c *self, PyObject *args) {
  /* TODO: YOUR CODE HERE */
  Matrix61c *other = (Matrix61c *)args;
  if (!PyObject_TypeCheck(args, &Matrix61cType)) {
    PyErr_SetString(PyExc_TypeError, "TypeError");
  }
  if (self->mat->rows != other->mat->rows ||
      self->mat->cols != other->mat->cols) {
    PyErr_SetString(PyExc_ValueError, "ValueError ");
  }

  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  add_matrix(rv->mat, self->mat, other->mat);
  return (PyObject *)rv;
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The
 * first operand is self, and the second operand can be obtained by casting
 * `args`.
 */
PyObject *Matrix61c_sub(Matrix61c *self, PyObject *args) {
  /* TODO: YOUR CODE HERE */
  Matrix61c *other = (Matrix61c *)args;
  if (!PyObject_TypeCheck(args, &Matrix61cType)) {
    PyErr_SetString(PyExc_TypeError, "TypeError");
  }
  if (self->mat->rows != other->mat->rows ||
      self->mat->cols != other->mat->cols) {
    PyErr_SetString(PyExc_ValueError, "ValueError ");
  }

  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  sub_matrix(rv->mat, self->mat, other->mat);
  return (PyObject *)rv;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second
 * operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c *self, PyObject *args) {
  /* TODO: YOUR CODE HERE */
  Matrix61c *other = (Matrix61c *)args;
  if (!PyObject_TypeCheck(args, &Matrix61cType)) {
    PyErr_SetString(PyExc_TypeError, "TypeError");
  }
  if (self->mat->rows != other->mat->rows ||
      self->mat->cols != other->mat->cols) {
    PyErr_SetString(PyExc_ValueError, "ValueError ");
  }

  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  mul_matrix(rv->mat, self->mat, other->mat);
  return (PyObject *)rv;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c *self) {
  /* TODO: YOUR CODE HERE */
  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  neg_matrix(rv->mat, self->mat);
  return (PyObject *)rv;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
  /* TODO: YOUR CODE HERE */
  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  abs_matrix(rv->mat, self->mat);
  return (PyObject *)rv;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the
 * argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
  /* TODO: YOUR CODE HERE */
  long other = PyLong_AsLong(optional);
  if (!PyLong_Check(optional)) {
    PyErr_SetString(PyExc_TypeError, "TypeError");
  }
  if ((self->mat->rows != self->mat->cols) || other < 0) {
    PyErr_SetString(PyExc_ValueError, "ValueError ");
  }

  Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
  matrix *new_mat;
  allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
  rv->mat = new_mat;
  rv->shape = self->shape;
  pow_matrix(rv->mat, self->mat, (int)other);
  return (PyObject *)rv;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number
 * methods you have define. You might find this link helpful:
 * https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    .nb_add = (binaryfunc)Matrix61c_add,
    .nb_subtract = (binaryfunc)Matrix61c_sub,
    .nb_multiply = (binaryfunc)Matrix61c_multiply,
    .nb_negative = (unaryfunc)Matrix61c_neg,
    .nb_absolute = (unaryfunc)Matrix61c_abs,
    .nb_power = (ternaryfunc)Matrix61c_pow,
};

/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and
 * (double/int) val. Return None in Python (this is different from returning
 * null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject *args) {
  /* TODO: YOUR CODE HERE */
  Py_ssize_t n = PyTuple_GET_SIZE(args);
  if (n != 4) {
    goto TypeError;
  }

  PyObject *a = PyTuple_GetItem(args, 0);
  PyObject *b = PyTuple_GetItem(args, 1);
  PyObject *c = PyTuple_GetItem(args, 2);
  if (!(PyLong_Check(a)) || !(PyLong_Check(b)) ||
      !(PyLong_Check(c) || PyFloat_Check(c))) {
  TypeError:
    PyErr_SetString(PyExc_TypeError, "TypeError");
    return 0;
  }
  long aValOFi = PyLong_AsLong(a);
  long bValOFj = PyLong_AsLong(b);
  double Val = PyFloat_AsDouble(c);
  if (self->mat->rows < aValOFi || self->mat->cols < bValOFj) {
    PyErr_SetString(PyExc_IndexError, "IndexError ");
    return 0;
  }
  set(self->mat, aValOFi, bValOFj, Val);
  return Py_None;
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject *args) {
  /* TODO: YOUR CODE HERE */
  /* TODO: YOUR CODE HERE */
  Py_ssize_t n = PyTuple_GET_SIZE(args);
  if (n != 2)
    goto TypeError;
  PyObject *a = PyTuple_GetItem(args, 0);
  PyObject *b = PyTuple_GetItem(args, 1);
  if (!(PyLong_Check(a)) || !(PyLong_Check(b))) {
  TypeError:
    PyErr_SetString(PyExc_TypeError, "TypeError");
    return 0;
  }
  long aValOFi = PyLong_AsLong(a);
  long bValOFj = PyLong_AsLong(b);
  if (self->mat->rows < aValOFi || self->mat->cols < bValOFj) {
    PyErr_SetString(PyExc_IndexError, "IndexError ");
    return 0;
  }
  long val = get(self->mat, aValOFi, bValOFj);
  return (PyObject *)val;
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and
 * Matrix61c_set_value as "set" You might find this link helpful:
 * https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "set matrix value"},
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "get matrix value"},
    {NULL, NULL, 0, NULL}};

/* INDEXING */

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed
 * result.
 */
PyObject *Matrix61c_subscript(Matrix61c *self, PyObject *key) {
  /* TODO: YOUR CODE HERE */
  // 1维数组和二维数组的参数判断
  // 1维数组
  if (self->mat->is_1d) {
    // 整数,需要返回一个数字
    if (PyLong_Check(key)) {
      long col = PyLong_AsLong(key);
      Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
      matrix *new_mat;
      allocate_matrix_ref(&new_mat, self->mat, 0, col, 1, 1);
      rv->mat = new_mat;
      rv->shape = get_shape(1, 1);
      return (PyObject *)rv;
    }
    // slide，需要返回一个数组
    else if (PySlice_Check(key)) {
      // PyObject *row = PyTuple_GetItem(key, 0);
      Py_ssize_t start, stop, step;
      if (PySlice_GetIndices(key, (self->mat->cols) * (self->mat->rows), &start,
                             &stop, &step) < 0) {
        return NULL;
      }

      // value error
      if (step != 1 || stop - start < 1) {
        PyErr_SetString(PyExc_ValueError, "ValueError");
        return 0;
      }

      // 返回数组
      // 默认一维数组给的和返回的都是一行，而不是一列的东西
      Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
      matrix *new_mat;
      allocate_matrix_ref(&new_mat, self->mat, 0, start, 1, stop - start);
      rv->mat = new_mat;
      rv->shape = get_shape(1, stop - start);
      return (PyObject *)rv;
    }
    // TypeError
    else {
      PyErr_SetString(PyExc_TypeError, "TypeError");
      return 0;
    }

  }
  // 二维数组
  else {
    // 整数,返回一个数组，第key行的数组
    if (PyLong_Check(key)) {
      long des = PyLong_AsLong(key);

      Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
      matrix *new_mat;
      allocate_matrix_ref(&new_mat, self->mat, (int)des, 0, 1, self->mat->cols);
      rv->mat = new_mat;
      rv->shape = get_shape(1, self->mat->cols);
      return (PyObject *)rv;

    }
    // slide,返回从start行到stop行的矩阵
    else if (PySlice_Check(key)) {
      // PyObject *row = PyTuple_GetItem(key, 0);
      Py_ssize_t start, stop, step;
      if (PySlice_GetIndices(key, (self->mat->cols) * (self->mat->rows), &start,
                             &stop, &step) < 0) {
        return NULL;
      }

      // value error
      if (step != 1 || stop - start < 1) {
        PyErr_SetString(PyExc_ValueError, "ValueError");
        return 0;
      }

      // 返回矩阵
      Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
      matrix *new_mat;
      allocate_matrix_ref(&new_mat, self->mat, start, 0, stop - start,
                          self->mat->cols);
      rv->mat = new_mat;
      rv->shape = get_shape(stop - start, self->mat->cols);
      return (PyObject *)rv;
    }
    // tuple
    else if (PyTuple_Check(key)) {
      PyObject *row = PyTuple_GetItem(key, 0);
      PyObject *col = PyTuple_GetItem(key, 1);

      // 如果row是slice
      Py_ssize_t start_r, stop_r, step_r;
      if (PySlice_Check(row)) {
        if (PySlice_GetIndices(row, self->mat->rows, &start_r, &stop_r,
                               &step_r) < 0) {
          return NULL;
        }

        // value error
        if (step_r != 1 || stop_r - start_r < 1) {
          PyErr_SetString(PyExc_ValueError, "ValueError");
          return 0;
        }
      } else {
        start_r = PyLong_AsSsize_t(row);
        stop_r = start_r + 1;
        step_r = 1;
      }

      // 如果cols是slice
      Py_ssize_t start_v, stop_v, step_v;
      if (PySlice_Check(col)) {
        if (PySlice_GetIndices(col, self->mat->cols, &start_v, &stop_v,
                               &step_v) < 0) {
          return NULL;
        }

        // value error
        if (step_v != 1 || stop_v - start_v < 1) {
          PyErr_SetString(PyExc_ValueError, "ValueError");
          return 0;
        }
      } else if (PyLong_Check(col)) {
        start_v = PyLong_AsSsize_t(col);
        stop_v = start_v + 1;
        step_v = 1;
      } else {
        PyErr_SetString(PyExc_TypeError, "TypeError");
        return 0;
      }

      // 返回矩阵
      Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
      matrix *new_mat;
      allocate_matrix_ref(&new_mat, self->mat, start_r, start_v,
                          stop_r - start_r, stop_v - start_v);
      rv->mat = new_mat;
      rv->shape = get_shape(stop_r - start_r, stop_v - start_v);
      return (PyObject *)rv;
    }
    // TypeError
    else {
      PyErr_SetString(PyExc_TypeError, "TypeError");
      return 0;
    }
  }
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed
 * result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c *self, PyObject *key, PyObject *v) {
  /* TODO: YOUR CODE HERE */
  // 得到子矩阵
  PyObject *subscript = Matrix61c_subscript(self, key);
  Matrix61c *subMatrix = (Matrix61c *)subscript;
  Py_ssize_t tupleLen = PyTuple_Size(subMatrix->shape);
  PyObject *row_item = PyTuple_GetItem(subMatrix->shape, 0);
  Py_ssize_t rows = PyLong_AsSsize_t(row_item);
  Py_ssize_t cols;
  if (tupleLen == 2) {
    PyObject *col_item = PyTuple_GetItem(subMatrix->shape, 1);
    cols = PyLong_AsSsize_t(col_item);
  } else {
    cols = 1;
  }

  // 子矩阵是一个值
  if (rows == 1 && cols == 1) {
    // v不是一个值,typeError
    if (!PyLong_Check(v) && !PyFloat_Check(v)) {
      PyErr_SetString(PyExc_TypeError, "one value condition,TypeError");
      return -1;
    }

    double val = PyFloat_AsDouble(v);
    set(subMatrix->mat, 0, 0, val);

  }
  // 子矩阵是一个一维的矩阵,并且不是仅仅只有一个值
  else if (subMatrix->mat->is_1d && !(rows == 1 && cols == 1)) {
    // v不是一个list,typeError
    if (!PyList_Check(v)) {
      PyErr_SetString(PyExc_TypeError, "1d condition TypeError");
      return -1;
    }

    // v的长度不符合或者v里面的值不是float或者int，valueError
    Py_ssize_t len = PyList_Size(v);
    if (rows * cols != len) {
      PyErr_SetString(PyExc_ValueError,
                      "1d condition error!the length of v is not compact");
      return -1;
    }
    for (Py_ssize_t i = 0; i < len; i++) {

      PyObject *item = PyList_GetItem(v, i);

      if (!PyLong_Check(item) && !PyFloat_Check(item)) {
        PyErr_SetString(PyExc_ValueError,
                        "1d condition error!list element must be int or float");
        return -1;
      }
    }

    // 修改子矩阵
    for (Py_ssize_t i = 0; i < len; i++) {
      PyObject *item = PyList_GetItem(v, i);
      double val = PyFloat_AsDouble(item);
      if (rows == 1) {
        set(subMatrix->mat, 0, i, val);
      } else {
        set(subMatrix->mat, i, 0, val);
      }
    }

  }
  // 子矩阵是一个二维的矩阵
  else {
    // v不是一个list,typeError
    if (!PyList_Check(v)) {
      PyErr_SetString(PyExc_TypeError, "TypeError");
      return -1;
    }

    // v的长度不符合或者v里面的值不是float或者int，valueError
    Py_ssize_t len = PyList_Size(v);
    if (rows * cols != len) {
      PyErr_SetString(PyExc_ValueError,
                      "2d condition error!the length of v is not compact");
      return -1;
    }
    for (Py_ssize_t i = 0; i < len; i++) {
      for (Py_ssize_t j = 0; j < len; j++) {
        PyObject *row = PyList_GetItem(v, i);
        PyObject *item = PyList_GetItem(row, j);

        if (!PyLong_Check(item) && !PyFloat_Check(item)) {
          PyErr_SetString(
              PyExc_ValueError,
              "2d condition error!list element must be int or float");
          return -1;
        }
      }
    }

    // 修改子矩阵
    for (Py_ssize_t i = 0; i < len; i++) {
      for (Py_ssize_t j = 0; j < len; j++) {
        PyObject *row = PyList_GetItem(v, i);
        PyObject *item = PyList_GetItem(row, j);
        double val = PyFloat_AsDouble(item);
        set(subMatrix->mat, i, j, val);
      }
    }
  }
  deallocate_matrix(subMatrix->mat);
  return 0;
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc)Matrix61c_subscript,
    (objobjargproc)Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {"shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0, "(rows, cols)"},
    {NULL} /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new};

struct PyModuleDef numcmodule = {PyModuleDef_HEAD_INIT, "numc",
                                 "Numc matrix operations", -1,
                                 Matrix61c_class_methods};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
  PyObject *m;

  if (PyType_Ready(&Matrix61cType) < 0)
    return NULL;

  m = PyModule_Create(&numcmodule);
  if (m == NULL)
    return NULL;

  Py_INCREF(&Matrix61cType);
  PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
  printf("CS61C Fall 2020 Project 4: numc imported!\n");
  fflush(stdout);
  return m;
}