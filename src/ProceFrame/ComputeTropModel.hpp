#pragma ident "$Id$"

#ifndef COMPUTETROPMODEL_HPP
#define COMPUTETROPMODEL_HPP

#include "TropModel.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This is a class to compute the main values related to a given
       *  GNSS tropospheric model.
       *
       * This class is intended to be used with GNSS Data Structures (GDS).
       * It is a more modular alternative to classes such as ModelObs
       * and ModelObsFixedStation.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Define the tropospheric model to be used
       *   NeillTropModel neillTM;
       *   neillTM.setReceiverLatitude(lat);
       *   neillTM.setReceiverHeight(height);
       *   neillTM.setDayOfYear(doy);
       *
       *      // Now, create the ComputeTropModel object
       *   ComputeTropModel computeTropo(neillTM);
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *   while(rin >> gRin)
       *   {
       *         // Apply the tropospheric model on the GDS
       *      gRin >> computeTropo;
       *   }
       *
       * @endcode
       *
       * The "ComputeTropModel" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * the main values of the corresponding tropospheric model: Total
       * tropospheric slant correction, dry vertical delay, wet vertical delay,
       * dry mapping function value and wet mapping function value.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites.
       *
       * Be warned that if a given satellite does not have the information
       * needed (mainly elevation), it will be summarily deleted from the data
       * structure. This also implies that if you try to use a
       * "ComputeTropModel" object without first defining the tropospheric
       * model, then ALL satellites will be deleted.
       *
       * @sa TropModel.hpp
       *
       */
    class ComputeTropModel                         
    {
    public:

         /// Default constructor.
        ComputeTropModel() : pTropModel(NULL) {};


         /** Explicit constructor.
          *
          * @param tropoModel   Tropospheric Model.
          *
          */
        ComputeTropModel(TropModel& tropoModel)
        { pTropModel = &tropoModel; };


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData)
            noexcept(false);

        virtual void Process(gnssRinex& gRin)
            noexcept(false)
        {
            Process(gRin.header.epoch, gRin.body);
        };

         /// Method to get a pointer to the default TropModel to be used
         /// with GNSS data structures.
        virtual TropModel *getTropModel() const
        { return pTropModel; };


         /** Method to set the default TropModel to be used with GNSS
          *  data structures.
          *
          * @param tropoModel   TropModel object to be used by default
          */
        virtual ComputeTropModel& setTropModel(TropModel& tropoModel)
        { 
            pTropModel = &tropoModel; return (*this); 
        };

        virtual ComputeTropModel& setAllParameters( const CommonTime& time,
                                                    const Position& rxPos, int flag=1)
        {
            if(pTropModel!=NULL)
            {
                (*pTropModel).setAllParameters(time, rxPos, flag);
            }
            else
            {
                InvalidRequest ir(getClassName() + "pTropModel is NULL");
                GPSTK_THROW(ir);
            }
            return (*this);
        };


        // Get source tropo map.
        virtual sourceValueMap getSourceTropoMap() const
        { return sourceTropoMap; }


         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor.
        virtual ~ComputeTropModel() {};


    private:

        double tropo;

        sourceValueMap sourceTropoMap;

         /// Pointer to default TropModel object when working with GNSS
         /// data structures.
        TropModel *pTropModel;


    }; // End of class 'ComputeTropModel'

      //@}

}  // End of namespace gpstk

#endif   // COMPUTETROPMODEL_HPP
