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

void bl2_hemv_blk_var2( conj_t  conjh,
                        obj_t*  alpha,
                        obj_t*  a,
                        obj_t*  x,
                        obj_t*  beta,
                        obj_t*  y,
                        hemv_t* cntl )
{
	obj_t   a11, a11_pack;
	obj_t   a10;
	obj_t   a21;
	obj_t   x1, x1_pack;
	obj_t   x0;
	obj_t   x2;
	obj_t   y1, y1_pack;

	dim_t   mn;
	dim_t   ij;
	dim_t   b_alg;

	// Even though this blocked algorithm is expressed only in terms of the
	// lower triangular case, the upper triangular case is still supported:
	// when bl2_acquire_mpart_tl2br() is passed a matrix that is stored in
	// in the upper triangle, and the requested subpartition resides in the
	// lower triangle (as is the case for this algorithm), the routine fills
	// the request as if the caller had actually requested the corresponding
	// "mirror" subpartition in the upper triangle, except that it marks the
	// subpartition for transposition (and conjugation).

	// Initialize objects for packing.
	bl2_obj_init_pack( &a11_pack );
	bl2_obj_init_pack( &x1_pack );
	bl2_obj_init_pack( &y1_pack );

	// Query dimension.
	mn = bl2_obj_length( *a );

	// y = beta * y;
	bl2_scalv_int( beta,
	               y,
	               cntl_sub_scalv( cntl ) );

	// Partition diagonally.
	for ( ij = 0; ij < mn; ij += b_alg )
	{
		// Determine the current algorithmic blocksize.
		b_alg = bl2_determine_blocksize_f( ij, mn,
		                                   a,
		                                   cntl_blocksize( cntl ) );

		// Acquire partitions for A11, A10, A21, x1, x0, x2, y1, and y0.
		bl2_acquire_mpart_tl2br( BLIS_SUBPART11,
		                         ij, b_alg, a, &a11 );
		bl2_acquire_mpart_tl2br( BLIS_SUBPART10,
		                         ij, b_alg, a, &a10 );
		bl2_acquire_mpart_tl2br( BLIS_SUBPART21,
		                         ij, b_alg, a, &a21 );
		bl2_acquire_vpart_f2b( BLIS_SUBPART1,
		                       ij, b_alg, x, &x1 );
		bl2_acquire_vpart_f2b( BLIS_SUBPART0,
		                       ij, b_alg, x, &x0 );
		bl2_acquire_vpart_f2b( BLIS_SUBPART2,
		                       ij, b_alg, x, &x2 );
		bl2_acquire_vpart_f2b( BLIS_SUBPART1,
		                       ij, b_alg, y, &y1 );

		// Initialize objects for packing A11, x1, and y1 (if needed).
		bl2_packm_init( &a11, &a11_pack,
		                cntl_sub_packm_a11( cntl ) );
		bl2_packv_init( &x1, &x1_pack,
		                cntl_sub_packv_x1( cntl ) );
		bl2_packv_init( &y1, &y1_pack,
		                cntl_sub_packv_y1( cntl ) );

		// Copy/pack A11, x1, y1 (if needed).
		bl2_packm_int( &BLIS_ONE,
		               &a11,
		               &a11_pack,
		               cntl_sub_packm_a11( cntl ) );
		bl2_packv_int( &x1,
		               &x1_pack,
		               cntl_sub_packv_x1( cntl ) );
		bl2_packv_int( &y1,
		               &y1_pack,
		               cntl_sub_packv_y1( cntl ) );

		// y1 = y1 + alpha * A10 * x0;
		bl2_gemv_int( BLIS_NO_TRANSPOSE,
		              BLIS_NO_CONJUGATE,
	                  alpha,
		              &a10,
		              &x0,
		              &BLIS_ONE,
		              &y1_pack,
		              cntl_sub_gemv_n_rp( cntl ) );

		// y1 = y1 + alpha * A11 * x1;
		bl2_hemv_int( conjh,
		              alpha,
		              &a11_pack,
		              &x1_pack,
		              &BLIS_ONE,
		              &y1_pack,
		              cntl_sub_hemv( cntl ) );

		// y1 = y1 + alpha * A21' * x2;
		bl2_gemv_int( bl2_apply_conj( conjh, BLIS_TRANSPOSE ),
		              BLIS_NO_CONJUGATE,
	                  alpha,
		              &a21,
		              &x2,
		              &BLIS_ONE,
		              &y1_pack,
		              cntl_sub_gemv_t_cp( cntl ) );

		// Copy/unpack y1 (if y1 was packed).
		bl2_unpackv_int( &y1_pack,
		                 &y1,
		                 cntl_sub_unpackv_y1( cntl ) );
	}

	// If any packing buffers were acquired within packm, release them back
	// to the memory manager.
	bl2_obj_release_pack( &a11_pack );
	bl2_obj_release_pack( &x1_pack );
	bl2_obj_release_pack( &y1_pack );
}
