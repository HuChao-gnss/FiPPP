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


#include "YDSTime.hpp"
#include "SaasTropModel.hpp"

#define THROW_IF_INVALID_DETAILED() {if (!valid) {\
         InvalidTropModel e;                                            \
         if(!validWeather)    e.addText("Invalid trop model: Weather"); \
         if(!validRxHeight)   e.addText("Invalid trop model: Rx Height"); \
         if(!validRxLatitude) e.addText("Invalid trop model: Rx Latitude"); \
         if(!validDOY)        e.addText("Invalid trop model: day of year"); \
         GPSTK_THROW(e);}}

namespace gpstk
{
    // Saastamoinen tropospheric model.
    // This model needs work; it is not the Saastamoinen model, but appears to be
    // a combination of the Neill mapping functions and an unknown delay model.
    // Based on Saastamoinen, J., 'Atmospheric
    // Correction for the Troposphere and Stratosphere in Radio Ranging of
    // Satellites,' Geophysical Monograph 15, American Geophysical Union, 1972,
    // and Ch. 9 of McCarthy, D. and Petit, G., IERS Conventions (2003), IERS
    // Technical Note 32, IERS, 2004. The mapping functions are from
    // Neill, A.E., 1996, 'Global Mapping Functions for the Atmosphere Delay of
    // Radio Wavelengths,' J. Geophys. Res., 101, pp. 3227-3246 (also see IERS TN 32).
    //
    // This model includes a wet and dry component, and requires input of the
    // geodetic latitude, day of year and height above the ellipsoid of the receiver.
    //
    // Usually, the caller will set the latitude and day of year at the same
    // time the weather is set
    //   SaasTropModel stm;
    //   stm.setReceiverLatitude(lat);
    //   stm.setDayOfYear(doy);
    //   stm.setWeather(T,P,H);
    // Then, when the correction (and/or delay and map) is computed, receiver height
    // should be set before the call to correction(elevation):
    //   stm.setReceiverHeight(height);
    //   trop_corr = stm.correction(elevation);
    //
    // NB in this model, units of 'temp' are degrees Celcius and
    // humid actually stores water vapor partial pressure in mbars
    //

    // constants for wet mapping function
    static const double SaasWetA[5] =
    { 0.00058021897, 0.00056794847, 0.00058118019, 0.00059727542, 0.00061641693 };
    static const double SaasWetB[5] =
    { 0.0014275268, 0.0015138625, 0.0014572752, 0.0015007428, 0.0017599082 };
    static const double SaasWetC[5] =
    { 0.043472961, 0.046729510, 0.043908931, 0.044626982, 0.054736038 };

    // constants for dry mapping function
    static const double SaasDryA[5] =
    { 0.0012769934, 0.0012683230, 0.0012465397, 0.0012196049, 0.0012045996 };
    static const double SaasDryB[5] =
    { 0.0029153695, 0.0029152299, 0.0029288445, 0.0029022565, 0.0029024912 };
    static const double SaasDryC[5] =
    { 0.062610505, 0.062837393, 0.063721774, 0.063824265, 0.064258455 };

    static const double SaasDryA1[5] =
    { 0.0, 0.000012709626, 0.000026523662, 0.000034000452, 0.000041202191 };
    static const double SaasDryB1[5] =
    { 0.0, 0.000021414979, 0.000030160779, 0.000072562722, 0.00011723375 };
    static const double SaasDryC1[5] =
    { 0.0, 0.000090128400, 0.000043497037, 0.00084795348, 0.0017037206 };

    // Default constructor
    SaasTropModel::SaasTropModel(void)
    {
        validWeather = false;
        validRxLatitude = false;
        validDOY = false;
        validRxHeight = false;
    } // end SaasTropModel::SaasTropModel()

       // Create a trop model using the minimum information: latitude and doy.
       // Interpolate the weather unless setWeather (optional) is called.
       // @param lat Latitude of the receiver in degrees.
       // @param day Day of year.
    SaasTropModel::SaasTropModel(const double& lat,
        const int& day)
    {
        validWeather = false;
        validRxHeight = false;
        SaasTropModel::setReceiverLatitude(lat);
        SaasTropModel::setDayOfYear(day);
    } // end SaasTropModel::SaasTropModel

