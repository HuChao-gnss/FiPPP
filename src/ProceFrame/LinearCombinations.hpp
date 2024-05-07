#pragma ident "$Id$"

/**
 * @file LinearCombinations.hpp
 * This class defines handy linear combinations of GDS data.
 */

#ifndef LINEARCOMBINATIONS_HPP
#define LINEARCOMBINATIONS_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "DataStructures.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class defines handy linear combinations of GDS data.
       *
       * This class is meant to be used with the GNSS data structures (GDS)
       * objects found in "DataStructures" class, and it is intended to be
       * coupled with class ComputeLinear.hpp.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Define a LinearCombinations object
       *   LinearCombinations comb;
       *
       *      // Object to compute linear combinations of data
       *      // Linear combinations will be computed in a FIFO basis
       *   ComputeLinear linear;
       *
       *      // Add a linear combination to compute PC combination using C1
       *   linear.addLinear(comb.pcCombWithC1);
       *
       *      // Add a linear combination to compute prefit residual using PC
       *   linear.addLinear(comb.pcPrefit);
       *
       *
       *      // Load observation data
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *      // Declare a MOPSTropModel object, setting the defaults
       *   MOPSTropModel mopsTM( nominalPos.getAltitude(),
       *                         nominalPos.getGeodeticLatitude(), 30);
       *
       *      // Object to compute the tropospheric data
       *   ComputeTropModel computeTropo(mopsTM);
       *
       *      // Declare a basic modeler
       *   BasicModel basic(nominalPos, SP3EphList);
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *
       *      gRin >> basic >> computeTropo >> linear;
       *
       *         // Dump results
       *      gRin.body.dump(cout,1);
       *
       *   }
       *
       * @endcode
       *
       * @sa ComputeLinear.hpp
       */
    class LinearCombinations
    {
    public:

        /// Default constructor
        LinearCombinations();

        /// Definition to compute PC combination of GPS, C1 and C2
        gnssLinearCombination pc21CombOfGPS;
        gnssLinearCombination lc21CombOfGPS;

        gnssLinearCombination pc12CombOfGPS;
        gnssLinearCombination lc12CombOfGPS;
        // added by ragnwang
        gnssLinearCombination pc15CombOfGPS;
        gnssLinearCombination lc15CombOfGPS;
        gnssLinearCombination pc125CombOfGPS;
        gnssLinearCombination lc125CombOfGPS;

        /// Definition to compute PC combination of Galileo, C1 and C5
        gnssLinearCombination pc51CombOfGAL;
        gnssLinearCombination lc51CombOfGAL;
        // added by ragnwang
        gnssLinearCombination pc15CombOfGAL;
        gnssLinearCombination lc15CombOfGAL;
        gnssLinearCombination pc17CombOfGAL;
        gnssLinearCombination lc17CombOfGAL;
        gnssLinearCombination pc18CombOfGAL;
        gnssLinearCombination lc18CombOfGAL;
        gnssLinearCombination pc16CombOfGAL;
        gnssLinearCombination lc16CombOfGAL;
        
        gnssLinearCombination pc157CombOfGAL;
        gnssLinearCombination pc1578CombOfGAL;
        gnssLinearCombination lc157CombOfGAL;
        gnssLinearCombination lc1578CombOfGAL;
        gnssLinearCombination pc15786CombOfGAL;
        gnssLinearCombination lc15786CombOfGAL;

        /// Definition to compute PC combination of BDS, C2 and C7
        gnssLinearCombination pc27CombOfBDS;
        gnssLinearCombination lc27CombOfBDS;

        gnssLinearCombination pc26CombOfBDS;
        gnssLinearCombination lc26CombOfBDS;

        gnssLinearCombination pc267CombOfBDS;
        gnssLinearCombination lc267CombOfBDS;
        // added by ragnwang
        gnssLinearCombination pc21CombOfBDS;
        gnssLinearCombination lc21CombOfBDS;
        //61
        gnssLinearCombination pc61CombOfBDS;
        gnssLinearCombination lc61CombOfBDS;

        gnssLinearCombination pc25CombOfBDS;
        gnssLinearCombination lc25CombOfBDS;
        gnssLinearCombination pc28CombOfBDS;
        gnssLinearCombination lc28CombOfBDS;

        gnssLinearCombination pc261CombOfBDS;
        gnssLinearCombination pc2615CombOfBDS;
        gnssLinearCombination lc261CombOfBDS;
        gnssLinearCombination lc2615CombOfBDS;

        gnssLinearCombination pc26158CombOfBDS;
        gnssLinearCombination lc26158CombOfBDS;
        // bdsfopt
        gnssLinearCombination pc15CombOfBDS;
        gnssLinearCombination lc15CombOfBDS;
        gnssLinearCombination pc12CombOfBDS;
        gnssLinearCombination lc12CombOfBDS;
        // 52
        gnssLinearCombination pc52CombOfBDS;
        gnssLinearCombination lc52CombOfBDS;
        gnssLinearCombination pc16CombOfBDS;
        gnssLinearCombination lc16CombOfBDS;
        gnssLinearCombination pc18CombOfBDS;
        gnssLinearCombination lc18CombOfBDS;

        gnssLinearCombination pc152CombOfBDS;
        gnssLinearCombination pc1526CombOfBDS;
        gnssLinearCombination lc152CombOfBDS;
        gnssLinearCombination lc1526CombOfBDS;

        gnssLinearCombination pc15268CombOfBDS;
        gnssLinearCombination lc15268CombOfBDS;

        gnssLinearCombination pc158CombOfBDS;
        gnssLinearCombination lc158CombOfBDS;

        /// Definition to compute the MW combination of GPS, C1+C2 and L1+L2
        gnssLinearCombination mw12CombOfGPS;
        gnssLinearCombination mw15CombOfGPS;

        gnssLinearCombination mw21CombOfGPS;
        gnssLinearCombination mw25CombOfGPS;
        /// Definition to compute the MW combination of GPS, C1+C5 and L1+L5

        /// Definition to compute the MW combination of Galileo, C1+C5 and L1+L5
        gnssLinearCombination mw15CombOfGAL;
        gnssLinearCombination mw17CombOfGAL;
        gnssLinearCombination mw18CombOfGAL;
        gnssLinearCombination mw16CombOfGAL;

        gnssLinearCombination mw51CombOfGAL;
        gnssLinearCombination mw57CombOfGAL;
        gnssLinearCombination mw58CombOfGAL;
        gnssLinearCombination mw56CombOfGAL;

        /// Definition to compute the MW combination of BDS, C2+C7 and L2+L7
        gnssLinearCombination mw25CombOfBDS;
        gnssLinearCombination mw27CombOfBDS;
        gnssLinearCombination mw26CombOfBDS;
        gnssLinearCombination mw21CombOfBDS;
        gnssLinearCombination mw61CombOfBDS;
        // added by ragnwang
        gnssLinearCombination mw28CombOfBDS;
        gnssLinearCombination mw15CombOfBDS;
        gnssLinearCombination mw12CombOfBDS;
        gnssLinearCombination mw16CombOfBDS;
        gnssLinearCombination mw18CombOfBDS;
        gnssLinearCombination mw17CombOfBDS;


        gnssLinearCombination mw51CombOfBDS;
        gnssLinearCombination mw52CombOfBDS;
        gnssLinearCombination mw56CombOfBDS;
        gnssLinearCombination mw58CombOfBDS;
        gnssLinearCombination mw57CombOfBDS;

        gnssLinearCombination mw21CombOfQZS;
        gnssLinearCombination mw25CombOfQZS;
        // gf
        gnssLinearCombination gf12CombOfGPS;
        gnssLinearCombination gf15CombOfGPS;
        gnssLinearCombination gf15CombOfGAL;
        gnssLinearCombination gf17CombOfGAL;
        gnssLinearCombination gf18CombOfGAL;
        gnssLinearCombination gf16CombOfGAL;
        gnssLinearCombination gf26CombOfBDS;
        gnssLinearCombination gf21CombOfBDS;
        gnssLinearCombination gf61CombOfBDS;
        gnssLinearCombination gf25CombOfBDS;
        gnssLinearCombination gf28CombOfBDS;
        gnssLinearCombination gf27CombOfBDS;
        gnssLinearCombination gf15CombOfBDS;
        gnssLinearCombination gf12CombOfBDS;
        gnssLinearCombination gf52CombOfBDS;
        gnssLinearCombination gf16CombOfBDS;
        gnssLinearCombination gf18CombOfBDS;
        gnssLinearCombination gf17CombOfBDS;


        /// Definition to compute prefit residual of GPS
        gnssLinearCombination c1PrefitOfGPS;
        gnssLinearCombination c2PrefitOfGPS;
        gnssLinearCombination c5PrefitOfGPS;

        gnssLinearCombination BL1CombOfGPS;
        gnssLinearCombination BL2CombOfGPS;

        /// Definition to compute prefit residual of Galileo
        gnssLinearCombination c1PrefitOfGAL;
        gnssLinearCombination c5PrefitOfGAL;
        gnssLinearCombination c6PrefitOfGAL;
        gnssLinearCombination c7PrefitOfGAL;
        gnssLinearCombination c8PrefitOfGAL;


        /// Definition to compute prefit residual of BDS
        gnssLinearCombination c1PrefitOfBDS;
        gnssLinearCombination c2PrefitOfBDS;
        gnssLinearCombination c5PrefitOfBDS;
        gnssLinearCombination c8PrefitOfBDS;
        gnssLinearCombination c7PrefitOfBDS;
        gnssLinearCombination c6PrefitOfBDS;

        // GLONASS
        gnssLinearCombination c1PrefitOfGLO;
        gnssLinearCombination c2PrefitOfGLO;
        gnssLinearCombination c3PrefitOfGLO;
        gnssLinearCombination c4PrefitOfGLO;
        gnssLinearCombination c6PrefitOfGLO;

        // QZSS
        gnssLinearCombination c1PrefitOfQZS;
        gnssLinearCombination c2PrefitOfQZS;
        gnssLinearCombination c5PrefitOfQZS;
        gnssLinearCombination c6PrefitOfQZS;

        /// Definition to compute prefit residual of GPS
        gnssLinearCombination l1PrefitOfGPS;
        gnssLinearCombination l2PrefitOfGPS;
        gnssLinearCombination l5PrefitOfGPS;

        /// Definition to compute prefit residual of Galileo
        gnssLinearCombination l1PrefitOfGAL;
        gnssLinearCombination l5PrefitOfGAL;
        gnssLinearCombination l6PrefitOfGAL;
        gnssLinearCombination l7PrefitOfGAL;
        gnssLinearCombination l8PrefitOfGAL;

        /// Definition to compute prefit residual of BDS
        gnssLinearCombination l1PrefitOfBDS;
        gnssLinearCombination l2PrefitOfBDS;
        gnssLinearCombination l5PrefitOfBDS;
        gnssLinearCombination l6PrefitOfBDS;
        gnssLinearCombination l7PrefitOfBDS;
        gnssLinearCombination l8PrefitOfBDS;

        // GLONASS
        gnssLinearCombination l1PrefitOfGLO;
        gnssLinearCombination l2PrefitOfGLO;
        gnssLinearCombination l3PrefitOfGLO;
        gnssLinearCombination l4PrefitOfGLO;
        gnssLinearCombination l6PrefitOfGLO;

        /// QZSS phase
        gnssLinearCombination l1PrefitOfQZS;
        gnssLinearCombination l2PrefitOfQZS;
        gnssLinearCombination l5PrefitOfQZS;
        gnssLinearCombination l6PrefitOfQZS;

        /// Definition to compute prefit residual of GPS
        gnssLinearCombination pc12PrefitOfGPS;
        gnssLinearCombination lc12PrefitOfGPS;

        gnssLinearCombination pc15PrefitOfGPS;
        gnssLinearCombination lc15PrefitOfGPS;
        gnssLinearCombination pc125PrefitOfGPS;
        gnssLinearCombination lc125PrefitOfGPS;

        gnssLinearCombination pc21PrefitOfGPS;
        gnssLinearCombination lc21PrefitOfGPS;

        gnssLinearCombination pc25PrefitOfGPS;
        gnssLinearCombination lc25PrefitOfGPS;

        /// Definition to compute prefit residual of Galileo
        gnssLinearCombination pc51PrefitOfGAL;
        gnssLinearCombination lc51PrefitOfGAL;

        // added by ragnwang
        gnssLinearCombination pc15PrefitOfGAL;
        gnssLinearCombination lc15PrefitOfGAL;
        gnssLinearCombination pc17PrefitOfGAL;
        gnssLinearCombination lc17PrefitOfGAL;
        gnssLinearCombination pc18PrefitOfGAL;
        gnssLinearCombination lc18PrefitOfGAL;
        gnssLinearCombination pc16PrefitOfGAL;
        gnssLinearCombination lc16PrefitOfGAL;
        gnssLinearCombination pc157PrefitOfGAL;
        gnssLinearCombination lc157PrefitOfGAL;
        gnssLinearCombination pc1578PrefitOfGAL;
        gnssLinearCombination lc1578PrefitOfGAL;
        gnssLinearCombination pc15786PrefitOfGAL;
        gnssLinearCombination lc15786PrefitOfGAL;

        /// Definition to compute prefit residual of BDS
        gnssLinearCombination pc27PrefitOfBDS;
        gnssLinearCombination lc27PrefitOfBDS;

        gnssLinearCombination pc26PrefitOfBDS;
        gnssLinearCombination lc26PrefitOfBDS;

        // 267
        gnssLinearCombination pc267PrefitOfBDS;
        gnssLinearCombination lc267PrefitOfBDS;

        // 21
        gnssLinearCombination pc21PrefitOfBDS;
        gnssLinearCombination lc21PrefitOfBDS;
        // 61
        gnssLinearCombination pc61PrefitOfBDS;
        gnssLinearCombination lc61PrefitOfBDS;

        gnssLinearCombination pc25PrefitOfBDS;
        gnssLinearCombination lc25PrefitOfBDS;

        gnssLinearCombination pc28PrefitOfBDS;
        gnssLinearCombination lc28PrefitOfBDS;

        gnssLinearCombination pc261PrefitOfBDS;
        gnssLinearCombination lc261PrefitOfBDS;
        gnssLinearCombination pc2615PrefitOfBDS;
        gnssLinearCombination lc2615PrefitOfBDS;
        gnssLinearCombination pc26158PrefitOfBDS;
        gnssLinearCombination lc26158PrefitOfBDS;

        // bdsfopt
        gnssLinearCombination pc15PrefitOfBDS;
        gnssLinearCombination lc15PrefitOfBDS;

        gnssLinearCombination pc12PrefitOfBDS;
        gnssLinearCombination lc12PrefitOfBDS;

        gnssLinearCombination pc52PrefitOfBDS;
        gnssLinearCombination lc52PrefitOfBDS;

        gnssLinearCombination pc16PrefitOfBDS;
        gnssLinearCombination lc16PrefitOfBDS;

        gnssLinearCombination pc18PrefitOfBDS;
        gnssLinearCombination lc18PrefitOfBDS;

		gnssLinearCombination pc152PrefitOfBDS;
		gnssLinearCombination lc152PrefitOfBDS;
		gnssLinearCombination pc1526PrefitOfBDS;
		gnssLinearCombination lc1526PrefitOfBDS;
        gnssLinearCombination pc15268PrefitOfBDS;
        gnssLinearCombination lc15268PrefitOfBDS;

        gnssLinearCombination pc158PrefitOfBDS;
        gnssLinearCombination lc158PrefitOfBDS;
        
        /////// prefit ambiguity /////
        // GPS
        gnssLinearCombination BL1PrefitOfGPS;
        gnssLinearCombination BL2PrefitOfGPS;
        gnssLinearCombination BL5PrefitOfGPS;
        gnssLinearCombination BL12PrefitOfGPS;
        gnssLinearCombination BL15PrefitOfGPS;
        gnssLinearCombination BL125PrefitOfGPS;
        //GAL
		gnssLinearCombination BL1PrefitOfGAL;
        gnssLinearCombination BL5PrefitOfGAL;
        gnssLinearCombination BL7PrefitOfGAL;
        gnssLinearCombination BL8PrefitOfGAL;
        gnssLinearCombination BL6PrefitOfGAL;
		gnssLinearCombination BL15PrefitOfGAL;
		gnssLinearCombination BL17PrefitOfGAL;
		gnssLinearCombination BL18PrefitOfGAL;
		gnssLinearCombination BL16PrefitOfGAL;
		gnssLinearCombination BL157PrefitOfGAL;
		gnssLinearCombination BL1578PrefitOfGAL;
		gnssLinearCombination BL15786PrefitOfGAL;
        // bds
        gnssLinearCombination BL1PrefitOfBDS;
        gnssLinearCombination BL5PrefitOfBDS;
        gnssLinearCombination BL2PrefitOfBDS;
        gnssLinearCombination BL6PrefitOfBDS;
        gnssLinearCombination BL8PrefitOfBDS;
        gnssLinearCombination BL7PrefitOfBDS;
        gnssLinearCombination BL27PrefitOfBDS;
        gnssLinearCombination BL26PrefitOfBDS;
        gnssLinearCombination BL61PrefitOfBDS;
        gnssLinearCombination BL25PrefitOfBDS;
        gnssLinearCombination BL28PrefitOfBDS;
        gnssLinearCombination BL15PrefitOfBDS;
        gnssLinearCombination BL52PrefitOfBDS;
        gnssLinearCombination BL16PrefitOfBDS;
        gnssLinearCombination BL18PrefitOfBDS;
        gnssLinearCombination BL158PrefitOfBDS;
        gnssLinearCombination BL152PrefitOfBDS;
        gnssLinearCombination BL1526PrefitOfBDS;
        gnssLinearCombination BL15268PrefitOfBDS;
        gnssLinearCombination BL267PrefitOfBDS;
        gnssLinearCombination BL261PrefitOfBDS;
        gnssLinearCombination BL2615PrefitOfBDS;
        gnssLinearCombination BL26158PrefitOfBDS;
        /////// add amb and clk to compute the residuals /////////
        // GPS
        gnssLinearCombination c1ResOfGPS;
        gnssLinearCombination c2ResOfGPS;
        gnssLinearCombination c5ResOfGPS;

        gnssLinearCombination l1ResOfGPS;
        gnssLinearCombination l2ResOfGPS;
        gnssLinearCombination l5ResOfGPS;
        // IF
        gnssLinearCombination pc12ResOfGPS;
        gnssLinearCombination pc15ResOfGPS;
        gnssLinearCombination pc125ResOfGPS;
        gnssLinearCombination lc12ResOfGPS;
        gnssLinearCombination lc15ResOfGPS;
        gnssLinearCombination lc125ResOfGPS;

        // BDS
        gnssLinearCombination c1ResOfBDS;
        gnssLinearCombination c2ResOfBDS;
        gnssLinearCombination c6ResOfBDS;
        gnssLinearCombination c5ResOfBDS;
        gnssLinearCombination c8ResOfBDS;
        gnssLinearCombination c7ResOfBDS;

        gnssLinearCombination l1ResOfBDS;
        gnssLinearCombination l2ResOfBDS;
        gnssLinearCombination l5ResOfBDS;
        gnssLinearCombination l6ResOfBDS;
        gnssLinearCombination l7ResOfBDS;
        gnssLinearCombination l8ResOfBDS;
        // linear combination
        gnssLinearCombination pc15ResOfBDS;
        gnssLinearCombination pc52ResOfBDS;
        gnssLinearCombination pc16ResOfBDS;
        gnssLinearCombination pc18ResOfBDS;

        gnssLinearCombination lc15ResOfBDS;
        gnssLinearCombination lc52ResOfBDS;
        gnssLinearCombination lc16ResOfBDS;
        gnssLinearCombination lc18ResOfBDS;
        // CI
        gnssLinearCombination pc158ResOfBDS;
        gnssLinearCombination lc158ResOfBDS;
        // normal
        gnssLinearCombination pc26ResOfBDS;
        gnssLinearCombination pc61ResOfBDS;
        gnssLinearCombination pc25ResOfBDS;
        gnssLinearCombination pc28ResOfBDS;
        gnssLinearCombination pc27ResOfBDS;

        gnssLinearCombination lc26ResOfBDS;
        gnssLinearCombination lc61ResOfBDS;
        gnssLinearCombination lc25ResOfBDS;
        gnssLinearCombination lc28ResOfBDS;
        gnssLinearCombination lc27ResOfBDS;
        //IF one
        gnssLinearCombination pc152ResOfBDS;
        gnssLinearCombination lc152ResOfBDS;
        gnssLinearCombination pc1526ResOfBDS;
        gnssLinearCombination lc1526ResOfBDS;
        gnssLinearCombination pc15268ResOfBDS;
        gnssLinearCombination lc15268ResOfBDS;
        // normals
        gnssLinearCombination pc261ResOfBDS;
        gnssLinearCombination lc261ResOfBDS;
        gnssLinearCombination pc267ResOfBDS;
        gnssLinearCombination lc267ResOfBDS;
        gnssLinearCombination pc2615ResOfBDS;
        gnssLinearCombination lc2615ResOfBDS;
        gnssLinearCombination pc26158ResOfBDS;
        gnssLinearCombination lc26158ResOfBDS;

        // GAL
        gnssLinearCombination c1ResOfGAL;
        gnssLinearCombination c5ResOfGAL;
        gnssLinearCombination c7ResOfGAL;
        gnssLinearCombination c8ResOfGAL;
        gnssLinearCombination c6ResOfGAL;

        gnssLinearCombination l1ResOfGAL;
        gnssLinearCombination l5ResOfGAL;
        gnssLinearCombination l7ResOfGAL;
        gnssLinearCombination l8ResOfGAL;
        gnssLinearCombination l6ResOfGAL;
        // IF pair-wise combination
        gnssLinearCombination pc15ResOfGAL;
        gnssLinearCombination pc17ResOfGAL;
        gnssLinearCombination pc18ResOfGAL;
        gnssLinearCombination pc16ResOfGAL;
        gnssLinearCombination lc15ResOfGAL;
        gnssLinearCombination lc17ResOfGAL;
        gnssLinearCombination lc18ResOfGAL;
        gnssLinearCombination lc16ResOfGAL;
        // IF linear combination
        gnssLinearCombination pc157ResOfGAL;
        gnssLinearCombination pc1578ResOfGAL;
        gnssLinearCombination pc15786ResOfGAL;
        gnssLinearCombination lc157ResOfGAL;
        gnssLinearCombination lc1578ResOfGAL;
        gnssLinearCombination lc15786ResOfGAL;


    public:

        /// Return the frequency of the combination in cycles: i * L1 + j * L2
        static double freqOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

        /// Return the wavelength of the combination in cycles: i * L1 + j * L2
        static double wavelengthOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);
        /// Return the f1 factor of the combination in cycles: i * L1 + j * L2
        static double firstFactorOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);
        /// Return the f2 factor of the combination in cycles: i * L1 + j * L2
        static double secondFactorOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

    }; // End of class 'LinearCombinations'


    double wavelengthOfMW(const SatelliteSystem& sys, const TypeID& type);
    double varOfMW(const SatelliteSystem& sys, const TypeID& type);

    //@}

}

#endif // LINEARCOMBINATIONS_HPP
