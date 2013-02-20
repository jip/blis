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

#include <unistd.h>
#include "blis2.h"

//           transa m     n     alpha    a        lda   x        incx  beta     y        incy
void dgemv_( char*, int*, int*, double*, double*, int*, double*, int*, double*, double*, int* );

int main( int argc, char** argv )
{
	obj_t a, x, y;
	obj_t a_tl, x_t, y_t;
	obj_t y_save;
	obj_t alpha, beta;
	dim_t m, n;
	dim_t m_tl, n_tl;
	dim_t p;
	dim_t p_begin, p_end, p_inc;
	int   m_input, n_input;
	num_t dt_a, dt_x, dt_y;
	num_t dt_alpha, dt_beta;
	int   r, n_repeats;

	double dtime;
	double dtime_save;
	double gflops;

	bl2_init();

	n_repeats = 3;

#ifndef PRINT
	p_begin = 40;
	p_end   = 2000;
	p_inc   = 40;

	m_input = -1;
	n_input = -1;
#else
	p_begin = 16;
	p_end   = 16;
	p_inc   = 1;

	m_input = 15;
	n_input = 15;
#endif

	dt_a = BLIS_DOUBLE;
	dt_x = BLIS_DOUBLE;
	dt_y = BLIS_DOUBLE;
	dt_alpha = BLIS_DOUBLE;
	dt_beta = BLIS_DOUBLE;

	for ( p = p_begin; p <= p_end; p += p_inc )
	{

		if ( m_input < 0 ) m = p * ( dim_t )abs(m_input);
		else               m =     ( dim_t )    m_input;
		if ( n_input < 0 ) n = p * ( dim_t )abs(n_input);
		else               n =     ( dim_t )    n_input;


		bl2_obj_create( dt_alpha, 1, 1, 0, 0, &alpha );
		bl2_obj_create( dt_beta,  1, 1, 0, 0, &beta );

		bl2_obj_create( dt_a, m, n, 0, 0, &a );
		bl2_obj_create( dt_x, n, 1, 0, 0, &x );
		bl2_obj_create( dt_y, m, 1, 0, 0, &y );
		bl2_obj_create( dt_y, m, 1, 0, 0, &y_save );

		bl2_randm( &a );
		bl2_randm( &x );
		bl2_randm( &y );


		bl2_setsc(  (2.0/1.0), 0.0, &alpha );
		bl2_setsc( -(1.0/1.0), 0.0, &beta );

#if 0
		m_tl = 200;
		n_tl = 200;

		m_tl = bl2_min( m_tl, bl2_obj_length( a ) );
		n_tl = bl2_min( n_tl, bl2_obj_width( a ) );

		bl2_acquire_mpart_tl2br( BLIS_SUBPART11, 0, m_tl, &a, &a_tl );
		bl2_acquire_mpart_t2b( BLIS_SUBPART1, 0, n_tl, &x, &x_t );
		bl2_acquire_mpart_t2b( BLIS_SUBPART1, 0, m_tl, &y, &y_t );
#else
		m_tl = m;
		n_tl = n;

		a_tl = a;
		x_t  = x;
		y_t  = y;
#endif


		bl2_copym( &y, &y_save );
	
		dtime_save = 1.0e9;

		for ( r = 0; r < n_repeats; ++r )
		{
			bl2_copym( &y_save, &y );


			dtime = bl2_clock();

#ifdef PRINT
			bl2_printm( "a", &a, "%4.1f", "" );
			bl2_printm( "x", &x, "%4.1f", "" );
			bl2_printm( "y", &y, "%4.1f", "" );
#endif

#ifdef BLIS
			bl2_obj_set_trans( BLIS_TRANSPOSE, a_tl );

			bl2_gemv( &alpha,
			          &a_tl,
			          &x_t,
			          &beta,
			          &y_t );

#else

			char    transa = 'T';
			int     mm     = bl2_obj_length( a_tl );
			int     nn     = bl2_obj_width( a_tl );
			int     lda    = bl2_obj_col_stride( a_tl );
			int     incx   = bl2_obj_vector_inc( x_t );
			int     incy   = bl2_obj_vector_inc( y_t );
			double* alphap = bl2_obj_buffer( alpha );
			double* ap     = bl2_obj_buffer( a_tl );
			double* xp     = bl2_obj_buffer( x_t );
			double* betap  = bl2_obj_buffer( beta );
			double* yp     = bl2_obj_buffer( y_t );

			dgemv_( &transa,
			        &mm,
			        &nn,
			        alphap,
			        ap, &lda,
			        xp, &incx,
			        betap,
			        yp, &incy );
#endif

#ifdef PRINT
			bl2_printm( "y after", &y, "%4.1f", "" );
			exit(1);
#endif


			dtime_save = bl2_clock_min_diff( dtime_save, dtime );
		}

		//gflops = ( 2.0 * m * n ) / ( dtime_save * 1.0e9 );
		gflops = ( 2.0 * m_tl * n_tl ) / ( dtime_save * 1.0e9 );

#ifdef BLIS
		printf( "data_gemv_blis" );
#else
		printf( "data_gemv_%s", BLAS );
#endif
		printf( "( %2ld, 1:4 ) = [ %4lu %4lu  %10.3e  %6.3f ];\n",
		        (p - p_begin + 1)/p_inc + 1, m, n, dtime_save, gflops );

		bl2_obj_free( &alpha );
		bl2_obj_free( &beta );

		bl2_obj_free( &a );
		bl2_obj_free( &x );
		bl2_obj_free( &y );
		bl2_obj_free( &y_save );
	}

	bl2_finalize();

	return 0;
}
