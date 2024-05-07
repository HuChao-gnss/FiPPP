#pragma ident "$Id$"

/**
 * @file DetectCSMW.hpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
 */

#ifndef DetectCSMW_HPP
#define DetectCSMW_HPP


#include <list>
#include "DataStructures.hpp"
#include "gnssData.hpp"
#include "LinearCombinations.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to detect cycle slips using MW observables.
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
       *   ComputeMelbourneWubbena getMW;
       *   DetectCSMW markCSMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getMW >> markCSMW;
       *   }
       * @endcode
       *
       * The "DetectCSMW" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will decide if a cycle slip has happened
       * in the given observable.
       *
       * The algorithm will use MW observables, and the LLI1 and LLI2 indexes.
       * The result (a 1 if a cycle slip is found, 0 otherwise) will be stored
       * in the data structure both as the CSL1 and CSL2 indexes.
       *
       * In taking the decision, this algorithm will use criteria as the
       * maximum interval of time between two successive epochs and the
       * maximum number of Melbourne-Wubbena wavelengths allowed above or
       * below the MW combination average for that arc.
       *
       * The default values are usually fine, but you may change them with the
       * appropriate methods. This is of special importance for the maximum
       * interval time, that should be adjusted for your sampling rate. It is
       * 61 seconds by default, which is appropriate for 30 seconds per sample
       * RINEX observation files.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the cycle slip indexes inserted along
       * their corresponding satellites. Be warned that if a given satellite
       * does not have the observations required, it will be summarily deleted
       * from the data structure.
       *
       * You should be aware that the Melbourne-Wubbena combination is based
       * on a mix of code and phase observations, so it is very noisy.
       * Therefore, it has a tendency to yield a high number of false
       * positives if you are not careful with its parameters. Because of
       * this, the default parameters are very conservative, i.e., the
       * detector is NOT very sensitive by default.
       *
       * Best results are achieved when using this detector as a "backup"
       * detector for detectors based in LI combination, like this:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *   ComputeLI getLI;
       *   LICSDetector markCSLI;
       *   ComputeMelbourneWubbena getMW;
       *   DetectCSMW markCSMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getLI >> getMW >> markCSLI >> markCSMW;
       *   }
       * @endcode
       *
       * @sa LICSDetector.hpp and LICSDetector2.hpp for more information.
       *
       * \warning Cycle slip detectors are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       * references: 
       *   see Geoferry Blewitt(1990), GRL; 
       *
       */
    class DetectCSMW 
    {
    public:

        /// Default constructor, setting default parameters.
        DetectCSMW()
            : deltaTMax(61.0),
            minCycles(1.0)
        {
            sysTypeSet.clear();
        };


        /** Common constructor.
         *
         * @param mLambdas      Maximum deviation allowed before declaring
         *                      cycle slip (in number of Melbourne-Wubbena
         *                      wavelengths).
         * @param dtMax         Maximum interval of time allowed between two
         *                      successive epochs, in seconds.
         */
        DetectCSMW( const double& dtMax );


        /** Return a satTypeValueMap object, adding the new data generated
         *  when calling this object.
         *
         * @param epoch     Time of observations.
         * @param gData     Data object holding the data.
         * @param epochflag Epoch flag.
         */
        virtual satTypeValueMap& Process( const CommonTime& epoch,
                                          const CommonTime& firstEpoch,
                                          satTypeValueMap& gData,
                                          const short& epochflag = 0 )
            noexcept(false);

        virtual gnssRinex& Process(gnssRinex& gRin)
        {
            Process(gRin.header.epoch, gRin.header.beginEpoch, gRin.body);
            return gRin;
        };


        virtual void setDeltaTMax(double dt)
        {
            deltaTMax = dt;
        };

        virtual void setMinCycles(double cyc)
        {
            minCycles = cyc;
        };


        /** Method to set the obsType.
         *
         * @param type      ObsType
         */
        virtual DetectCSMW& addType(const SatelliteSystem& sys,
                                    const TypeID& type)
        {   
            sysTypeSet[sys].insert(type);
            return (*this); 
        };


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor
        virtual ~DetectCSMW() {};


    private:

        // wavelength for current MW-combination
        double wavelengthMW;


        // variance for current MW-combination
        double varianceMW;

        std::map<SatelliteSystem, TypeIDSet> sysTypeSet;

        /** Maximum interval of time allowed between two successive
         *  epochs, in seconds.
         */
        double deltaTMax;

        double minCycles;

        /// A structure used to store filter data for a SV.
        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData() : formerEpoch(CommonTime::BEGINNING_OF_TIME),
                           windowSize(0), meanMW(0.0)
            {};

            CommonTime formerEpoch; ///< The previous epoch time stamp.
            int windowSize;         ///< Size of current window, in samples.
            double meanMW;          ///< Accumulated mean value of combination.
            double varMW;           ///< Accumulated std value of combination.
        };

        typedef std::map<SatID, std::map<TypeID, filterData>> MWData;

        /// Map holding the information regarding every satellite
        MWData mwData;

        /** Method that implements the Melbourne-Wubbena cycle slip
         *  detection algorithm.
         *
         * @param epoch     Time of observations.
         * @param sat       SatID.
         * @param epochflag Epoch flag.
         * @param mw        Current MW observation value.
         */
        virtual double getDetection( const CommonTime& epoch,
                                     const CommonTime& firstepoch,
                                     const SatID& sat,
                                     const short& epochflag,
                                     const TypeID& mwType,
                                     const double& mw);


    }; // End of class 'DetectCSMW'

    //@}

}  // End of namespace gpstk

#endif   // DetectCSMW_HPP
