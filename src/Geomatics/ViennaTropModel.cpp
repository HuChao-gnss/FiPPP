#pragma ident "$Id$"

/**
 * @file TropModel.cpp
 * Base class for tropospheric models, plus implementations of several
 * published models
 */

#include "ViennaTropModel.hpp"
#include "MJD.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   //---------------------------------------------------------------------
      /* Tropospheric model based in the Vienna mapping functions.
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

      // Constructor to create a Vienna trop model providing the position
      // of the receiver and current time.
      //
      // @param RX   Receiver position.
      // @param time Time.
    ViennaTropModel::ViennaTropModel(const Position& RX,
                                     const CommonTime& time)
    {
        setReceiverLatitude(RX.getGeodeticLatitude( ));
        setReceiverLongitude(RX.getLongitude());
        setReceiverHeight(RX.getAltitude());
        setModifiedJulianDate(time);
    }


      // Load GPT2 grid file.
      // @param file GPT2 grid file.
    void ViennaTropModel::loadFile(const string& file)
        noexcept(false)
    {
        gpt2DataVec.clear();

        double vec[44] = {0.0};

        ifstream inpf( file.c_str() );

        if( !inpf )
        {
            FileMissingException fme("Could not open GPT2 file " + file);
            GPSTK_THROW(fme);
        }

        string temp;

        // first comment line
        getline(inpf, temp);


        bool ok(true);

        string line;

        while( !inpf.eof() && inpf.good() )
        {
            getline(inpf,line);
            stripTrailing(line,'\r');

            if( inpf.eof() ) break;

            if( inpf.bad() ) { ok = false; break; }

//            cout << line << endl;

            vector<string> data( split(line) );

            GPT2Data gpt2Data;

            //pgrid(n,1:5)  = vec(3:7) -  pressure in Pascal
            for (int i=0; i<5; i++)
                gpt2Data.pgrid[i] = asDouble(data[i+2]);

            //Tgrid(n,1:5)  = vec (8:12) - temperature in Kelvin
            for (int i=0; i<5; i++)
                gpt2Data.Tgrid[i] = asDouble(data[i+7]);

            //Qgrid(n,1:5)  = vec(13:17)/1000.d0 // specific humidity in kg/kg
            for (int i=0; i<5; i++)
                gpt2Data.Qgrid[i] = asDouble(data[i+12])/1e+3;

            //dTgrid(n,1:5) = vec(18:22)/1000.d0 // temperature lapse rate in Kelvin/m
            for (int i=0; i<5; i++)
                gpt2Data.dTgrid[i] = asDouble(data[i+17])/1e+3;
            // u(n) = vec(23)            // geoid undulation in m
            gpt2Data.undu = asDouble(data[22]);
            //Hs(n) = vec(24)            // orthometric grid height in m
            gpt2Data.Hs = asDouble(data[23]);
            //ahgrid(n,1:5) = vec(25:29)/1000.d0 // hydrostatic mapping function coefficient, dimensionless
            for (int i=0; i<5; i++)
                gpt2Data.ahgrid[i] = asDouble(data[i+24])/1e+3;
            //awgrid(n,1:5) = vec(30:34)/1000.d0 // wet mapping function coefficient, dimensionless
            for (int i=0; i<5; i++)
                gpt2Data.awgrid[i] = asDouble(data[i+29])/1e+3;
            //lagrid(n,1:5) = vec(35:39)         // water vapour decrease factor, dimensionless
            for (int i=0; i<5; i++)
                gpt2Data.lagrid[i] = asDouble(data[i+34]);
            //Tmgrid(n,1:5) = vec(40:44)         // weighted mean temperature, Kelvin
            for (int i=0; i<5; i++)
                gpt2Data.Tmgrid[i] = asDouble(data[i+39]);

            gpt2DataVec.push_back( gpt2Data );

        } // End of 'while(...)'

        inpf.close();

   }  // end ViennaTropModel::loadFile()


   //mean gravity in m/s**2
   static const double meanGravity( 9.80665 );

   //molar mass of dry air in kg/mol
   static const double mmDryAir( 28.965e-3 );

   //universal gas constant in J/K/mol
   static const double uGasConstant( 8.3143 );


      // Compute and return the full tropospheric delay. The receiver
      // latitude, longitude, height and modified julian date must has
      // been set before using the appropriate constructor or the provided
      // methods.
      //
      // @param elevation Elevation of satellite as seen at receiver,
      // in degrees
   double ViennaTropModel::correction(double elevation) const
      noexcept(false)
   {

      if(!valid)
      {
         if(!validLat)
         {
            throw InvalidTropModel("Invalid Vienna trop model: Rx Latitude");
         }

         if(!validLon)
         {
            throw InvalidTropModel("Invalid Vienna trop model: Rx Longitude");
         }

         if(!validHeight)
         {
            throw InvalidTropModel("Invalid Vienna trop model: Rx Height");
         }

         if(!validMJD)
         {
            throw InvalidTropModel("Invalid Vienna trop model: modified julian date");
         }
      }

      double map_dry(ViennaTropModel::dry_mapping_function(elevation));

      double map_wet(ViennaTropModel::wet_mapping_function(elevation));

         // Compute tropospheric delay
      double tropDelay( (ViennaTropModel::dry_zenith_delay() * map_dry) +
                        (ViennaTropModel::wet_zenith_delay() * map_wet) );

      return tropDelay;

   }  // end ViennaTropModel::correction(elevation)


      /* Compute and return the full tropospheric delay, given the
       * positions of receiver and satellite.
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
   double ViennaTropModel::correction( const Position& RX,
                                       const Position& SV )
      noexcept(false)
   {

      try
      {
         setReceiverLatitude(RX.getGeodeticLatitude());
         setReceiverLongitude(RX.getLongitude());
         setReceiverHeight( RX.getAltitude() );
         setWeather();
      }
      catch(GeometryException& e)
      {
         valid = false;
      }

      if(!valid)
      {
         throw InvalidTropModel("Invalid model");
      }

      double c;
      try
      {
         c = ViennaTropModel::correction(RX.elevationGeodetic(SV));
      }
      catch(InvalidTropModel& e)
      {
         GPSTK_RETHROW(e);
      }

      return c;

   }  // end ViennaTropModel::correction(RX,SV)


      /* Compute and return the full tropospheric delay, given the
       * positions of receiver and satellite and the time tag.
       *
       * This version is more useful within positioning algorithms, where
       * the receiver position may vary; it computes the elevation (and
       * other receiver location information as height and latitude), and
       * passes them to appropriate methods.
       *
       * @param RX  Receiver position.
       * @param SV  Satellite position.
       * @param tt  Time (CommonTime object).
       */
   double ViennaTropModel::correction( const Position& RX,
                                       const Position& SV,
                                       const CommonTime& tt )
      noexcept(false)
   {

      setModifiedJulianDate(tt);

      return ViennaTropModel::correction(RX,SV);

   }  // end ViennaTropModel::correction(RX,SV,TT)


      /* Compute and return the full tropospheric delay, given the
       * positions of receiver and satellite and the modified julian
       * date.
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
   double ViennaTropModel::correction( const Position& RX,
                                       const Position& SV,
                                       const double& mjd )
      noexcept(false)
   {

      setModifiedJulianDate(mjd);

      return ViennaTropModel::correction(RX,SV);

   }  // end ViennaTropModel::correction(RX,SV,mjd)


      // Compute and return the zenith delay for the dry component of
      // the troposphere.
   double ViennaTropModel::dry_zenith_delay(void) const
      noexcept(false)
   {

      if( !valid )
      {
         throw InvalidTropModel("Invalid model");
      }

      double ddry(0.0);

      //calculate denominator f
      double f = 1.0 - 0.00266 * std::cos(2.0*ViennaLat*DEG_TO_RAD)
                     - 0.00000028 * ViennaHeight;

      //calculate the zenith hydrostatic delay
      ddry = 0.0022768*pressure/f;

      return ddry;

   }  // end ViennaTropModel::dry_zenith_delay()


      // Compute and return the zenith delay for the wet component of
      // the troposphere.
   double ViennaTropModel::wet_zenith_delay(void) const
      noexcept(false)
   {

      if( !valid )
      {
         throw InvalidTropModel("Invalid model");
      }

      double dwet(0.0);

      //coefficients
      double k1( 77.604 );                      //K/hPa
      double k2( 64.79 );                       //K/hPa
      double k2p( k2 - k1*18.0152/28.9644 );    //K/hPa
      double k3( 377600.0 );                    //KK/hPa

      //specific gas constant for dry consituents
      double Rd( uGasConstant/mmDryAir );

      dwet = 1e-6*(k2p + k3/mtWaterVapor)*Rd/(dfWaterVapor + 1.0)/meanGravity*pWaterVapor;

      return dwet;

   }  // end ViennaTropModel::wet_zenith_delay()


      // Compute and return the mapping function for dry component of
      // the troposphere.
      //
      // @param elevation Elevation of satellite as seen at receiver, in
      //                  degrees
   double ViennaTropModel::dry_mapping_function(double elevation) const
      noexcept(false)
   {
      if(!valid)
      {
         if(!validLat)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Latitude" ) );
         }

         if(!validLon)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Longitude" ) );
         }

         if(!validHeight)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Height" ) );
         }

         if(!validMJD)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: \
                                            modified julian date" ) );
         }

         GPSTK_THROW(InvalidTropModel( "Valid flag corrupted in Vienna trop \
                                        model" ) );
      }

      double vmf1h(0.0);

      //     reference day is 28 January
      //     this is taken from Niell (1996) to be consistent

      double doy(ViennaMJD - 44239.0 + 1 - 28);

      double bh(0.0029), c0h(0.062);
      double phh(0.0);
      double c11h(0.0), c10h(0.0);

      if( ViennaLat < 0.0 ){ /* southern hemisphere*/
          phh = PI;
          c11h = 0.007;
          c10h = 0.002;
      }
      else{ /* northern hemisphere*/
          phh = 0.0;
          c11h = 0.005;
          c10h = 0.001;
      }

      double clat( std::cos(ViennaLat*DEG_TO_RAD) );
      double cdoy( std::cos(doy/365.25*2.0*PI+phh) );

      double ch( c0h + ((cdoy + 1.0)*c11h/2.0 + c10h)*(1.0 - clat) );

      double sine = std::sin(elevation*DEG_TO_RAD);

      double beta(0.0), gamma(0.0), topcon(0.0);

      beta = bh/(sine + ch);
      gamma = hmfCoeff/(sine + beta);
      topcon = 1.0 + hmfCoeff/(1.0 + bh/(1.0 + ch));

      vmf1h = topcon/(sine + gamma);

      //  height correction [Niell, 1996]
      double a_ht( 2.53e-5 );
      double b_ht( 5.49e-3 );
      double c_ht( 1.14e-3 );
      double hs_km( ViennaHeight/1000.0 );

      beta = b_ht/(sine + c_ht);
      gamma = a_ht/(sine + beta);
      topcon = 1.0 + a_ht/(1.0 + b_ht/(1.0 + c_ht));

      double ht_corr_coef = 1.0/sine - topcon/(sine + gamma);
      double ht_corr = ht_corr_coef*hs_km;

      vmf1h += ht_corr;

      return vmf1h;

   }  // end ViennaTropModel::dry_mapping_function()


      // Compute and return the mapping function for wet component of the
      // troposphere.
      //
      // @param elevation Elevation of satellite as seen at receiver,
      //                  in degrees.
   double ViennaTropModel::wet_mapping_function(double elevation) const
      noexcept(false)
   {
      if(!valid)
      {
         if(!validLat)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Latitude" ) );
         }

         if(!validLon)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Longitude" ) );
         }

         if(!validHeight)
         {
            GPSTK_THROW( InvalidTropModel( "Invalid Vienna trop model: Rx \
                                            Height" ) );
         }

         if(!validMJD)
         {
            GPSTK_THROW( InvalidTropModel(" Invalid Vienna trop model: \
                                            modified julian date" ) );
         }

         GPSTK_THROW(InvalidTropModel( "Valid flag corrupted in Vienna trop \
                                        model" ) );
      }

      double vmf1w(0.0);

      //     reference day is 28 January
      //     this is taken from Niell (1996) to be consistent

      double doy = ViennaMJD - 44239.0 + 1 - 28;

      double sine = std::sin(elevation*DEG_TO_RAD);

      double bw( 0.00146 );
      double cw( 0.04391 );
      double beta = bw/(sine + cw);
      double gamma = wmfCoeff/(sine + beta);
      double topcon = 1.0 + wmfCoeff/(1.0 + bw/(1.0 + cw));

      vmf1w = topcon/(sine + gamma);

      return vmf1w;

   }  // end ViennaTropModel::wet_mapping_function()


      // This method configure the model to estimate the weather using
      // latitude, longitude, height and modified julian date (MJD). It
      // is called automatically when setting those parameters.
   void ViennaTropModel::setWeather()
      noexcept(false)
   {

      if(!validLat)
      {
         valid = false;
         throw InvalidTropModel( "ViennaTropModel must have Rx latitude \
                                  before computing weather ");
      }

      if(!validLon)
      {
         valid = false;
         throw InvalidTropModel( "ViennaTropModel must have Rx longitude \
                                  before computing weather ");
      }

      if(!validHeight)
      {
         valid = false;
         throw InvalidTropModel( "ViennaTropModel must have Rx height \
                                  before computing weather ");
      }

      if(!validMJD)
      {
         valid = false;
         throw InvalidTropModel( "ViennaTropModel must have modified julian \
                                  date before computing weather " );
      }

      if( gpt2DataVec.empty() )
      {
         valid = false;
         throw InvalidTropModel( "ViennaTropModel must have GPT2 grid data \
                                  before computing weather ");
      }

      //change the reference epoch to January 1 2000
      double mjd( ViennaMJD - 51544.5 );

      //factors for amplitudes
      double cosfy = std::cos(mjd/365.25*2.0*PI);
      double coshy = std::cos(mjd/365.25*4.0*PI);
      double sinfy = std::sin(mjd/365.25*2.0*PI);
      double sinhy = std::sin(mjd/365.25*4.0*PI);

      double plon(0.0);

      // only positive longitude in degress
      if( ViennaLon < 0.0 )
          plon = ViennaLon + 360.0;
      else
          plon = ViennaLon;

      //transform to polar distance in degrees
      double ppod(90.0 - ViennaLat);

      //find the index (line in the grid file) of the nearest point
      int ipod( floor(ppod+1.0) );
      int ilon( floor(plon+1.0) );

      // normalized (to one) differences, can be positive or negative
      double diffpod = ppod - (ipod - 0.5);
      double difflon = plon - (ilon - 0.5);

      if(ipod == 181) ipod = 180;

      if (ilon == 361) ilon = 1;

      if (ilon == 0) ilon = 360;

      int indx[4]={0};

      // get the number of the corresponding line
      indx[0] = (ipod - 1)*360 + ilon;
      indx[0] -= 1;  //-1 to use in c

      // near the poles: nearest neighbour interpolation, otherwise: bilinear
      // with the 1 degree grid the limits are lower and upper
      int ibilinear = 0;

      if( (ppod > 0.5) && (ppod < 179.5) ) ibilinear = 1;

      // case of nearest neighborhood
      if (ibilinear == 0) { // then

          int ix = indx[0] - 1;  //-1 to use in c

          GPT2Data data( gpt2DataVec[ix] );

          // transforming ellipsoidal height to orthometric height
          double hgt = ViennaHeight - data.undu;

          double T0, p0, Q;

          // pressure, temperature at the height of the grid
          T0 = data.Tgrid[0] +
               data.Tgrid[1]*cosfy + data.Tgrid[2]*sinfy +
               data.Tgrid[3]*coshy + data.Tgrid[4]*sinhy;

          p0 = data.pgrid[0] +
               data.pgrid[1]*cosfy + data.pgrid[2]*sinfy +
               data.pgrid[3]*coshy + data.pgrid[4]*sinhy;

          // specific humidity
          Q = data.Qgrid[0] +
              data.Qgrid[1]*cosfy + data.Qgrid[2]*sinfy +
              data.Qgrid[3]*coshy + data.Qgrid[4]*sinhy;

          // lapse rate of the temperature
          tLapseRate = data.dTgrid[0] +
                       data.dTgrid[1]*cosfy + data.dTgrid[2]*sinfy +
                       data.dTgrid[3]*coshy + data.dTgrid[4]*sinhy;

          // station height - grid height
          double redh = hgt - data.Hs;

          // temperature at station height in Celsius
          temperature = T0 + tLapseRate*redh - 273.150;

          // temperature lapse rate in degrees / km
          tLapseRate = tLapseRate*1000.0;

          // virtual temperature in Kelvin
          double Tv = T0*(1+0.6077*Q);

          double c = meanGravity*mmDryAir/(uGasConstant*Tv);

          // pressure in hPa
          pressure = (p0*::exp(-c*redh))/100.0;

          // hydrostatic coefficient ah
          hmfCoeff = data.ahgrid[0] +
                     data.ahgrid[1]*cosfy + data.ahgrid[2]*sinfy +
                     data.ahgrid[3]*coshy + data.ahgrid[4]*sinhy;

          // wet coefficient aw
          wmfCoeff = data.awgrid[0] +
                     data.awgrid[1]*cosfy + data.awgrid[2]*sinfy +
                     data.awgrid[3]*coshy + data.awgrid[4]*sinhy;

          // water vapour decrease factor la
          dfWaterVapor = data.lagrid[0] +
                         data.lagrid[1]*cosfy + data.lagrid[2]*sinfy +
                         data.lagrid[3]*coshy + data.lagrid[4]*sinhy;

          // mean temperature of the water vapor Tm
          mtWaterVapor = data.Tmgrid[0] +
                         data.Tmgrid[1]*cosfy + data.Tmgrid[2]*sinfy +
                         data.Tmgrid[3]*coshy + data.Tmgrid[4]*sinhy;

          // water vapor pressure in hPa
          double e0 = Q*p0/(0.622 + 0.378*Q)/100.0;  // on the grid

          // on the station height - (14) Askne and Nordius, 1987
          pWaterVapor = e0 * std::pow( (100.0*pressure/p0),(dfWaterVapor+1) );
      }
      else { // bilinear interpolation

          int sign;

          sign = diffpod>0 ? 1 : -1;
          int ipod1 = ipod + sign;

          sign = difflon>0 ? 1 : -1;
          int ilon1 = ilon + sign;

          // changed for the 1 degree grid
          if (ilon1 == 361) ilon1 = 1;

          if (ilon1 == 0) ilon1 = 360;

          // get the number of the line
          indx[1] = (ipod1 - 1)*360 + ilon;   // along same longitude
          indx[2] = (ipod  - 1)*360 + ilon1;  // along same polar distance
          indx[3] = (ipod1 - 1)*360 + ilon1;  // diagonal

          indx[1] -= 1;  //-1 to use in c
          indx[2] -= 1;  //-1 to use in c
          indx[3] -= 1;  //-1 to use in c

          int l = 0;

          GPT2Data data;

          double undul[4] = {0.0};
          double Ql[4] = {0.0};
          double dTl[4] = {0.0};
          double Tl[4] = {0.0};
          double pl[4] = {0.0};
          double ahl[4] = {0.0};
          double awl[4] = {0.0};
          double lal[4] = {0.0};
          double Tml[4] = {0.0};
          double el[4] = {0.0};

          do{ // do l = 1,4

              //transforming ellipsoidal height to orthometric height:
              //Hortho = -N + Hell
              int ix = indx[l];

              data = gpt2DataVec[ix];

              undul[l] = data.undu;
              double hgt = ViennaHeight - data.undu;

              double T0, p0, Q;

              //pressure, temperature at the height of the grid
              T0 = data.Tgrid[0] +
                   data.Tgrid[1]*cosfy + data.Tgrid[2]*sinfy +
                   data.Tgrid[3]*coshy + data.Tgrid[4]*sinhy;

              p0 = data.pgrid[0] +
                   data.pgrid[1]*cosfy + data.pgrid[2]*sinfy +
                   data.pgrid[3]*coshy + data.pgrid[4]*sinhy;

              //humidity
              Ql[l] = data.Qgrid[0] +
                      data.Qgrid[1]*cosfy + data.Qgrid[2]*sinfy +
                      data.Qgrid[3]*coshy + data.Qgrid[4]*sinhy;

              //reduction = stationheight - gridheight
              double redh = hgt - data.Hs;

              //lapse rate of the temperature in degree / m
              dTl[l] = data.dTgrid[0] +
                       data.dTgrid[1]*cosfy + data.dTgrid[2]*sinfy +
                       data.dTgrid[3]*coshy + data.dTgrid[4]*sinhy;

              //temperature reduction to station height
              Tl[l] = T0 + dTl[l]*redh - 273.150;

              //virtual temperature
              double Tv = T0*(1+0.6077*Ql[l]);
              double c = meanGravity*mmDryAir/(uGasConstant*Tv);

              //pressure in hPa
              pl[l] = (p0 * std::exp(-c*redh))/100.0;

              //hydrostatic coefficient ah
              ahl[l] = data.ahgrid[0] +
                       data.ahgrid[1]*cosfy + data.ahgrid[2]*sinfy +
                       data.ahgrid[3]*coshy + data.ahgrid[4]*sinhy;

              //wet coefficient aw
              awl[l] = data.awgrid[0] +
                       data.awgrid[1]*cosfy + data.awgrid[2]*sinfy +
                       data.awgrid[3]*coshy + data.awgrid[4]*sinhy;

              //water vapor decrease factor la
              lal[l] = data.lagrid[0] +
                       data.lagrid[1]*cosfy + data.lagrid[2]*sinfy +
                       data.lagrid[3]*coshy + data.lagrid[4]*sinhy;

              //mean temperature of the water vapor Tm
              Tml[l] = data.Tmgrid[0] +
                       data.Tmgrid[1]*cosfy + data.Tmgrid[2]*sinfy +
                       data.Tmgrid[3]*coshy + data.Tmgrid[4]*sinhy;

              //water vapor pressure in hPa
              double e0 = Ql[l]*p0/(0.622 + 0.378*Ql[l])/100.0; // on the grid
              // on the station height  (14) Askne and Nordius, 1987
              el[l] = e0 * std::pow( (100.0*pl[l]/p0), (lal[l]+1.0) );

              l++;

          } while (l< 4);//end do

          double dnpod1 = fabs(diffpod); // distance nearer point
          double dnpod2 = 1.0 - dnpod1;   // distance to distant point
          double dnlon1 = fabs(difflon);
          double dnlon2 = 1.0 - dnlon1;

          double R1(0.0), R2(0.0);

          //pressure
          R1 = dnpod2*pl[0]+dnpod1*pl[1];
          R2 = dnpod2*pl[2]+dnpod1*pl[3];
          pressure = dnlon2*R1+dnlon1*R2;

          //temperature
          R1 = dnpod2*Tl[0]+dnpod1*Tl[1];
          R2 = dnpod2*Tl[2]+dnpod1*Tl[3];
          temperature = dnlon2*R1+dnlon1*R2;

          //temperature in degree per km
          R1 = dnpod2*dTl[0]+dnpod1*dTl[1];
          R2 = dnpod2*dTl[2]+dnpod1*dTl[3];
          tLapseRate = (dnlon2*R1+dnlon1*R2)*1000.0;

          //water vapor pressure in hPa
          R1 = dnpod2*el[0]+dnpod1*el[1];
          R2 = dnpod2*el[2]+dnpod1*el[3];
          pWaterVapor = dnlon2*R1+dnlon1*R2;

          //hydrostatic
          R1 = dnpod2*ahl[0]+dnpod1*ahl[1];
          R2 = dnpod2*ahl[2]+dnpod1*ahl[3];
          hmfCoeff = dnlon2*R1+dnlon1*R2;

          //wet
          R1 = dnpod2*awl[0]+dnpod1*awl[1];
          R2 = dnpod2*awl[2]+dnpod1*awl[3];
          wmfCoeff = dnlon2*R1+dnlon1*R2;

          //undulation
          R1 = dnpod2*undul[0]+dnpod1*undul[1];
          R2 = dnpod2*undul[2]+dnpod1*undul[3];
          geoidUndu = dnlon2*R1+dnlon1*R2;

          //water vapor decrease factor la
          R1 = dnpod2*lal[0]+dnpod1*lal[1];
          R2 = dnpod2*lal[2]+dnpod1*lal[3];
          dfWaterVapor = dnlon2*R1+dnlon1*R2;

          //mean temperature of the water vapor Tm
          R1 = dnpod2*Tml[0]+dnpod1*Tml[1];
          R2 = dnpod2*Tml[2]+dnpod1*Tml[3];
          mtWaterVapor = dnlon2*R1+dnlon1*R2;

      } //else

      valid = validLat && validLon && validHeight && validMJD;

   }  // end ViennaTropModel::setWeather()


      // Define the receiver latitude; this is required before calling
      // correction() or any of the zenith_delay routines.
      //
      // @param lat  Latitude of receiver, in degrees.
   void ViennaTropModel::setReceiverLatitude(const double& lat)
   {
      ViennaLat = lat;
      validLat = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


      // Define the receiver longitude; this is required before calling
      // correction() or any of the zenith_delay routines.
      //
      // @param lon  Longitude of receiver, in degrees.
   void ViennaTropModel::setReceiverLongitude(const double& lon)
   {
      ViennaLon = lon;

      validLon = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


      // Define the receiver height; this is required before calling
      // correction() or any of the zenith_delay routines.
      //
      // @param ht   Height of the receiver above ellipsoid,
      //             in meters.
   void ViennaTropModel::setReceiverHeight(const double& ht)
   {
      ViennaHeight = ht;
      validHeight = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


      // Set the time when tropospheric correction will be computed for,
      // in modified julian date.
      //
      // @param mjd  Modified Julian Date.
   void ViennaTropModel::setModifiedJulianDate(const double& mjd)
   {
      ViennaMJD = mjd;
      validMJD = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


      // Set the time when tropospheric correction will be computed for,
      // in modified julian date.
      //
      // @param time  Time object.
   void ViennaTropModel::setModifiedJulianDate(const CommonTime& time)
   {

      ViennaMJD = static_cast<double>((static_cast<MJD>(time)).mjd);
      validMJD = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


      /* Convenient method to set all model parameters in one pass.
       *
       * @param time  Time object.
       * @param rxPos Receiver position object.
       */
   void ViennaTropModel::setAllParameters( const CommonTime& time,
                                           const Position& rxPos )
   {
      MJD tmjd = static_cast<MJD>(time);
      ViennaMJD = static_cast<double>(tmjd.mjd);
      validMJD = true;

      ViennaLat = rxPos.getGeodeticLatitude();
      validLat = true;
      ViennaLon = rxPos.getLongitude();
      validLon = true;
      ViennaHeight = rxPos.getHeight();
      validHeight = true;

         // Change the value of field "valid" if everything is already set
      valid = validLat && validLon && validHeight && validMJD;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();

   }


} // end namespace gpstk