       // Create a trop model with weather.
       // @param lat Latitude of the receiver in degrees.
       // @param day Day of year.
       // @param wx the weather to use for this correction.
    SaasTropModel::SaasTropModel(const double& lat,
        const int& day,
        const WxObservation& wx)
    {
        validRxHeight = false;
        SaasTropModel::setReceiverLatitude(lat);
        SaasTropModel::setDayOfYear(day);
        SaasTropModel::setWeather(wx);
    }  // end SaasTropModel::SaasTropModel(weather)

       // Create a tropospheric model from explicit weather data
       // @param lat Latitude of the receiver in degrees.
       // @param day Day of year.
       // @param T temperature in degrees Celsius
       // @param P atmospheric pressure in millibars
       // @param H relative humidity in percent
    SaasTropModel::SaasTropModel(const double& lat,
        const int& day,
        const double& T,
        const double& P,
        const double& H)
    {
        validRxHeight = false;
        SaasTropModel::setReceiverLatitude(lat);
        SaasTropModel::setDayOfYear(day);
        SaasTropModel::setWeather(T, P, H);
    } // end SaasTropModel::SaasTropModel()

       // re-define this to get the throws correct
    double SaasTropModel::correction(double elevation) const
    {
        if (!valid) {
            if (!validWeather) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: weather"));
            if (!validRxLatitude) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: Rx Latitude"));
            if (!validRxHeight) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: Rx Height"));
            if (!validDOY) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: day of year"));
            GPSTK_THROW(
                InvalidTropModel("Valid flag corrupted in Saastamoinen trop model"));
        }

        if (elevation < 0.0) return 0.0;

        double corr = 0.0;
        try {
            corr = (dry_zenith_delay() * dry_mapping_function(elevation)
                + wet_zenith_delay() * wet_mapping_function(elevation));
        }
        catch (Exception& e) { GPSTK_RETHROW(e); }

        return corr;

    }  // end SaasTropModel::correction(elevation)

       // Compute and return the full tropospheric delay, given the positions of
       // receiver and satellite and the time tag. This version is most useful
       // within positioning algorithms, where the receiver position and timetag
       // may vary; it computes the elevation (and other receiver location
       // information) and passes them to appropriate set...() routines
       // and the correction(elevation) routine.
       // @param RX  Receiver position
       // @param SV  Satellite position
       // @param tt  Time tag of the signal
    double SaasTropModel::correction(const Position& RX,
        const Position& SV,
        const CommonTime& tt)
    {
        SaasTropModel::setReceiverHeight(RX.getHeight());
        SaasTropModel::setReceiverLatitude(RX.getGeodeticLatitude());
        SaasTropModel::setDayOfYear(int((static_cast<YDSTime>(tt).doy)));

        if (!valid) {
            if (!validWeather) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: weather"));
            if (!validRxLatitude) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: Rx Latitude"));
            if (!validRxHeight) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: Rx Height"));
            if (!validDOY) GPSTK_THROW(
                InvalidTropModel("Invalid Saastamoinen trop model: day of year"));
            valid = true;
        }

        double corr = 0.0;
        try {
            corr = SaasTropModel::correction(RX.elevation(SV));
        }
        catch (Exception& e) { GPSTK_RETHROW(e); }

        return corr;

    }  // end SaasTropModel::correction(RX,SV,TT)

    double SaasTropModel::correction(const Xvt& RX,
        const Xvt& SV,
        const CommonTime& tt)
    {
        Position R(RX), S(SV);
        return SaasTropModel::correction(R, S, tt);
    }

    // Compute and return the zenith delay for dry component of the troposphere
    double SaasTropModel::dry_zenith_delay(void) const
    {
        THROW_IF_INVALID_DETAILED();

        //return (0.0022768*pr/(1-0.00266*::cos(2*lat*DEG_TO_RAD)-0.00028*ht/1000.));
        return SaasDryDelay(press, latitude, height);

    }  // end SaasTropModel::dry_zenith_delay()

