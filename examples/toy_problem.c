#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "c-snopt.h"

void toyfunc(integer kopt, doublereal *f, doublereal *dfdx, const doublereal *x, void *f_data){
  if(f != NULL){
    f[0] = x[1];
    f[1] = x[0] * x[0] + 4 * x[1] * x[1];
    f[2] = pow(x[0] - 2, 2) + x[1] * x[1];
  }

  int i, j;

  if(dfdx != NULL){
    dfdx[0] = 1;
    dfdx[1] = 2 * x[0]; dfdx[2] = 4 * 2 * x[1];
    dfdx[3] = 2 * (x[0] - 2); dfdx[4] = 2 * x[1];
  }

  return;
}

int main(int argc, char **argv){
  int i, j, k;
  /*-----------------------------------------------------------------------*/
  snopt_problem *prob;
  
  prob = snopt_problem_create(2, 3,
			      "toy", "toy.out", "*stdout*",
			      &toyfunc, NULL, 0,
			      -1, 0);

  if(prob == NULL)
    goto mem_error;

  double
    xlow[] = {0, -1e6},
    xup[] = {1e6, 1e6},
    flow[] = {-1e6, -1e6, -1e6},
    fup[] = {1e6, 4, 5};

  snopt_set_xlow(xlow, prob);
  snopt_set_xup(xup, prob);

  snopt_set_flow(flow, prob);
  snopt_set_fup(fup, prob);
  
  int
    Df_idx[] = {0, 1,
		1, 0,
		1, 1,
		2, 0,
		2, 1};
 
  snopt_set_Df_idx(5, Df_idx, prob);

  //snopt_problem_jacobian(prob);
  
  snopt_problem_init(prob);

  snopt_problem_specfile("toy.spc", prob);
  
  snopt_cold_optimise(prob);

  printf("\n%lf\t%lf\n", prob->x[0], prob->x[1]);

  snopt_problem_close(prob);
  
  return 0;
mem_error:
  fprintf(stderr, "main: Insufficient memory. Quitting.\n");
  return 1;
}
