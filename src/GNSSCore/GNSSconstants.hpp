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
 * @file GNSSconstants.hpp
 * Constants as defined in the GPS-ICD-200D and by all RINEX GNSSs
 */

#ifndef GPSTK_GNSS_CONSTANTS_HPP
#define GPSTK_GNSS_CONSTANTS_HPP

#include "Exception.hpp"
#include "DeprecatedConsts.hpp"
#include "MathBase.hpp"

namespace gpstk
{
      /**
       * @ingroup Geodetic
       * @name GNSS Constants
       * Time constants are in TimeConstants.hpp
       */
      //@{

       /** The maximum number of active satellites forGPS/GAL/GLO/BDS/QZSS constellation. */
    const long MAX_PRN_GPS = 32;
    const long MAX_PRN_GAL = 36;
    const long MAX_PRN_GLO = 32;
    const long MAX_PRN_BDS = 60;
    const long MAX_PRN_QZS = 7;

      // ---------------- independent of GNSS ----------------------
      /// GPS value of PI; also specified by GAL
   const double PI        = 3.141592653589793238462643383280;
      /// GPS value of PI*2
   const double TWO_PI    = 6.283185307179586476925286766559;
      /// GPS value of PI**0.5
   const double SQRT_PI   = 1.772453850905516027298167483341;
      /// relativity constant (sec/sqrt(m))
   const double REL_CONST = -4.442807633e-10;
   
      /// Conversion Factor from degrees to radians (units: degrees^-1)
   static const double DEG_TO_RAD = PI/180.0;
      /// Conversion Factor from radians to degrees (units: degrees)
   static const double RAD_TO_DEG = 180.0/PI;

   /// Conversion Factor from arcseconds to radians (unit: arcseconds^-1)
   static const double AS_TO_RAD = 4.848136811095359935899141e-6;
   /// Conversion Factor from radians to arcseconds (unit: arcseconds)
   static const double RAD_TO_AS = 206264.8062470963551564734;
   /// Conversion Factor from seconds of time to radians (unit: seconds^-1)
   static const double SEC_TO_RAD = 7.272205216643039903848712e-5;
   /// Conversion Factor from radians to seconds of time (unit: seconds)
   static const double RAD_TO_SEC = 43200.0 / PI;
   /// Arcseconds in a full circle
   static const double TURNAS = 1296000.0;
   //       /// Seconds per day
   static const double DAY_TO_SEC = 86400.0;
   //       /// Days per Julian Century
   static const double JC = 36525.0;
   //       /// Modified Julian Date of the J2000 Epoch
   static const double MJD_J2000 = 51544.5;
   //       /// Julian Date of Modified Julian Date zero
   static const double JD_TO_MJD = 2400000.5;
   //       /// Reference epoch (J2000.0), Julian Data
   static const double JD_J2000 = 2451545.0;
   // 
   // 
		// IERS 2010 Numerical Standards
		//-------------------------------------------------------------------------

    /// Constant of gravitation (m^3/kg/s^2)
   static const double G = 6.67428e-11;
   /// Astronomical unit (m)
   static const double AU = 1.49597870700e11;

   /// Solar radiation pressure at 1 AU (N/m^2)
   static const double P_SRP = 4.560e-6;

   /// Geocentric gravitational constant
   static const double GM_EARTH = 3.986004418e14;
   /// Equatorial radius of the Earth (m)
   static const double RE_EARTH = 6378136.6;
   /// Nominal mean angular velocity of the Earth (rad/s)
   static const double OMEGA_EARTH = 7.292115e-5;
   /// Flattening factor of the Earth
   static const double FLAT_EARTH = 1.0 / 298.25642;
   /// Mean equatorial gravity (m/s^2)
   static const double GE_EARTH = 9.7803278;
   /// Dynamical form factor of the Earth
   static const double J2_EARTH = 1.0826359e-3;

   /// Heliocentric gravitational constant
   static const double GM_SUN = 1.32712442099e20;
   /// Mean radius of the Sun (m)
   static const double RE_SUN = 6.95700e+8;