       // Compute and return the zenith delay for wet component of the troposphere
    double SaasTropModel::wet_zenith_delay(void) const
    {
        THROW_IF_INVALID_DETAILED();

        double T = temp + CELSIUS_TO_KELVIN;

        // partial pressure due to water vapor. Leick 4th ed 8.2.4
        double pwv = 0.01 * humid * ::exp(-37.2465 + 0.213166 * T - 0.000256908 * T * T);
        // IERS2003 Ch 9 pg 99 - very similar to Leick above
        //double pwv = 0.01*humid
        //      * 0.01*::exp(33.93711047-1.9121316e-2*T+1.2378847e-5*T*T-6.3431645e3/T)
        //      * (1.00062+3.14e-6*press+5.6e-7*temp);

        // Saastamoinen 1973 Atmospheric correction for the troposphere and
        // stratosphere in radio ranging of satellites. The use of artificial
        // satellites for geodesy, Geophys. Monogr. Ser. 15, Amer. Geophys. Union,
        // pp. 274-251, 1972, modified for gravity as in Davis etal 1985
        return (0.002277 * ((1255 / T) + 0.05) * pwv /
            (1 - 0.00266 * ::cos(2 * latitude * DEG_TO_RAD) - 0.00028 * height / 1000.));

    }  // end SaasTropModel::wet_zenith_delay()

       // Compute and return the mapping function for dry component of the troposphere
       // @param elevation Elevation of satellite as seen at receiver, in degrees
    double SaasTropModel::dry_mapping_function(double elevation) const
    {
        THROW_IF_INVALID_DETAILED();
        if (elevation < 0.0) return 0.0;

        double lat, t, ct;
        lat = fabs(latitude);         // degrees
        t = doy - 28.;                // mid-winter
        if (latitude < 0)              // southern hemisphere
            t += 365.25 / 2.;
        t *= 360.0 / 365.25;            // convert to degrees
        ct = ::cos(t * DEG_TO_RAD);

        double a, b, c;
        if (lat < 15.) {
            a = SaasDryA[0];
            b = SaasDryB[0];
            c = SaasDryC[0];
        }
        else if (lat < 75.) {          // coefficients are for 15,30,45,60,75 deg
            int i = int(lat / 15.0) - 1;
            double frac = (lat - 15. * (i + 1)) / 15.;
            a = SaasDryA[i] + frac * (SaasDryA[i + 1] - SaasDryA[i]);
            b = SaasDryB[i] + frac * (SaasDryB[i + 1] - SaasDryB[i]);
            c = SaasDryC[i] + frac * (SaasDryC[i + 1] - SaasDryC[i]);

            a -= ct * (SaasDryA1[i] + frac * (SaasDryA1[i + 1] - SaasDryA1[i]));
            b -= ct * (SaasDryB1[i] + frac * (SaasDryB1[i + 1] - SaasDryB1[i]));
            c -= ct * (SaasDryC1[i] + frac * (SaasDryC1[i + 1] - SaasDryC1[i]));
        }
        else {
            a = SaasDryA[4] - ct * SaasDryA1[4];
            b = SaasDryB[4] - ct * SaasDryB1[4];
            c = SaasDryC[4] - ct * SaasDryC1[4];
        }

        double se = ::sin(elevation * DEG_TO_RAD);
        double map = (1. + a / (1. + b / (1. + c))) / (se + a / (se + b / (se + c)));

        a = 0.0000253;
        b = 0.00549;
        c = 0.00114;
        map += (height / 1000.0) * (1. / se - (1 + a / (1. + b / (1. + c))) / (se + a / (se + b / (se + c))));

        return map;

    }  // end SaasTropModel::dry_mapping_function()

       // Compute and return the mapping function for wet component of the troposphere
       // @param elevation Elevation of satellite as seen at receiver, in degrees.
    double SaasTropModel::wet_mapping_function(double elevation) const
    {
        THROW_IF_INVALID_DETAILED();
        if (elevation < 0.0) return 0.0;

        double a, b, c, lat;
        lat = fabs(latitude);         // degrees
        if (lat < 15.) {
            a = SaasWetA[0];
            b = SaasWetB[0];
            c = SaasWetC[0];
        }
        else if (lat < 75.) {          // coefficients are for 15,30,45,60,75 deg
            int i = int(lat / 15.0) - 1;
            double frac = (lat - 15. * (i + 1)) / 15.;
            a = SaasWetA[i] + frac * (SaasWetA[i + 1] - SaasWetA[i]);
            b = SaasWetB[i] + frac * (SaasWetB[i + 1] - SaasWetB[i]);
            c = SaasWetC[i] + frac * (SaasWetC[i + 1] - SaasWetC[i]);
        }
        else {
            a = SaasWetA[4];
            b = SaasWetB[4];
            c = SaasWetC[4];
        }

        double se = ::sin(elevation * DEG_TO_RAD);
        double map = (1. + a / (1. + b / (1. + c))) / (se + a / (se + b / (se + c)));

        return map;

    }

