#pragma ident "$Id$"

/**
 * @file CorrectRecBias.cpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
 */

#include "CorrectRecBias.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

    // Return a string identifying this object.
    std::string CorrectRecBias::getClassName() const
    { return "CorrectRecBias"; }


      /* Return a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& CorrectRecBias::Process( const CommonTime& time,
                                                  satTypeValueMap& gData )
        noexcept(false)
    {

        try
        {

            // Compute station latitude and longitude
            double lat( nominalPos.geodeticLatitude() );
            double lon( nominalPos.longitude() );

            // Define station position as a Triple, in ECEF
            Triple rcvPos( nominalPos.getX(),
                           nominalPos.getY(),
                           nominalPos.getZ() );
            //test
            if(tide[0] == 0.0 &&
                    tide[1] == 0.0&&
                    tide[2] == 0.0 )
            {
                Exception e(getClassName() + " tide is zero");
                GPSTK_THROW(e);
            }

            // Compute initial displacement vectors, in meters [UEN]
            Triple initialBias( tide + monument );

            if(debug)
            {
               cout << "tide:" << endl;
               cout << tide << endl;
               cout << "monument:" << endl;
               cout << monument << endl;
            }

            Triple dispL1( initialBias );
            Triple dispL2( initialBias );
            Triple dispL5( initialBias );
            Triple dispL6( initialBias );
            Triple dispL7( initialBias );
            Triple dispL8( initialBias );
            Triple dispL9( initialBias );


            // Define a Triple that will hold satellite position, in ECEF
            Triple satPos(0.0, 0.0, 0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat( it->first );

                 /// Position of antenna L1 phase center with respect to ARP ([UEN]).
                Triple L1PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L2PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L5PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L6PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L7PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L8PhaseCenter( 0.0, 0.0, 0.0 );
                Triple L9PhaseCenter( 0.0, 0.0, 0.0 );

                Triple zeroTrip(0.0, 0.0, 0.0);

                // Check if we have a valid Antenna object
                if( antenna.isValid() )
                {
                    // Compute phase center offsets
                    if( sat.system == SatelliteSystem::GPS )
                    {
                        try
                        {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::G01 );
                        }
                        catch(Exception& e)
                        {
                            L1PhaseCenter = zeroTrip;
                        }

                        try
                        {
                            L2PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::G02 );
                        }
                        catch(Exception& e)
                        {
							L2PhaseCenter
								= zeroTrip;
                        }

                        try
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::G05 );
                        }
                        catch(Exception& e)
                        {
                            L5PhaseCenter
								= antenna.getAntennaEccentricity(Antenna::G02);
                        }
                    }
                    else if( sat.system == SatelliteSystem::Glonass )
                    {
                        try
                        {
                        L1PhaseCenter 
                            = antenna.getAntennaEccentricity( Antenna::R01 );
                        }
                        catch(Exception& e)
                        {
                            L1PhaseCenter = zeroTrip;
                        }

                        try
                        {
                            L2PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::R02 );
                        }
                        catch(Exception& e)
                        {
                            L2PhaseCenter = zeroTrip;
                        }
                    }
                    else if( sat.system == SatelliteSystem::Galileo )
                    {
                        try
                        {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::E01 );
                        }
                        catch(Exception& e)
                        {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::E05 );
                        }
                        catch(Exception& e)
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L7PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::E07 );
                        }
                        catch(Exception& e)
                        {
                            L7PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L8PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::E08 );
                        }
                        catch(Exception& e)
                        {
                            L8PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try 
                        {
                            L6PhaseCenter 
                                = antenna.getAntennaEccentricity( Antenna::E06 );
                        }
                        catch(Exception& e) 
                        {
                            L6PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                    }
                    else if( sat.system == SatelliteSystem::QZSS )
                    {
                        try {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::J01 );
                        }
                        catch(Exception& e) {
                            L1PhaseCenter = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try {
                            L2PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::J02 );
                        }
                        catch(Exception& e) {
                            L2PhaseCenter = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::J05 );
                        }
                        catch(Exception& e) {
                            L5PhaseCenter = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try {
                            L6PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::J06 );
                        }
                        catch(Exception& e) {
                            L6PhaseCenter = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                    }
                    else if( sat.system == SatelliteSystem::BeiDou )
                    {
                        try
                        {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::C01 );
                        }
                        catch(Exception& e)
                        {
                            L1PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L2PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::C02 );
                        }
                        catch(Exception& e)
                        {
                            L2PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try
                        {
                            L6PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::C06 );
                        }
                        catch(Exception& e)
                        {
                            L6PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::C05 );
                        }
                        catch(Exception& e)
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }

                        try
                        {
                            L7PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::C07 );
                        }
                        catch(Exception& e)
                        {
                            L7PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                        try
                        {
                            L8PhaseCenter
                                = antenna.getAntennaEccentricity(Antenna::C08);
                        }
                        catch (Exception& e)
                        {
                            L8PhaseCenter
                                = antenna.getAntennaEccentricity(Antenna::G02);
                        }
                    }
                    else if( sat.system == SatelliteSystem::IRNSS )
                    {
                        try
                        {
                            L5PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::I05 );
                            L9PhaseCenter 
                                = antenna.getAntennaEccentricity(Antenna::I09 );
                        }
                        catch(Exception& e)
                        {
                            L5PhaseCenter = zeroTrip;
                            L7PhaseCenter = zeroTrip;
                        }
                    }
                }


                if( ( (*it).second.find(TypeID::satXECEF) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satYECEF) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satZECEF) == (*it).second.end() ) )
                {
                    satRejectedSet.insert( sat );
                    continue;
                }
                else
                {
                    // Get satellite position out of GDS
                    satPos[0] = (*it).second[TypeID::satXECEF];
                    satPos[1] = (*it).second[TypeID::satYECEF];
                    satPos[2] = (*it).second[TypeID::satZECEF];
                }


                // Declare the variables where antenna PC variations
                // will be stored.
                Triple L1Var( 0.0, 0.0, 0.0 );
                Triple L2Var( 0.0, 0.0, 0.0 );
                Triple L5Var( 0.0, 0.0, 0.0 );
                Triple L6Var( 0.0, 0.0, 0.0 );
                Triple L7Var( 0.0, 0.0, 0.0 );
                Triple L8Var( 0.0, 0.0, 0.0 );
                Triple L9Var( 0.0, 0.0, 0.0 );

                // because of the GAL and BDS dont have PCV correction data.
                // we use the GPS second frequency PCV correction data. ragnwang 2023-05
                // Check if we have a valid Antenna object
                if( antenna.isValid() )
                {
                    // Check if we have elevation information
                    if( (*it).second.find(TypeID::elevation) != (*it).second.end() )
                    {

                        // Get elevation value
                        double elev( (*it).second[TypeID::elevation] );

                        // Check if azimuth is also required
                        if( !useAzimuth )
                        {
                            // In this case, use methods that only need elevation
                            try
                            {
                                // Compute phase center variation values
                                if( sat.system == SatelliteSystem::GPS )
                                {
                                    try
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::G01, 
                                           elev );
                                    }
                                    catch(Exception& e)
                                    {
                                        L1Var = zeroTrip;
                                    }
                                    try 
                                    {
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::G02, 
                                           elev );
                                    }
                                    catch(Exception& e)
                                    {
                                        L2Var = zeroTrip;
                                    }
                                    try 
                                    {
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::G05, 
                                            elev );
                                    }
                                    catch(Exception& e)
                                    {
										L5Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                }
                                else if( sat.system == SatelliteSystem::Glonass )
                                {
                                    try 
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::R01, 
                                            elev );
                                    }
                                    catch(Exception& e)
                                    {
                                        L1Var = zeroTrip;
                                    }
                                    try 
                                    {
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::R02, 
                                            elev );
                                    }
                                    catch(Exception& e)
                                    {
                                        L2Var = zeroTrip;
                                    }
                                }
                                else if( sat.system == SatelliteSystem::Galileo ) // no PCV available
                                {
                                    try 
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::E01, 
                                            elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L1Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                    try 
                                    {
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::E05, 
                                                elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L5Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                    try
                                    {
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::E06, 
                                                                        elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L6Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }

                                    try 
                                    {
                                        L7Var 
                                        = antenna.getAntennaPCVariation(Antenna::E07, 
                                            elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L7Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                    try 
                                    {
                                        L8Var
                                        = antenna.getAntennaPCVariation(Antenna::E08, 
                                            elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L8Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                }
                                else if( sat.system == SatelliteSystem::BeiDou ) // no PCV available
                                {
                                    try 
                                    {
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::C02, 
                                            elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L2Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                    try 
                                    {
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::C06, 
                                             elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L6Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                    try 
                                    {
                                        L7Var 
                                        = antenna.getAntennaPCVariation(Antenna::C07, 
                                            elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L7Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                }
                                else if( sat.system == SatelliteSystem::QZSS ) // no PCV available
                                {
                                    try 
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::J01, 
                                                elev );
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::J02, 
                                                elev );
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::J05, 
                                                elev );
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::J06, 
                                                elev );
                                    }
                                    catch(Exception& e)
                                    {
										L1Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
										L2Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
										L5Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
										L6Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }

                                }
                                else if( sat.system == SatelliteSystem::IRNSS ) // no PCV available
                                {
                                    try 
                                    { 
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::I05, 
                                                                        elev );
                                        L9Var 
                                        = antenna.getAntennaPCVariation(Antenna::I09, 
                                                                        elev );
                                    }
                                    catch(Exception& e) 
                                    {
										L5Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
										L9Var = antenna.getAntennaPCVariation(Antenna::G02,
											elev);
                                    }
                                }
                            }
                            catch(InvalidRequest& ir)
                            {
                                // Throw an exception if something unexpected
                                // happens
                                ProcessingException e( getClassName() + ":"
                                        + "Unexpected problem found when trying to "
                                        + "compute antenna offsets" );

                                GPSTK_THROW(e);
                            }
                        }
                        else
                        {
                            // Check if we have azimuth information
                            if( (*it).second.find(TypeID::azimuth) !=
                                                            (*it).second.end() )
                            {

                                // Get azimuth value
                                double azim( (*it).second[TypeID::azimuth] );

                                // Use a gentle fallback mechanism to get antenna
                                // phase center variations
                                try
                                {
                                    // Compute phase center variation values
                                    if( sat.system == SatelliteSystem::GPS )
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::G01, 
                                                                        elev, azim );
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::G02, 
                                                                        elev, azim );
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::G05, 
                                                                        elev, azim );
                                    }
                                    else if( sat.system == SatelliteSystem::Glonass )
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::R01, 
                                                                        elev, azim );
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::R02, 
                                                                        elev, azim );
                                    }
                                    else if( sat.system == SatelliteSystem::Galileo ) // no PCV available
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::E01, 
                                                                        elev, azim );
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::E05, 
                                                                        elev, azim );
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::E06, 
                                                                        elev, azim );
                                        L7Var 
                                        = antenna.getAntennaPCVariation(Antenna::E07, 
                                                                        elev, azim );
                                        L8Var 
                                        = antenna.getAntennaPCVariation(Antenna::E08, 
                                                                        elev, azim );
                                    }
                                    else if( sat.system == SatelliteSystem::QZSS ) // no PCV available
                                    {
                                        L1Var 
                                        = antenna.getAntennaPCVariation(Antenna::J01, 
                                                                        elev, azim );
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::J02, 
                                                                        elev, azim );
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::J05, 
                                                                        elev, azim );
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::J06, 
                                                                        elev, azim );
                                    }
                                    else if( sat.system == SatelliteSystem::BeiDou ) // no PCV available
                                    {
                                        L2Var 
                                        = antenna.getAntennaPCVariation(Antenna::C02, 
                                                                        elev, azim );
                                        L6Var 
                                        = antenna.getAntennaPCVariation(Antenna::C06, 
                                                                        elev, azim );
                                        L7Var 
                                        = antenna.getAntennaPCVariation(Antenna::C07, 
                                                                        elev, azim );
                                    }
                                    else if( sat.system == SatelliteSystem::IRNSS ) // no PCV available
                                    {
                                        L5Var 
                                        = antenna.getAntennaPCVariation(Antenna::I05, 
                                                                        elev, azim );
                                        L9Var 
                                        = antenna.getAntennaPCVariation(Antenna::I09, 
                                                                        elev, azim );
                                    }
                                }
                                catch(InvalidRequest& ir)
                                {
                                    // We  "graceful degrade" to a simpler mechanism
                                    try
                                    {
                                        // Compute phase center variation values
                                        if( sat.system == SatelliteSystem::GPS )
                                        {
                                            L1Var 
												= antenna.getAntennaPCVariation(Antenna::G01,
													elev, azim);
                                            L2Var
                                                = antenna.getAntennaPCVariation(Antenna::G02,
                                                    elev, azim);
                                            L5Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                        }
                                        else if( sat.system == SatelliteSystem::Glonass )
                                        {
                                            L1Var 
                                            = antenna.getAntennaPCVariation(Antenna::R01,
                                                                            elev );
                                            L2Var 
                                            = antenna.getAntennaPCVariation(Antenna::R02,
                                                                            elev );
                                        }
                                        else if( sat.system == SatelliteSystem::Galileo )
                                        {
                                            L1Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L5Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L6Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L7Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);

                                            L8Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                        }
                                        else if( sat.system == SatelliteSystem::QZSS )
                                        {
                                            L1Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L2Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L5Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                            L6Var 
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                        }
                                        else if( sat.system == SatelliteSystem::BeiDou )
                                        {
                                            L1Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
											L2Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
											L5Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
											L6Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
											L7Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
											L8Var
												= antenna.getAntennaPCVariation(Antenna::G02,
													elev, azim);
                                        }
                                        else if( sat.system == SatelliteSystem::IRNSS )
                                        {
                                            L5Var 
                                            = antenna.getAntennaPCVariation(Antenna::I05,
                                                                            elev );
                                            L9Var 
                                            = antenna.getAntennaPCVariation(Antenna::I09,
                                                                            elev );
                                        }
                                    }
                                    catch(InvalidRequest& ir)
                                    {
                                         L1Var = Triple(0.0, 0.0, 0.0);
                                         L2Var = Triple(0.0, 0.0, 0.0);
                                         L5Var = Triple(0.0, 0.0, 0.0);
                                         L6Var = Triple(0.0, 0.0, 0.0);
                                         L7Var = Triple(0.0, 0.0, 0.0);
                                         L8Var = Triple(0.0, 0.0, 0.0);
                                         L9Var = Triple(0.0, 0.0, 0.0);
                                    }
                                }
                            }
                            else
                            {
                                // Throw an exception if something unexpected happens
                                ProcessingException e( getClassName() + ":"
                                    + "Azimuth information could not be found, "
                                    + "so antenna PC offsets can not be computed");

                                GPSTK_THROW(e);

                            }  // End of 'if( (*it).second.find(TypeID::azimuth) !=...'

                        }  // End of 'if( !useAzimuth )'
                    }
                    else
                    {
                        // Throw an exception if there is no elevation data
                        ProcessingException e( getClassName() + ":"
                            + "Elevation information could not be found, "
                            + "so antenna PC offsets can not be computed" );

                        GPSTK_THROW(e);

                    }  // End of 'if( (*it).second.find(TypeID::elevation) != ...'

                }  // End of 'if( antenna.isValid() )...'


                // Compute vector station-satellite, in ECEF
                Triple ray(rcvPos - satPos);

                ray = ray.unitVector();

                // Rotate vector ray to UEN reference frame
                ray = (ray.R3(lon)).R2(-lat);

                // Convert ray to an unitary vector
                ray = ray.unitVector();

                // Update displacement vectors with current phase centers
                Triple dL1( dispL1 + L1PhaseCenter );
                Triple dL2( dispL2 + L2PhaseCenter );
                Triple dL5( dispL5 + L5PhaseCenter );
                Triple dL6( dispL6 + L6PhaseCenter );
                Triple dL7( dispL7 + L7PhaseCenter );
                Triple dL8( dispL8 + L8PhaseCenter );

                if(debug)
                {
                    cout << "sat:" << sat << endl;
                    cout << "L1PhaseCenter:" << L1PhaseCenter << endl;
                    cout << "L2PhaseCenter:" << L2PhaseCenter << endl;
                    cout << "L5PhaseCenter:" << L5PhaseCenter << endl;
                }

                // Compute corrections = displacement vectors components
                // along ray direction.
                double corr1( 0.0 );
                double corr2( 0.0 );
                double corr5( 0.0 );
                double corr6( 0.0 );
                double corr7( 0.0 );
                double corr8( 0.0 );
                // add minus to dL1.dot changed by ragnwang
                if(sat.system == SatelliteSystem::GPS)
                {
                    corr1 = dL1.dot(ray) + L1Var[0];
                    corr2 = dL2.dot(ray) + L2Var[0];
                    corr5 = dL5.dot(ray) + L5Var[0];

                    (*it).second[TypeID::rcvCorrC1G] = corr1 ;
                    (*it).second[TypeID::rcvCorrC2G] = corr2 ;
                    (*it).second[TypeID::rcvCorrC5G] = corr5 ;
                    (*it).second[TypeID::rcvCorrL1G] = corr1 ;
                    (*it).second[TypeID::rcvCorrL2G] = corr2 ;
                    (*it).second[TypeID::rcvCorrL5G] = corr5 ;
                }
                else if(sat.system == SatelliteSystem::Galileo)
                {
                    corr1 = dL1.dot(ray) + L1Var[0];
                    corr5 = dL5.dot(ray) + L5Var[0];
                    corr7 = dL7.dot(ray) + L7Var[0];
                    corr8 = dL8.dot(ray) + L8Var[0];
                    corr6 = dL6.dot(ray) + L6Var[0];

                    (*it).second[TypeID::rcvCorrC1E] = corr1 ;
                    (*it).second[TypeID::rcvCorrC5E] = corr5 ;
                    (*it).second[TypeID::rcvCorrC7E] = corr7 ;
                    (*it).second[TypeID::rcvCorrC8E] = corr8 ;
                    (*it).second[TypeID::rcvCorrC6E] = corr6 ;

                    (*it).second[TypeID::rcvCorrL1E] = corr1 ;
                    (*it).second[TypeID::rcvCorrL5E] = corr5 ;
                    (*it).second[TypeID::rcvCorrL7E] = corr7 ;
                    (*it).second[TypeID::rcvCorrL8E] = corr8 ;
                    (*it).second[TypeID::rcvCorrL6E] = corr6 ;

                }
                else if(sat.system == SatelliteSystem::Glonass)
                {
                    corr1 = dL1.dot(ray) + L1Var[0];
                    corr2 = dL2.dot(ray) + L2Var[0];

                    (*it).second[TypeID::rcvCorrC1R] = corr1 ;
                    (*it).second[TypeID::rcvCorrC2R] = corr2 ;
                    (*it).second[TypeID::rcvCorrL1R] = corr1 ;
                    (*it).second[TypeID::rcvCorrL2R] = corr2 ;

                }
                else if(sat.system == SatelliteSystem::BeiDou)
                {
                    corr5 = dL5.dot(ray) + L5Var[0];
                    corr7 = dL7.dot(ray) + L7Var[0];
                    corr6 = dL6.dot(ray) + L6Var[0];
                    corr2 = dL2.dot(ray) + L2Var[0];
                    corr1 = dL1.dot(ray) + L1Var[0];
                    corr8 = dL8.dot(ray) + L8Var[0];

                    (*it).second[TypeID::rcvCorrC5C] = corr5 ;
                    (*it).second[TypeID::rcvCorrC7C] = corr7 ;
                    (*it).second[TypeID::rcvCorrC6C] = corr6 ;
                    (*it).second[TypeID::rcvCorrC2C] = corr2 ;
                    (*it).second[TypeID::rcvCorrC1C] = corr1 ;
                    (*it).second[TypeID::rcvCorrC8C] = corr8 ;

                    (*it).second[TypeID::rcvCorrL5C] = corr5 ;
                    (*it).second[TypeID::rcvCorrL7C] = corr7 ;
                    (*it).second[TypeID::rcvCorrL6C] = corr6 ;
                    (*it).second[TypeID::rcvCorrL2C] = corr2 ;
                    (*it).second[TypeID::rcvCorrL1C] = corr1 ;
                    (*it).second[TypeID::rcvCorrL8C] = corr8 ;
                }
                else if(sat.system == SatelliteSystem::QZSS)
                {
                    corr1 = dL1.dot(ray) + L1Var[0];
                    corr2 = dL2.dot(ray) + L2Var[0];
                    corr5 = dL5.dot(ray) + L5Var[0];
                    corr6 = dL6.dot(ray) + L6Var[0];

                    (*it).second[TypeID::rcvCorrC1C] = corr1 ;
                    (*it).second[TypeID::rcvCorrC2C] = corr2 ;
                    (*it).second[TypeID::rcvCorrC5C] = corr5 ;
                    (*it).second[TypeID::rcvCorrC6C] = corr6 ;
                    (*it).second[TypeID::rcvCorrL1C] = corr1 ;
                    (*it).second[TypeID::rcvCorrL2C] = corr2 ;
                    (*it).second[TypeID::rcvCorrL5C] = corr5 ;
                    (*it).second[TypeID::rcvCorrL6C] = corr6 ;
                }

            } // End of 'for(satTypeValueMap::iterator = it, ...)'

            firstTime = false;

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'CorrectRecBias::Process()'

    void CorrectRecBias::Process(gnssRinex& gData)
        noexcept(false)
    {

        antModel = gData.header.antType ;

        if(debug)
        {
            cout << "antModel:" << antModel << endl;
        }

                // Must set antenna file first
        if(pAntexReader == NULL)
        {
            ProcessingException e("CorrectRecBias::No antenna file!");
            cout << e << endl;
            exit(-1);
        }

        try
        {
            antenna = pAntexReader->getAntenna(antModel);
        }
        catch(ObjectNotFound& notFound)
        {
            // new antenna model
            if(antModel.size() == 20)
            {
                antModel.replace(16,4,"NONE");
            }
            else
            {
                // Some stations has special format, so we just erase the
                // last 'NONE' and find the antenna.2017-04-17
                antModel.erase(antModel.size()-4,4);
            }

            // new receiver antenna with new antenna model
            try
            {
                antenna = pAntexReader->getAntenna( antModel );
            }
            catch(ObjectNotFound& notFound)
            {
                GPSTK_RETHROW(notFound);
            }
        }

        monument   = gData.header.antennaDeltaHEN;
        nominalPos = gData.header.antennaPosition;

        SourceID source(gData.header.source);
        string station = source.sourceName;

        if(pStaTides == NULL)
        {
            cerr << "pStaTides must be set" << endl;
            exit(-1);
        }

        pStaTides->setNominalPosition( nominalPos );
        tide = pStaTides->getTides( gData.header.epoch, station);

        Process( gData.header.epoch, gData.body );

    } // End of method 'CorrectRecBias::Process()'

}  // End of namespace gpstk
