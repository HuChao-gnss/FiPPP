#pragma ident "$Id$"

/**
 * @file RequiredObs.hpp
 * This class filters out satellites with observations grossly out of bounds.
 */

#ifndef RequiredObs_HPP
#define RequiredObs_HPP

#include "SatID.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"
#include "AboutOptions.hpp"
using namespace fippp;
namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class filters out satellites that don't have the required
       *  observations.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   TypeIDSet requiredSet;
       *   requiredSet.insert(TypeID::C1);
       *   requiredSet.insert(TypeID::C2);
       *
       *   RequiredObs requireTypes(requiredSet);
       *
       *   while(rin >> gRin)
       *   {
       *         // Satellites without P1 and P2 observations will be deleted
       *      gRin >> requireTypes;
       *   }
       * @endcode
       *
       * The "RequiredObs" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will check that the previously
       * given list of observation types is meet by each satellite.
       *
       * Be warned that if a given satellite does not have ALL the observations
       * required, the full satellite record will be summarily deleted from the
       * data structure.
       *
       */
    class RequiredObs                         
    {
    public:

        /// Default constructor.
        RequiredObs()
        {};


        /** Explicit constructor
         *
         * @param sys       System to be required.
         * @param type      TypeID to be required.
         */
        RequiredObs( const SatelliteSystem& sys,
                            const TypeID& type )
        { setRequiredType(sys,type); };

        /** Explicit constructor
         *
         * @param sys       System to be required.
         * @param type      TypeID to be required.
         */
        RequiredObs( const TypeID& type,
                            const SatelliteSystem& sys=SatelliteSystem::GPS)
        { setRequiredType(sys,type); };


        /** Explicit constructor
         *
         * @param typeSet   Set of TypeID's to be required.
         */
        RequiredObs( const SatelliteSystem& sys,
                            const TypeIDSet& typeSet )
        {
            if(sys == SatelliteSystem::GPS)
            {
                requiredTypeSetOfGPS = typeSet;
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                requiredTypeSetOfGLO = typeSet;
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                requiredTypeSetOfGAL = typeSet;
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                requiredTypeSetOfBDS = typeSet;
            }
        };


        /** Return a satTypeValueMap object, checking the required
         *  observables.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
            noexcept(false);


        /** Method to add a TypeID to be required.
         *
         * @param type      Extra TypeID to be required.
         */
        virtual RequiredObs& addRequiredType( const SatelliteSystem& sys,
                                                     const TypeID& type )
        {
            if(sys == SatelliteSystem::GPS)
            {
                requiredTypeSetOfGPS.insert(type);
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                requiredTypeSetOfGLO.insert(type);
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                requiredTypeSetOfGAL.insert(type);
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                requiredTypeSetOfBDS.insert(type);
            }

            return (*this);
        };

        /** Method to add a TypeID to be required.
         *
         * @param type      Extra TypeID to be required.
         */
        virtual RequiredObs& addRequiredType( const TypeID& type,
                const SatelliteSystem& sys=SatelliteSystem::GPS)
        {
            if(sys == SatelliteSystem::GPS)
            {
                requiredTypeSetOfGPS.insert(type);
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                requiredTypeSetOfGLO.insert(type);
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                requiredTypeSetOfGAL.insert(type);
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                requiredTypeSetOfBDS.insert(type);
            }

            return (*this);
        };


        /** Method to add a set of TypeID's to be required.
         *
         * @param typeSet    Set of TypeID's to be required.
         */
        virtual RequiredObs& addRequiredType( const SatelliteSystem& sys,
                                                     const TypeIDSet& typeSet );


        /** Method to set a TypeID to be required. This method will erase
         *  previous required types.
         *
         * @param type      TypeID to be required.
         */
        virtual RequiredObs& setRequiredType( const SatelliteSystem& sys,
                                                     const TypeID& type )
        {
            if(sys == SatelliteSystem::GPS)
            {
                requiredTypeSetOfGPS.clear();
                requiredTypeSetOfGPS.insert(type);
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                requiredTypeSetOfGLO.clear();
                requiredTypeSetOfGLO.insert(type);
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                requiredTypeSetOfGAL.clear();
                requiredTypeSetOfGAL.insert(type);
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                requiredTypeSetOfBDS.clear();
                requiredTypeSetOfBDS.insert(type);
            }

            return (*this);
        };


         /** Method to set the TypeID's to be required. This method will erase
          *  previous types.
          *
          * @param typeSet       Set of TypeID's to be required.
          */
        virtual RequiredObs& setRequiredType( const SatelliteSystem& sys,
                                                     const TypeIDSet& typeSet )
        {
            if(sys == SatelliteSystem::GPS)
            {
                requiredTypeSetOfGPS.clear();
                requiredTypeSetOfGPS = typeSet;
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                requiredTypeSetOfGLO.clear();
                requiredTypeSetOfGLO = typeSet;
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                requiredTypeSetOfGAL.clear();
                requiredTypeSetOfGAL = typeSet;
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                requiredTypeSetOfBDS.clear();
                requiredTypeSetOfBDS = typeSet;
            }

            return (*this);
        };


        /// Method to get the set of TypeID's to be required.
        virtual TypeIDSet getRequiredType(const SatelliteSystem& sys) const
        {
            TypeIDSet ret;

            if(sys == SatelliteSystem::GPS)
            {
                ret = requiredTypeSetOfGPS;
            }
            else if(sys == SatelliteSystem::Glonass)
            {
                ret = requiredTypeSetOfGLO;
            }
            else if(sys == SatelliteSystem::Galileo)
            {
                ret = requiredTypeSetOfGAL;
            }
            else if(sys == SatelliteSystem::BeiDou)
            {
                ret = requiredTypeSetOfBDS;
            }

            return ret;
        };


        /** Return a gnssRinex object, checking the required observables.
         *
         * @param gData    Data object holding the data.
         */
        virtual void Process(gnssRinex& gData)
            noexcept(false)
        {
            Process(gData.body);
        };


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;

        void setOpts(const Options::options_t& opt)
        {
            opts = opt;
        }

        /// Destructor
        virtual ~RequiredObs() {};


    private:

        /// Set of types to be required
        TypeIDSet requiredTypeSetOfGPS;
        TypeIDSet requiredTypeSetOfGLO;
        TypeIDSet requiredTypeSetOfGAL;
        TypeIDSet requiredTypeSetOfBDS;
        Options::options_t opts;

    }; // End of class 'RequiredObs'

    //@}

}  // End of namespace gpstk

#endif   // RequiredObs_HPP
