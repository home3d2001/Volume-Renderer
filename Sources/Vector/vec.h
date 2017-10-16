/*******************************************************
 ** Slice Based Volume Renderer                       **
 ** Copyright (c) 2017 Nathan V Morrical              **
 **                                                   **
 ** For information about this project contact        **
 ** Nathan Morical at                                 **
 **    natemorrical@gmail.com                         **
 ** or visit                                          **
 **    https://n8vm.github.io/SBVR/                   **
 *******************************************************/

#ifndef VEC_H
#define VEC_H

#ifdef OpenCL
#include "./vec_cl.h"
#elif defined __cplusplus
#include "./vec_cpp.h"
#else
  #error Only C++/OpenCL can compile vec.h!
#endif

#endif
