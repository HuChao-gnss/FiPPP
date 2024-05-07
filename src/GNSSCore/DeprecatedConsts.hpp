//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file DeprecatedConsts.hpp
 * Constants that have been removed from GNSSconstants.hpp for
 * deprecation in favor of those in the generated file FreqConsts.hpp
 */

#ifndef GPSTK_DEPRECATEDCONSTS_HPP
#define GPSTK_DEPRECATEDCONSTS_HPP

#include "SatelliteSystem.hpp"

namespace gpstk
{
	/// m/s, speed of light; this value defined by GPS but applies to GAL and GLO.
	const double C_MPS = 2.99792458e8;
    
    // ---------------- GPS ----------------------------------
      /// GPS L1 carrier frequency in Hz
   const double L1_FREQ_GPS   = 1575.42e6;
      /// GPS L2 carrier frequency in Hz
   const double L2_FREQ_GPS   = 1227.60e6;
      /// GPS L5 carrier frequency in Hz.
   const double L5_FREQ_GPS   = 1176.45e6;
      /// GPS L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GPS  = C_MPS/L1_FREQ_GPS;
      /// GPS L2 carrier wavelength in meters
   const double L2_WAVELENGTH_GPS  = C_MPS / L2_FREQ_GPS;
      /// GPS L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GPS  = C_MPS / L5_FREQ_GPS;

   // =   (f1^2*lambda1 * N1 - f2^2*lambda2 *N2)/(f1^2-f2^2)
   // =   (f1*N1 - f2*N2) * clight/(f1^2-f2^2)
   // =   (f1*N1 - f2*N1 + f2*N1 -f2*N2) * clight/(f1^2-f2^2)
   // = ( (f1-f2)*N1 + f2*Nw ) * clight/(f1^2-f2^2)
   // =   clight/(f1+f2)*N1 + f2*clight/(f1^2-f2^2)*Nw
   // =   lambda1*f1/(f1+f2)*N1 + lambda2*f2*f2/(f1^2-f2^2)*Nw
   // =   lambda1*f1/(f1+f2)*N1 + lambda1*f1*f2/(f1^2-f2^2)*Nw
   // 
   // lambLCN1 = lambda1*f1/(f1+f2)
   // lambLCNw = lambda2*f2^2/(f1^2-f2^2)

  /// GPS LC (L1+L2) carrier wavelength in meters
   const double LC12_WAVELENGTH_GPS = C_MPS / (L1_FREQ_GPS + L2_FREQ_GPS);
   /// GPS LC (L1+L5) carrier wavelength in meters
   const double LC15_WAVELENGTH_GPS = C_MPS / (L1_FREQ_GPS + L5_FREQ_GPS);
   const double LC25_WAVELENGTH_GPS = C_MPS / (L2_FREQ_GPS + L5_FREQ_GPS);

   /// GPS WL (L1+L2) carrier wavelength in meters
   const double WL12_WAVELENGTH_GPS = C_MPS / (L1_FREQ_GPS - L2_FREQ_GPS);
   /// GPS WL (L1+L5) carrier wavelength in meters
   const double WL15_WAVELENGTH_GPS = C_MPS / (L1_FREQ_GPS - L5_FREQ_GPS);
   const double WL25_WAVELENGTH_GPS = C_MPS / (L1_FREQ_GPS - L5_FREQ_GPS);
   

   // ---------------- GLO ----------------------------------
      // GLO Frequency(Hz)
      //   f1 is 1602.0e6 + n*562.5e3 Hz = 9 * (178 + n*0.0625) MHz
      //   f2    1246.0e6 + n*437.5e3 Hz = 7 * (178 + n*0.0625) MHz
      // where n is the time- and satellite-dependent 'frequency channel'
      // -7 <= n <= 7
      /// GLO L1 carrier base frequency in Hz.
   const double L1_FREQ_GLO       = 1602.0e6;
      /// GLO L1 carrier frequency step size in Hz.
   const double L1_FREQ_STEP_GLO  = 562.5e3;
      /// GLO L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_GLO = 0.187136365793;
      /// GLO L2 carrier base frequency in Hz.
   const double L2_FREQ_GLO       = 1246.0e6;
      /// GLO L2 carrier frequency step size in Hz.
   const double L2_FREQ_STEP_GLO  = 437.5e3;
      /// GLO L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_GLO = 0.240603898876;
      /// GLO L3 carrier frequency in Hz.
   const double L3_FREQ_GLO       = 1202.025e6;
      /// GLO L3 carrier wavelength in meters.
   const double L3_WAVELENGTH_GLO = 0.249406175412;
      /// GLO L4
   const double L4_FREQ_GLO = 1600.995e6;
   const double L4_WAVELENGTH_GLO = C_MPS / L4_FREQ_GLO;
      /// GLO L6
   const double L6_FREQ_GLO = 1248.06e6;
   const double L6_WAVELENGTH_GLO = C_MPS / L6_FREQ_GLO;