    // Re-define the weather data.
    // If called, typically called before any calls to correction().
    // @param T temperature in degrees Celsius
    // @param P atmospheric pressure in millibars
    // @param H relative humidity in percent
    void SaasTropModel::setWeather(const double& T,
        const double& P,
        const double& H)
    {
        temp = T;
        press = P;
        // humid actually stores water vapor partial pressure
        double exp = 7.5 * T / (T + 237.3);
        humid = 6.11 * (H / 100.) * std::pow(10.0, exp);

        validWeather = true;
        valid = (validWeather && validRxHeight && validRxLatitude && validDOY);

    }

    // Re-define the tropospheric model with explicit weather data.
    // Typically called just before correction().
    // @param wx the weather to use for this correction
    void SaasTropModel::setWeather(const WxObservation& wx)
    {
        try
        {
            SaasTropModel::setWeather(wx.temperature, wx.pressure, wx.humidity);
        }
        catch (InvalidParameter& e)
        {
            valid = validWeather = false;
            GPSTK_RETHROW(e);
        }
    }

    // Define the receiver height; this required before calling
    // correction() or any of the zenith_delay or mapping_function routines.
    void SaasTropModel::setReceiverHeight(const double& ht)
    {
        height = ht;
        validRxHeight = true;
        valid = (validWeather && validRxHeight && validRxLatitude && validDOY);
    }

    // Define the latitude of the receiver; this is required before calling
    // correction() or any of the zenith_delay or mapping_function routines.
    void SaasTropModel::setReceiverLatitude(const double& lat)
    {
        latitude = lat;
        validRxLatitude = true;
        valid = (validWeather && validRxHeight && validRxLatitude && validDOY);
    }

    // Define the day of year; this is required before calling
    // correction() or any of the zenith_delay or mapping_function routines.
    void SaasTropModel::setDayOfYear(const int& d)
    {
        doy = d;
        if (doy > 0 && doy < 367) validDOY = true; else validDOY = false;
        valid = (validWeather && validRxHeight && validRxLatitude && validDOY);
    }

