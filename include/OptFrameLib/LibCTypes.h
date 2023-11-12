#pragma once

typedef void* FakeFEvaluatorPtr;
typedef void* FakeFConstructivePtr;
typedef void* FakeFNSPtr;
typedef void* FakePythonObjPtr;
typedef void* FakeEnginePtr;

// ============= compatible api components =============

extern "C" struct LibSearchOutput {
  int status;     // ("status", c_int),
  bool has_best;  // ("has_best", ctypes.c_bool),
  void* best_s;   // ("best_s", ctypes.py_object),
  double best_e;  // ("best_e", ctypes.c_double)]
};

extern "C" struct LibArrayDouble {
  int size;   // ("size", c_int),
  double* v;  // ("v", ctypes.c_double_p)]
};
