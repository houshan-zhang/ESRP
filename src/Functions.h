#ifndef __FUNCTIONS_HEADER__
#define __FUNCTIONS_HEADER__

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "Variables.h"

using namespace std;

bool IsEq( double a, double b );

void GenerateGraph(InstStruct* Inst);

void ComputeResult(InstStruct* Inst);

void Sample(InstStruct* Inst);

void SetSupplier(InstStruct* Inst, vector<SuppStruct*> Supp, double sign);

void SetSupplierStatus(InstStruct* Inst, vector<SuppStruct*> Supp, int t);

void ChgCPXCoef(InstStruct* Inst, int t);

void BuildCPXModel(InstStruct* Inst);

void ChangeCPXModel(InstStruct* Inst);

void SolveCPXModel(InstStruct* Inst);

void ReadParam(InstStruct* Inst, char* input);

void ReadFile(InstStruct* Inst, char* paramfile);

bool CheckEnds(char* input);

void DeleteSpace(char* str);

void CleanCPXModel(InstStruct *Inst);

void FreeMemory(InstStruct* Inst);

#endif
