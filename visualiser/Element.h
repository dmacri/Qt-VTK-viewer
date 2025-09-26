#pragma once

#ifdef __CUDACC__
  #define OPENCALF __host__ __device__ 
#else	
  #define OPENCALF inline
#endif

// TODO: GB: OOpenCal has the class, can we remove its from here?
// class Element
// {
// 	public:
// 		virtual void composeElement(char* str) = 0;
		
// 		virtual char* stringEncoding() = 0;
		
//         virtual Color* outputValue() = 0;

// 		virtual void startStep(int step) = 0;
// };
