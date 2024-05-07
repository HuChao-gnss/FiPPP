


#ifndef ComputeSatPos_HPP
#define ComputeSatPos_HPP

#include "XvtStore.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /** This class is used to compute gnss satellite's relativity correction, 
       *  satellite position and velocity at transmission time.
       *
       * A typical way to use this class follows:
       *
       * @code
       * 
       * @endcode
       *
       *
       */
    class ComputeSatPos                         
    {
    public:

         /// Default constructor. Observable C1 will be used for computations
         /// and satellites with elevation less than 10 degrees will be
         /// deleted.
        ComputeSatPos()
            : pEphStore(NULL) 
        {};


         /** Explicit constructor, ephemeris to be used and whether TGD will
          *  be computed or not.
          *
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param applyTGD      Whether or not C1 observable will be
          *                      corrected from TGD effect.
          *
          */
        ComputeSatPos( XvtStore<SatID>& ephStore)
        {
            pEphStore = &ephStore;
        };


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process(const CommonTime& time,
                                         std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
                                         satTypeValueMap& gData)
            noexcept(false);


        virtual gnssRinex& Process(gnssRinex& gRin)
            noexcept(false)
        {
            Process(gRin.header.epoch, gRin.header.satShortTypes, gRin.body);
            return gRin;
        };


        virtual Xvt ComputeAtTransmitTime(const CommonTime& tr,
                                          const double& pr,
                                          const RinexSatID/*SatID*/& sat)
            noexcept(false);

        virtual void rotateEarth(double dt, Xvt& svPosVel);


         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor.
        virtual ~ComputeSatPos() {};


    protected:


        /// Pointer to XvtStore<SatID> object
        XvtStore<SatID>* pEphStore;

    }; // End of class 'ComputeSatPos'

      //@}

}  // End of namespace gpstk

#endif   // ComputeSatPos_HPP
