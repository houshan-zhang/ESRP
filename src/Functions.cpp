#include "Functions.h"
#include "Variables.h"

bool IsEq( double a, double b )
{
   return abs(a-b) <= EPSILON;
}

void GenerateGraph(InstStruct* Inst)
{
   srand(Inst->ProduceSeed);
   Inst->Cust = vector<CustStruct*>(Inst->NumCust);
   Inst->CustPos = new int[Inst->NumCust];
   Inst->Supp = vector<SuppStruct*>(Inst->NumSupp);
   int counter = 0;
   /* Generate customer */
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      Inst->Cust[i] = new CustStruct;
      CustStruct* cust = Inst->Cust[i];
      cust->id = i;
      cust->pos = i;
      Inst->CustPos[i] = i;
      cust->x = 100*(rand()/double(RAND_MAX));
      cust->y = 100*(rand()/double(RAND_MAX));
   }
   /* Generate supplier */
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      Inst->Supp[j] = new SuppStruct;
      SuppStruct* supp = Inst->Supp[j];
      supp->id = j;
      supp->x = 100*(rand()/double(RAND_MAX));
      supp->y = 100*(rand()/double(RAND_MAX));
      supp->bandwidth = rand()%40000+10000;
      double tmp = rand()/double(RAND_MAX);
      /* ---------- */
      supp->working = 1.0;
      if( Inst->DisasterType == 1 )
      {
         if (sqrt(pow(supp->x-50, 2) + pow(supp->y-50, 2 )) <= sqrt(10000.0*(1.0-Inst->Prob1)/3.1415926))
            supp->working = 0.0;
      }
      if( Inst->DisasterType == 2 )
      {
         if( sqrt(pow(supp->x-25, 2) + pow(supp->y-25, 2 )) <= sqrt(2500.0*(1.0-Inst->Prob1)/3.1415926) )
            supp->working = 0.0;
         else if( sqrt(pow(supp->x-75, 2) + pow(supp->y-25, 2 )) <= sqrt(2500.0*(1.0-Inst->Prob1)/3.1415926) )
            supp->working = 0.0;
         else if( sqrt(pow(supp->x-25, 2) + pow(supp->y-75, 2 )) <= sqrt(2500.0*(1.0-Inst->Prob1)/3.1415926) )
            supp->working = 0.0;
         else if( sqrt(pow(supp->x-75, 2) + pow(supp->y-75, 2 )) <= sqrt(2500.0*(1.0-Inst->Prob1)/3.1415926) )
            supp->working = 0.0;
      }
      if( Inst->DisasterType == 3 )
      {
         if( supp->x >= 50-50*(1.0-Inst->Prob1) && supp->x <= 50+50*(1.0-Inst->Prob1) )
         {
            supp->working = 0.0;
         }
      }
      if( Inst->DisasterType == 4 )
      {
         supp->working = tmp < Inst->Prob1 ? 1.0 : 0.0;
      }
      /* ---------- */
      if( IsEq(supp->working, 1.0) )
         Inst->NumWork++;
      else
      {
         supp->pos = counter;
         Inst->Reco.push_back(supp);
         counter++;
      }
      supp->recover = 0.0;
   }
   Inst->NumReco = Inst->NumSupp - Inst->NumWork;
   /* Generate edge */
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      CustStruct* cust = Inst->Cust[i];
      for( int j = 0; j < Inst->NumSupp; j++ )
      {
         SuppStruct* supp = Inst->Supp[j];
         if( sqrt(pow(cust->x-supp->x, 2) + pow(cust->y-supp->y, 2)) <= Inst->QoS )
         {
            EdgeStruct* edge = new EdgeStruct;
            Inst->Edge.push_back(edge);
            cust->Edge.push_back(edge);
            supp->Edge.push_back(edge);
            edge->id = Inst->NumEdge;
            edge->pos = Inst->NumCust + Inst->NumEdge;
            edge->cust = cust;
            edge->supp = supp;
            Inst->NumEdge++;
         }
      }
   }
   /* Write degree */
   for( int i = 0; i < Inst->NumCust; i++ )
      Inst->Cust[i]->degree = Inst->Cust[i]->Edge.size();
   for( int j = 0; j < Inst->NumSupp; j++ )
      Inst->Supp[j]->degree = Inst->Supp[j]->Edge.size();
   /* Set nearest supplier */
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      SuppStruct* supp = Inst->Supp[j];
      if( j == Inst->NumSupp-1 )
         supp->near = Inst->Supp[0];
      else
         supp->near = Inst->Supp[j+1];
   }
}

