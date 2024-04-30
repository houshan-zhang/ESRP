#ifndef __BENDERS_HEADER__
#define __BENDERS_HEADER__


#include "Variables.h"
#include "Functions.h"

using namespace std;


/* Build the Benders master problem formulation of the NRP */
void BuildMasterModel(InstStruct *Inst);

/* Solve the NRP under BD framework */
void SolveMasterModel(InstStruct *Inst);

/* Get the right hand side and coefficients of Benders cut */
void GetBendersCut(InstStruct* Inst, int t, double& mincost);

/* Build the Benders subproblem formulation of the NRP */
void BuildSubModel(InstStruct *Inst);

/* Free and close CPX environment */
void CleanBendersModel(InstStruct *Inst);

/* CPLEX CALLBACK of adding Benders optimality cuts for binary relaxed solution */
int CPXPUBLIC BinCallback(CPXCENVptr Env, void* cb_data, int wherefrom, void* cb_handle, int* useraction_p);

/* CPLEX CALLBACK of adding Benders optimality cuts for fractional relaxed solution */
int CPXPUBLIC FracCallback(CPXCENVptr Env, void* cb_data, int wherefrom, void* cb_handle, int* useraction_p);


#endif
