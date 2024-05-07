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


#ifndef SAAS_TROP_MODEL_HPP
#define SAAS_TROP_MODEL_HPP

#include "TropModel.hpp"
#include "MJD.hpp"
namespace gpstk
{
      /** Saastamoinen tropospheric model based on Saastamoinen, J.,
       * 'Atmospheric Correction for the Troposphere and Stratosphere
       * in Radio Ranging of Satellites,' Geophysical Monograph 15,
       * American Geophysical Union, 1972, and Ch 9 of McCarthy, D and
       * Petit, G, IERS Conventions (2003), IERS Technical Note 32,
       * IERS, 2004. The mapping functions are from Neill, A.E., 1996,
       * 'Global Mapping Functions for the Atmosphere Delay of Radio
       * Wavelengths,' J. Geophys. Res., 101, pp. 3227-3246 (also see
       * IERS TN 32).
       *
       * This model includes a wet and dry component, and requires
       * input of the geodetic latitude, day of year and height above
       * the ellipsoid of the receiver.
       *
       * Usually, the caller will set the latitude and day of year at the same
       * time the weather is set
       *   SaasTropModel stm;
       *   stm.setReceiverLatitude(lat);
       *   stm.setDayOfYear(doy);
       *   stm.setWeather(T,P,H);
       * Then, when the correction (and/or delay and map) is computed,
       * receiver height should be set before the call to
       * correction(elevation):
       *   stm.setReceiverHeight(height);
       *   trop_corr = stm.correction(elevation);
       *
       * @note in this model, units of 'temp' are degrees Celsius and
       * humid actually stores water vapor partial pressure in mbars
       */
   class SaasTropModel : public TropModel
   {
   public:
         /// Empty constructor
      SaasTropModel(void);

         /** Create a trop model using the minimum information:
          * latitude and doy.
          * @param lat Latitude of the receiver in degrees.
          * @param day Day of year.
          */
      SaasTropModel(const double& lat,
                    const int& day);

         /** Create a trop model with weather.
          * @param lat Latitude of the receiver in degrees.
          * @param day Day of year.
          * @param wx the weather to use for this correction.
          * @throw InvalidParameter
          */
      SaasTropModel(const double& lat,
                    const int& day,
                    const WxObservation& wx);

         /** Create a tropospheric model from explicit weather data
          * @param lat Latitude of the receiver in degrees.
          * @param day Day of year.
          * @param T temperature in degrees Celsius
          * @param P atmospheric pressure in millibars
          * @param H relative humidity in percent
          * @throw InvalidParameter
          */
      SaasTropModel(const double& lat,
                    const int& day,
                    const double& T,
                    const double& P,
                    const double& H);

         /// Return the name of the model
      virtual std::string name(void)
      { return std::string("Saas"); }

         /** Compute and return the full tropospheric delay
          * @param elevation Elevation of satellite as seen at
          *   receiver, in degrees
          * @throw InvalidTropModel
          */
      virtual double correction(double elevation) const;
         /**
          * Compute and return the full tropospheric delay, given the
          * positions of receiver and satellite and the time tag. This
          * version is most useful within positioning algorithms,
          * where the receiver position and timetag may vary; it
          * computes the elevation (and other receiver location
          * information) and passes them to appropriate set...()
          * routines and the correction(elevation) routine.
          * @param RX  Receiver position
          * @param SV  Satellite position
          * @param tt  Time tag of the signal
          * @throw InvalidTropModel
          */
      virtual double correction(const Position& RX,
                                const Position& SV,
                                const CommonTime& tt);

         /** \deprecated
          * Compute and return the full tropospheric delay, given the
          * positions of receiver and satellite and the time tag. This
          * version is most useful within positioning algorithms,
          * where the receiver position and timetag may vary; it
          * computes the elevation (and other receiver location
          * information) and passes them to appropriate set...()
          * routines and the correction(elevation) routine.
          * @param RX  Receiver position in ECEF cartesian coordinates (meters)
          * @param SV  Satellite position in ECEF cartesian coordinates (meters)
          * @param tt  Time tag of the signal
          * @throw InvalidTropModel
          */
      virtual double correction(const Xvt& RX,
                                const Xvt& SV,
                                const CommonTime& tt);

