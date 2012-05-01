#ifndef F2C_INCLUDE
#include "f2c.h"
#endif

#ifndef __FORTRAN_IO__
#define __FORTRAN_IO__

#define FORTRAN_NAME(func) func ## _

#define STATUS_EXISTS 0
#define STATUS_NEW 1
#define STATUS_SCRATCH 2
#define STATUS_REPLACE 3
#define STATUS_UNKNOWN 4
#define STATUS_APPEND 5

extern void FORTRAN_NAME(fopenwrap)
(integer *info, const integer *fd,
 const char *fname, const integer *status,
 const ftnlen fname_len);

extern void FORTRAN_NAME(fclosewrap)
(const integer *fd);

#endif
