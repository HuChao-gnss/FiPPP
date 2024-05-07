
#ifndef COMPUTE_SAT_ATTITUDE_HPP
#define COMPUTE_SAT_ATTITUDE_HPP


#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"
#include "AntexReader.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

    class ComputeSatAttitude
    {
    public:

        /// Default constructor.
        ComputeSatAttitude()
            : pRefSys(NULL), pSolSys(NULL), pAntexReader(NULL)
        {};


        /** Set ReferenceSystem object to be used.
        *
        * @param ref  ReferenceSystem object.
        */
        virtual ComputeSatAttitude& setReferenceSystem(ReferenceSystem& ref)
        { pRefSys = &ref; return (*this); };


        /** Set SolarSystem object to be used.
        *
        * @param sol  SolarSystem object.
        */
        virtual ComputeSatAttitude& setSolarSystem(SolarSystem& sol)
        { pSolSys = &sol; return (*this); };


        /** Set AntexReader object to be used.
        *
        * @param antex  AntexReader object.
        */
        virtual ComputeSatAttitude& setAntexReader(AntexReader& antex)
        { pAntexReader = &antex; return (*this); };


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            noexcept(false);


        virtual void Process(gnssRinex& gData)
            noexcept(false)
        { Process(gData.header.epoch, gData.body);  };


        satYawDataMap getAttitude( const CommonTime& epoch,
                                   const satVectorMap& orbits )
            noexcept(false);


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor
        virtual ~ComputeSatAttitude() {};

    private:

        bool noonManeuver(const CommonTime& epoch,
                          const double beta,
                          const double betaLimit,
                          const double mu,
                          const double muRate,
                          const double maxYawRate,
                          const double yawBias,
                          const Vector<double>& unit_rsat,
                          const Vector<double>& unit_rsun,
                          double& modeledYaw);

        bool nightManeuver(const CommonTime& epoch,
                           const double beta,
                           const double betaLimit,
                           const double mu,
                           const double muRate,
                           const double maxYawRate,
                           const double yawBias,
                           const Vector<double>& unit_rsat,
                           const Vector<double>& unit_rsun,
                           double& modeledYaw);

        bool shadowManeuver(const CommonTime& epoch,
                            const double beta,
                            const double betaLimit,
                            const double mu,
                            const double muRate,
                            const double maxYawRate,
                            const double yawBias,
                            const Vector<double>& unit_rsat,
                            const Vector<double>& unit_rsun,
                            double& modeledYaw);

    private:

        /// Pointer to ReferenceSystem
        ReferenceSystem* pRefSys;

        /// Pointer to SolarSystem
        SolarSystem* pSolSys;

        /// Pointer to AntexReader
        AntexReader* pAntexReader;

        double nominalYaw;
        double modeledYaw;

        satYawDataMap satYawData;
        satVectorMap satOrbits;

    }; // End of class 'ComputeSatAttitude'

    //@}

    }  // End of namespace gpstk

#endif   // COMPUTE_SAT_ATTITUDE_HPP
