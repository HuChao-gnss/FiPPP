#pragma ident "$Id$"

/**
* @file ComputeWindUp.cpp
* This class computes the wind-up effect on the phase observables, in radians.
*/

//============================================================================
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
//
//============================================================================


#include "ComputeWindUp.hpp"
#include "FreqConsts.hpp"

using namespace std;

using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{

	// Returns a string identifying this object.
	std::string ComputeWindUp::getClassName() const
	{ return "ComputeWindUp"; }

    void ComputeWindUp::Init()
    {
        TypeIDSet typeSetGPS;
        typeSetGPS.insert(TypeID::L1G);
        typeSetGPS.insert(TypeID::L2G);
        typeSetGPS.insert(TypeID::L5G);
                                     
        TypeIDSet typeSetGAL;        
        typeSetGAL.insert(TypeID::L1E);
        typeSetGAL.insert(TypeID::L5E);
        typeSetGAL.insert(TypeID::L7E);
        typeSetGAL.insert(TypeID::L8E);
        typeSetGAL.insert(TypeID::L6E);
                                     
        TypeIDSet typeSetGLO;        
        typeSetGLO.insert(TypeID::L1R);
        typeSetGLO.insert(TypeID::L2R);
        typeSetGLO.insert(TypeID::L3R);
        typeSetGLO.insert(TypeID::L4R);
        typeSetGLO.insert(TypeID::L6R);
                                     
        TypeIDSet typeSetBDS;        
        typeSetBDS.insert(TypeID::L1C);
        typeSetBDS.insert(TypeID::L2C);
        typeSetBDS.insert(TypeID::L5C);
        typeSetBDS.insert(TypeID::L6C);
        typeSetBDS.insert(TypeID::L7C);
        typeSetBDS.insert(TypeID::L8C);
                                     
        TypeIDSet typeSetQZS;        
        typeSetQZS.insert(TypeID::L1J);
        typeSetQZS.insert(TypeID::L2J);
        typeSetQZS.insert(TypeID::L5J);
        typeSetQZS.insert(TypeID::L6J);


        sysTypes[SatelliteSystem::GPS] = typeSetGPS;
        sysTypes[SatelliteSystem::Galileo] = typeSetGAL;
        sysTypes[SatelliteSystem::Glonass] = typeSetGLO;
        sysTypes[SatelliteSystem::BeiDou] = typeSetBDS;
        sysTypes[SatelliteSystem::QZSS] = typeSetQZS;
    };


	/* Returns a satTypeValueMap object, adding the new data generated when
	 * calling this object.
	 *
	 * @param time      Epoch corresponding to the data.
	 * @param gData     Data object holding the data.
	 */
	satTypeValueMap& ComputeWindUp::Process( const CommonTime& time,
		satTypeValueMap& gData )
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
			for ( satTypeValueMap::iterator it = gData.begin();
				it != gData.end();
				++it )
			{

				// First check if this satellite has previous arc information
				if( satPhaseData.find( (*it).first ) == satPhaseData.end() )
				{
					// If it doesn't have an entry, insert one
					satPhaseData[ (*it).first ].arcNum = 0.0;
				};

				// Then, check both if there is arc information, and if current
				// arc number is different from arc number in storage (which
				// means a cycle slip happened)
				if ( (*it).second.find(TypeID::satArc) != (*it).second.end() &&
					(*it).second(TypeID::satArc) != satPhaseData[ (*it).first ].arcNum )
				{
					// If different, update satellite arc in storage
					satPhaseData[ (*it).first ].arcNum = (*it).second(TypeID::satArc);

					// Reset phase information
					satPhaseData[ (*it).first ].previousPhaseStation = 0.0;
					satPhaseData[ (*it).first ].previousPhaseSat = 0.0;

				}


				// Use ephemeris if satellite position is not already computed
				if( ( (*it).second.find(TypeID::satXECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satYECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satZECEF) == (*it).second.end() ) )
				{

					if(pEphemeris==NULL)
					{

						// If ephemeris is missing, then remove all satellites
						satRejectedSet.insert( (*it).first );

                        cerr << "ComputeWindUp:: ephemeris is NULL!" << endl;
                        exit(-1);
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

                TypeIDSet typeSet;
                typeSet = sysTypes[(*it).first.system];

                double windUp = getWindUp((*it).first, time, svPos, sunPos);

                for(auto typeIt = typeSet.begin(); typeIt!=typeSet.end(); typeIt++)
                {
                    int n = asInt( (*typeIt).asString().substr(1,1) );

                    if(debug)
                    {
                        cout << getClassName() << "frequency:" << n << endl;
                    }

                    double wavelength;
                    if((*it).first.system == SatelliteSystem::Glonass)
                    {
                        if( n ==1 )
                        {
                            wavelength = (*it).second[TypeID::wavelengthL1R];
                        }
                        else if(n==2)
                        {
                            wavelength = (*it).second[TypeID::wavelengthL2R];
                        }
                        else
                        {
                            wavelength = getWavelength((*it).first.system, n);
                        }
                    }
                    else
                    {
                        wavelength = getWavelength((*it).first.system, n);
                    }

                    double temp = wavelength/TWO_PI * windUp; 

                    string windUpTypeStr = "windUp" + (*typeIt).asString();
                    TypeID windUpType = TypeID(windUpTypeStr);


                    if(debug)
                    {
                        cout << "freq:" << n 
                             << "wavelength:" << wavelength  
                             << "windUp:" << windUp 
                             << "temp:" << temp
                             << endl;
                    }

                    // insert windUp into gData
                    (*it).second[windUpType] = temp;
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

	}  // End of method 'ComputeWindUp::Process()'





	/* Compute the value of the wind-up, in radians.
	* @param sat       Satellite IDmake
	* @param time      Epoch of interest
	* @param satpos    Satellite position, as a Triple
	* @param sunpos    Sun position, as a Triple
	*
	* @return Wind-up computation, in radians
	*/
	double ComputeWindUp::getWindUp( const SatID& satid,
		const CommonTime& time,
		const Triple& sat,
		const Triple& sunPosition )
	{

		// Get satellite rotation angle

        if(nominalPos.X()==0.0 && 
           nominalPos.Y()==0.0 && 
           nominalPos.Z()==0.0)
        {
            Exception e("nominalPos must be given!");
            GPSTK_THROW(e);
        }

		// Get vector from Earth mass center to receiver
		Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

		// Vector from SV to Sun center of mass
		Triple gps_sun( sunPosition-sat );

		// satellite-fixed system
		Triple ri(0.0,0.0,0.0), rj(0.0,0.0,0.0), rk(0.0,0.0,0.0);
		//Triple ri2(0.0,0.0,0.0), rj2(0.0,0.0,0.0), rk2(0.0,0.0,0.0);

		if(peclipse != NULL) // from eclipse
		{
			// Unitary vector from satellite to Earth mass center (ECEF)
			rk = (-1.0)*(sat.unitVector());

			// ri = rj x rk (ECEF)
			ri = (peclipse->getSatBodyX(satid)).unitVector();

			// rj = rk x ri: Rotation axis of solar panels (ECEF)
			rj = (rk.cross(ri)).unitVector();
		}
		else // nominal attitude
		{
			// Define rk: Unitary vector from satellite to Earth mass center
			rk = ( ( (-1.0)*(sat.unitVector()) ) );

			// Define rj: rj = rk x gps_sun, then make sure it is unitary
			rj = ( (rk.cross(gps_sun)).unitVector() );

			// Define ri: ri = rj x rk, then make sure it is unitary
			// Now, ri, rj, rk form a base in the satellite body reference
			// frame, expressed in the ECEF reference frame
			ri = ( (rj.cross(rk)).unitVector() );
		}

		// Compute unitary vector vector from satellite to RECEIVER
		Triple rrho( (rxPos-sat).unitVector() );

		// Projection of "rk" vector to line of sight vector (rrho)
		double zk(rrho.dot(rk));

		// Get a vector without components on rk (i.e., belonging
		// to ri, rj plane)
		Triple dpp(rrho-zk*rk);

		// Compute dpp components in ri, rj plane
		double xk(dpp.dot(ri));
		double yk(dpp.dot(rj));

		// Compute satellite rotation angle, in radians
		double alpha1(std::atan2(yk,xk));


		// Get receiver rotation angle

		// Redefine rk: Unitary vector from Receiver to Earth mass center
		rk = (-1.0)*(rxPos.unitVector());

		// Let's define a NORTH unitary vector in the Up, East, North
		// (UEN) topocentric reference frame
		Triple delta(0.0, 0.0, 1.0);

		// Rotate delta to XYZ reference frame
		delta =
			(delta.R2(nominalPos.geodeticLatitude())).R3(-nominalPos.longitude());


		// Computation of reference trame unitary vectors for receiver
		// rj = rk x delta, and make it unitary
		rj = (rk.cross(delta)).unitVector();

		// ri = rj x rk, and make it unitary
		ri = (rj.cross(rk)).unitVector();

		// Projection of "rk" vector to line of sight vector (rrho)
		zk = rrho.dot(rk);

		// Get a vector without components on rk (i.e., belonging
		// to ri, rj plane)
		dpp = rrho-zk*rk;

		// Compute dpp components in ri, rj plane
		xk = dpp.dot(ri);
		yk = dpp.dot(rj);

		// Compute receiver rotation angle, in radians
		double alpha2(std::atan2(yk,xk));

		double wind_up(0.0);

		alpha1 = alpha1 + wind_up;

		double da1(alpha1-satPhaseData[satid].previousPhaseSat);

		double da2(alpha2-satPhaseData[satid].previousPhaseStation);

		// Let's avoid problems when passing from 359 to 0 degrees.
		satPhaseData[satid].previousPhaseSat += std::atan2( std::sin(da1),
			std::cos(da1) );

		satPhaseData[satid].previousPhaseStation += std::atan2( std::sin(da2),
			std::cos(da2) );

		// Compute wind up effect in radians
		wind_up = satPhaseData[satid].previousPhaseSat - 
                  satPhaseData[satid].previousPhaseStation;

		return wind_up;

	}  // End of method 'ComputeWindUp::getWindUp()'


    void ComputeWindUp::Process(gnssRinex& gData)
        noexcept(false)
    {
        Process( gData.header.epoch, gData.body );

    }  // End of method 'ComputeWindUp::Process()'

}  // End of namespace gpstk
