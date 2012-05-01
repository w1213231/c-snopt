#ifndef __FORTRAN_IO__
#include "fortran-io.h"
#endif

#ifndef __SNOPT__
#include "snopt.h"
#endif

#ifndef __C_SNOPT__
#define __C_SNOPT__

#define _PRINT_FD_ 20
#define _SUMM_FD_ 21
#define _SPEC_FD_ 22

#define CSNOPT_SUCCESS 0
#define CSNOPT_MEM_ERROR -1
#define CSNOPT_FILE_ERROR -2
#define CSNOPT_SNMEM_ERROR -40
#define CSNOPT_SNJAC_ERROR -41

typedef void (*snopt_objfunc)(integer kopt, doublereal *f, doublereal *dfdx,
			      const doublereal *x, void *f_data);

typedef struct{
  /*-----------------------------------------------------------------------
    Problem description
    -----------------------------------------------------------------------*/
  /*Dimension of {variable, function}*/  
  integer n, nf;
  /*Problem Name*/
  char *probname;
  /*Print, Summary streams*/
  char *print_stream, *summ_stream, *spec_stream;
  /*Objective function, and function data*/
  snopt_objfunc objfunc;
  void *objdata;
  /*Objective row; zero based index (unlike Fortran)*/
  integer objrow;
  /*Approximate number of non-zero entries of the jacobian
    if n_Df < 0, len_Df <- (n * nf)
    else len_Df <- n_Df + 1 */
  integer n_Df;
  /*Approximate number of non-zero entries of the linear constraint
    if n_A < 0, len_A <- (n * nf)
    else len_A <- n_A + 1 */
  integer n_A;
  /*-----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------*/
  /*Bounds for {function, variable}*/
  doublereal
    *xlow, *xup,
    *flow, *fup;
  /*-----------------------------------------------------------------------*/
  integer *Df_idx_i, *Df_idx_j, ne_Df, len_Df;
  /*-----------------------------------------------------------------------*/
  doublereal *A;
  integer *A_idx_i, *A_idx_j, ne_A, len_A;
  /*-----------------------------------------------------------------------
    Variables holding the initial guess, solution.
    -----------------------------------------------------------------------*/
  doublereal
    /*
      On entry
      --------
      Initial guess for the variable, na,
      
      On exit
      -------
      Final value of the variable, KKT multiplier 
      corresponding to the variable bounds
    */
    *x, *xmul,
    /*
      On entry
      --------
      Initial guess for the {function, multiplier}.
      On exit
      ------
      Final value of the {function, multiplier}.
      corresponding to the variable bound
    */
    *f, *fmul;
  
  /*-----------------------------------------------------------------------
    Variables having to do with the actual snoptA call
    Do NOT tweak this part unless you know what you're doing.
    (And by that you are familiar with SNOPT's Fortran calling convention,
    and with the source of C-SNOPT).
    -----------------------------------------------------------------------*/
  /*Real workspace*/
  doublereal *snopt_rw;
  integer snopt_lenrw;

  /*Integer workspace*/
  integer *snopt_iw;
  integer snopt_leniw;

  /*Character workspace*/
  char *snopt_cw;
  integer snopt_lencw;

  /*File handles for Printing and Summary*/  
  integer snopt_print_fd, snopt_summ_fd, snopt_spec_fd;
  
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/
  
  /*Variable, function "state", see SNOPT documentation,
    stuff involving Warm starts*/
  integer *xstate, *fstate;
  integer nS;

  /*Number of infeasibilities*/
  integer nInf;
  /*Sum of infeasibilities*/
  doublereal sInf;
} snopt_problem;

snopt_problem* snopt_problem_create
(int n, int nf,
 char *probname,
 char *print_stream, char *summ_stream,
 snopt_objfunc objfunc, void *objdata, int objrow,
 int n_Df, int n_A);

int snopt_problem_init(snopt_problem *prob);
int snopt_problem_specfile(char *spec_stream, snopt_problem *prob);
int snopt_problem_jacobian(snopt_problem *prob);

void snopt_problem_close(snopt_problem *prob);

int snopt_cold_optimise(snopt_problem *prob);
/*-----------------------------------------------------------------------*/
int snopt_set_xlow(double *y, snopt_problem *prob);
int snopt_set_xup(double *y, snopt_problem *prob);

int snopt_set_xlow1(double y, snopt_problem *prob);
int snopt_set_xup1(double y, snopt_problem *prob);

int snopt_set_flow(double *y, snopt_problem *prob);
int snopt_set_fup(double *y, snopt_problem *prob);

int snopt_set_flow1(double y, snopt_problem *prob);
int snopt_set_fup1(double y, snopt_problem *prob);

int snopt_set_Df_idx(int ne_Df, int *Df_idx, snopt_problem *prob);
#endif

