//#define SACRIFICE_BITS 16
//#define SB1 (SACRIFICE_BITS - 1)
//#define b_FastCmp_to_IntConst(a,b) (((a) >> ((b) & ~SB1)) & (1 << ((b) & SB1)))
//
//#define k 32
//#define d 4
//#define koverd k / d
///* numBins is 2^d */
//#define numBins 16
//
///* Algorithm 3 in Fast radix Sort for sparse linear algebra on GPU */
///* assumes size is POT */
//
///* Update: the register specifier is not supported by opencl... 
//https://www.khronos.org/registry/OpenCL/sdk/2.1/docs/man/xhtml/storageSpecifiers.html
//
//	Nvidia compiles, but that behavior isn't a guarantee on other platforms ...
//*/
////inline void threadPrivateHistogram(
////	cl_int* input, // the data to sort
////	cl_int n_begin, // the begining index of the segment
////	cl_int n_end, // the ending index of the segment
////	cl_int li, //local thread id
////	cl_int ls, //local size
////	register unsigned *ha, register unsigned *hb, register unsigned *hc, register unsigned *hd, //histogram registers
////	register unsigned *he, register unsigned *hf, register unsigned *hg, register unsigned *hh,
////	register unsigned *hi, register unsigned *hj, register unsigned *hk, register unsigned *hl,
////	register unsigned *hm, register unsigned *hn, register unsigned *ho, register unsigned *hp
////) {
////	/* Using private registers, since local private array can spill to global memory */
////	*ha = *hb = *hc = *hd = *he = *hf = *hg = *hh = *hi = *hj = *hk = *hl = *hm = *hn = *ho = *hp = 0;
////
////	/* Segments are a multiple of 2 * ls long. Each thread bins two elements at a time. */
////	for (unsigned int i = n_begin + li * 2; i < n_end; i+= ls * 2) 
////	{
////		cl_int key = input[i];
////		cl_int key2 = input[i + 1];
////
////		/* if the two keys are equal, we shift ((bin >> X) & 1) by 1 to the left.
////		   this results in adding both elements to the same bin, since ((bin >> X) & 1) << 1 would equal 2. */
////		cl_int multiplicity = (key == key2);
////
////		key = (1 << key) | (1 << key2);
////		
////		/* Incrementing all bins leads to better thread convergence per multiprocessor */
////		*ha = *ha + ((key >> 0) & 1) << multiplicity;
////		*hb = *hb + ((key >> 1) & 1) << multiplicity;
////		*hc = *hc + ((key >> 2) & 1) << multiplicity;
////		*hd = *hd + ((key >> 3) & 1) << multiplicity;
////		*he = *he + ((key >> 4) & 1) << multiplicity;
////		*hf = *hf + ((key >> 5) & 1) << multiplicity;
////		*hg = *hg + ((key >> 6) & 1) << multiplicity;
////		*hh = *hh + ((key >> 7) & 1) << multiplicity;
////		*hi = *hi + ((key >> 8) & 1) << multiplicity;
////		*hj = *hj + ((key >> 9) & 1) << multiplicity;
////		*hk = *hk + ((key >> 10) & 1) << multiplicity;
////		*hl = *hl + ((key >> 11) & 1) << multiplicity;
////		*hm = *hm + ((key >> 12) & 1) << multiplicity;
////		*hn = *hn + ((key >> 13) & 1) << multiplicity;
////		*ho = *ho + ((key >> 14) & 1) << multiplicity;
////		*hp = *hp + ((key >> 15) & 1) << multiplicity;
////	}
////}
//#undef SACRIFICE_BITS
//#undef SB1
//#undef b_FastCmp_to_IntConst(a,b)
//#undef k
//#undef d
//#undef koverd
//#undef numBins
