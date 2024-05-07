#pragma ident "$Id$"
/**
* this part added by ragn wang 2023/03/03 for detect the cycle slip
* based on GF combination.
*/


#include"DetectCSGF.hpp"


#define debug 0
using namespace gpstk;

namespace fippp
{
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::string DetectCSGF::getClassName() const
	{
		return "DetecCSGF";
	}
	/// <summary>
	/// process the data and return the data
	/// </summary>
	/// <param name="epoch"> current epoch </param>
	/// <param name="gData"> gnss data </param>
	/// <param name="epochflag"> current epoch flag </param>
	/// <returns>gnss Data</returns>
	satTypeValueMap& DetectCSGF::Process(const CommonTime& epoch,
		const CommonTime& beginEpoch,
		satTypeValueMap& gData, const short& epochflag)
		noexcept(false)
	{
		//
		try
		{

			double value(0.0);
			SatIDSet satRejectedSet;
			double elv(0.0);	// the elevation of satllite
			/* loop through all the data in the current epoch */
			for (satTypeValueMap::iterator it = gData.begin(); 
				it != gData.end(); 
				++it)
			{
				SatID sat = (*it).first;
				SatelliteSystem sys = sat.system;
				elv = (*it).second[TypeID::elevation];
				/* get gf type for current system */
				TypeIDSet gfTypeSet = sysTypeSet[sys];

				for(auto typeIt = gfTypeSet.begin();
					typeIt != gfTypeSet.end(); typeIt++)
				{
					TypeID gfType = (*typeIt);
					try
					{
						value = (*it).second(gfType);
					}
					catch (...)
					{
						continue;
					}

					/* detect the cycle slip by gf combination */
					double csFlag = getDetection(epoch, beginEpoch, sat, epochflag, gfType, value, elv);
					if ((*it).second.find(TypeID::CSFlag) != (*it).second.end())
					{
						double lastCSFlag = (*it).second[TypeID::CSFlag];
						(*it).second[TypeID::CSFlag] = lastCSFlag || csFlag;
					}
					else
					{
						(*it).second[TypeID::CSFlag] = csFlag;
					}
				}
			}

			return gData;
		}
		catch (const std::exception& u)
		{
			ProcessingException e(getClassName() + ":" + u.what());
			GPSTK_THROW(e);
		}		
	};


	double DetectCSGF::getDetection(const CommonTime& epoch,
		const CommonTime& beginEpoch,
		const SatID& sat,
		const short& epochFlag,
		const TypeID& gfType,
		const double& gf,
		const double& elv) 
	{

		bool reportCS(false);
		// difference between current and former epoch, in sec
		double deltaEpoch(0.0);
		
		// difference between current and former gf values in m
		double deltaGF(0.0);
		
		// Threshold parameter of GF
		double bGF(0.0), tGF(0.0);

		// Get the difference between current epoch and former epoch,
		// in seconds
		deltaEpoch = (epoch - gfData[sat][gfType].formerEpoch);
		
		if (debug)
		{
			cout << "epoch:" << YDSTime(epoch) << endl;
			cout << "sat:" << sat << endl;
			cout << "deltaEpoch:" << deltaEpoch << endl;
		}

		// Store if first epoch
		if (epoch == beginEpoch)
		{
			gfData[sat][gfType].formerEpoch = epoch;
			gfData[sat][gfType].gf = gf;
			return 0.0;
		}
		//difference between current value of gf and 
		deltaGF = std::abs(gf - gfData[sat][gfType].gf);

		// store current epoch as former epoch
		gfData[sat][gfType].formerEpoch = epoch;
		gfData[sat][gfType].gf = gf;

		/**
		* cycle-slip
		* if deltaGF is greater than the threshold, return 1.0
		*/
		// in rtklib: bGF = 0.05
		// in GAMP:
		if (deltaEpoch > 0 && deltaEpoch <= 1)			bGF = 0.05;
		else if (deltaEpoch > 1  && deltaEpoch <= 20)	bGF = 0.1 / 20.0 * deltaEpoch + 0.05;
		else if (deltaEpoch > 20 && deltaEpoch <= 60)	bGF = 0.15;
		else if (deltaEpoch > 60 && deltaEpoch <= 100)	bGF = 0.25;
		else											bGF = 0.35;

		if (elv <= 15)	tGF = -bGF / 15.0 * elv * bGF + 2 * bGF;
		else tGF = bGF;

		if (deltaGF > tGF)
		{
			if (debug)
			{
				std::cout << "GF-CS happended!" << std::endl;
			}
			return 1.0;
		}

		return 0.0;
	}
} // End of namespace gpstk