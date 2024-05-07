#pragma ident "$ID: ragn wang 2023-03-15 $"

//
// 
//

#include "CorrectIFCB.hpp"

using namespace gpstk;

namespace fippp
{
	/// <summary>
	/// return the class name
	/// </summary>
	/// <returns></returns>
	std::string CorrectIFCB::getClassName() const
	{
		return "CorrectIFCB";
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="satTypes"></param>
	/// <param name="gData"></param>
	/// <returns></returns>
	satTypeValueMap& CorrectIFCB::Process(const CommonTime& time,
		std::map<RinexSatID, TypeIDVec>& satShortTypes, satTypeValueMap& gData)
		noexcept(false)
	{

		// loop through the all satellite
		for (auto it_data = gData.begin();
			it_data != gData.end();
			it_data++)
		{
			RinexSatID sat(it_data->first);
			SatelliteSystem sys(sat.system);
			if (sys != SatelliteSystem::GPS)	continue;

			TypeIDVec satTypes = satShortTypes[sat];
			auto it_type = std::find(satTypes.begin(), satTypes.end(), TypeID::L5G);
			// Skip if there is no tertiary frequency observation for this satellite
			if (it_type == satTypes.end())	continue;
			double pr((*it_data).second(*it_type));
			// Values can be added directly to the IF13 equation
			double ifcb(getIFCBValue(time, pr, sat));
			// this can be added directly to the L5G equation.
			it_data->second[TypeID::L5G] += ifcb / BETA_GPS_L1L5;
		}	//END of for loop through all satellite data

		return gData;
	}


	double CorrectIFCB::getIFCBValue(const CommonTime& time, const double& pr, const RinexSatID sat)
	{
		double ifcb;
		int nhalf(2);
		CommonTime tt(time - pr / C_MPS);
		try
		{
			if (pIFCBStore != NULL)
			{
				// Values can be added directly to the IF13 equation
				ifcb = pIFCBStore->getSatIFCB(sat, tt, nhalf);
			}
			else
			{
				cerr << getClassName() << "pIFCBStore should be given" << std::endl;
				exit(-1);
			}
		}
		catch (Exception& e)
		{
			GPSTK_THROW(e);
		}

		return ifcb;
	}




}