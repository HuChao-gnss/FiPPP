#pragma ident "$Id$"
/**
* this part added by ragn wang 2023/03/03 for detect the cycle slip 
* based on GF combination.
*/

#ifndef DetectCSGF_HPP
#define DetectCSGF_HPP


#include <list>
#include "DataStructures.hpp"
#include "gnssData.hpp"
#include "LinearCombinations.hpp"
#include "SatelliteSystem.hpp"

using namespace gpstk;

namespace fippp
{
	
	class DetectCSGF
	{
	public:
		/* default construction */
		DetectCSGF() {
			sysTypeSet.clear();
		};

		//
		//
		virtual satTypeValueMap & Process(const CommonTime& epoch,
			const CommonTime& beginEpoch,
			satTypeValueMap& gData,
			const short& epochflag = 0)
			noexcept(false);
		
		//
		virtual gnssRinex& Process(gnssRinex& gRin)
		{
			Process(gRin.header.epoch, gRin.header.beginEpoch, gRin.body);
			return gRin;
		};

		//
		virtual DetectCSGF& addType(const SatelliteSystem& sys,
			const TypeID& type) 
		{
			sysTypeSet[sys].insert(type);
			return (*this);
		}

		virtual DetectCSGF& addLinear(const SatelliteSystem& sys,
			const gnssLinearCombination& linear)
		{
			systemCombs[sys].push_back(linear);
			return (*this);
		}

		//
		virtual std::string getClassName(void) const;

		//
		virtual ~DetectCSGF() {};


	private:
		
		//
		std::map<SatelliteSystem, TypeIDSet> sysTypeSet;
		// 
		std::map<SatelliteSystem, LinearCombList> systemCombs;

		//
		struct filterData
		{
			filterData() : formerEpoch(CommonTime::BEGINNING_OF_TIME),
				windowSize(0), gf(0.0)
			{};

			CommonTime formerEpoch;	// the previous epoch time
			int windowSize;			// In order to record the number of consecutive epochs without cycle slip 
			double gf;				// the value of gf
		};

		// set up gfdata for the more frequency storing the data
		typedef std::map<SatID, std::map<TypeID, filterData>> GFData;
		// define the current epoch gf data
		//typedef std::map<SatID, std::map<TypeID, double>> cGFData;
		//
		GFData gfData;
		//
		virtual double getDetection(const CommonTime& epoch,
			const CommonTime& beginEpoch,
			const SatID& sat,
			const short& epochFlag,
			const TypeID& gfType,
			const double& gf,
			const double& elv);


	}; // End of class 'DetectCSGF'
}	// End of namespace gpstk

#endif // !DetectCSGF_HPP