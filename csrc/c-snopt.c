#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "snopt.h"
#include "c-snopt.h"

/*#define _SNOPT_DEBUG_*/

static int snopt_problem_workspace_update
(int lencw, int leniw, int lenrw,
 snopt_problem *prob)
{
  if(lenrw > prob->snopt_lenrw){
    free(prob->snopt_rw);
    prob->snopt_rw = calloc(lenrw, sizeof(doublereal));
    if(prob->snopt_rw == NULL)
      goto mem_error;
    prob->snopt_lenrw = lenrw;
  }

  if(leniw > prob->snopt_leniw){
    free(prob->snopt_iw);
    prob->snopt_iw = calloc(leniw, sizeof(integer));
    if(prob->snopt_iw == NULL)
      goto mem_error;
    prob->snopt_leniw = leniw;
  }
  
  if(lencw > prob->snopt_lencw){
    free(prob->snopt_cw);
    prob->snopt_cw = calloc(lencw * 8, sizeof(char));
    if(prob->snopt_cw == NULL)
      goto mem_error;
    prob->snopt_lencw = lencw;
  }

  return CSNOPT_SUCCESS;

mem_error:
  fprintf(stderr, "snopt_problem_workspace_update: Cannot allocate memory.\n");
  return CSNOPT_MEM_ERROR;
}

static int snopt_problem_mem_update
(snopt_problem *prob)
{  
  integer info, mincw, miniw, minrw;
  integer nxname = 1, nfname = 1;
  FORTRAN_NAME(snmema)(&info, &(prob->nf), &(prob->n),
		       &nxname, &nfname,
		       &(prob->ne_A), &(prob->ne_Df),
		       &mincw, &miniw, &minrw,
		       prob->snopt_cw, &(prob->snopt_lencw),
		       prob->snopt_iw, &(prob->snopt_leniw),
		       prob->snopt_rw, &(prob->snopt_lenrw),
		       8 * prob->snopt_lencw);
  if(info == 81)
    goto snmema_error;

  if(snopt_problem_workspace_update(mincw, miniw, minrw, prob) == -1)
    goto mem_error;
  
  return CSNOPT_SUCCESS;

mem_error:
  fprintf(stderr, "snopt_problem_mem_update: Cannot allocate memory.");
  return CSNOPT_MEM_ERROR;
snmema_error:
  fprintf(stderr, "snopt_problem_mem_update: snmema returned 81\n");
  return CSNOPT_SNMEM_ERROR;
}

static inline int snopt_problem_sninit
(integer print_fd, integer summ_fd, snopt_problem *prob)
{
  FORTRAN_NAME(sninit)
    (&print_fd, &summ_fd,
     prob->snopt_cw, &(prob->snopt_lencw),
     prob->snopt_iw, &(prob->snopt_leniw),
     prob->snopt_rw, &(prob->snopt_lenrw), prob->snopt_lencw * 8);

  return CSNOPT_SUCCESS;
}

static int snopt_compat_func(integer *status, const integer *n, const doublereal *x,
			     const integer *need_f, const integer *nf, doublereal *f,
			     const integer *need_Df, const integer *len_Df, doublereal *Df,
			     char *cu, integer *lencu,
			     integer *iu, integer *leniu,
			     doublereal *ru, integer *lenru){
  int i, j, k;
  void *objdata;
  objdata = (void *) cu;

  snopt_objfunc objfunc;
  objfunc = (snopt_objfunc) iu;
  /*-----------------------------------------------------------------------*/
  doublereal *func, *dfunc;
  if(*need_f != 0)
    func = f;
  else
    func = NULL;

  if(*need_Df != 0)
    dfunc = Df;
  else
    dfunc = NULL;
  /*-----------------------------------------------------------------------*/
  objfunc(*n, func, dfunc,
	  x, objdata);
  
  return CSNOPT_SUCCESS;
}

/*-----------------------------------------------------------------------*/

