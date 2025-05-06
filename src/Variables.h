#ifndef __VARIABLES_HEADER__
#define __VARIABLES_HEADER__


#include "../ThirdParty/cplex/include/cplex.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

#define BENDERS_EPSILON             1e-3
#define EPSILON                     1e-5
#define PrintErr()                  printf("ERROR! [%s:%d]\n", __FILE__, __LINE__)

struct SampStruct;
struct SuppStruct;
struct CustStruct;
struct EdgeStruct;

/* Edge structure in graph */
struct EdgeStruct
{
   int id;
   int pos; /* Position in subproblem */
   CustStruct* cust; /* customer */
   SuppStruct* supp; /* supplier */
};

/* Customer structure in graph */
struct CustStruct
{
   int id;
   int pos; /* Position in subproblem */
   double x;
   double y;
   int degree;
   vector<EdgeStruct*> Edge;
   vector<double> demand;
   vector<double> cost;
};

/* Supplier structure in graph */
struct SuppStruct
{
   int id;
   int pos = -1; /* Position in master problem */
   double x;
   double y;
   int degree;
   double liveprob;
   vector<EdgeStruct*> Edge;
   int bandwidth;
   double working;
   double recover;
   vector<bool> alive;
   double status; /* status = alive[t] && (working || recover) */
   SuppStruct* near;
};

/* Sample structure in graph */
struct SampStruct
{
   int id;
   int pos; /* Position in master problem */
   double* Cost;
   double* Demand;
   double* Sum;
};

/* Problem InstStruct structure */
struct InstStruct
{
   /* Settings */
   int QoS; /* Quality of Service */
   int SolvingSetting; /* Solving settings (1 for (CPX), 2 for (Benders) or 3 for (Benders + Cuts)) */
   int DisasterType;
   double TimeLimit; /* Time limit */
   //TODO
   double MemLimit; /* Memory limit */
   double MinGap;
   int Budget; /* Cardinality restriction */
   int ProduceSeed = 1; /* Random seed of producing live-arc graphs (default: 1) */
   bool isCasc = false; /* Whether consider cascading failures of secondary disasters */

   /* Graph structures */
   int NumSamp; /* Number of sampling */
   int NumRSamp = 10000; /* Number of sampling for computing simulation result */
   int NumCust; /* Nmber of custormer */
   int NumSupp; /* Number of supplier */
   int NumWork = 0; /* Number of working supplier */
   int NumReco; /* Number of supplier may be recovered (NumReco = NumSupp-NumWork) */
   int NumEdge = 0; /* Number of egde */
   vector<SampStruct*> Samp; /* Sample information */
   vector<CustStruct*> Cust; /* Custormer information */
   vector<SuppStruct*> Supp; /* Supplier information */
   vector<SuppStruct*> Reco; /* Damaged supplier information */
   vector<EdgeStruct*> Edge; /* Edge information */
   int* CustPos; /* Customers' position in MILP model of CPX */
   double Prob1 = 0.5;
   double Prob2 = -1.0;
   int NumIter = 0;

   /* Variables and parameters in solving */
   int NumBinCut = 0; /* Number of Benders cuts for binary relaxed solution */
   int NumFracCut = 0; /* Number of Benders cuts for fractional relaxed solution */
   int LastDepth = 0; /* Last branch-and-bound node depth */
   double LastRelaxObj = -1e+15; /* Last Lp value */
   double CutRhs = 0; /* Right hand side (constant term) of Benders cut */
   int NumThread = 1; /* Number of threads */
   int NumNode; /* Number of branch-and-bound nodes */
   double OptObj; /* Optimal objective value */
   double Result = 0.0; /* Optimal objective value (compute a large enough number of scenarios) */
   double RelaxObj; /* Relaxed objective value */
   double* CutVal; /* Coefficients of Benders cut */
   int* CutInd; /* Variables indexes of Benders cut */
   vector<SuppStruct*> OneSupp;
   double* Pi; /* dual values */
   double VioVal; /* Violated value of Benders cut */
   double* RelaxSol; /* Relaxed solution in branch-and-bound tree */
   double BuildTime;

   /* CPX */
   CPXLPptr  CPXLp; /* CPX Lp point for the Benders model */
   CPXENVptr CPXEnv; /* CPX environment point for the Benders model */
   CPXENVptr CPXEnvMaster; /* CPX environment point for the Benders model */
   CPXLPptr  CPXLpMaster; /* CPX Lp point for the Benders model */
   CPXENVptr CPXEnvSub; /* CPX environment point for the Benders model */
   CPXLPptr  CPXLpSub; /* CPX Lp point for the Benders model */
   double CPXSolvingTime; /* CPX solving time */
   int CPXSeed = -1; /* CPX random seed */
   int CPXStatus; /* CPX status */
};


#endif
