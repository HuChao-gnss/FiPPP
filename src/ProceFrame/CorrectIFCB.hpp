#pragma ident "$ID: chao hu ragn wang 2023-03-15 $"

#ifndef GPSTK_CorrectIFCB_HPP
#define GPSTK_CorrectIFCB_HPP

// corretIfcb is used to correct GPS ifcb 
// bias
//

#include "gnssData.hpp"
#include "IFCBDataReader.hpp"
#include "GNSSconstants.hpp"

using namespace gpstk;

namespace fippp
{
	class CorrectIFCB
	{
	public:
		// default construct
		CorrectIFCB() :pIFCBStore(NULL)
		{};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="epoch"></param>
		/// <param name="satTypes"></param>
		/// <param name="gData"></param>
		/// <returns></returns>
		virtual satTypeValueMap& Process(const CommonTime& time,
			std::map<RinexSatID, TypeIDVec>& satShortTypes,
			satTypeValueMap& gData)
			noexcept(false);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="gRin"></param>
		/// <returns></returns>
		virtual void Process(gnssRinex& gRin)
			noexcept(false)
		{
			Process(gRin.header.epoch, gRin.header.satShortTypes, gRin.body);
		}

		virtual void setIFCBStore(IFCBDataReader& ifcbStore)
		{
			pIFCBStore = &ifcbStore;
		}

		virtual double getIFCBValue(const CommonTime& time, const double& pr, const RinexSatID sat);

		// Returns a string identifying this object.
		virtual std::string getClassName() const;

		// default
		~CorrectIFCB() {};

	private:
		IFCBDataReader* pIFCBStore;
	};
}


#endif // !GPSTK_CorrectIFCB_HPP
