
#ifndef KeepSystems_HPP
#define KeepSystems_HPP

#include "gnssData.hpp"
#include "SatelliteSystem.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class keep the observations of the given satellite systems.
       *
       * This class is meant to be used with the GNSS data structures (GDS)
       * objects found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Define a linear combination to compute PC using C1
       *
       *      // Load observation data
       *   std::fstream rin("ebre0300.02o");
       * 
       *   std::string sysStr = "G"
       *
       *   KeepSystems chooseSys;
       *   chooseSys.setSystems(sysStr);
       *
       *   Rx3ObsData rxData;
       *
       *   while(true)
       *   {
       *      rin >> rxData;
       *      chooseSys.Process(rxData);
       *   }
       *
       * @endcode
       *
       */
    class KeepSystems                         
    {
    public:

         /// Default constructor
        KeepSystems()
        { 
           string sysStr = "G" ;
           setSystems(sysStr);
        };


         /** Common constructor
          *
          * @param linearComb   Linear combination to be computed.
          */
        KeepSystems(std::string sysStr)
        {
            setSystems(sysStr);
        };


        virtual void setSystems(std::string sysStr);


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            noexcept(false);


        virtual void Process(gnssRinex& gRin)
            noexcept(false)
        { 
            Process(gRin.header.epoch, gRin.body); 
        };

         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~KeepSystems() {};


    private:

        std::vector<SatelliteSystem> sysVec;

    }; // End class KeepSystems

    //@}

}  // End of namespace gpstk

#endif   // KeepSystems_HPP
