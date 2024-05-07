#pragma ident "$Id$"

/**
 * @file ComputeIonoDelay.cpp
 * This is a class to compute the main values related to a given
 * GNSS IONEX model, i.e., the ionospheric correction and DCB values
 * 
 * reference GNSSBOX
 */


#include "ComputeIonoDelay.hpp"


#define debug 0

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Return a string identifying this object.
   std::string ComputeIonoDelay::getClassName() const
   { return "ComputeIonoDelay"; }



      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   ComputeIonoDelay::ComputeIonoDelay(const Position& RxCoordinates)
      noexcept(false)
   {
      pIonoModel = NULL;
      setInitialRxPosition(RxCoordinates);

   }  // End of constructor 'ComputeIonoDelay::ComputeIonoDelay()'



      /** Explicit constructor, taking as input reference station
       * coordinates and ionex maps (Ionex Store object) to be used.
       *
       * @param RxCoordinates    Receiver coordinates.
       * @param ionoStore           IonoModel object to be used by default.
       * @param dObservable      Observable type to be used by default.
       * @param applyDCB         Whether or not P1 observable will be
       *                         corrected from DCB effect.
       * @param ionoMapType      Type of ionosphere mapping function (string)
       *                         @sa IonoModel::iono_mapping_function
       */
   ComputeIonoDelay::ComputeIonoDelay( const Position& RxCoordinates,
                                       IonoModel& ionoStore)
         noexcept(false)
   {

      setInitialRxPosition(RxCoordinates);
      setIonoModel(ionoStore);

   }  // End of constructor 'ComputeIonoDelay::ComputeIonoDelay()'



      /** Return a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeIonoDelay::Process( const CommonTime& time,
                                               std::map<RinexSatID, TypeIDVec>& satShortTypes,
                                               std::map<RinexSatID, int>& glonassFreqNo,
                                               satTypeValueMap& gData )
      noexcept(false)
   {

      SatIDSet satRejectedSet;

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv)
         {
            SatID sat = stv->first;
            SatelliteSystem system = (*stv).first.system;
            char sysChar(asChar(system));

               // If elevation or azimuth is missing, then remove satellite
            if( stv->second.find(TypeID::elevation) != stv->second.end() &&
                stv->second.find(TypeID::azimuth)   != stv->second.end() )
            {
                  // Scalars to hold satellite elevation, azimuth, ionospheric
                  // map and ionospheric slant delays
               double elevation( stv->second(TypeID::elevation) );
               double azimuth(   stv->second(TypeID::azimuth)   );

               try
               {
                   TypeIDVec typeVec = satShortTypes[sat];

                   for(int i=0; i<typeVec.size(); i++)
                   {
                       // 3-char type str, e.g. L1G/L2G/C1G/C2G...
                       std::string typeStr= typeVec[i].asString();

                       // carrier-band
                       int n = asInt(string(1, typeStr[1]));

                       // glonass
                       double freq;
                       if(sysChar=='R')
                       {
                           int k = glonassFreqNo[sat];
                           freq = getFreq( (*stv).first.system, n, k);
                       }
                       else
                       {
                           freq = getFreq( (*stv).first.system, n);
                       }

                       if(debug)
                       {
                           cout << typeStr << " freq:" << freq << endl; 
                       }

                       double iono_map_func;
                       iono_map_func = iono_mapping_function(elevation);

                       double ionoMap;

                       if(typeStr[0]=='C')
                       {
                           ionoMap = C2_FACT/(freq*freq)*iono_map_func;
                       }
                       else if(typeStr[0] == 'L')
                       {
                           ionoMap = -1.0 * C2_FACT/(freq*freq)*iono_map_func;
                       }

                       if(debug)
                       {
                           cout << "iono_map_func" << iono_map_func << endl;
                           cout << "ionoMap" << ionoMap << endl;
                       }

                       string ionoMapTypeStr = "ionoMap" + typeStr;

                       if(debug)
                       {
                           cout << getClassName() << "ionoMapTypeStr:" << ionoMapTypeStr << endl;
                       }

                       TypeID ionoMapType(ionoMapTypeStr);

                       // by multipying the ionoMap, the ionospheric delay will be expressed as:
                       // I(f) = ionoMap(f) * TEC; 
                       // 1 TECU is about 0.162 meters. 
                       // see GNSS theroy and algorithms
                       (*stv).second[ionoMapType]  = ionoMap;

                       // according carrier-band, construct ionospheric-delay type
                       string ionoTypeStr = "slantIono" + typeStr;

                       if(debug)
                       {
                           cout << getClassName() << "ionoTypeStr:" << ionoTypeStr << endl;
                       }

                       TypeID ionoType(ionoTypeStr);

                       double ionoDelay;

                       // code
                       if(pIonoModel!=NULL)
                       {
                           if(typeStr[0]=='C')
                           {
                              // compute ionospheric delay for current carrier-band
                              ionoDelay 
                                  = (*pIonoModel).getCorrection(time, rxPos, elevation, azimuth, freq);
                           }
                           else if(typeStr[0] == 'L')
                           {
                              ionoDelay 
                                  = -1.0*(*pIonoModel).getCorrection(time, rxPos, elevation, azimuth, freq);
                           }

                           if(debug)
                           {
                               cout << "type:" << typeStr << endl;
                               cout << "ionoDelay:" << ionoDelay << endl;
                           }

                           (*stv).second[ionoType] = ionoDelay;
                       }
                   }

               }
               catch(InvalidRequest)
               {
                  satRejectedSet.insert( stv->first );
                  continue;    // Skip this SV if problems arise
               }
            }
            else
            {
               satRejectedSet.insert( stv->first );
               continue;
            }  // End of 'if( stv->second.find(TypeID::elevation) == ... '

         }  // End of loop 'for(stv = gData.begin()...'

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& e)
      {

         GPSTK_RETHROW(e);

      }

   }  // End of method 'ComputeIonoDelay::Process()'

      /** Ionosphere mapping function
       *
       * @param elevation     Elevation of satellite as seen at receiver
       *                      (degrees).
       * @param ionoMapType   Type of ionosphere mapping function (string)
       *                      (0) NONE no mapping function is applied
       *                      (1) SLM  Single Layer Model (IGS)
       *                      (2) MSLM Modified Single Layer Model (CODE)
       *                      (3) ESM  Extended Slab Model (JLP)
       *
       * Details at: www.aiub.unibe.ch/download/users/schaer/igsiono/doc/mslm.pdf
       *
       * @warning No implementation for JPL's mapping function.
       */
   double ComputeIonoDelay::iono_mapping_function( const double& elevation,
                                                   const std::string& ionoMapType ) const
   {

         // map
      double imap(1.0);
         // Earth's radius in KM
      double Re = 6371.0;
         // zenith angle
      double z0 = 90.0 - elevation;

      if( ionoMapType == "SLM" )
      {
            // As explained in: Hofmann-Wellenhof et al. (2004) - GPS Theory and
            // practice, 5th edition, SpringerWienNewYork, Chapter 6.3, pg. 102

            // ionosphere height in KM
         double ionoHeight = 450.0;
            // zenith angle of the ionospheric pierce point (IPP)
         double sinzipp  = Re / (Re + ionoHeight) * std::sin(z0*DEG_TO_RAD);
         double zipprad  = std::asin(sinzipp);

         imap      = 1.0/std::cos(zipprad);

      }
      else if( ionoMapType == "MSLM" )
      {
         // maximum zenith distance is 80 degrees
         if( z0 <= 80.0 )
         {
           // ionosphere height in KM
           double ionoHeight = 506.7;
           double alfa       = 0.9782;
           // zenith angle of the ionospheric pierce point (IPP)
           double sinzipp = Re / (Re + ionoHeight)
               * std::sin(alfa * z0 * DEG_TO_RAD);
           double zipprad = std::asin(sinzipp);

           imap = 1.0/std::cos(zipprad);
         }

      }
      else if( ionoMapType == "ESM" )
      {
         InvalidRequest e( getClassName() + "ESM is not supported until now: " );
         GPSTK_THROW(e)
      }

      return imap;

   }  // End of method 'IonexStore::iono_mapping_function()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ComputeIonoDelay::setInitialRxPosition(const Position& RxCoordinates)
      noexcept(false)
   {

      try
      {
         rxPos = RxCoordinates;
         return 0;

      }
      catch(GeometryException)
      {
         return -1;
      }

   }  // End of method 'ComputeIonoDelay::setInitialRxPosition()'



}  // End of namespace gpstk