void Sample(InstStruct* Inst)
{
   /* Generate sample */
   srand(0);
   Inst->Samp = vector<SampStruct*>(Inst->NumRSamp);
   for( int t = 0; t < Inst->NumRSamp; t++ )
   {
      SampStruct* samp = new SampStruct;
      Inst->Samp[t] = samp;
      samp->id = t;
      samp->pos = Inst->NumReco + t;
      samp->Cost = new double[Inst->NumCust];
      samp->Demand = new double[Inst->NumCust];
      samp->Sum = new double[Inst->NumCust];
   }
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      CustStruct* cust = Inst->Cust[i];
      cust->demand = vector<double>(Inst->NumRSamp);
      cust->cost = vector<double>(Inst->NumRSamp);
      for( int t = 0; t < Inst->NumRSamp; t++ )
      {
         cust->demand[t] = rand()%3000;
         cust->cost[t] = 1.0/Inst->NumSamp;
         Inst->Samp[t]->Cost[i] = cust->cost[t];
         Inst->Samp[t]->Demand[i] = cust->demand[t];
         Inst->Samp[t]->Sum[i] = cust->cost[t] * cust->demand[t];
      }
   }
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      SuppStruct* supp = Inst->Supp[j];
      supp->alive = vector<bool>(Inst->NumRSamp);
      if( IsEq( Inst->Prob2, -1.0 ) )
         supp->liveprob = 0.5 + 0.5*(rand()/double(RAND_MAX));
      else
         supp->liveprob = Inst->Prob2;
      //TODO
      if( Inst->isCasc && IsEq(supp->working, 0.0) )
         supp->liveprob -= 0.1;
      for( int t = 0; t < Inst->NumRSamp; t++ )
      {
         double tmp = rand()/double(RAND_MAX);
         supp->alive[t] = tmp < supp->liveprob ? true : false;
      }
   }
   //TODO
   if( Inst->isCasc )
   {
      for( int j = 0; j < Inst->NumSupp; j++ )
      {
         SuppStruct* supp = Inst->Supp[j];
         for( int t = 0; t < Inst->NumRSamp; t++ )
         {
            if( IsEq(supp->alive[t], 0.0) || IsEq(supp->near->alive[t], 1.0) )
               continue;
            double tmp = rand()/double(RAND_MAX);
            supp->alive[t] = tmp < 0.1 ? false : true;
         }
      }
   }
}

