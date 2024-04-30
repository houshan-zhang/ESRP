#include "Benders.h"



/* CPLEX CALLBACK of adding Benders optimality cuts for binary relaxed solution */
int CPXPUBLIC BinCallback(CPXCENVptr Env, void* cb_data, int wherefrom, void* cb_handle, int* useraction_p)
{
   InstStruct* Inst = (InstStruct*)cb_handle;
   *useraction_p = CPX_CALLBACK_DEFAULT;

   Inst->CutRhs = 0.0;
   for( int j = 0; j < Inst->NumReco; j++ )
      Inst->CutVal[j] = 0.0; /* Initialize the cut Coefficients */
   Inst->CutVal[Inst->NumReco] = 1;
   double mincost;
   bool isviolate = false;

   /* Get relaxed solution */
   Inst->CPXStatus = CPXgetcallbacknodex(Env, cb_data, wherefrom, Inst->RelaxSol, 0, Inst->NumReco + Inst->NumSamp - 1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get the indexes where the solution takes the value of one */
   Inst->OneSupp.resize(0);
   for( int j = 0; j < Inst->NumReco; j++ )
   {
      SuppStruct* supp = Inst->Reco[j];
      if( Inst->RelaxSol[j] >= 0.999 )
         Inst->OneSupp.push_back(supp);
      else if( Inst->RelaxSol[j] >= 0.001 )
         PrintErr();
   }

   /* Open Suppliers */
   SetSupplier(Inst, Inst->OneSupp, 1.0);
   //cout<<Inst->OneSupp.size()<<" - "<<Inst->Budget<<endl;

   /* Calculate and add possible Benders cuts for each scenario */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int j = 0; j < Inst->NumReco; j++ )
         Inst->CutVal[j] = 0.0; /* Initialize the cut Coefficients */
      /* Change coeffcients */
      ChgCPXCoef(Inst, t);

      /* Set suppliers status */
      SetSupplierStatus(Inst, Inst->Supp, t);

      /* Solve subproblem */
      Inst->CPXStatus = CPXlpopt(Inst->CPXEnvSub, Inst->CPXLpSub);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      Inst->CPXStatus = CPXgetobjval(Inst->CPXEnvSub, Inst->CPXLpSub, &mincost);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      Inst->CutRhs = mincost;

      /* Get dual values */
      Inst->CPXStatus = CPXgetpi(Inst->CPXEnvSub, Inst->CPXLpSub, Inst->Pi, Inst->NumCust, Inst->NumCust+Inst->NumSupp-1);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      //for( int j = 0; j < Inst->NumSupp; j++ )
      //   cout<<Inst->Pi[j]<<" - ";
      //cout<<endl;

      /* Get the right hand side of Benders cut */
      for( unsigned j = 0; j < Inst->OneSupp.size(); j++ )
      {
         SuppStruct* supp = Inst->OneSupp[j];
         if( supp->alive[t] )
            Inst->CutRhs -= supp->bandwidth * Inst->Pi[supp->id];
      }

      /* Get coefficients of Benders cut */
      for( int j = 0; j < Inst->NumReco; j++ )
      {
         SuppStruct* supp = Inst->Reco[j];
         if( supp->alive[t] )
         {
            Inst->CutVal[j] = -supp->bandwidth * Inst->Pi[supp->id];
         }
      }

      /* Calculate the violated value of cut */
      Inst->VioVal = mincost - Inst->RelaxSol[Inst->NumReco + t];

      /* If the violated value is greater than epsilon, then add the Benders cut by callback function */
      if( Inst->VioVal > BENDERS_EPSILON )
      {
         if( !isviolate )
         {
            isviolate = true;
            Inst->NumIter++;
         }
         Inst->CutInd[Inst->NumReco] = Inst->NumReco + t;
         Inst->CPXStatus = CPXcutcallbackadd(Env, cb_data, wherefrom, Inst->NumReco + 1, Inst->CutRhs, 'G', Inst->CutInd, Inst->CutVal, 0);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         Inst->NumBinCut++; /* Number of Benders cuts for binary solution plus one */
#if 0
         /* TEST PRINT */
         cout << "Benders Cut, Sample: " << t << endl;
         for( int i = 0; i < Inst->NumReco; i++ )
            cout << Inst->CutVal[i] << " S(" << Inst->Reco[Inst->CutInd[i]]->id << ") + ";
         cout << Inst->CutVal[Inst->NumReco] << " V(" << t << ")";
         cout << " >= " << Inst->CutRhs << endl;
         /* TEST PRINT */
#endif
      }
   }

   /* Close Suppliers */
   SetSupplier(Inst, Inst->OneSupp, 0.0);

   (*useraction_p) = CPX_CALLBACK_SET;
   return 0;
}

