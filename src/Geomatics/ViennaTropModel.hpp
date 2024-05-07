#pragma ident "$Id$"

#ifndef VIENNA_TROP_MODEL_HPP
#define VIENNA_TROP_MODEL_HPP
      //--------------------------------------------------------------------

      /** Tropospheric model based in the Vienna mapping functions.
       *
       * This model uses the mapping functions developed by Boehm, J., et.al.
       * and published in Boehm, J., et al., 2006, 'Troposphere mapping
       * functions for GPS and very long baseline interferometry from European
       * Centre for Medium-Range Weather Forecasts operational analysis data'
       * J. Geophys. Res., 111, B02406 (also see IERS TN 36).
       *
       * A typical way to use this model follows:
       *
       * @code
       *   ViennaTropModel viennaTM;
       *   viennaTM.loadFile(file);
       *   viennaTM.setReceiverLatitude(lat);
       *   viennaTM.setReceiverLongitude(lon);
       *   viennaTM.setReceiverHeight(height);
       *   viennaTM.setModifiedJulianDate(mjd);
       *
       * @endcode
       *
       * Once all the basic model parameters are set (latitude, longitude,
       * height and modified julian date), then we are able to compute the
       * tropospheric correction as a function of elevation:
       *
       * @code
       *   trop = viennaTM.correction(elevation);
       * @endcode
       *
       */

#include "TropModel.hpp"
namespace gpstk
{
   class ViennaTropModel : public TropModel
   {
   public:

       struct GPT2Data
       {
           double pgrid[5];
           double Tgrid[5];
           double Qgrid[5];
           double dTgrid[5];
           double undu;
           double Hs;
           double ahgrid[5];
           double awgrid[5];
           double lagrid[5];
           double Tmgrid[5];
       };

   public:

         /// Default constructor
      ViennaTropModel(void)
      {
          validLat=false;
          validLon=false;
          validHeight=false;
          validMJD=false;
          valid=false;
          gpt2DataVec.clear();
      };


         /// Constructor to create a Vienna trop model providing the
         /// latitude, the longitude, the height of the receiver above
         /// ellipsoid (as defined by ellipsoid model), and the modified
         /// julian date.
         ///
         /// @param lat  Latitude of receiver, in degrees.
         /// @param lon  Longitude of receiver, in degrees.
         /// @param ht   Height of the receiver above ellipsoid,
         ///             in meters.
         /// @param mjd  Modified Julian Date.
      ViennaTropModel( const double& lat,
                       const double& lon,
                       const double& ht,
                       const double& mjd )
      {
          setReceiverLatitude(lat);
          setReceiverLongitude(lon);
          setReceiverHeight(ht);
          setModifiedJulianDate(mjd);
      };


         /// Constructor to create a Vienna trop model providing the position
         /// of the receiver and current time.
         ///
         /// @param RX   Receiver position.
         /// @param time Time.
      ViennaTropModel( const Position& RX,
                       const CommonTime& time );


         /// Load GPT2 grid file.
         ///
         /// @param file GPT2 grid file.
      void loadFile(const std::string& file)
         noexcept(false);


         /// Compute and return the full tropospheric delay. The receiver
         /// latitude, longitude, height and modified julian date must has
         /// been set before using the appropriate constructor or the provided
         /// methods.
         ///
         /// @param elevation Elevation of satellite as seen at receiver,
         ///                  in degrees.
      virtual double correction(double elevation) const
         noexcept(false);


         /** Compute and return the full tropospheric delay, given the
          *  positions of receiver and satellite.
          *
          * This version is more useful within positioning algorithms, where
          * the receiver position may vary; it computes the elevation (and
          * other receiver location information as latitude, longitude and
          * height) and passes them to appropriate methods.
          *
          * You must set time using method setModifiedJulianDate() before
          * calling this method.
          *
          * @param RX  Receiver position.
          * @param SV  Satellite position.
          */
      virtual double correction( const Position& RX,
                                 const Position& SV )
         noexcept(false);


         /** Compute and return the full tropospheric delay, given the
          *  positions of receiver and satellite and the time tag.
          *
          * This version is more useful within positioning algorithms, where
          * the receiver position may vary; it computes the elevation (and
          * other receiver location information as latitude, longitude and
          * height) and passes them to appropriate methods.
          *
          * @param RX  Receiver position.
          * @param SV  Satellite position.
          * @param tt  Time (CommonTime object).
          */
      virtual double correction( const Position& RX,
                                 const Position& SV,
                                 const CommonTime& tt )
        noexcept(false);