snopt_problem* snopt_problem_create
(int n, int nf,
 char *probname,
 char *print_stream, char *summ_stream,
 snopt_objfunc objfunc, void *objdata, int objrow,
 int n_Df, int n_A)
{
  int i, j, k;
  integer info;
  char *tmp;
  /*-----------------------------------------------------------------------*/
  snopt_problem *prob;
  prob = calloc(1, sizeof(snopt_problem));
  if(prob == NULL)
    goto mem_error;

  prob->n = n; prob->nf = nf;
  prob->objfunc = objfunc; prob->objdata = objdata; prob->objrow = objrow;
  prob->n_Df = n_Df; prob->n_A = n_A;
  /*-----------------------------------------------------------------------
    Copy character names, so that we can free them later without
    secondary consequences.
    -----------------------------------------------------------------------*/
  tmp = calloc(500, sizeof(char));
  if(tmp == NULL)
    goto mem_error;
  strcpy(tmp, probname);
  prob->probname = tmp;
  /*-----------------------------------------------------------------------*/
  if(print_stream != NULL){
    tmp = calloc(500, sizeof(char));
    if(tmp == NULL)
      goto mem_error;
    strcpy(tmp, print_stream);
    prob->print_stream = tmp;
  }
  /*-----------------------------------------------------------------------*/
  if(summ_stream != NULL){
    tmp = calloc(500, sizeof(char));
    if(tmp == NULL)
      goto mem_error;
    strcpy(tmp, summ_stream);
    prob->summ_stream = tmp;
  }
  /*-----------------------------------------------------------------------
    Initialize x, xmul, xstate
    -----------------------------------------------------------------------*/
  prob->x = calloc(prob->n, sizeof(doublereal));
  if(prob->x == NULL)
    goto mem_error;

  prob->xmul = calloc(prob->n, sizeof(doublereal));
  if(prob->xmul == NULL)
    goto mem_error;

  /*Defaults to zero*/
  prob->xstate = calloc(prob->n, sizeof(integer));
  if(prob->xstate == NULL)
    goto mem_error;
  /*-----------------------------------------------------------------------
    Initialize xlow, xup
    -----------------------------------------------------------------------*/
  prob->xlow = calloc(prob->n, sizeof(doublereal));
  if(prob->xlow == NULL)
    goto mem_error;
  for(i = 0; i < prob->n; i++)
    prob->xlow[i] = -INFINITY;

  prob->xup = calloc(prob->n, sizeof(doublereal));
  if(prob->xup == NULL)
    goto mem_error;  
  for(i = 0; i < prob->n; i++)
    prob->xup[i] = INFINITY;  
  /*-----------------------------------------------------------------------
    Initialize f, fmul, fstate
    -----------------------------------------------------------------------*/
  prob->f = calloc(prob->nf, sizeof(doublereal));
  if(prob->f == NULL)
    goto mem_error;

  prob->fmul = calloc(prob->nf, sizeof(doublereal));
  if(prob->fmul == NULL)
    goto mem_error;
  
  /*Defaults to zero*/
  prob->fstate = calloc(prob->nf, sizeof(integer));
  if(prob->fstate == NULL)
    goto mem_error;
  /*-----------------------------------------------------------------------
    Initialize flow, fup
    -----------------------------------------------------------------------*/
  prob->flow = calloc(prob->nf, sizeof(doublereal));
  if(prob->flow == NULL)
    goto mem_error;
  for(i = 0; i < prob->nf; i++)
    prob->flow[i] = -INFINITY;

  prob->fup = calloc(prob->nf, sizeof(doublereal));
  if(prob->fup == NULL)
    goto mem_error;
  for(i = 0; i < prob->nf; i++)
    prob->fup[i] = INFINITY;
  /*-----------------------------------------------------------------------
    Initialize the Jacobian matrix index variables
    -----------------------------------------------------------------------*/
  if(prob->n_Df < 0)
    prob->len_Df = prob->nf * prob->n;
  else
    prob->len_Df = prob->n_Df + 1;

  prob->Df_idx_i = calloc(prob->len_Df, sizeof(integer));
  if(prob->Df_idx_i == NULL)
    goto mem_error;

  prob->Df_idx_j = calloc(prob->len_Df, sizeof(integer));
  if(prob->Df_idx_j == NULL)
    goto mem_error;

  prob->ne_Df = 0;
  /*-----------------------------------------------------------------------
    Initialize the linear constraint matrix index variables
    -----------------------------------------------------------------------*/
  if(prob->n_A < 0)
    prob->len_A = prob->nf * prob->n;
  else
    prob->len_A = prob->n_A + 1;

  prob->A = calloc(prob->len_A, sizeof(doublereal));
  if(prob->A == NULL)
    goto mem_error;
  
  prob->A_idx_i = calloc(prob->len_A, sizeof(integer));
  if(prob->A_idx_i == NULL)
    goto mem_error;
  
  prob->A_idx_j = calloc(prob->len_A, sizeof(integer));
  if(prob->A_idx_j ==NULL)
    goto mem_error;

  prob->ne_A = 0;
  
  /*-----------------------------------------------------------------------
    Initialize with minimal arrays and find min{c, i, r}w's
    Let's not output anything to the user's streams yet.
    -----------------------------------------------------------------------*/
  if(snopt_problem_workspace_update(500, 500, 500, prob) == CSNOPT_MEM_ERROR)
    goto mem_error;

#ifdef _SNOPT_DEBUG_
  snopt_problem_sninit(-1, 6, prob);
#else
  snopt_problem_sninit(-1, -1, prob);
#endif
  /*-----------------------------------------------------------------------*/
  return prob;
  /*-----------------------------------------------------------------------
    Error conditions
    -----------------------------------------------------------------------*/  
mem_error:
  snopt_problem_close(prob);
  fprintf(stderr, "snopt_problem_create: Problem allocating memory.\n");
  return NULL;
}