      // ---------------- Galileo ----------------------------------
      /// GAL L1 (E1) carrier frequency in Hz
   const double L1_FREQ_GAL   = L1_FREQ_GPS;
      /// GAL L5 (E5a) carrier frequency in Hz.
   const double L5_FREQ_GAL   = L5_FREQ_GPS;
      /// GAL L6 (E6) carrier frequency in Hz.
   const double L6_FREQ_GAL   = 1278.75e6;
      /// GAL L7 (E5b) carrier frequency in Hz.
   const double L7_FREQ_GAL   = 1207.140e6;
      /// GAL L8 (E5a+E5b) carrier frequency in Hz.
   const double L8_FREQ_GAL   = 1191.795e6;

      /// GAL L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GAL  = C_MPS / L1_FREQ_GAL; //L1_WAVELENGTH_GPS;
      /// GAL L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GAL = C_MPS / L5_FREQ_GAL;  // L5_WAVELENGTH_GPS;
      /// GAL L6 carrier wavelength in meters.
   const double L6_WAVELENGTH_GAL = C_MPS / L6_FREQ_GAL;  // 0.234441805;
      /// GAL L7 carrier wavelength in meters.
   const double L7_WAVELENGTH_GAL = C_MPS / L7_FREQ_GAL;  // 0.24834937;
      /// GAL L8 carrier wavelength in meters.
   const double L8_WAVELENGTH_GAL = C_MPS / L8_FREQ_GAL;  //0.251547001;

   /// GAL LC (L1+L5) carrier wavelength in meters
   const double LC15_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL + L5_FREQ_GAL);
   const double LC17_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL + L7_FREQ_GAL);
   const double LC18_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL + L8_FREQ_GAL);
   const double LC16_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL + L6_FREQ_GAL);

   /// GAL WL (L1+L5) carrier wavelength in meters
   const double WL15_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL - L5_FREQ_GAL);
   const double WL17_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL - L7_FREQ_GAL);
   const double WL18_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL - L8_FREQ_GAL);
   const double WL16_WAVELENGTH_GAL = C_MPS / (L1_FREQ_GAL - L6_FREQ_GAL);

   const double WL57_WAVELENGTH_GAL = -9.768408536982731;
   const double WL58_WAVELENGTH_GAL = -19.536817073965461;
   const double WL56_WAVELENGTH_GAL = -2.930522561094819;

   const double WL78_WAVELENGTH_GAL = 19.536817073965461;
   const double WL76_WAVELENGTH_GAL = -4.186460801564027;

   const double WL86_WAVELENGTH_GAL = -3.447673601288023;

      // ---------------- Geostationary (SBAS) ---------------------
      /// GEO L1 carrier frequency in Hz
   const double L1_FREQ_GEO   = L1_FREQ_GPS;
      /// GEO L5 carrier frequency in Hz.
   const double L5_FREQ_GEO   = L5_FREQ_GPS;

      /// GEO L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GEO  = L1_WAVELENGTH_GPS;
      /// GEO L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GEO  = L5_WAVELENGTH_GPS;

      // ---------------- BeiDou ----------------------------------
   const double L3_FREQ_BDS   = 1268.52e6;  // B3I (BDS2/3)

      /// BDS L1 carrier wavelength in meters.
   //const double L1_WAVELENGTH_BDS  = 0.192039486310276;
      /// BDS L2 carrier wavelength in meters.
   //const double L2_WAVELENGTH_BDS  = C_MPS / L2_FREQ_BDS;
      /// BDS L3 carrier wavelength in meters.
   const double L3_WAVELENGTH_BDS  = 0.236332464604421;

     // BDS-3 L1/L2/L5/L7/L6
    // BDS-2 L2/L7/L6
   // BDS: B1C/B1I/B3I/B2a/B2/B2b(B2I)/B3I
   //       1   2   3   5  8   7        6
   const double L1_FREQ_BDS = 1575.420e6; // B1C (BDS-3)
   const double L2_FREQ_BDS = 1561.098e6; // B1I (BDS-3/BDS-2)
   const double L5_FREQ_BDS = 1176.450e6; // B2a (BDS-3)
   const double L8_FREQ_BDS = 1191.795e6; // B2=B2a+B2b/2
   const double L7_FREQ_BDS = 1207.140e6; // B2b/B2I (BDS-3/BDS-2)
   const double L6_FREQ_BDS = 1268.520e6; // B3  (BDS-3/BDS-2)
   const double L1_WAVELENGTH_BDS = C_MPS / L1_FREQ_BDS;
   const double L2_WAVELENGTH_BDS = C_MPS / L2_FREQ_BDS;
   const double L5_WAVELENGTH_BDS = C_MPS / L5_FREQ_BDS;
   const double L7_WAVELENGTH_BDS = C_MPS / L7_FREQ_BDS;
   const double L8_WAVELENGTH_BDS = C_MPS / L8_FREQ_BDS;
   const double L6_WAVELENGTH_BDS = C_MPS / L6_FREQ_BDS;

   /// BDS LC (L2+L7) carrier wavelength in meters.
   const double LC27_WAVELENGTH_BDS = C_MPS / (L2_FREQ_BDS + L7_FREQ_BDS);
   const double LC26_WAVELENGTH_BDS = C_MPS / (L2_FREQ_BDS + L6_FREQ_BDS);

   const double WL27_WAVELENGTH_BDS = C_MPS / (L2_FREQ_BDS - L7_FREQ_BDS);
   const double WL26_WAVELENGTH_BDS = C_MPS / (L2_FREQ_BDS - L6_FREQ_BDS);

      // ---------------- QZSS ----------------------------------
      /// QZS L1 carrier frequency in Hz.
   const double L1_FREQ_QZS   = L1_FREQ_GPS;
      /// QZS L2 carrier frequency in Hz.
   const double L2_FREQ_QZS   = L2_FREQ_GPS;
      /// QZS L5 carrier frequency in Hz.
   const double L5_FREQ_QZS   = L5_FREQ_GPS;
      /// QZS LEX(6) carrier frequency in Hz.
   const double L6_FREQ_QZS   = L6_FREQ_GAL;

      /// QZS L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_QZS   = L1_WAVELENGTH_GPS;
      /// QZS L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_QZS   = L2_WAVELENGTH_GPS;
      /// QZS L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_QZS   = L5_WAVELENGTH_GPS;
      /// QZS LEX(6) carrier wavelength in meters.
   const double L6_WAVELENGTH_QZS   = L6_WAVELENGTH_GAL;

     // ---------------- IRNSS ----------------------------------
   const double L5_FREQ_IRNSS = L5_FREQ_GPS;
   const double L9_FREQ_IRNSS = 2492.028e6;
   const double L5_WAVELENGTH_IRNSS = L5_WAVELENGTH_GPS;
   const double L9_WAVELENGTH_IRNSS = C_MPS / L9_FREQ_IRNSS;


   //     // ---------------- Convenience routines ---------------------