/* CPLEX CALLBACK of adding Benders optimality cuts for fractional relaxed solution */
int CPXPUBLIC FracCallback(CPXCENVptr Env, void* cb_data, int wherefrom, void* cb_handle, int* useraction_p)
{
   InstStruct* Inst = (InstStruct*)cb_handle;
   *useraction_p = CPX_CALLBACK_DEFAULT;

   Inst->CutRhs = 0.0;
   int CPXDepth = 0;
   double RelaxObj;
   double IncObj;
   double mincost;
   double FracEPS;
   bool isviolate = false;
   for( int j = 0; j < Inst->NumReco; j++ )
      Inst->CutVal[j] = 0.0; /* Initialize the cut Coefficients */
   Inst->CutVal[Inst->NumReco] = 1;


   /* Get node index */
   Inst->CPXStatus=CPXgetcallbacknodeinfo(Env, cb_data, wherefrom, 0, CPX_CALLBACK_INFO_NODE_DEPTH, &CPXDepth);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get relaxed objective value of current node */
   Inst->CPXStatus=CPXgetcallbacknodeobjval(Env, cb_data, wherefrom, &RelaxObj);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get incumbent objective value of original problem */
   Inst->CPXStatus=CPXgetcallbackinfo(Env, cb_data, wherefrom, CPX_CALLBACK_INFO_BEST_INTEGER, &IncObj);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   FracEPS = (IncObj-RelaxObj)/(1000*Inst->NumSamp) + BENDERS_EPSILON;

   //cout << CPXDepth << " - " << RelaxObj << " - " << Inst->LastRelaxObj << endl;
   /* The separation procedure stops if the dual bound improves by less than 0.001 */
#if 1
   //if( CPXDepth%10 == 0 && RelaxObj - Inst->LastRelaxObj > 1 )
   if( RelaxObj - Inst->LastRelaxObj > 0.001 )
   {
      Inst->LastDepth = CPXDepth;
      Inst->LastRelaxObj = RelaxObj;
   }
   else
      return 0;
#endif

   /* Get relaxed solution */
   Inst->CPXStatus = CPXgetcallbacknodex(Env, cb_data, wherefrom, Inst->RelaxSol, 0, Inst->NumReco + Inst->NumSamp - 1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Open suppliers */
   for( int j = 0; j < Inst->NumReco; j++ )
   {
      SuppStruct* supp = Inst->Reco[j];
      SetSupplier(Inst, {supp}, Inst->RelaxSol[j]);
   }

   /* Calculate and add possible Benders cuts for each scenario */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int j = 0; j < Inst->NumReco; j++ )
         Inst->CutVal[j] = 0.0; /* Initialize the cut Coefficients */
      GetBendersCut(Inst, t, mincost);
      Inst->CutInd[Inst->NumReco] = Inst->NumReco + t;
      /* Calculate the violated value of cut */
      Inst->VioVal = mincost - Inst->RelaxSol[Inst->NumReco + t];

      /* If the violated value is greater than epsilon, then add the Benders cut by callback function */
      //cout<<Inst->VioVal<<" - "<<FracEPS<<endl;
      if( Inst->VioVal > FracEPS )
      {
         if( !isviolate )
         {
            isviolate = true;
            Inst->NumIter++;
         }
         Inst->CPXStatus = CPXcutcallbackadd(Env, cb_data, wherefrom, Inst->NumReco + 1, Inst->CutRhs, 'G', Inst->CutInd, Inst->CutVal, 0);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         Inst->NumFracCut++; /* Number of Benders cuts for binary solution plus one */
#if 0
         /* TEST PRINT */
         cout << "Benders Cut, Sample: " << t << endl;
         for( int i = 0; i < Inst->NumReco; i++ )
            cout << Inst->CutVal[i] << " S(" << Inst->Reco[Inst->CutInd[i]]->id << ") + ";
         cout << Inst->CutVal[Inst->NumReco] << " V(" << t << ")";
         cout << " >= " << Inst->CutRhs << endl;
         /* TEST PRINT */
#endif
      }
   }

   /* Close suppliers */
   SetSupplier(Inst, Inst->Reco, 0.0);

   (*useraction_p) = CPX_CALLBACK_SET;
   return 0;
}

