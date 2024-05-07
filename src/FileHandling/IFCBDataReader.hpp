#pragma ident "$ID: chao hu ragn wang 2023-03-15 $"

#ifndef GPSTK_IFCBDataReader_HPP
#define GPSTK_IFCBDataReader_HPP

// ifcb data reader is used to read GPS ifcb 
// bias
// 
//
#include <string>
#include <map>
#include <fstream>

#include "Exception.hpp"
#include "StringUtils.hpp"
#include "SatID.hpp"
#include "TypeID.hpp"
#include "Epoch.hpp"
#include "RinexSatID.hpp"
#include "BiasDataReader.hpp"
#include "DataStructures.hpp"

using namespace gpstk;

namespace fippp
{
	class IFCBDataReader
	{
	public:
		// default constructor
		IFCBDataReader() 
		{
			satIfcbData.clear();
		};


		IFCBDataReader(const char* fn)
		{
			open(fn);
		}

		void open(const char* fn);
		void open(const std::string& fn);

		double getSatIFCB(const RinexSatID& sat, const CommonTime& ttag, const int nhalf);

		~IFCBDataReader() {};

		struct ifcbData_t
		{
			ifcbData_t() :
				value(0.0),
				std(0.0)
			{};
			double value;
			double std;
		};
		typedef std::map<CommonTime, ifcbData_t> IFCBData;
		typedef std::map<RinexSatID, IFCBData> satIFCBData;

		void checkIFCBData(satIFCBData& ifcbData);

	private:

		std::string mode;
		std::fstream fstm;

		satIFCBData satIfcbData;

		void loadData()
			noexcept(false);
	};
}


#endif // !GPSTK_IFCBDataReader_HPP


