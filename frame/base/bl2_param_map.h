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

// --- BLIS to BLAS/LAPACK mappings --------------------------------------------

void bl2_param_map_blis_to_netlib_side( side_t side, char* blas_side );
void bl2_param_map_blis_to_netlib_uplo( uplo_t uplo, char* blas_uplo );
void bl2_param_map_blis_to_netlib_trans( trans_t trans, char* blas_trans );
void bl2_param_map_blis_to_netlib_diag( diag_t diag, char* blas_diag );
void bl2_param_map_blis_to_netlib_machval( machval_t machval, char* blas_machval );


// --- BLAS/LAPACK to BLIS mappings --------------------------------------------

void bl2_param_map_netlib_to_blis_side( char side, side_t* blis_side );
void bl2_param_map_netlib_to_blis_uplo( char uplo, uplo_t* blis_uplo );
void bl2_param_map_netlib_to_blis_trans( char trans, trans_t* blis_trans );
void bl2_param_map_netlib_to_blis_diag( char diag, diag_t* blis_diag );


// --- BLIS char to BLIS mappings ----------------------------------------------

void bl2_param_map_char_to_blis_side( char side, side_t* blis_side );
void bl2_param_map_char_to_blis_uplo( char uplo, uplo_t* blis_uplo );
void bl2_param_map_char_to_blis_trans( char trans, trans_t* blis_trans );
void bl2_param_map_char_to_blis_conj( char conj, conj_t* blis_conj );
void bl2_param_map_char_to_blis_diag( char diag, diag_t* blis_diag );


// --- BLIS to BLIS char mappings ----------------------------------------------

void bl2_param_map_blis_to_char_side( side_t blis_side, char* side );
void bl2_param_map_blis_to_char_uplo( uplo_t blis_uplo, char* uplo );
void bl2_param_map_blis_to_char_trans( trans_t blis_trans, char* trans );
void bl2_param_map_blis_to_char_conj( trans_t blis_conj, char* conj );
void bl2_param_map_blis_to_char_diag( diag_t blis_diag, char* diag );
