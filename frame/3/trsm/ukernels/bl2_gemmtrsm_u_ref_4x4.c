/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2013, The University of Texas

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas nor the names of its
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

#include "blis2.h"


#undef  GENTFUNC
#define GENTFUNC( ctype, ch, varname, gemmukr, trsmukr ) \
\
void PASTEMAC(ch,varname)( \
                           dim_t   k, \
                           ctype*  alpha, \
                           ctype*  aR, \
                           ctype*  a, \
                           ctype*  bdB, \
                           ctype*  bd, \
                           ctype*  b, \
                           ctype*  c, inc_t rs_c, inc_t cs_c \
                         ) \
{ \
	ctype*      minus_one = PASTEMAC(ch,m1); \
\
	const inc_t rs_b      = 4; \
	const inc_t cs_b      = 1; \
\
	PASTEMAC(ch,gemmukr)( k, \
	                      minus_one, \
	                      aR, \
	                      bdB, \
	                      alpha, \
	                      b, rs_b, cs_b ); \
\
	PASTEMAC(ch,trsmukr)( a, \
	                      b, \
	                      bd, \
	                      c, rs_c, cs_c ); \
}

INSERT_GENTFUNC_BASIC2( gemmtrsm_u_ref_4x4, gemm_ref_4x4, trsm_u_ref_4x4 )