   /// Moon-Earth mass ratio
   static const double RATIO_ME = 0.0123000383;
   static const double GM_MOON = GM_EARTH * RATIO_ME;
   /// Mean radius of the Moon (m)
   static const double RE_MOON = 1738000.0;

   /// Sun-Mercury mass ratio
   static const double RATIO_SMe = 6.0236e6;
   static const double GM_MERCURY = GM_SUN / RATIO_SMe;

   /// Sun-Venus mass ratio
   static const double RATIO_SVe = 4.08523719e5;
   static const double GM_VENUS = GM_SUN / RATIO_SVe;

   /// Sun-Mars mass ratio
   static const double RATIO_SMa = 3.09870359e6;
   static const double GM_MARS = GM_SUN / RATIO_SMa;

   /// Sun-Jupiter mass ratio
   static const double RATIO_SJ = 1.047348644e3;
   static const double GM_JUPITER = GM_SUN / RATIO_SJ;

   /// Sun-Saturn mass ratio
   static const double RATIO_SSa = 3.4979018e3;
   static const double GM_SATURN = GM_SUN / RATIO_SSa;

   /// Sun-Uranus mass ratio
   static const double RATIO_SU = 2.290298e4;
   static const double GM_URANUS = GM_SUN / RATIO_SU;

   /// Sun-Neptune mass ratio
   static const double RATIO_SN = 1.941226e4;
   static const double GM_NEPTUNE = GM_SUN / RATIO_SN;

   /// Sun-Pluto mass ratio
   static const double RATIO_SP = 1.36566e8;
   static const double GM_PLUTO = GM_SUN / RATIO_SP;

   /// Ionospheric constant k
   const double C2_FACT = 40.3e+16;

      // ---------------- GPS --------------------------------------

      /// Hz, GPS Oscillator or chip frequency
   const double OSC_FREQ_GPS  = 10.23e6;
      /// Hz, GPS chip rate of the P & Y codes
   const double PY_CHIP_FREQ_GPS = OSC_FREQ_GPS;
      /// Hz, GPS chip rate of the C/A code
   const double CA_CHIP_FREQ_GPS = OSC_FREQ_GPS / 10.0;
      /// Hz, GPS Base freq w/o relativisitic effects
   const double RSVCLK_GPS    = 10.22999999543e6;
      /// GPS L1 frequency in units of oscillator frequency
   const double L1_MULT_GPS   = 154.0;
      /// GPS L2 frequency in units of oscillator frequency
   const double L2_MULT_GPS   = 120.0;
      /// GPS L5 frequency in units of oscillator frequency.
   const double L5_MULT_GPS   = 115.0;
      /// GPS Gamma constants
   const double GAMMA_GPS_12 = (L1_MULT_GPS/L2_MULT_GPS) * (L1_MULT_GPS/L2_MULT_GPS);
   //const double GAMMA_GPS_L1L2 = 1.646944444444444444;
   const double GAMMA_GPS_L1L2 = SQR(L1_FREQ_GPS)/SQR(L2_FREQ_GPS);

   const double GAMMA_GPS_15 = (L1_MULT_GPS/L5_MULT_GPS) * (L1_MULT_GPS/L5_MULT_GPS);
   //const double GAMMA_GPS_L1L5 = 1.793270321;
   const double GAMMA_GPS_L1L5 = SQR(L1_FREQ_GPS) / SQR(L5_FREQ_GPS);
   //const double GAMMA_GPS_L2L5 = 1.088846881;
   const double GAMMA_GPS_L2L5 = SQR(L2_FREQ_GPS) / SQR(L5_FREQ_GPS);
   const double GAMMA_GPS = 1.646944444;  // legacy notation and hard-coded value; wherefore the extra "44"??

