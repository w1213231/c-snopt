/* Josh Griffin ... modeled after npsol.h written by:          */
/* Mike Gertz - 2-Aug-98                                       */
/* Function prototypes for functions in the snopt distribution */

/* Modified by Akshay Srinivasan for C-SNOPT*/

#ifndef __FORTRAN_IO__
#include "fortran-io.h"
#endif

#ifndef __SNOPT__
#define __SNOPT__

extern void FORTRAN_NAME(snopta)
( integer *start, integer *nef, integer *n,
  integer *nxname, integer *nfname, doublereal *objadd, integer *objrow,
  char *prob, U_fp usrfun, integer *iafun, integer *javar,
  integer *lena, integer *nea, doublereal *a, integer *igfun,
  integer *jgvar, integer *leng, integer *neg,
  doublereal *xlow, doublereal *xupp,
  char *xnames, doublereal *flow, doublereal *fupp, char *fnames,
  doublereal *x, integer *xstate, doublereal *xmul, doublereal *f,
  integer *fstate, doublereal *fmul, integer *inform, integer *mincw,
  integer *miniw, integer *minrw, integer *ns, integer *ninf,
  doublereal *sinf, char *cu, integer *lencu, integer *iu, integer *leniu,
  doublereal *ru, integer *lenru, char *cw, integer *lencw,
  integer *iw, integer *leniw, doublereal *rw, integer *lenrw, ftnlen
  prob_len, ftnlen xnames_len, ftnlen fnames_len, ftnlen cu_len, ftnlen cw_len );

extern void FORTRAN_NAME(sninit)
( integer *iPrint, integer *iSumm, char *cw,
  integer *lencw, integer *iw, integer *leniw,
  doublereal *rw, integer *lenrw, ftnlen cw_len );

extern int FORTRAN_NAME(sngeti)
( char *buffer, integer *ivalue, integer *inform,
  char *cw, integer *lencw, integer *iw,
  integer *leniw, doublereal *rw, integer *lenrw,
  ftnlen buffer_len, ftnlen cw_len);

extern int FORTRAN_NAME(snset)
( char *buffer, integer *iprint, integer *isumm,
  integer *inform, char *cw, integer *lencw,
  integer *iw, integer *leniw,
  doublereal *rw, integer *lenrw,
  ftnlen buffer_len, ftnlen cw_len);

extern int FORTRAN_NAME(snseti)
( char *buffer, integer *ivalue, integer *iprint,
  integer *isumm, integer *inform, char *cw,
  integer *lencw, integer *iw, integer *leniw,
  doublereal *rw, integer *lenrw, ftnlen buffer_len,
  ftnlen cw_len );

extern int FORTRAN_NAME(snsetr)
( char *buffer, doublereal *rvalue, integer * iprint,
  integer *isumm, integer *inform, char *cw,
  integer *lencw, integer *iw, integer *leniw,
  doublereal *rw, integer *lenrw, ftnlen buffer_len,
  ftnlen cw_len );

extern void FORTRAN_NAME(snspec)
( integer *ispecs, integer *inform, char *cw,
  integer *lencw, integer *iw, integer *leniw,
  doublereal *rw, integer *lenrw, ftnlen cw_len);

extern void FORTRAN_NAME(snmema)
( integer *iExit, integer *nef, integer *n, integer *nxname,
  integer *nfname, integer *nea, integer *neg,
  integer *mincw, integer *miniw, integer *minrw,
  char *cw, integer *lencw, integer *iw,
  integer *leniw, doublereal *rw, integer *lenrw,
  ftnlen cw_len);

extern int FORTRAN_NAME(snjac)
  ( integer *iExit, //integer *iprint, integer *isumm,
    integer *nef, integer *n, U_fp userfg,
    integer *iafun, integer *javar, integer *lena, integer *nea, doublereal *a,
    integer *igfun, integer *jgvar, integer *leng, integer *neg,
    doublereal *x, doublereal *xlow, doublereal *xupp,
    integer *mincw, integer *miniw, integer *minrw,
    char *cu, integer *lencu, integer *iu, integer *leniu, doublereal *ru, integer *lenru,
    char *cw, integer *lencw, integer *iw, integer *leniw, doublereal *rw, integer *lenrw,
    ftnlen cu_len, ftnlen cw_len);

#endif
