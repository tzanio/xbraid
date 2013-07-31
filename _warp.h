/*BHEADER**********************************************************************
 * Copyright (c) 2013,  Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * This file is part of WARP.  See file COPYRIGHT for details.
 *
 * WARP is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (as published by the Free
 * Software Foundation) version 2.1 dated February 1999.
 *
 ***********************************************************************EHEADER*/

#ifndef _warp_HEADER
#define _warp_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "warp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * Main data structures and accessor macros
 *--------------------------------------------------------------------------*/

typedef struct
{
   warp_Int     request_type; /* recv type = 1 */
   warp_Int     num_requests;
   MPI_Request *requests;
   MPI_Status  *status;
   void        *buffer;
   warp_Vector *vector_ptr;
   
} _warp_CommHandle;

typedef struct
{
   warp_Int     level;
   warp_Int     ilower, iupper;
   warp_Int     clower, cupper, cfactor, ncpoints;

   warp_Vector *ua;  /* unknown vectors            (C-points only)*/
   warp_Float  *ta;  /* time values                (all points) */
   warp_Vector *va;  /* restricted unknown vectors (all points, NULL on level 0) */
   warp_Vector *wa;  /* rhs vectors f-v            (all points, NULL on level 0) */

   warp_Int          recv_index; /* -1 means no receive */
   warp_Int          send_index; /* -1 means no send */
   _warp_CommHandle *recv_handle;
   _warp_CommHandle *send_handle;

   /* pointers to the original memory allocation for ua, ta, va, and wa */
   warp_Vector *ua_alloc;
   warp_Float  *ta_alloc;
   warp_Vector *va_alloc;
   warp_Vector *wa_alloc;

} _warp_Grid;

typedef struct _warp_Core_struct
{
   MPI_Comm             comm;
   warp_Float           tstart;    /* start time */
   warp_Float           tstop;     /* start time */
   warp_Int             ntime;     /* initial number of time intervals */
   warp_App             app;       /* application data for the user */

   warp_PtFcnPhi        phi;       /* apply phi function */
   warp_PtFcnInit       init;      /* return an initial solution vector */
   warp_PtFcnClone      clone;     /* clone a vector */
   warp_PtFcnFree       free;      /* free up a vector */
   warp_PtFcnSum        sum;       /* vector sum */
   warp_PtFcnDot        dot;       /* dot product */
   warp_PtFcnWrite      write;     /* write the vector */
   warp_PtFcnBufSize    bufsize;   /* return buffer size */
   warp_PtFcnBufPack    bufpack;   /* pack a buffer */
   warp_PtFcnBufUnpack  bufunpack; /* unpack a buffer */
   warp_PtFcnCoarsen    coarsen;   /* (optional) return a coarsened vector */
   warp_PtFcnRefine     refine;    /* (optional) return a refined vector */

   warp_Int             max_levels;
   warp_Int             nrelax;    /* number of pre-relaxations */
   warp_Float           tol;       /* stopping tolerance */
   warp_Int            *cfactors;  /* coarsening factors */
   warp_Int             cfdefault; /* default coarsening factor*/
   warp_Int             max_iter;
   warp_Int             niter;     /* number of iterations */
   warp_Float           rnorm;     /* residual norm */

   warp_Int             gupper;    /* global upper index on the fine grid */

   warp_Int             nlevels;
   _warp_Grid         **grids;

} _warp_Core;

/* Accessor macros */

#define _warp_CommHandleElt(handle, elt)  ((handle) -> elt)

#define _warp_GridElt(grid, elt)  ((grid) -> elt)

#define _warp_CoreElt(core, elt)  (  (core) -> elt )
#define _warp_CoreFcn(core, fcn)  (*((core) -> fcn))

/*--------------------------------------------------------------------------
 * Memory allocation macros
 *--------------------------------------------------------------------------*/

#define _warp_TAlloc(type, count) \
( (type *)malloc((size_t)(sizeof(type) * (count))) )

#define _warp_CTAlloc(type, count) \
( (type *)calloc((size_t)(count), (size_t)sizeof(type)) )

#define _warp_TReAlloc(ptr, type, count) \
( (type *)realloc((char *)ptr, (size_t)(sizeof(type) * (count))) )

#define _warp_TFree(ptr) \
( free((char *)ptr), ptr = NULL )

/*--------------------------------------------------------------------------
 * Error handling
 *--------------------------------------------------------------------------*/

extern warp_Int _warp_error_flag;

/*--------------------------------------------------------------------------
 * Coarsening macros
 *--------------------------------------------------------------------------*/

#define _warp_MapFineToCoarse(findex, cfactor, cindex) \
( cindex = (findex)/(cfactor) )
#define _warp_MapCoarseToFine(cindex, cfactor, findex) \
( findex = (cindex)*(cfactor) )
#define _warp_IsFPoint(index, cfactor) \
( (index)%(cfactor) )
#define _warp_IsCPoint(index, cfactor) \
( !_warp_IsFPoint(index, cfactor) )

/*--------------------------------------------------------------------------
 * Prototypes
 *--------------------------------------------------------------------------*/

/* util.c */
warp_Int
_warp_ProjectInterval( warp_Int   ilower,
                       warp_Int   iupper,
                       warp_Int   index,
                       warp_Int   stride,
                       warp_Int  *pilower,
                       warp_Int  *piupper );

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
