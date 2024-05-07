#pragma ident "$ID: chao hu, ragnwang*$"

#ifndef FiPPP_ComputeResidual_HPP
#define FiPPP_ComputeResidual_HPP

#include "SatID.hpp"
#include "AboutOptions.hpp"
#include "Variable.hpp"
#include "DataStructures.hpp"

using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp{

	class ComputeResidual{
	public:
		// defualt constructor
		ComputeResidual()
		{}
		// process
		void Process(gnssRinex& gRin)
		{
			Process(gRin.header, gRin.body);
		}

		void Process(const gnssRinexHeader& gHeader, satTypeValueMap& gData)
			noexcept(false);

		// set sets
		ComputeResidual& setAmbSet(const TypeIDSet& typeSet)
		{
			ambTypeSet = typeSet;
			return (*this);
		}

		ComputeResidual& setSols(const Vector<double>& solutions)
		{
			sols = solutions;
			return (*this);
		}

		ComputeResidual& setUnkSet(const VariableSet& varSet)
		{
			unkSet = varSet;
			return (*this);
		}
		void addLinear(const SatelliteSystem& sys, gnssLinearCombination& comb)
		{
			systemCombs[sys].push_back(comb);
		};

		LinearCombList getLinear(const SatelliteSystem& sys)
		{
			return systemCombs[sys];
		}

		std::map<SatelliteSystem, LinearCombList> getLinear()
		{
			return systemCombs;
		}
		// default destructor
		~ComputeResidual() {}

	private:
		// ambiguity type id set
		TypeIDSet ambTypeSet;
		// pre epoch solutions
		Vector<double> sols;
		// unknown variable set
		VariableSet unkSet;
		//
		std::map<SatelliteSystem, LinearCombList> systemCombs;
		// get solutions
		double getSolution(const TypeID& type)	noexcept(false);
		double getSolution(const TypeID& type, const SatID& sat)
			noexcept(false);

	};

}

#endif // !FiPPP_ComputeResidual_HPP
