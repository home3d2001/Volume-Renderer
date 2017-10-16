# SBVR
Slice Based Volume Renderer

Report
	* How I solved the assignment:
		* Most of this project was solved by following the GPU Gems article on slice based volume rendering. I implemented the camera controls myself, and recycled an old OpenGL/OpenCL project to speed up development.
	* What resources I used:
		* C++, Visual Studio 2017, CMake, OpenGL, OpenCL, GLFW, GLM, GLEW. 
	* Known bugs/TODO
		* Can't currently change out datasets without editing in code text. Need to read from command line args.
		* Can't alter the transfer function from within the application. The transfer function is a texture which must be edited externally.
		* OpenCL/OpenGL should be used to compute the proxy geometry.
		* I'd like to add a clipping plane, the ability to translate and rotate the volume, and the ability to translate the camera.
	* Program controls
		* Click and drag or arrow keys control camera.
		* +/- control camera dolly
		* yuiop control how many samples to take
		* qaws show camera direction, edge points, proxy geometry, and sampled volume respectively.
		* r resets the selected camera
		* 0 shows the tracked camera perspective
		* 1 shows the untracked camera perspective