void GetBendersCut(InstStruct* Inst, int t, double& mincost)
{
   /* Change coeffcients */
   ChgCPXCoef(Inst, t);

   /* Set suppliers status */
   SetSupplierStatus(Inst, Inst->Supp, t);

   /* Solve subproblem */
   Inst->CPXStatus = CPXlpopt(Inst->CPXEnvSub, Inst->CPXLpSub);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus = CPXgetobjval(Inst->CPXEnvSub, Inst->CPXLpSub, &mincost);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CutRhs = mincost;

   /* Get dual values */
   Inst->CPXStatus = CPXgetpi(Inst->CPXEnvSub, Inst->CPXLpSub, Inst->Pi, Inst->NumCust, Inst->NumCust+Inst->NumSupp-1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   //for( int j = 0; j < Inst->NumSupp; j++ )
   //   cout<<Inst->Pi[j]<<" - ";
   //cout<<endl;

   /* Get coefficients of Benders cut */
   for( int j = 0; j < Inst->NumReco; j++ )
   {
      SuppStruct* supp = Inst->Reco[j];
      if( supp->alive[t] )
      {
         Inst->CutRhs -= supp->bandwidth * Inst->Pi[supp->id] * Inst->RelaxSol[supp->pos];
         Inst->CutVal[j] = -supp->bandwidth * Inst->Pi[supp->id];
      }
   }
}

/* Build the Benders subproblem formulation */
void BuildSubModel(InstStruct* Inst)
{
   /* Open CPX environment */
   Inst->CPXEnvSub = CPXopenCPLEX(&(Inst->CPXStatus));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   /* Create NRP InstStruct */
   Inst->CPXLpSub = CPXcreateprob(Inst->CPXEnvSub, &(Inst->CPXStatus), "CPX Sub formulation");
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   /* CPX_OFF is default value of ScreenOutput in callback mode */
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnvSub, CPXPARAM_ScreenOutput, CPX_OFF);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   /* set threads is critical */
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnvSub, CPXPARAM_Threads, 1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   /* Select dual simplex method optimizer */
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnvSub, CPXPARAM_LPMethod, CPX_ALG_DUAL);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Create and add variables to the Benders subproblem */
   /* Attention! The variable type cannot be specified. If not, cplex determines that the problem is an MIP problem */
   int NumVar = Inst->NumCust + Inst->NumEdge;
   double* Obj = new double[NumVar]; /* Objective Coefficients */
   double* Lb = new double[NumVar]; /* Variables low bounds */
   double* Ub = new double[NumVar]; /* Variables upper bounds */
   char** NameVar = new char*[NumVar]; /* Variables names */

   for( int i = 0; i < NumVar; i++ )
      NameVar[i] = new char[100];

   /* ( Col - 1 ) Create customer variables */
   int counter = 0;
   for( int j = 0; j < Inst->NumCust; j++ )
   {
      Obj[counter] = 0.0;
      Lb[counter] = 0.0;
      Ub[counter] = 1e+15;
      sprintf(NameVar[counter], "loss-%d", j);
      counter++;
   }

   /* ( Col - 2 ) Create flow variables */
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      CustStruct* cust = Inst->Cust[i];
      for( int j = 0; j < cust->degree; j++ )
      {
         Obj[counter] = 0.0;
         Lb[counter] = 0.0;
         Ub[counter] = 1e+15;
         sprintf(NameVar[counter], "flow-%d-%d", i, cust->Edge[j]->supp->id);
         counter++;
      }
   }

   /* Add variables information to CPX */
   Inst->CPXStatus = CPXnewcols(Inst->CPXEnvSub, Inst->CPXLpSub, NumVar, Obj, Lb, Ub, NULL, NameVar);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   delete []Obj;
   delete []Lb;
   delete []Ub;
   for( int i = 0; i < NumVar; i++ )
   {
      delete []NameVar[i];
   }
   delete []NameVar;

   CPXchgobjsen(Inst->CPXEnvSub, Inst->CPXLpSub, CPX_MIN); /* Set optimization direction to be "MIN" */

   /* Add the constraints of the subproblem */
   int* ConsInd = new int[Inst->NumEdge + 1];
   double* ConsVal = new double[Inst->NumEdge + 1];
   double rhs;
   char sense;
   int rbegin = 0;

   /* ( Row - 1 ) Creat the demand constraints */
   sense = 'E'; /* Set the constraint "==" */
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      CustStruct* cust = Inst->Cust[i];
      for( int j = 0; j < cust->degree; j++ )
      {
         EdgeStruct* edge = cust->Edge[j];
         ConsVal[j] = 1;
         ConsInd[j] = edge->pos;
      }
      ConsVal[cust->degree] = 1;
      ConsInd[cust->degree] = cust->pos;
      rhs = 0.0;
      Inst->CPXStatus=CPXaddrows(Inst->CPXEnvSub, Inst->CPXLpSub, 0, 1, cust->degree+1, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   }

   /* ( Row - 2 ) Creat the Capacity constraint */
   sense = 'L'; /* Set the constraint "<=" */
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      SuppStruct* supp = Inst->Supp[j];
      for( int i = 0; i < supp->degree; i++ )
      {
         EdgeStruct* edge = supp->Edge[i];
         ConsVal[i] = 1;
         ConsInd[i] = edge->pos;
      }
      rhs = 0.0;
      Inst->CPXStatus=CPXaddrows(Inst->CPXEnvSub, Inst->CPXLpSub, 0, 1, supp->degree, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   }

   delete []ConsVal;
   delete []ConsInd;
}