int snopt_problem_init(snopt_problem *prob){
  int ret;
  ret = snopt_problem_mem_update(prob);

  switch(ret){
  case CSNOPT_MEM_ERROR:
    goto mem_error;
  case CSNOPT_SNMEM_ERROR:
    goto snmem_error;
  }
      
  /*-----------------------------------------------------------------------
    Open Print, Summ streams
    -----------------------------------------------------------------------*/
  integer info, fopen_stat = STATUS_UNKNOWN;
  if(prob->print_stream == NULL){
    /*No output*/
    prob->snopt_print_fd = -1;
  }
  else{
    if(prob->snopt_print_fd == 0){
      if(strcmp(prob->print_stream, "*stdout*") == 0)
  	prob->snopt_print_fd = 6;
      else if(strcmp(prob->print_stream, "*stderr*") == 0)
  	prob->snopt_print_fd = 0;
      else{
  	prob->snopt_print_fd = _PRINT_FD_;
	fopenwrap_(&info, &prob->snopt_print_fd, prob->print_stream,
		   &fopen_stat, strlen(prob->print_stream));
	if(info != 0)
	  goto file_error;
      }
    }
  }
  /*-----------------------------------------------------------------------*/
  if(prob->summ_stream == NULL){
    /*No output*/
    prob->snopt_summ_fd = -1;
  }
  else{
    if(prob->snopt_summ_fd == 0){
      if(strcmp(prob->summ_stream, "*stdout*") == 0)
  	prob->snopt_summ_fd = 6;
      else if(strcmp(prob->summ_stream, "*stderr*") == 0)
  	prob->snopt_summ_fd = 0;
      else{
  	prob->snopt_summ_fd = _SUMM_FD_;
	fopenwrap_(&info, &prob->snopt_summ_fd, prob->summ_stream,
		   &fopen_stat, strlen(prob->summ_stream));
	if(info != 0)
	  goto file_error;
      }
    }
  }
  /*-----------------------------------------------------------------------*/  
  snopt_problem_sninit(prob->snopt_print_fd, prob->snopt_summ_fd, prob);
  
  return CSNOPT_SUCCESS;

mem_error:
  fprintf(stderr, "snopt_problem_init: Cannot allocate memory.\n");
  return CSNOPT_MEM_ERROR;
snmem_error:
  fprintf(stderr, "snopt_problem_init: SNMEMA did not work.\n");
  return CSNOPT_SNMEM_ERROR;
file_error:
  fprintf(stderr, "snopt_problem_init: Cannot open print or summ file.\n");
  return CSNOPT_FILE_ERROR;
}

