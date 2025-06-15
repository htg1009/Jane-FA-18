#ifndef __ENGINE_H__
#define __ENGINE_H__

float GetCommandedThrust(PlaneParams *P, float ThrustPercent);
void SetEngineWarnings(long TurnOffFlags, long TurnOnFlags, long WarningFlags);
void LeftEngineOutEffects(int On);
void RightEngineOutEffects(int On);
void ShutLeftEngineDown(void);
void ShutRightEngineDown(void);
void ShutBothEnginesDown(void);
void ToggleLeftEngineStatus(void);
void ToggleRightEngineStatus(void);
float GetFuelFlow(PlaneParams *P, float ThrottlePercent);
float CalcFuelFlow(PlaneParams *P,float ThrottlePercent,float Altitude,int UseMinMach);
float GetSingleEngineFuelFlowAndThrottlePos(PlaneParams *P, int EngineSpooling,float ThrottlePercent,float *ThrottlePos,float CommandedThrottlePos,float *CurFuelFlow,float *GoalFuelFlow,float *NumStepsToThrottleGoal,float Altitude,int *LastTimeFuelFlowCalc);

extern float EMD_lTemp;
extern float EMD_rTemp;

extern float EMD_Amad_lTemp;
extern float EMD_Amad_rTemp;

void CalcTemp(PlaneParams *P,float NumStepsToThrottleGoal);

extern float EMD_lOlio;
extern float EMD_rOlio;

extern float EMD_lOpen;
extern float EMD_rOpen;


void CalcPsi(PlaneParams *P,float NumStepsToThrottleGoal);

extern float EMD_lSpin;
extern float EMD_rSpin;

void CalcRPMs(PlaneParams *P,float NumStepsToLeftThrottleGoal,float NumStepsToRightThrottleGoal);
void ConsumeFuel(PlaneParams *P, unsigned int DeltaTicks);

extern void StartEngineSounds();
extern void StopEngineSounds();
extern int g_iBaseBettySoundLevel;
extern BOOL g_bBingoFuel;

void CalcFuelBurn(float FuelAmount, float *Endurance, float *DistInMiles);


#endif