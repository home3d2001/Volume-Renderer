#ifndef DIM_H
#define DIM_H

//#define OCT2D
#ifdef OpenCL
	#define DIM 2
#else
	#ifdef OCT2D
	#define DIM 2
	#else
	#define DIM 3
	#endif // OCT2D
#endif

#define DIM2D 2
#define DIM3D 3

#endif
