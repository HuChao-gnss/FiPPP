#pragma ident "ID: ragn wang 2023-03-18"
//
// This part is used to remove the bds2 satellites.
// 
//
#include "RinexSatID.hpp"
#include "gnssData.hpp"
#include "AboutOptions.hpp"
#include "StringProcess.hpp"

using namespace gpstk;

namespace fippp
{
	class EraseSat
	{
	public:
		EraseSat() {};

		EraseSat(Options::options_t opt) : opts(opt) 
		{
			opts = opt;
			exPrns = opts.exPrns;
			satIDsfromPrns();
		}

		void setOpts(Options::options_t opt)
		{
			opts = opt;
			exPrns = opts.exPrns;
			satIDsfromPrns();
		}

		virtual void Process(gnssRinex& gRin)
		{ 
			if(!opts.bds2)
				eraseBDS2(gRin.body); 
			if (exSats.size())
				gRin.body.removeSatID(exSats);
		}

		virtual ~EraseSat() {};

	private:
		Options::options_t opts;
		std::string exPrns;
		SatIDSet exSats;
		// erase bds2 satllite
		virtual satTypeValueMap& eraseBDS2(satTypeValueMap& gData)
			noexcept(false);

		void satIDsfromPrns();
	};
}
