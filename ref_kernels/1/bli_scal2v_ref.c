/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name(s) of the copyright holder(s) nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

#undef  GENTFUNC
#define GENTFUNC( ctype, ch, opname, arch, suf ) \
\
void PASTEMAC(ch,opname,arch,suf) \
     ( \
             conj_t  conjx, \
             dim_t   n, \
       const void*   alpha0, \
       const void*   x0, inc_t incx, \
             void*   y0, inc_t incy, \
       const cntx_t* cntx  \
     ) \
{ \
	if ( bli_zero_dim1( n ) ) return; \
\
	const ctype* alpha = alpha0; \
	const ctype* x     = x0; \
	      ctype* y     = y0; \
\
	if ( bli_teq0s( ch, *alpha ) ) \
	{ \
		/* If alpha is zero, use setv. */ \
\
		const ctype* zero = PASTEMAC(ch,0); \
\
		/* Query the context for the kernel function pointer. */ \
		const num_t dt     = PASTEMAC(ch,type); \
		setv_ker_ft setv_p = bli_cntx_get_ukr_dt( dt, BLIS_SETV_KER, cntx ); \
\
		setv_p \
		( \
		  BLIS_NO_CONJUGATE, \
		  n, \
		  zero, \
		  y0, incy, \
		  cntx  \
		); \
		return; \
	} \
	else if ( bli_teq1s( ch, *alpha ) ) \
	{ \
		/* If alpha is one, use copyv. */ \
\
		/* Query the context for the kernel function pointer. */ \
		const num_t  dt      = PASTEMAC(ch,type); \
		copyv_ker_ft copyv_p = bli_cntx_get_ukr_dt( dt, BLIS_COPYV_KER, cntx ); \
\
		copyv_p \
		( \
		  conjx, \
		  n, \
		  x0, incx, \
		  y0, incy, \
		  cntx  \
		); \
		return; \
	} \
\
	if ( bli_is_conj( conjx ) ) \
	{ \
		if ( incx == 1 && incy == 1 ) \
		{ \
			PRAGMA_SIMD \
			for ( dim_t i = 0; i < n; ++i ) \
			{ \
				bli_tscal2js( ch,ch,ch,ch, *alpha, x[i], y[i] ); \
			} \
		} \
		else \
		{ \
			for ( dim_t i = 0; i < n; ++i ) \
			{ \
				bli_tscal2js( ch,ch,ch,ch, *alpha, *x, *y ); \
\
				x += incx; \
				y += incy; \
			} \
		} \
	} \
	else \
	{ \
		if ( incx == 1 && incy == 1 ) \
		{ \
			PRAGMA_SIMD \
			for ( dim_t i = 0; i < n; ++i ) \
			{ \
				bli_tscal2s( ch,ch,ch,ch, *alpha, x[i], y[i] ); \
			} \
		} \
		else \
		{ \
			for ( dim_t i = 0; i < n; ++i ) \
			{ \
				bli_tscal2s( ch,ch,ch,ch, *alpha, *x, *y ); \
\
				x += incx; \
				y += incy; \
			} \
		} \
	} \
}

INSERT_GENTFUNC_BASIC( scal2v, BLIS_CNAME_INFIX, BLIS_REF_SUFFIX )

