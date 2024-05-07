// This is used to remove the satellite.

#include "EraseSat.hpp"

using namespace gpstk;
using namespace gpstk::StringUtils;
namespace fippp
{
	void EraseSat::satIDsfromPrns()
		noexcept(false)
	{
		std::vector<std::string> prns;
		stringSplit(exPrns, prns, '-');
		for (auto itPrn = prns.begin();
			itPrn != prns.end();
			++itPrn)
		{
			std::string prn(*itPrn);
			char sys(*prn.substr(0, 1).c_str());
			int id(asInt(prn.substr(1, 2)));
			SatID sat(id, asSatelliteSystem(sys));
			exSats.insert(sat);
		}
	}

	satTypeValueMap& EraseSat::eraseBDS2(satTypeValueMap& gData)
		noexcept(false)
	{
		SatIDSet satRejected;
		// loop through the all satellite
		for(auto it_data = gData.begin();
			it_data != gData.end();
			it_data++)
		{
			SatID sat(it_data->first);
			SatelliteSystem sys(sat.system);
			int prn(sat.id);

			if (sys == SatelliteSystem::BeiDou && prn < 19)
			{
				satRejected.insert(sat);
			}

		} // End of for loop through all satellite

		gData.removeSatID(satRejected);

		return gData;
	}
}