void ComputeResult(InstStruct* Inst)
{
   /* Get relaxed solution */
   double* sol = new double[Inst->NumReco];
   if( Inst->SolvingSetting >= 2 )
      Inst->CPXStatus = CPXgetx(Inst->CPXEnvMaster, Inst->CPXLpMaster, sol, 0, Inst->NumReco-1);
   if( Inst->SolvingSetting <= 1 )
      Inst->CPXStatus = CPXgetx(Inst->CPXEnv, Inst->CPXLp, sol, 0, Inst->NumReco-1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get the indexes where the solution takes the value of one */
   Inst->OneSupp.resize(0);
   for( int j = 0; j < Inst->NumReco; j++ )
   {
      SuppStruct* supp = Inst->Reco[j];
      if( sol[j] >= 0.99 )
         Inst->OneSupp.push_back(supp);
      else if( sol[j] >= 0.01 )
         PrintErr();
   }
   delete []sol;

   /* Open Suppliers */
   SetSupplier(Inst, Inst->OneSupp, 1.0);

   for( int t = 0; t < Inst->NumRSamp; t++ )
   {
      ChgCPXCoef(Inst, t);
      SetSupplierStatus(Inst, Inst->Supp, t);

      double tmp;
      Inst->CPXStatus = CPXlpopt(Inst->CPXEnvSub, Inst->CPXLpSub);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      Inst->CPXStatus = CPXgetobjval(Inst->CPXEnvSub, Inst->CPXLpSub, &tmp);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      tmp = tmp * Inst->NumSamp / Inst->NumRSamp;
      Inst->Result += tmp ;
   }
}

void SetSupplier(InstStruct* Inst, vector<SuppStruct*> Supp, double sign)
{
   for( unsigned int j = 0; j < Supp.size(); j++ )
   {
      SuppStruct* supp = Supp[j];
      if( !IsEq(supp->working, 0.0) ) { PrintErr(); }
      supp->recover = sign;
   }
}

void SetSupplierStatus(InstStruct* Inst, vector<SuppStruct*> Supp, int t)
{
   int row_pos;
   for( unsigned int j = 0; j < Supp.size(); j++ )
   {
      SuppStruct* supp = Supp[j];
      if( supp->alive[t] )
         supp->status = IsEq(supp->working, 1.0) ? supp->working : supp->recover;
      else
         supp->status = 0.0;
      double rhs = supp->bandwidth * supp->status;
      row_pos = Inst->NumCust + supp->id;
      Inst->CPXStatus = CPXchgrhs(Inst->CPXEnvSub, Inst->CPXLpSub, 1, &row_pos, &rhs);
      if( Inst->CPXStatus ) { PrintErr(); }
   }
}

void ChgCPXCoef(InstStruct* Inst, int t)
{
   Inst->CPXStatus = CPXchgobj(Inst->CPXEnvSub, Inst->CPXLpSub, Inst->NumCust, Inst->CustPos, Inst->Samp[t]->Cost);
   if( Inst->CPXStatus ) { PrintErr(); }
   Inst->CPXStatus = CPXchgrhs(Inst->CPXEnvSub, Inst->CPXLpSub, Inst->NumCust, Inst->CustPos, Inst->Samp[t]->Demand);
   if( Inst->CPXStatus ) { PrintErr(); }
}

/* Build the original problem formulation */
void BuildCPXModel(InstStruct* Inst)
{
   /* Open Env and Lp */
   Inst->CPXEnv = CPXopenCPLEX(&(Inst->CPXStatus));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   Inst->CPXLp = CPXcreateprob(Inst->CPXEnv, &(Inst->CPXStatus), "CPX  formulation");
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Create and add variables to the  problem */
   int NumVar = Inst->NumReco + Inst->NumSamp * Inst->NumCust + Inst->NumSamp * Inst->NumEdge;
   double* Obj = new double[NumVar]; /* Objective Coefficients */
   double* Lb = new double[NumVar]; /* Variables low bounds */
   double* Ub = new double[NumVar]; /* Variables upper bounds */
   char* TypeVar = new char[NumVar]; /* Variables types */
   char** NameVar = new char*[NumVar]; /* Variables names */

   for( int i = 0; i < NumVar; i++ )
      NameVar[i] = new char[100];

   /* ( Col - 1 ) Create supplier variables */
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

   /* ( Col - 2 ) Create customer variables */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int j = 0; j < Inst->NumCust; j++ )
      {
         Obj[counter] = Inst->Samp[t]->Cost[j];
         Lb[counter] = 0.0;
         Ub[counter] = 1e+15;
         TypeVar[counter] = 'C';
         sprintf(NameVar[counter], "loss-%d-%d", t, j);
         counter++;
      }
   }
   if( counter != Inst->NumReco + Inst->NumSamp * Inst->NumCust ) { PrintErr(); }

   /* ( Col - 3 ) Create flow variables */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustStruct* cust = Inst->Cust[i];
         for( int j = 0; j < cust->degree; j++ )
         {
            Obj[counter] = 0.0;
            Lb[counter] = 0.0;
            Ub[counter] = 1e+15;
            TypeVar[counter] = 'C';
            sprintf(NameVar[counter], "flow-%d-%d-%d", t, i, cust->Edge[j]->supp->id);
            counter++;
         }
      }
   }
   if( counter != NumVar ) { PrintErr(); }

   /* Add variables information to CPX */
   Inst->CPXStatus = CPXnewcols(Inst->CPXEnv, Inst->CPXLp, NumVar, Obj, Lb, Ub, TypeVar, NameVar);
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

   CPXchgobjsen(Inst->CPXEnv, Inst->CPXLp, CPX_MIN); /* Set optimization direction to be "MIN" */

   /* Add the constraints of the  problem */
   int* ConsInd = new int[max(Inst->NumEdge, Inst->NumReco) + 1];
   double* ConsVal = new double[max(Inst->NumEdge, Inst->NumReco) + 1];
   char sense;
   int rbegin = 0;
   /* ( Row - 1 ) Creat the Cardinality (budget) constraint */
   sense = 'L'; /* Set the constraint "<=" */
   counter = 0;
   for( int j = 0; j < Inst->NumSupp; j++ )
   {
      if( IsEq(Inst->Supp[j]->working, 0.0) )
      {
         ConsVal[counter] = 1;
         ConsInd[counter] = counter;
         counter++;
      }
   }
   double rhs = double(Inst->Budget);
   Inst->CPXStatus=CPXaddrows(Inst->CPXEnv, Inst->CPXLp, 0, 1, Inst->NumReco, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* ( Row - 2 ) Creat the demand constraints */
   sense = 'E'; /* Set the constraint "==" */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustStruct* cust = Inst->Cust[i];
         for( int j = 0; j < cust->degree; j++ )
         {
            EdgeStruct* edge = cust->Edge[j];
            ConsVal[j] = 1;
            ConsInd[j] = Inst->NumReco + Inst->NumSamp*Inst->NumCust + t*Inst->NumEdge + edge->id;
         }
         ConsVal[cust->degree] = 1;
         ConsInd[cust->degree] = Inst->NumReco + t*Inst->NumCust + i;
         rhs = Inst->Samp[t]->Demand[i];
         Inst->CPXStatus=CPXaddrows(Inst->CPXEnv, Inst->CPXLp, 0, 1, cust->degree+1, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
         if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
      }
   }
   /* ( Row - 3 ) Creat the Capacity constraint */
   sense = 'L'; /* Set the constraint "<=" */
   for( int t = 0; t < Inst->NumSamp; t++ )
   {
      for( int j = 0; j < Inst->NumSupp; j++ )
      {
         SuppStruct* supp = Inst->Supp[j];
         for( int i = 0; i < supp->degree; i++ )
         {
            EdgeStruct* edge = supp->Edge[i];
            ConsVal[i] = 1;
            ConsInd[i] = Inst->NumReco + Inst->NumSamp*Inst->NumCust + t*Inst->NumEdge + edge->id;
         }
         if( IsEq(supp->working, 1.0) && supp->alive[t] )
         {
            rhs = supp->bandwidth;
            Inst->CPXStatus=CPXaddrows(Inst->CPXEnv, Inst->CPXLp, 0, 1, supp->degree, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
            if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         }
         else if( IsEq(supp->working, 0.0) && supp->alive[t] )
         {
            rhs = 0.0;
            ConsVal[supp->degree] = -supp->bandwidth;
            ConsInd[supp->degree] = supp->pos;
            Inst->CPXStatus=CPXaddrows(Inst->CPXEnv, Inst->CPXLp, 0, 1, supp->degree+1, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
            if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         }
         else
         {
            rhs = 0.0;
            Inst->CPXStatus=CPXaddrows(Inst->CPXEnv, Inst->CPXLp, 0, 1, supp->degree, &rhs, &sense, &rbegin, ConsInd, ConsVal, NULL, NULL);
            if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
         }
      }
   }

   delete []ConsVal;
   delete []ConsInd;
}

void ChangeCPXModel(InstStruct* Inst)
{
   vector<SuppStruct*> prior(Inst->NumReco);
   for( int i = 0; i < Inst->NumReco; i++ )
      prior[i] = Inst->Reco[i];
   if( Inst->SolvingSetting == -1 )
   {
      mt19937 g(0);
      shuffle(prior.begin(), prior.end(), g);
   }
   else if( Inst->SolvingSetting == -2 )
      sort(prior.begin(), prior.end(), [](SuppStruct* a, SuppStruct* b) { return a->bandwidth > b->bandwidth; });
   else if( Inst->SolvingSetting == -3 )
      sort(prior.begin(), prior.end(), [](SuppStruct* a, SuppStruct* b) { return a->degree > b->degree; });
   else if( Inst->SolvingSetting == -4 )
      sort(prior.begin(), prior.end(), [](SuppStruct* a, SuppStruct* b) { return a->liveprob > b->liveprob; });
   char sign = 'B';
   double value = 1.0;
   for( int i = 0; i < min(Inst->Budget, Inst->NumReco); i++ )
   {
      CPXchgbds(Inst->CPXEnv, Inst->CPXLp, 1, &prior[i]->pos, &sign, &value);
   }
}

void SolveCPXModel(InstStruct* Inst)
{
   /* Output information to the screen */
   CPXsetintparam(Inst->CPXEnv, CPX_PARAM_SCRIND, CPX_ON);

   /* Set the random seed during the Branch-and-Bound process in CPX */
   if (Inst->CPXSeed != -1)
   {
      Inst->CPXStatus = CPXsetintparam(Inst->CPXEnv, CPX_PARAM_RANDOMSEED, Inst->CPXSeed);
      if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   }

   /* Set the number of Threads in the calculation */
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnv, CPX_PARAM_THREADS, Inst->NumThread);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

#if 0
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnv, CPXPARAM_MIP_Cuts_Implied, -1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnv, CPXPARAM_MIP_Cuts_FlowCovers, -1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus = CPXsetintparam(Inst->CPXEnv, CPXPARAM_MIP_Cuts_MIRCut, -1);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
#endif

   /* Set the timelimit of the CPX (i.e., total timelimit minus the time cost in presolving) */
   cout << "CPX Solving Time Limit: " << Inst->TimeLimit << endl;
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnv, CPXPARAM_TimeLimit, Inst->TimeLimit);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set the absolute tolerance of CPX to 0.0 */
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnv, CPX_PARAM_EPAGAP, 0.0);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Set the relative tolerance of CPX to 0.0 */
   Inst->CPXStatus = CPXsetdblparam(Inst->CPXEnv, CPX_PARAM_EPGAP, 0.0);

   /* CPX optimization process */
   clock_t TimeStart=clock();
   Inst->CPXStatus = CPXmipopt(Inst->CPXEnv, Inst->CPXLp);
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   clock_t TimeEnd=clock();
   Inst->CPXSolvingTime = (double)(TimeEnd - TimeStart)/(double)CLOCKS_PER_SEC;

   /* Get the best feasible objective value within timelimit*/
   Inst->CPXStatus=CPXgetmipobjval(Inst->CPXEnv, Inst->CPXLp, &(Inst->OptObj));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   /* Get the best relaxed objective value within timelimit*/
   Inst->CPXStatus=CPXgetbestobjval(Inst->CPXEnv, Inst->CPXLp, &(Inst->RelaxObj));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }

   Inst->NumNode = CPXgetnodecnt(Inst->CPXEnv, Inst->CPXLp); /* Get the number of branch-and-bound nodes */
}


