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

void bl2_gemv_blk_var1( obj_t*  alpha,
                        obj_t*  a,
                        obj_t*  x,
                        obj_t*  beta,
                        obj_t*  y,
                        gemv_t* cntl )
{
	obj_t a1, a1_pack;
	obj_t y1, y1_pack;

	dim_t m_trans;
	dim_t i;
	dim_t b_alg;

	// Initialize objects for packing.
	bl2_obj_init_pack( &a1_pack );
	bl2_obj_init_pack( &y1_pack );

	// Query dimension in partitioning direction.
	m_trans = bl2_obj_length_after_trans( *a );

	// Partition along the m dimension.
	for ( i = 0; i < m_trans; i += b_alg )
	{
		// Determine the current algorithmic blocksize.
		b_alg = bl2_determine_blocksize_f( i, m_trans,
		                                   a,
		                                   cntl_blocksize( cntl ) );

		// Acquire partitions for A1 and y1.
		bl2_acquire_mpart_t2b( BLIS_SUBPART1,
		                       i, b_alg, a, &a1 );
		bl2_acquire_vpart_f2b( BLIS_SUBPART1,
		                       i, b_alg, y, &y1 );

		// Initialize objects for packing A1 and y1 (if needed).
		bl2_packm_init( &a1, &a1_pack,
		                cntl_sub_packm_a( cntl ) );
		bl2_packv_init( &y1, &y1_pack,
		                cntl_sub_packv_y( cntl ) );

		// Copy/pack A1, y1 (if needed).
		bl2_packm_int( &BLIS_ONE,
		               &a1,
		               &a1_pack,
		               cntl_sub_packm_a( cntl ) );
		bl2_packv_int( &y1,
		               &y1_pack,
		               cntl_sub_packv_y( cntl ) );

		// y1 = beta * y1 + alpha * A1 * x;
		bl2_gemv_int( BLIS_NO_TRANSPOSE,
		              BLIS_NO_CONJUGATE,
		              alpha,
		              &a1_pack,
		              x,
		              beta,
		              &y1_pack,
		              cntl_sub_gemv( cntl ) );

		// Copy/unpack y1 (if y1 was packed).
		bl2_unpackv_int( &y1_pack,
		                 &y1,
		                 cntl_sub_unpackv_y( cntl ) );
	}

	// If any packing buffers were acquired within packm, release them back
	// to the memory manager.
	bl2_obj_release_pack( &a1_pack );
	bl2_obj_release_pack( &y1_pack );
}
