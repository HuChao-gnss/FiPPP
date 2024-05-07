#pragma ident "$Id$"

/**
 * @file StochasticModel.cpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
// 
// 2023-5-25
// Add the random walk process noise value to the previous epoch value
// @ragn wang
//
//============================================================================


#include "StochasticModel.hpp"

namespace gpstk
{


      // Get element of the process noise matrix Q
   double RandomWalkModel::getQ()
   {

         // Compute current variance
      double variance(qprime*std::abs(currentTime - previousTime));

	  if(variance == 0.0)
		  variance = qprime*30; // default: 30 seconds

         // Return variance
      return variance;

   }  // End of method 'PhaseAmbiguityModel::getQ()'





      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void RandomWalkModel::Prepare( const SatID& sat,
                                  gnssRinex& gData )
   {

         // Update previous epoch
      setPreviousTime(currentTime);

      setCurrentTime(gData.header.epoch);

      return;

   }  // End of method 'RandomWalkModel::Prepare()'



      // Get element of the state transition matrix Phi
   double PhaseAmbiguityModel::getPhi()
   {

         // Check if there is a cycle slip
      if(cycleSlip)
      {
         return 0.0;
      }
      else
      {
         return 1.0;
      }

   }  // End of method 'PhaseAmbiguityModel::getPhi()'



      // Get element of the process noise matrix Q
   double PhaseAmbiguityModel::getQ()
   {

         // Check if there is a cycle slip
      if(cycleSlip)
      {
         return variance;
      }
      else
      {
         return 0.0; //original code
		 //return 1e-16; // a very small value
      }

   }  // End of method 'PhaseAmbiguityModel::getQ()'



      /* This method checks if a cycle slip happened.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void PhaseAmbiguityModel::checkCS( const SatID& sat,
                                      satTypeValueMap& data,
                                      const SourceID& source )
   {

      try
      {

            // By default, assume there is no cycle slip
         setCS(false);

            // Check if satellite is present at this epoch
         if( data.find(sat) == data.end() )
         {
            // If satellite is not present, declare CS and exit
            setCS(true);

            return;
         }


         if (!watchSatArc)
         {
               // In this case, we only use cycle slip flags
               // Check if there was a cycle slip
            if (data(sat)(csFlagType) > 0.0)
            {
               setCS(true);
            }

         }
         else
         {
               // Check if this satellite has previous entries
            if( satArcMap[ source ].find(sat) == satArcMap[ source ].end() )
            {
                  // If it doesn't have an entry, insert one
               satArcMap[ source ][ sat ] = 0.0;
            };


               // Check if arc number is different than arc number in storage
            if ( data(sat)(TypeID::satArc) != satArcMap[ source ][ sat ] )
            {
               setCS(true);
               satArcMap[ source ][ sat ] = data(sat)(TypeID::satArc);
            }

         }
      }
      catch(Exception& e)
      {
         setCS(true);
      }

      return;

   } // End of method 'PhaseAmbiguityModel::checkCS()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for zenital wet
       * tropospheric delay is set to 3e-8 m*m/s (equivalent to about
       * 1.0 cm*cm/h).
       *
	   * \Warning: This function should be used after prepare, or tmData
	   * will be blank and default qprime is set.
       */
   TropoRandomWalkModel& TropoRandomWalkModel::setQprime(double qp)
   {

         // Look at each source being currently managed
      for( std::map<SourceID, tropModelData>::iterator it = tmData.begin();
           it != tmData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'TropoRandomWalkModel::setQprime()'



      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void TropoRandomWalkModel::Prepare( const SatID& sat,
                                       gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );
      // added by ragnwang to calculate the process noise.
      if (firstTime)
      {
          setFirstTime(source, gData.header.epoch);
          firstTime = false;
      }

         // Second, let's update current epoch for this source
      setCurrentTime(source, gData.header.epoch );

         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, tmData[source].currentTime);

      return;

   }  // End of method 'TropoRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void TropoRandomWalkModel::computeQ( const SatID& sat,
                                        satTypeValueMap& data,
                                        SourceID& source )
   {

         // Compute current variance
      variance += tmData[ source ].qprime
                 * std::abs( tmData[ source ].currentTime
                           - tmData[ source ].previousTime );

      return;

   }  // End of method 'TropoRandomWalkModel::computeQ()'

   void TropoRandomWalkModel::computeQ( const SatID& sat,
                                        typeValueMap& tData,
                                        SourceID& source )
   {

         // Compute current variance
      variance = tmData[ source ].qprime
                 * std::abs( tmData[ source ].currentTime
                           - tmData[ source ].previousTime );

      return;

   }  // End of method 'TropoRandomWalkModel::computeQ()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for tropospheric gradient
	   * is set to 5e-10 m*m/s.
       *
	   * \Warning: This function should be used after prepare, or gmData
	   * will be blank and default qprime is set.
       */
   TropoGradRandomWalkModel& TropoGradRandomWalkModel::setQprime(double qp)
   {

         // Look at each source being currently managed
      for( std::map<SourceID, tropGradModelData>::iterator it = gmData.begin();
           it != gmData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'TropoGradRandomWalkModel::setQprime()'




      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void TropoGradRandomWalkModel::Prepare( const SatID& sat,
                                       gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );

         // Second, let's update current epoch for this source
      setCurrentTime(source, gData.header.epoch );

         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, gmData[source].currentTime);

      return;

   }  // End of method 'TropoGradRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void TropoGradRandomWalkModel::computeQ( const SatID& sat,
                                        satTypeValueMap& data,
                                        SourceID& source )
   {

         // Compute current variance
      variance = gmData[ source ].qprime
                 * std::abs( gmData[ source ].currentTime
                           - gmData[ source ].previousTime );

      return;

   }  // End of method 'TropoRandomWalkModel::computeQ()'



      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for satellite bias 
       * is set to 0.01 m*m/s ( variation is about 0.1 m/s).
       *
       */
   IonoRandomWalkModel& IonoRandomWalkModel::setQprime(double qp)
   {

         // Look at each satellite being currently managed
      for( std::map<SatID, ionoModelData>::iterator it = imData.begin();
           it != imData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'IonoRandomWalkModel::setQprime()'



      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void IonoRandomWalkModel::Prepare( const SatID& sat,
                                      gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );
      // added by ragnwang to calculate the process noise.
      if (sats.find(sat)==sats.end())
      {
          setFirstTime(sat, gData.header.epoch);
          sats.insert(sat);
      }
         // Second, let's update current epoch for this source
      setCurrentTime(sat, gData.header.epoch );

            // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(sat, imData[sat].currentTime);

      return;

   }  // End of method 'IonoRandomWalkModel::Prepare()'


      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void IonoRandomWalkModel::computeQ( const SatID& sat,
                                          satTypeValueMap& data,
                                          SourceID& source )
   {
	   double  ele(0.0);
	   try
	   {
		   ele = data[sat][TypeID::elevation];
	   }
	   catch(...)
	   {
		   ele = 7.0;
	   }

	   if(ele < 7.0) ele = 7.0;

	   double q = imData[ sat ].qprime;
	   if(ele < 30.0) q = q/2.0/std::sin(ele*DEG_TO_RAD);

	   // Compute current variance
	   variance += q * std::abs( imData[ sat ].currentTime
		   - imData[ sat ].previousTime );

      return;

   }  // End of method 'IonoRandomWalkModel::computeQ()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for receiver bias 
       * is set to 3e-8 m*m/s (equivalent to about * 1.0 cm*cm/h).
       *
       */
   RecBiasRandomWalkModel& RecBiasRandomWalkModel::setQprime(double qp)
   {

         // Look at each source being currently managed
      for( std::map<SourceID, recBiasModelData>::iterator it = rbData.begin();
           it != rbData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'RecBiasRandomWalkModel::setQprime()'




      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void RecBiasRandomWalkModel::Prepare( const SatID& sat,
                                         gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );

         // Second, let's update current epoch for this source
      setCurrentTime(source, gData.header.epoch );


         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, rbData[source].currentTime);

      return;

   }  // End of method 'RecBiasRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void RecBiasRandomWalkModel::computeQ( const SatID& sat,
                                          satTypeValueMap& data,
                                          SourceID& source )
   {

         // Compute current variance
      variance = rbData[ source ].qprime
                 * std::abs( rbData[ source ].currentTime
                           - rbData[ source ].previousTime );

      return;

   }  // End of method 'RecBiasRandomWalkModel::computeQ()'



      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for satellite bias 
       * is set to 3e-8 m*m/s (equivalent to about * 1.0 cm*cm/h).
       *
       */
   SatBiasRandomWalkModel& SatBiasRandomWalkModel::setQprime(double qp)
   {

         // Look at each satellite being currently managed
      for( std::map<SatID, satBiasModelData>::iterator it = sbData.begin();
           it != sbData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'SatBiasRandomWalkModel::setQprime()'



      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void SatBiasRandomWalkModel::Prepare( const SatID& sat,
                                         gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );

         // Second, let's update current epoch for this source
      setCurrentTime(sat, gData.header.epoch );

         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(sat, sbData[sat].currentTime);

      return;

   }  // End of method 'SatBiasRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void SatBiasRandomWalkModel::computeQ( const SatID& sat,
                                          satTypeValueMap& data,
                                          SourceID& source )
   {

         // Compute current variance
      variance = sbData[ sat ].qprime
                 * std::abs( sbData[ sat ].currentTime
                           - sbData[ sat ].previousTime );

      return;

   }  // End of method 'SatBiasRandomWalkModel::computeQ()'



      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for ISB is set to 
	   * 3e-4 m*m/s (equivalent to about 1.0 m*m/h).
       *
	   * \Warning: This function should be used after prepare, or ISBmData
	   * will be blank and default qprime is set.
       */
   ISBRandomWalkModel& ISBRandomWalkModel::setQprime(double qp)
   {

         // Look at each source being currently managed
      for( std::map<SourceID, ISBData>::iterator it = ISBmData.begin();
           it != ISBmData.end();
           ++it )
      {
            // Assign new process spectral density value
         (*it).second.qprime = qp;
      }

      return (*this);

   }  // End of method 'ISBRandomWalkModel::setQprime()'





      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void ISBRandomWalkModel::Prepare( const SatID& sat,
                                       gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );

         // Second, let's update current epoch for this source
      setCurrentTime(source, gData.header.epoch );

         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, ISBmData[source].currentTime);

      return;

   }  // End of method 'ISBRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void ISBRandomWalkModel::computeQ( const SatID& sat,
                                        satTypeValueMap& data,
                                        SourceID& source )
   {

         // Compute current variance
      variance = ISBmData[ source ].qprime
                 * std::abs( ISBmData[ source ].currentTime
                           - ISBmData[ source ].previousTime );

      return;

   }  // End of method 'ISBRandomWalkModel::computeQ()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for IFCB is set to 
	   * 3e-4 m*m/s (equivalent to about 1.0 m*m/h).
       *
	   * \Warning: This function should be used after prepare, or IFCBmData
	   * will be blank and default qprime is set.
       */
   IFCBRandomWalkModel& IFCBRandomWalkModel::setQprime(double qp)
   {

	   // Look at each source being currently managed
	   for( std::map<SourceID, std::map<SatID, IFCBData> >::iterator it = IFCBmData.begin();
		   it != IFCBmData.end();
		   ++it )
	   {
		   // For each source, then loop all satellites
		   for( std::map<SatID, IFCBData >::iterator it2 = (*it).second.begin();
			   it2 != (*it).second.end();
			   ++it2 )
		   {
			   // Assign new process spectral density value
			   (*it2).second.qprime = qp;
		   }

	   }

	   return (*this);

   }  // End of method 'IFCBRandomWalkModel::setQprime()'





      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void IFCBRandomWalkModel::Prepare( const SatID& sat,
                                       gnssRinex& gData )
   {

         // First, get current source
      SourceID source( gData.header.source );
      // added by ragnwang to calculate the process noise.
      if (sats.find(sat)==sats.end())
      {
          setFirstTime(source, sat, gData.header.epoch);
          sats.insert(sat);
      }
         // Second, let's update current epoch for this source
      setCurrentTime(source, sat, gData.header.epoch );

         // Third, compute Q value
      computeQ(sat, gData.body, source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, sat, IFCBmData[source][sat].currentTime);

      return;

   }  // End of method 'IFCBRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void IFCBRandomWalkModel::computeQ( const SatID& sat,
                                        satTypeValueMap& data,
                                        SourceID& source )
   {

         // Compute current variance
      variance += IFCBmData[ source ][sat].qprime
		  * std::abs( IFCBmData[ source ][sat].currentTime
		  - IFCBmData[ source ][sat].previousTime );

      return;

   }  // End of method 'IFCBRandomWalkModel::computeQ()'


   IFBRandomWalkModel& IFBRandomWalkModel::setQprime(double qp)
   {

       // Look at each source being currently managed
       for (std::map<SourceID, std::map<SatID, IFBData> >::iterator it = IFBmData.begin();
           it != IFBmData.end();
           ++it)
       {
           // For each source, then loop all satellites
           for (std::map<SatID, IFBData >::iterator it2 = (*it).second.begin();
               it2 != (*it).second.end();
               ++it2)
           {
               // Assign new process spectral density value
               (*it2).second.qprime = qp;
           }

       }

       return (*this);

   }  // End of method 'IFBRandomWalkModel::setQprime()'





      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void IFBRandomWalkModel::Prepare(const SatID& sat,
       gnssRinex& gData)
   {

       // First, get current source
       SourceID source(gData.header.source);
       // added by ragnwang to calculate the process noise.
       if (sats.find(sat) == sats.end())
       {
           setFirstTime(source, sat, gData.header.epoch);
           sats.insert(sat);
       }
       // Second, let's update current epoch for this source
       setCurrentTime(source, sat, gData.header.epoch);

       // Third, compute Q value
       computeQ(sat, gData.body, source);

       // Fourth, prepare for next iteration updating previous epoch
       setPreviousTime(source, sat, IFBmData[source][sat].currentTime);

   }  // End of method 'IFCBRandomWalkModel::Prepare()'

   // IFB random walk
   void IFBRandomWalkModel::computeQ(const SatID& sat,
	   satTypeValueMap& data,
	   SourceID& source)
   {

	   // Compute current variance
	   variance += IFBmData[source][sat].qprime
		   * std::abs(IFBmData[source][sat].currentTime
			   - IFBmData[source][sat].previousTime);

   }  // End of method 'IFBRandomWalkModel::computeQ()'




}  // End of namespace gpstk
