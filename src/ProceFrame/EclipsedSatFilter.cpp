#pragma ident "$Id$"
/**
* @file EclipsedSatFilter.cpp
* This class caculates the satellite attitude.
*/


#include "EclipsedSatFilter.hpp"

using namespace std;

namespace gpstk
{

	// Returns a string identifying this object.
	std::string EclipsedSatFilter::getClassName() const
	{ return "EclipsedSatFilter"; }


	// Max yaw rate for GPS BLOCK II/IIA satellites( 1996-2008 JPL average solutions)
	// unit in °/s
	static const double yawRateForGPSIIA[32] = 
	{
		0.1211, 0.1339, 0.1230, 0.1233, 0.1180, 0.1266, 0.1269, 0.1033,
		0.1278, 0.0978, 0.2000, 0.1990, 0.2000, 0.0815, 0.1303, 0.0838,
		0.1401, 0.1069, 0.0980, 0.1030, 0.1366, 0.1025, 0.1140, 0.1089,
		0.1001, 0.1227, 0.1194, 0.1260, 0.1228, 0.1165, 0.0969, 0.1140
	};

	// Max yaw rate for GPS BLOCK IIR satellites
	// unit in °/s
	static const double yawRateForGPSIIR = 0.20;

	// Max yaw rate for GPS BLOCK IIF satellites
	// unit in °/s
	static const double yawRateForGPSIIF = 0.11;

	// Max yaw rate for GLONASS-M satellites
	// unit in °/s
	static const double yawRateForGLOM = 0.25;

	// Max yaw rate for GALILEO satellites(IOV/FOC)
	// unit in °/s
	static const double yawRateForGAL = 0.203;

	// Max yaw rate for BDS satellites( GEO/IGSO/MEO)
	// unit in °/s, this parameter is not needed for BDS.
	static const double yawRateForBDS = 0.203;


	// yaw bias for GPS BLOCK IIA satellites( after 2004 Feb)
	// unit in degree
	static const double yawBiasForGPSIIA = 0.50;

	// yaw bias for GPS BLOCK IIF satellites
	// (USED FOR SMALL NEG BETA NOON TURNS ONLY)
	// unit in degree
	static const double yawBiasForGPSIIF = -0.90; //

	// yaw bias for other system is 0.0

	// The angle limit for GNSS shadow entry.
	// unit in degree
	static const double ANIGHTForGPS = 180.0 + 13.25;
	static const double ANIGHTForGLO = 180.0 + 14.20;
	// https://www.gsc-europa.eu/support-to-developers/galileo-satellite-metadata
	static const double ANIGHTForGALIOV = 180.0 + 15.0;
	static const double ANIGHTForGALFOC = 180.0 + 10.0;
	static const double ANIGHTForBDS = 180.0 + 4.0;
	// add other system here


