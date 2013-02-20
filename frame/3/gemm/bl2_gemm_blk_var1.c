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

void bl2_gemm_blk_var1( obj_t*  alpha,
                        obj_t*  a,
                        obj_t*  b,
                        obj_t*  beta,
                        obj_t*  c,
                        gemm_t* cntl )
{
	obj_t a1, a1_pack;
	obj_t b_pack;
	obj_t c1, c1_pack;

	dim_t i;
	dim_t b_alg;
	dim_t m_trans;

	// Initialize all pack objects that are passed into packm_init().
	bl2_obj_init_pack( &a1_pack );
	bl2_obj_init_pack( &b_pack );
	bl2_obj_init_pack( &c1_pack );

	// Query dimension in partitioning direction.
	m_trans = bl2_obj_length_after_trans( *a );

	// Scale C by beta (if instructed).
	bl2_scalm_int( beta,
	               c,
	               cntl_sub_scalm( cntl ) );

	// Initialize object for packing B.
	bl2_packm_init( b, &b_pack,
	                cntl_sub_packm_b( cntl ) );

	// Pack B and scale by alpha (if instructed).
	bl2_packm_int( alpha,
	               b, &b_pack,
	               cntl_sub_packm_b( cntl ) );

	// Partition along the m dimension.
	for ( i = 0; i < m_trans; i += b_alg )
	{
		// Determine the current algorithmic blocksize.
		// NOTE: Use of a (for execution datatype) is intentional!
		// This causes the right blocksize to be used if c and a are
		// complex and b is real.
		b_alg = bl2_determine_blocksize_f( i, m_trans, a,
		                                   cntl_blocksize( cntl ) );

		// Acquire partitions for A1 and C1.
		bl2_acquire_mpart_t2b( BLIS_SUBPART1,
		                       i, b_alg, a, &a1 );
		bl2_acquire_mpart_t2b( BLIS_SUBPART1,
		                       i, b_alg, c, &c1 );

		// Initialize objects for packing A1 and C1.
		bl2_packm_init( &a1, &a1_pack,
		                cntl_sub_packm_a( cntl ) );
		bl2_packm_init( &c1, &c1_pack,
		                cntl_sub_packm_c( cntl ) );

		// Pack A1 and scale by alpha (if instructed).
		bl2_packm_int( alpha,
		               &a1, &a1_pack,
		               cntl_sub_packm_a( cntl ) );

		// Pack C1 and scale by beta (if instructed).
		bl2_packm_int( beta,
		               &c1, &c1_pack,
		               cntl_sub_packm_c( cntl ) );

		// Perform gemm subproblem.
		bl2_gemm_int( alpha,
		              &a1_pack,
		              &b_pack,
		              beta,
		              &c1_pack,
		              cntl_sub_gemm( cntl ) );

		// Unpack C1 (if C1 was packed).
		bl2_unpackm_int( &c1_pack, &c1,
		                 cntl_sub_unpackm_c( cntl ) );
	}

	// If any packing buffers were acquired within packm, release them back
	// to the memory manager.
	bl2_obj_release_pack( &a1_pack );
	bl2_obj_release_pack( &b_pack );
	bl2_obj_release_pack( &c1_pack );
}
