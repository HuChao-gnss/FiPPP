#pragma ident "$Id$"

/**
* @file EclipsedSatFilter.hpp
* This class caculates the satellite attitude.
*/

#ifndef GPSTK_EclipseSatFilter_HPP
#define GPSTK_EclipseSatFilter_HPP

// Add antex reader for determining GNSS system

#include <math.h>
#include "DataStructures.hpp"
#include "Triple.hpp"
#include "SunPosition.hpp"
#include "Position.hpp"
#include "GNSSconstants.hpp"                   // DEG_TO_RAD
#include "AntexReader.hpp"
#include "SP3EphemerisStore.hpp"
#include "EOPDataStore.hpp"
#include "IERSConventions.hpp"
#include "gnssData.hpp"

namespace gpstk
{


	class EclipsedSatFilter 
	{
	public:

		/// Default constructor.
		EclipsedSatFilter() 
            : pAntexReader(NULL), 
              eclipseModel(2),
              pSp3EphList(NULL)
		{ eclipseDataMap.clear();};


		/** Sets AntexReader object to be used.
		*
		* @param antexObj  AntexReader object containing satellite
		*                  antenna data.
		*/
		virtual EclipsedSatFilter& setAntexReader(AntexReader& antexObj)
		{ pAntexReader = &antexObj; return (*this); };



		/** Sets the strategy of eclipse.
		 *
		 * @param model  1 = delete satellites; 
		 *               2 = remain nominal attitude; 
		 *               3 = the true attitude use Kouba's code;
		 */
		virtual EclipsedSatFilter& setEclipseModel(int model)
		{ 
            eclipseModel = model; 
            return (*this); 
        };


		/** Sets SP3Ephemeris object to be used.
		 *
		 * @param antexObj  SP3Ephemeris object containing satellite
		 *                  data.
		 */
		virtual EclipsedSatFilter& setSP3Ephemeris(SP3EphemerisStore& sp3Obj)
		{ 
            pSp3EphList = &sp3Obj; 
            return (*this); 
        };


		/** Caculate the X-axis  unit vector of satellite-fixed coordinate frame in ECEF
		 * @param satPos  Satellite position in ECEF;
		 * @param sunPos Sun position in ECEF;
		 */
		virtual Triple ComputesatbdXYZ(Triple &satPos, Triple &sunPos);


		/** Returns a satTypeValueMap object, adding the new data generated
		*  when calling this object.
		*
		* @param epoch     Time of observations.
		* @param gData     Data object holding the data.
		*/
		virtual satTypeValueMap& Process( const CommonTime& epoch,
			satTypeValueMap& gData )
			noexcept(false);

		virtual void Process(gnssRinex& gData)
			noexcept(false);

		/** Caculate the beta angle, in degree,-90.0~90.0.
		* @param satPos    satellite position in ITRF
		* @param satVel     satellite velocity in inertial
		* @param sunPos    sun position in ITRF
		*/
		virtual double ComputeBeta(const Triple& satPos,
			const Triple& satVel, 
			const Triple& sunPos);


		/** Get the max yaw rate.
		* @param sat        satellite 
		* @param type     satellite type string from antenna file
		*/
		virtual double getYawRate(const SatID& sat,  const std::string& type);


		/** Get the yaw bias.
		* @param type     satellite type string from antenna file
		*/
		virtual double getYawBias(const std::string& type);


		/** Get the angle limit for GNSS shadow entry.
		* @param sat        satID
		* @param type     satellite type string from antenna file
		*/
		virtual double getANIGHT(const SatID& sat, const std::string& type);


		/** Determine whether satellite is in eclipse or not..
		* @param epoch			observe time
		* @param sat				satID
		* @param satType		satellite type string from antenna file
		* @param svcos			the cosine angle between sun-earth-satellite
		* @param asntxyz		spacecraft-fixed X axis
		* @param beta				the acute angle between the Sun direction and the
orbit plane, unit in degree.
		* @param svPos			satellite position in ECEF
		* @param svVelIner		satellite velocity in ECI
		* @param murate		the average orbital angular velocity, unit in degree/s
		* @param mu				The angle formed between the satellite position vector 
and orbit midnight, unit in rad.
		* @param yangle			nominal yaw angle, unit in degree
		* @param phi				the true yaw angle, unit in degree
		* @param yawRate		the max  yaw-angle velocity, unit in degree/s
		* @param yawBias		the yaw bias 
		* @param ANIGHT		the shadow crossing's threshold value for u
		* @return int					0 = nominal attitude, 1 = Eclipse
		*/
		virtual int yaw_angle(const CommonTime& epoch, 
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
			double& ANIGHT);


		/** Iteration to compute GLONASS noon turn threshold
		* reference				Dilssner 2010 ASR or Kouba EclipseReadMe
		* @param beta			the acute angle between the Sun direction and the
orbit plane, unit in degree
		* @param murate	the average orbital angular velocity, unit in degree/s
		* @param yrate		the max  yaw-angle velocity, unit in degree/s
		* @param anoon		unit in degree
		* @param cnoon		the cosine value of anoon
		* @param yawend   (yaw end - yaw start)/2      
		*/
		void init_cnoon(const double beta,
			const double murate,
			const double yrate,
			double &anoon,
			double &cnoon,
			double &yawend);


