#include <stdio.h>
#include <math.h>
#include <string.h>

#include "c-snopt.h"

int snopt_set_xlow(double *y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->n; i++)
    prob->xlow[i] = y[i];
  return CSNOPT_SUCCESS;
}

int snopt_set_xlow1(double y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->n; i++)
    prob->xlow[i] = y;
  return CSNOPT_SUCCESS;
}

int snopt_set_xup(double *y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->n; i++)
    prob->xup[i] = y[i];
  return CSNOPT_SUCCESS;
}

int snopt_set_xup1(double y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->n; i++)
    prob->xup[i] = y;
  return CSNOPT_SUCCESS;
}

int snopt_set_flow(double *y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->nf; i++)
    prob->flow[i] = y[i];
  return CSNOPT_SUCCESS;
}

int snopt_set_flow1(double y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->nf; i++)
    prob->flow[i] = y;
  return CSNOPT_SUCCESS;
}

int snopt_set_fup(double *y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->nf; i++)
    prob->fup[i] = y[i];
  return CSNOPT_SUCCESS;
}

int snopt_set_fup1(double y, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < prob->nf; i++)
    prob->fup[i] = y;
  return CSNOPT_SUCCESS;
}


int snopt_set_Df_idx(int ne_Df, int *Df_idx, snopt_problem *prob){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  for(i = 0; i < ne_Df; i++){
    prob->Df_idx_i[i] = Df_idx[i * 2 + 0] + 1;
    prob->Df_idx_j[i] = Df_idx[i * 2 + 1] + 1;
  }
  prob->ne_Df = ne_Df;

  return CSNOPT_SUCCESS;
}