/* read parameters from a string */
void ReadParam(InstStruct* Inst, char* input)
{
   char* name;
   char* value;
   char* setfile;
   char* token;
   setfile = NULL;
   token = strtok(input, " ");
   while( token != NULL )
   {
      char *equalsign = strchr(token, '=');
      if( equalsign != NULL )
      {
         *equalsign = '\0';
         name = token;
         value = equalsign + 1;
         /* set parameter values */
         if( !strcmp(name, "MODE") ) /* Solving settings (1 for (Cplex), 2 for (Benders) or 3 for (Benders + Initial Cuts)) */
            Inst->SolvingSetting = atoi(value);
         else if( !strcmp(name, "CUST") ) /* Number of customers */
            Inst->NumCust = atoi(value);
         else if( !strcmp(name, "SUPP") ) /* Number of suppliers */
            Inst->NumSupp = atoi(value);
         else if( !strcmp(name, "BUDGET") ) /* Budget (i.e. the Cardinality restriction) */
            Inst->Budget = atoi(value);
         else if( !strcmp(name, "SAMP") ) /* Number of the Monte-Carlo simulations */
            Inst->NumSamp = atoi(value);
         else if( !strcmp(name, "QOS") ) /* Limit of the quality of service */
            Inst->QoS = atoi(value);
         else if( !strcmp(name, "TIME") ) /* Time limitation (s) */
            Inst->TimeLimit = atof(value);
         else if( !strcmp(name, "MEM") ) /* Memory limitation (MB) */
            Inst->MemLimit = atoi(value);
         else if( !strcmp(name, "SEED") ) /* Random seed of sampling */
            Inst->ProduceSeed = atoi(value);
         else if( !strcmp(name, "MINGAP") ) /* Minimal gap */
            Inst->MinGap = atof(value);
         else if( !strcmp(name, "TYPE") )
            Inst->DisasterType = atoi(value);
         else if( !strcmp(name, "PROB1") )
            Inst->Prob1 = atof(value);
         else if( !strcmp(name, "PROB2") )
            Inst->Prob2 = atof(value);
         else if( !strcmp(name, "CASC") )
            Inst->isCasc = atoi(value);
         else if( !strcmp(name, "SETTING") )
         {
            if( CheckEnds(value) )
            {
               setfile = new char[200];
               strcpy(setfile, value);
            }
            else
               printf("unidentified settings file - %s\n", value);
         }
         else
            printf("undefined parameter - %s\n", name);
      }
      token = strtok(NULL, " ");
   }
   if( setfile != NULL )
   {
      ReadFile(Inst, setfile);
      delete []setfile;
   }
   Inst->NumRSamp = max(Inst->NumRSamp, Inst->NumSamp);
}