void snopt_problem_close(snopt_problem *prob){
  if(prob == NULL)
    return;
  
  free(prob->probname);
  free(prob->print_stream); free(prob->summ_stream);
  free(prob->spec_stream);

  free(prob->xlow); free(prob->xup);
  free(prob->flow); free(prob->fup);

  free(prob->Df_idx_i); free(prob->Df_idx_j);

  free(prob->A); free(prob->A_idx_i); free(prob->A_idx_j);

  free(prob->x); free(prob->xmul);
  free(prob->f); free(prob->fmul);

  free(prob->snopt_rw);
  free(prob->snopt_iw);
  free(prob->snopt_cw);

  /*-----------------------------------------------------------------------
    Close files if we opened them
    -----------------------------------------------------------------------*/
  if(prob->snopt_print_fd == _PRINT_FD_)
    fclosewrap_(&prob->snopt_print_fd);

  if(prob->snopt_summ_fd == _SUMM_FD_)
    fclosewrap_(&prob->snopt_summ_fd);

  free(prob->xstate);
  free(prob->fstate); 

  free(prob);
  return;
}

int snopt_problem_specfile
(char *spec_stream, snopt_problem *prob)
{
  integer info, fopen_stat = STATUS_EXISTS;
  prob->spec_stream = calloc(500, sizeof(char));
  if(prob->spec_stream == NULL)
    goto mem_error;
  strcpy(prob->spec_stream, spec_stream);
  
  /*I think its a bit stupid to read the spec from *stdin* */
  if(strcmp(prob->spec_stream, "*stdin*") == 0)
    prob->snopt_spec_fd = 5;
  else{
    prob->snopt_spec_fd = _SPEC_FD_;
    fopenwrap_(&info, &prob->snopt_spec_fd, prob->spec_stream,
	       &fopen_stat, strlen(prob->spec_stream));
    if(info != 0)
      goto file_error;
  }
 
  FORTRAN_NAME(snspec)
    (&prob->snopt_spec_fd, &info,
     prob->snopt_cw, &prob->snopt_lencw,
     prob->snopt_iw, &prob->snopt_leniw,
     prob->snopt_rw, &prob->snopt_lenrw,
     prob->snopt_lencw * 8);
  
  if(prob->snopt_spec_fd == _SPEC_FD_)
      fclosewrap_(&prob->snopt_spec_fd);

  return CSNOPT_SUCCESS;
  
mem_error:
  fprintf(stderr, "snopt_problem_specfile: Cannot allocate memory.\n");
  return CSNOPT_MEM_ERROR;

file_error:
  fprintf(stderr, "snopt_problem_specfile: Cannot open spec file.\n");
  return CSNOPT_FILE_ERROR;
}


