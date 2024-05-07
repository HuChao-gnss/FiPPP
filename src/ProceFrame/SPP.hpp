#pragma ident "$ID: ChaoHu ragnwang*$"
//
// This part is used to single point positioning.
// Refer to RTKPOS
//
#ifndef SPP_HPP
#define SPP_HPP

#include "gnssData.hpp"
#include "CommonTime.hpp"
#include "TypeID.hpp"
#include "StringUtils.hpp"
#include "ComputeSatPos.hpp"
#include "Rinex3EphemerisStore.hpp"
#include "ConvertObs.hpp"
#include "IonoModel.hpp"
#include "SaasTropModel.hpp"
#include "SolverBase.hpp"
#include "TypeID.hpp"
#include "gnssData.hpp"

#include "ComputeSPP.hpp"
#include "XYZ2ENU.hpp"
#include "AboutOptions.hpp"

using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp
{
	class SPP
	{
	public:
		//default constuct
		SPP():
			pOutStream(NULL)
		{};

		SPP(Rinex3EphemerisStore& nav, const Options::options_t& opt) :
			navEph(nav),
			opts(opt),
			pOutStream(NULL)
		{}
		/// <summary>
		/// return class name
		/// </summary>
		/// <returns></returns>
		std::string getClassName() const;
		/// process the data
		virtual void Process(gnssRinex& gRin)
			noexcept(false);
		//
		void setOpts(const Options::options_t& opt)
		{
			opts = opt;
		}

		void setFwdOutStream(std::ofstream* pStream)
		{
			pOutStream = pStream;
		};

		void setTraceStream(std::ofstream* pStream)
		{ pTraceStream = pStream; }

		void setRefPos(const Triple& pos)
		{ refPos = pos; }


		// print solution
		void printSol(std::ofstream* pOutStream, const gnssRinexHeader& gRinHeader);

        // print newline
        void printNewLine(std::ofstream* pOutStream);

		//default destruct
		virtual ~SPP() {};

	private:
		Options::options_t opts;
		// reference station position
		Position refPos;
		// broadcast epheremic data
		Rinex3EphemerisStore navEph;
		// out file stream
		std::ofstream* pOutStream;

		std::ofstream* pTraceStream;

	};
}


#endif // !SPP_HPP