/* read parameters from a file */
void ReadFile(InstStruct* Inst, char* paramfile)
{
   char* name;
   char* value;
   FILE* file = fopen(paramfile, "r");
   if( file != NULL )
   {
      char line[200];
      while( fgets(line, sizeof(line), file) )
      {
         char* delimiter = strchr(line, '=');
         if( delimiter != NULL )
         {
            *delimiter = '\0';
            /* parameter name */
            name = line;
            DeleteSpace(name);
            /* parameter value */
            value = delimiter + 1;
            DeleteSpace(value);
            /* set parameter values */
            if( !strcmp(name, "MODE") ) /* Solving settings (1 for (Cplex), 2 for (Benders) or 3 for (Benders + Initial Cuts)) */
               Inst->SolvingSetting = atoi(value);
            else if( !strcmp(name, "CUST") ) /* Number of customers */
               Inst->NumCust = atoi(value);
            else if( !strcmp(name, "SUPP") ) /* Number of suppliers */
               Inst->NumSupp = atoi(value);
            else if( !strcmp(name, "BUDGET") ) /* Budget (i.e. the Cardinality restriction) */
               Inst->Budget = atoi(value);
            else if( !strcmp(name, "SAMP") ) /* Number of the Monte-Carlo simulations */
               Inst->NumSamp = atoi(value);
            else if( !strcmp(name, "QOS") ) /* Limit of the quality of service */
               Inst->QoS = atoi(value);
            else if( !strcmp(name, "TIME") ) /* Time limitation (s) */
               Inst->TimeLimit = atof(value);
            else if( !strcmp(name, "MEM") ) /* Memory limitation (MB) */
               Inst->MemLimit = atoi(value);
            else if( !strcmp(name, "SEED") ) /* Random seed of sampling */
               Inst->ProduceSeed = atoi(value);
            else if( !strcmp(name, "MINGAP") ) /* Minimal gap */
               Inst->MinGap = atof(value);
            else if( !strcmp(name, "TYPE") )
               Inst->DisasterType = atoi(value);
            else if( !strcmp(name, "PROB1") )
               Inst->Prob1 = atof(value);
            else if( !strcmp(name, "PROB2") )
               Inst->Prob2 = atof(value);
            else if( !strcmp(name, "CASC") )
               Inst->isCasc = atoi(value);
            else
               printf("undefined parameter - %s\n", name);
         }
      }
      fclose(file);
   }
   else
   {
      printf("parameter file not found - using default parameters\n");
   }
}

