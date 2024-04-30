#include "Functions.h"
#include "Variables.h"
#include "Benders.h"

int main(int argc, char** argv)
{

   InstStruct Inst;
   Inst.ProduceSeed = 1; /* Random seed of sampling */

   if( argc > 2 )
   {
      cout<<"Error input!"<<endl;
      exit(-1);
   }

   ReadParam(&Inst, argv[1]);

   cout << endl << endl << "Input parameters information:" << endl;
   if(Inst.SolvingSetting == -1)
      cout << "Solving setting:\t\t\t\t" << "Heur-1 (Random repair)" << endl;
   else if(Inst.SolvingSetting == -2)
      cout << "Solving setting:\t\t\t\t" << "Heur-2 (Capacity-based repair)" << endl;
   else if(Inst.SolvingSetting == -3)
      cout << "Solving setting:\t\t\t\t" << "Heur-3 (Degree-based repair)" << endl;
   else if(Inst.SolvingSetting == -4)
      cout << "Solving setting:\t\t\t\t" << "Heur-4 (Risk-based repair)" << endl;
   else if(Inst.SolvingSetting == 1)
      cout << "Solving setting:\t\t\t\t" << "1 (CPX solver)" << endl;
   else if(Inst.SolvingSetting == 2)
      cout << "Solving setting:\t\t\t\t" << "2 (Benders framework)" << endl;
   else if(Inst.SolvingSetting == 3)
      cout << "Solving setting:\t\t\t\t" << "3 (Benders framework plus initial cuts)" << endl;
   else if(Inst.SolvingSetting == 4)
      cout << "Solving setting:\t\t\t\t" << "4 (United Benders framework plus initial cuts)" << endl;
   else
   {
      cout << "Error input!" << endl;
      exit(-1);
   }
   cout << "Number of Customers:\t\t\t\t" << Inst.NumCust << endl;
   cout << "Number of Suppliers:\t\t\t\t" << Inst.NumSupp << endl;
   cout << "Budget (Cardinality restriction):\t\t" << Inst.Budget << endl;
   cout << "Disaster Type:\t\t\t" << Inst.DisasterType << endl;
   cout << "Quality of Service:\t\t\t\t" << Inst.QoS << endl;
   cout << "Time limitation:\t\t\t\t" << Inst.TimeLimit <<endl;
   cout << "Number of samplings:\t\t\t\t" << Inst.NumSamp << endl;
   cout << "Random seed of graph generating:\t\t" << Inst.ProduceSeed << endl << endl << endl;





   /* Generate graph */
   clock_t GenerateTimeStart = clock();
   cout << "Generate graph start......" <<endl;
   GenerateGraph(&Inst);

   /* Build the live-arc graphs and implementing presolving methods */
   Sample(&Inst);
   clock_t GenerateTimeEnd = clock();
   double GenerateTime=(double)(GenerateTimeEnd-GenerateTimeStart)/(double)CLOCKS_PER_SEC;
   cout << "Generate graph end. (time: " << GenerateTime << " s)" << endl << endl;

   if( Inst.SolvingSetting <= 1 )
   {
      BuildCPXModel(&Inst);
      if( Inst.SolvingSetting < 0 )
         ChangeCPXModel(&Inst);
      //CPXwriteprob(Inst.CPXEnv, Inst.CPXLp, "test.lp", NULL);
      SolveCPXModel(&Inst);
      CleanCPXModel(&Inst);
   }
   else if( Inst.SolvingSetting >= 2 )
   {
      /* Solve the NetRec under BD framework */
      cout << endl << "Solve the network recovery problem under Benders decomposition framework......" << endl << endl;

      /* Build Benders formulation */
      clock_t BuildTimeStart = clock();
      cout << "Build CPX formulation start......" <<endl;
      BuildSubModel(&Inst);
      BuildMasterModel(&Inst);
      clock_t BuildTimeEnd = clock();
      Inst.BuildTime=(double)(BuildTimeEnd - BuildTimeStart)/(double)CLOCKS_PER_SEC;
      cout << "Build CPX formulation end. (time: "<< Inst.BuildTime << " s)" << endl << endl;

      /* Solve */
      SolveMasterModel(&Inst);

      /* Free and close CPX environment */
      CleanBendersModel(&Inst);
   }

   /* Free the memory allocated */
   FreeMemory(&Inst);
   cout << endl << "Finish." << endl;
}
