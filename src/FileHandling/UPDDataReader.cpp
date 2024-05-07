#pragma ident "$Id: chaohu, ragnwang* $"

/**
 * @file UPDDataReader.cpp
 */

#include "UPDDataReader.hpp"
#include "StringUtils.hpp"


#define debug 0

using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp
{

	void UPDDataReader::loadIRCWL(const string& fn, const CommonTime& today)
		noexcept(false)
	{
		strm.close();
		strm.open(fn.c_str(), std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open and load the IRC WL product");
			GPSTK_THROW(e);
			exit(-1);
		}
		try
		{
			// check the time
			CivilTime cvT(today);
			int year(cvT.year), mon(cvT.month), day(cvT.day);

			updData_t updData;
			bool firstTime(true);
			// read ewl upd file
			while (!strm.eof())
			{
				string line;
				getline(strm, line);

				if (line.substr(60, 13) == "END OF HEADER")
					break;

				if (line.substr(60, 15) == "ANALYSIS CENTER")
				{
					if(line.substr(0, 3)=="GFZ")	acname="gbm";
					if(line.substr(0, 3)=="GRG")	acname="grg";
					arprod = "IRC";
					continue;
				}

				if (line.substr(0, 2) != "WL"||
					line.substr(60, 7) != "COMMENT")
					continue;
				
				
				RinexSatID sat(line.substr(3, 3));
				// check time
				int fYear(asInt(line.substr(6, 5))), fMon(asInt(line.substr(11, 4))), fDay(asInt(line.substr(15, 3)));
				if (fYear != year || fMon != mon || fDay != day)
					return;
				if (firstTime)
				{
					updWLData.clear();
					firstTime = false;
				}
				updData.value = asDouble(line.substr(37, 16));
				updData.std = 0.;
				updWLData.insert(make_pair(sat, updData));
			}	// End of while

			strm.close();	// close file stream
			
		}
		catch (...)
		{
			strm.close();
			Exception e("Cannot load IRC WL data");
			GPSTK_THROW(e);
		}
	}

	void UPDDataReader::openEWL(const string& fn)
	{
		strm.close();
		strm.open(fn.c_str(), std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadEWL();
	}	// End of openEWL string fn

	void UPDDataReader::openEWL(const char* fn)
	{
		strm.close();
		strm.open(fn, std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + *fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadEWL();
	}	// End of openEWL char* fn

	void UPDDataReader::openWL(const string& fn)
	{
		strm.close();
		strm.open(fn.c_str(), std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadWL();
	}	// End of openWL string fn

	void UPDDataReader::openWL(const char* fn)
	{
		strm.close();
		strm.open(fn, std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + *fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadWL();
	}	// End of openWL char* fn

	void UPDDataReader::openNL(const string& fn)
	{
		strm.close();
		strm.open(fn.c_str(), std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadNL();
	}	// End of openNL string fn

	void UPDDataReader::openNL(const char* fn)
	{
		strm.close();
		strm.open(fn, std::ios::in);
		if (!strm.is_open())
		{
			Exception e("Cannot open file " + *fn);
			GPSTK_THROW(e);
			exit(-1);
		}
		loadNL();
	}	// End of openNL char* fn


	void UPDDataReader::loadEWL()
		noexcept(false)
	{
		try
		{
			updEWLData.clear();
			updData_t updData;
			// read ewl upd file
			while (!strm.eof())
			{
				string line;
				getline(strm, line);
				if (line.substr(0, 1) == "%")
					continue;
				else if (line.substr(0, 3) == "EOF")
					break;

				RinexSatID sat(line.substr(1, 3));
				updData.value = asDouble(line.substr(10, 10));
				updData.std = asDouble(line.substr(20, 10));
				updEWLData.insert(make_pair(sat, updData));
			}	// End of while

			strm.close();	// close file stream
		}
		catch (Exception& e)
		{
			strm.close();
			std::cerr << e.getLocation() << std::endl;
			std::cerr << e.getText() << std::endl;
			GPSTK_THROW(e);
		}
	}	// End of loadEWL

	void UPDDataReader::loadWL()
		noexcept(false)
	{
		try
		{
			updWLData.clear();
			updData_t updData;
			// read ewl upd file
			while (!strm.eof())
			{
				string line;
				getline(strm, line);
				
				if (line.substr(0, 1) == "%")
					continue;
				else if (line.substr(0,3) == "EOF")
					break;

				RinexSatID sat(line.substr(1, 3));
				updData.value = asDouble(line.substr(10, 10));
				updData.std = asDouble(line.substr(20, 10));
				updWLData.insert(make_pair(sat, updData));
			}	// End of while

			strm.close();	// close file stream
			arprod = "UPD";
		}
		catch (Exception& e)
		{
			strm.close();
			std::cerr << e.getLocation() << std::endl;
			std::cerr << e.getText() << std::endl;
			GPSTK_THROW(e);
		}
	}	// End of loadWL

	void UPDDataReader::loadNL()
		noexcept(false)
	{
		try
		{
			updNLData.clear();
			CommonTime epoch(CommonTime::BEGINNING_OF_TIME);
			updData_t updData;
			// read ewl upd file
			while (!strm.eof())
			{
				string line;
				getline(strm, line);

				if (line.substr(0, 1) == "%")
					continue;
				else if (line.substr(0, 3) == "EOF")
					break;
				else if (line.substr(0, 11) == " EPOCH-TIME")
				{
					long double mjd(asInt(line.substr(11, 8)));
					double sod(asDouble(line.substr(19, 10)));
					MJD mjdT(mjd, TimeSystem::GPS);
					epoch = mjdT.convertToCommonTime() + sod;
					continue;
				}

				RinexSatID sat(line.substr(1, 3));
				updData.value = asDouble(line.substr(10, 10));
				updData.std = asDouble(line.substr(20, 10));
				updNLData[epoch][sat] = updData;
			}	// End of while

			strm.close();	// close file stream
		}
		catch (Exception& e)
		{
			strm.close();
			std::cerr << e.getLocation() << std::endl;
			std::cerr << e.getText() << std::endl;
			GPSTK_THROW(e);
		}
	}	// End of load NL

	double UPDDataReader::getSatUPD(const SatID& sat,
		const TypeID& type,	const CommonTime& epoch)
		noexcept(false)
	{
		try
		{
			if (type == TypeID::EWL)
			{
				auto itData = updEWLData.find(sat);
				if (itData != updEWLData.end())
				{
					return itData->second.value;
				}
				else
				{
					Exception e("Cannot get " + asString(sat.system) + asString<int>(sat.id)\
					+ " EWL UPD value!");
					GPSTK_THROW(e);
				}
			}
			else if (type == TypeID::WL)
			{
				auto itData = updWLData.find(sat);
				if (itData != updWLData.end())
				{
					return itData->second.value;
				}
				else
				{
					Exception e("Cannot get " + asString(sat.system) + asString<int>(sat.id)\
						+ " WL UPD value!");
					GPSTK_THROW(e);
				}
			}	// End of type == WL
			else if (type == TypeID::NL)
			{
				auto itEpoch = updNLData.find(epoch);
				if (itEpoch != updNLData.end())
				{
					auto itData = itEpoch->second.find(sat);
					if (itData != itEpoch->second.end())
					{
						return itData->second.value;
					}
					else
					{
						Exception e("Cannot get " + asString(sat.system) + asString<int>(sat.id)\
							+ " NL UPD value!");
						GPSTK_THROW(e);
					}
				}
				else
				{
					Exception e("Cannot get " + asString(epoch) + " " + asString(sat.system)\
						+ asString<int>(sat.id) + " NL UPD value!");
					GPSTK_THROW(e);
				}
			}	// End of get NL data
		}
		catch (Exception& e)
		{
			std::cerr << e.getLocation();
			std::cerr << ": " + e.getText() << std::endl;
			GPSTK_THROW(e);
		}
        return -0.;
	}	// End of getSatUPD


}  // End of namespace gpstk
