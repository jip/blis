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

void bl2_her2k_basic_check( obj_t*   alpha,
                            obj_t*   a,
                            obj_t*   bh,
                            obj_t*   alpha_conj,
                            obj_t*   b,
                            obj_t*   ah,
                            obj_t*   beta,
                            obj_t*   c )
{
	err_t e_val;

	// Check object datatypes.

	e_val = bl2_check_noninteger_object( alpha );
	bl2_check_error_code( e_val );

	e_val = bl2_check_noninteger_object( beta );
	bl2_check_error_code( e_val );

	e_val = bl2_check_floating_object( a );
	bl2_check_error_code( e_val );

	e_val = bl2_check_floating_object( bh );
	bl2_check_error_code( e_val );

	e_val = bl2_check_noninteger_object( alpha_conj );
	bl2_check_error_code( e_val );

	e_val = bl2_check_floating_object( b );
	bl2_check_error_code( e_val );

	e_val = bl2_check_floating_object( ah );
	bl2_check_error_code( e_val );

	e_val = bl2_check_floating_object( c );
	bl2_check_error_code( e_val );

	// Check object dimensions.

	e_val = bl2_check_scalar_object( alpha );
	bl2_check_error_code( e_val );

	e_val = bl2_check_scalar_object( beta );
	bl2_check_error_code( e_val );

	e_val = bl2_check_level3_dims( a, bh, c );
	bl2_check_error_code( e_val );

	e_val = bl2_check_scalar_object( alpha_conj );
	bl2_check_error_code( e_val );

	e_val = bl2_check_level3_dims( b, ah, c );
	bl2_check_error_code( e_val );

	// Check matrix structure.

	e_val = bl2_check_general_object( a );
	bl2_check_error_code( e_val );

	e_val = bl2_check_general_object( bh );
	bl2_check_error_code( e_val );

	e_val = bl2_check_general_object( b );
	bl2_check_error_code( e_val );

	e_val = bl2_check_general_object( ah );
	bl2_check_error_code( e_val );
}

void bl2_her2k_check( obj_t*   alpha,
                      obj_t*   a,
                      obj_t*   b,
                      obj_t*   beta,
                      obj_t*   c )
{
	err_t e_val;
	obj_t ah, bh;

	// Alias A and B to A^H and B^H so we can perform dimension checks.
	bl2_obj_alias_with_trans( BLIS_CONJ_TRANSPOSE, *a, ah );
	bl2_obj_alias_with_trans( BLIS_CONJ_TRANSPOSE, *b, bh );

	// Check basic properties of the operation.

	bl2_her2k_basic_check( alpha, a, &bh, alpha, b, &ah, beta, c );

	// Check for real-valued beta.

	e_val = bl2_check_real_valued_object( beta );
	bl2_check_error_code( e_val );

	// Check matrix squareness.

	e_val = bl2_check_square_object( c );
	bl2_check_error_code( e_val );

	// Check matrix structure.

	e_val = bl2_check_hermitian_object( c );
	bl2_check_error_code( e_val );
}

void bl2_her2k_int_check( obj_t*   alpha,
                          obj_t*   a,
                          obj_t*   bh,
                          obj_t*   alpha_conj,
                          obj_t*   b,
                          obj_t*   ah,
                          obj_t*   beta,
                          obj_t*   c,
                          her2k_t* cntl )
{
	err_t e_val;

	// Check basic properties of the operation.

	bl2_her2k_basic_check( alpha, a, bh, alpha_conj, b, ah, beta, c );

	// Check control tree pointer

	e_val = bl2_check_valid_cntl( ( void* )cntl );
	bl2_check_error_code( e_val );
}