/* check whether the input parameter is a .set file */
bool CheckEnds(char* input)
{
   size_t length = strlen(input);
   if( length < strlen(".set") )
      return false;
   char* ends = input + (length - strlen(".set"));
   if( !strcmp(ends, ".set") )
      return true;
   else
      return false;
}

/* remove space at the head and tail of the string */
void DeleteSpace(char* str)
{
   char *start = str;
   char *end = str + strlen(str) - 1;

   while( *start == ' ' )
      start++;

   while( end > start && *end == ' ' )
      end--;

   /* add the end flag */
   *(end + 1) = '\0';

   /* change the starting position */
   memmove(str, start, strlen(start) + 1);
}

/* Free and close CPX environment */
void CleanCPXModel(InstStruct* Inst)
{
   /* Clena  problem */
   Inst->CPXStatus=CPXfreeprob(Inst->CPXEnv, &(Inst->CPXLp));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
   Inst->CPXStatus=CPXcloseCPLEX(&(Inst->CPXEnv));
   if( Inst->CPXStatus ) { PrintErr(); cout << "CPXStatus:" << Inst->CPXStatus << endl; }
}

void FreeMemory(InstStruct* Inst)
{
   delete []Inst->CustPos;
   for( int i = 0; i < Inst->NumCust; i++ )
      delete Inst->Cust[i];
   for( int j = 0; j < Inst->NumSupp; j++ )
      delete Inst->Supp[j];
   for( int e = 0; e < Inst->NumEdge; e++ )
      delete Inst->Edge[e];
   for( int t = 0; t < Inst->NumRSamp; t++ )
   {
      delete []Inst->Samp[t]->Cost;
      delete []Inst->Samp[t]->Demand;
      delete []Inst->Samp[t]->Sum;
      delete Inst->Samp[t];
   }
}
