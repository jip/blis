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

extern hemv_t* hemv_cntl_bs_ke_lrow_ucol;
extern hemv_t* hemv_cntl_bs_ke_lcol_urow;
extern hemv_t* hemv_cntl_ge_lrow_ucol;
extern hemv_t* hemv_cntl_ge_lcol_urow;

void bl2_symv( obj_t*  alpha,
               obj_t*  a,
               obj_t*  x,
               obj_t*  beta,
               obj_t*  y )
{
	hemv_t* hemv_cntl;
	num_t   dt_targ_a;
	num_t   dt_targ_x;
	num_t   dt_targ_y;
	bool_t  a_is_contig;
	bool_t  x_is_contig;
	bool_t  y_is_contig;
	obj_t   alpha_local;
	obj_t   beta_local;
	num_t   dt_alpha;
	num_t   dt_beta;

	// Check parameters.
	if ( bl2_error_checking_is_enabled() )
		bl2_symv_check( alpha, a, x, beta, y );


	// Query the target datatypes of each object.
	dt_targ_a = bl2_obj_target_datatype( *a );
	dt_targ_x = bl2_obj_target_datatype( *x );
	dt_targ_y = bl2_obj_target_datatype( *y );

	// Determine whether each operand is stored contiguously.
	a_is_contig = ( bl2_obj_is_row_stored( *a ) ||
	                bl2_obj_is_col_stored( *a ) );
	x_is_contig = ( bl2_obj_vector_inc( *x ) == 1 );
	y_is_contig = ( bl2_obj_vector_inc( *y ) == 1 );


	// Create an object to hold a copy-cast of alpha. Notice that we use
	// the type union of the target datatypes of a and x to prevent any
	// unnecessary loss of information during the computation.
	dt_alpha = bl2_datatype_union( dt_targ_a, dt_targ_x );
	bl2_obj_init_scalar_copy_of( dt_alpha,
	                             BLIS_NO_CONJUGATE,
	                             alpha,
	                             &alpha_local );

	// Create an object to hold a copy-cast of beta. Notice that we use
	// the datatype of y. Here's why: If y is real and beta is complex,
	// there is no reason to keep beta_local in the complex domain since
	// the complex part of beta*y will not be stored. If y is complex and
	// beta is real then beta is harmlessly promoted to complex.
	dt_beta = dt_targ_y;
	bl2_obj_init_scalar_copy_of( dt_beta,
	                             BLIS_NO_CONJUGATE,
	                             beta,
	                             &beta_local );


	// If all operands are contiguous, we choose a control tree for calling
	// the unblocked implementation directly without any blocking.
	if ( a_is_contig &&
	     x_is_contig &&
	     y_is_contig )
	{
		// We use two control trees to handle the four cases corresponding to
		// combinations of upper/lower triangular storage and row/column-storage.
		// The row-stored lower triangular and column-stored upper triangular
		// trees are identical. Same for the remaining two trees.
		if ( bl2_obj_is_lower( *a ) )
		{
			if ( bl2_obj_is_row_stored( *a ) ) hemv_cntl = hemv_cntl_bs_ke_lrow_ucol;
			else                               hemv_cntl = hemv_cntl_bs_ke_lcol_urow;
		}
		else // if ( bl2_obj_is_upper( *a ) )
		{
			if ( bl2_obj_is_row_stored( *a ) ) hemv_cntl = hemv_cntl_bs_ke_lcol_urow;
			else                               hemv_cntl = hemv_cntl_bs_ke_lrow_ucol;
		}
	}
	else
	{
		// Mark objects with unit stride as already being packed. This prevents
		// unnecessary packing from happening within the blocked algorithm.
		if ( a_is_contig ) bl2_obj_set_pack_schema( BLIS_PACKED_UNSPEC, *a );
		if ( x_is_contig ) bl2_obj_set_pack_schema( BLIS_PACKED_VECTOR, *x );
		if ( y_is_contig ) bl2_obj_set_pack_schema( BLIS_PACKED_VECTOR, *y );

		// Here, we make a similar choice as above, except that (1) we look
		// at storage tilt, and (2) we choose a tree that performs blocking.
		if ( bl2_obj_is_lower( *a ) )
		{
			if ( bl2_obj_is_row_tilted( *a ) ) hemv_cntl = hemv_cntl_ge_lrow_ucol;
			else                               hemv_cntl = hemv_cntl_ge_lcol_urow;
		}
		else // if ( bl2_obj_is_upper( *a ) )
		{
			if ( bl2_obj_is_row_tilted( *a ) ) hemv_cntl = hemv_cntl_ge_lcol_urow;
			else                               hemv_cntl = hemv_cntl_ge_lrow_ucol;
		}
	}


	// Invoke the internal back-end with the copy-casts of scalars and the
	// chosen control tree. Set conjh to BLIS_NO_CONJUGATE to invoke the
	// symmetric (and not Hermitian) algorithms.
	bl2_hemv_int( BLIS_NO_CONJUGATE,
	              &alpha_local,
	              a,
	              x,
	              &beta_local,
	              y,
	              hemv_cntl );
}


