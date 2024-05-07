#pragma ident "ID: chaohu ragnwang* chaohu@cumt.edu.cn "

#ifndef CombinationOptions_HPP
#define CombinationOptions_HPP

#include "DetectCSGF.hpp"
#include "DetectCSMW.hpp"
#include "LinearCombinations.hpp"
#include "ComputeCombination.hpp"
#include "RequiredObs.hpp"
#include "AboutOptions.hpp"
#include "ComputeResidual.hpp"

using namespace gpstk;

namespace fippp
{
	class CombinationOptions
	{
	public:
		// default construct
		CombinationOptions()
		{};
		CombinationOptions(LinearCombinations lc) : linear(lc)
		{}

		virtual void addComputeCSType(ComputeCombination& computeMW, ComputeCombination& computeGF);
		// add cycle slip detected methods combiantion
		virtual void addCSType(DetectCSGF& detectGFCS, DetectCSMW& detectMWCS);
		// add compute linear combination
		virtual void addLCType(ComputeCombination& computeLC);
		// add compute prefit residual combination
		virtual void addPrefitType(ComputeCombination& computePrefit, ComputeResidual& computeRes);
		// add reqObs Type
		virtual void addReqObsType(RequiredObs& reqObs);

		//
		CombinationOptions& setOpts(const Options::options_t& opt)
		{
			opts = opt;
			return (*this);
		}
		// default destruct
		virtual ~CombinationOptions() {};

	private:
		Options::options_t opts;
		LinearCombinations linear;
	};
}

#endif // !CombinationsOption_HPP