		/** BDS attitude model, ON-YS, according to Dai Xiaolei's paper.
		* @param epoch			the observe time
		* @param sat				satellite id and system
		* @param satType		satellite type
		* @param beta				the acute angle between the Sun direction and the
orbit plane, unit in degree
		* @param beta0			the threshold, 4.0 degree for BDS satellite
		* @param yangle         nominal yaw angle
		* @param mu				The angle formed between the satellite position vector 
and orbit midnight, unit in rad, only for output
		* @param murate		the orbital angular velocity, unit in degree/s
		* @param svPos			satellite position in ECEF, only for output
		* @param svVelIner		satellite velocity in ECI
		* @param phi				the true yaw angle, unit in degree
		* @param santxyz		spacecraft-fixed X axis
		* @return						0 = YS; 1 = ON
		*/
		int bds_yaw(const CommonTime &epoch,
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
			Triple &santxyz);


		/** Compute the start and end time of eclipse.
		* @param sat				satellite id and system
		* @param satType		satellite type
		* @param night			the flag of shadow corssing
		* @param noon			the flag of noon maneuver
		* @param anoon			the threshold value for noon maneuver, unit in degree
		* @param murate		the orbital angular velocity, unit in degree/s
		* @param beta				the acute angle between the Sun direction and the
orbit plane, unit in degree
		* @param anight			the threshold of shadow crossing, unit in degree
		* @param yangle         nominal yaw angle
		*/
		void turntime(const CommonTime &epoch,
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
			double yangle);


		/** Get the angle limit for GNSS shadow entry.
		* @param epoch			observe time
		* @param sat				satID
		* @param satType		satellite type string from antenna file
		* @param ybias			the yaw bias 
		* @param det
		* @param yrate			the max  yaw-angle velocity, unit in degree/s
		* @param beta				the acute angle between the Sun direction and the
orbit plane, unit in degree.
		* @param yawend		
		* @param phi				the true yaw angle, unit in degree
		* @param anoon			the threshold for noon turn,unit in degree
		* @param murate		the average orbital angular velocity, unit in degree/s
		* @param beta0			the threshold for beta, unit in degree.	
		* @param yangle			nominal yaw angle, unit in degree
		* @param anight			the shadow crossing's threshold value for u
		* @return int					0 = during eclipse, 1 = Eclipse exit
		*/
		int  shadowcss(const CommonTime &epoch,
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
			double anight);


		/** Get the angle limit for GNSS shadow entry.
		* @param epoch			observe time
		* @param sat				satID
		* @param satType		satellite type string from antenna file
		* @param ybias			the yaw bias 
		* @param det
		* @param yrate			the max  yaw-angle velocity, unit in degree/s
		* @param beta				the acute angle between the Sun direction and the
orbit plane, unit in degree.
		* @param yawend		
		* @param phi				the true yaw angle, unit in degree
		* @param anoon			the threshold for noon turn,unit in degree
		* @param murate		the average orbital angular velocity, unit in degree/s
		* @param beta0			the threshold for beta, unit in degree.	
		* @param yangle			nominal yaw angle, unit in degree
		* @param anight			the shadow crossing's threshold value for u
		* @return int					0 = during eclipse, 1 = Eclipse exit
		*/
		int noonturn(const CommonTime &epoch,
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
			double anight);


		/** Get the true body-fixed coordinate orientation when satellite is in eclipse.
		* @param phi				satellite true yaw angle
		* @param yanlge			satellite nominal yaw angle
		* @param v					satellite velocity in ECI
		* @param r					satellite position in ECEF
		* @param santxyz		the body-fixed coordinate orientation
		*/
		void yaw2phi(const double phi,
			const double yanlge,
			const Triple &v,
			const Triple &r,
			Triple &santxyz);


		/// Returns a string identifying this object.
		virtual std::string getClassName(void) const;

		
		/// Destructor
		virtual ~EclipsedSatFilter() {};


	private:

		/// Pointer to object containing satellite antenna data, if available.
		AntexReader* pAntexReader;

		/// Strategy when satellite is in maneuver or shadow crossing
		int eclipseModel;

		/// A structure used to store filter data for a SV.
		struct eclipseData
		{
			// Default constructor initializing the data in the structure
			eclipseData() : eclipseStartTime(CommonTime::BEGINNING_OF_TIME),
				eclipseEndTime(CommonTime::BEGINNING_OF_TIME),
				neclipse(0),betaInit(0.0), phiInit(0.0),bodyx(0.0,0.0,0.0) {};

			CommonTime eclipseStartTime;    ///< The start time of eclipse.
			CommonTime eclipseEndTime;    ///< The end time of eclipse.
			int neclipse;///< The flag of eclipse.
			double betaInit;///< The initial value of beta.
			double phiInit;///< The initial value of yaw angle.
			Triple bodyx;///< The satellite body X-asix. 

		};

		/// Map holding the eclipse regarding every satellite
		std::map<SatID, eclipseData> eclipseDataMap;

        SP3EphemerisStore* pSp3EphList;

	public:

		/// Get the satellite body X-asix
		Triple getSatBodyX(const SatID& sat)
		{return eclipseDataMap[sat].bodyx;};


	}; // End of class 'EclipsedSatFilter'

	//@}

}  // End of namespace gpstk

#endif   // GPSTK_EclipseSatFilter_HPP
