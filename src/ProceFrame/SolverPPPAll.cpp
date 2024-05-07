#pragma ident "id: chaohu, ragnwang*"

/**
* @file SolverPPPAll.cpp
* This file is an import part of fippp.
* It is used to solve GPS/GAL/BDS PPP Float solution.
*/

#include "SolverPPPAll.hpp"
#include "SystemTime.hpp"
#include "ENUUtil.hpp"
#include "Stats.hpp"
#include <direct.h>


using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp
{
	int SolverPPPAll::classIndex = 9600000;

	int SolverPPPAll::getIndex() const
	{ return index; }

	std::string SolverPPPAll::getClassName() const
	{ return "SolverPPPAll"; }

	SolverPPPAll& SolverPPPAll::setRecClk()
	{
		if (opts.system.find("G") != std::string::npos)
			reclkTypeSet.insert(TypeID::dcdtGPS);
		if (opts.system.find("C") != std::string::npos)
			reclkTypeSet.insert(TypeID::dcdtBDS);
		if (opts.system.find("E") != std::string::npos)
			reclkTypeSet.insert(TypeID::dcdtGAL);
		if (opts.system.find("R") != std::string::npos)
			reclkTypeSet.insert(TypeID::dcdtGLO);
		if (opts.system.find("J") != std::string::npos)
			reclkTypeSet.insert(TypeID::dcdtQZS);
		return (*this);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	SolverPPPAll& SolverPPPAll::setAmbSet()
	{
		if (ionoopt == Options::UC1)
		{
			ambTypeSet.insert(TypeID::BL1G);
			ambTypeSet.insert(TypeID::BL1E);
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BL1C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BL2C);
				}
			}
			
		}

		if (ionoopt == Options::UC12)
		{
			ambTypeSet.insert(TypeID::BL1G);
			ambTypeSet.insert(TypeID::BL2G);
			ambTypeSet.insert(TypeID::BL1E);
			ambTypeSet.insert(TypeID::BL5E);
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BL1C);
					ambTypeSet.insert(TypeID::BL5C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BL2C);
					ambTypeSet.insert(TypeID::BL6C);
				}
			}
			
		}	// end of ionoopt_uc12
		if (ionoopt == Options::IF12)
		{
			ambTypeSet.insert(TypeID::BLC12G);
			ambTypeSet.insert(TypeID::BLC15E);
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC15C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC26C);
				}
			}
			
		}	// end of ionoopt_if12
		if (ionoopt == Options::UC123)
		{
			ambTypeSet.insert(TypeID::BL1G);
			ambTypeSet.insert(TypeID::BL2G);
			ambTypeSet.insert(TypeID::BL5G);

			ambTypeSet.insert(TypeID::BL1E);
			ambTypeSet.insert(TypeID::BL5E);
			ambTypeSet.insert(TypeID::BL7E);
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BL1C);
					ambTypeSet.insert(TypeID::BL5C);
					ambTypeSet.insert(TypeID::BL2C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BL2C);
					ambTypeSet.insert(TypeID::BL6C);
					ambTypeSet.insert(TypeID::BL1C);
					ambTypeSet.insert(TypeID::BL7C);
				}
			}
			
		}	//end of if ionoopt_uc123
		if (ionoopt == Options::IF1213)
		{
			// GPS
			ambTypeSet.insert(TypeID::BLC12G);
			ambTypeSet.insert(TypeID::BLC15G);
			ambTypeSet.insert(TypeID::BLC15E);
			ambTypeSet.insert(TypeID::BLC17E);
			// bds3
			if (!bdsComb)
			{
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC27C);
				}
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC15C);
					ambTypeSet.insert(TypeID::BLC52C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC61C);
				}
			}
		}	// End of if1213
		if (ionoopt == Options::IF123)
		{
			//GPS
			ambTypeSet.insert(TypeID::BLC125G);
			//GAL
			ambTypeSet.insert(TypeID::BLC157E);
			//bds
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC152C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC261C);
				}
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC267C);
				}
			}
		}
		if (ionoopt == Options::UC1234)
		{
			// GPS
			ambTypeSet.insert(TypeID::BL1G);
			ambTypeSet.insert(TypeID::BL2G);
			ambTypeSet.insert(TypeID::BL5G);
			// GAL
			ambTypeSet.insert(TypeID::BL1E);
			ambTypeSet.insert(TypeID::BL5E);
			ambTypeSet.insert(TypeID::BL7E);
			ambTypeSet.insert(TypeID::BL8E);
			if (!bdsComb)
			{
				//bds
				ambTypeSet.insert(TypeID::BL1C);
				ambTypeSet.insert(TypeID::BL5C);
				ambTypeSet.insert(TypeID::BL2C);
				ambTypeSet.insert(TypeID::BL6C);
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BL7C);
				}
			}
		}	// end of uc1234
		if (ionoopt == Options::IF121314)
		{
			// GPS
			ambTypeSet.insert(TypeID::BLC12G);
			ambTypeSet.insert(TypeID::BLC15G);
			ambTypeSet.insert(TypeID::BLC15E);
			ambTypeSet.insert(TypeID::BLC17E);
			ambTypeSet.insert(TypeID::BLC18E);

			// bds
			if (!bdsComb)
			{
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC27C);
				}
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC15C);
					ambTypeSet.insert(TypeID::BLC12C);
					ambTypeSet.insert(TypeID::BLC16C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC21C);
					ambTypeSet.insert(TypeID::BLC25C);
				}
			}
		}	// end of if121314
		if (ionoopt == Options::IF1234)
		{
			//GPS
			ambTypeSet.insert(TypeID::BLC125G);
			//GAL
			ambTypeSet.insert(TypeID::BLC1578E);
			//bds
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC1526C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC2615C);
				}
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC267C);
				}
			}
		}	// end of if1234
		// 5 frequency combination
		if (ionoopt == Options::UC12345)
		{
			// GPS
			ambTypeSet.insert(TypeID::BL1G);
			ambTypeSet.insert(TypeID::BL2G);
			ambTypeSet.insert(TypeID::BL5G);
			// GAL
			ambTypeSet.insert(TypeID::BL1E);
			ambTypeSet.insert(TypeID::BL5E);
			ambTypeSet.insert(TypeID::BL7E);
			ambTypeSet.insert(TypeID::BL8E);
			ambTypeSet.insert(TypeID::BL6E);
			if (!bdsComb)
			{
				//bds
				ambTypeSet.insert(TypeID::BL1C);
				ambTypeSet.insert(TypeID::BL5C);
				ambTypeSet.insert(TypeID::BL2C);
				ambTypeSet.insert(TypeID::BL6C);
				ambTypeSet.insert(TypeID::BL8C);
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BL7C);
				}
			}
			
		} // end of if (ionoopt == IONOOPT_UC12345)

		if (ionoopt == Options::IF12131415)
		{
			// GPS
			ambTypeSet.insert(TypeID::BLC12G);
			ambTypeSet.insert(TypeID::BLC15G);
			ambTypeSet.insert(TypeID::BLC15E);
			ambTypeSet.insert(TypeID::BLC17E);
			ambTypeSet.insert(TypeID::BLC18E);
			ambTypeSet.insert(TypeID::BLC16E);
			
			// bds
			if (!bdsComb)
			{
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC27C);
				}
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC15C);
					ambTypeSet.insert(TypeID::BLC52C);
					ambTypeSet.insert(TypeID::BLC16C);
					ambTypeSet.insert(TypeID::BLC18C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC26C);
					ambTypeSet.insert(TypeID::BLC61C);
					ambTypeSet.insert(TypeID::BLC25C);
					ambTypeSet.insert(TypeID::BLC28C);
				}
			}
			
		}	// end of 'if (ionoopt == IONOOPT_IF12131415)'

		if (ionoopt == Options::IF12345)
		{
			//GPS
			ambTypeSet.insert(TypeID::BLC125G);
			//GAL
			ambTypeSet.insert(TypeID::BLC15786E);
			//bds
			if (!bdsComb)
			{
				if (bdsfopt)
				{
					ambTypeSet.insert(TypeID::BLC15268C);
				}
				else
				{
					ambTypeSet.insert(TypeID::BLC26158C);
				}
				if (bds2)
				{
					ambTypeSet.insert(TypeID::BLC267C);
				}
			}
			
		}	// end of 'if (ionoopt == IONOOPT_IF12345)'

		if (bdsComb)
		{
			if (bdsComb == Options::CCI)
			{
				ambTypeSet.insert(TypeID::BLC15C);
				ambTypeSet.insert(TypeID::BLC18C);
				ambTypeSet.insert(TypeID::BLC26C);
			}
			else if (bdsComb == Options::CI)
			{
				ambTypeSet.insert(TypeID::BLC158C);
				ambTypeSet.insert(TypeID::BLC26C);
			}
		}

		return (*this);
	}	// end of SolverPPPAll& SolverPPPAll::setAmbSet()
	/// <summary>
	/// 
	/// </summary>
	/// <param name=""></param>
	void SolverPPPAll::setUpEquations(void)
	{
		Variable dx(TypeID::dX);
		Variable dy(TypeID::dY);
		Variable dz(TypeID::dZ);
		dx.setInitialVariance(VAR_POS);
		dy.setInitialVariance(VAR_POS);
		dz.setInitialVariance(VAR_POS);

		whiteNoiseDXModel.setSigma(60);
		whiteNoiseDYModel.setSigma(60);
		whiteNoiseDZModel.setSigma(60);
		if (kinematic)
		{
			dx.setModel(&whiteNoiseDXModel);
			dy.setModel(&whiteNoiseDYModel);
			dz.setModel(&whiteNoiseDZModel);
		}
		else
		{
			dx.setModel(&constantDXModel);
			dy.setModel(&constantDYModel);
			dz.setModel(&constantDZModel);

		}

		VariableSet xyzVarSet;
		xyzVarSet.insert(dx);
		xyzVarSet.insert(dy);
		xyzVarSet.insert(dz);

		dcdtGPSModel.setSigma(60);
		dcdtGALModel.setSigma(60);
		dcdtBDSModel.setSigma(60);
		Variable dcdtGPS(TypeID::dcdtGPS, &dcdtGPSModel);
		Variable dcdtGAL(TypeID::dcdtGAL, &dcdtGALModel);
		Variable dcdtBDS(TypeID::dcdtBDS, &dcdtBDSModel);
		dcdtGPS.setInitialVariance(VAR_POS);
		dcdtGAL.setInitialVariance(VAR_POS);
		dcdtBDS.setInitialVariance(VAR_POS);
		// tropo
		tropoModel.setQprime(1.0e-8);
		Variable tropo(TypeID::wetMap, &tropoModel);
		// iono
		slantIonoC1GModel.setQprime(4.0e-2);
		slantIonoC1EModel.setQprime(4.0e-2);
		slantIonoC1CModel.setQprime(4.0e-2);
		slantIonoC2CModel.setQprime(4.0e-2);
		Variable ionoC1G(TypeID::slantIonoC1G, &slantIonoC1GModel, true, true, false);
		Variable ionoC1E(TypeID::slantIonoC1E, &slantIonoC1EModel, true, true, false);
		Variable ionoC1C(TypeID::slantIonoC1C, &slantIonoC1CModel, true, true, false);
		Variable ionoC2C(TypeID::slantIonoC2C, &slantIonoC2CModel, true, true, false);
		
		// ifb
		Variable ifbC5G;
		Variable ifbC1C;
		Variable ifbC5C;
		Variable ifbC2C;
		Variable ifbC6C;
		Variable ifbC8C;
		Variable ifbC7C;
		Variable ifbC7E;
		Variable ifbC8E;
		Variable ifbC6E;
		Variable ifbC26C;


		ifbC5G.setType(TypeID::ifbC5G).setModel(&ifbC5GRWModel);
		ifbC1C.setType(TypeID::ifbC1C).setModel(&ifbC1CRWModel);
		ifbC5C.setType(TypeID::ifbC5C).setModel(&ifbC5CRWModel);
		ifbC2C.setType(TypeID::ifbC2C).setModel(&ifbC2CRWModel);
		ifbC6C.setType(TypeID::ifbC6C).setModel(&ifbC6CRWModel);
		ifbC8C.setType(TypeID::ifbC8C).setModel(&ifbC8CRWModel);
		ifbC7C.setType(TypeID::ifbC7C).setModel(&ifbC7CRWModel);
		ifbC7E.setType(TypeID::ifbC7E).setModel(&ifbC7ERWModel);
		ifbC8E.setType(TypeID::ifbC8E).setModel(&ifbC8ERWModel);
		ifbC6E.setType(TypeID::ifbC6E).setModel(&ifbC6ERWModel);
		ifbC26C.setType(TypeID::ifbC26C).setModel(&ifbC26CRWModel);

		ifbC5G.setInitialVariance(VAR_IFB);
		ifbC1C.setInitialVariance(VAR_IFB);
		ifbC5C.setInitialVariance(VAR_IFB);
		ifbC2C.setInitialVariance(VAR_IFB);
		ifbC6C.setInitialVariance(VAR_IFB);
		ifbC7C.setInitialVariance(VAR_IFB);
		ifbC8C.setInitialVariance(VAR_IFB);
		ifbC7E.setInitialVariance(VAR_IFB);
		ifbC8E.setInitialVariance(VAR_IFB);
		ifbC6E.setInitialVariance(VAR_IFB);
		ifbC26C.setInitialVariance(VAR_IFB);

		// ifcb
		ifcbL5GModel.setQprime(1.0e-4);
		ifcbL7CModel.setQprime(1.0e-4);
		ifcbL1CModel.setQprime(1.0e-4);
		ifcbL5CModel.setQprime(1.0e-4);
		ifcbL2CModel.setQprime(1.0e-4);
		ifcbL6CModel.setQprime(1.0e-4);
		ifcbL8CModel.setQprime(1.0e-4);
		ifcbL7EModel.setQprime(1.0e-4);
		ifcbL8EModel.setQprime(1.0e-4);
		ifcbL6EModel.setQprime(1.0e-4);
		Variable ifcbL5G(TypeID::ifcbL5G, &ifcbL5GModel, true, true, false);
		Variable ifcbL7C(TypeID::ifcbL7C, &ifcbL7CModel, true, true, false);
		Variable ifcbL1C(TypeID::ifcbL1C, &ifcbL1CModel, true, true, false);
		Variable ifcbL5C(TypeID::ifcbL5C, &ifcbL5CModel, true, true, false);
		Variable ifcbL2C(TypeID::ifcbL2C, &ifcbL2CModel, true, true, false);
		Variable ifcbL6C(TypeID::ifcbL6C, &ifcbL6CModel, true, true, false);
		Variable ifcbL8C(TypeID::ifcbL8C, &ifcbL8CModel, true, true, false);
		Variable ifcbL7E(TypeID::ifcbL7E, &ifcbL7EModel, true, true, false);
		Variable ifcbL8E(TypeID::ifcbL8E, &ifcbL8EModel, true, true, false);
		Variable ifcbL6E(TypeID::ifcbL6E, &ifcbL6EModel, true, true, false);
		ifcbL5G.setInitialVariance(VAR_IFCB);
		ifcbL7C.setInitialVariance(VAR_IFCB);
		ifcbL1C.setInitialVariance(VAR_IFCB);
		ifcbL5C.setInitialVariance(VAR_IFCB);
		ifcbL2C.setInitialVariance(VAR_IFCB);
		ifcbL6C.setInitialVariance(VAR_IFCB);
		ifcbL8C.setInitialVariance(VAR_IFCB);
		ifcbL7E.setInitialVariance(VAR_IFCB);
		ifcbL8E.setInitialVariance(VAR_IFCB);
		ifcbL6E.setInitialVariance(VAR_IFCB);

		// ambiguity gps
		Variable BL1G(TypeID::BL1G, &BL1GModel, true, true, true);
		Variable BL2G(TypeID::BL2G, &BL2GModel, true, true, true);
		Variable BL5G(TypeID::BL5G, &BL2GModel, true, true, true);
		Variable BLC12G(TypeID::BLC12G, &BLC12GModel, true, true, true);
		Variable BLC15G(TypeID::BLC15G, &BLC15GModel, true, true, true);
		Variable BLC125G(TypeID::BLC125G, &BLC125GModel, true, true, true);
		BL1G.setInitialVariance(VAR_BIAS);
		BL2G.setInitialVariance(VAR_BIAS);
		BL5G.setInitialVariance(VAR_BIAS);
		BLC12G.setInitialVariance(VAR_BIAS);
		BLC15G.setInitialVariance(VAR_BIAS);
		BLC125G.setInitialVariance(VAR_BIAS);
		// bds
		Variable BL1C(TypeID::BL1C, &BL1CModel, true, true, true);
		Variable BL2C(TypeID::BL2C, &BL2CModel, true, true, true);
		Variable BL5C(TypeID::BL5C, &BL5CModel, true, true, true);
		Variable BL6C(TypeID::BL6C, &BL6CModel, true, true, true);
		Variable BL7C(TypeID::BL7C, &BL7CModel, true, true, true);
		Variable BL8C(TypeID::BL8C, &BL8CModel, true, true, true);
		BL1C.setInitialVariance(VAR_BIAS);
		BL2C.setInitialVariance(VAR_BIAS);
		BL5C.setInitialVariance(VAR_BIAS);
		BL6C.setInitialVariance(VAR_BIAS);
		BL7C.setInitialVariance(VAR_BIAS);
		BL8C.setInitialVariance(VAR_BIAS);

		Variable BLC15C(TypeID::BLC15C, &BLC15CModel, true, true, true);
		Variable BLC12C(TypeID::BLC12C, &BLC12CModel, true, true, true);
		Variable BLC52C(TypeID::BLC52C, &BLC52CModel, true, true, true);
		Variable BLC16C(TypeID::BLC16C, &BLC16CModel, true, true, true);
		Variable BLC18C(TypeID::BLC18C, &BLC18CModel, true, true, true);
		Variable BLC158C(TypeID::BLC158C, &BLC158CModel, true, true, true);
		Variable BLC152C(TypeID::BLC152C, &BLC152CModel, true, true, true);
		Variable BLC1526C(TypeID::BLC1526C, &BLC1526CModel, true, true, true);
		Variable BLC15268C(TypeID::BLC15268C, &BLC15268CModel, true, true, true);

		BLC15C.setInitialVariance(VAR_BIAS);
		BLC12C.setInitialVariance(VAR_BIAS);
		BLC52C.setInitialVariance(VAR_BIAS);
		BLC16C.setInitialVariance(VAR_BIAS);
		BLC18C.setInitialVariance(VAR_BIAS);
		BLC158C.setInitialVariance(VAR_BIAS);
		BLC152C.setInitialVariance(VAR_BIAS);
		BLC1526C.setInitialVariance(VAR_BIAS);
		BLC15268C.setInitialVariance(VAR_BIAS);

		// default combination
		Variable BLC26C(TypeID::BLC26C, &BLC26CModel, true, true, true);
		Variable BLC27C(TypeID::BLC27C, &BLC27CModel, true, true, true);
		Variable BLC21C(TypeID::BLC21C, &BLC21CModel, true, true, true);
		Variable BLC61C(TypeID::BLC61C, &BLC61CModel, true, true, true);
		Variable BLC25C(TypeID::BLC25C, &BLC25CModel, true, true, true);
		Variable BLC28C(TypeID::BLC28C, &BLC28CModel, true, true, true);
		Variable BLC267C(TypeID::BLC267C, &BLC267CModel, true, true, true);
		Variable BLC261C(TypeID::BLC261C, &BLC261CModel, true, true, true);
		Variable BLC2615C(TypeID::BLC2615C, &BLC2615CModel, true, true, true);
		Variable BLC26158C(TypeID::BLC26158C, &BLC26158CModel, true, true, true);

		BLC26C.setInitialVariance(VAR_BIAS);
		BLC27C.setInitialVariance(VAR_BIAS);
		BLC21C.setInitialVariance(VAR_BIAS);
		BLC61C.setInitialVariance(VAR_BIAS);
		BLC25C.setInitialVariance(VAR_BIAS);
		BLC28C.setInitialVariance(VAR_BIAS);
		BLC267C.setInitialVariance(VAR_BIAS);
		BLC261C.setInitialVariance(VAR_BIAS);
		BLC2615C.setInitialVariance(VAR_BIAS);
		BLC26158C.setInitialVariance(VAR_BIAS);

		// gal
		Variable BL1E(TypeID::BL1E, &BL1EModel, true, true, true);
		Variable BL5E(TypeID::BL5E, &BL5EModel, true, true, true);
		Variable BL7E(TypeID::BL7E, &BL7EModel, true, true, true);
		Variable BL8E(TypeID::BL8E, &BL8EModel, true, true, true);
		Variable BL6E(TypeID::BL6E, &BL6EModel, true, true, true);
		BL1E.setInitialVariance(VAR_BIAS);
		BL5E.setInitialVariance(VAR_BIAS);
		BL7E.setInitialVariance(VAR_BIAS);
		BL8E.setInitialVariance(VAR_BIAS);
		BL6E.setInitialVariance(VAR_BIAS);
		
		Variable BLC15E(TypeID::BLC15E, &BLC15EModel, true, true, true);
		Variable BLC17E(TypeID::BLC17E, &BLC17EModel, true, true, true);
		Variable BLC18E(TypeID::BLC18E, &BLC18EModel, true, true, true);
		Variable BLC16E(TypeID::BLC16E, &BLC16EModel, true, true, true);
		BLC15E.setInitialVariance(VAR_BIAS);
		BLC17E.setInitialVariance(VAR_BIAS);
		BLC18E.setInitialVariance(VAR_BIAS);
		BLC16E.setInitialVariance(VAR_BIAS);

		Variable BLC157E(TypeID::BLC157E, &BLC157EModel, true, true, true);
		Variable BLC1578E(TypeID::BLC1578E, &BLC1578EModel, true, true, true);
		Variable BLC15786E(TypeID::BLC15786E, &BLC15786EModel, true, true, true);
		BLC157E.setInitialVariance(VAR_BIAS);
		BLC1578E.setInitialVariance(VAR_BIAS);
		BLC15786E.setInitialVariance(VAR_BIAS);

		// define equation object for code/phase observation, and add variables
		if (ionoopt == Options::UC1)
		{
			Equation equC1G(TypeID::prefitC1G, SatelliteSystem::GPS);
			Equation equL1G(TypeID::prefitL1G, SatelliteSystem::GPS);
			// add variable
			equC1G.addVarSet(xyzVarSet);
			equL1G.addVarSet(xyzVarSet);
			// add dcdt
			equC1G.addVariable(dcdtGPS, 1.0);
			equL1G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equC1G.addVariable(tropo);
			equL1G.addVariable(tropo);
			// iono
			equC1G.addVariable(ionoC1G, 1.0);
			equL1G.addVariable(ionoC1G, -1.0);
			// ambiguity
			equL1G.addVariable(BL1G, 1.0);
			// set weight
			equC1G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equL1G.setWeight(SQR(1 / errmeasLG));

			//gal
			Equation equC1E(TypeID::prefitC1E, SatelliteSystem::Galileo);
			Equation equL1E(TypeID::prefitL1E, SatelliteSystem::Galileo);

			//insert xyz
			equC1E.addVarSet(xyzVarSet);
			equL1E.addVarSet(xyzVarSet);
			// add dcdt
			equC1E.addVariable(dcdtGAL, 1.0);
			equL1E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equC1E.addVariable(tropo);
			equL1E.addVariable(tropo);
			// add iono
			equC1E.addVariable(ionoC1E, 1.0);
			equL1E.addVariable(ionoC1E, -1.0);

			// add ambiguity
			equL1E.addVariable(BL1E, 1.0);

			// set weight
			equC1E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equL1E.setWeight(SQR(1 / errmeasLE));

			// BDS
			Equation equC2C(TypeID::prefitC2C, SatelliteSystem::BeiDou);
			Equation equL2C(TypeID::prefitL2C, SatelliteSystem::BeiDou);
			Equation equC1C(TypeID::prefitC1C, SatelliteSystem::BeiDou);
			Equation equL1C(TypeID::prefitL1C, SatelliteSystem::BeiDou);

			// add xyz
			equC2C.addVarSet(xyzVarSet);
			equL2C.addVarSet(xyzVarSet);
			equC1C.addVarSet(xyzVarSet);
			equL1C.addVarSet(xyzVarSet);

			// add clk
			equC2C.addVariable(dcdtBDS, 1.0);
			equL2C.addVariable(dcdtBDS, 1.0);
			equC1C.addVariable(dcdtBDS, 1.0);
			equL1C.addVariable(dcdtBDS, 1.0);

			// add tropo
			equC2C.addVariable(tropo);
			equL2C.addVariable(tropo);
			equC1C.addVariable(tropo);
			equL1C.addVariable(tropo);
			// add iono
			if (bdsfopt)
			{
				equC1C.addVariable(ionoC1C, 1.0);
				equL1C.addVariable(ionoC1C, -1.0);
			}
			else
			{
				equC2C.addVariable(ionoC2C, 1.0);
				equL2C.addVariable(ionoC2C, -1.0);
			}

			// add ambiguity
			equL1C.addVariable(BL1C, 1.0);
			equL2C.addVariable(BL2C, 1.0);

			// set weight
			equC1C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC2C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equL1C.setWeight(SQR(1 / errmeasLC));
			equL2C.setWeight(SQR(1 / errmeasLC));

			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equC1E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
					}
					else
					{
						equSystem.addEquation(equC2C);
					}
				}	// end of if !bdsComb
				
			}	// end of if codeOnly
			else
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equL1G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equL1E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equL1C);
					}
					else
					{
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equL2C);
					}
				}	// end of if !bdsComb
				
			}	// end of if !codeOnly

		}	// end of Options::UC1
		else if (ionoopt == Options::UC12)
		{
			Equation equC1G(TypeID::prefitC1G, SatelliteSystem::GPS);
			Equation equL1G(TypeID::prefitL1G, SatelliteSystem::GPS);
			Equation equC2G(TypeID::prefitC2G, SatelliteSystem::GPS);
			Equation equL2G(TypeID::prefitL2G, SatelliteSystem::GPS);
			// add variable
			equC1G.addVarSet(xyzVarSet);
			equL1G.addVarSet(xyzVarSet);
			equC2G.addVarSet(xyzVarSet);
			equL2G.addVarSet(xyzVarSet);
			// add dcdt
			equC1G.addVariable(dcdtGPS, 1.0);
			equL1G.addVariable(dcdtGPS, 1.0);
			equC2G.addVariable(dcdtGPS, 1.0);
			equL2G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equC1G.addVariable(tropo);
			equL1G.addVariable(tropo);
			equC2G.addVariable(tropo);
			equL2G.addVariable(tropo);
			// iono
			equC1G.addVariable(ionoC1G, 1.0);
			equL1G.addVariable(ionoC1G, -1.0);
			equC2G.addVariable(ionoC1G, GAMMA_GPS_L1L2);
			equL2G.addVariable(ionoC1G, -GAMMA_GPS_L1L2);
			// ambiguity
			equL1G.addVariable(BL1G, 1.0);
			equL2G.addVariable(BL2G, 1.0);
			// set weight
			equC1G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equC2G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equL1G.setWeight(SQR(1 / errmeasLG));
			equL2G.setWeight(SQR(1 / errmeasLG));

			//gal
			Equation equC1E(TypeID::prefitC1E, SatelliteSystem::Galileo);
			Equation equL1E(TypeID::prefitL1E, SatelliteSystem::Galileo);
			Equation equC5E(TypeID::prefitC5E, SatelliteSystem::Galileo);
			Equation equL5E(TypeID::prefitL5E, SatelliteSystem::Galileo);

			//insert xyz
			equC1E.addVarSet(xyzVarSet);
			equL1E.addVarSet(xyzVarSet);
			equC5E.addVarSet(xyzVarSet);
			equL5E.addVarSet(xyzVarSet);
			// add dcdt
			equC1E.addVariable(dcdtGAL, 1.0);
			equL1E.addVariable(dcdtGAL, 1.0);
			equC5E.addVariable(dcdtGAL, 1.0);
			equL5E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equC1E.addVariable(tropo);
			equL1E.addVariable(tropo);
			equC5E.addVariable(tropo);
			equL5E.addVariable(tropo);
			// add iono
			equC1E.addVariable(ionoC1E, 1.0);
			equL1E.addVariable(ionoC1E, -1.0);
			equC5E.addVariable(ionoC1E, GAMMA_GAL_L1L5);
			equL5E.addVariable(ionoC1E, -GAMMA_GAL_L1L5);

			// add ambiguity
			equL1E.addVariable(BL1E, 1.0);
			equL5E.addVariable(BL5E, 1.0);

			// set weight
			equC1E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC5E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equL1E.setWeight(SQR(1 / errmeasLE));
			equL5E.setWeight(SQR(1 / errmeasLE));

			// BDS
			Equation equC2C(TypeID::prefitC2C, SatelliteSystem::BeiDou);
			Equation equL2C(TypeID::prefitL2C, SatelliteSystem::BeiDou);
			Equation equC6C(TypeID::prefitC6C, SatelliteSystem::BeiDou);
			Equation equL6C(TypeID::prefitL6C, SatelliteSystem::BeiDou);
			Equation equC1C(TypeID::prefitC1C, SatelliteSystem::BeiDou);
			Equation equL1C(TypeID::prefitL1C, SatelliteSystem::BeiDou);
			Equation equC5C(TypeID::prefitC5C, SatelliteSystem::BeiDou);
			Equation equL5C(TypeID::prefitL5C, SatelliteSystem::BeiDou);

			// add xyz
			equC2C.addVarSet(xyzVarSet);
			equL2C.addVarSet(xyzVarSet);
			equC6C.addVarSet(xyzVarSet);
			equL6C.addVarSet(xyzVarSet);
			equC1C.addVarSet(xyzVarSet);
			equL1C.addVarSet(xyzVarSet);
			equC5C.addVarSet(xyzVarSet);
			equL5C.addVarSet(xyzVarSet);

			// add clk
			equC2C.addVariable(dcdtBDS, 1.0);
			equL2C.addVariable(dcdtBDS, 1.0);
			equC6C.addVariable(dcdtBDS, 1.0);
			equL6C.addVariable(dcdtBDS, 1.0);
			equC1C.addVariable(dcdtBDS, 1.0);
			equL1C.addVariable(dcdtBDS, 1.0);
			equC5C.addVariable(dcdtBDS, 1.0);
			equL5C.addVariable(dcdtBDS, 1.0);

			// add tropo
			equC2C.addVariable(tropo);
			equL2C.addVariable(tropo);
			equC6C.addVariable(tropo);
			equL6C.addVariable(tropo);
			equC1C.addVariable(tropo);
			equL1C.addVariable(tropo);
			equC5C.addVariable(tropo);
			equL5C.addVariable(tropo);
			// add iono
			if (bdsfopt)
			{
				equC1C.addVariable(ionoC1C, 1.0);
				equL1C.addVariable(ionoC1C, -1.0);
				equC5C.addVariable(ionoC1C, GAMMA_BDS_L1L5);
				equL5C.addVariable(ionoC1C, -GAMMA_BDS_L1L5);
			}
			else
			{
				equC2C.addVariable(ionoC2C, 1.0);
				equL2C.addVariable(ionoC2C, -1.0);
				equC6C.addVariable(ionoC2C, GAMMA_BDS_L2L6);
				equL6C.addVariable(ionoC2C, -GAMMA_BDS_L2L6);
			}

			// add ambiguity
			equL1C.addVariable(BL1C, 1.0);
			equL5C.addVariable(BL5C, 1.0);
			equL2C.addVariable(BL2C, 1.0);
			equL6C.addVariable(BL6C, 1.0);

			// set weight
			equC1C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC5C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC2C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC6C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equL1C.setWeight(SQR(1 / errmeasLC));
			equL5C.setWeight(SQR(1 / errmeasLC));
			equL2C.setWeight(SQR(1 / errmeasLC));
			equL6C.setWeight(SQR(1 / errmeasLC));

			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equC5E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equC5C);
					}
					else
					{
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equC6C);
					}
				}	// End of if !bdsComb
				
			}	// end of if codeOnly
			else
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equL1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equL2G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equL1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equL5E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equL1C);
						equSystem.addEquation(equC5C);
						equSystem.addEquation(equL5C);
					}
					else
					{
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equL2C);
						equSystem.addEquation(equC6C);
						equSystem.addEquation(equL6C);
					}
				}	//End of if !bdsComb
				
			}	// end of if !codeOnly
		}	//end of Options::UC12
		else if (ionoopt == Options::IF12)
		{
			// GPS
			Equation equPC12G(TypeID::prefitPC12G, SatelliteSystem::GPS);
			Equation equLC12G(TypeID::prefitLC12G, SatelliteSystem::GPS);

			// insert variable
			equPC12G.addVarSet(xyzVarSet);
			equLC12G.addVarSet(xyzVarSet);
			// insert tropo
			equPC12G.addVariable(tropo);
			equLC12G.addVariable(tropo);
			// clock
			equPC12G.addVariable(dcdtGPS, 1.0);
			equLC12G.addVariable(dcdtGPS, 1.0);
			// ambiguity
			equLC12G.addVariable(BLC12G, 1.0);

			// set weight
			equPC12G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equLC12G.setWeight(SQR(1/errmeasLG));

			// GAL
			Equation equPC15E(TypeID::prefitPC15E, SatelliteSystem::Galileo);
			Equation equLC15E(TypeID::prefitLC15E, SatelliteSystem::Galileo);
			
			// insert variable
			equPC15E.addVarSet(xyzVarSet);
			equLC15E.addVarSet(xyzVarSet);
			// clk
			equPC15E.addVariable(dcdtGAL, 1.0);
			equLC15E.addVariable(dcdtGAL, 1.0);
			// tropo
			equPC15E.addVariable(tropo);
			equLC15E.addVariable(tropo);
			//ambiguity
			equLC15E.addVariable(BLC15E, 1.0);
			//set weight
			equPC15E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equLC15E.setWeight(SQR(1 / errmeasLE));

			// BDS
			Equation equPC15C(TypeID::prefitPC15C, SatelliteSystem::BeiDou);
			Equation equLC15C(TypeID::prefitLC15C, SatelliteSystem::BeiDou);
			Equation equPC26C(TypeID::prefitPC26C, SatelliteSystem::BeiDou);
			Equation equLC26C(TypeID::prefitLC26C, SatelliteSystem::BeiDou);

			if (bdsfopt)
			{
				// insert xyz variable
				equPC15C.addVarSet(xyzVarSet);
				equLC15C.addVarSet(xyzVarSet);
				// add clk
				equPC15C.addVariable(dcdtBDS, 1.0);
				equLC15C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC15C.addVariable(tropo);
				equLC15C.addVariable(tropo);
				// add ambiguity
				equLC15C.addVariable(BLC15C, 1.0);
				// set weight
				equPC15C.setWeight(SQR(1/(errmeasLC*errratioC)));
				equLC15C.setWeight(SQR(1/errmeasLC));
			}
			else
			{
				// add xyz
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);
				// add clk
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);
				// add ambiguity
				equLC26C.addVariable(BLC26C, 1.0);
				// set weight
				equPC26C.setWeight(SQR(1/(errmeasLC*errratioC)));
				equLC26C.setWeight(SQR(1/errmeasLC));
			}

			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equPC15E);
				// bds 
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
					}
				}				
			}
			else
			{
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equLC12G);
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equLC15E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equLC15C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equLC26C);
					}
				}
				
			}	// end of if codeOnly

		}	// end of if(ionoopt == IF12)
		else if (ionoopt == Options::UC123)
		{
			Equation equC1G(TypeID::prefitC1G, SatelliteSystem::GPS);
			Equation equL1G(TypeID::prefitL1G, SatelliteSystem::GPS);
			Equation equC2G(TypeID::prefitC2G, SatelliteSystem::GPS);
			Equation equL2G(TypeID::prefitL2G, SatelliteSystem::GPS);
			Equation equC5G(TypeID::prefitC5G, SatelliteSystem::GPS);
			Equation equL5G(TypeID::prefitL5G, SatelliteSystem::GPS);
			// add variable
			equC1G.addVarSet(xyzVarSet);
			equL1G.addVarSet(xyzVarSet);
			equC2G.addVarSet(xyzVarSet);
			equL2G.addVarSet(xyzVarSet);
			equC5G.addVarSet(xyzVarSet);
			equL5G.addVarSet(xyzVarSet);
			// add dcdt
			equC1G.addVariable(dcdtGPS, 1.0);
			equL1G.addVariable(dcdtGPS, 1.0);
			equC2G.addVariable(dcdtGPS, 1.0);
			equL2G.addVariable(dcdtGPS, 1.0);
			equC5G.addVariable(dcdtGPS, 1.0);
			equL5G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equC1G.addVariable(tropo);
			equL1G.addVariable(tropo);
			equC2G.addVariable(tropo);
			equL2G.addVariable(tropo);
			equC5G.addVariable(tropo);
			equL5G.addVariable(tropo);
			// iono
			equC1G.addVariable(ionoC1G, 1.0);
			equL1G.addVariable(ionoC1G, -1.0);
			equC2G.addVariable(ionoC1G, GAMMA_GPS_L1L2);
			equL2G.addVariable(ionoC1G, -GAMMA_GPS_L1L2);
			equC5G.addVariable(ionoC1G, GAMMA_GPS_L1L5);
			equL5G.addVariable(ionoC1G, -GAMMA_GPS_L1L5);
			// ifb
			if (opts.PPPModel==Options::IFCB_IFB||
				opts.PPPModel==Options::IFCB)
			{
				equC5G.addVariable(ifcbL5G, 1.0);
				equC5G.addVariable(ifcbL5G, -1.0);
			}
			else if(opts.PPPModel==Options::IFB)
				equC5G.addVariable(ifbC5G, 1.0);

			// ambiguity
			equL1G.addVariable(BL1G, 1.0);
			equL2G.addVariable(BL2G, 1.0);
			equL5G.addVariable(BL5G, 1.0);
			// set weight
			equC1G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equC2G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equC5G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equL1G.setWeight(SQR(1 / errmeasLG));
			equL2G.setWeight(SQR(1 / errmeasLG));
			equL5G.setWeight(SQR(1 / errmeasLG));

			//gal
			Equation equC1E(TypeID::prefitC1E, SatelliteSystem::Galileo);
			Equation equL1E(TypeID::prefitL1E, SatelliteSystem::Galileo);
			Equation equC5E(TypeID::prefitC5E, SatelliteSystem::Galileo);
			Equation equL5E(TypeID::prefitL5E, SatelliteSystem::Galileo);
			Equation equC7E(TypeID::prefitC7E, SatelliteSystem::Galileo);
			Equation equL7E(TypeID::prefitL7E, SatelliteSystem::Galileo);

			//insert xyz
			equC1E.addVarSet(xyzVarSet);
			equL1E.addVarSet(xyzVarSet);
			equC5E.addVarSet(xyzVarSet);
			equL5E.addVarSet(xyzVarSet);
			equC7E.addVarSet(xyzVarSet);
			equL7E.addVarSet(xyzVarSet);
			// add dcdt
			equC1E.addVariable(dcdtGAL, 1.0);
			equL1E.addVariable(dcdtGAL, 1.0);
			equC5E.addVariable(dcdtGAL, 1.0);
			equL5E.addVariable(dcdtGAL, 1.0);
			equC7E.addVariable(dcdtGAL, 1.0);
			equL7E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equC1E.addVariable(tropo);
			equL1E.addVariable(tropo);
			equC5E.addVariable(tropo);
			equL5E.addVariable(tropo);
			equC7E.addVariable(tropo);
			equL7E.addVariable(tropo);
			// add iono
			equC1E.addVariable(ionoC1E, 1.0);
			equL1E.addVariable(ionoC1E, -1.0);
			equC5E.addVariable(ionoC1E, GAMMA_GAL_L1L5);
			equL5E.addVariable(ionoC1E, -GAMMA_GAL_L1L5);
			equC7E.addVariable(ionoC1E, GAMMA_GAL_L1L7);
			equL7E.addVariable(ionoC1E, -GAMMA_GAL_L1L7);
			// add ifb
			if (opts.PPPModel==Options::IFCB)
			{
				equC7E.addVariable(ifcbL7E, 1.0);
				equL7E.addVariable(ifcbL7E, -1.0);
			}
			else if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
			{
				equC7E.addVariable(ifbC7E, 1.0);
			}
			
			// add ambiguity
			equL1E.addVariable(BL1E, 1.0);
			equL5E.addVariable(BL5E, 1.0);
			equL7E.addVariable(BL7E, 1.0);

			// set weight
			equC1E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC5E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC7E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equL1E.setWeight(SQR(1 / errmeasLE));
			equL5E.setWeight(SQR(1 / errmeasLE));
			equL7E.setWeight(SQR(1 / errmeasLE));


			// BDS
			Equation equC2C(TypeID::prefitC2C, SatelliteSystem::BeiDou);
			Equation equL2C(TypeID::prefitL2C, SatelliteSystem::BeiDou);
			Equation equC6C(TypeID::prefitC6C, SatelliteSystem::BeiDou);
			Equation equL6C(TypeID::prefitL6C, SatelliteSystem::BeiDou);
			Equation equC1C(TypeID::prefitC1C, SatelliteSystem::BeiDou);
			Equation equL1C(TypeID::prefitL1C, SatelliteSystem::BeiDou);
			Equation equC5C(TypeID::prefitC5C, SatelliteSystem::BeiDou);
			Equation equL5C(TypeID::prefitL5C, SatelliteSystem::BeiDou);

			// bds2
			Equation equC7C(TypeID::prefitC7C, SatelliteSystem::BeiDou);
			Equation equL7C(TypeID::prefitL7C, SatelliteSystem::BeiDou);

			// add xyz
			equC2C.addVarSet(xyzVarSet);
			equL2C.addVarSet(xyzVarSet);
			equC6C.addVarSet(xyzVarSet);
			equL6C.addVarSet(xyzVarSet);
			equC1C.addVarSet(xyzVarSet);
			equL1C.addVarSet(xyzVarSet);
			equC5C.addVarSet(xyzVarSet);
			equL5C.addVarSet(xyzVarSet);

			// add clk
			equC2C.addVariable(dcdtBDS, 1.0);
			equL2C.addVariable(dcdtBDS, 1.0);
			equC6C.addVariable(dcdtBDS, 1.0);
			equL6C.addVariable(dcdtBDS, 1.0);
			equC1C.addVariable(dcdtBDS, 1.0);
			equL1C.addVariable(dcdtBDS, 1.0);
			equC5C.addVariable(dcdtBDS, 1.0);
			equL5C.addVariable(dcdtBDS, 1.0);

			// add tropo
			equC2C.addVariable(tropo);
			equL2C.addVariable(tropo);
			equC6C.addVariable(tropo);
			equL6C.addVariable(tropo);
			equC1C.addVariable(tropo);
			equL1C.addVariable(tropo);
			equC5C.addVariable(tropo);
			equL5C.addVariable(tropo);
			// add iono
			if (bdsfopt)
			{
				equC1C.addVariable(ionoC1C, 1.0);
				equL1C.addVariable(ionoC1C, -1.0);
				equC5C.addVariable(ionoC1C, GAMMA_BDS_L1L5);
				equL5C.addVariable(ionoC1C, -GAMMA_BDS_L1L5);
				equC2C.addVariable(ionoC1C, GAMMA_BDS_L1L2);
				equL2C.addVariable(ionoC1C, -GAMMA_BDS_L1L2);
				// add ifb
				if(opts.PPPModel==Options::IFCB_IFB||
					opts.PPPModel==Options::IFB)
					equC2C.addVariable(ifbC2C, 1.0);
				else if (opts.PPPModel == Options::IFCB)
				{
					equC2C.addVariable(ifcbL2C, 1.0);
					equL2C.addVariable(ifcbL2C, -1.0);
				}
			}
			else
			{
				equC2C.addVariable(ionoC2C, 1.0);
				equL2C.addVariable(ionoC2C, -1.0);
				equC6C.addVariable(ionoC2C, GAMMA_BDS_L2L6);
				equL6C.addVariable(ionoC2C, -GAMMA_BDS_L2L6);
				equC1C.addVariable(ionoC2C, GAMMA_BDS_L2L1);
				equL1C.addVariable(ionoC2C, -GAMMA_BDS_L2L1);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
					equC1C.addVariable(ifbC1C, 1.0);
				else if (opts.PPPModel == Options::IFCB)
				{
					equC1C.addVariable(ifcbL1C, 1.0);
					equL1C.addVariable(ifcbL1C, -1.0);
				}
			}

			// add ambiguity
			equL1C.addVariable(BL1C, 1.0);
			equL5C.addVariable(BL5C, 1.0);
			equL2C.addVariable(BL2C, 1.0);
			equL6C.addVariable(BL6C, 1.0);

			// set weight
			equC1C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC5C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC2C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC6C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equL1C.setWeight(SQR(1 / errmeasLC));
			equL5C.setWeight(SQR(1 / errmeasLC));
			equL2C.setWeight(SQR(1 / errmeasLC));
			equL6C.setWeight(SQR(1 / errmeasLC));

			if (bds2)
			{
				// add xyz
				equC7C.addVarSet(xyzVarSet);
				equL7C.addVarSet(xyzVarSet);
				// add clk
				equC7C.addVariable(dcdtBDS, 1.0);
				equL7C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equC7C.addVariable(tropo);
				equL7C.addVariable(tropo);
				// iono
				equC7C.addVariable(ionoC2C, GAMMA_BDS_L2L7);
				equL7C.addVariable(ionoC2C, -GAMMA_BDS_L2L7);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
					equC7C.addVariable(ifbC7C, 1.0);
				else if (opts.PPPModel == Options::IFCB)
				{
					equC7C.addVariable(ifcbL7C, 1.0);
					equL7C.addVariable(ifcbL7C, -1.0);
				}
				//ambiguity
				equL7C.addVariable(BL7C, 1.0);
				// set weight
				equC7C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equL7C.setWeight(SQR(1 / errmeasLC));

			}	// end of if bds2


			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equC7E);
				// bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equC5C);
						equSystem.addEquation(equC2C);
					}
					else
					{
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equC6C);
						equSystem.addEquation(equC1C);
					}

					if (bds2)
					{
						equSystem.addEquation(equC7C);
					}
				}
				
			}	// end of if codeOnly
			else
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equL1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equL2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equL5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equL1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equL5E);
				equSystem.addEquation(equC7E);
				equSystem.addEquation(equL7E);
				// bds 
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equL1C);
						equSystem.addEquation(equC5C);
						equSystem.addEquation(equL5C);
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equL2C);
					}
					else
					{
						equSystem.addEquation(equC1C);
						equSystem.addEquation(equL1C);
						equSystem.addEquation(equC2C);
						equSystem.addEquation(equL2C);
						equSystem.addEquation(equC6C);
						equSystem.addEquation(equL6C);
					}

					if (bds2)
					{
						equSystem.addEquation(equC7C);
						equSystem.addEquation(equL7C);
					}
				}	// end of if !bdsComb
				
			}	// end of if !codeOnly
		}	//end of if ionoopt == options::uc123
		else if (ionoopt == Options::IF1213)
		{
			// gps
			Equation equPC12G(TypeID::prefitPC12G, SatelliteSystem::GPS);
			Equation equPC15G(TypeID::prefitPC15G, SatelliteSystem::GPS);
			Equation equLC12G(TypeID::prefitLC12G, SatelliteSystem::GPS);
			Equation equLC15G(TypeID::prefitLC15G, SatelliteSystem::GPS);

			//gal
			Equation equPC15E(TypeID::prefitPC15E, SatelliteSystem::Galileo);
			Equation equLC15E(TypeID::prefitLC15E, SatelliteSystem::Galileo);
			Equation equPC17E(TypeID::prefitPC17E, SatelliteSystem::Galileo);
			Equation equLC17E(TypeID::prefitLC17E, SatelliteSystem::Galileo);

			// bds2
			Equation equPC27C(TypeID::prefitPC27C, SatelliteSystem::BeiDou);
			Equation equLC27C(TypeID::prefitLC27C, SatelliteSystem::BeiDou);
			// bds3
			Equation equPC15C(TypeID::prefitPC15C, SatelliteSystem::BeiDou);
			Equation equLC15C(TypeID::prefitLC15C, SatelliteSystem::BeiDou);
			// bds3 normal
			Equation equPC26C(TypeID::prefitPC26C, SatelliteSystem::BeiDou);
			Equation equLC26C(TypeID::prefitLC26C, SatelliteSystem::BeiDou);

			// change
			Equation equPC52C(TypeID::prefitPC52C, SatelliteSystem::BeiDou);
			Equation equLC52C(TypeID::prefitLC52C, SatelliteSystem::BeiDou);

			Equation equPC61C(TypeID::prefitPC61C, SatelliteSystem::BeiDou);
			Equation equLC61C(TypeID::prefitLC61C, SatelliteSystem::BeiDou);

			// add xyz
			equPC12G.addVarSet(xyzVarSet);
			equLC12G.addVarSet(xyzVarSet);
			equPC15G.addVarSet(xyzVarSet);
			equLC15G.addVarSet(xyzVarSet);
			// add clk
			equPC12G.addVariable(dcdtGPS, 1.0);
			equLC12G.addVariable(dcdtGPS, 1.0);
			equPC15G.addVariable(dcdtGPS, 1.0);
			equLC15G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC12G.addVariable(tropo);
			equLC12G.addVariable(tropo);
			equPC15G.addVariable(tropo);
			equLC15G.addVariable(tropo);
			// add ifb
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFCB)
			{
				equPC15G.addVariable(ifcbL5G, 1.0);
				equLC15G.addVariable(ifcbL5G, -1.0);
			}
			else if (opts.PPPModel == Options::IFB)
			{
				equPC15G.addVariable(ifbC5G, 1.0);
			}
			//add ambiguity
			equLC12G.addVariable(BLC12G, 1.0);
			equLC15G.addVariable(BLC15G, 1.0);
			// set weight
			// The weight under the current 
			// combination does not need to be multiplied by fact.
			equPC12G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equPC15G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equLC12G.setWeight(SQR(1 / errmeasLG));
			equLC15G.setWeight(SQR(1 / errmeasLG));

			//equPC12G.setWeight(SQR((errmeasLG * errratioG)));
			//equPC15G.setWeight(SQR((errmeasLG * errratioG)));
			//equLC12G.setWeight(SQR(errmeasLG));
			//equLC15G.setWeight(SQR(errmeasLG));

			// gal
			equPC15E.addVarSet(xyzVarSet);
			equLC15E.addVarSet(xyzVarSet);
			equPC17E.addVarSet(xyzVarSet);
			equLC17E.addVarSet(xyzVarSet);
			// add clk
			equPC15E.addVariable(dcdtGAL, 1.0);
			equLC15E.addVariable(dcdtGAL, 1.0);
			equPC17E.addVariable(dcdtGAL, 1.0);
			equLC17E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equPC15E.addVariable(tropo);
			equLC15E.addVariable(tropo);
			equPC17E.addVariable(tropo);
			equLC17E.addVariable(tropo);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
				equPC17E.addVariable(ifbC7E, 1.0);
			else if (opts.PPPModel == Options::IFCB)
			{
				equPC17E.addVariable(ifcbL7E, 1.0);
				equLC17E.addVariable(ifcbL7E, -1.0);
			}
			//add ambiguity
			equLC15E.addVariable(BLC15E, 1.0);
			equLC17E.addVariable(BLC17E, 1.0);

			// set weight
			equPC15E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equPC17E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equLC15E.setWeight(SQR(1 / errmeasLE));
			equLC17E.setWeight(SQR(1 / errmeasLE));

			// bds
			if (bdsfopt)
			{
				// normal pair-wise combination
				// xyz
				equPC15C.addVarSet(xyzVarSet);
				equLC15C.addVarSet(xyzVarSet);

				// clk
				equPC15C.addVariable(dcdtBDS, 1.0);
				equLC15C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC15C.addVariable(tropo);
				equLC15C.addVariable(tropo);


				// 52
				equPC52C.addVarSet(xyzVarSet);
				equLC52C.addVarSet(xyzVarSet);
				equPC52C.addVariable(dcdtBDS, 1.0);
				equLC52C.addVariable(dcdtBDS, 1.0);
				equPC52C.addVariable(tropo);
				equLC52C.addVariable(tropo);

				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equPC52C.addVariable(ifbC2C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equLC52C.addVariable(ifcbL2C, 1.0);
					equLC52C.addVariable(ifcbL2C, -1.0);
				}
				// ambiguity
				equLC15C.addVariable(BLC15C, 1.0);
				equLC52C.addVariable(BLC52C, 1.0);
				// set weight
				equPC15C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC52C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC15C.setWeight(SQR(1 / errmeasLC));
				equLC52C.setWeight(SQR(1 / errmeasLC));

			}
			else
			{
				// add xyz
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);

				equPC61C.addVarSet(xyzVarSet);
				equLC61C.addVarSet(xyzVarSet);

				// add clk
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);

				equPC61C.addVariable(dcdtBDS, 1.0);
				equLC61C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);

				equPC61C.addVariable(tropo);
				equLC61C.addVariable(tropo);
				// add ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equPC61C.addVariable(ifbC1C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equPC61C.addVariable(ifcbL1C, 1.0);
					equLC61C.addVariable(ifcbL1C, -1.0);
				}
				// add ambiguity
				equLC26C.addVariable(BLC26C, 1.0);
				equLC61C.addVariable(BLC61C, 1.0);
				// set weight
				equPC26C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC61C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC26C.setWeight(SQR(1 / errmeasLC));
				equLC61C.setWeight(SQR(1 / errmeasLC));
				if (bds2)
				{
					equPC27C.addVarSet(xyzVarSet);
					equLC27C.addVarSet(xyzVarSet);
					// clk
					equPC27C.addVariable(dcdtBDS, 1.0);
					equLC27C.addVariable(dcdtBDS, 1.0);
					// tropo
					equPC27C.addVariable(tropo);
					equLC27C.addVariable(tropo);
					// ifb
					if (opts.PPPModel == Options::IFCB_IFB ||
						opts.PPPModel == Options::IFB)
						equPC27C.addVariable(ifbC7C, 1.0);
					else if (opts.PPPModel == Options::IFCB)
					{
						equPC27C.addVariable(ifcbL7C, 1.0);
						equLC27C.addVariable(ifcbL7C, -1.0);
					}
					
					// ambiguity
					equLC27C.addVariable(BLC27C, 1.0);
					// weight
					equPC27C.setWeight(SQR(1 / (errmeasLC * errratioC)));
					equLC27C.setWeight(SQR(1 / errmeasLC));
				}
			}	// end of if bdsfopt

			// equsystem add equs
			equSystem.clearEquations();
			if (codeOnly)
			{	// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equPC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equPC17E);
				//bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equPC52C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equPC61C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
						}
					}
				}	// end of if !bdsComb				

			}	// end of if codeOnly
			else
			{
				// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equLC12G);
				equSystem.addEquation(equPC15G);
				equSystem.addEquation(equLC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equLC15E);
				equSystem.addEquation(equPC17E);
				equSystem.addEquation(equLC17E);
				//bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equLC15C);
						equSystem.addEquation(equPC52C);
						equSystem.addEquation(equLC52C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equLC26C);
						equSystem.addEquation(equPC61C);
						equSystem.addEquation(equLC61C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
							equSystem.addEquation(equLC27C);
						}
					}
				}	// end of if !bdsComb
				
			}	// end of if !codeOnly
		}	// end of if if1213
		else if (ionoopt == Options::IF123)
		{
			// gps
			Equation equPC125G(TypeID::prefitPC125G, SatelliteSystem::GPS);
			Equation equLC125G(TypeID::prefitLC125G, SatelliteSystem::GPS);
			// gal
			Equation equPC157E(TypeID::prefitPC157E, SatelliteSystem::Galileo);
			Equation equLC157E(TypeID::prefitLC157E, SatelliteSystem::Galileo);


			// add xyz
			equPC125G.addVarSet(xyzVarSet);
			equLC125G.addVarSet(xyzVarSet);
			// add clk
			equPC125G.addVariable(dcdtGPS, 1.0);
			equLC125G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC125G.addVariable(tropo);
			equLC125G.addVariable(tropo);
			// add ifb
			if (opts.PPPModel==Options::IFB)
				equPC125G.addVariable(ifbC5G, 1.0);

			// add ambiguity
			equLC125G.addVariable(BLC125G, 1.0);
			//set weight
			equPC125G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equLC125G.setWeight(SQR(1 / errmeasLG));

			// gal
			equPC157E.addVarSet(xyzVarSet);
			equLC157E.addVarSet(xyzVarSet);
			// add clk
			equPC157E.addVariable(dcdtGAL, 1.0);
			equLC157E.addVariable(dcdtGAL, 1.0);
			// add trop
			equPC157E.addVariable(tropo);
			equLC157E.addVariable(tropo);
			// add ifb absorted into clk
			if (opts.PPPModel == Options::IFB)
				equPC157E.addVariable(ifbC7E, 1.0);
			// add ambiguity
			equLC157E.addVariable(BLC157E, 1.0);
			//set weight
			equPC157E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equLC157E.setWeight(SQR(1 / errmeasLE));

			// bds2
			Equation equPC267C(TypeID::prefitPC267C, SatelliteSystem::BeiDou);
			Equation equLC267C(TypeID::prefitLC267C, SatelliteSystem::BeiDou);
			if (bds2)
			{
				// add xyz
				equPC267C.addVarSet(xyzVarSet);
				equLC267C.addVarSet(xyzVarSet);
				// add clk
				equPC267C.addVariable(dcdtBDS, 1.0);
				equLC267C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC267C.addVariable(tropo);
				equLC267C.addVariable(tropo);
				// add ifb
				if (opts.PPPModel==Options::IFB)
					equPC267C.addVariable(ifbC7C, 1.0);
				// add ambiguity
				equLC267C.addVariable(BLC267C, 1.0);
				// set weight
				equPC267C.setWeight(SQR(1/(errmeasLC*errratioC)));
				equLC267C.setWeight(SQR(1 / errmeasLC));
			}
			//// bds3
			Equation equPC152C(TypeID::prefitPC152C, SatelliteSystem::BeiDou);
			Equation equLC152C(TypeID::prefitLC152C, SatelliteSystem::BeiDou);
			Equation equPC261C(TypeID::prefitPC261C, SatelliteSystem::BeiDou);
			Equation equLC261C(TypeID::prefitLC261C, SatelliteSystem::BeiDou);
			if (bdsfopt)
			{	//
				equPC152C.addVarSet(xyzVarSet);
				equLC152C.addVarSet(xyzVarSet);
				// clk
				equPC152C.addVariable(dcdtBDS, 1.0);
				equLC152C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC152C.addVariable(tropo);
				equLC152C.addVariable(tropo);
				//ifb
				if (opts.PPPModel == Options::IFB)
					equPC152C.addVariable(ifbC2C, 1.0);
				//ambiguity
				equLC152C.addVariable(BLC152C, 1.0);
				//set weight
				equPC152C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC152C.setWeight(SQR(1 / errmeasLC));
			}
			else
			{
				equPC261C.addVarSet(xyzVarSet);
				equLC261C.addVarSet(xyzVarSet);
				// clk
				equPC261C.addVariable(dcdtBDS, 1.0);
				equLC261C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC261C.addVariable(tropo);
				equLC261C.addVariable(tropo);
				//ifb
				if (opts.PPPModel == Options::IFB)
					equPC261C.addVariable(ifbC1C,1.0);
				//ambiguity
				equLC261C.addVariable(BLC261C, 1.0);
				//set weight
				equPC261C.setWeight(SQR(1 / (errmeasLC * errratioC)) );
				equLC261C.setWeight(SQR(1 / errmeasLC));
			}


			// add equ into equations
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equPC157E);
				// bds
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC152C);
					}
					else
					{
						equSystem.addEquation(equPC261C);
					}
				}	// End of if !bdsComb
				
			}
			else
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equLC125G);
				equSystem.addEquation(equPC157E);
				equSystem.addEquation(equLC157E);
				// bds
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
						equSystem.addEquation(equLC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC152C);
						equSystem.addEquation(equLC152C);
					}
					else
					{
						equSystem.addEquation(equPC261C);
						equSystem.addEquation(equLC261C);
					}
				}	// if !bdsComb
				
			}
		}	// End of if ionoopt == IF123
		else if (ionoopt == Options::UC1234)
		{
			Equation equC1G(TypeID::prefitC1G, SatelliteSystem::GPS);
			Equation equL1G(TypeID::prefitL1G, SatelliteSystem::GPS);
			Equation equC2G(TypeID::prefitC2G, SatelliteSystem::GPS);
			Equation equL2G(TypeID::prefitL2G, SatelliteSystem::GPS);
			Equation equC5G(TypeID::prefitC5G, SatelliteSystem::GPS);
			Equation equL5G(TypeID::prefitL5G, SatelliteSystem::GPS);
			// add variable
			equC1G.addVarSet(xyzVarSet);
			equL1G.addVarSet(xyzVarSet);
			equC2G.addVarSet(xyzVarSet);
			equL2G.addVarSet(xyzVarSet);
			equC5G.addVarSet(xyzVarSet);
			equL5G.addVarSet(xyzVarSet);
			// add dcdt
			equC1G.addVariable(dcdtGPS, 1.0);
			equL1G.addVariable(dcdtGPS, 1.0);
			equC2G.addVariable(dcdtGPS, 1.0);
			equL2G.addVariable(dcdtGPS, 1.0);
			equC5G.addVariable(dcdtGPS, 1.0);
			equL5G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equC1G.addVariable(tropo);
			equL1G.addVariable(tropo);
			equC2G.addVariable(tropo);
			equL2G.addVariable(tropo);
			equC5G.addVariable(tropo);
			equL5G.addVariable(tropo);
			// iono
			equC1G.addVariable(ionoC1G, 1.0);
			equL1G.addVariable(ionoC1G, -1.0);
			equC2G.addVariable(ionoC1G, GAMMA_GPS_L1L2);
			equL2G.addVariable(ionoC1G, -GAMMA_GPS_L1L2);
			equC5G.addVariable(ionoC1G, GAMMA_GPS_L1L5);
			equL5G.addVariable(ionoC1G, -GAMMA_GPS_L1L5);
			// ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFCB)
			{
				equC5G.addVariable(ifcbL5G, 1.0);
				equL5G.addVariable(ifcbL5G, -1.0);
			}
			else if (opts.PPPModel == Options::IFB)
				equC5G.addVariable(ifbC5G, 1.0);

			// ambiguity
			equL1G.addVariable(BL1G, 1.0);
			equL2G.addVariable(BL2G, 1.0);
			equL5G.addVariable(BL5G, 1.0);
			// set weight
			equC1G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equC2G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equC5G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equL1G.setWeight(SQR(1 / errmeasLG));
			equL2G.setWeight(SQR(1 / errmeasLG));
			equL5G.setWeight(SQR(1 / errmeasLG));

			//gal
			Equation equC1E(TypeID::prefitC1E, SatelliteSystem::Galileo);
			Equation equL1E(TypeID::prefitL1E, SatelliteSystem::Galileo);
			Equation equC5E(TypeID::prefitC5E, SatelliteSystem::Galileo);
			Equation equL5E(TypeID::prefitL5E, SatelliteSystem::Galileo);
			Equation equC7E(TypeID::prefitC7E, SatelliteSystem::Galileo);
			Equation equL7E(TypeID::prefitL7E, SatelliteSystem::Galileo);
			Equation equC8E(TypeID::prefitC8E, SatelliteSystem::Galileo);
			Equation equL8E(TypeID::prefitL8E, SatelliteSystem::Galileo);

			//insert xyz
			equC1E.addVarSet(xyzVarSet);
			equL1E.addVarSet(xyzVarSet);
			equC5E.addVarSet(xyzVarSet);
			equL5E.addVarSet(xyzVarSet);
			equC7E.addVarSet(xyzVarSet);
			equL7E.addVarSet(xyzVarSet);
			equC8E.addVarSet(xyzVarSet);
			equL8E.addVarSet(xyzVarSet);
			// add dcdt
			equC1E.addVariable(dcdtGAL, 1.0);
			equL1E.addVariable(dcdtGAL, 1.0);
			equC5E.addVariable(dcdtGAL, 1.0);
			equL5E.addVariable(dcdtGAL, 1.0);
			equC7E.addVariable(dcdtGAL, 1.0);
			equL7E.addVariable(dcdtGAL, 1.0);
			equC8E.addVariable(dcdtGAL, 1.0);
			equL8E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equC1E.addVariable(tropo);
			equL1E.addVariable(tropo);
			equC5E.addVariable(tropo);
			equL5E.addVariable(tropo);
			equC7E.addVariable(tropo);
			equL7E.addVariable(tropo);
			equC8E.addVariable(tropo);
			equL8E.addVariable(tropo);
			// add iono
			equC1E.addVariable(ionoC1E, 1.0);
			equL1E.addVariable(ionoC1E, -1.0);
			equC5E.addVariable(ionoC1E, GAMMA_GAL_L1L5);
			equL5E.addVariable(ionoC1E, -GAMMA_GAL_L1L5);
			equC7E.addVariable(ionoC1E, GAMMA_GAL_L1L7);
			equL7E.addVariable(ionoC1E, -GAMMA_GAL_L1L7);
			equC8E.addVariable(ionoC1E, GAMMA_GAL_L1L8);
			equL8E.addVariable(ionoC1E, -GAMMA_GAL_L1L8);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
			{
				equC7E.addVariable(ifbC7E, 1.0);
				equC8E.addVariable(ifbC8E, 1.0);
			}
			else if (opts.PPPModel == Options::IFCB)
			{
				equC7E.addVariable(ifcbL7E, 1.0);
				equL7E.addVariable(ifcbL7E, -1.0);
				equC8E.addVariable(ifcbL8E, 1.0);
				equL8E.addVariable(ifcbL8E, -1.0);
			}
			
			// add ambiguity
			equL1E.addVariable(BL1E, 1.0);
			equL5E.addVariable(BL5E, 1.0);
			equL7E.addVariable(BL7E, 1.0);
			equL8E.addVariable(BL8E, 1.0);

			// set weight
			equC1E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC5E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC7E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equC8E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equL1E.setWeight(SQR(1 / errmeasLE));
			equL5E.setWeight(SQR(1 / errmeasLE));
			equL7E.setWeight(SQR(1 / errmeasLE));
			equL8E.setWeight(SQR(1 / errmeasLE));


			// BDS
			Equation equC2C(TypeID::prefitC2C, SatelliteSystem::BeiDou);
			Equation equL2C(TypeID::prefitL2C, SatelliteSystem::BeiDou);
			Equation equC6C(TypeID::prefitC6C, SatelliteSystem::BeiDou);
			Equation equL6C(TypeID::prefitL6C, SatelliteSystem::BeiDou);
			Equation equC1C(TypeID::prefitC1C, SatelliteSystem::BeiDou);
			Equation equL1C(TypeID::prefitL1C, SatelliteSystem::BeiDou);
			Equation equC5C(TypeID::prefitC5C, SatelliteSystem::BeiDou);
			Equation equL5C(TypeID::prefitL5C, SatelliteSystem::BeiDou);

			// bds2
			Equation equC7C(TypeID::prefitC7C, SatelliteSystem::BeiDou);
			Equation equL7C(TypeID::prefitL7C, SatelliteSystem::BeiDou);

			// add xyz
			equC2C.addVarSet(xyzVarSet);
			equL2C.addVarSet(xyzVarSet);
			equC6C.addVarSet(xyzVarSet);
			equL6C.addVarSet(xyzVarSet);
			equC1C.addVarSet(xyzVarSet);
			equL1C.addVarSet(xyzVarSet);
			equC5C.addVarSet(xyzVarSet);
			equL5C.addVarSet(xyzVarSet);

			// add clk
			equC2C.addVariable(dcdtBDS, 1.0);
			equL2C.addVariable(dcdtBDS, 1.0);
			equC6C.addVariable(dcdtBDS, 1.0);
			equL6C.addVariable(dcdtBDS, 1.0);
			equC1C.addVariable(dcdtBDS, 1.0);
			equL1C.addVariable(dcdtBDS, 1.0);
			equC5C.addVariable(dcdtBDS, 1.0);
			equL5C.addVariable(dcdtBDS, 1.0);

			// add tropo
			equC2C.addVariable(tropo);
			equL2C.addVariable(tropo);
			equC6C.addVariable(tropo);
			equL6C.addVariable(tropo);
			equC1C.addVariable(tropo);
			equL1C.addVariable(tropo);
			equC5C.addVariable(tropo);
			equL5C.addVariable(tropo);
			// add iono
			if (bdsfopt)
			{
				equC1C.addVariable(ionoC1C, 1.0);
				equL1C.addVariable(ionoC1C, -1.0);
				equC5C.addVariable(ionoC1C, GAMMA_BDS_L1L5);
				equL5C.addVariable(ionoC1C, -GAMMA_BDS_L1L5);
				equC2C.addVariable(ionoC1C, GAMMA_BDS_L1L2);
				equL2C.addVariable(ionoC1C, -GAMMA_BDS_L1L2);
				equC6C.addVariable(ionoC1C, GAMMA_BDS_L1L6);
				equL6C.addVariable(ionoC1C, -GAMMA_BDS_L1L6);
				// add ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equC2C.addVariable(ifbC2C, 1.0);
					equC6C.addVariable(ifbC6C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equC2C.addVariable(ifcbL2C, 1.0);
					equL2C.addVariable(ifcbL2C, -1.0);
					equC6C.addVariable(ifcbL6C, 1.0);
					equL6C.addVariable(ifcbL6C, -1.0);
				}
				
			}
			else
			{
				equC2C.addVariable(ionoC2C, 1.0);
				equL2C.addVariable(ionoC2C, -1.0);
				equC6C.addVariable(ionoC2C, GAMMA_BDS_L2L6);
				equL6C.addVariable(ionoC2C, -GAMMA_BDS_L2L6);
				equC1C.addVariable(ionoC2C, GAMMA_BDS_L2L1);
				equL1C.addVariable(ionoC2C, -GAMMA_BDS_L2L1);
				equC5C.addVariable(ionoC2C, GAMMA_BDS_L2L5);
				equL5C.addVariable(ionoC2C, -GAMMA_BDS_L2L5);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equC1C.addVariable(ifbC1C, 1.0);
					equC5C.addVariable(ifbC5C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equC1C.addVariable(ifcbL1C, 1.0);
					equL1C.addVariable(ifcbL1C, -1.0);
					equC5C.addVariable(ifcbL5C, 1.0);
					equL5C.addVariable(ifcbL5C, -1.0);
				}
				
			}

			// add ambiguity
			equL1C.addVariable(BL1C, 1.0);
			equL5C.addVariable(BL5C, 1.0);
			equL2C.addVariable(BL2C, 1.0);
			equL6C.addVariable(BL6C, 1.0);

			// set weight
			equC1C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC5C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC2C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equC6C.setWeight(SQR(1 / (errmeasLC * errratioC)));
			equL1C.setWeight(SQR(1 / errmeasLC));
			equL5C.setWeight(SQR(1 / errmeasLC));
			equL2C.setWeight(SQR(1 / errmeasLC));
			equL6C.setWeight(SQR(1 / errmeasLC));

			if (bds2)
			{
				// add xyz
				equC7C.addVarSet(xyzVarSet);
				equL7C.addVarSet(xyzVarSet);
				// add clk
				equC7C.addVariable(dcdtBDS, 1.0);
				equL7C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equC7C.addVariable(tropo);
				equL7C.addVariable(tropo);
				// iono
				equC7C.addVariable(ionoC2C, GAMMA_BDS_L2L7);
				equL7C.addVariable(ionoC2C, -GAMMA_BDS_L2L7);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
					equC7C.addVariable(ifbC7C, 1.0);
				else if (opts.PPPModel == Options::IFCB)
				{
					equC7C.addVariable(ifcbL7C, 1.0);
					equL7C.addVariable(ifcbL7C, -1.0);
				}
				
				//ambiguity
				equL7C.addVariable(BL7C, 1.0);
				// set weight
				equC7C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equL7C.setWeight(SQR(1 / errmeasLC));

			}	// end of if bds2


			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equC7E);
				equSystem.addEquation(equC8E);
				// bds
				if (!bdsComb)
				{
					equSystem.addEquation(equC1C);
					equSystem.addEquation(equC5C);
					equSystem.addEquation(equC2C);
					equSystem.addEquation(equC6C);
					if (bds2)
					{
						equSystem.addEquation(equC7C);
					}
				}	//End of if !bdsComb
			}	// end of if codeOnly
			else
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equL1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equL2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equL5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equL1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equL5E);
				equSystem.addEquation(equC7E);
				equSystem.addEquation(equL7E);
				equSystem.addEquation(equC8E);
				equSystem.addEquation(equL8E);
				// bds
				if (!bdsComb)
				{
					equSystem.addEquation(equC1C);
					equSystem.addEquation(equL1C);
					equSystem.addEquation(equC5C);
					equSystem.addEquation(equL5C);
					equSystem.addEquation(equC2C);
					equSystem.addEquation(equL2C);
					equSystem.addEquation(equC6C);
					equSystem.addEquation(equL6C);
					if (bds2)
					{
						equSystem.addEquation(equC7C);
						equSystem.addEquation(equL7C);
					}
				}	// end of if !bdsComb				
			}	// end of if !codeOnly
		}	// end of if UC1234
		else if (ionoopt == Options::IF121314)
		{
			// gps
			Equation equPC12G(TypeID::prefitPC12G, SatelliteSystem::GPS);
			Equation equPC15G(TypeID::prefitPC15G, SatelliteSystem::GPS);
			Equation equLC12G(TypeID::prefitLC12G, SatelliteSystem::GPS);
			Equation equLC15G(TypeID::prefitLC15G, SatelliteSystem::GPS);

			//gal
			Equation equPC15E(TypeID::prefitPC15E, SatelliteSystem::Galileo);
			Equation equLC15E(TypeID::prefitLC15E, SatelliteSystem::Galileo);
			Equation equPC17E(TypeID::prefitPC17E, SatelliteSystem::Galileo);
			Equation equLC17E(TypeID::prefitLC17E, SatelliteSystem::Galileo);
			Equation equPC18E(TypeID::prefitPC18E, SatelliteSystem::Galileo);
			Equation equLC18E(TypeID::prefitLC18E, SatelliteSystem::Galileo);

			// bds2
			Equation equPC27C(TypeID::prefitPC27C, SatelliteSystem::BeiDou);
			Equation equLC27C(TypeID::prefitLC27C, SatelliteSystem::BeiDou);
			// bds3
			Equation equPC15C(TypeID::prefitPC15C, SatelliteSystem::BeiDou);
			Equation equLC15C(TypeID::prefitLC15C, SatelliteSystem::BeiDou);
			Equation equPC16C(TypeID::prefitPC16C, SatelliteSystem::BeiDou);
			Equation equLC16C(TypeID::prefitLC16C, SatelliteSystem::BeiDou);
			// bds3 normal
			Equation equPC26C(TypeID::prefitPC26C, SatelliteSystem::BeiDou);
			Equation equLC26C(TypeID::prefitLC26C, SatelliteSystem::BeiDou);
			Equation equPC25C(TypeID::prefitPC25C, SatelliteSystem::BeiDou);
			Equation equLC25C(TypeID::prefitLC25C, SatelliteSystem::BeiDou);

			// change
			Equation equPC52C(TypeID::prefitPC52C, SatelliteSystem::BeiDou);
			Equation equLC52C(TypeID::prefitLC52C, SatelliteSystem::BeiDou);

			Equation equPC61C(TypeID::prefitPC61C, SatelliteSystem::BeiDou);
			Equation equLC61C(TypeID::prefitLC61C, SatelliteSystem::BeiDou);

			// add xyz
			equPC12G.addVarSet(xyzVarSet);
			equLC12G.addVarSet(xyzVarSet);
			equPC15G.addVarSet(xyzVarSet);
			equLC15G.addVarSet(xyzVarSet);
			// add clk
			equPC12G.addVariable(dcdtGPS, 1.0);
			equLC12G.addVariable(dcdtGPS, 1.0);
			equPC15G.addVariable(dcdtGPS, 1.0);
			equLC15G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC12G.addVariable(tropo);
			equLC12G.addVariable(tropo);
			equPC15G.addVariable(tropo);
			equLC15G.addVariable(tropo);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFCB)
			{
				equPC15G.addVariable(ifcbL5G, 1.0);
				equLC15G.addVariable(ifcbL5G, -1.0);
			}
			else if (opts.PPPModel == Options::IFB)
			{
				equPC15G.addVariable(ifbC5G, 1.0);
			}
			//add ambiguity
			equLC12G.addVariable(BLC12G, 1.0);
			equLC15G.addVariable(BLC15G, 1.0);
			// set weight
			// The weight under the current 
			// combination does not need to be multiplied by fact.
			equPC12G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equPC15G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equLC12G.setWeight(SQR(1 / errmeasLG));
			equLC15G.setWeight(SQR(1 / errmeasLG));

			// gal
			equPC15E.addVarSet(xyzVarSet);
			equLC15E.addVarSet(xyzVarSet);
			equPC17E.addVarSet(xyzVarSet);
			equLC17E.addVarSet(xyzVarSet);
			equPC18E.addVarSet(xyzVarSet);
			equLC18E.addVarSet(xyzVarSet);
			// add clk
			equPC15E.addVariable(dcdtGAL, 1.0);
			equLC15E.addVariable(dcdtGAL, 1.0);
			equPC17E.addVariable(dcdtGAL, 1.0);
			equLC17E.addVariable(dcdtGAL, 1.0);
			equPC18E.addVariable(dcdtGAL, 1.0);
			equLC18E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equPC15E.addVariable(tropo);
			equLC15E.addVariable(tropo);
			equPC17E.addVariable(tropo);
			equLC17E.addVariable(tropo);
			equPC18E.addVariable(tropo);
			equLC18E.addVariable(tropo);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
			{
				equPC17E.addVariable(ifbC7E, 1.0);
				equPC18E.addVariable(ifbC8E, 1.0);
			}
			else if (opts.PPPModel == Options::IFCB)
			{
				equPC17E.addVariable(ifcbL7E, 1.0);
				equLC17E.addVariable(ifcbL7E, -1.0);
				equPC18E.addVariable(ifcbL8E, 1.0);
				equLC18E.addVariable(ifcbL8E, -1.0);
			}
			
			//add ambiguity
			equLC15E.addVariable(BLC15E, 1.0);
			equLC17E.addVariable(BLC17E, 1.0);
			equLC18E.addVariable(BLC18E, 1.0);

			// set weight
			equPC15E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equPC17E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equPC18E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equLC15E.setWeight(SQR(1 / errmeasLE));
			equLC17E.setWeight(SQR(1 / errmeasLE));
			equLC18E.setWeight(SQR(1 / errmeasLE));

			// bds
			if (bdsfopt)
			{
				// normal pair-wise combination
				// xyz
				equPC15C.addVarSet(xyzVarSet);
				equLC15C.addVarSet(xyzVarSet);
				equPC52C.addVarSet(xyzVarSet);
				equLC52C.addVarSet(xyzVarSet);
				equPC16C.addVarSet(xyzVarSet);
				equLC16C.addVarSet(xyzVarSet);
				// clk
				equPC15C.addVariable(dcdtBDS, 1.0);
				equLC15C.addVariable(dcdtBDS, 1.0);
				equPC52C.addVariable(dcdtBDS, 1.0);
				equLC52C.addVariable(dcdtBDS, 1.0);
				equPC16C.addVariable(dcdtBDS, 1.0);
				equLC16C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC15C.addVariable(tropo);
				equLC15C.addVariable(tropo);
				equPC16C.addVariable(tropo);
				equLC16C.addVariable(tropo);
				
				equPC52C.addVariable(tropo);
				equLC52C.addVariable(tropo);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equPC52C.addVariable(ifbC2C, 1.0);
					equPC16C.addVariable(ifbC6C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equPC52C.addVariable(ifcbL2C, 1.0);
					equLC52C.addVariable(ifcbL2C, -1.0);
					equPC16C.addVariable(ifcbL6C, 1.0);
					equLC16C.addVariable(ifcbL6C, -1.0);
				}
				
				// ambiguity
				equLC15C.addVariable(BLC15C, 1.0);
				equLC52C.addVariable(BLC52C, 1.0);
				equLC16C.addVariable(BLC16C, 1.0);
				// set weight
				equPC15C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC52C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC16C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC15C.setWeight(SQR(1 / errmeasLC));
				equLC52C.setWeight(SQR(1 / errmeasLC));
				equLC16C.setWeight(SQR(1 / errmeasLC));
				
			}
			else
			{
				// add xyz
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);

				equPC61C.addVarSet(xyzVarSet);
				equLC61C.addVarSet(xyzVarSet);

				equPC25C.addVarSet(xyzVarSet);
				equLC25C.addVarSet(xyzVarSet);
				// add clk
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);

				equPC61C.addVariable(dcdtBDS, 1.0);
				equLC61C.addVariable(dcdtBDS, 1.0);

				equPC25C.addVariable(dcdtBDS, 1.0);
				equLC25C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);

				equPC61C.addVariable(tropo);
				equLC61C.addVariable(tropo);

				equPC25C.addVariable(tropo);
				equLC25C.addVariable(tropo);
				// add ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equPC61C.addVariable(ifbC1C, 1.0);
					equPC25C.addVariable(ifbC5C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equPC61C.addVariable(ifcbL1C, 1.0);
					equLC61C.addVariable(ifcbL1C, -1.0);
					equPC25C.addVariable(ifcbL5C, 1.0);
					equLC25C.addVariable(ifcbL5C, -1.0);
				}
				
				// add ambiguity
				equLC26C.addVariable(BLC26C, 1.0);
				equLC61C.addVariable(BLC61C, 1.0);
				equLC25C.addVariable(BLC25C, 1.0);
				// set weight
				equPC26C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC61C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC25C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC26C.setWeight(SQR(1 / errmeasLC));
				equLC61C.setWeight(SQR(1 / errmeasLC));
				equLC25C.setWeight(SQR(1 / errmeasLC));
				if (bds2)
				{
					equPC27C.addVarSet(xyzVarSet);
					equLC27C.addVarSet(xyzVarSet);
					// clk
					equPC27C.addVariable(dcdtBDS, 1.0);
					equLC27C.addVariable(dcdtBDS, 1.0);
					// tropo
					equPC27C.addVariable(tropo);
					equLC27C.addVariable(tropo);
					// ifb
					if (opts.PPPModel == Options::IFCB_IFB ||
						opts.PPPModel == Options::IFB)
						equPC27C.addVariable(ifbC7C, 1.0);
					else if (opts.PPPModel == Options::IFCB)
					{
						equPC27C.addVariable(ifcbL7C, 1.0);
						equLC27C.addVariable(ifcbL7C, -1.0);
					}
					
					// ambiguity
					equLC27C.addVariable(BLC27C, 1.0);
					// weight
					equPC27C.setWeight(SQR(1 / (errmeasLC * errratioC)));
					equLC27C.setWeight(SQR(1 / errmeasLC));
				}
			}	// end of if bdsfopt

			// equsystem add equs
			equSystem.clearEquations();
			if (codeOnly)
			{	// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equPC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equPC17E);
				equSystem.addEquation(equPC18E);
				//bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equPC52C);
						equSystem.addEquation(equPC16C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equPC61C);
						equSystem.addEquation(equPC25C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
						}
					}
				}	// End of if !bdsComb				
			}	// end of if codeOnly
			else
			{
				// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equLC12G);
				equSystem.addEquation(equPC15G);
				equSystem.addEquation(equLC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equLC15E);
				equSystem.addEquation(equPC17E);
				equSystem.addEquation(equLC17E);
				equSystem.addEquation(equPC18E);
				equSystem.addEquation(equLC18E);
				//bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equLC15C);
						equSystem.addEquation(equPC52C);
						equSystem.addEquation(equLC52C);
						equSystem.addEquation(equPC16C);
						equSystem.addEquation(equLC16C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equLC26C);
						equSystem.addEquation(equPC61C);
						equSystem.addEquation(equLC61C);
						equSystem.addEquation(equPC25C);
						equSystem.addEquation(equLC25C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
							equSystem.addEquation(equLC27C);
						}
					}
				}	// End of if !bdsComb
				
			}	// end of if !codeOnly
		}	// End of IF121314
		else if (ionoopt == Options::IF1234)
		{
			// gps
			Equation equPC125G(TypeID::prefitPC125G, SatelliteSystem::GPS);
			Equation equLC125G(TypeID::prefitLC125G, SatelliteSystem::GPS);
			// gal
			Equation equPC1578E(TypeID::prefitPC1578E, SatelliteSystem::Galileo);
			Equation equLC1578E(TypeID::prefitLC1578E, SatelliteSystem::Galileo);
			// bds2
			Equation equPC267C(TypeID::prefitPC267C, SatelliteSystem::BeiDou);
			Equation equLC267C(TypeID::prefitLC267C, SatelliteSystem::BeiDou);
			// bds3
			Equation equPC1526C(TypeID::prefitPC1526C, SatelliteSystem::BeiDou);
			Equation equLC1526C(TypeID::prefitLC1526C, SatelliteSystem::BeiDou);
			Equation equPC2615C(TypeID::prefitPC2615C, SatelliteSystem::BeiDou);
			Equation equLC2615C(TypeID::prefitLC2615C, SatelliteSystem::BeiDou);

			// add xyz
			equPC125G.addVarSet(xyzVarSet);
			equLC125G.addVarSet(xyzVarSet);
			// add clk
			equPC125G.addVariable(dcdtGPS, 1.0);
			equLC125G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC125G.addVariable(tropo);
			equLC125G.addVariable(tropo);
			// add ifb
			if (opts.PPPModel==Options::IFB)
				equPC125G.addVariable(ifbC5G, 1.0);
			// add ambiguity
			equLC125G.addVariable(BLC125G, 1.0);
			//set weight
			equPC125G.setWeight(SQR(1 / (errmeasLG * errratioG)));
			equLC125G.setWeight(SQR(1 / errmeasLG));

			// gal
			equPC1578E.addVarSet(xyzVarSet);
			equLC1578E.addVarSet(xyzVarSet);
			// add clk
			equPC1578E.addVariable(dcdtGAL, 1.0);
			equLC1578E.addVariable(dcdtGAL, 1.0);
			// add trop
			equPC1578E.addVariable(tropo);
			equLC1578E.addVariable(tropo);
			// add ifb absorted into clk
			if (opts.PPPModel == Options::IFB)
				equPC1578E.addVariable(ifbC8E, 1.0);
			// add ambiguity
			equLC1578E.addVariable(BLC1578E, 1.0);
			//set weight
			equPC1578E.setWeight(SQR(1 / (errmeasLE * errratioE)));
			equLC1578E.setWeight(SQR(1 / errmeasLE));

			// bds
			if (bds2)
			{
				// add xyz
				equPC267C.addVarSet(xyzVarSet);
				equLC267C.addVarSet(xyzVarSet);
				//add clk
				equPC267C.addVariable(dcdtBDS, 1.0);
				equLC267C.addVariable(dcdtBDS, 1.0);
				//add tropo
				equPC267C.addVariable(tropo);
				equLC267C.addVariable(tropo);
				if (opts.PPPModel == Options::IFB)
					equPC267C.addVariable(ifbC7C, 1.0);
				// add ambiguity
				equLC267C.addVariable(BLC267C, 1.0);
				//set weight
				equPC267C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC267C.setWeight(SQR(1 / errmeasLC));
			}
			if (bdsfopt)
			{	//
				equPC1526C.addVarSet(xyzVarSet);
				equLC1526C.addVarSet(xyzVarSet);
				// clk
				equPC1526C.addVariable(dcdtBDS, 1.0);
				equLC1526C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC1526C.addVariable(tropo);
				equLC1526C.addVariable(tropo);
				// ifb
				if (opts.PPPModel == Options::IFB)
					equPC1526C.addVariable(ifbC6C, 1.0);
				//ambiguity
				equLC1526C.addVariable(BLC1526C, 1.0);
				//set weight
				equPC1526C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC1526C.setWeight(SQR(1 / errmeasLC));
			}
			else
			{
				equPC2615C.addVarSet(xyzVarSet);
				equLC2615C.addVarSet(xyzVarSet);
				// clk
				equPC2615C.addVariable(dcdtBDS, 1.0);
				equLC2615C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC2615C.addVariable(tropo);
				equLC2615C.addVariable(tropo);
				// ifb
				if (opts.PPPModel == Options::IFB)
					equPC2615C.addVariable(ifbC5C, 1.0);
				//ambiguity
				equLC2615C.addVariable(BLC2615C, 1.0);
				//set weight
				equPC2615C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC2615C.setWeight(SQR(1 / errmeasLC));
			}


			// add equ into equations
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equPC1578E);
				// bds
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC1526C);
					}
					else
					{
						equSystem.addEquation(equPC2615C);
					}
				}	// End of if !bdsComb
			}	// End of if codeOnly
			else
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equLC125G);
				equSystem.addEquation(equPC1578E);
				equSystem.addEquation(equLC1578E);
				// bds
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
						equSystem.addEquation(equLC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC1526C);
						equSystem.addEquation(equLC1526C);
					}
					else
					{
						equSystem.addEquation(equPC2615C);
						equSystem.addEquation(equLC2615C);
					}
				}	// End of if !bdsComb
			}	// end of if !codeOnly
		}	// end of ionoopt==IF1234
		else if (ionoopt == Options::UC12345)
		{
			Equation equC1G(TypeID::prefitC1G, SatelliteSystem::GPS);
			Equation equL1G(TypeID::prefitL1G, SatelliteSystem::GPS);
			Equation equC2G(TypeID::prefitC2G, SatelliteSystem::GPS);
			Equation equL2G(TypeID::prefitL2G, SatelliteSystem::GPS);
			Equation equC5G(TypeID::prefitC5G, SatelliteSystem::GPS);
			Equation equL5G(TypeID::prefitL5G, SatelliteSystem::GPS);
			// add variable
			equC1G.addVarSet(xyzVarSet);
			equL1G.addVarSet(xyzVarSet);
			equC2G.addVarSet(xyzVarSet);
			equL2G.addVarSet(xyzVarSet);
			equC5G.addVarSet(xyzVarSet);
			equL5G.addVarSet(xyzVarSet);
			// add dcdt
			equC1G.addVariable(dcdtGPS, 1.0);
			equL1G.addVariable(dcdtGPS, 1.0);
			equC2G.addVariable(dcdtGPS, 1.0);
			equL2G.addVariable(dcdtGPS, 1.0);
			equC5G.addVariable(dcdtGPS, 1.0);
			equL5G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equC1G.addVariable(tropo);
			equL1G.addVariable(tropo);
			equC2G.addVariable(tropo);
			equL2G.addVariable(tropo);
			equC5G.addVariable(tropo);
			equL5G.addVariable(tropo);
			// iono
			equC1G.addVariable(ionoC1G, 1.0);
			equL1G.addVariable(ionoC1G, -1.0);
			equC2G.addVariable(ionoC1G, GAMMA_GPS_L1L2);
			equL2G.addVariable(ionoC1G, -GAMMA_GPS_L1L2);
			equC5G.addVariable(ionoC1G, GAMMA_GPS_L1L5);
			equL5G.addVariable(ionoC1G, -GAMMA_GPS_L1L5);
			// ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
			{
				equC5G.addVariable(ifbC5G, 1.0);
			}
			else if (opts.PPPModel == Options::IFCB)
			{
				equC5G.addVariable(ifcbL5G, 1.0);
				equL5G.addVariable(ifcbL5G, -1.0);
			}
			// ambiguity
			equL1G.addVariable(BL1G, 1.0);
			equL2G.addVariable(BL2G, 1.0);
			equL5G.addVariable(BL5G, 1.0);
			// set weight
			equC1G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equC2G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equC5G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equL1G.setWeight(SQR(1/errmeasLG));
			equL2G.setWeight(SQR(1/errmeasLG));
			equL5G.setWeight(SQR(1/errmeasLG));

			//gal
			Equation equC1E(TypeID::prefitC1E, SatelliteSystem::Galileo);
			Equation equL1E(TypeID::prefitL1E, SatelliteSystem::Galileo);
			Equation equC5E(TypeID::prefitC5E, SatelliteSystem::Galileo);
			Equation equL5E(TypeID::prefitL5E, SatelliteSystem::Galileo);
			Equation equC7E(TypeID::prefitC7E, SatelliteSystem::Galileo);
			Equation equL7E(TypeID::prefitL7E, SatelliteSystem::Galileo);
			Equation equC8E(TypeID::prefitC8E, SatelliteSystem::Galileo);
			Equation equL8E(TypeID::prefitL8E, SatelliteSystem::Galileo);
			Equation equC6E(TypeID::prefitC6E, SatelliteSystem::Galileo);
			Equation equL6E(TypeID::prefitL6E, SatelliteSystem::Galileo);

			//insert xyz
			equC1E.addVarSet(xyzVarSet);
			equL1E.addVarSet(xyzVarSet);
			equC5E.addVarSet(xyzVarSet);
			equL5E.addVarSet(xyzVarSet);
			equC7E.addVarSet(xyzVarSet);
			equL7E.addVarSet(xyzVarSet);
			equC8E.addVarSet(xyzVarSet);
			equL8E.addVarSet(xyzVarSet);
			equC6E.addVarSet(xyzVarSet);
			equL6E.addVarSet(xyzVarSet);
			// add dcdt
			equC1E.addVariable(dcdtGAL, 1.0);
			equL1E.addVariable(dcdtGAL, 1.0);
			equC5E.addVariable(dcdtGAL, 1.0);
			equL5E.addVariable(dcdtGAL, 1.0);
			equC7E.addVariable(dcdtGAL, 1.0);
			equL7E.addVariable(dcdtGAL, 1.0);
			equC8E.addVariable(dcdtGAL, 1.0);
			equL8E.addVariable(dcdtGAL, 1.0);
			equC6E.addVariable(dcdtGAL, 1.0);
			equL6E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equC1E.addVariable(tropo);
			equL1E.addVariable(tropo);
			equC5E.addVariable(tropo);
			equL5E.addVariable(tropo);
			equC7E.addVariable(tropo);
			equL7E.addVariable(tropo);
			equC8E.addVariable(tropo);
			equL8E.addVariable(tropo);
			equC6E.addVariable(tropo);
			equL6E.addVariable(tropo);
			// add iono
			equC1E.addVariable(ionoC1E, 1.0);
			equL1E.addVariable(ionoC1E, -1.0);
			equC5E.addVariable(ionoC1E, GAMMA_GAL_L1L5);
			equL5E.addVariable(ionoC1E, -GAMMA_GAL_L1L5);
			equC7E.addVariable(ionoC1E, GAMMA_GAL_L1L7);
			equL7E.addVariable(ionoC1E, -GAMMA_GAL_L1L7);
			equC8E.addVariable(ionoC1E, GAMMA_GAL_L1L8);
			equL8E.addVariable(ionoC1E, -GAMMA_GAL_L1L8);
			equC6E.addVariable(ionoC1E, GAMMA_GAL_L1L6);
			equL6E.addVariable(ionoC1E, -GAMMA_GAL_L1L6);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFB)
			{
				equC7E.addVariable(ifbC7E, 1.0);
				equC8E.addVariable(ifbC8E, 1.0);
				equC6E.addVariable(ifbC6E, 1.0);
			}
			else if (opts.PPPModel == Options::IFCB)
			{
				equC7E.addVariable(ifcbL7E, 1.0);
				equL7E.addVariable(ifcbL7E, -1.0);
				equC8E.addVariable(ifcbL8E, 1.0);
				equL8E.addVariable(ifcbL8E, -1.0);
				equC6E.addVariable(ifcbL6E, 1.0);
				equL6E.addVariable(ifcbL6E, -1.0);
			}
			
			// add ambiguity
			equL1E.addVariable(BL1E, 1.0);
			equL5E.addVariable(BL5E, 1.0);
			equL7E.addVariable(BL7E, 1.0);
			equL8E.addVariable(BL8E, 1.0);
			equL6E.addVariable(BL6E, 1.0);
			
			// set weight
			equC1E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equC5E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equC7E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equC8E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equC6E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equL1E.setWeight(SQR(1/errmeasLE));
			equL5E.setWeight(SQR(1/errmeasLE));
			equL7E.setWeight(SQR(1/errmeasLE));
			equL8E.setWeight(SQR(1/errmeasLE));
			equL6E.setWeight(SQR(1/errmeasLE));


			// BDS
			Equation equC2C(TypeID::prefitC2C, SatelliteSystem::BeiDou);
			Equation equL2C(TypeID::prefitL2C, SatelliteSystem::BeiDou);
			Equation equC6C(TypeID::prefitC6C, SatelliteSystem::BeiDou);
			Equation equL6C(TypeID::prefitL6C, SatelliteSystem::BeiDou);
			Equation equC1C(TypeID::prefitC1C, SatelliteSystem::BeiDou);
			Equation equL1C(TypeID::prefitL1C, SatelliteSystem::BeiDou);
			Equation equC5C(TypeID::prefitC5C, SatelliteSystem::BeiDou);
			Equation equL5C(TypeID::prefitL5C, SatelliteSystem::BeiDou);
			Equation equC8C(TypeID::prefitC8C, SatelliteSystem::BeiDou);
			Equation equL8C(TypeID::prefitL8C, SatelliteSystem::BeiDou);

			// bds2
			Equation equC7C(TypeID::prefitC7C, SatelliteSystem::BeiDou);
			Equation equL7C(TypeID::prefitL7C, SatelliteSystem::BeiDou);

			// add xyz
			equC2C.addVarSet(xyzVarSet);
			equL2C.addVarSet(xyzVarSet);
			equC6C.addVarSet(xyzVarSet);
			equL6C.addVarSet(xyzVarSet);
			equC1C.addVarSet(xyzVarSet);
			equL1C.addVarSet(xyzVarSet);
			equC5C.addVarSet(xyzVarSet);
			equL5C.addVarSet(xyzVarSet);
			equC8C.addVarSet(xyzVarSet);
			equL8C.addVarSet(xyzVarSet);
			// add clk
			equC2C.addVariable(dcdtBDS, 1.0);
			equL2C.addVariable(dcdtBDS, 1.0);
			equC6C.addVariable(dcdtBDS, 1.0);
			equL6C.addVariable(dcdtBDS, 1.0);
			equC1C.addVariable(dcdtBDS, 1.0);
			equL1C.addVariable(dcdtBDS, 1.0);
			equC5C.addVariable(dcdtBDS, 1.0);
			equL5C.addVariable(dcdtBDS, 1.0);
			equC8C.addVariable(dcdtBDS, 1.0);
			equL8C.addVariable(dcdtBDS, 1.0);
			// add tropo
			equC2C.addVariable(tropo);
			equL2C.addVariable(tropo);
			equC6C.addVariable(tropo);
			equL6C.addVariable(tropo);
			equC1C.addVariable(tropo);
			equL1C.addVariable(tropo);
			equC5C.addVariable(tropo);
			equL5C.addVariable(tropo);
			equC8C.addVariable(tropo);
			equL8C.addVariable(tropo);
			// add iono
			if (bdsfopt)
			{
				equC1C.addVariable(ionoC1C, 1.0);
				equL1C.addVariable(ionoC1C, -1.0);
				equC5C.addVariable(ionoC1C, GAMMA_BDS_L1L5);
				equL5C.addVariable(ionoC1C, -GAMMA_BDS_L1L5);
				equC2C.addVariable(ionoC1C, GAMMA_BDS_L1L2);
				equL2C.addVariable(ionoC1C, -GAMMA_BDS_L1L2);
				equC6C.addVariable(ionoC1C, GAMMA_BDS_L1L6);
				equL6C.addVariable(ionoC1C, -GAMMA_BDS_L1L6);
				equC8C.addVariable(ionoC1C, GAMMA_BDS_L1L8);
				equL8C.addVariable(ionoC1C, -GAMMA_BDS_L1L8);
				// add ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equC2C.addVariable(ifbC2C, 1.0);
					equC6C.addVariable(ifbC6C, 1.0);
					equC8C.addVariable(ifbC8C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equC2C.addVariable(ifcbL2C, 1.0);
					equL2C.addVariable(ifcbL2C, -1.0);
					equC6C.addVariable(ifcbL6C, 1.0);
					equL6C.addVariable(ifcbL6C, -1.0);
					equC8C.addVariable(ifcbL8C, 1.0);
					equL8C.addVariable(ifcbL8C, -1.0);
				}
				
			}
			else
			{
				equC2C.addVariable(ionoC2C, 1.0);
				equL2C.addVariable(ionoC2C, -1.0);
				equC6C.addVariable(ionoC2C, GAMMA_BDS_L2L6);
				equL6C.addVariable(ionoC2C, -GAMMA_BDS_L2L6);
				equC1C.addVariable(ionoC2C, GAMMA_BDS_L2L1);
				equL1C.addVariable(ionoC2C, -GAMMA_BDS_L2L1);
				equC5C.addVariable(ionoC2C, GAMMA_BDS_L2L5);
				equL5C.addVariable(ionoC2C, -GAMMA_BDS_L2L5);
				equC8C.addVariable(ionoC2C, GAMMA_BDS_L2L8);
				equL8C.addVariable(ionoC2C, -GAMMA_BDS_L2L8);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equC1C.addVariable(ifbC1C, 1.0);
					equC5C.addVariable(ifbC5C, 1.0);
					equC8C.addVariable(ifbC8C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equC1C.addVariable(ifcbL1C, 1.0);
					equL1C.addVariable(ifcbL1C, -1.0);
					equC5C.addVariable(ifcbL5C, 1.0);
					equL5C.addVariable(ifcbL5C, -1.0);
					equC8C.addVariable(ifcbL8C, 1.0);
					equL8C.addVariable(ifcbL8C, -1.0);
				}
				
			}
			
			// add ambiguity
			equL1C.addVariable(BL1C, 1.0);
			equL5C.addVariable(BL5C, 1.0);
			equL2C.addVariable(BL2C, 1.0);
			equL6C.addVariable(BL6C, 1.0);
			equL8C.addVariable(BL8C, 1.0);

			// set weight
			equC1C.setWeight( SQR(1/(errmeasLC*errratioC)) );
			equC5C.setWeight( SQR(1/(errmeasLC*errratioC)) );
			equC2C.setWeight( SQR(1/(errmeasLC*errratioC)) );
			equC6C.setWeight( SQR(1/(errmeasLC*errratioC)) );
			equC8C.setWeight( SQR(1/(errmeasLC*errratioC)) );
			equL1C.setWeight( SQR(1/errmeasLC) );
			equL5C.setWeight( SQR(1/errmeasLC) );
			equL2C.setWeight( SQR(1/errmeasLC) );
			equL6C.setWeight( SQR(1/errmeasLC) );
			equL8C.setWeight( SQR(1/errmeasLC) );

			if (bds2)
			{
				// add xyz
				equC7C.addVarSet(xyzVarSet);
				equL7C.addVarSet(xyzVarSet);
				// add clk
				equC7C.addVariable(dcdtBDS, 1.0);
				equL7C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equC7C.addVariable(tropo);
				equL7C.addVariable(tropo);
				// iono
				equC7C.addVariable(ionoC2C, GAMMA_BDS_L2L7);
				equL7C.addVariable(ionoC2C, -GAMMA_BDS_L2L7);
				// ifb
				if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equC7C.addVariable(ifbC7C, 1.0);
				}
				else if (opts.PPPModel == Options::IFCB)
				{
					equC7C.addVariable(ifcbL7C, 1.0);
					equL7C.addVariable(ifcbL7C, -1.0);
				}
				
				//ambiguity
				equL7C.addVariable(BL7C, 1.0);
				// set weight
				equC7C.setWeight( SQR(1/(errmeasLC*errratioC)) );
				equL7C.setWeight( SQR(1/errmeasLC) );

			}	// end of if bds2


			// add equs into equsystem
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equC7E);
				equSystem.addEquation(equC8E);
				equSystem.addEquation(equC6E);
				// bds
				if (!bdsComb)
				{
					equSystem.addEquation(equC1C);
					equSystem.addEquation(equC5C);
					equSystem.addEquation(equC2C);
					equSystem.addEquation(equC6C);
					equSystem.addEquation(equC8C);
					if (bds2)
					{
						equSystem.addEquation(equC7C);
					}
				}	// End of if(!bdsComb)
				
			}	// end of if codeOnly
			else
			{
				equSystem.addEquation(equC1G);
				equSystem.addEquation(equL1G);
				equSystem.addEquation(equC2G);
				equSystem.addEquation(equL2G);
				equSystem.addEquation(equC5G);
				equSystem.addEquation(equL5G);
				equSystem.addEquation(equC1E);
				equSystem.addEquation(equL1E);
				equSystem.addEquation(equC5E);
				equSystem.addEquation(equL5E);
				equSystem.addEquation(equC7E);
				equSystem.addEquation(equL7E);
				equSystem.addEquation(equC8E);
				equSystem.addEquation(equL8E);
				equSystem.addEquation(equC6E);
				equSystem.addEquation(equL6E);
				// bds
				if (!bdsComb)
				{
					equSystem.addEquation(equC1C);
					equSystem.addEquation(equL1C);
					equSystem.addEquation(equC5C);
					equSystem.addEquation(equL5C);
					equSystem.addEquation(equC2C);
					equSystem.addEquation(equL2C);
					equSystem.addEquation(equC6C);
					equSystem.addEquation(equL6C);
					equSystem.addEquation(equC8C);
					equSystem.addEquation(equL8C);
					if (bds2)
					{
						equSystem.addEquation(equC7C);
						equSystem.addEquation(equL7C);
					}
				}	// End of if !bdsComb
				
			}	// end of if !codeOnly
		}	// end of if uc12345
		else if (ionoopt == Options::IF12345)
		{
			// gps
			Equation equPC125G(TypeID::prefitPC125G, SatelliteSystem::GPS);
			Equation equLC125G(TypeID::prefitLC125G, SatelliteSystem::GPS);
			// gal
			Equation equPC15786E(TypeID::prefitPC15786E, SatelliteSystem::Galileo);
			Equation equLC15786E(TypeID::prefitLC15786E, SatelliteSystem::Galileo);
			// bds2
			Equation equPC267C(TypeID::prefitPC267C, SatelliteSystem::BeiDou);
			Equation equLC267C(TypeID::prefitLC267C, SatelliteSystem::BeiDou);
			// bds3
			Equation equPC15268C(TypeID::prefitPC15268C, SatelliteSystem::BeiDou);
			Equation equLC15268C(TypeID::prefitLC15268C, SatelliteSystem::BeiDou);
			Equation equPC26158C(TypeID::prefitPC26158C, SatelliteSystem::BeiDou);
			Equation equLC26158C(TypeID::prefitLC26158C, SatelliteSystem::BeiDou);

			// add xyz
			equPC125G.addVarSet(xyzVarSet);
			equLC125G.addVarSet(xyzVarSet);
			// add clk
			equPC125G.addVariable(dcdtGPS, 1.0);
			equLC125G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC125G.addVariable(tropo);
			equLC125G.addVariable(tropo);
			// add ifb
			if (opts.PPPModel==Options::IFB)
				equPC125G.addVariable(ifbC5G, 1.0);
			// add ambiguity
			equLC125G.addVariable(BLC125G, 1.0);
			//set weight
			equPC125G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equLC125G.setWeight(SQR(1/errmeasLG));

			// gal
			equPC15786E.addVarSet(xyzVarSet);
			equLC15786E.addVarSet(xyzVarSet);
			// add clk
			equPC15786E.addVariable(dcdtGAL, 1.0);
			equLC15786E.addVariable(dcdtGAL, 1.0);
			// add trop
			equPC15786E.addVariable(tropo);
			equLC15786E.addVariable(tropo);
			// add ifb
			if (opts.PPPModel==Options::IFB)
				equPC15786E.addVariable(ifbC6E, 1.0);
			
			// add ambiguity
			equLC15786E.addVariable(BLC15786E, 1.0);
			//set weight
			equPC15786E.setWeight(SQR(1/(errmeasLE*errratioE)));
			equLC15786E.setWeight(SQR(1/errmeasLE));

			// bds
			if (bds2)
			{
				// add xyz
				equPC267C.addVarSet(xyzVarSet);
				equLC267C.addVarSet(xyzVarSet);
				//add clk
				equPC267C.addVariable(dcdtBDS, 1.0);
				equLC267C.addVariable(dcdtBDS, 1.0);
				//add tropo
				equPC267C.addVariable(tropo);
				equLC267C.addVariable(tropo);
				// ifb
				if (opts.PPPModel==Options::IFB)
					equPC267C.addVariable(ifbC7C, 1.0);
				// add ambiguity
				equLC267C.addVariable(BLC267C, 1.0);
				// set weight
				equPC267C.setWeight(SQR(1/(errmeasLC*errratioC)));
				equLC267C.setWeight(SQR(1/errmeasLC));
			}
			if (bdsfopt)
			{	//
				equPC15268C.addVarSet(xyzVarSet);
				equLC15268C.addVarSet(xyzVarSet);
				// clk
				equPC15268C.addVariable(dcdtBDS, 1.0);
				equLC15268C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC15268C.addVariable(tropo);
				equLC15268C.addVariable(tropo);
				// ifb
				if (opts.PPPModel==Options::IFB)
					equPC15268C.addVariable(ifbC8C, 1.0);
				//ambiguity
				equLC15268C.addVariable(BLC15268C, 1.0);
				//set weight
				equPC15268C.setWeight(SQR(1/(errmeasLC*errratioC)));
				equLC15268C.setWeight(SQR(1/errmeasLC));
			}
			else
			{
				equPC26158C.addVarSet(xyzVarSet);
				equLC26158C.addVarSet(xyzVarSet);
				// clk
				equPC26158C.addVariable(dcdtBDS, 1.0);
				equLC26158C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC26158C.addVariable(tropo);
				equLC26158C.addVariable(tropo);
				// ifb
				if (opts.PPPModel==Options::IFB)
					equPC26158C.addVariable(ifbC8C, 1.0);
				
				//ambiguity
				equLC26158C.addVariable(BLC26158C, 1.0);
				//set weight
				equPC26158C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC26158C.setWeight(SQR(1 / errmeasLC));
			}

			
			// add equ into equations
			equSystem.clearEquations();
			if (codeOnly)
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equPC15786E);
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15268C);
					}
					else
					{
						equSystem.addEquation(equPC26158C);
					}
				}	// End of if(!bdsComb)
			}	// End of if codeOnly
			else
			{
				equSystem.addEquation(equPC125G);
				equSystem.addEquation(equLC125G);
				equSystem.addEquation(equPC15786E);
				equSystem.addEquation(equLC15786E);
				// bds
				if (!bdsComb)
				{
					if (bds2)
					{
						equSystem.addEquation(equPC267C);
						equSystem.addEquation(equLC267C);
					}
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15268C);
						equSystem.addEquation(equLC15268C);
					}
					else
					{
						equSystem.addEquation(equPC26158C);
						equSystem.addEquation(equLC26158C);
					}
				}	// end of if !bdsComb
				
			}	// end of if !codeOnly

		}	// end of if if12345
		else if (ionoopt==Options::IF12131415)
		{
			// gps
			Equation equPC12G(TypeID::prefitPC12G, SatelliteSystem::GPS);
			Equation equPC15G(TypeID::prefitPC15G, SatelliteSystem::GPS);
			Equation equLC12G(TypeID::prefitLC12G, SatelliteSystem::GPS);
			Equation equLC15G(TypeID::prefitLC15G, SatelliteSystem::GPS);

			//gal
			Equation equPC15E(TypeID::prefitPC15E, SatelliteSystem::Galileo);
			Equation equLC15E(TypeID::prefitLC15E, SatelliteSystem::Galileo);
			Equation equPC17E(TypeID::prefitPC17E, SatelliteSystem::Galileo);
			Equation equLC17E(TypeID::prefitLC17E, SatelliteSystem::Galileo);
			Equation equPC18E(TypeID::prefitPC18E, SatelliteSystem::Galileo);
			Equation equLC18E(TypeID::prefitLC18E, SatelliteSystem::Galileo);
			Equation equPC16E(TypeID::prefitPC16E, SatelliteSystem::Galileo);
			Equation equLC16E(TypeID::prefitLC16E, SatelliteSystem::Galileo);

			// bds2
			Equation equPC27C(TypeID::prefitPC27C, SatelliteSystem::BeiDou);
			Equation equLC27C(TypeID::prefitLC27C, SatelliteSystem::BeiDou);
			// bds3
			Equation equPC15C(TypeID::prefitPC15C, SatelliteSystem::BeiDou);
			Equation equLC15C(TypeID::prefitLC15C, SatelliteSystem::BeiDou);
			Equation equPC16C(TypeID::prefitPC16C, SatelliteSystem::BeiDou);
			Equation equLC16C(TypeID::prefitLC16C, SatelliteSystem::BeiDou);
			Equation equPC18C(TypeID::prefitPC18C, SatelliteSystem::BeiDou);
			Equation equLC18C(TypeID::prefitLC18C, SatelliteSystem::BeiDou);
			// change
			Equation equPC52C(TypeID::prefitPC52C, SatelliteSystem::BeiDou);
			Equation equLC52C(TypeID::prefitLC52C, SatelliteSystem::BeiDou);

			Equation equPC61C(TypeID::prefitPC61C, SatelliteSystem::BeiDou);
			Equation equLC61C(TypeID::prefitLC61C, SatelliteSystem::BeiDou);

			// bds3 normal
			Equation equPC26C(TypeID::prefitPC26C, SatelliteSystem::BeiDou);
			Equation equLC26C(TypeID::prefitLC26C, SatelliteSystem::BeiDou);
			Equation equPC25C(TypeID::prefitPC25C, SatelliteSystem::BeiDou);
			Equation equLC25C(TypeID::prefitLC25C, SatelliteSystem::BeiDou);
			Equation equPC28C(TypeID::prefitPC28C, SatelliteSystem::BeiDou);
			Equation equLC28C(TypeID::prefitLC28C, SatelliteSystem::BeiDou);
			/// GPS
			// add xyz
			equPC12G.addVarSet(xyzVarSet);
			equLC12G.addVarSet(xyzVarSet);
			equPC15G.addVarSet(xyzVarSet);
			equLC15G.addVarSet(xyzVarSet);
			// add clk
			equPC12G.addVariable(dcdtGPS, 1.0);
			equLC12G.addVariable(dcdtGPS, 1.0);
			equPC15G.addVariable(dcdtGPS, 1.0);
			equLC15G.addVariable(dcdtGPS, 1.0);
			// add tropo
			equPC12G.addVariable(tropo);
			equLC12G.addVariable(tropo);
			equPC15G.addVariable(tropo);
			equLC15G.addVariable(tropo);
			// add ifb
			if (opts.PPPModel == Options::IFCB_IFB ||
				opts.PPPModel == Options::IFCB)
			{
				equPC15G.addVariable(ifcbL5G, 1.0);
				equLC15G.addVariable(ifcbL5G, -1.0);
			}
			else if (opts.PPPModel == Options::IFB)
			{
				equPC15G.addVariable(ifbC5G, 1.0);
			}
			//add ambiguity
			equLC12G.addVariable(BLC12G, 1.0);
			equLC15G.addVariable(BLC15G, 1.0);
			// set weight
			// The weight under the current 
			// combination does not need to be multiplied by fact.
			equPC12G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equPC15G.setWeight(SQR(1/(errmeasLG*errratioG)));
			equLC12G.setWeight(SQR(1/errmeasLG));
			equLC15G.setWeight(SQR(1/errmeasLG));

			// gal
			equPC15E.addVarSet(xyzVarSet);
			equLC15E.addVarSet(xyzVarSet);
			equPC17E.addVarSet(xyzVarSet);
			equLC17E.addVarSet(xyzVarSet);
			equPC18E.addVarSet(xyzVarSet);
			equLC18E.addVarSet(xyzVarSet);
			equPC16E.addVarSet(xyzVarSet);
			equLC16E.addVarSet(xyzVarSet);
			// add clk
			equPC15E.addVariable(dcdtGAL, 1.0);
			equLC15E.addVariable(dcdtGAL, 1.0);
			equPC17E.addVariable(dcdtGAL, 1.0);
			equLC17E.addVariable(dcdtGAL, 1.0);
			equPC18E.addVariable(dcdtGAL, 1.0);
			equLC18E.addVariable(dcdtGAL, 1.0);
			equPC16E.addVariable(dcdtGAL, 1.0);
			equLC16E.addVariable(dcdtGAL, 1.0);
			// add tropo
			equPC15E.addVariable(tropo);
			equLC15E.addVariable(tropo);
			equPC17E.addVariable(tropo);
			equLC17E.addVariable(tropo);
			equPC18E.addVariable(tropo);
			equLC18E.addVariable(tropo);
			equPC16E.addVariable(tropo);
			equLC16E.addVariable(tropo);
			// add ifb
			if (opts.PPPModel==Options::IFCB_IFB||
				opts.PPPModel==Options::IFB)
			{
				equPC17E.addVariable(ifbC7E, 1.0);
				equPC18E.addVariable(ifbC8E, 1.0);
				equPC16E.addVariable(ifbC6E, 1.0);
			}
			else if (opts.PPPModel==Options::IFCB)
			{
				equPC17E.addVariable(ifcbL7E, 1.0);
				equLC17E.addVariable(ifcbL7E, -1.0);
				equPC18E.addVariable(ifcbL8E, 1.0);
				equLC18E.addVariable(ifcbL8E, -1.0);
				equPC16E.addVariable(ifcbL6E, 1.0);
				equLC16E.addVariable(ifcbL6E, -1.0);
			}
			
			//add ambiguity
			equLC15E.addVariable(BLC15E, 1.0);
			equLC17E.addVariable(BLC17E, 1.0);
			equLC18E.addVariable(BLC18E, 1.0);
			equLC16E.addVariable(BLC16E, 1.0);

			// set weight
			equPC15E.setWeight( SQR(1/(errmeasLE*errratioE)) );
			equPC17E.setWeight( SQR(1/(errmeasLE*errratioE)) );
			equPC18E.setWeight( SQR(1/(errmeasLE*errratioE)) );
			equPC16E.setWeight( SQR(1/(errmeasLE*errratioE)) );
			equLC15E.setWeight( SQR(1/errmeasLE) );
			equLC17E.setWeight( SQR(1/errmeasLE) );
			equLC18E.setWeight( SQR(1/errmeasLE) );
			equLC16E.setWeight( SQR(1/errmeasLE) );

			// bds
			if (bdsfopt)
			{
				// normal pair-wise combination
					// xyz
				equPC15C.addVarSet(xyzVarSet);
				equLC15C.addVarSet(xyzVarSet);
				// 52
				equPC52C.addVarSet(xyzVarSet);
				equLC52C.addVarSet(xyzVarSet);
				equPC16C.addVarSet(xyzVarSet);
				equLC16C.addVarSet(xyzVarSet);
				equPC18C.addVarSet(xyzVarSet);
				equLC18C.addVarSet(xyzVarSet);
				// clk
				equPC15C.addVariable(dcdtBDS, 1.0);
				equLC15C.addVariable(dcdtBDS, 1.0);
				//52
				equPC52C.addVariable(dcdtBDS, 1.0);
				equLC52C.addVariable(dcdtBDS, 1.0);
				equPC16C.addVariable(dcdtBDS, 1.0);
				equLC16C.addVariable(dcdtBDS, 1.0);
				equPC18C.addVariable(dcdtBDS, 1.0);
				equLC18C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC15C.addVariable(tropo);
				equLC15C.addVariable(tropo);
				equPC52C.addVariable(tropo);
				equLC52C.addVariable(tropo);
				equPC16C.addVariable(tropo);
				equLC16C.addVariable(tropo);
				equPC18C.addVariable(tropo);
				equLC18C.addVariable(tropo);
				// ifb
				if (opts.PPPModel==Options::IFCB_IFB||
					opts.PPPModel==Options::IFB)
				{
					equPC52C.addVariable(ifbC2C, 1.0);
					equPC16C.addVariable(ifbC6C, 1.0);
					equPC18C.addVariable(ifbC8C, 1.0);
				}
				else if (opts.PPPModel==Options::IFCB)
				{
					equPC52C.addVariable(ifcbL2C, 1.0);
					equLC52C.addVariable(ifcbL2C, -1.0);
					equPC16C.addVariable(ifcbL6C, 1.0);
					equLC16C.addVariable(ifcbL6C, -1.0);
					equPC18C.addVariable(ifcbL8C, 1.0);
					equLC18C.addVariable(ifcbL8C, -1.0);
				}
				// ambiguity
				equLC15C.addVariable(BLC15C, 1.0);
				equLC52C.addVariable(BLC52C, 1.0);
				equLC16C.addVariable(BLC16C, 1.0);
				equLC18C.addVariable(BLC18C, 1.0);
				// set weight
				equPC15C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC52C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC16C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC18C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC15C.setWeight(SQR(1 / errmeasLC));
				equLC52C.setWeight(SQR(1 / errmeasLC));
				equLC16C.setWeight(SQR(1 / errmeasLC));
				equLC18C.setWeight(SQR(1 / errmeasLC));
			}
			else
			{
				// add xyz
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);
				//
				equPC61C.addVarSet(xyzVarSet);
				equLC61C.addVarSet(xyzVarSet);
				equPC25C.addVarSet(xyzVarSet);
				equLC25C.addVarSet(xyzVarSet);
				equPC28C.addVarSet(xyzVarSet);
				equLC28C.addVarSet(xyzVarSet);
				// add clk
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);
				//61
				equPC61C.addVariable(dcdtBDS, 1.0);
				equLC61C.addVariable(dcdtBDS, 1.0);
				equPC25C.addVariable(dcdtBDS, 1.0);
				equLC25C.addVariable(dcdtBDS, 1.0);
				equPC28C.addVariable(dcdtBDS, 1.0);
				equLC28C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);
				//61
				equPC61C.addVariable(tropo);
				equLC61C.addVariable(tropo);
				equPC25C.addVariable(tropo);
				equLC25C.addVariable(tropo);
				equPC28C.addVariable(tropo);
				equLC28C.addVariable(tropo);
				// add ifb
				if (opts.PPPModel==Options::IFCB)
				{
					equPC61C.addVariable(ifcbL1C, 1.0);
					equLC61C.addVariable(ifcbL1C, -1.0);
					equPC25C.addVariable(ifcbL5C, 1.0);
					equLC25C.addVariable(ifcbL5C, -1.0);
					equPC28C.addVariable(ifcbL8C, 1.0);
					equLC28C.addVariable(ifcbL8C, -1.0);
				}
				else if (opts.PPPModel == Options::IFCB_IFB ||
					opts.PPPModel == Options::IFB)
				{
					equPC61C.addVariable(ifbC1C, 1.0);
					equPC25C.addVariable(ifbC5C, 1.0);
					equPC28C.addVariable(ifbC8C, 1.0);
				}
				
				// add ambiguity
				equLC26C.addVariable(BLC26C, 1.0);
				equLC61C.addVariable(BLC21C, 1.0);
				equLC25C.addVariable(BLC25C, 1.0);
				equLC28C.addVariable(BLC28C, 1.0);
				// set weight
				equPC26C.setWeight( SQR(1/(errmeasLC*errratioC)) );
				equPC61C.setWeight( SQR(1/(errmeasLC*errratioC)) );
				equPC25C.setWeight( SQR(1/(errmeasLC*errratioC)) );
				equPC28C.setWeight( SQR(1/(errmeasLC*errratioC)) );
				equLC26C.setWeight( SQR(1/errmeasLC) );
				equLC61C.setWeight( SQR(1/errmeasLC) );
				equLC25C.setWeight( SQR(1/errmeasLC) );
				equLC28C.setWeight( SQR(1/errmeasLC) );
				if (bds2)
				{
					equPC27C.addVarSet(xyzVarSet);
					equLC27C.addVarSet(xyzVarSet);
					// clk
					equPC27C.addVariable(dcdtBDS, 1.0);
					equLC27C.addVariable(dcdtBDS, 1.0);
					// tropo
					equPC27C.addVariable(tropo);
					equLC27C.addVariable(tropo);
					// ifb
					if (opts.PPPModel==Options::IFCB_IFB||
						opts.PPPModel==Options::IFB)
						equPC27C.addVariable(ifbC7C, 1.0);
					else if (opts.PPPModel == Options::IFCB)
					{
						equPC27C.addVariable(ifcbL7C, 1.0);
						equLC27C.addVariable(ifcbL7C, -1.0);
					}
					
					// ambiguity
					equLC27C.addVariable(BLC27C, 1.0);
					// weight
					equPC27C.setWeight( SQR(1/(errmeasLC*errratioC)) );
					equLC27C.setWeight( SQR(1/errmeasLC) );
				}
			}	// end of if bdsfopt

			// equsystem add equs
			equSystem.clearEquations();
			if (codeOnly)
			{	// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equPC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equPC17E);
				equSystem.addEquation(equPC18E);
				equSystem.addEquation(equPC16E);
				//bds
				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equPC52C);
						equSystem.addEquation(equPC16C);
						equSystem.addEquation(equPC18C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equPC61C);
						equSystem.addEquation(equPC25C);
						equSystem.addEquation(equPC28C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
						}
					}
				}	// End of if !bdsComb

			}	// End of if codeOnly
			else
			{
				// gps
				equSystem.addEquation(equPC12G);
				equSystem.addEquation(equLC12G);
				equSystem.addEquation(equPC15G);
				equSystem.addEquation(equLC15G);
				//gal
				equSystem.addEquation(equPC15E);
				equSystem.addEquation(equLC15E);
				equSystem.addEquation(equPC17E);
				equSystem.addEquation(equLC17E);
				equSystem.addEquation(equPC18E);
				equSystem.addEquation(equLC18E);
				equSystem.addEquation(equPC16E);
				equSystem.addEquation(equLC16E);
				//bds

				if (!bdsComb)
				{
					if (bdsfopt)
					{
						equSystem.addEquation(equPC15C);
						equSystem.addEquation(equLC15C);
						equSystem.addEquation(equPC52C);
						equSystem.addEquation(equLC52C);
						equSystem.addEquation(equPC16C);
						equSystem.addEquation(equLC16C);
						equSystem.addEquation(equPC18C);
						equSystem.addEquation(equLC18C);
					}
					else
					{
						equSystem.addEquation(equPC26C);
						equSystem.addEquation(equLC26C);
						equSystem.addEquation(equPC61C);
						equSystem.addEquation(equLC61C);
						equSystem.addEquation(equPC25C);
						equSystem.addEquation(equLC25C);
						equSystem.addEquation(equPC28C);
						equSystem.addEquation(equLC28C);
						if (bds2)
						{
							equSystem.addEquation(equPC27C);
							equSystem.addEquation(equLC27C);
						}
					}
				}	// End of if !bdsComb
				
			}	// end of if codeOnly

		}	// end of if if12131415

		// bds3Combination
		if (bdsComb)
		{
			Equation equPC15C(TypeID::prefitPC15C, SatelliteSystem::BeiDou);
			Equation equLC15C(TypeID::prefitLC15C, SatelliteSystem::BeiDou);
			Equation equPC18C(TypeID::prefitPC18C, SatelliteSystem::BeiDou);
			Equation equLC18C(TypeID::prefitLC18C, SatelliteSystem::BeiDou);
			//
			Equation equPC158C(TypeID::prefitPC158C, SatelliteSystem::BeiDou);
			Equation equLC158C(TypeID::prefitLC158C, SatelliteSystem::BeiDou);
			
			Equation equPC26C(TypeID::prefitPC26C, SatelliteSystem::BeiDou);
			Equation equLC26C(TypeID::prefitLC26C, SatelliteSystem::BeiDou);
			

			if (bdsComb == Options::CCI)
			{	/* B1C/B2a,B1C/B2, B1I/B3I */
				// xyz
				equPC15C.addVarSet(xyzVarSet);
				equLC15C.addVarSet(xyzVarSet);
				equPC18C.addVarSet(xyzVarSet);
				equLC18C.addVarSet(xyzVarSet);
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);
				// clk
				equPC15C.addVariable(dcdtBDS, 1.0);
				equLC15C.addVariable(dcdtBDS, 1.0);
				equPC18C.addVariable(dcdtBDS, 1.0);
				equLC18C.addVariable(dcdtBDS, 1.0);
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);
				// tropo
				equPC15C.addVariable(tropo);
				equLC15C.addVariable(tropo);
				equPC18C.addVariable(tropo);
				equLC18C.addVariable(tropo);
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);
				// ifb
				if (opts.PPPModel==Options::IFCB)
				{
					equPC18C.addVariable(ifcbL8C, 1.0);
					equLC18C.addVariable(ifcbL8C, -1.0);
					equPC26C.addVariable(ifcbL6C, 1.0);
					equLC26C.addVariable(ifcbL6C, -1.0);
				}
				else if (opts.PPPModel == Options::IFB ||
					opts.PPPModel == Options::IFCB_IFB)
				{
					equPC18C.addVariable(ifbC8C, 1.0);
					equPC26C.addVariable(ifbC26C, 1.0);
				}
				// ambiguity
				equLC15C.addVariable(BLC15C, 1.0);
				equLC18C.addVariable(BLC18C, 1.0);
				equLC26C.addVariable(BLC26C, 1.0);
				// set weight
				equPC15C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC18C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equPC26C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC15C.setWeight(SQR(1 / errmeasLC));
				equLC18C.setWeight(SQR(1 / errmeasLC));
				equLC26C.setWeight(SQR(1 / errmeasLC));

			}	// end of if CCI
			else if (bdsComb == Options::CI)
			{	/* B1C/B2/B2, B1I/B3I */
				/// add xyz
				equPC158C.addVarSet(xyzVarSet);
				equLC158C.addVarSet(xyzVarSet);
				equPC26C.addVarSet(xyzVarSet);
				equLC26C.addVarSet(xyzVarSet);
				// add clk
				equPC158C.addVariable(dcdtBDS, 1.0);
				equLC158C.addVariable(dcdtBDS, 1.0);
				equPC26C.addVariable(dcdtBDS, 1.0);
				equLC26C.addVariable(dcdtBDS, 1.0);
				// add tropo
				equPC158C.addVariable(tropo);
				equLC158C.addVariable(tropo);
				equPC26C.addVariable(tropo);
				equLC26C.addVariable(tropo);
				// add ifb
				if (opts.PPPModel==Options::IFB||
					opts.PPPModel==Options::IFCB_IFB)
				{
					equPC158C.addVariable(ifbC8C, 1.0);
					equPC26C.addVariable(ifbC26C, 1.0);
				}
				// add ambituig
				equLC158C.addVariable(BLC158C, 1.0);
				equLC26C.addVariable(BLC26C, 1.0);
				//set weight
				equPC158C.setWeight(SQR(1 / (errmeasLC * errratioC)));
				equLC158C.setWeight(SQR(1 / errmeasLC));
				equPC26C.setWeight(SQR(1 / (errmeasLC * errratioC)) );
				equLC26C.setWeight(SQR(1 / errmeasLC));
			}	// end of if CI

			if (codeOnly)
			{
				if (bdsComb == Options::CCI)
				{
					equSystem.addEquation(equPC15C);
					equSystem.addEquation(equPC18C);
					equSystem.addEquation(equPC26C);
				}
				else if (bdsComb == Options::CI)
				{
					equSystem.addEquation(equPC158C);
					equSystem.addEquation(equPC26C);
				}
			}
			else
			{
				if (bdsComb == Options::CCI)
				{
					equSystem.addEquation(equPC15C);
					equSystem.addEquation(equLC15C);
					equSystem.addEquation(equPC18C);
					equSystem.addEquation(equLC18C);
					equSystem.addEquation(equPC26C);
					equSystem.addEquation(equLC26C);
				}
				else if (bdsComb == Options::CI)
				{
					equSystem.addEquation(equPC158C);
					equSystem.addEquation(equLC158C);
					equSystem.addEquation(equPC26C);
					equSystem.addEquation(equLC26C);
				}
			}
		}
		// End of if bdsComb
		setAmbSet();
	} // end of SolverPPPAll::setUpEquations(void)
	/// <summary>
	/// 
	/// </summary>
	/// <param name="sat"></param>
	/// <param name="linear"></param>
	/// <param name="type"></param>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="c"></param>
	void SolverPPPAll::getABCoeff(const SatID& sat, const LinearCombList& linear,
		const TypeID& type, double& a, double& b, double& c)
		noexcept(false)
	{
		try
		{
			// get alpha, beta value
			SatelliteSystem sys(sat.system);
			std::string typeStr(type.asString());
			int f1(asInt(typeStr.substr(8, 1))), f2(asInt(typeStr.substr(9, 1)));

			a = getAlpha(sys, f1, f2);
			b = getBeta(sys, f1, f2);
			if (a * b == 0.)
			{
				InvalidParameter e("Invalid Alpha & beta Parameter!");
				GPSTK_THROW(e);
			}
			// get C value
			c = 0;
			for (auto itLinear = linear.begin();
				itLinear != linear.end();
				++itLinear)
			{
				if ((*itLinear).header == type)
				{
					for (auto itValue = itLinear->body.begin();
						itValue != itLinear->body.end();
						++itValue)
					{
						c += SQR(itValue->second);
					}
					break;
				}
			}
		}
		catch (Exception& e)
		{
			cerr << e << endl;
			GPSTK_THROW(e);
		}
	}	// End of getABCoeff

	/// <summary>
	/// 
	/// </summary>
	/// <param name="equations"></param>
	/// <param name="R"></param>
	void SolverPPPAll::varrPWLC(Matrix<double>& R)
		noexcept(false)
	{
		//
		// equations contain the equations used to 
		// calculate the R matrix in the current epoch,
		// and adjust some values of the R matrix 
		// according to it.
		// 
		try
		{
			set<Equation> equations(equSystem.getCurrentEquationsSet());
			int row(0);	// record the equation row location
			// for loop all equation
			for (auto itEqu = equations.begin();
				itEqu != equations.end();
				++itEqu)
			{
				int deltaCol(0);
				double a, b, sqrR;
				SatID satSys((*itEqu).header.equationSat);

				if (bdsComb)
				{
					if (satSys.system != SatelliteSystem::BeiDou)
					{
						if (ionoopt == Options::UC1 ||
							ionoopt == Options::UC12 ||
							ionoopt == Options::UC123 ||
							ionoopt == Options::UC1234 ||
							ionoopt == Options::UC12345)
						{
							++row;
							continue;
						}
					}
				}

				LinearCombList linearCoeff(sysLinear[satSys.system]);
				TypeID equType( (*itEqu).getIndepType() );
				string strEquType( equType.asString().substr(0, 9) );
				bool match(false);
				// get a, b value;
				getABCoeff(satSys, linearCoeff, equType, a, b, sqrR);
				if(ionoopt==Options::IF12||
					ionoopt==Options::IF123||
					ionoopt==Options::IF1234||
					ionoopt==Options::IF12345)
				{
					// Main Diagonal Assignment
					R(row, row) *= sqrR;
					++row;
					continue;
				}
				// for loop  through the remaining equations
					// to find the other quations of the same sat.
				for (auto itEqu1 = ++itEqu;
					itEqu1 != equations.end();
					++itEqu1)
				{
					++deltaCol;
					SatID matchSatSys((*itEqu1).header.equationSat);
					TypeID equType1((*itEqu1).getIndepType());
					string strEquType1(equType1.asString().substr(0, 9));
					if (matchSatSys == satSys && strEquType == strEquType1)
					{
						match = true;
						double a1, b1, sqrR1;

						getABCoeff(matchSatSys, linearCoeff, equType1, a1, b1, sqrR1);
						// off-diagonal assignment
						R(row, row + deltaCol) = R(row + deltaCol, row)
							= R(row, row) * a * a1;
					}

				}	// end for loop through the remaining equations.
				--itEqu;

				// Main Diagonal Assignment
				R(row, row) *= sqrR;
				++row;
				
			}	// end of for loop all equation
		}
		catch (Exception& u)
		{
			// Throw an exception if something unexpected happens
			ProcessingException e(getClassName() + ":"
				+ StringUtils::asString(getIndex()) + ":"
				+ u.what());

			GPSTK_THROW(e);
		}

	}; // end of 'varrPWLC(const set<Equation>& equations, Matrix<double> R)'

	/// <summary>
	/// 
	/// </summary>
	/// <param name="gRin"></param>
	/// <returns></returns>
	gnssRinex& SolverPPPAll::preCompute(gnssRinex& gRin)
	{
		try
		{
			sourceRinex sRin;
			sRin.addGnssRinex(gRin);
			equSystem.Prepare(sRin);
			if (0)
			{
				equSystem.dumpEquations(cout);
			}
			int numUnknowns( equSystem.getTotalNumVariables() );

			currentUnkSet = equSystem.getVarUnknowns();
			
			measVector = equSystem.getPrefitsVector();
			hMatrix = equSystem.getGeometryMatrix();
			rMatrix = equSystem.getWeightsMatrix();

			if (bdsComb)
			{
				varrPWLC(rMatrix);
			}
			else
			{
				if (ionoopt != Options::UC1 &&
					ionoopt != Options::UC12 &&
					ionoopt != Options::UC123 &&
					ionoopt != Options::UC1234 &&
					ionoopt != Options::UC12345)
				{
					varrPWLC(rMatrix);
				}
			}

			// reset filter
			CommonTime epoch(gRin.header.epoch);

			if (firstTime)
			{
				Vector<double> initialState(numUnknowns, 0.0);
				Matrix<double> initialCov(numUnknowns, numUnknowns, 0.0);
				for (auto v : currentUnkSet)
				{
					int index = v.getNowIndex();
					initialCov(index, index) = v.getInitialVariance();
				}

				// reset kalman filter state and covariance matrix
				xhat = initialState;
				P = initialCov;

				// firstTime false
				firstTime = false;
			}
			else
			{
				Vector<double> currentState(numUnknowns, 0.0);
				Matrix<double> currentCov(numUnknowns, numUnknowns, 0.0);
				// Fill the state vector
				for (auto v : currentUnkSet)
				{
					int now_index = v.getNowIndex();
					int old_index = v.getPreIndex();
					if (old_index != -1)
					{
						currentState(now_index) = solution(old_index);
					}
				}

				// We need a copy of 'currentUnkSet'
				VariableSet tempSet(currentUnkSet);
				for (auto v : currentUnkSet)
				{
					int nowIndex = v.getNowIndex();
					int oldIndex = v.getPreIndex();

					// old Variable
					if (oldIndex != -1)
					{
						for (auto v2 : tempSet)
						{
							int nowIndex2 = v2.getNowIndex();
							int oldIndex2 = v2.getPreIndex();

							// old Variable
							if (oldIndex2 != -1)
							{
								currentCov(nowIndex, nowIndex2)
									= currentCov(nowIndex2, nowIndex)
									= covMatrix(oldIndex, oldIndex2);
							}
							else // new variable
							{
								currentCov(nowIndex, nowIndex2)
									= currentCov(nowIndex2, nowIndex)
									= 0.0;
							}
						}
					}
					else // new variable
					{
						// only diagnol element is assigned to the new variable
						currentCov(nowIndex, nowIndex) = v.getInitialVariance();
					}

					// erase var
					tempSet.erase(v);

				}  // End of for( VariableSet::const_iterator itVar1 = currentUnkSet...'

				   // Reset Kalman filter to current state and covariance matrix
				xhat = currentState;
				P = currentCov;

			}	// end of if firstTime



			// State Transition Matrix (PhiMatrix)
			phiMatrix = equSystem.getPhiMatrix();
			qMatrix = equSystem.getQMatrix();

			traceSet(pTraceStream, "unknown state: ", currentUnkSet);
			traceMatrix(pTraceStream, "preFilter phiMatrix: ", phiMatrix);
			traceMatrix(pTraceStream, "preFilter qMatrix: ", qMatrix);

			if (0)
			{
				cout << "unknown state: " << std::endl;
				for(auto itUnk=currentUnkSet.begin();
					itUnk != currentUnkSet.end();
					++itUnk)
				{
					cout << *itUnk << std::endl;
				}
				cout << "phiMatrix" << endl;
				cout << phiMatrix << endl;

				cout << "qMatrix" << endl;
				cout << qMatrix << endl;
			}

		}
		catch (Exception& u)
		{
			// Throw an exception if something unexpected happens
			ProcessingException e(getClassName() + ":"
				+ StringUtils::asString(getIndex()) + ":"
				+ u.what());

			GPSTK_THROW(e);
		}
		return gRin;
	}	// end of gnssRinex& SolverPPPAll::preCompute(gnssRinex& gRin)
	/// <summary>
	/// 
	/// </summary>
	/// <param name="gRin"></param>
	/// <returns></returns>
	gnssRinex& SolverPPPAll::Compute(gnssRinex& gRin)
	{
		TimeUpdate(phiMatrix, qMatrix);
		MeasUpdate(measVector, hMatrix, rMatrix);
		//KalmanFilter(phiMatrix, qMatrix, measVector, hMatrix, rMatrix);
		return gRin;
	}	// end of gnssRinex& SolverPPPAll::Compute(gnssRinex& gRin)
	/// <summary>
	/// 
	/// </summary>
	/// <param name="gRin"></param>
	/// <returns></returns>
	gnssRinex& SolverPPPAll::postCompute(gnssRinex& gRin)
	{
		try
		{
			// store the results of ppp filter
			gRin.header.rcvPos = Triple(rcvPos.X() + getSolution(TypeID::dX, solution),
				rcvPos.Y() + getSolution(TypeID::dY, solution),
				rcvPos.Z() + getSolution(TypeID::dZ, solution));
			CommonTime epoch(gRin.header.epoch);
			CivilTime cv(epoch);

			gnssRinex gBack(gRin);

			TypeIDSet typeSet;

			typeSet.insert(TypeID::dX);
			typeSet.insert(TypeID::dY);
			typeSet.insert(TypeID::dZ);

			typeSet.insert(TypeID::wetMap);
			typeSet.insert(TypeID::weight);
			typeSet.insert(TypeID::satArc);

			if (ionoopt == Options::IF12)
			{
				typeSet.insert(TypeID::prefitPC12G);
				typeSet.insert(TypeID::prefitLC12G);
				typeSet.insert(TypeID::prefitPC15E);
				typeSet.insert(TypeID::prefitLC15E);
				if (bdsfopt)
				{
					typeSet.insert(TypeID::prefitPC15C);
					typeSet.insert(TypeID::prefitLC15C);
				}
				else
				{
					typeSet.insert(TypeID::prefitPC26C);
					typeSet.insert(TypeID::prefitLC26C);
				}
				
			}
			else if (ionoopt == Options::IF1234)
			{
				typeSet.insert(TypeID::prefitPC125G);
				typeSet.insert(TypeID::prefitLC125G);
				typeSet.insert(TypeID::prefitPC1578E);
				typeSet.insert(TypeID::prefitLC1578E);
				if (bdsfopt)
				{
					typeSet.insert(TypeID::prefitPC1526C);
					typeSet.insert(TypeID::prefitLC1526C);
				}
				else
				{
					typeSet.insert(TypeID::prefitPC2615C);
					typeSet.insert(TypeID::prefitLC2615C);
				}
			}
			else if (ionoopt == Options::UC12345)
			{
				typeSet.insert(TypeID::prefitC1G);
				typeSet.insert(TypeID::prefitC2G);
				typeSet.insert(TypeID::prefitC5G);
				typeSet.insert(TypeID::prefitL1G);
				typeSet.insert(TypeID::prefitL2G);
				typeSet.insert(TypeID::prefitL5G);
				typeSet.insert(TypeID::prefitC1E);
				typeSet.insert(TypeID::prefitC5E);
				typeSet.insert(TypeID::prefitC7E);
				typeSet.insert(TypeID::prefitC8E);
				typeSet.insert(TypeID::prefitC6E);
				typeSet.insert(TypeID::prefitL1E);
				typeSet.insert(TypeID::prefitL5E);
				typeSet.insert(TypeID::prefitL7E);
				typeSet.insert(TypeID::prefitL8E);
				typeSet.insert(TypeID::prefitL6E);
				typeSet.insert(TypeID::prefitC2C);
				typeSet.insert(TypeID::prefitC6C);
				typeSet.insert(TypeID::prefitC1C);
				typeSet.insert(TypeID::prefitC5C);
				typeSet.insert(TypeID::prefitC8C);
				typeSet.insert(TypeID::prefitL2C);
				typeSet.insert(TypeID::prefitL6C);
				typeSet.insert(TypeID::prefitL1C);
				typeSet.insert(TypeID::prefitL5C);
				typeSet.insert(TypeID::prefitL8C);
			}

			// keep needed observation data for backward processing
			gBack.keepOnlyTypeID(typeSet);

			// save forward-processing data
			if (forwardProcessing)
			{
				int numSat = gRin.numSats();
				// print states
				printOptState(epoch, gRin);
				// print forward float solutions
				if (pFloatSolStream&&opts.outPos)
					printSols(pFloatSolStream, epoch, solution, covMatrix, numSat);

				// store the forward rinex data
				forwardRinexData[epoch] = gBack;
				forwardStateData[epoch] = solution;
				forwardCovData[epoch] = covMatrix;
			}

		}
		catch (Exception& u)
		{
			// Throw an exception if something unexpected happens
			ProcessingException e(getClassName() + ":"
				+ StringUtils::asString(getIndex()) + ":"
				+ u.what());

			GPSTK_THROW(e);

		}

		return gRin;
	}	// end of gnssRinex& SolverPPPAll::postCompute(gnssRinex& gRin)
	/// <summary>
	/// 
	/// </summary>
	/// <param name="phiMatrix"></param>
	/// <param name="processNoiseCovariance"></param>
	/// <returns></returns>
	int SolverPPPAll::TimeUpdate(const Matrix<double>& phiMatrix,
		const Matrix<double>& processNoiseCovariance)
		noexcept(false)
	{
		// Check whether the phiMatrix is square
		if (!(phiMatrix.isSquare()))
		{
			InvalidSolver e("phiMatrix is not square");
			GPSTK_THROW(e);
		}

		// Get the number of unknowns being processed
		int numUnknowns(equSystem.getTotalNumVariables());
		int stateSize(xhat.size());
		if (!(stateSize == numUnknowns))
		{
			InvalidSolver e("TimeUpdate(): Size of a posteriori state estimation vector \
do not match the number of unknowns");
			GPSTK_THROW(e);
		}

		int phiRow = static_cast<int>(phiMatrix.rows());
		if (!(phiRow == numUnknowns))
		{
			InvalidSolver e("Number of unknowns does not match dimension \
of phiMatrix");
			GPSTK_THROW(e);
		}

		if (!(qMatrix.isSquare()))
		{
			InvalidSolver e("qMatrix is not square");
			GPSTK_THROW(e);
		}

		int qRow = static_cast<int>(qMatrix.rows());
		if (!(qRow == numUnknowns))
		{
			InvalidSolver e("Number of unknowns does not match dimension \
of qMatrix");
			GPSTK_THROW(e);
		}

		try
		{
			// Compute the a priori state vector
			xhatminus = phiMatrix * xhat;	
			Matrix<double> phiT(transpose(phiMatrix));
			// Compute the a priori estimate error covariance matrix
			Pminus = phiMatrix * P * phiT + processNoiseCovariance;

		}
		catch (...)
		{
			InvalidSolver e("TimeUpdate(): Unable to predict next state.");
			GPSTK_THROW(e);
			return -1;
		}
		traceVector(pTraceStream, "predict xhat: ", xhatminus);
		traceMatrix(pTraceStream, "predict Pminus: ", Pminus);
		return 0;
	}	//end of timeUpdate()
	/// <summary>
	/// 
	/// </summary>
	/// <param name="prefitResiduals"></param>
	/// <param name="designMatrix"></param>
	/// <param name="weightMatrix"></param>
	/// <returns></returns>
	int SolverPPPAll::MeasUpdate(const Vector<double>& prefitResiduals,
		const Matrix<double>& designMatrix,
		const Matrix<double>& weightMatrix)
		noexcept(false)
	{
		// By default, results are invalid
		valid = false;
		if (!(weightMatrix.isSquare()))
		{
			InvalidSolver e("MeasUpdate(): Weight matrix is not square");
			GPSTK_THROW(e);
		}


		int wRow = static_cast<int>(weightMatrix.rows());
		int pRow = static_cast<int>(prefitResiduals.size());
		if (!(wRow == pRow))
		{
			InvalidSolver e("MeasUpdate(): prefitResiduals size does not match dimension of \
weightMatrix");
			GPSTK_THROW(e);
		}


		int gRow = static_cast<int>(designMatrix.rows());
		if (!(gRow == pRow))
		{
			InvalidSolver e("MeasUpdate(): prefitResiduals size does not match \
dimension of designMatrix");
			GPSTK_THROW(e);
		}


		int gCol = static_cast<int>(designMatrix.cols());
		int aprioriStateRow = static_cast<int>(xhatminus.size());
		if (!(gCol == aprioriStateRow))
		{
			InvalidSolver e("MeasUpdate(): designMatrix size and a priori state \
estimation vector do not match.");
			GPSTK_THROW(e);
		}


		int pMCol = static_cast<int>(Pminus.cols());
		if (pMCol != aprioriStateRow)
		{
			InvalidSolver e("MeasUpdate(): Sizes of a priori error covariance \
matrix and a priori state estimation vector do not match.");
			GPSTK_THROW(e);
		}

		//Matrix<double> invWeight(inverseChol(weightMatrix));
		//// After checking sizes, let's do the real correction work
		Matrix<double> invPMinus;
		Matrix<double> designMatrixT(transpose(designMatrix));

		try
		{
			invPMinus = inverseChol(Pminus);
		}
		catch (...)
		{
			InvalidSolver e("MeasUpdate(): Unable to compute invPMinus matrix.");
			GPSTK_THROW(e);
			return -1;
		}

		try
		{
			Matrix<double> invTemp(designMatrixT * weightMatrix * designMatrix +
				invPMinus);
			
			/*Matrix<double> invTemp(designMatrixT * invWeight * designMatrix +
				invPMinus);*/
			// Compute the a posteriori error covariance matrix
			P = inverseChol(invTemp);
		}
		catch (...)
		{
			InvalidSolver e("MeasUpdate(): Unable to compute P matrix.");
			GPSTK_THROW(e);
			return -1;
		}

		try
		{
			xhat = P * ((designMatrixT * weightMatrix * prefitResiduals) +
				(invPMinus * xhatminus));
			/*xhat = P * ((designMatrixT * invWeight * prefitResiduals) +
				(invPMinus * xhatminus));*/
		}
		catch (Exception e)
		{
			InvalidSolver eis("MeasUpdate(): Unable to compute xhat.");
			GPSTK_THROW(eis);
			return -1;
		}

		xhatminus = xhat;
		Pminus = P;
		solution = xhat;
		covMatrix = P;
		// Compute the postfit residuals Vector
		postfitResiduals = prefitResiduals - (designMatrix * solution);
		// If everything is fine so far, then the results should be valid
		valid = true;

		//trace
		traceVector(pTraceStream, "afterFilter xhat: ", xhat);
		traceVector(pTraceStream, "afterFilter postfitResiduals: ", postfitResiduals);

		return 0;
	}	// end of measUpdate();
	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	/// <param name="varSet"></param>
	/// <returns></returns>
	VariableSet SolverPPPAll::getVarSet(const TypeID& type, const VariableSet& varSet) const
		noexcept(false)
	{
		VariableSet tempVarSet;

		// Declare an iterator for 'stateMap' and go to the first element
		for (auto varIt = varSet.begin(); varIt != varSet.end(); varIt++)
		{
			if ((*varIt).getType() == type)
			{
				tempVarSet.insert((*varIt));
			}
		};

		// Else, return the corresponding value
		return tempVarSet;
	}	//end of getVarSet()
	void SolverPPPAll::printTime(std::ofstream* outStr,
		const CommonTime& epoch)
	{
		YDSTime time(epoch);
		CivilTime cvT(epoch);

		// print time
		*outStr << fixed
			<< setprecision(2)
			<< setw(4) << time.year
			<< setw(3) << cvT.month
			<< setw(3) << cvT.day
			<< setw(3) << cvT.hour
			<< setw(3) << cvT.minute
			<< setw(6) << cvT.second
			<< setw(4) << time.doy
			<< setw(9) << time.sod;
	}
	void SolverPPPAll::printOptState(const CommonTime& epoch,
		const gnssRinex& gRin)
	{
		// set out file
		if (opts.outState && !pFloatStaStream)
		{
			std::string fOutf = opts.aimOutPath + ".state";
			pFloatStaStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pFloatStaStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pFloatStaStream = NULL;
				delete pFloatStaStream;
			}
		}
		if (opts.outAmb && !pFloatAmbStream)
		{
			std::string fOutf = opts.aimOutPath + ".amb";
			pFloatAmbStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pFloatAmbStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pFloatAmbStream = NULL;
				delete pFloatAmbStream;
			}
		}
		if (opts.outEle && !pEleAziStream)
		{
			std::string fOutf = opts.aimOutPath + ".ele";
			pEleAziStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pEleAziStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pEleAziStream = NULL;
				delete pEleAziStream;
			}
		}
		if (opts.outPdop && !pPDOPStream)
		{
			std::string fOutf = opts.aimOutPath + ".pdop";
			pPDOPStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pPDOPStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pPDOPStream = NULL;
				delete pPDOPStream;
			}
		}
		if (opts.outResL && !pResLStream)
		{
			std::string fOutf = opts.aimOutPath + ".resl";
			pResLStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pResLStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pResLStream = NULL;
				delete pResLStream;
			}
		}
		if (opts.outResP && !pResPStream)
		{
			std::string fOutf = opts.aimOutPath + ".resp";
			pResPStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pResPStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pResPStream = NULL;
				delete pResPStream;
			}
		}
		if (opts.outTrop && !pTropStream)
		{
			std::string fOutf = opts.aimOutPath + ".trop";
			pTropStream = new ofstream(fOutf.c_str(), ios::out);
			if (!pTropStream->is_open())
			{
				std::cout << "Cannot open file: " << fOutf << std::endl;
				pTropStream = NULL;
				delete pTropStream;
			}
		}
		// End of out file stream
		// output file
		// Equation sets and order
		std::set<Equation> equSys(equSystem.getCurrentEquationsSet());
		// Determine the parameters that need to be output
		if (pFloatStaStream)
		{
			// print time line
			*pFloatStaStream << "# ";
			printTime(pFloatStaStream, epoch);
			*pFloatStaStream << setw(6) << gRin.numSats();
			*pFloatStaStream << std::endl;
			// record data
			satTypeStrValue satValues;
			auto itData = gRin.body.begin();
			for(itData;
				itData!=gRin.body.end();
				++itData)
			{
				SatID sat(itData->first);
				try
				{
					double dt((itData->second.find(TypeID::cdtSat)->second) / C_MPS);
					double ele(itData->second.find(TypeID::elevation)->second);
					double azi(itData->second.find(TypeID::azimuth)->second);
					int cs(itData->second.find(TypeID::CSFlag)->second);
					// record value
					satValues[sat].insert(make_pair("dtSat", dt));
					satValues[sat].insert(make_pair("elevation", ele));
					satValues[sat].insert(make_pair("azimuth", azi));
					satValues[sat].insert(make_pair("CS", cs));
				}
				catch (...)
				{
					continue;
				}
				
			}	// End of for loop gRin data
			// record amb
			auto itUnk = currentUnkSet.begin();
			for (itUnk;
				itUnk != currentUnkSet.end();
				++itUnk)
			{
				SatID sat(itUnk->getSatellite());
				string strType(itUnk->getType().asString());
				int ind(itUnk->getNowIndex());
				double value(solution(ind));
				// check
				if (strType.find("BL") == string::npos)
					continue;
				// record value
				satValues[sat].insert(make_pair(strType, value));
			}//End of record amb
			// record residual
			int i(0);
			auto itEqu = equSys.begin();
			for (itEqu;
				itEqu != equSys.end();
				++itEqu)
			{
				SatID sat(itEqu->getEquationSat());
				TypeID equType(itEqu->getIndepType());
				std::string strType(equType.asString());
				if (strType.find("prefit") == std::string::npos)
				{++i; continue; }
				double value(postfitResiduals(i));
				// record value
				satValues[sat].insert(make_pair(strType.substr(6), value));
				++i;
			}	// End of for loop all the equations
			// out result
			// out data
			auto itValues = satValues.begin();
			for (itValues;
				itValues != satValues.end();
				++itValues)
			{
				SatID sat(itValues->first);
				// out sat
				*pFloatStaStream << " " << asChar(sat.system);
				if (sat.id < 10)
					*pFloatStaStream << "0";
				*pFloatStaStream << sat.id;
				// out values
				for (auto itType = itValues->second.begin();
					itType != itValues->second.end();
					++itType)
				{
					string strType(itType->first);
					double value(itType->second);
					// out type
					*pFloatStaStream
						<< setw(10)
						<< strType;
					// out value
					*pFloatStaStream
						<< fixed
						<< setprecision(4)
						<< setw(11) << value;
				}	// End of the types
				// new line
				*pFloatStaStream << endl;
			}	// End of for loop the data
		}	// End of output states
		if (pPDOPStream)
		{
			// print time
			*pPDOPStream << "# ";
			printTime(pPDOPStream, epoch);
			*pPDOPStream << setw(6) << gRin.numSats();
			*pPDOPStream << " #";
			double value(gRin.header.dops[1]);
			*pPDOPStream
				<< fixed
				<< setprecision(4)
				<< setw(10)
				<< value
				<< endl;
		}
		if (pFloatAmbStream)
		{
			// print time
			*pFloatAmbStream << "# ";
			printTime(pFloatAmbStream, epoch);
			*pFloatAmbStream << setw(6) << gRin.numSats();
			*pFloatAmbStream << std::endl;
			// print value
			satTypeStrValue satValues;
			// record data
			auto itUnk = currentUnkSet.begin();
			for(itUnk;
				itUnk!=currentUnkSet.end();
				++itUnk)
			{
				SatID sat(itUnk->getSatellite());
				string strType(itUnk->getType().asString());
				int ind(itUnk->getNowIndex());
				double value(solution(ind));
				// check
				if (strType.find("BL") == string::npos)
					continue;
				// record value
				satValues[sat].insert(make_pair(strType, value));
			}
			// out data
			auto itValues = satValues.begin();
			for(itValues;
				itValues!=satValues.end();
				++itValues)
			{
				SatID sat(itValues->first);
				// out sat
				*pFloatAmbStream << " " << asChar(sat.system);
				if (sat.id < 10)
					*pFloatAmbStream << "0";
				*pFloatAmbStream << sat.id;
				// out values
				for(auto itType=itValues->second.begin();
					itType!=itValues->second.end();
					++itType)
				{
					string strType(itType->first);
					double value(itType->second);
					// out type
					*pFloatAmbStream
						<< setw(10)
						<< strType;
					// out value
					*pFloatAmbStream
						<< fixed
						<< setprecision(4)
						<< setw(10) << value;
				}	// End of the types
				// new line
				*pFloatAmbStream << endl;
			}	// End of for loop the data
		}	// End of if pFloatAmbStream
		if (pEleAziStream)
		{
			// time
			(*pEleAziStream) << "# ";
			printTime(pEleAziStream, epoch);
			*pEleAziStream << setw(6) << gRin.numSats();
			*pEleAziStream << endl;
			// value
			auto itData = gRin.body.begin();
			for (itData;
				itData != gRin.body.end();
				++itData)
			{
				SatID sat(itData->first);
				double ele(itData->second.getValue(TypeID::elevation));
				double azi(itData->second.getValue(TypeID::azimuth));
				// out
				*pEleAziStream << " "
					<< asChar(sat.system);
				if (sat.id < 10)
					*pEleAziStream << "0";
				* pEleAziStream 
					<< sat.id
					<< fixed
					<< setprecision(4)
					<< setw(10) << ele
					<< setw(10) << azi
					<< endl;
			}	// End of for loop
		} // End of output elev azi
		if (pTropStream)
		{
			// time
			(*pTropStream) << "# ";
			printTime(pTropStream, epoch);
			*pTropStream << setw(6) << gRin.numSats();
			*pTropStream << " #";
			// value
			int ind(getVariable(TypeID::wetMap, currentUnkSet).getNowIndex());
			double trop(solution(ind));
			// out
			*pTropStream
				<< fixed
				<< setprecision(4)
				<< setw(10) << trop
				<< endl;
		}
		if (pResLStream)
		{
			// variable
			satTypeStrValue satValues;
			// time
			(*pResLStream) << "# ";
			printTime(pResLStream, epoch);
			*pResLStream << setw(6) << gRin.numSats();
			*pResLStream << endl;
			int i(0);
			auto itEqu = equSys.begin();
			for (itEqu;
				itEqu != equSys.end();
				++itEqu)
			{
				SatID sat(itEqu->getEquationSat());
				TypeID equType(itEqu->getIndepType());
				std::string strType(equType.asString());
				if (strType.find("prefitL") == std::string::npos)
				{++i; continue; }

				double value(postfitResiduals(i));
				// record value
				satValues[sat].insert(make_pair(strType.substr(6), value));
				++i;
			}	// End of for loop all the equations
			// out res
			auto itValues = satValues.begin();
			for(itValues;
				itValues!=satValues.end();
				++itValues)
			{
				SatID sat(itValues->first);
				// out sat
				*pResLStream << " " << asChar(sat.system);
				if (sat.id < 10)
					*pResLStream << "0";
				*pResLStream << sat.id;
				// out values
				for(auto itType=itValues->second.begin();
					itType!=itValues->second.end();
					++itType)
				{
					string strType(itType->first);
					double value(itType->second);
					// out type
					*pResLStream 
						<< setw(10)
						<< strType;
					// out value
					*pResLStream
						<< fixed
						<< setprecision(4)
						<< setw(10) << value;
				}	// End of for loop the types
				// new line
				*pResLStream << endl;
			}	// End of for loop the record data
		}
		if (pResPStream)
		{
			// variable
			satTypeStrValue satValues;
			// time
			(*pResPStream) << "# ";
			printTime(pResPStream, epoch);
			*pResPStream << setw(6) << gRin.numSats();
			*pResPStream << endl;
			// record data
			int i(0);
			auto itEqu = equSys.begin();
			for (itEqu;
				itEqu != equSys.end();
				++itEqu)
			{
				SatID sat(itEqu->getEquationSat());
				TypeID equType(itEqu->getIndepType());
				std::string strType(equType.asString());
				if (strType.find("prefitP") == std::string::npos)
				{ ++i; continue; }

				double value(postfitResiduals(i));
				// record value
				satValues[sat].insert(make_pair(strType.substr(6), value));
				++i;
			}	// End of for loop all the equations
			// out res
			auto itValues = satValues.begin();
			for (itValues;
				itValues != satValues.end();
				++itValues)
			{
				SatID sat(itValues->first);
				// out sat
				*pResPStream << " " << asChar(sat.system);
				if (sat.id < 10)
					*pResPStream << "0";
				*pResPStream << sat.id;
				// out values
				for (auto itType = itValues->second.begin();
					itType != itValues->second.end();
					++itType)
				{
					string strType(itType->first);
					double value(itType->second);
					// out type
					*pResPStream
						<< setw(10)
						<< strType;
					// out value
					*pResPStream
						<< fixed
						<< setprecision(4)
						<< setw(10) << value;
				}	// End of for loop the types
				// new line
				*pResPStream << endl;
			}	// End of for loop the record data
		}	// End of if(pResPStream)

	}	// End of print Options states
	void SolverPPPAll::printNewLine()
	{
		if (pFloatSolStream != NULL)
			*pFloatSolStream << std::endl;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="outStr"></param>
	/// <param name="epoch"></param>
	/// <param name="stateVec"></param>
	/// <param name="covMatrix"></param>
	/// <param name="numSat"></param>
	void SolverPPPAll::printSols(std::ofstream* outStr,
		const CommonTime& epoch,
		const Vector<double>& state,
		const Matrix<double>& covMatrix,
		int numSat)
	{
		printTime(outStr, epoch);

		double dx, dy, dz;
		double x(0.), y(0.), z(0.), e(0.), n(0.), u(0.), rmsENU(0.), rmsXYZ(0.);
		double dx_, dy_, dz_;
		Vector<double> dXYZ(3, 0.0);
		Vector<double> enu(3, 0.);

		dx = getSolution(TypeID::dX, state);
		dy = getSolution(TypeID::dY, state);
		dz = getSolution(TypeID::dZ, state);

		// out esx, esy, esz
		x = rcvPos.X() + dx;
		y = rcvPos.Y() + dy;
		z = rcvPos.Z() + dz;

		dx_ = x - refPos.X();
		dy_ = y - refPos.Y();
		dz_ = z - refPos.Z();

		if (refPos.mag() != 0.0)
		{
			dXYZ[0] = dx_; dXYZ[1] = dy_; dXYZ[2] = dz_;
			enu = dXYZ * rotationMatrix;
			e = enu[0]; n = enu[1]; u = enu[2];
			rmsENU = std::sqrt(e * e + n * n + u * u);
			rmsXYZ = std::sqrt(SQR(dx_) + SQR(dy_) + SQR(dz_));
		}
		
		*outStr << fixed
			<< setprecision(4)
			<< setw(15) << x
			<< setw(15) << y
			<< setw(15) << z;

		if (outENU)
		{
			// out enu value
			*outStr << fixed
				<< setprecision(4)
				<< setw(9) << e
				<< setw(9) << n
				<< setw(9) << u
				<< setw(9) << rmsENU
				<< setw(3) << 6
				;
		}
		else
		{
			// out dx dy dz value
			*outStr << fixed
				<< setprecision(4)
				<< setw(9) << dx_
				<< setw(9) << dy_
				<< setw(9) << dz_
				<< setw(9) << rmsXYZ
				<< setw(3) << 6
				;
		}
		
		* outStr << endl;
	}	// end of printSols
	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	/// <param name="varSet"></param>
	/// <returns></returns>
	Variable SolverPPPAll::getVariable(const TypeID& type,
		const VariableSet& varSet) const
		noexcept(false)
	{
		// Declare an iterator for 'stateMap' and go to the first element
		VariableSet::const_iterator it = varSet.begin();

		// Look for a variable with the same type
		while ((*it).getType() != type &&
			it != varSet.end())
		{
			++it;

			// If the same type is not found, throw an exception
			if (it == varSet.end())
			{
				InvalidRequest e("Type not found in varSet.");
				GPSTK_THROW(e);
			}
		}

		// Else, return the corresponding value
		return (*it);
	}	// end of getVariable()
	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	/// <param name="stateVec"></param>
	/// <returns></returns>
	double SolverPPPAll::getSolution(const TypeID& type,
		const Vector<double>& stateVec) const
		noexcept(false)
	{
		// Declare an varIterator for 'stateMap' and go to the first element
		auto varIt = currentUnkSet.begin();
		while ((*varIt).getType() != type)
		{
			varIt++;
			// If the same type is not found, throw an exception
			if (varIt == currentUnkSet.end())
			{
				InvalidRequest e(type.asString() + "not found in solution vector.");
				GPSTK_THROW(e);
			}
		}

		// Else, return the corresponding value
		return stateVec((*varIt).getNowIndex());
	} // end of getSolution()
}	// End of fippp
