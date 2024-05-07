#pragma ident "$ID: ragn wang 2023-03-15 $"

#include "IFCBDataReader.hpp"
#include "StringUtils.hpp"
#include "MJD.hpp"
#include "TimeString.hpp"
#include "MiscMath.hpp"

using namespace gpstk;
using namespace gpstk::StringUtils;

#define debug 0
namespace fippp
{
	void IFCBDataReader::open(const char* fn)
	{    
		// We need to be sure current data stream is closed
		fstm.close();

		// Open data stream
		fstm.open(fn, std::ios::in);
		if (!fstm.is_open())
		{
			std::cerr << "The file " << fn << " doesn't exist!" << std::endl;
			exit(-1);
		}

		loadData();
		
	}

	void IFCBDataReader::open(const std::string& fn) 
	{
		// We need to be sure current data stream is closed
		fstm.close();

		// Open data stream
		fstm.open(fn.c_str(), std::ios::in);
		if (!fstm.is_open())
		{
			std::cerr << "The file " << fn << " doesn't exist!" << std::endl;
			exit(-1);
		}

		loadData();
	}

	void IFCBDataReader::loadData()
		noexcept(false)
	{
		try
		{
			std::string line;
			CommonTime epoch(CommonTime::BEGINNING_OF_TIME);
			while (!fstm.eof())
			{
				std::getline(fstm, line);
				if (line.substr(0, 3) == "EOF")
				{
					break;
				}
				std::string fC = line.substr(0, 1);
				// first char is "%" or "x" continue
				if (fC == "%" || fC == "x")	continue;
				if (line.substr(1, 10) == "EPOCH-TIME")
				{	// get epoch time
					long double mjd(asInt(line.substr(11, 8)));
					double sod(asDouble(line.substr(19, 10)));
					MJD mjdT(mjd,TimeSystem::GPS);
					epoch = mjdT.convertToCommonTime() + sod;
					continue;
				}
				if (line.substr(1, 1) == "G")
				{
					RinexSatID sat(line.substr(1, 3));
					double val(asDouble(line.substr(12, 10)));
					double std(asDouble(line.substr(22, 10)));
					ifcbData_t ifcb;
					ifcb.value = val;
					ifcb.std = std;
					satIfcbData[sat][epoch] = ifcb;
				}
			}	//END of while strm.eof
			fstm.close();
			// remove the single epoch stallite, because of not all GPS
			// satellites have ifcb value.
			checkIFCBData(satIfcbData);
		}
		catch (...)
		{
			fstm.close();
			return;
		}
	}

	double IFCBDataReader::getSatIFCB(const RinexSatID& sat, const CommonTime& ttag, const int nhalf)
	{
		double ifcb;
		try
		{
			static const char* fmt =
				" at time %F/%.3g %4Y/%02m/%02d %2H:%02M:%.3f %P";

			typename IFCBData::const_iterator it1, it2, kt;
			typename satIFCBData::const_iterator it;
			it = satIfcbData.find(sat);
			if (it == satIfcbData.end())	return 0.0;
			// sat data:
			const IFCBData& it_data(it->second);
			if (it_data.size() < 2)
			{
				InvalidRequest e("Inadequate data (size < 2) for satellite " +
					gpstk::StringUtils::asString(sat) +
					printTime(ttag, fmt));
				GPSTK_THROW(e);
			}
			// directly find:
			it1 = it_data.find(ttag);
			bool exactMatch(it1 != it_data.end());
			if (exactMatch)	return (*it1).second.value;
			// get the >= ttag value.
			it1 = it2 = it_data.lower_bound(ttag);
			if (it1 == it_data.end())
			{
				InvalidRequest e("No ifcb data in time range for satllite: " +
					gpstk::StringUtils::asString(sat) + printTime(ttag, fmt));
				GPSTK_THROW(e);
			}

			if (it1 == it_data.begin())
			{
				if (exactMatch && nhalf == 2)
				{
					++(it2 = it1);
				}
				InvalidRequest e("No ifcb data in before(1) time for satllite: " +
					gpstk::StringUtils::asString(sat) + printTime(ttag, fmt));
				GPSTK_THROW(e);
			}

			if (--it1 == it_data.begin())
			{
				if (nhalf == 1)
				{
					++(it2 = it1);
				}

				InvalidRequest e("No ifcb data in before(2) time for satllite: " +
					gpstk::StringUtils::asString(sat) + printTime(ttag, fmt));
				GPSTK_THROW(e);
			}


			for (int k = 0; k < nhalf-1; k++)
			{
				bool last(k == nhalf - 2);

				if (--it1 == it_data.begin() && !last)
				{
					InvalidRequest e("No ifcb data in before(3) time for satllite: " +
						gpstk::StringUtils::asString(sat) + printTime(ttag, fmt));
					GPSTK_THROW(e);
				}

				if (++it2 == it_data.end())
				{
					if (exactMatch && last && it1 != it_data.begin())
					{
						it2--;
						it1--;
					}
					else
					{
						InvalidRequest e("No ifcb data in after(3) time for satllite: " +
							gpstk::StringUtils::asString(sat) + printTime(ttag, fmt));
						GPSTK_THROW(e);
					}
				}
			}	// end for get 4 value aroud it1 and it2

			// pull data
			size_t n, Nlow(nhalf - 1), Nhi(nhalf), Nmatch(nhalf);
			CommonTime ttag0(it1->first);
			vector<double> times, ifcbVec;

			kt = it1; n = 0;
			while (true)
			{
				if (exactMatch && ABS(kt->first - ttag)<1.e-8)
				{
					Nmatch = n;
				}
				times.push_back(kt->first - ttag0);
				ifcbVec.push_back(kt->second.value);

				if (kt == it2) break;
				++kt;
				++n;
			}

			if (exactMatch && Nmatch == (int)(nhalf - 1)) { Nlow++; Nhi++; }
			double dt(ttag - ttag0), slope;
			// linear interpolation
			slope = (ifcbVec[Nhi] - ifcbVec[Nlow]) / (times[Nhi] - times[Nlow]);
			ifcb = ifcbVec[Nlow] + slope * (dt - times[Nlow]);
			return ifcb;
		}
		catch (Exception e)
		{
			std::cerr << e << std::endl;
			exit(-1);
		}
		return ifcb;
	}

	void IFCBDataReader::checkIFCBData(satIFCBData& ifcbData)
	{
		try
		{
			SatIDSet satRejected;
			for (auto it_data = ifcbData.begin();
				it_data != ifcbData.end();
				it_data++)
			{
				if ((*it_data).second.size() == 1)
				{
					satRejected.insert((*it_data).first);
				}
			}

			if (satRejected.size() == 0)	return;
			for (auto it_sat = satRejected.begin();
				it_sat != satRejected.end();
				it_sat++)
			{
				ifcbData.erase(*it_sat);
			}

		}
		catch (Exception& e)
		{
			std::cerr << e << std::endl;
			GPSTK_THROW(e);
		}
	}
}