/* Build the Benders master formulation */
void BuildMasterModel(InstStruct* Inst)
{
   /* Initialization */
   Inst->CutInd = new int[Inst->NumReco+1];
   for( int j = 0; j < Inst->NumReco; j++ )
      Inst->CutInd[j] = j;
   Inst->CutVal = new double[Inst->NumReco+1];
   /* Relaxed solution during solving process */
   Inst->RelaxSol = new double[Inst->NumReco+Inst->NumSamp];
   Inst->Pi = new double[Inst->NumSupp];

   /* Open CPX environment */
   Inst->CPXEnvMaster = CPXopenCPLEX(&(Inst->CPXStatus));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Create NRP InstStruct */
   Inst->CPXLpMaster = CPXcreateprob(Inst->CPXEnvMaster, &(Inst->CPXStatus), "CPX Master formulation");
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Create and add variables to the master problem */
   int NumVar;
   NumVar = Inst->NumReco + Inst->NumSamp;
   double* Obj = new double[NumVar]; /* Objective Coefficients */
   double* Lb = new double[NumVar]; /* Variables low bounds */
   double* Ub = new double[NumVar]; /* Variables upper bounds */
   char* TypeVar = new char[NumVar]; /* Variables types */
   char** NameVar = new char*[NumVar]; /* Variables names */

   for( int i = 0; i < NumVar; i++ )
      NameVar[i] = new char[100];

   /* ( Col - 1 ) Create binary variables for each supplier*/
   int counter = 0;
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      if( IsEq(Inst->Supp[j]->working, 0.0) )
      {
         Obj[counter] = 0.0;
         Lb[counter] = 0.0;
         Ub[counter] = 1.0;
         TypeVar[counter] = 'B';
         sprintf(NameVar[counter], "supplier-%d", j);
         counter++;
      }
   }
   if( counter != Inst->NumReco ) { PrintErr(); }
   //Inst->CPXStatus = CPXchgobjoffset(Inst->CPXEnvMaster, Inst->CPXLpMaster, Inst->Budget);

   /* ( Col - 2 ) Create continuous variables for each scenario */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      Obj[counter] = 1;
      Lb[counter] = 0.0;
      Ub[counter] = 1e+15;
      TypeVar[counter] = 'C';
      sprintf(NameVar[counter], "scenario-%d", t);
      counter++;
   }

   /* Add variables information to CPX */
   Inst->CPXStatus = CPXnewcols(Inst->CPXEnvMaster, Inst->CPXLpMaster, NumVar, Obj, Lb, Ub, TypeVar, NameVar);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   delete []Obj;
   delete []Lb;
   delete []Ub;
   delete []TypeVar;

   for( int i = 0; i < NumVar; i++ )
   {
      delete []NameVar[i];
   }
   delete []NameVar;

   CPXchgobjsen(Inst->CPXEnvMaster, Inst->CPXLpMaster, CPX_MIN); /* Set optimization direction to be "MIN" */

   /* Add the constraints of the master problem */
   int* ConsInd = new int[Inst->NumReco + 1];
   double* ConsVal = new double[Inst->NumReco + 1];
   char sense;
   int rbegin = 0;
   double mincost;
   /* ( Row - 1 ) Creat the Cardinality (budget) constraint */
   sense = 'L'; /* Set the constraint "<=" */
   counter = 0;
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      if( IsEq(Inst->Supp[j]->working, 0.0) )
      {
         ConsVal[counter] = 1;
         ConsInd[counter] = Inst->Supp[j]->pos;
         counter++;
      }
   }
   double rhs = double(Inst->Budget);
   Inst->CPXStatus=CPXaddrows(Inst->CPXEnvMaster, Inst->CPXLpMaster, 0, 1, Inst->NumReco, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   if( Inst->SolvingSetting == 3 )
   {
      /* ( Row - 2 ) Creat the submodular constraints */
      sense = 'G'; /* Set the constraint ">=" */
      for( int j = 0; j < Inst->NumReco; j++ )
         ConsVal[j] = 0.0; /* Initialize the cut Coefficients */
      for( int t = 0; t < Inst->NumSamp; t++ )
      {
         for( int j = 0; j < Inst->NumReco; j++ )
            ConsVal[j] = 0.0; /* Initialize the cut Coefficients */
         counter = 0;
         SampStruct* samp = Inst->Samp[t];
         SetSupplier(Inst, Inst->Reco, 0.0);
         SetSupplierStatus(Inst, Inst->Supp, t);
         ChgCPXCoef(Inst, t);
         Inst->CPXStatus = CPXlpopt(Inst->CPXEnvSub, Inst->CPXLpSub);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         Inst->CPXStatus = CPXgetobjval(Inst->CPXEnvSub, Inst->CPXLpSub, &rhs);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         for( int j = 0; j < Inst->NumSupp; j++ )
         {
            SuppStruct* supp = Inst->Supp[j];
            if( IsEq(supp->working, 0.0) )
            {
               SetSupplier(Inst, {supp}, 1.0);
               SetSupplierStatus(Inst, {supp}, t);
               if( supp->alive[t] )
               {
                  Inst->CPXStatus = CPXlpopt(Inst->CPXEnvSub, Inst->CPXLpSub);
                  if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
                  Inst->CPXStatus = CPXgetobjval(Inst->CPXEnvSub, Inst->CPXLpSub, &mincost);
                  if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
               }
               else
                  mincost = rhs;
               ConsInd[counter] = supp->pos;
               ConsVal[counter] += (rhs - mincost);
               SetSupplier(Inst, {supp}, 0.0);
               SetSupplierStatus(Inst, {supp}, t);
               counter++;
            }
         }
         if( counter != Inst->NumReco ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         ConsInd[Inst->NumReco] = samp->pos;
         ConsVal[Inst->NumReco] = 1;
         Inst->CPXStatus = CPXaddrows(Inst->CPXEnvMaster, Inst->CPXLpMaster, 0, 1, Inst->NumReco+1, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL,NULL);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      }
#if 1
      /* ( Row - 3 ) Creat the aggregated capacity constraints */
      sense = 'G'; /* Set the constraint ">=" */
      for( int j = 0; j < Inst->NumReco; j++ )
         ConsVal[j] = 0.0; /* Initialize the cut Coefficients */
      for( int t = 0; t < Inst->NumSamp; t++ )
      {
         for( int j = 0; j < Inst->NumReco; j++ )
            ConsVal[j] = 0.0; /* Initialize the cut Coefficients */
         counter = 0;
         SampStruct* samp = Inst->Samp[t];
         rhs = 0.0;
         for( int i = 0; i < Inst->NumCust; i++ )
         {
            CustStruct* cust = Inst->Cust[i];
            rhs += cust->demand[t];
         }
         for( int j = 0; j < Inst->NumSupp; j++ )
         {
            SuppStruct* supp = Inst->Supp[j];
            if( IsEq(supp->working, 1.0) && supp->alive[t] )
               rhs -= supp->bandwidth;
         }
         rhs = double(rhs)/Inst->NumSamp;
         for( int j = 0; j < Inst->NumSupp; j++ )
         {
            SuppStruct* supp = Inst->Supp[j];
            if( IsEq(supp->working, 0.0) )
            {
               ConsInd[counter] = supp->pos;
               if( supp->alive[t] )
                  ConsVal[counter] = supp->bandwidth;
               else
                  ConsVal[counter] = 0.0;
               counter++;
            }
         }
         if( counter != Inst->NumReco ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         ConsInd[Inst->NumReco] = samp->pos;
         ConsVal[Inst->NumReco] = 1;
         Inst->CPXStatus = CPXaddrows(Inst->CPXEnvMaster, Inst->CPXLpMaster, 0, 1, Inst->NumReco+1, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL,NULL);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      }
#endif
   }


   delete []ConsVal;
   delete []ConsInd;
}

/* Solve the NRP under BD framework */
void SolveMasterModel(InstStruct* Inst)
{
   /* Output information to the screen */
   CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_SCRIND, CPX_ON);

   /* Set the random seed during the Branch-and-Bound process in CPX */
   if (Inst->CPXSeed != -1)
   {
      Inst->CPXStatus = CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_RANDOMSEED, Inst->CPXSeed);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   }

   /* Set the number of Threads in the calculation */
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_THREADS, Inst->NumThread);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set the timelimit of the CPX (i.e., total timelimit minus the time cost in presolving) */
   double CPXTimeLimit = 0.0;
   if (Inst->TimeLimit <= Inst->BuildTime)
      CPXTimeLimit = 0.0;
   else
      CPXTimeLimit = Inst->TimeLimit - Inst->BuildTime;
   cout << "CPX Solving Time Limit: " << CPXTimeLimit << endl;
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnvMaster, CPXPARAM_TimeLimit, CPXTimeLimit);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set the absolute tolerance of CPX to 0.0 */
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnvMaster, CPX_PARAM_EPAGAP, 0.0);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set the relative tolerance of CPX to 0.0 */
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnvMaster, CPX_PARAM_EPGAP, 0.0);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* * solving the MIP model */
   clock_t TimeStart=clock();

   /* Set presolving keeping row structures and the callback functions working on the original model */
   CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_MIPCBREDLP, CPX_OFF);
   CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_PRELINEAR, CPX_OFF);
   CPXsetintparam(Inst->CPXEnvMaster, CPX_PARAM_REDUCE, CPX_PREREDUCE_PRIMALONLY);

   /* Set callback function of CPX for those binary relaxed solution */
   Inst->CPXStatus = CPXsetlazyconstraintcallbackfunc(Inst->CPXEnvMaster, BinCallback, Inst);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set callback function of CPX for those fractional relaxed solution */
   Inst->CPXStatus = CPXsetusercutcallbackfunc(Inst->CPXEnvMaster, FracCallback, Inst);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* CPX optimization process */
   Inst->CPXStatus = CPXmipopt(Inst->CPXEnvMaster, Inst->CPXLpMaster);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

