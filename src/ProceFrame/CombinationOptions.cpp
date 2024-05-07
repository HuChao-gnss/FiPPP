#pragma ident "$ ID: ChaoHu ragnwang* $"

#include "CombinationOptions.hpp"

using namespace gpstk;

namespace fippp
{
	void CombinationOptions::addComputeCSType(ComputeCombination& computeMW, 
		ComputeCombination& computeGF)
	{
		computeMW.setBdsFopt(opts.bdsfopt);
		computeGF.setBdsFopt(opts.bdsfopt);
		if (opts.ionoopt>=Options::UC12)
		{
			computeMW.addLinear(SatelliteSystem::GPS, linear.mw12CombOfGPS);
			computeGF.addLinear(SatelliteSystem::GPS, linear.gf12CombOfGPS);
			computeMW.addLinear(SatelliteSystem::Galileo, linear.mw15CombOfGAL);
			computeGF.addLinear(SatelliteSystem::Galileo, linear.gf15CombOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw15CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf15CombOfBDS);
				}
				else
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw26CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf26CombOfBDS);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC123)
		{
			computeMW.addLinear(SatelliteSystem::GPS, linear.mw15CombOfGPS);
			computeGF.addLinear(SatelliteSystem::GPS, linear.gf15CombOfGPS);
			computeMW.addLinear(SatelliteSystem::Galileo, linear.mw17CombOfGAL);
			computeGF.addLinear(SatelliteSystem::Galileo, linear.gf17CombOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					//computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw12CombOfBDS);
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw52CombOfBDS);
					//computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf12CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf52CombOfBDS);
				}
				else
				{
					if (opts.bds2)
					{
						computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw27CombOfBDS);
						computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf27CombOfBDS);
					}
					//computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw21CombOfBDS);
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw61CombOfBDS);
					//computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf21CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf61CombOfBDS);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC1234)
		{
			computeMW.addLinear(SatelliteSystem::Galileo, linear.mw18CombOfGAL);
			computeGF.addLinear(SatelliteSystem::Galileo, linear.gf18CombOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw16CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf16CombOfBDS);
				}
				else
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw25CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf25CombOfBDS);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC12345)
		{
			computeMW.addLinear(SatelliteSystem::Galileo, linear.mw16CombOfGAL);
			computeGF.addLinear(SatelliteSystem::Galileo, linear.gf16CombOfGAL);
			// bds 
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw18CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf18CombOfBDS);
				}
				else
				{
					computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw28CombOfBDS);
					computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf28CombOfBDS);
				}
			}
			
		}

		if (opts.bdsComb)
		{
			computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw15CombOfBDS);
			computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw12CombOfBDS);
			computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw16CombOfBDS);
			computeMW.addLinear(SatelliteSystem::BeiDou, linear.mw18CombOfBDS);
			//
			computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf15CombOfBDS);
			computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf12CombOfBDS);
			computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf16CombOfBDS);
			computeGF.addLinear(SatelliteSystem::BeiDou, linear.gf18CombOfBDS);
		}
	}
	/// <summary>
	/// add the CS type in the ppp processing
	/// </summary>
	/// <param name="detectGFCS"></param>
	/// <param name="detectMWCS"></param>
	void CombinationOptions::addCSType(DetectCSGF& detectGFCS, DetectCSMW& detectMWCS)
	{
		if (opts.ionoopt >= Options::UC12)
		{
			// gps
			detectGFCS.addType(SatelliteSystem::GPS, TypeID::GF12G);
			detectMWCS.addType(SatelliteSystem::GPS, TypeID::MW12G);
			//gal
			detectGFCS.addType(SatelliteSystem::Galileo, TypeID::GF15E);
			detectMWCS.addType(SatelliteSystem::Galileo, TypeID::MW15E);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF15C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW15C);
				}
				else
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF26C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW26C);
				}
				if (opts.bds2)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF26C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW26C);
				}
			}
			

		}
		if (opts.ionoopt >= Options::UC123)
		{
			detectGFCS.addType(SatelliteSystem::GPS, TypeID::GF15G);
			detectMWCS.addType(SatelliteSystem::GPS, TypeID::MW15G);
			//gal
			detectGFCS.addType(SatelliteSystem::Galileo, TypeID::GF17E);
			detectMWCS.addType(SatelliteSystem::Galileo, TypeID::MW17E);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF52C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW52C);
				}
				else
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF61C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW61C);
				}
				if (opts.bds2)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF27C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW27C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC1234)
		{
			detectGFCS.addType(SatelliteSystem::Galileo, TypeID::GF18E);
			detectMWCS.addType(SatelliteSystem::Galileo, TypeID::MW18E);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF16C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW16C);
				}
				else
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF25C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW25C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC12345)
		{
			detectGFCS.addType(SatelliteSystem::Galileo, TypeID::GF16E);
			detectMWCS.addType(SatelliteSystem::Galileo, TypeID::MW16E);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF18C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW18C);
				}
				else
				{
					detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF28C);
					detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW28C);
				}
			}
			
		}

		if (opts.bdsComb)
		{
			detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF15C);
			detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW15C);
			//detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF12C);
			detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF52C);
			//detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW12C);
			detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW52C);
			detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF16C);
			detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW16C);
			detectGFCS.addType(SatelliteSystem::BeiDou, TypeID::GF18C);
			detectMWCS.addType(SatelliteSystem::BeiDou, TypeID::MW18C);
		}

	}
	/// <summary>
	/// add compute linear combination
	/// </summary>
	/// <param name="computeLC"></param>
	void CombinationOptions::addLCType(ComputeCombination& computeLC)
	{
		computeLC.setBdsFopt(opts.bdsfopt);
		if (opts.ionoopt == Options::IF12)
		{
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc12CombOfGPS);

			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc15CombOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
				}
			}
		}	// end of if12
		else if (opts.ionoopt == Options::IF1213)
		{
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc15CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc15CombOfGPS);
			// gal
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc17CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc17CombOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc12CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc52CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc12CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc52CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc21CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc61CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc21CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc61CombOfBDS);
					if (opts.bds2)
					{
						computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc27CombOfBDS);
						computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc27CombOfBDS);
					}
				}
			}	// end of if !bdsComb
		}	// end of if1213
		else if (opts.ionoopt == Options::IF123)
		{
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc125CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc125CombOfGPS);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc157CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc157CombOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc152CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc152CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc261CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc261CombOfBDS);
				}
				if (opts.bds2)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc267CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc267CombOfBDS);
				}
			}	// end of if !bdsComb
			
		}	// end of if IF123
		else if (opts.ionoopt == Options::IF121314)
		{
			// compute linear combination value
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc15CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc15CombOfGPS);
			// gal
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc17CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc18CombOfGAL);

			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc17CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc18CombOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc27CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc27CombOfBDS);
				}
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc12CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc52CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc16CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc12CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc52CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc16CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc21CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc61CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc25CombOfBDS);

					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc21CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc61CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc25CombOfBDS);
				}
			}	// end of if !bdsComb
			
		}	// end of IF121314
		else if (opts.ionoopt == Options::IF1234)
		{
			// compute linear combination value
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc125CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc125CombOfGPS);
			// gal
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc1578CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc1578CombOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc267CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc267CombOfBDS);
				}
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc1526CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc1526CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc2615CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc2615CombOfBDS);
				}
			}// end of if !bdsComb
		}	// end of if if1234
		else if (opts.ionoopt == Options::IF12131415)
		{
			// compute linear combination value
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc15CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc12CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc15CombOfGPS);
			// gal
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc17CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc18CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc16CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc15CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc17CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc18CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc16CombOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc27CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc27CombOfBDS);
				}
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc12CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc52CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc16CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc18CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc12CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc52CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc16CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc18CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc21CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc61CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc25CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc28CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc21CombOfBDS);
					//computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc61CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc25CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc28CombOfBDS);
				}
			}
			
		}	// end of IF12131415
		else if (opts.ionoopt == Options::IF12345)
		{
			// compute linear combination value
			computeLC.addLinear(SatelliteSystem::GPS, linear.pc125CombOfGPS);
			computeLC.addLinear(SatelliteSystem::GPS, linear.lc125CombOfGPS);
			// gal
			computeLC.addLinear(SatelliteSystem::Galileo, linear.pc15786CombOfGAL);
			computeLC.addLinear(SatelliteSystem::Galileo, linear.lc15786CombOfGAL);
			// bds
			if (!opts.bdsComb)
			{

				if (opts.bds2)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc267CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc267CombOfBDS);
				}
				if (opts.bdsfopt)
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15268CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15268CombOfBDS);
				}
				else
				{
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26158CombOfBDS);
					computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26158CombOfBDS);
				}
			}
		}	//end of add linear combination
		// end of if IF12345

		if (opts.bdsComb)
		{
			if (opts.bdsComb == Options::CCI)
			{
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc15CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc18CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc15CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc18CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
			}
			else if (opts.bdsComb == Options::CI)
			{
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc158CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc158CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.pc26CombOfBDS);
				computeLC.addLinear(SatelliteSystem::BeiDou, linear.lc26CombOfBDS);
			}
		}
	}
	/// <summary>
	/// add compute prefit linear combination
	/// </summary>
	/// <param name="computePrefit"></param>
	void CombinationOptions::addPrefitType(ComputeCombination& computePrefit, ComputeResidual& computeRes)
	{
		computePrefit.setBdsFopt(opts.bdsfopt);
		// prefit residual single frequency
		if (opts.ionoopt >= Options::UC1)
		{
			computePrefit.addLinear(SatelliteSystem::GPS, linear.c1PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.l1PrefitOfGPS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitC1G, 1.0);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitL1G, -1.0);

			computePrefit.addLinear(SatelliteSystem::Galileo, linear.c1PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.l1PrefitOfGAL);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitC1E, 1.0);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitL1E, -1.0);
			//gps
			computeRes.addLinear(SatelliteSystem::GPS, linear.c1ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.l1ResOfGPS);
			//gal
			computeRes.addLinear(SatelliteSystem::Galileo, linear.c1ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.l1ResOfGAL);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c1PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l1PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC1C, 1.0);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL1C, -1.0);
					// bds
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c1ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l1ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c2PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l2PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC2C, 1.0);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL2C, -1.0);
					// bds
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c2ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l2ResOfBDS);
				}
			}
		}
		if (opts.ionoopt >= Options::UC12)
		{
			// gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.c2PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.l2PrefitOfGPS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitC2G, GAMMA_GPS_L1L2);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitL2G, -GAMMA_GPS_L1L2);
			computeRes.addLinear(SatelliteSystem::GPS, linear.c2ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.l2ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.c5PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.l5PrefitOfGAL);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitC5E, GAMMA_GAL_L1L5);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitL5E, -GAMMA_GAL_L1L5);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.c5ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.l5ResOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c5PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l5PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC5C, GAMMA_BDS_L1L5);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL5C, -GAMMA_BDS_L1L5);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c5ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l5ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c6PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l6PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC6C, GAMMA_BDS_L2L6);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL6C, -GAMMA_BDS_L2L6);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c6ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l6ResOfBDS);
				}	// end of if bdsfopt
			}	// end of if !bdsComb
			
		}	// end of if ionoopt==IONOOPT_IF12
		if (opts.ionoopt >= Options::UC123)
		{
			computePrefit.addLinear(SatelliteSystem::GPS, linear.c5PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.l5PrefitOfGPS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitC5G, GAMMA_GPS_L1L5);
			computePrefit.addTypeIonCoeff(SatelliteSystem::GPS, TypeID::prefitL5G, -GAMMA_GPS_L1L5);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.c5ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.l5ResOfGPS);
			// Gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.c7PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.l7PrefitOfGAL);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitC7E, GAMMA_GAL_L1L7);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitL7E, -GAMMA_GAL_L1L7);
			//residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.c7ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.l7ResOfGAL);

			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c2PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l2PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC2C, GAMMA_BDS_L1L2);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL2C, -GAMMA_BDS_L1L2);
					//residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c2ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l2ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c1PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l1PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC1C, GAMMA_BDS_L2L1);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL1C, -GAMMA_BDS_L2L1);
					//residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c1ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l1ResOfBDS);
					if (opts.bds2)
					{
						computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c7PrefitOfBDS);
						computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l7PrefitOfBDS);
						computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC7C, GAMMA_BDS_L2L7);
						computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL7C, -GAMMA_BDS_L2L7);
						// residual
						computeRes.addLinear(SatelliteSystem::BeiDou, linear.c7ResOfBDS);
						computeRes.addLinear(SatelliteSystem::BeiDou, linear.l7ResOfBDS);
					}
				}
			}	// end of if !bdsComb
			
		}
		if (opts.ionoopt >= Options::UC1234)
		{
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.c8PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.l8PrefitOfGAL);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitC8E, GAMMA_GAL_L1L8);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitL8E, -GAMMA_GAL_L1L8);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.c8ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.l8ResOfGAL);

			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c6PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l6PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC6C, GAMMA_BDS_L1L6);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL6C, -GAMMA_BDS_L1L6);
					//residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c6ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l6ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c5PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l5PrefitOfBDS);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC5C, GAMMA_BDS_L2L5);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL5C, -GAMMA_BDS_L2L5);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.c5ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.l5ResOfBDS);
				}
			}	// end of if !bdsComb
		}
		if (opts.ionoopt >= Options::UC12345)
		{
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.c6PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.l6PrefitOfGAL);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitC6E, GAMMA_GAL_L1L6);
			computePrefit.addTypeIonCoeff(SatelliteSystem::Galileo, TypeID::prefitL6E, -GAMMA_GAL_L1L6);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.c6ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.l6ResOfGAL);
			if (!opts.bdsComb)
			{
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c8PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l8PrefitOfBDS);
				// residual
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.c8ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.l8ResOfBDS);
				if (opts.bdsfopt)
				{
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC8C, GAMMA_BDS_L1L8);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL8C, -GAMMA_BDS_L1L8);

				}
				else
				{
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC8C, GAMMA_BDS_L2L8);
					computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL8C, -GAMMA_BDS_L2L8);

				}
			}
			
		}

		if (opts.bdsComb)
		{
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c1PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l1PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c5PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l5PrefitOfBDS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC1C, 1.0);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC5C, GAMMA_BDS_L1L5);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL1C, -1.0);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL5C, -GAMMA_BDS_L1L5);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c2PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l2PrefitOfBDS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC2C, GAMMA_BDS_L1L2);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL2C, -GAMMA_BDS_L1L2);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c6PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l6PrefitOfBDS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC6C, GAMMA_BDS_L1L6);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL6C, -GAMMA_BDS_L1L6);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.c8PrefitOfBDS);
			computePrefit.addLinear(SatelliteSystem::BeiDou, linear.l8PrefitOfBDS);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitC8C, GAMMA_BDS_L1L8);
			computePrefit.addTypeIonCoeff(SatelliteSystem::BeiDou, TypeID::prefitL8C, -GAMMA_BDS_L1L8);
			// residual
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.c1ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.c5ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.c2ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.c6ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.c8ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.l1ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.l5ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.l2ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.l6ResOfBDS);
			computeRes.addLinear(SatelliteSystem::BeiDou, linear.l8ResOfBDS);
		}

		// linear combination prefit residual
		if (opts.ionoopt == Options::IF12)
		{
			//gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc12PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc12ResOfGPS);
			//gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc15PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc15ResOfGAL);
			// bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
				}
			}
		}	// end of if12
		else if (opts.ionoopt == Options::IF1213)
		{
			//gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc15PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc15PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc15ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc15ResOfGPS);
			
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc17PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc17PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc17ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc17ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc27PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc27PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc27ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc27ResOfBDS);
				}
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc52PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc52PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc52ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc52ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc61PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc61PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc61ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc61ResOfBDS);
				}
			}	// end of if !bdsComb
		}	// end of if1213
		else if (opts.ionoopt == Options::IF123)
		{
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc125PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc125PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc125ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc125ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc157PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc157PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc157ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc157ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc152PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc152PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc152ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc152ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc261PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc261PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc261ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc261ResOfBDS);
				}
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc267PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc267PrefitOfBDS);
					//
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc267ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc267ResOfBDS);
				}	// end of if bdsfopt
			}	// end of if !bdsComb
		}
		else if (opts.ionoopt == Options::IF121314)
		{
			//gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc15PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc15PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc15ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc15ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc17PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc18PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc17PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc18PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc17ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc17ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc18ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc18ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc27PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc27PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc27ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc27ResOfBDS);
				}
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc52PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc16PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc52PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc16PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc52ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc52ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc16ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc16ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc61PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc25PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc61PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc25PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc61ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc61ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc25ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc25ResOfBDS);
				}
			}	// end of if !bdsComb
			
		}
		else if (opts.ionoopt == Options::IF1234)
		{
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc125PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc125PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc125ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc125ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc1578PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc1578PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc1578ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc1578ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc1526PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc1526PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc1526ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc1526ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc2615PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc2615PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc2615ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc2615ResOfBDS);
				}
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc267PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc267PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc267ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc267ResOfBDS);
				}	// end of if bdsfopt
			}	// end of if !bdsComb		

		}	// end of ionoopt == IONOOPT_IF1234
		else if (opts.ionoopt == Options::IF12131415)
		{
			//gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc15PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc12PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc15PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc12ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc15ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc15ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc17PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc18PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc16PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc15PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc17PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc18PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc16PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc15ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc17ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc17ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc18ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc18ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc16ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc16ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc27PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc27PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc27ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc27ResOfBDS);
				}
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc52PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc16PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc18PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc12PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc52PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc16PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc18PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc52ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc52ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc16ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc16ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc18ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc18ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc61PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc25PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc28PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
					//computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc21PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc61PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc25PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc28PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc61ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc61ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc25ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc25ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc28ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc28ResOfBDS);
				}
			}	// end of if !bdsComb
			
		}	// end of ionoopt == IONOOPT_IF12131415
		else if (opts.ionoopt == Options::IF12345)
		{
			// gps
			computePrefit.addLinear(SatelliteSystem::GPS, linear.pc125PrefitOfGPS);
			computePrefit.addLinear(SatelliteSystem::GPS, linear.lc125PrefitOfGPS);
			// residual
			computeRes.addLinear(SatelliteSystem::GPS, linear.pc125ResOfGPS);
			computeRes.addLinear(SatelliteSystem::GPS, linear.lc125ResOfGPS);
			// gal
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.pc15786PrefitOfGAL);
			computePrefit.addLinear(SatelliteSystem::Galileo, linear.lc15786PrefitOfGAL);
			// residual
			computeRes.addLinear(SatelliteSystem::Galileo, linear.pc1578ResOfGAL);
			computeRes.addLinear(SatelliteSystem::Galileo, linear.lc1578ResOfGAL);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15268PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15268PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15268ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15268ResOfBDS);
				}
				else
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26158PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26158PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26158ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26158ResOfBDS);
				}
				if (opts.bds2)
				{
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc267PrefitOfBDS);
					computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc267PrefitOfBDS);
					// residual
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc267ResOfBDS);
					computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc267ResOfBDS);
				}	// end of if bdsfopt
			}
		}	// end of if ionoopt==IONOOPT_IF12345

		if (opts.bdsComb)
		{
			if (opts.bdsComb == Options::CCI)
			{
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc15PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc18PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc15PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc18PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
				// residual
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc15ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc18ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc15ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc18ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
			}
			else if (opts.bdsComb == Options::CI)
			{
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc158PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc158PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.pc26PrefitOfBDS);
				computePrefit.addLinear(SatelliteSystem::BeiDou, linear.lc26PrefitOfBDS);
				// residual
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc158ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.pc26ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc158ResOfBDS);
				computeRes.addLinear(SatelliteSystem::BeiDou, linear.lc26ResOfBDS);
			}
		}


	}
	
	/// <summary>
	/// add require obs type in ppp processing
	/// </summary>
	/// <param name="reqObs"></param>
	void CombinationOptions::addReqObsType(RequiredObs& reqObs)
	{
		if (opts.ionoopt >= Options::UC1)
		{
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::C1G);
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::L1G);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::C1E);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::L1E);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C1C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L1C);
				}
				else
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C2C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L2C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC12)
		{
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::C2G);			
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::L2G);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::C5E);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::L5E);
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C5C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L5C);
				}
				else
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C6C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L6C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC123)
		{
			// gps
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::C5G);
			reqObs.addRequiredType(SatelliteSystem::GPS, TypeID::L5G);
			// gal
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::C7E);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::L7E);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C2C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L2C);
				}
				else
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C1C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L1C);
				}
				if (opts.bds2)
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C7C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L7C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC1234)
		{
			//gal
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::C8E);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::L8E);
			//bds
			if (!opts.bdsComb)
			{
				if (opts.bdsfopt)
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C6C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L6C);
				}
				else
				{
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C5C);
					reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L5C);
				}
			}
			
		}
		if (opts.ionoopt >= Options::UC12345)
		{
			//gal
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::C6E);
			reqObs.addRequiredType(SatelliteSystem::Galileo, TypeID::L6E);
			//bds
			if (!opts.bdsComb)
			{
				reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C8C);
				reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L8C);
			}
		}	// end of modify reqObs

		// bdsComb
		if (opts.bdsComb)
		{
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C1C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L1C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C5C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L5C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C2C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L2C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C6C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L6C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::C8C);
			reqObs.addRequiredType(SatelliteSystem::BeiDou, TypeID::L8C);
		}

	}
}