   // addded by ragnwang
   const double BETA_GPS_L1L2 = 1.0 / (1.0 - GAMMA_GPS_L1L2);
   const double BETA_GPS_L1L5 = 1.0 / (1.0 - GAMMA_GPS_L1L5);
   const double APHA_GPS_L1L2 = -GAMMA_GPS_L1L2 / (1.0 - GAMMA_GPS_L1L2);
   const double APHA_GPS_L1L5 = -GAMMA_GPS_L1L5 / (1.0 - GAMMA_GPS_L1L5);
   const double BETA_GPS_L2L5 = 1.0 / (1.0 - GAMMA_GPS_L2L5);
   const double APHA_GPS_L2L5 = -GAMMA_GPS_L2L5 / (1.0 - GAMMA_GPS_L2L5);

      /// Reference Semi-major axis. From IS-GPS-800 Table 3.5-2 in meters.
   const double A_REF_GPS = 26559710.0;
      /// Omega reference value from Table 30-I converted to radians
   const double OMEGADOT_REF_GPS = -2.6e-9 * PI;
      /// Beta limit for GPS eclipse, unit: degrees
   const double BETA_GPS = 13.25;
   inline
   short getLegacyFitInterval(const short iodc, const short fiti)
   {
         /* check the IODC */
      if (iodc < 0 || iodc > 1023)
      {
            /* error in iodc, return minimum fit */
         return 4;
      }

      if ( ( ( (fiti == 0) && (iodc & 0xFF) < 240 )
             || (iodc & 0xFF) > 255 ) )
      {
            /* fit interval of 4 hours */
         return 4;
      }
      else if (fiti == 1)
      {
         if( ((iodc & 0xFF) < 240 || (iodc & 0xFF) > 255))
         {
               /* fit interval of 6 hours */
            return 6;
         }
         else if(iodc >=240 && iodc <=247)
         {
               /* fit interval of 8 hours */
            return 8;
         }
         else if( ( (iodc >= 248) && (iodc <= 255) ) || iodc == 496 )
         {
               /* fit interval of 14 hours */
            return 14;
         }
         else if((iodc >= 497 && iodc <=503) || (iodc >= 1021 && iodc <= 1023))
         {
               /* fit interval of 26 hours */
            return 26;
         }
         else if(iodc >= 504 && iodc <=510)
         {
               /* fit interval of 50 hours */
            return 50;
         }
         else if( iodc == 511 || ( (iodc >= 752) && (iodc <= 756) ) )
         {
               /* fit interval of 74 hours */
            return 74;
         }
         else if(iodc == 757)
         {
               /* fit interval of 98 hours */
            return 98;
         }
         else
         {
            InvalidRequest exc("Invalid IODC Value For sv Block");
            GPSTK_THROW(exc);
         }
      }
      else
      {
            /* error in ephemeris/iodc, return minimum fit */
         return 4;
      }

      return 0; // never reached
   }

      // ---------------- GLONASS ----------------------------------
      /// GLO Fundamental chip rate in Hz.
   const double OSC_FREQ_GLO  = 5.11e6;
      /// GLO Chip rate of the P & Y codes in Hz.
   const double PY_CHIP_FREQ_GLO = OSC_FREQ_GLO;
      /// GLO Chip rate of the C/A code in Hz.
   const double CA_CHIP_FREQ_GLO = OSC_FREQ_GLO / 10.0;

      /// GLO Fundamental oscillator freq in Hz.
   const double PSC_FREQ_GLO  = 5.00e6;
      /// GLO Base freq w/o relativisitic effects in Hz.
   const double RSVCLK_GLO    = 4.99999999782e6;

   /// GLONASS Gamma constant
   const double GAMMA_GLO = 81.0 / 49.0;

      /// GLO L1 multiplier.
   const double L1_MULT_GLO   = 320.4;
      /// GLO L2 multiplier.
   const double L2_MULT_GLO   = 249.2;