         /** Compute and return the zenith delay for dry component
          * of the troposphere
          * @throw InvalidTropModel
          */
      virtual double dry_zenith_delay(void) const;

         /** Compute and return the zenith delay for wet component
          * of the troposphere
          * @throw InvalidTropModel
          */
      virtual double wet_zenith_delay(void) const;

         /** Compute and return the mapping function for dry component of
          * the troposphere.
          * @note this function will return infinity at zero elevation.
          * @param elevation Elevation of satellite as seen at
          *   receiver, in degrees
          * @throw InvalidTropModel
          */
      virtual double dry_mapping_function(double elevation) const;

         /** Compute and return the mapping function for wet component of
          * the troposphere.
          * @param elevation Elevation of satellite as seen at
          *   receiver, in degrees
          * @throw InvalidTropModel
          */
      virtual double wet_mapping_function(double elevation) const;

         /** Re-define the tropospheric model with explicit weather data.
          * Typically called just before correction().
          * @param wx the weather to use for this correction
          * @throw InvalidParameter
          */
      virtual void setWeather(const WxObservation& wx);

         /** Define the weather data; typically called just before correction().
          * @param T temperature in degrees Celsius
          * @param P atmospheric pressure in millibars
          * @param H relative humidity in percent
          * @throw InvalidParameter
          */
      virtual void setWeather(const double& T,
                              const double& P,
                              const double& H);

         /** Define the receiver height; this required before calling
          * correction() or any of the zenith_delay or
          * mapping_function routines.
          * @param ht Height of the receiver in meters.
          */
      void setReceiverHeight(const double& ht);

         /** Define the latitude of the receiver; this is required
          * before calling correction() or any of the zenith_delay or
          * mapping_function routines.
          * @param lat Latitude of the receiver in degrees.
          */
      void setReceiverLatitude(const double& lat);

         /** Define the day of year; this is required before calling
          * correction() or any of the zenith_delay or
          * mapping_function routines.
          * @param d Day of year.
          */
      void setDayOfYear(const int& d);

	  void setAllParameters(const CommonTime& time, const Position& rxPos, int flag=1)
	  {
          doy = static_cast<YDSTime>(time).doy;
		  double mjd = static_cast<MJD>(time).mjd;
          //doy = (TWO_PI * (mjd - 44266.0) / 365.25);       //mjd -44239 + 1 - 28
          //doy = static_cast<YDSTime>(time).doy;
          height = rxPos.getHeight() < 0.0 ? 0.0 : rxPos.getHeight();
          latitude = rxPos.getGeodeticLatitude();
          longitude = rxPos.getLongitude();
		  /*press = 1013.25 * ::pow(1.0 - 2.2557e-5 * height, 5.2568);
		  temp = 15.0-6.5e-3*height;
		  humid = 0.0;*/
		  if (!flag)
		  {
			  press = 1013.25 * ::pow(1.0 - 2.2557E-5 * height, 5.2568);
			  temp = 15.0 - 6.5e-3 * height;
			  humid = 0.7;
		  }
		  else
		  {
			  getGPT(rxPos, mjd, press, temp);
			  humid = 0.;
		  }
          setWeather(temp, press, humid);
          setReceiverHeight(height);
          setReceiverLatitude(latitude);
          setReceiverLongitude(longitude);
          setDayOfYear(doy);
	  }
      // added by ragnwang
      void getGPT(const Position& rxPos, const double mjd, double& pressure, double& temper);

   private:
      double longitude;
      double height;             ///< height (m) of the receiver above the geoid
      double latitude;           ///< latitude (deg) of receiver
      int doy;                   ///< day of year
      bool validWeather;
      bool validRxLatitude;
      bool validRxHeight;
      bool validDOY;
   };

}
#endif
