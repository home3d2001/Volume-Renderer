#include "clfw.hpp"
inline cl_int multiplyM4V2_p(
	cl::Buffer &input,
	cl::Buffer &result,
	cl_int numV4,
	cl_int offset,
	glm::mat4 matrix
) {
	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["multiplyM4V2Kernel"];

	cl_float16 temp = {
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	};

	error |= kernel.setArg(0, input);
	error |= kernel.setArg(1, temp);
	error |= kernel.setArg(2, result);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NDRange(offset), cl::NDRange(numV4), cl::NullRange);
	return error;
}

inline cl_int multiplyM4V4_p(
	cl::Buffer &VBuffer,
	cl::Buffer &result,
	cl_int numV4,
	cl_int offset,
	glm::mat4 matrix
) {
	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["multiplyM4V4Kernel"];

	cl_float16 temp = {
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	};

	error |= kernel.setArg(0, VBuffer);
	error |= kernel.setArg(1, temp);
	error |= kernel.setArg(2, result);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NDRange(offset), cl::NDRange(numV4), cl::NullRange);
	return error;
}

inline void checkError(int error, int id) {
	//if (error == CLK_ENQUEUE_FAILURE) 
	//	printf("gid %d got CLK_ENQUEUE_FAILURE\n", id);
	//if (error == CLK_INVALID_QUEUE) 
	//	printf("gid %d got CLK_INVALID_QUEUE\n", id);
	//if (error == CLK_INVALID_NDRANGE) 
	//	printf("gid %d got CLK_INVALID_NDRANGE\n", id);
	//if (error == CLK_INVALID_EVENT_WAIT_LIST) 
	//	printf("gid %d got CLK_INVALID_EVENT_WAIT_LIST\n", id);
	//if (error == CLK_DEVICE_QUEUE_FULL)
	//	printf("gid %d got CLK_DEVICE_QUEUE_FULLn", id);
	//if (error == CLK_INVALID_ARG_SIZE) 
	//	printf("gid %d got CLK_INVALID_ARG_SIZE\n", id);
	//if (error == CLK_EVENT_ALLOCATION_FAILURE) 
	//	printf("gid %d got CLK_EVENT_ALLOCATION_FAILURE\n", id);
	//if (error == CLK_OUT_OF_RESOURCES) 
	//	printf("gid %d got CLK_OUT_OF_RESOURCES\n", id);
}

//__kernel void `Parallelsim_internal(
//	queue_t queue,
//	cl_int location,
//	cl_int recursionLevel
//	)
//{
//	if (location == 0)
//		printf("level %d\n", recursionLevel);
//	if (recursionLevel < 512) {
//		ndrange_t ndrange = ndrange_1D(1);
//		int result = enqueue_kernel(
//			queue,
//			CLK_ENQUEUE_FLAGS_NO_WAIT,
//			ndrange,
//			^{ DynamicParallelsim_internal(queue, location, recursionLevel + 1); });
//		//checkError(result, location);
//	}
//	else {
//		printf("id %d recursed successfully\n", location);
//	}
//}
//__kernel void DynamicParallelismTest (
//	queue_t queue
//	) 
//{
//	int gid = get_global_id(0);

//	ndrange_t ndrange = ndrange_1D(1);

//	int result = enqueue_kernel(
//		queue,
//		CLK_ENQUEUE_FLAGS_NO_WAIT,
//		ndrange,
//		^{ DynamicParallelsim_internal(queue, gid, 0); });
//	//checkError(result, gid);
//}
/* Dynamic Parallelism Test*/
//inline cl_int DynamicParallelsim() 
//{
//	cl_int error = 0;
//	cl::CommandQueue &queue = CLFW::DefaultQueue;
//	cl::Kernel &kernel = CLFW::Kernels["DynamicParallelismTest"];
//	cl::Buffer test;
//	cl_int n = 1;
//	CLFW::getBuffer(test, "test", sizeof(cl_int) * n);

//	error |= kernel.setArg(0, CLFW::DeviceQueue);
//	queue.finish();

//	auto start = std::chrono::high_resolution_clock::now();
//	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(n), cl::NullRange);
//	queue.finish();
//	auto elapsed = high_resolution_clock::now() - start;
//	cout<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<endl;
//	
//	vector<cl_int> result;
//	error |= CLFW::Download<cl_int>(test, n, result);

//	for (int i = 0; i < n; ++i) {
//		cout << result[i] << endl;
//	}
//	return error;
//}