    ///
    void SaasTropModel::getGPT(const Position& rxPos, const double mjd, double& pressure, double& temper)
    {
		double V[10][10], W[10][10];
		int I, N, M, NMAX, MMAX;
		double DOY, TEMP0, PRES0, APM, APA, ATM, ATA, HORT, X, Y, Z;
		double DLAT, DLON, DHGT, PRES, TEMP, UNDU;

		double a_geoid[55] = {
			-5.6195e-001,-6.0794e-002,-2.0125e-001,-6.4180e-002,-3.6997e-002,
			+1.0098e+001,+1.6436e+001,+1.4065e+001,+1.9881e+000,+6.4414e-001,
			-4.7482e+000,-3.2290e+000,+5.0652e-001,+3.8279e-001,-2.6646e-002,
			+1.7224e+000,-2.7970e-001,+6.8177e-001,-9.6658e-002,-1.5113e-002,
			+2.9206e-003,-3.4621e+000,-3.8198e-001,+3.2306e-002,+6.9915e-003,
			-2.3068e-003,-1.3548e-003,+4.7324e-006,+2.3527e+000,+1.2985e+000,
			+2.1232e-001,+2.2571e-002,-3.7855e-003,+2.9449e-005,-1.6265e-004,
			+1.1711e-007,+1.6732e+000,+1.9858e-001,+2.3975e-002,-9.0013e-004,
			-2.2475e-003,-3.3095e-005,-1.2040e-005,+2.2010e-006,-1.0083e-006,
			+8.6297e-001,+5.8231e-001,+2.0545e-002,-7.8110e-003,-1.4085e-004,
			-8.8459e-006,+5.7256e-006,-1.5068e-006,+4.0095e-007,-2.4185e-008 };
		double b_geoid[55] = {
			+0.0000e+000,+0.0000e+000,-6.5993e-002,+0.0000e+000,+6.5364e-002,
			-5.8320e+000,+0.0000e+000,+1.6961e+000,-1.3557e+000,+1.2694e+000,
			0.0000e+000,-2.9310e+000,+9.4805e-001,-7.6243e-002,+4.1076e-002,
			+0.0000e+000,-5.1808e-001,-3.4583e-001,-4.3632e-002,+2.2101e-003,
			-1.0663e-002,+0.0000e+000,+1.0927e-001,-2.9463e-001,+1.4371e-003,
			-1.1452e-002,-2.8156e-003,-3.5330e-004,+0.0000e+000,+4.4049e-001,
			+5.5653e-002,-2.0396e-002,-1.7312e-003,+3.5805e-005,+7.2682e-005,
			+2.2535e-006,+0.0000e+000,+1.9502e-002,+2.7919e-002,-8.1812e-003,
			+4.4540e-004,+8.8663e-005,+5.5596e-005,+2.4826e-006,+1.0279e-006,
			+0.0000e+000,+6.0529e-002,-3.5824e-002,-5.1367e-003,+3.0119e-005,
			-2.9911e-005,+1.9844e-005,-1.2349e-006,-7.6756e-009,+5.0100e-008
		};
		double ap_mean[55] = {
			+1.0108e+003,+8.4886e+000,+1.4799e+000,-1.3897e+001,+3.7516e-003,
			-1.4936e-001,+1.2232e+001,-7.6615e-001,-6.7699e-002,+8.1002e-003,
			-1.5874e+001,+3.6614e-001,-6.7807e-002,-3.6309e-003,+5.9966e-004,
			+4.8163e+000,-3.7363e-001,-7.2071e-002,+1.9998e-003,-6.2385e-004,
			-3.7916e-004,+4.7609e+000,-3.9534e-001,+8.6667e-003,+1.1569e-002,
			+1.1441e-003,-1.4193e-004,-8.5723e-005,+6.5008e-001,-5.0889e-001,
			-1.5754e-002,-2.8305e-003,+5.7458e-004,+3.2577e-005,-9.6052e-006,
			-2.7974e-006,+1.3530e+000,-2.7271e-001,-3.0276e-004,+3.6286e-003,
			-2.0398e-004,+1.5846e-005,-7.7787e-006,+1.1210e-006,+9.9020e-008,
			+5.5046e-001,-2.7312e-001,+3.2532e-003,-2.4277e-003,+1.1596e-004,
			+2.6421e-007,-1.3263e-006,+2.7322e-007,+1.4058e-007,+4.9414e-009
		};
		double bp_mean[55] = {
			+0.0000e+000,+0.0000e+000,-1.2878e+000,+0.0000e+000,+7.0444e-001,
			+3.3222e-001,+0.0000e+000,-2.9636e-001,+7.2248e-003,+7.9655e-003,
			+0.0000e+000,+1.0854e+000,+1.1145e-002,-3.6513e-002,+3.1527e-003,
			+0.0000e+000,-4.8434e-001,+5.2023e-002,-1.3091e-002,+1.8515e-003,
			+1.5422e-004,+0.0000e+000,+6.8298e-001,+2.5261e-003,-9.9703e-004,
			-1.0829e-003,+1.7688e-004,-3.1418e-005,+0.0000e+000,-3.7018e-001,
			+4.3234e-002,+7.2559e-003,+3.1516e-004,+2.0024e-005,-8.0581e-006,
			-2.3653e-006,+0.0000e+000,+1.0298e-001,-1.5086e-002,+5.6186e-003,
			+3.2613e-005,+4.0567e-005,-1.3925e-006,-3.6219e-007,-2.0176e-008,
			+0.0000e+000,-1.8364e-001,+1.8508e-002,+7.5016e-004,-9.6139e-005,
			-3.1995e-006,+1.3868e-007,-1.9486e-007,+3.0165e-010,-6.4376e-010
		};
		double ap_amp[55] = {
			-1.0444e-001,+1.6618e-001,-6.3974e-002,+1.0922e+000,+5.7472e-001,
			-3.0277e-001,-3.5087e+000,+7.1264e-003,-1.4030e-001,+3.7050e-002,
			+4.0208e-001,-3.0431e-001,-1.3292e-001,+4.6746e-003,-1.5902e-004,
			+2.8624e+000,-3.9315e-001,-6.4371e-002,+1.6444e-002,-2.3403e-003,
			+4.2127e-005,+1.9945e+000,-6.0907e-001,-3.5386e-002,-1.0910e-003,
			-1.2799e-004,+4.0970e-005,+2.2131e-005,-5.3292e-001,-2.9765e-001,
			-3.2877e-002,+1.7691e-003,+5.9692e-005,+3.1725e-005,+2.0741e-005,
			-3.7622e-007,+2.6372e+000,-3.1165e-001,+1.6439e-002,+2.1633e-004,
			+1.7485e-004,+2.1587e-005,+6.1064e-006,-1.3755e-008,-7.8748e-008,
			-5.9152e-001,-1.7676e-001,+8.1807e-003,+1.0445e-003,+2.3432e-004,
			+9.3421e-006,+2.8104e-006,-1.5788e-007,-3.0648e-008,+2.6421e-010
		};
		double bp_amp[55] = {
			+0.0000e+000,+0.0000e+000,+9.3340e-001,+0.0000e+000,+8.2346e-001,
			+2.2082e-001,+0.0000e+000,+9.6177e-001,-1.5650e-002,+1.2708e-003,
			+0.0000e+000,-3.9913e-001,+2.8020e-002,+2.8334e-002,+8.5980e-004,
			+0.0000e+000,+3.0545e-001,-2.1691e-002,+6.4067e-004,-3.6528e-005,
			-1.1166e-004,+0.0000e+000,-7.6974e-002,-1.8986e-002,+5.6896e-003,
			-2.4159e-004,-2.3033e-004,-9.6783e-006,+0.0000e+000,-1.0218e-001,
			-1.3916e-002,-4.1025e-003,-5.1340e-005,-7.0114e-005,-3.3152e-007,
			+1.6901e-006,+0.0000e+000,-1.2422e-002,+2.5072e-003,+1.1205e-003,
			-1.3034e-004,-2.3971e-005,-2.6622e-006,+5.7852e-007,+4.5847e-008,
			+0.0000e+000,+4.4777e-002,-3.0421e-003,+2.6062e-005,-7.2421e-005,
			+1.9119e-006,+3.9236e-007,+2.2390e-007,+2.9765e-009,-4.6452e-009
		};
		double at_mean[55] = {
			+1.6257e+001,+2.1224e+000,+9.2569e-001,-2.5974e+001,+1.4510e+000,
			+9.2468e-002,-5.3192e-001,+2.1094e-001,-6.9210e-002,-3.4060e-002,
			-4.6569e+000,+2.6385e-001,-3.6093e-002,+1.0198e-002,-1.8783e-003,
			+7.4983e-001,+1.1741e-001,+3.9940e-002,+5.1348e-003,+5.9111e-003,
			+8.6133e-006,+6.3057e-001,+1.5203e-001,+3.9702e-002,+4.6334e-003,
			+2.4406e-004,+1.5189e-004,+1.9581e-007,+5.4414e-001,+3.5722e-001,
			+5.2763e-002,+4.1147e-003,-2.7239e-004,-5.9957e-005,+1.6394e-006,
			-7.3045e-007,-2.9394e+000,+5.5579e-002,+1.8852e-002,+3.4272e-003,
			-2.3193e-005,-2.9349e-005,+3.6397e-007,+2.0490e-006,-6.4719e-008,
			-5.2225e-001,+2.0799e-001,+1.3477e-003,+3.1613e-004,-2.2285e-004,
			-1.8137e-005,-1.5177e-007,+6.1343e-007,+7.8566e-008,+1.0749e-009
		};
		double bt_mean[55] = {
			+0.0000e+000,+0.0000e+000,+1.0210e+000,+0.0000e+000,+6.0194e-001,
			+1.2292e-001,+0.0000e+000,-4.2184e-001,+1.8230e-001,+4.2329e-002,
			+0.0000e+000,+9.3312e-002,+9.5346e-002,-1.9724e-003,+5.8776e-003,
			+0.0000e+000,-2.0940e-001,+3.4199e-002,-5.7672e-003,-2.1590e-003,
			+5.6815e-004,+0.0000e+000,+2.2858e-001,+1.2283e-002,-9.3679e-003,
			-1.4233e-003,-1.5962e-004,+4.0160e-005,+0.0000e+000,+3.6353e-002,
			-9.4263e-004,-3.6762e-003,+5.8608e-005,-2.6391e-005,+3.2095e-006,
			-1.1605e-006,+0.0000e+000,+1.6306e-001,+1.3293e-002,-1.1395e-003,
			+5.1097e-005,+3.3977e-005,+7.6449e-006,-1.7602e-007,-7.6558e-008,
			+0.0000e+000,-4.5415e-002,-1.8027e-002,+3.6561e-004,-1.1274e-004,
			+1.3047e-005,+2.0001e-006,-1.5152e-007,-2.7807e-008,+7.7491e-009
		};
		double at_amp[55] = {
			-1.8654e+000,-9.0041e+000,-1.2974e-001,-3.6053e+000,+2.0284e-002,
			+2.1872e-001,-1.3015e+000,+4.0355e-001,+2.2216e-001,-4.0605e-003,
			+1.9623e+000,+4.2887e-001,+2.1437e-001,-1.0061e-002,-1.1368e-003,
			-6.9235e-002,+5.6758e-001,+1.1917e-001,-7.0765e-003,+3.0017e-004,
			+3.0601e-004,+1.6559e+000,+2.0722e-001,+6.0013e-002,+1.7023e-004,
			-9.2424e-004,+1.1269e-005,-6.9911e-006,-2.0886e+000,-6.7879e-002,
			-8.5922e-004,-1.6087e-003,-4.5549e-005,+3.3178e-005,-6.1715e-006,
			-1.4446e-006,-3.7210e-001,+1.5775e-001,-1.7827e-003,-4.4396e-004,
			+2.2844e-004,-1.1215e-005,-2.1120e-006,-9.6421e-007,-1.4170e-008,
			+7.8720e-001,-4.4238e-002,-1.5120e-003,-9.4119e-004,+4.0645e-006,
			-4.9253e-006,-1.8656e-006,-4.0736e-007,-4.9594e-008,+1.6134e-009
		};
		double bt_amp[55] = {
			+0.0000e+000,+0.0000e+000,-8.9895e-001,+0.0000e+000,-1.0790e+000,
			-1.2699e-001,+0.0000e+000,-5.9033e-001,+3.4865e-002,-3.2614e-002,
			+0.0000e+000,-2.4310e-002,+1.5607e-002,-2.9833e-002,-5.9048e-003,
			+0.0000e+000,+2.8383e-001,+4.0509e-002,-1.8834e-002,-1.2654e-003,
			-1.3794e-004,+0.0000e+000,+1.3306e-001,+3.4960e-002,-3.6799e-003,
			-3.5626e-004,+1.4814e-004,+3.7932e-006,+0.0000e+000,+2.0801e-001,
			+6.5640e-003,-3.4893e-003,-2.7395e-004,+7.4296e-005,-7.9927e-006,
			-1.0277e-006,+0.0000e+000,+3.6515e-002,-7.4319e-003,-6.2873e-004,
			-8.2461e-005,+3.1095e-005,-5.3860e-007,-1.2055e-007,-1.1517e-007,
			+0.0000e+000,+3.1404e-002,+1.5580e-002,-1.1428e-003,+3.3529e-005,
			+1.0387e-005,-1.9378e-006,-2.7327e-007,+7.5833e-009,-9.2323e-009
		};

		DLAT = rxPos.getGeodeticLatitude();
		DLON = rxPos.getLongitude();
		DHGT = rxPos.getHeight();

		//      Reference day is 28 January 1980                                           
		//      This is taken from Niell (1996) to be consistent (See References)          
		//      For constant values use: doy = 91.3125
		DOY = mjd - 44239 + 1 - 28;

		//		Define degree n and order m EGM  
		NMAX = MMAX = 9;

		//      Define unit vector
		X = ::cos(DLAT) * ::cos(DLON);
		Y = ::cos(DLAT) * ::sin(DLON);
		Z = ::sin(DLAT);

		//      Legendre polynomials                                                       
		V[1 - 1][1 - 1] = 1.0;
		W[1 - 1][1 - 1] = 0.0;
		V[2 - 1][1 - 1] = Z * V[1 - 1][1 - 1];
		W[2 - 1][1 - 1] = 0.0;

		for (N = 2; N <= NMAX; N++) {
			V[N + 1 - 1][1 - 1] = ((2 * N - 1) * Z * V[N - 1][1 - 1] - (N - 1) * V[N - 1 - 1][1 - 1]) / N;
			W[N + 1 - 1][1 - 1] = 0.0;
		}

		for (M = 1; M <= NMAX; M++) {
			V[M + 1 - 1][M + 1 - 1] = (2 * M - 1) * (X * V[M - 1][M - 1] - Y * W[M - 1][M - 1]);
			W[M + 1 - 1][M + 1 - 1] = (2 * M - 1) * (X * W[M - 1][M - 1] + Y * V[M - 1][M - 1]);
			if (M < NMAX) {
				V[M + 2 - 1][M + 1 - 1] = (2 * M + 1) * Z * V[M + 1 - 1][M + 1 - 1];
				W[M + 2 - 1][M + 1 - 1] = (2 * M + 1) * Z * W[M + 1 - 1][M + 1 - 1];
			}
			for (N = M + 2; N <= NMAX; N++) {
				V[N + 1 - 1][M + 1 - 1] = ((2 * N - 1) * Z * V[N - 1][M + 1 - 1] - (N + M - 1) * V[N - 1 - 1][M + 1 - 1]) / (N - M);
				W[N + 1 - 1][M + 1 - 1] = ((2 * N - 1) * Z * W[N - 1][M + 1 - 1] - (N + M - 1) * W[N - 1 - 1][M + 1 - 1]) / (N - M);
			}
		}

		//      Geoidal height                                                             
		UNDU = 0.0;
		I = 0;

		for (N = 0; N <= NMAX; N++) {
			for (M = 0; M <= N; M++) {
				I = I + 1;
				UNDU = UNDU + (a_geoid[I - 1] * V[N + 1 - 1][M + 1 - 1] + b_geoid[I - 1] * W[N + 1 - 1][M + 1 - 1]);
			}
		}

		//      orthometric height                                                         
		HORT = DHGT - UNDU;

		//      Surface pressure on the geoid                                              
		APM = 0.0;
		APA = 0.0;
		I = 0;


		for (N = 0; N <= NMAX; N++) {
			for (M = 0; M <= N; M++) {
				I = I + 1;
				APM = APM + (ap_mean[I - 1] * V[N + 1 - 1][M + 1 - 1] + bp_mean[I - 1] * W[N + 1 - 1][M + 1 - 1]);
				APA = APA + (ap_amp[I - 1] * V[N + 1 - 1][M + 1 - 1] + bp_amp[I - 1] * W[N + 1 - 1][M + 1 - 1]);
			}
		}

		PRES0 = APM + APA * ::cos(DOY / 365.25 * TWO_PI);

		//      height correction for pressure                                             
		PRES = PRES0 * ::pow(1.0 - 0.0000226 * HORT, 5.225);

		//      Surface temperature on the geoid                                           
		ATM = 0.0;
		ATA = 0.0;
		I = 0;

		for (N = 0; N <= NMAX; N++) {
			for (M = 0; M <= N; M++) {
				I = I + 1;
				ATM = ATM + (at_mean[I - 1] * V[N + 1 - 1][M + 1 - 1] + bt_mean[I - 1] * W[N + 1 - 1][M + 1 - 1]);
				ATA = ATA + (at_amp[I - 1] * V[N + 1 - 1][M + 1 - 1] + bt_amp[I - 1] * W[N + 1 - 1][M + 1 - 1]);
			}
		}

		TEMP0 = ATM + ATA * ::cos(DOY / 365.25 * TWO_PI);

		//      height correction for temperature 
		TEMP = TEMP0 - 0.0065 * HORT;


		pressure = PRES;
		temper = TEMP;
		//*undu = UNDU;
    }
}