	/* Returns a satTypeValueMap object, adding the new data generated
	*  when calling this object.
	*
	* @param epoch     Time of observations.
	* @param gData     Data object holding the data.
	*/
	satTypeValueMap& EclipsedSatFilter::Process( const CommonTime& epoch,
		satTypeValueMap& gData )
		noexcept(false)
	{

		try
		{
			// Must set antenna file first
			if(pAntexReader == NULL)
			{
				ProcessingException e("No antenna file!");
				GPSTK_THROW(e);
			}

			// Insert satellites being deleted.
			SatIDSet satRejectedSet;

			// The earth rotation, unit in rad/s
			double earthRotate = 7.2921151467e-5;

			// Compute Sun position at this epoch, in ECEF
			// Since the algorithm of solar coordinates in GPSTK is  rough , the calculated 
			// solar coordinates are not accurate, which result in several minites error of the 
			// start/end time of the maneuver/shadow-crossing.
			SunPosition sunPosition;
			Triple sunPos(sunPosition.getPosition(epoch));

			// Define a Triple that will hold satellite position and
			// velocity, in ECEF
			Triple svPos(0.0, 0.0, 0.0),svPosCIS(0.0,0.0,0.0);
			Triple svVel(0.0, 0.0, 0.0),svVelCIS(0.0, 0.0, 0.0); 

			// Loop through all the satellites
			satTypeValueMap::iterator it;
			for (it = gData.begin(); it != gData.end(); ++it) 
			{

				// Check if satellite position is not already computed
				if( ( (*it).second.find(TypeID::satXECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satYECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satZECEF) == (*it).second.end() ) )
				{

					// If satellite position is missing, then schedule this 
					// satellite for removal
					satRejectedSet.insert( (*it).first );
					continue;
				}
				else
				{
					// Get satellite position out of GDS
					svPos[0] = (*it).second[TypeID::satXECEF];
					svPos[1] = (*it).second[TypeID::satYECEF];
					svPos[2] = (*it).second[TypeID::satZECEF];
				}

				// Check if satellite velocity is not already computed
				if( ( (*it).second.find(TypeID::satVXECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satVYECEF) == (*it).second.end() ) ||
					( (*it).second.find(TypeID::satVZECEF) == (*it).second.end() ) )
				{

					// If satellite velocity is missing, then schedule this 
					// satellite for removal
					satRejectedSet.insert( (*it).first );
					continue;
				}
				else
				{
					// Get satellite position out of GDS
					svVel[0] = (*it).second[TypeID::satVXECEF];
					svVel[1] = (*it).second[TypeID::satVYECEF];
					svVel[2] = (*it).second[TypeID::satVZECEF];
				}

				// Caculate the satellite-fixed coordinate orientation
				// which is independent on the velocity or satellite attitude.
				Triple satbdX(0.0,0.0,0.0),satbdX_nominal(0.0,0.0,0.0);
				satbdX = ComputesatbdXYZ(svPos, sunPos);
				//satbdX = ComputesatbdXYZ(svPosCIS, sunPosCIS);
				satbdX_nominal = satbdX; // copy for nominal attitude

				// Caculate the approximate inertial velocity.			
				Triple svVelCIS2(0.0,0.0,0.0);
				svVelCIS2[0] = svVel[0] - earthRotate*svPos[1];
				svVelCIS2[1] = svVel[1] + earthRotate*svPos[0];
				svVelCIS2[2] = svVel[2];

				Triple n( svPos.cross(svVelCIS2) ); // orbit normal
				Triple p( sunPos.cross(n) ); // 6am in orbit plane
				// unit vector
				Triple es(svPos.unitVector());
				Triple esun(sunPos.unitVector());
				Triple en(n.unitVector());
				Triple ep(p.unitVector());

				//Triple n( svPosCIS.cross(svVelCIS) ); // orbit normal
				//Triple p( sunPosCIS.cross(n) ); // 6am in orbit plane
				// unit vector
				//Triple es(svPosCIS.unitVector());
				//Triple esun(sunPosCIS.unitVector());
				//Triple en(n.unitVector());
				//Triple ep(p.unitVector());

				// The beta angle of sun above the orbit plane
				// sun beta, -90~90, unit in degree
				// used for test noon maneuver
				//double beta1 = (PI/2.0 - std::acos(esun.dot(en)))*RAD_TO_DEG;	
				double beta = ComputeBeta(svPos,svVelCIS2,sunPos);
				//double beta = ComputeBeta(svPosCIS,svVelCIS,sunPosCIS);
				gData[(*it).first][TypeID::beta] = beta; // insert

				double E = std::acos(es.dot(ep));
				double mu = PI/2.0 + ( ( (es.dot(esun))<= 0.0)?-E:E); // From midnight
				// 0-360
				if(mu < 0.0) mu += 2.0*PI;
				if(mu >=2.0*PI) mu -= 2.0*PI;
				// -270-90
				//if( mu < -PI/2.0) mu += 2.0*PI;
				//else if(mu >= PI/2.0) mu -= 2.0*PI;

				gData[(*it).first][TypeID::mu] = mu*RAD_TO_DEG; // insert

				// Get the satellite id and type from antenna file
				std::stringstream sat;
				if( (*it).first.system == SatelliteSystem::GPS )
				{
					sat << "G";
					if( (*it).first.id < 10 )
					{
						sat << "0";
					}
					sat << (*it).first.id;
				}
				else if( (*it).first.system == SatelliteSystem::Glonass)
				{
					sat << "R";
					if( (*it).first.id < 10 )
					{
						sat << "0";
					}
					sat << (*it).first.id;
				}
				else if( (*it).first.system == SatelliteSystem::Galileo )
				{
					sat << "E";
					if( (*it).first.id < 10 )
					{
						sat << "0";
					}
					sat << (*it).first.id;
				}
				else if( (*it).first.system == SatelliteSystem::BeiDou)
				{
					sat << "C";
					if( (*it).first.id < 10 )
					{
						sat << "0";
					}
					sat << (*it).first.id;
				}

				// Get satellite antenna information out of AntexReader object
				Antenna antenna( pAntexReader->getAntenna( sat.str(), epoch ) );
				std::string satType(antenna.getAntennaType());

				if(satType.find("GALILEO-FOC")!= string::npos)
					satType = "GALILEO-2";
				else if(satType.find("GALILEO-IOV")!= string::npos)
					satType = "GALILEO-1";

				// The max yaw rate/yaw bias/ANIGHT for this satellite
				double yawRate = getYawRate( (*it).first, satType);
				double yawBias = getYawBias(satType);
				double ANIGHT = getANIGHT((*it).first,satType);

				// The actual satellite orbit angle rate(MURATE)
				// ~0.00836 FOR GPS; ~ 0.00888 GLONASS
				//double murate = std::sqrt( (svVelCIS.dot(svVelCIS))/(svPosCIS.dot(svPosCIS)) )/DEG_TO_RAD;
				double murate = std::sqrt( (svVelCIS2.dot(svVelCIS2))/(svPos.dot(svPos)) )/DEG_TO_RAD;
				// The angle between sun-earth-satellite, which is used for determine
				// whether maneuver or shadow-crossing. This condition is equal to
				// use both mu and beta
				double svcos = esun.dot(es);

				double yangle(0.0), phi(0.0);
				// 0 = nominal attitude, 1 = actual attitude
				if ( yaw_angle(epoch,(*it).first,satType,svcos,satbdX,beta,svPos,svVelCIS2,murate,mu,yangle,phi,yawRate,yawBias,ANIGHT) )
				{
					// Insert nonimal and actual yaw angle
					gData[(*it).first][TypeID::yawangle] = yangle;
					gData[(*it).first][TypeID::phiangle] = phi;
					double anglediff = std::abs(yangle - phi);
					double factor(1.0);
					if(anglediff <= 20.0) factor = 1.5;
					else if(anglediff <= 90.0) factor = 3.0-1.5*(90.0-anglediff)/70.0;
					else if(anglediff <= 180.0) factor = 6.0-3.0*(180.0-anglediff)/90.0;
					else if(anglediff <= 360.0) factor = 6.0;

					// Three strategies has be adopted.
					if(eclipseModel == 1)
					{
						// delete this satellites
						satRejectedSet.insert( (*it).first );
					}
					else if(eclipseModel == 2)
					{
						// remain nominal attitude
						eclipseDataMap[(*it).first].bodyx = satbdX_nominal;
					}
					else if(eclipseModel == 3)
					{
						// use the modified attitude
						eclipseDataMap[(*it).first].bodyx = satbdX;
						// At the same time, degrade its weight
						gData[(*it).first][TypeID::varC1] 
                            = gData[(*it).first][TypeID::varC2] 
						        = gData[(*it).first][TypeID::varC1]*factor;


						gData[(*it).first][TypeID::varL1] 
                            = gData[(*it).first][TypeID::varL2] 
                                = gData[(*it).first][TypeID::varL1]*factor;

						gData[(*it).first][TypeID::varPC12] 
                            = gData[(*it).first][TypeID::varPC12]*factor;

						gData[(*it).first][TypeID::varLC12] 
                            = gData[(*it).first][TypeID::varLC12]*factor;

					}

				}
				else
				{
					// BDS GEO will alawys not be deleted.
					if( satType=="BEIDOU-2G" || satType == "COMPASS-GEO")
					{
						eclipseDataMap[(*it).first].bodyx = satbdX;
						// Insert nominal and actual yaw angle
						gData[(*it).first][TypeID::yawangle] = yangle;
						gData[(*it).first][TypeID::phiangle] = 0.0; // = nominal

					}
					else
					{
						eclipseDataMap[(*it).first].bodyx = satbdX_nominal;
						// Insert nominal and actual yaw angle
						gData[(*it).first][TypeID::yawangle] = yangle;
						gData[(*it).first][TypeID::phiangle] = yangle; // = nominal
					}
			
				}


			} // End for satellite loop

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

	}  // End of 'EclipsedSatFilter::Process()'

	void EclipsedSatFilter::Process(gnssRinex& gData)
		noexcept(false)
	{

		try
		{
			Process(gData.header.epoch, gData.body);

			return;

		}
		catch(Exception& u)
		{
			// Throw an exception if something unexpected happens
			ProcessingException e( getClassName() + ":"
				+ u.what() );

			GPSTK_THROW(e);

		}

	}  // End of 'EclipsedSatFilter::Process()'


	//Caculate the X-axis  unit vector of satellite-fixed coordinate frame in ECEF
	Triple EclipsedSatFilter::ComputesatbdXYZ(Triple &satPos, Triple &sunPos)
	{
		// Vector from satellite MC to sun MC
		Triple gps_sun(sunPos - satPos);

		// Define the satellite-fixed coordinate frame (ri, rj, rk) in ECEF

		// Unit vector from satellite MC to earth MC
		Triple rk( ( (-1.0)*(satPos.unitVector() ) ) );

		// Unit vector, rj = rk x gps_sun
		Triple rj( (rk.cross(gps_sun)).unitVector() );

		// Unit vector ri = rj x rk
		Triple ri = (rj.cross(rk)).unitVector();

		return ri; // X-axis

	}



	// Compute the beta
	double EclipsedSatFilter::ComputeBeta(const Triple& satPos,
		const Triple& satVel, 
		const Triple& sunPos)
	{
		// unit vector
		Triple sv( satVel.unitVector() );
		Triple sp( satPos.unitVector() );
		Triple ss( sunPos.unitVector() );

		// +90.0
		double beta = std::acos((sv.cross(sp)).dot(ss))*RAD_TO_DEG;

		return beta - 90.0;
	}


	// Get the max yaw rate.
	double EclipsedSatFilter::getYawRate(const SatID& sat,  const std::string& type)
	{
		double maxYawRate(0.0);

		if(type == "BLOCK IIA" || type == "BLOCK II")
		{
			int i = sat.id;
			maxYawRate = yawRateForGPSIIA[i];
		}
		else if(type == "BLOCK IIR-M" || type == "BLOCK IIR-A" || type == "BLOCK IIR-B")
		{
			maxYawRate = yawRateForGPSIIR;
		}
		else if(type == "BLOCK IIF")
		{
			maxYawRate = yawRateForGPSIIF;
		}
		else if(sat.system == SatelliteSystem::Glonass)
		{
			maxYawRate = yawRateForGLOM;
		}
		else if(sat.system == SatelliteSystem::Galileo)
		{
			maxYawRate = yawRateForGAL;
		}
		else if(sat.system == SatelliteSystem::BeiDou)
		{
			maxYawRate = yawRateForBDS;
		}

		return maxYawRate;

	}


	// Get GNSS yaw bias
	double EclipsedSatFilter::getYawBias(const std::string& type)
	{
		double YawBias(0.0);

		if(type == "BLOCK IIA" || type == "BLOCK II")
		{
			YawBias = yawBiasForGPSIIA;
		}
		else if(type == "BLOCK IIF")
		{
			YawBias = yawBiasForGPSIIF;
		}
		else
		{
			YawBias = 0.0;
		}

		return YawBias;
	}


	// Get threshold for GNSS shadow entry
	double EclipsedSatFilter::getANIGHT(const SatID& sat, const std::string& type)
	{
		double anight(0.0);

		if(sat.system == SatelliteSystem::GPS)
		{
			anight = ANIGHTForGPS;
		}
		else if(sat.system == SatelliteSystem::Glonass)
		{
			anight = ANIGHTForGLO;
		}
		else if(sat.system == SatelliteSystem::BeiDou)
		{
			anight = ANIGHTForBDS;
		}
		else if(sat.system == SatelliteSystem::Galileo)
		{
			if(type == "GALILEO-1") // E11 E12 E19 E20
			{
				anight = ANIGHTForGALIOV;
			}
			else
			{
				anight = ANIGHTForGALFOC;
			}
		}

		return anight;

	}


	int EclipsedSatFilter::yaw_angle(const CommonTime& epoch, 
			const SatID& sat, 
			const std::string& satType,
			const double svcos,
			Triple &santxyz,
			double beta,
			Triple& svPos,
			Triple& svVelIner,
			const double murate,
			const double mu,
			double& yangle,
			double& phi,
			double& yawRate,
			double& yawBias,
			double& ANIGHT)
	{
		// eclipse flag(1=true，0=false）
		int noon(0), night(0);

		double halfhr=1800.0;
		if (sat.system==SatelliteSystem::Galileo)	halfhr=0.0;

		// BDS=2*beta0, FOC=4.1，IOV=2.0
		double beta0 = 2.0;
		if (satType=="GALILEO-2")	beta0=4.1; // only for GALILEO-FOC

		// Use ANIGHT value to determine noon maneuver only for GALILEO/BDS
		// Other systems adopt average orbital angular velocity.
		double anoon(0.0); // unit in degree
		if (sat.system==SatelliteSystem::Galileo|| sat.system==SatelliteSystem::BeiDou)
		{
			anoon = ANIGHT-180.0;
		}
		else
		{
			anoon = std::atan(murate/yawRate)*RAD_TO_DEG;
		}

		// The initial threshold value for noon maneuver and midnight maneuver
		double cnoon = std::cos(anoon*DEG_TO_RAD); // positive value, approxomate to 1.0

		// Caculate the threshold for GLONASS noon maneuver
		double yawend(0.0);
		if ( (sat.system==SatelliteSystem::Glonass) && (std::abs(beta)<anoon) )
		{
			init_cnoon(beta,murate,yawRate,anoon,cnoon,yawend);
		}		

		// The threshold value of shadow-crossing/midnight
		double cnight = std::cos(ANIGHT*DEG_TO_RAD); // negative value, approxomate to -1.0
		// Update for IIR/GALILEO/BDS
		if ( (sat.system==SatelliteSystem::Galileo) || (sat.system==SatelliteSystem::BeiDou) 
			|| (satType.find("BLOCK IIR")!=string::npos) )
		{
			// IIR only midnight, not shadow crossing
			cnight = std::cos((anoon+180.0)*DEG_TO_RAD);
		}

		// This method is relatively simple and is equivalent to the method using beta and mu.
		if (svcos > cnoon)
		{
			noon = 1;
		}

		if (svcos < cnight)
		{
			night = 1;
		}

		// When beta>=beta0 for GALILEO(IOV 2 OR FOC 4.1), no eclipse will
		// happen.
		if ( (sat.system==SatelliteSystem::Galileo) && (std::abs(beta)>=beta0) )	
			night = noon = 0;

		// nominal yaw anlge (the angle between satellite body x-axis and inertial velocity)
		// IGS convention: the sign is always opposite to beta, unit in degree
		yangle = std::acos(santxyz.dot(svVelIner)/std::sqrt(svVelIner.dot(svVelIner)))*RAD_TO_DEG*(beta>0.0?-1.0:1.0);

		// For GLONASS-K1 satellite, no attittude control model has been released.
		if(satType.find("GLONASS-K1")!=string::npos)
			return 0; // always use nominal attitude

		// BDS is different from other system.
		if (sat.system==SatelliteSystem::BeiDou)
		{
			if(!bds_yaw(epoch,sat,satType,beta,beta0,yangle,mu,murate,svPos,svVelIner,phi,santxyz))	
			{
				return 0; // YS
			}
			else
			{
				return 1; // ON
			}
		}

		// Other system except BDS
		double det(0.0), dttag(0.0);
		if (night||noon)
		{
			// unit in degree
			det = std::sqrt( (180.0-std::acos(svcos)*RAD_TO_DEG)*(180.0-std::acos(svcos)*RAD_TO_DEG)-beta*beta );	
			phi = PI/2;

			// Check if already after a midnight or noon
			if (night)
			{
				// if midnight maneuver
				if (std::abs(yangle)<90.0) det = -det;

				if (det!=0.0) 
					phi=std::atan2(-std::tan(beta*DEG_TO_RAD),std::sin(-det*DEG_TO_RAD))*RAD_TO_DEG;//初始化实际姿态
			}

			if (noon)
			{
				// if noon maneuver
				det=std::sqrt( (std::acos(svcos)*RAD_TO_DEG)*(std::acos(svcos)*RAD_TO_DEG)-beta*beta );
				if (std::abs(yangle)>90.0) det=-det;

				if (det!=0.0) 
					phi=std::atan2(-std::tan(beta*DEG_TO_RAD),std::sin(PI-det*DEG_TO_RAD))*RAD_TO_DEG;//初始化实际姿态
			}

			// Compute the eclipse start and end time, only once
			if (!eclipseDataMap[sat].neclipse)
			{
				// set the eclipse flag
				eclipseDataMap[sat].neclipse = 1;
				// The time whensatellite reaches to orbit noon or midnight.
				eclipseDataMap[sat].eclipseStartTime = epoch+det/murate;
				// unit in degree, beta threshold
				yawend=std::atan(murate/yawRate)*RAD_TO_DEG;

				if(sat.system == SatelliteSystem::Galileo  )
					yawend = beta0;

				// noon maneuver and midnight maneuver
				// GPS IIR's midnight is just like the noon maneuver
				if ((satType.find("BLOCK IIR")!=string::npos||noon)&&std::abs(beta)<yawend)
				{
					turntime(epoch,sat,satType,night,noon,santxyz,anoon,mu,murate,beta,ANIGHT,yangle);
				}

				// shadow crossing( no midnight maneuver)
				if (satType.find("BLOCK IIR")==string::npos&&night)
				{
					turntime(epoch,sat,satType,night,noon,santxyz,anoon,mu,murate,beta,ANIGHT,yangle);
				}

			}

			if ((night&&svcos<cnight)||(noon&&svcos>cnoon))
			{
				// dttag=std::abs(epoch-eclipseDataMap[sat].eclipseStartTime);// No need

				// Set GPS/GLONASS/GALILEO if |beta|=<0.07deg
				// The sign of beta may be changed and cause big error.
				if (std::abs(beta)<0.07&&eclipseDataMap[sat].betaInit==0.0)	
					eclipseDataMap[sat].betaInit=beta; //store this value
			}
		}

		// If satellite is in eclipse
		if (eclipseDataMap.find(sat) != eclipseDataMap.end() &&
			eclipseDataMap[sat].neclipse)
		{
			// in eclipse region
			if (epoch>=eclipseDataMap[sat].eclipseStartTime&&epoch<=(eclipseDataMap[sat].eclipseEndTime+halfhr))
			{

				if (std::abs(beta)<0.07&&eclipseDataMap[sat].betaInit!=0.0) 
					beta=eclipseDataMap[sat].betaInit; // use the stored beta

				// unit vector
				Triple v = svVelIner.unitVector();	
				Triple r = svPos.unitVector();

				// The true yaw angle is equal to nominal yaw angle when satellite gets out of eclipse. 
				det=murate*(eclipseDataMap[sat].eclipseEndTime-eclipseDataMap[sat].eclipseStartTime)/2;

				// IIF shadow yaw rate(NOT using 0.06degree/s)
				yawend=(atan2(-std::tan(beta*DEG_TO_RAD),std::sin(det*DEG_TO_RAD))-
					atan2(-std::tan(beta*DEG_TO_RAD),std::sin(-det*DEG_TO_RAD)))/DEG_TO_RAD/
					(eclipseDataMap[sat].eclipseEndTime-eclipseDataMap[sat].eclipseStartTime);

				// shadow crrosing or noon maneuver
				if (svcos<0)
				{
					// Compute the actual yaw angle during shadow crossing
					if(shadowcss(epoch,sat,satType,yawBias,det,yawRate,beta,yawend,phi,anoon,murate,beta0,yangle,ANIGHT))	
					{
						return 0; // nominal attitude(shadow exit)
					}
				}
				else
				{		
					// Compute the actual yaw angle during noon maneuver
					if(noonturn(epoch,sat,satType,yawBias,det,yawRate,beta,yawend,phi,anoon,murate,beta0,yangle,ANIGHT))	
					{
						return 0; // nominal attitude(noon maneuver exit)
					}
				}

				// rotate Z-axis with the true yaw angle
				yaw2phi(phi,yangle,v,r,santxyz);

				return 1; // actual attitude
			}
			// if satellite gets out eclipse region
			else if (epoch>=(eclipseDataMap[sat].eclipseEndTime+halfhr))
			{
				// initialize
				eclipseDataMap[sat].neclipse = 0;	
				eclipseDataMap[sat].betaInit = 0.0;	
				eclipseDataMap[sat].phiInit = 0.0; 


			}

		}

		return 0; // nominal attitude

	} // End of yaw_angle


	// Compute the noon threshold value for GLONASS
	void EclipsedSatFilter::init_cnoon(double beta,
		double murate,
		double yrate,
		double &anoon,
		double &cnoon,
		double &yawend)
	{
		// (yaw end - yaw begin)/2
		yawend = 75.0; // initial value

		// ITERATION FOR YAWEND OF THE GLONASS  NOON TURN
		for (int i=0; i<3; ++i)	
		{
			yawend = std::abs(atan2(-std::tan(beta*D2R),std::sin(PI-D2R*murate*yawend/yrate))/D2R
			-atan2(-std::tan(beta*D2R),std::sin(PI+D2R*murate*yawend/yrate))/D2R)/2;
		}

		anoon=murate*yawend/yrate;
		cnoon=std::cos(anoon*D2R);
	}


	// BDS attitude, different from other system
	int EclipsedSatFilter::bds_yaw(const CommonTime &epoch,
			const SatID &sat,
			const std::string &satType,
			const double beta,
			const double beta0,
			const double yangle,
			const double mu,
			const double murate,	
			const Triple &svPos,
			const Triple &svVelIner,
			double &phi,
			Triple &santxyz)
	{

		if( satType=="BEIDOU-2G" || satType == "COMPASS-GEO")
		{
			//eclipseDataMap[sat].neclipse = 1.0; // always ON
			santxyz = svVelIner.unitVector(); // X-axis coincide with velocity
			phi = 0.0;
			return 0; // set as NOMINAL

		}
		else // IGSO or MEO
		{
			// The nominal  yaw rate of the satellite
			double yanglev = murate*std::tan(beta*DEG_TO_RAD)*std::cos(mu)/
				( (std::sin(mu))*(std::sin(mu))+(std::tan(beta*DEG_TO_RAD))*(std::tan(beta*DEG_TO_RAD)) );

			// YS -> ON
			if( ((std::abs(beta)<=2*beta0 && std::abs(yangle)<=5.0) ||
				(std::abs(beta)<=2*beta0 && 5.0<std::abs(yangle) && std::abs(yangle)<20.0 && yanglev*yangle>0)) &&
				(!eclipseDataMap[sat].neclipse) )
			{
				eclipseDataMap[sat].neclipse = 1.0; // ON
			}

			// ON -> YS
			if( ((std::abs(beta)>2*beta0 && std::abs(yangle)<=5.0) ||
				(std::abs(beta)>2*beta0 && 5.0<std::abs(yangle) && std::abs(yangle)<20.0 && yanglev*yangle>0)) &&
				(eclipseDataMap[sat].neclipse))
			{
				eclipseDataMap[sat].neclipse = 0.0; // YS
			}

		}

		if (eclipseDataMap.find(sat) != eclipseDataMap.end() &&
			eclipseDataMap[sat].neclipse)
		{
			// if ON
			santxyz = svVelIner.unitVector(); // X-axis coincide with velocity
			phi = 0.0;
			return 1;
		}

		return 0; // nominal attitude

	}


void EclipsedSatFilter::turntime(const CommonTime &epoch,
			const SatID &sat,
			const std::string &satType,
			int night,
			int noon,
			Triple &santxyz,
			double anoon,
			double mu,
			double murate,
			double beta,
			double anight,
			double yangle)
{
	// The half time of eclipse
	double midt(0.0);

	// GPS IIR will remain nominal attitude during shadow-crossing.
	if (noon||satType.find("BLOCK IIR")!=string::npos)
	{
		if (sat.system==SatelliteSystem::Glonass||sat.system==SatelliteSystem::Galileo)
		{

			midt = anoon/murate;
			eclipseDataMap[sat].eclipseStartTime -= midt;
			eclipseDataMap[sat].eclipseEndTime = eclipseDataMap[sat].eclipseStartTime+2*midt;

			if (satType.find("GALILEO-2")!=string::npos)	// only for FOC
			{
				// The u at eclipse start time
				double elapsetime = epoch - eclipseDataMap[sat].eclipseStartTime;
				mu = mu - murate*elapsetime*DEG_TO_RAD;
				if(mu < 0.0) mu += 2.0*PI;
				else if(mu > 2.0*PI) mu -= 2.0*PI;

				double yangle3 = std::atan2(-std::tan(beta*DEG_TO_RAD),std::sin(mu))*RAD_TO_DEG;

				// Store this value
				eclipseDataMap[sat].phiInit = yangle3;

			}

		}
		else 
		{
			midt = std::abs(beta)*std::sqrt(anoon/std::abs(beta)-1.0)/murate;
			eclipseDataMap[sat].eclipseStartTime -= midt;
			eclipseDataMap[sat].eclipseEndTime =eclipseDataMap[sat].eclipseStartTime+2*midt;
		}
	}
	else if (night) // shadow crossing
	{

		midt=std::sqrt( (anight-180.0)*(anight-180.0)-beta*beta)/murate; // second
		eclipseDataMap[sat].eclipseStartTime -= midt;
		eclipseDataMap[sat].eclipseEndTime = eclipseDataMap[sat].eclipseStartTime+2*midt;

		if (satType.find("GALILEO-2")!=string::npos)	// only for FOC
		{ 
			// The u at eclipse start time
			double elapsetime = epoch - eclipseDataMap[sat].eclipseStartTime;
			mu = mu - murate*elapsetime*DEG_TO_RAD;
			if(mu < 0.0) mu += 2.0*PI;
			else if(mu > 2.0*PI) mu -= 2.0*PI;

			double yangle3 = std::atan2(-std::tan(beta*DEG_TO_RAD),std::sin(mu))*RAD_TO_DEG;

			// Store this value
			eclipseDataMap[sat].phiInit = yangle3;

		}

	}

}


int  EclipsedSatFilter::shadowcss(const CommonTime &epoch,
								   const SatID &sat,
								   const std::string &satType,
								   double ybias,
								   double det,
								   double yrate,
								   double beta,
								   double yawend,
								   double &phi,
								   double anoon, 
								   double murate,
								   double beta0,
								   double yanlge,
								   double anight)
{
	double betay, betae,midt,phisign;

	// GPS II/IIA should be implemented.

	// GPS IIR(like the noon maneuver)
	if (satType.find("BLOCK IIR")!=string::npos)
	{
		yrate*=beta>0?1:-1;
		phi=atan2(-std::tan(beta*DEG_TO_RAD),std::sin(-det*DEG_TO_RAD))/DEG_TO_RAD+
			yrate*(epoch-eclipseDataMap[sat].eclipseStartTime);

		// IIR end condition
		if ((yanlge/phi)>=1.0||(phi/yanlge)<0.0)	return 1;
	}

	// GPS  IIF
	if (satType.find("BLOCK IIF")!=string::npos)
	{
		if (epoch<eclipseDataMap[sat].eclipseEndTime)
		{
			yrate*=ybias>0?1:-1;
			// Correct IIF  shadow crossing using computing yaw rate (yawend)
			// according to th USAF document.
			phi=std::atan2(-std::tan(beta*DEG_TO_RAD),std::sin(-det*DEG_TO_RAD))*RAD_TO_DEG+
				yawend*(epoch-eclipseDataMap[sat].eclipseStartTime);
		}
		else
		{
			return 1;
		}
	}

	// GLONASS
	if (satType.find("GLONASS-M")!=string::npos)
	{
		// No recovery is need for GLONASS satellites.
		if (epoch>eclipseDataMap[sat].eclipseEndTime) return 1;

		// GLONASS-M satellite will rotate with the maximum speed(0.25) to reach the nominal
		// attitude when it enters the shadow. 
		yawend = yrate*(beta>0?1:-1);
		phi=atan2(-std::tan(beta*DEG_TO_RAD),std::sin(-det*DEG_TO_RAD))/DEG_TO_RAD+
			yawend*(epoch-eclipseDataMap[sat].eclipseStartTime);

		// Then the nominal attitude is maintained until shadow exit.
		yawend=atan2(-std::tan(beta*DEG_TO_RAD),std::sin(det*DEG_TO_RAD))/DEG_TO_RAD;
		if ((yawend/phi)>=1.0||(phi/yawend)<0.0)	phi=yawend;
	}


	// GALILEO IOV
	if (satType.find("GALILEO-1")!=string::npos)
	{
		betay=-std::sin(beta0*DEG_TO_RAD)*(beta>0?1:-1);
		double deltaT = eclipseDataMap[sat].eclipseEndTime - eclipseDataMap[sat].eclipseStartTime;
		CommonTime midTime(eclipseDataMap[sat].eclipseStartTime);
		midTime += deltaT/2.0;
		yawend=std::sin((epoch-midTime)*murate*DEG_TO_RAD);
		betae=0.5*(betay-std::sin(beta*DEG_TO_RAD))+0.5*(betay+std::sin(beta*DEG_TO_RAD))
			*std::cos(PI*std::abs(yawend*std::cos(beta*DEG_TO_RAD))/std::sin(anoon*DEG_TO_RAD));
		phi  =atan2(betae,yawend*std::cos(beta*DEG_TO_RAD))/DEG_TO_RAD;	
	}

	// GALILEO FOC
	if (satType.find("GALILEO-2")!=string::npos)
	{
		phisign=PI/2*RAD_TO_DEG*(eclipseDataMap[sat].phiInit>0?1:-1);
		phi=phisign+(eclipseDataMap[sat].phiInit-phisign)*
			std::cos(2*PI/5656.0*(epoch-eclipseDataMap[sat].eclipseStartTime));
	}

	return 0;
}


int EclipsedSatFilter::noonturn(const CommonTime &epoch,
								 const SatID &sat,
								 const std::string &satType,
								 double ybias,
								 double det,
								 double yrate,
								 double beta,
								 double yawend,
								 double &phi,
								 double anoon, 
								 double murate,
								 double beta0,
								 double yanlge,
								 double anight)
{

	double betay, betae,midt,phisign;

	yrate*=-beta>0?1:-1;
	phi=atan2(-std::tan(beta*DEG_TO_RAD),std::sin(PI-det*DEG_TO_RAD))*RAD_TO_DEG+
		yrate*(epoch-eclipseDataMap[sat].eclipseStartTime);

	// only for GPS II/IIA or IIF because of iys yaw bias 
	if (sat.system==SatelliteSystem::GPS&&(beta*(ybias>0?1:-1)<=std::abs(ybias))&&(beta*ybias)>0)
	{
		yrate*=ybias>0?1:-1;
		phi=atan2(-std::tan(beta*DEG_TO_RAD),std::sin(PI-det*DEG_TO_RAD))/DEG_TO_RAD+
			yrate*(epoch-eclipseDataMap[sat].eclipseStartTime);
	}

	if (satType.find("BLOCK IIR")!=string::npos) // GPS IIR
	{
		if ((phi/yanlge)>=1||(phi/yanlge)<0.0)	return 1;
	}
	else
	{
		if ((satType.find("GLONASS-M") != string::npos||
			satType.find("GALILEO-1") != string::npos ||
			satType.find("GALILEO-2") != string::npos)&&
			epoch>eclipseDataMap[sat].eclipseEndTime)	return 1;

		if (sat.system==SatelliteSystem::GPS&&beta*(ybias>0?1:-1)<=std::abs(ybias)&&(beta*ybias)>0.0&&
			(((phi-(ybias>0?1:-1)*360.0)/yanlge)<=1.0||
			((phi-(ybias>0?1:-1)*360.0)/yanlge)<0.0))	 return 1;

		// -0.7 or -0.9
		//if (sat.system==SatelliteSystem::GPS&&(beta*(ybias>0?1:-1)>0.7||beta*ybias<=0.0)&&
		if (sat.system==SatelliteSystem::GPS&&(beta*(ybias>0?1:-1)>0.9||beta*ybias<=0.0)&&
			((phi/yanlge)>=1||(phi/yanlge)<0.0))
		{
				return 1;
		}
	}

	// GALILEO IOV
	if (satType.find("GALILEO-1")!=string::npos)
	{
		betay=-std::sin(beta0*DEG_TO_RAD)*(beta>0?1:-1);
		double deltaT = eclipseDataMap[sat].eclipseEndTime - eclipseDataMap[sat].eclipseStartTime;
		CommonTime midTime(eclipseDataMap[sat].eclipseStartTime);
		midTime += deltaT/2.0;
		yawend=std::sin(PI+(epoch-midTime)*murate*DEG_TO_RAD);
		betae=0.5*(betay-std::sin(beta*DEG_TO_RAD))+0.5*(betay+std::sin(beta*DEG_TO_RAD))
			*std::cos(PI*std::abs(yawend*std::cos(beta*DEG_TO_RAD))/std::sin(anoon*DEG_TO_RAD));
		phi =atan2(betae,yawend*std::cos(beta*DEG_TO_RAD))*RAD_TO_DEG;
	}

	// GALILEO FOC
	if (satType.find("GALILEO-2")!=string::npos)
	{
		phisign=PI/2*RAD_TO_DEG*(eclipseDataMap[sat].phiInit>0?1:-1);
		phi=phisign+(eclipseDataMap[sat].phiInit-phisign)*
			std::cos(2*PI/5656.0*(epoch-eclipseDataMap[sat].eclipseStartTime));
	}

	return 0;
}


// Get the true body-fixed coordinate orientation when satellite is in eclipse
void EclipsedSatFilter::yaw2phi(const double phi,
								 const double yanlge,
								 const Triple &v,
								 const Triple &r,
								 Triple &santxyz)
{
	//cout << santxyz << endl;
	// rotation according to Kouba
	santxyz[0]=(std::cos((phi-yanlge)*DEG_TO_RAD)*(v[1]-v[2]*r[1]/r[2])-std::cos(phi*DEG_TO_RAD)*
		(santxyz[1]-santxyz[2]*r[1]/r[2]))/(santxyz[0]*v[1]-santxyz[1]*v[0]
	+((santxyz[1]*v[2]-santxyz[2]*v[1])*r[0]+(santxyz[2]*v[0]-santxyz[0]
	*v[2])*r[1])/r[2]);

	santxyz[1]=(std::cos(phi*DEG_TO_RAD)-(v[0]-v[2]*r[0]/r[2])*santxyz[0])/(v[1]-v[2]*r[1]/r[2]);	

	santxyz[2]=(-r[0]*santxyz[0]-r[1]*santxyz[1])/r[2];
	//cout << santxyz << endl;
}




} // End of namespace gpstk