      /// Constant for the max array index in SV accuracy table.
   const int SV_ACCURACY_GLO_INDEX_MAX = 15;
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m.
      /// Further details in ICD-GLO-v5.0, Table 4.4 in Section 4.4.
   const double SV_ACCURACY_GLO_INDEX[] = { 1.0,  2.0,   2.5,   4.0,   5.0,
                                            7.0, 10.0,  12.0,  14.0,  16.0,
                                            32.0, 64.0, 128.0, 256.0, 512.0,
                                            9.999999999999e99               };

   // ---------------- GALILEO --------------------------------------
		/// GAL L5 frequency in units of oscillator frequency
   const double L5_MULT_GAL = 115.0;
   /// GAL L8 frequency in units of oscillator frequency
   const double L8_MULT_GAL = 116.5;
   /// GAL L7 frequency in units of oscillator frequency
   const double L7_MULT_GAL = 118.0;
   /// GAL L1 frequency in units of oscillator frequency
   const double L1_MULT_GAL = 154.0;

   /// GAL L6 frequency in units of oscillator frequency
   const double L6_MULT_GAL = 125.0;
   // 
		  /// Galileo Gamma constant, E1 and E5a
   const double GAMMA_GAL_L1L5 = SQR(L1_FREQ_GAL)/SQR(L5_FREQ_GAL);
   //       /// Galileo Gamma constant, E1 and E6
   const double GAMMA_GAL_L1L6 = SQR(L1_FREQ_GAL)/SQR(L6_FREQ_GAL);
   //       /// Galileo Gamma constant, E1 and E5b
   const double GAMMA_GAL_L1L7 = SQR(L1_FREQ_BDS)/SQR(L7_FREQ_GAL);
   //       /// Galileo Gamma constant, E1 and E5a+E5b
   const double GAMMA_GAL_L1L8 = SQR(L1_FREQ_GAL)/SQR(L8_FREQ_GAL);

   const double BETA_GAL_L1L5 = 1.0 / (1.0 - GAMMA_GAL_L1L5);
   const double BETA_GAL_L1L7 = 1.0 / (1.0 - GAMMA_GAL_L1L7);
   const double BETA_GAL_L1L8 = 1.0 / (1.0 - GAMMA_GAL_L1L8);
   const double BETA_GAL_L1L6 = 1.0 / (1.0 - GAMMA_GAL_L1L6);
   const double APHA_GAL_L1L5 = -GAMMA_GAL_L1L5 / (1.0 - GAMMA_GAL_L1L5);
   const double APHA_GAL_L1L7 = -GAMMA_GAL_L1L7 / (1.0 - GAMMA_GAL_L1L7);
   const double APHA_GAL_L1L8 = -GAMMA_GAL_L1L8 / (1.0 - GAMMA_GAL_L1L8);
   const double APHA_GAL_L1L6 = -GAMMA_GAL_L1L6 / (1.0 - GAMMA_GAL_L1L6);

   // ---------------- BeiDou ----------------------------------
      /// The maximum number of active satellites in the BDS constellation.
   const long MAX_PRN_COM     = 46;

   const double L2_MULT_BDS = 152.6;
   const double L7_MULT_BDS = 118.0;
   const double L6_MULT_BDS = 124.0;