//     /// Compute wavelength for the given satellite system (sat.id is ignored) at the
//     /// given RINEX frequency band n(=1,2,5,6,7,8). Return 0 if the frequency n is
//     /// not valid for the system.
//     /// Calls for system GLO must include the frequency channel number N (-7<=N<=7).
   inline double getFreq(const SatelliteSystem& sys, const int& n, const int N = 0)
	   throw()
   {
	   switch (sys) {
	   case SatelliteSystem::GPS:
		   if (n == 1) return L1_FREQ_GPS;
		   else if (n == 2) return L2_FREQ_GPS;
		   else if (n == 5) return L5_FREQ_GPS;
		   break;
	   case SatelliteSystem::Glonass:
		   if (n == 1) return ((L1_FREQ_GLO + N * L1_FREQ_STEP_GLO));
		   else if (n == 2) return ((L2_FREQ_GLO + N * L2_FREQ_STEP_GLO));
		   else if (n == 3) return L3_FREQ_GLO; // CDMA signal
		   else if (n == 4) return L4_FREQ_GLO; // CDMA signal
		   else if (n == 6) return L6_FREQ_GLO; // CDMA signal
		   break;
	   case SatelliteSystem::Galileo:
		   if (n == 1) return L1_FREQ_GAL;
		   else if (n == 5) return L5_FREQ_GAL;
		   else if (n == 6) return L6_FREQ_GAL;
		   else if (n == 7) return L7_FREQ_GAL;
		   else if (n == 8) return L8_FREQ_GAL;
		   break;
	   case SatelliteSystem::SBAS:
		   if (n == 1) return L1_FREQ_GEO;
		   else if (n == 5) return L5_FREQ_GEO;
		   break;
	   case SatelliteSystem::BeiDou:
		   if (n == 1) return L1_FREQ_BDS;      //B1C
		   else if (n == 2) return L2_FREQ_BDS; //B1I
		   else if (n == 5) return L5_FREQ_BDS; //B2a
		   else if (n == 7) return L7_FREQ_BDS; //B2b(B2I)
		   else if (n == 8) return L8_FREQ_BDS; //B2
		   else if (n == 6) return L6_FREQ_BDS; //B3I
		   break;
	   case SatelliteSystem::QZSS:
		   if (n == 1) return L1_FREQ_QZS;
		   else if (n == 2) return L2_FREQ_QZS;
		   else if (n == 5) return L5_FREQ_QZS;
		   else if (n == 6) return L6_FREQ_QZS;
		   break;
	   case SatelliteSystem::IRNSS:
		   if (n == 5) return L5_FREQ_IRNSS;
		   else if (n == 9) return L9_FREQ_IRNSS;
		   break;
	   }

	   return 0.0;
   }
}

#endif // GPSTK_DEPRECATEDCONSTS_HPP
