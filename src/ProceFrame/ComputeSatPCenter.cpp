#pragma ident "$Id$"

/**
* @file ComputeSatPCenter.cpp
* This class computes the satellite antenna phase correction, in meters.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================


#include "DataStructures.hpp"
#include "ComputeSatPCenter.hpp"

using namespace std;

#define debug 0
namespace gpstk
{

	// Returns a string identifying this object.
	std::string ComputeSatPCenter::getClassName() const
	{ return "ComputeSatPCenter"; }


	/* Returns a satTypeValueMap object, adding the new data generated when
	* calling this object.
	*
	* @param time      Epoch corresponding to the data.
	* @param gData     Data object holding the data.
	*/
	satTypeValueMap& ComputeSatPCenter::Process(const CommonTime& time,
                                                std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
		                                        satTypeValueMap& gData)
		noexcept(false)
	{

		try
		{

			// Compute Sun position at this epoch
			SunPosition sunPosition;
			Triple sunPos(sunPosition.getPosition(time));

			// Define a Triple that will hold satellite position, in ECEF
			Triple svPos(0.0, 0.0, 0.0);

			SatIDSet satRejectedSet;

			// Loop through all the satellites
			satTypeValueMap::iterator it;
			for (it = gData.begin(); it != gData.end(); ++it)
			{

				// Use ephemeris if satellite position is not already computed
				if( ( (*it).second.find(TypeID::satXECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satYECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satZECEF) == (*it).second.end() ) )
				{

					if(pEphemeris==NULL)
					{

						// If ephemeris is missing, then remove all satellites
						satRejectedSet.insert( (*it).first );

						continue;
					}
					else
					{

						// Try to get satellite position
						// if it is not already computed
						try
						{
							// For our purposes, position at receive time
							// is fine enough
							Xvt svPosVel(pEphemeris->getXvt( (*it).first, time ));

							// If everything is OK, then continue processing.
							svPos[0] = svPosVel.x.theArray[0];
							svPos[1] = svPosVel.x.theArray[1];
							svPos[2] = svPosVel.x.theArray[2];

						}
						catch(...)
						{

							// If satellite is missing, then schedule it
							// for removal
							satRejectedSet.insert( (*it).first );

							continue;
						}

					}

				}
				else
				{

					// Get satellite position out of GDS
					svPos[0] = (*it).second[TypeID::satXECEF];
					svPos[1] = (*it).second[TypeID::satYECEF];
					svPos[2] = (*it).second[TypeID::satZECEF];

				}  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'

                TypeIDVec typeVec = satShortTypes[(*it).first];

                for(int i=0; i<typeVec.size(); i++)
                {
                    // 3-char type str, e.g. L1G/L2G/C1G/C2G...
                    std::string typeStr= typeVec[i].asString();

                    // convert typeStr to frequencyType
                    Antenna::frequencyType freqType;

                    if(      typeStr == "C1G" || typeStr == "L1G") freqType = Antenna::G01;
                    else if( typeStr == "C2G" || typeStr == "L2G") freqType = Antenna::G02;
                    else if( typeStr == "C5G" || typeStr == "L5G") freqType = Antenna::G05;
                    else if( typeStr == "C1E" || typeStr == "L1E") freqType = Antenna::E01;
                    else if( typeStr == "C5E" || typeStr == "L5E") freqType = Antenna::E05;
                    else if( typeStr == "C7E" || typeStr == "L7E") freqType = Antenna::E07;
                    else if( typeStr == "C8E" || typeStr == "L8E") freqType = Antenna::E08;
                    else if( typeStr == "C6E" || typeStr == "L6E") freqType = Antenna::E06;
                    else if( typeStr == "C1C" || typeStr == "L1C") freqType = Antenna::C01;
                    else if( typeStr == "C2C" || typeStr == "L2C") freqType = Antenna::C02;
                    else if( typeStr == "C5C" || typeStr == "L5C") freqType = Antenna::C05;
                    else if( typeStr == "C6C" || typeStr == "L6C") freqType = Antenna::C06;
                    else if( typeStr == "C7C" || typeStr == "L7C") freqType = Antenna::C07;
                    else if( typeStr == "C1R" || typeStr == "L1R") freqType = Antenna::R01;
                    else if( typeStr == "C2R" || typeStr == "L2R") freqType = Antenna::R02;

                    if(debug)
                    {
                        cout << (*it).first << ": freq:" << typeStr << endl;
                    }

				    // Let's get the satellite antenna phase correction value in
				    // meters, and insert it in the GNSS data structure.
                    try
                    {
                        double satPCenter = getSatPCenter((*it).first, freqType, time, svPos, sunPos);

                        // create satPCenterType for this typeStr
				        (*it).second[TypeID("satPCenter"+typeStr)] =satPCenter;
                    }
                    catch(InvalidAntex& e)
                    {
                        cout << e << endl;
                        cout << "remove sat:" << (*it).first << endl;
                        satRejectedSet.insert((*it).first);
                    }
                    catch(ObjectNotFound& e)
                    {
                        satRejectedSet.insert((*it).first);
                    }
                    catch(Exception& e)
                    {
                        cout << e << endl;
                        cout << typeStr << endl;
                        satRejectedSet.insert((*it).first);
                    };
                }


			}  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

			// Remove satellites with missing data
			gData.removeSatID(satRejectedSet);

			return gData;

		}
		catch(Exception& u)
		{

			// Throw an exception if something unexpected happens
			ProcessingException e( getClassName() + ":"
				+ u.what() );

			GPSTK_THROW(e);

		}

	}  // End of method 'ComputeSatPCenter::Process()'



	/* Compute the value of satellite antenna phase correction, in meters.
	* @param sat       Satellite ID
	* @param time      Epoch of interest
	* @param satpos    Satellite position, as a Triple
	* @param sunpos    Sun position, as a Triple
	*
	* @return Satellite antenna phase correction, in meters.
	*/
	double ComputeSatPCenter::getSatPCenter( const RinexSatID/*SatID*/& sat,
                                             const Antenna::frequencyType& freqType,
		                                     const CommonTime& time,
		                                     const Triple& satpos,
		                                     const Triple& sunPosition )
	{

        if(nominalPos.X()==0.0 && 
           nominalPos.Y()==0.0 && 
           nominalPos.Z()==0.0)
        {
            Exception e("nominalPos must be given!");
            GPSTK_THROW(e);
        }

		Triple ri(0.0,0.0,0.0), rj(0.0,0.0,0.0), rk(0.0,0.0,0.0);
		//Triple ri2(0.0,0.0,0.0), rj2(0.0,0.0,0.0), rk2(0.0,0.0,0.0);

		if(peclipse != NULL) // attitude from eclipse
		{

			// Unitary vector from satellite to Earth mass center (ECEF)
			rk = (-1.0)*(satpos.unitVector());

			// ri = rj x rk (ECEF)
			ri = (peclipse->getSatBodyX(sat)).unitVector();

			// rj = rk x ri: Rotation axis of solar panels (ECEF)
			rj = (rk.cross(ri)).unitVector();

		}
		else // always nominal attitude
		{
			// Unitary vector from satellite to Earth mass center (ECEF)
			rk = ( (-1.0)*(satpos.unitVector()) ) ;

			// Unitary vector from Earth mass center to Sun (ECEF)
			ri =  sunPosition.unitVector() ;

			// rj = rk x ri: Rotation axis of solar panels (ECEF)
			rj = (rk.cross(ri)).unitVector(); // Must unit vector, 2017-11-02

			// Redefine ri: ri = rj x rk (ECEF)
			ri = rj.cross(rk);

			// Let's convert ri to an unitary vector. (ECEF)
			ri = ri.unitVector();

		}

		// Get vector from Earth mass center to receiver
		Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

		// Compute unitary vector vector from satellite to RECEIVER
		Triple rrho( (rxPos-satpos).unitVector() );

		// When not using Antex information, if satellite belongs to block
		// "IIR" its correction is 0.0, else it will depend on satellite model.

		// This variable that will hold the correction, 0.0 by default
		double svPCcorr(0.0);

		// Check is Antex antenna information is available or not, and if
		// available, whether satellite phase center information is absolute
		// or relative
		if( pAntexReader == NULL )
		{
            Exception e(getClassName() + "pAntexReader is NULL"); 
            GPSTK_THROW(e);
		}

		// We will need the elevation, in degrees. It is found using
		// dot product and the corresponding unitary angles
		double nadir = std::acos( rrho.dot(rk) ) * RAD_TO_DEG;

		// Different satellite system has different nadir angle:
		// GPS:14.0 for BLOCK I(all retired), 17.0 for else
		// GLONASS: 15.0 for all
		// GALILEO: 13.0 for all
		// BDS: 9.0 for GEO/IGSO, 14.0 for MEO(13.0 of ESA)
		// QZSS: 10.0 for all
		// IRNSS: 14 for all

        try
        {
            if(debug)
            {
                cout << "satStr:"<< sat.toString() << endl;
            }

		    // Get satellite antenna information out of AntexReader object
		    Antenna antenna( pAntexReader->getAntenna( sat.toString(), time ) );

		    // get the largest nadir angle for GPS
		    int max = antenna.getAntennaNoAziMap()[freqType].size() - 1;
            if(max > 0)
            {
		        nadir = ( nadir > max) ? max: nadir;
            }
		    double elev(90.0 - nadir);

            Triple satAnt, var;

            string antType( antenna.getAntennaType() );
            try
            {
		        // Get antenna eccentricity for freqType , in satellite reference system.
		        // NOTE: It is NOT in ECEF, it is in UEN!!!
		        satAnt = ( antenna.getAntennaEccentricity( freqType ) );
		        // Now, get the phase center variation(only elevation).
		        var = ( antenna.getAntennaPCVariation( freqType, elev) );

                if(debug)
                {
                    cout << "satAnt:" << satAnt << endl;
                    cout << "var:" << var<< endl;
                }
            }
            catch(InvalidRequest& e)
            {
				// we use the second frequency of GPS to correct others, chao hu/ragnwang
                if(freqType==Antenna::G05)
                {
		            int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
                    if(max > 0)
                    {
		                nadir = ( nadir > max) ? max: nadir;
                    }
		            double elev(90.0 - nadir);

		            satAnt = ( antenna.getAntennaEccentricity( Antenna::G02) );
		            var = ( antenna.getAntennaPCVariation( Antenna::G02, elev) );

                    if(debug)
                    {
                        cout << "satAnt:" << satAnt << endl;
                        cout << "var:" << var<< endl;
                    }
                }
                else if(freqType==Antenna::C01)
                {
		            int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
                    if(max > 0)
                    {
		                nadir = ( nadir > max) ? max: nadir;
                    }
		            double elev(90.0 - nadir);

		            satAnt = ( antenna.getAntennaEccentricity( Antenna::G02) );
		            var = ( antenna.getAntennaPCVariation( Antenna::G02, elev) );

                    if(debug)
                    {
                        cout << "satAnt:" << satAnt << endl;
                        cout << "var:" << var<< endl;
                    }
                }
				else if (freqType == Antenna::C05)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				else if (freqType == Antenna::C06)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				else if (freqType == Antenna::C08)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				else if (freqType==Antenna::C02)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				else if (freqType == Antenna::C07)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				// galileo
				else if (freqType == Antenna::E01)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				// galileo
				else if (freqType == Antenna::E05)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				// galileo
				else if (freqType == Antenna::E07)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				// galileo
				else if (freqType == Antenna::E08)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
				// galileo
				else if (freqType == Antenna::E06)
				{
					int max = antenna.getAntennaNoAziMap()[Antenna::G02].size() - 1;
					if (max > 0)
					{
						nadir = (nadir > max) ? max : nadir;
					}
					double elev(90.0 - nadir);

					satAnt = (antenna.getAntennaEccentricity(Antenna::G02));
					var = (antenna.getAntennaPCVariation(Antenna::G02, elev));

					if (debug)
					{
						cout << "satAnt:" << satAnt << endl;
						cout << "var:" << var << endl;
					}
				}
            }

		    // Change to ECEF(only PCO)
		    Triple svAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

            if(debug)
            {
                cout << "svAntenna:" << svAntenna << endl;
            }

		    // Projection of "svAntenna" vector to line of sight vector rrho
		    svPCcorr =  (rrho.dot(svAntenna)) - var[0];

        }
        catch(InvalidRequest& e)
        {
            svPCcorr = 0.0;
        }
        catch(InvalidAntex& e)
        {
            GPSTK_RETHROW(e);
        }
        catch(ObjectNotFound& e)
        {
            GPSTK_RETHROW(e);
        }

		// This correction is interpreted as an "advance" in the signal,
		// instead of a delay. Therefore, it has negative sign
		return (-svPCcorr);

	}  // End of method 'ComputeSatPCenter::getSatPCenter()'


    void ComputeSatPCenter::Process(gnssRinex& gData)
        noexcept(false)
    {
        Process( gData.header.epoch, gData.header.satShortTypes, gData.body );

    }  // End of method 'ComputeWindUp::Process()'



}  // End of namespace gpstk