   const double GAMMA_BDS_L1L2 = SQR(L1_FREQ_BDS) / SQR(L2_FREQ_BDS);   //1.018432791852536;
   const double GAMMA_BDS_L5L2 = SQR(L5_FREQ_BDS) / SQR(L2_FREQ_BDS);   //1.793270321361056;
   const double GAMMA_BDS_L1L5 = SQR(L1_FREQ_BDS) / SQR(L5_FREQ_BDS);   //1.793270321361056;
   const double GAMMA_BDS_L1L7 = SQR(L1_FREQ_BDS) / SQR(L7_FREQ_BDS);   //1.703246193622520;
   const double GAMMA_BDS_L1L8 = SQR(L1_FREQ_BDS) / SQR(L8_FREQ_BDS);   //1.747388973825267;
   const double GAMMA_BDS_L1L6 = SQR(L1_FREQ_BDS) / SQR(L6_FREQ_BDS);   //1.542403746097812;
   const double GAMMA_BDS_L2L1 = SQR(L2_FREQ_BDS) / SQR(L1_FREQ_BDS);   //0.981900826446281;
   const double GAMMA_BDS_L6L1 = SQR(L6_FREQ_BDS) / SQR(L1_FREQ_BDS);   //0.981900826446281;
   const double GAMMA_BDS_L2L5 = SQR(L2_FREQ_BDS) / SQR(L5_FREQ_BDS);   //1.760813610586011;
   const double GAMMA_BDS_L2L8 = SQR(L2_FREQ_BDS) / SQR(L8_FREQ_BDS);   //1.715762677522150;
   const double GAMMA_BDS_L2L7 = SQR(L2_FREQ_BDS) / SQR(L7_FREQ_BDS);   //1.67241884515943;
   const double GAMMA_BDS_L2L6 = SQR(L2_FREQ_BDS) / SQR(L6_FREQ_BDS);   //1.51448751300728;


   const double BETA_BDS_L2L6 = 1.0 / (1.0 - GAMMA_BDS_L2L6);
   const double BETA_BDS_L2L1 = 1.0 / (1.0 - GAMMA_BDS_L2L1);
   const double BETA_BDS_L2L5 = 1.0 / (1.0 - GAMMA_BDS_L2L5);
   const double BETA_BDS_L2L8 = 1.0 / (1.0 - GAMMA_BDS_L2L8);
   const double BETA_BDS_L2L7 = 1.0 / (1.0 - GAMMA_BDS_L2L7);
   const double APHA_BDS_L2L6 = -GAMMA_BDS_L2L6 / (1.0 - GAMMA_BDS_L2L6);
   const double APHA_BDS_L2L1 = -GAMMA_BDS_L2L1 / (1.0 - GAMMA_BDS_L2L1);
   const double APHA_BDS_L2L5 = -GAMMA_BDS_L2L5 / (1.0 - GAMMA_BDS_L2L5);
   const double APHA_BDS_L2L8 = -GAMMA_BDS_L2L8 / (1.0 - GAMMA_BDS_L2L8);
   const double APHA_BDS_L2L7 = -GAMMA_BDS_L2L7 / (1.0 - GAMMA_BDS_L2L7);
   const double BETA_BDS_L1L5 = 1.0 / (1.0 - GAMMA_BDS_L1L5);
   const double BETA_BDS_L1L2 = 1.0 / (1.0 - GAMMA_BDS_L1L2);
   const double BETA_BDS_L1L6 = 1.0 / (1.0 - GAMMA_BDS_L1L6);
   const double BETA_BDS_L1L8 = 1.0 / (1.0 - GAMMA_BDS_L1L8);
   const double APHA_BDS_L1L5 = -GAMMA_BDS_L1L5 / (1.0 - GAMMA_BDS_L1L5);
   const double APHA_BDS_L1L2 = -GAMMA_BDS_L1L2 / (1.0 - GAMMA_BDS_L1L2);
   const double APHA_BDS_L1L6 = -GAMMA_BDS_L1L6 / (1.0 - GAMMA_BDS_L1L6);
   const double APHA_BDS_L1L8 = -GAMMA_BDS_L1L8 / (1.0 - GAMMA_BDS_L1L8);

      // ---------------- QZSS ----------------------------------
      // PRN range defined in QZSS ICD 5.1.1.11.1
      // PRN 198-202 reserved for maint./test, not to be used 
      // by users. 
      // These need to be defined in order to differentiate
      // QZSS from GPS in cases where QZSS is broadcasting 
      // GPS-similar signals and navigation messages.
      // Defined as int to be compatible with gpstk::SatID.id member.
   const int MIN_ICD_PRN_QZS = 193;
   const int MAX_ICD_PRN_QZS = 202;

      //@}
   
} // namespace

// this is at the end because it needs some of the constants defined above
//#include "FreqConsts.hpp"

#endif //GPSTK_GNSS_CONSTANTS_HPP
