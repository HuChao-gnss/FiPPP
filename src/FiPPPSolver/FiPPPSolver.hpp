#pragma ident "$ chaohu, ragnwang* $"
/*
* Author: Chao Hu, Ragn Wang*
*/
#include <iostream>
#include <direct.h>

#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"

#include "GPSEphemerisStore.hpp"

#include "SaasTropModel.hpp"
#include "BasicFramework.hpp"

#include "GNSSconstants.hpp"

#include "YDSTime.hpp"

#include "ConfigReader.hpp"
#include "LeapSecStore.hpp"
#include "SolarSystem.hpp"
#include "AntexReader.hpp"
#include "BLQDataReader.hpp"
#include "Rinex3EphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "EOPDataStore.hpp"
#include "BiasDataReader.hpp"
#include "XYZStore.hpp"
#include "UPDDataReader.hpp"
#include "ReferenceSystem.hpp"
#include "DecimateData.hpp"
#include "KeepSystems.hpp"
#include "FilterCode.hpp"
#include "ChooseOptimalObs.hpp"
#include "CorrectCodeBiases.hpp"
#include "ConvertObs.hpp"
#include "RequiredObs.hpp"
#include "LinearCombinations.hpp"
#include "ComputeCombination.hpp"
#include "DetectCSMW.hpp"
#include "DetectCSGF.hpp"
#include "MarkArc.hpp"
#include "ComputeSatPos.hpp"
#include "ComputeDerivative.hpp"
#include "ComputeTropModel.hpp"
#include "GravitationalDelay.hpp"
#include "ComputeSatAttitude.hpp"
#include "EclipsedSatFilter.hpp"
#include "ComputeSatPCenter.hpp"
#include "ComputeWindUp.hpp"
#include "ComputeStaTides.hpp"
#include "CorrectRecBias.hpp"
#include "ComputeElevWeights.hpp"
#include "XYZ2ENU.hpp"
#include "DumpRinex.hpp"
#include "DumpTypeSatValueData.hpp"
#include "gnssData.hpp"
#include "Variable.hpp"

#include "FindFile.hpp"
#include "IFCBDataReader.hpp"
#include "CorrectIFCB.hpp"
#include "EraseSat.hpp"
#include "SolverPPPAll.hpp"
#include "CombinationOptions.hpp"
#include "SPP.hpp"
#include "AboutOptions.hpp"
#include "SolverPPPAR.hpp"
#include "ComputeResidual.hpp"
#include "Trace.hpp"


#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

#include "EstimateBias.h"
#ifdef __cplusplus
}
#endif // _cplusplus

using namespace std;
using namespace gpstk;
using namespace fippp;


// A new class is declared that will handle program behaviour
// This class inherits from BasicFramework
class positioning_algo : public BasicFramework
{
public:
	// Constructor declaration
	positioning_algo(char* arg0);


protected:

	// Method that will take care of processing
	virtual void process();

	// Method that hold code to be run BEFORE processing
	virtual void spinUp();

private:

	// These field represent options at command line interface (CLI)
	CommandOptionWithArg _configFile;

	Options opts;	// in order to put string
};