int snopt_problem_jacobian(snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/  
  integer Cold = 0, ObjRow = prob->objrow + 1;
  doublereal ObjAdd = 0.;

  integer nxnames = 1, nfnames = 1;
  char tmpx[8], tmpf[8];  
  
  integer mincw, miniw, minrw;
  integer info;
  /*-----------------------------------------------------------------------
    Push pointers into cu and iu, and call snoptA
    -----------------------------------------------------------------------*/
  char *cu;
  cu = (char *) prob->objdata;
  integer *iu;
  iu = (integer *) prob->objfunc;
  integer lencu = 1, leniu = 1; 

  //&prob->snopt_print_fd, &prob->snopt_summ_fd,
  FORTRAN_NAME(snjac)
    (&info, 
     &prob->nf, &prob->n, &snopt_compat_func,
     prob->A_idx_i, prob->A_idx_j, &prob->len_A, &prob->ne_A, prob->A,
     prob->Df_idx_i, prob->Df_idx_j, &prob->len_Df, &prob->ne_Df,
     prob->x, prob->xlow, prob->xup,
     &mincw, &miniw, &minrw,
     cu, &lencu, iu, &leniu, prob->snopt_rw, &prob->snopt_lenrw,
     prob->snopt_cw, &prob->snopt_lencw, prob->snopt_iw, &prob->snopt_leniw, prob->snopt_rw, &prob->snopt_lenrw, lencu, 8 * prob->snopt_lencw);

  if((info == 82) || (info == 83) || (info == 84)){
    if(snopt_problem_workspace_update(mincw, miniw, minrw, prob) == CSNOPT_MEM_ERROR)
      goto mem_error;
#ifdef _SNOPT_DEBUG_
    snopt_problem_sninit(-1, 6, prob);
#else
    snopt_problem_sninit(-1, -1, prob);
#endif

    FORTRAN_NAME(snjac)
      (&info, 
       &prob->nf, &prob->n, &snopt_compat_func,
       prob->A_idx_i, prob->A_idx_j, &prob->len_A, &prob->ne_A, prob->A,
       prob->Df_idx_i, prob->Df_idx_j, &prob->len_Df, &prob->ne_Df,
       prob->x, prob->xlow, prob->xup,
       &mincw, &miniw, &minrw,
       cu, &lencu, iu, &leniu, prob->snopt_rw, &prob->snopt_lenrw,
       prob->snopt_cw, &prob->snopt_lencw, prob->snopt_iw, &prob->snopt_leniw, prob->snopt_rw, &prob->snopt_lenrw, lencu, 8 * prob->snopt_lencw);
  }

  if(info != 0)
    goto snjac_error;
  
  return CSNOPT_SUCCESS;

mem_error:
  fprintf(stderr, "snopt_problem_jacobian: Cannot allocate memory.\n");
  return CSNOPT_MEM_ERROR;
snjac_error:
  fprintf(stderr, "snopt_problem_jacobian: snjac encountered error: %d\n", info);
  return CSNOPT_SNJAC_ERROR;
}

/*-----------------------------------------------------------------------*/
int snopt_cold_optimise(snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/  
  integer Cold = 0, ObjRow = prob->objrow + 1;
  doublereal ObjAdd = 0.;

  integer nxnames = 1, nfnames = 1;
  char tmpx[8], tmpf[8];  
  
  integer mincw, miniw, minrw;
  integer info;
  /*-----------------------------------------------------------------------
    Push pointers into cu and iu, and call snoptA
    -----------------------------------------------------------------------*/
  char *cu;
  cu = (char *) prob->objdata;
  integer *iu;
  iu = (integer *) prob->objfunc;
  integer lencu = 1, leniu = 1; 
  
  snopta_(&Cold, &prob->nf, &prob->n, &nxnames, &nfnames,
	  &ObjAdd, &ObjRow, prob->probname, &snopt_compat_func,
	  prob->A_idx_i, prob->A_idx_j, &(prob->len_A), &(prob->ne_A), prob->A,
	  prob->Df_idx_i, prob->Df_idx_j, &(prob->len_Df), &(prob->ne_Df),
	  prob->xlow, prob->xup, tmpx, prob->flow, prob->fup, tmpf,
	  prob->x, prob->xstate, prob->xmul, prob->f, prob->fstate, prob->fmul,
	  &info, &mincw, &miniw, &minrw,
	  &prob->nS, &prob->nInf, &prob->sInf,
	  cu, &lencu, iu, &leniu, prob->snopt_rw, &prob->snopt_lenrw,
	  prob->snopt_cw, &prob->snopt_lencw, prob->snopt_iw, &prob->snopt_leniw, prob->snopt_rw, &prob->snopt_lenrw,
	  strlen(prob->probname), 8, 8,
	  lencu, 8 * prob->snopt_lencw);

  return info;
}