//
// Define BLAS-like interfaces with homogeneous-typed operands.
//
#undef  GENTFUNC
#define GENTFUNC( ctype, ch, opname, varname ) \
\
void PASTEMAC(ch,opname)( \
                          uplo_t  uploa, \
                          conj_t  conja, \
                          conj_t  conjx, \
                          dim_t   m, \
                          ctype*  alpha, \
                          ctype*  a, inc_t rs_a, inc_t cs_a, \
                          ctype*  x, inc_t incx, \
                          ctype*  beta, \
                          ctype*  y, inc_t incy  \
                        ) \
{ \
	const num_t dt = PASTEMAC(ch,type); \
\
	obj_t       alphao, ao, xo, betao, yo; \
\
	inc_t       rs_x, cs_x; \
	inc_t       rs_y, cs_y; \
\
	rs_x = incx; cs_x = m * incx; \
	rs_y = incy; cs_y = m * incy; \
\
	bl2_obj_create_scalar_with_attached_buffer( dt, alpha, &alphao ); \
	bl2_obj_create_scalar_with_attached_buffer( dt, beta,  &betao  ); \
\
	bl2_obj_create_with_attached_buffer( dt, m, m, a, rs_a, cs_a, &ao ); \
	bl2_obj_create_with_attached_buffer( dt, m, 1, x, rs_x, cs_x, &xo ); \
	bl2_obj_create_with_attached_buffer( dt, m, 1, y, rs_y, cs_y, &yo ); \
\
	bl2_obj_set_uplo( uploa, ao ); \
	bl2_obj_set_conj( conja, ao ); \
	bl2_obj_set_conj( conjx, xo ); \
\
	PASTEMAC0(opname)( &alphao, \
	                   &ao, \
	                   &xo, \
	                   &betao, \
	                   &yo ); \
}

INSERT_GENTFUNC_BASIC( symv, symv )


//
// Define BLAS-like interfaces with heterogeneous-typed operands.
//
#undef  GENTFUNC3U12
#define GENTFUNC3U12( ctype_a, ctype_x, ctype_y, ctype_ax, cha, chx, chy, chax, opname, varname ) \
\
void PASTEMAC3(cha,chx,chy,opname)( \
                                    uplo_t    uploa, \
                                    conj_t    conja, \
                                    conj_t    conjx, \
                                    dim_t     m, \
                                    ctype_ax* alpha, \
                                    ctype_a*  a, inc_t rs_a, inc_t cs_a, \
                                    ctype_x*  x, inc_t incx, \
                                    ctype_y*  beta, \
                                    ctype_y*  y, inc_t incy  \
                                  ) \
{ \
	bl2_check_error_code( BLIS_NOT_YET_IMPLEMENTED ); \
}

INSERT_GENTFUNC3U12_BASIC( symv, symv )

#ifdef BLIS_ENABLE_MIXED_DOMAIN_SUPPORT
INSERT_GENTFUNC3U12_MIX_D( symv, symv )
#endif

#ifdef BLIS_ENABLE_MIXED_PRECISION_SUPPORT
INSERT_GENTFUNC3U12_MIX_P( symv, symv )
#endif