#if 0
   /* TEST PRINT */
   double* sol = new double[Inst->NumReco+Inst->NumSamp];
   Inst->CPXStatus = CPXgetx(Inst->CPXEnvMaster, Inst->CPXLpMaster, sol, 0, Inst->NumReco+Inst->NumSamp-1);
   for( int j = 0; j < Inst->NumReco; j++ )
   {
      if( sol[j] >= 0.999 )
         cout << sol[j] << " S(" << Inst->Reco[j]->id << ")  ";
   }
   cout << endl;
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      cout << sol[t+Inst->NumReco] << " V(" << t << ")  ";
   }
   cout << endl;
   delete []sol;
   /* TEST PRINT */
#endif

   clock_t TimeEnd=clock();
   Inst->CPXSolvingTime = (double)(TimeEnd - TimeStart)/(double)CLOCKS_PER_SEC;

   /* Get the best feasible objective value within timelimit*/
   Inst->CPXStatus=CPXgetmipobjval(Inst->CPXEnvMaster, Inst->CPXLpMaster, &(Inst->OptObj));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get the best relaxed objective value within timelimit*/
   Inst->CPXStatus=CPXgetbestobjval(Inst->CPXEnvMaster, Inst->CPXLpMaster, &(Inst->RelaxObj));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   Inst->NumNode = CPXgetnodecnt(Inst->CPXEnvMaster, Inst->CPXLpMaster); /* Get the number of branch-and-bound nodes */

   /* Output the statistic information */
   cout<<endl<<endl;
   cout << "Results statistics:"<<endl;
   cout << "\t(a) Optimal objective value:\t\t\t\t" << Inst->OptObj <<endl;
   cout << "\t(b) Relaxed objective value:\t\t\t\t" << Inst->RelaxObj <<endl;
   cout << "\t(c) Number of branch-and-bound nodes:\t\t\t" << Inst->NumNode <<endl;
   cout << "\t(d) CPX solving time:\t\t\t\t\t" << Inst->CPXSolvingTime << endl;
   cout << "\t(e) Total time:\t\t\t\t\t\t" << Inst->BuildTime + Inst->CPXSolvingTime << endl;
   cout << "\t(f) Number of Benders cuts for binary solution:\t\t" <<  Inst->NumBinCut << endl;
   cout << "\t(g) Number of Benders cuts for fractional solution:\t"<<  Inst->NumFracCut << endl;
   cout << "\t(h) Number of Benders iterations:\t\t\t"<< Inst->NumIter << endl;
}

/* Free and close CPX environment */
void CleanBendersModel(InstStruct* Inst)
{
   /* Clena master problem */
   Inst->CPXStatus=CPXfreeprob(Inst->CPXEnvMaster, &(Inst->CPXLpMaster));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus=CPXcloseCPLEX(&(Inst->CPXEnvMaster));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Clean subproblem */
   Inst->CPXStatus=CPXfreeprob(Inst->CPXEnvSub, &(Inst->CPXLpSub));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus=CPXcloseCPLEX(&(Inst->CPXEnvSub));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   delete []Inst->CutVal;
   delete []Inst->CutInd;
   delete []Inst->RelaxSol;
   delete []Inst->Pi;
}
