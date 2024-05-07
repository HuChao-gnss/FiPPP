#pragma ident "$id: chao hu, ragnwang*$"

#include "ComputeResidual.hpp"

namespace fippp{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="gData"></param>
	void ComputeResidual::Process(const gnssRinexHeader& gHeader, satTypeValueMap& gData)
		noexcept(false)
	{
		//if (gHeader.epoch == gHeader.beginEpoch)
		//	return;
		//for loop through gData
		for (auto itData = gData.begin();
			itData != gData.end();
			++itData)
		{
			SatID sat(itData->first);
			LinearCombList sysLists(systemCombs[sat.system]);
			// loop throgh syslist
			for (auto itType = sysLists.begin();
				itType != sysLists.end();
				++itType)
			{
				TypeID prefitType(itType->header);
				// check
				if (itData->second.find(prefitType) == itData->second.end())
					continue;
				// loop body
				for (auto itTypeBody = itType->body.begin();
					itTypeBody != itType->body.end();
					++itTypeBody)
				{
					TypeID bodyType(itTypeBody->first);
					// get value
					double temp(0.);
					try
					{
						if(gHeader.epoch==gHeader.beginEpoch)
						{
							std::map<TypeID, double> dcdtr = gHeader.dcdtr;
							if (dcdtr.find(bodyType) != dcdtr.end())
								temp = dcdtr[bodyType];
						}
						else
						{
							if (bodyType.asString().substr(0, 1) == "B")
								temp = 0.;
							else
								temp = getSolution(bodyType);
						}
						
					}
					catch (...)
					{
						temp = 0.;
					}
					// compute
					itData->second[prefitType] += temp * (*itTypeBody).second;
				}	//End of loop itType body

			}	// End of for loop through sysLists
		} // End of for loop itData
	}	// End of process

	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	double ComputeResidual::getSolution(const TypeID& type)
		noexcept(false)
	{
		auto varIt = unkSet.begin();
		for (varIt;
			varIt != unkSet.end();
			++varIt)
		{
			if (varIt->getType() == type)
				return sols((*varIt).getNowIndex());
		}
		InvalidRequest e(type.asString() + "not found in sols vector.");
		GPSTK_THROW(e);
	}

	double ComputeResidual::getSolution(const TypeID& type,
		const SatID& sat)
		noexcept(false)
	{
		auto varIt = unkSet.begin();
		for (varIt;
			varIt != unkSet.end();
			++varIt)
		{
			if (varIt->getSatellite() == sat && varIt->getType() == type)
				return sols((*varIt).getNowIndex());
		}
		InvalidRequest e(type.asString() + "not found in sols vector.");
		GPSTK_THROW(e);
	}
}	// End of namespace fippp