         /** Compute and return the full tropospheric delay, given the
          *  positions of receiver and satellite and the modified julian
          *  date.
          *
          * This version is more useful within positioning algorithms, where
          * the receiver position may vary; it computes the elevation (and
          * other receiver location information as latitude, longitude and
          * height) and passes them to appropriate methods.
          *
          * @param RX  Receiver position.
          * @param SV  Satellite position.
          * @param mjd Modified Julian Date.
          */
      virtual double correction( const Position& RX,
                                 const Position& SV,
                                 const double& mjd )
         noexcept(false);


         /// Compute and return the zenith delay for dry component of
         /// the troposphere.
      virtual double dry_zenith_delay(void) const
         noexcept(false);


         /// Compute and return the zenith delay for wet component of
         /// the troposphere.
      virtual double wet_zenith_delay(void) const
         noexcept(false);


         /// Compute and return the mapping function for dry component of
         /// the troposphere.
         ///
         /// @param elevation Elevation of satellite as seen at receiver, in
         ///                  degrees
      virtual double dry_mapping_function(double elevation) const
         noexcept(false);


         /// Compute and return the mapping function for wet component of
         /// the troposphere.
         ///
         /// @param elevation Elevation of satellite as seen at
         ///                  receiver, in degrees
      virtual double wet_mapping_function(double elevation) const
         noexcept(false);


         /// This method configure the model to estimate the weather using
         /// latitude, longitude, height and modified julian date (MJD). It
         /// is called automatically when setting those parameters.
      void setWeather()
         noexcept(false);


         /// In Vienna tropospheric model, this is a dummy method kept here
         /// just for consistency,
      virtual void setWeather( const double& T,
                               const double& P,
                               const double& H )
         noexcept(false) {};


         /// In Vienna tropospheric model, this is a dummy method kept here
         /// just for consistency
      virtual void setWeather(const WxObservation& wx)
         noexcept(false) {};


         /// Define the receiver latitude; this is required before calling
         /// correction() or any of the zenith_delay routines.
         ///
         /// @param lat  Latitude of receiver, in degrees.
      virtual void setReceiverLatitude(const double& lat);


         /// Define the receiver longitude; this is required before calling
         /// correction() or any of the zenith_delay routines.
         ///
         /// @param lat  Longitude of receiver, in degrees.
      virtual void setReceiverLongitude(const double& lon);


         /// Define the receiver height; this is required before calling
         /// correction() or any of the zenith_delay routines.
         ///
         /// @param ht   Height of the receiver above ellipsoid,
         ///             in meters.
      virtual void setReceiverHeight(const double& ht);


         /// Set the time when tropospheric correction will be computed for,
         /// in Modified Julian Date.
         ///
         /// @param mjd  Modified Julian Date.
      virtual void setModifiedJulianDate(const double& mjd);


         /// Set the time when tropospheric correction will be computed for,
         /// in Modified Julian Date.
         ///
         /// @param time  Time object.
      virtual void setModifiedJulianDate(const CommonTime& time);


         /** Convenient method to set all model parameters in one pass.
          *
          * @param time  Time object.
          * @param rxPos Receiver position object.
          */
      virtual void setAllParameters( const CommonTime& time,
                                     const Position& rxPos );


   private:

      std::vector<GPT2Data> gpt2DataVec;

      double ViennaMJD;
      double ViennaLat;
      double ViennaLon;
      double ViennaHeight;

      bool validMJD;
      bool validLat;
      bool validLon;
      bool validHeight;

      // pressure in hPa
      double pressure;
      // temperature in degree Celsius
      double temperature;
      // temperature lapse rate in degrees per km
      double tLapseRate;
      // mean temperature of the water vapor in degrees Kelvin
      double mtWaterVapor;
      // water vapor pressure in hPa
      double pWaterVapor;
      // hydrostatic mapping function coefficient at zero height (VMF1)
      double hmfCoeff;
      // wet mapping function coefficient (VMF1)
      double wmfCoeff;
      // water vapor decrease factor
      double dfWaterVapor;
      // geoid undulation in m
      double geoidUndu;

   };    // end class ViennaTropModel


      //@}

}
#endif   // TROPOSPHERIC_MODELS_GPSTK
