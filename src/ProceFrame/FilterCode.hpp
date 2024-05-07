

#ifndef FilterCode_HPP
#define FilterCode_HPP

#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class filters out satellites with observations grossly out of
       *  bounds.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *   std::fstream rin("ebre0300.02o");
       *
       *   FilterCode myFilter;
       *  
       *   gnssRinex gRin
       *
       *   while(true)
       *   {
       *      rin >> gRin;
       *      myFilter.Process(gRin);
       *   }
       *
       * @endcode
       *
       * Warning:
       * The filter will firstly check the satellite number stored
       * in 'gRin', if the number is too small, e.g. less than 2, the 
       * 'SVNumException' will be thrown, indicating data exception for current
       * epoch.
       *
       * The "FilterCode" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will check that the given code
       * observations are within some (preassigned) boundaries.
       *
       * By default, the algorithm will check C1 observables, the minimum limit
       * is 15000000.0 meters and the maximum limit is 30000000.0 meters. You
       * may change all these settings with the appropriate set methods.
       *
       * Also, you may set more than one observable to be checked by passing a
       * "TypeIDSet" object to the appropriate constructors or methods. For
       * instance:
       *
       * @code
       *   TypeIDSet typeSet;
       *   typeSet.insert(TypeID::P1);
       *   typeSet.insert(TypeID::P2);
       *
       *   myFilter.setFilteredType(typeSet);
       * @endcode
       *
       * Be warned that if a given satellite does not have the observations
       * required, or if their are out of bounds, the full satellite record
       * will be summarily deleted from the data structure.
       *
       */
   class FilterCode                         
   {
   public:

	   // Default constructor.
	   FilterCode() 
           : minLimit(15000000.0), maxLimit(45000000.0)
	   {};


         /** Explicit constructor
          *
          * @param type      TypeID to be filtered.
          * @param min       Minimum limit (in meters).
          * @param max       Maximum limit (in meters).
          */
      FilterCode( const double& min,
                         const double& max )
         : minLimit(min), maxLimit(max)
      {};



      /** Returns a gnssRinex object, filtering the target observables.
       *
       * @param gData    Data object holding the data.
       */
      virtual satTypeValueMap& Process( SysTypesMap& mapObsTypes,
                                        std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                        satTypeValueMap& gData) 
          noexcept(false);


      virtual gnssRinex& Process(gnssRinex& gRin) 
          noexcept(false)
      {
          Process(gRin.header.mapObsTypes, gRin.header.satTypes, gRin.body);
          return gRin;
      };


         /** Method to set the minimum limit.
          * @param min       Minimum limit (in meters).
          */
      virtual FilterCode& setMinLimit(const double& min)
      { minLimit = min; return (*this); };


         /** Method to set the maximum limit.
          * @param max       Maximum limit (in meters).
          */
      virtual FilterCode& setMaxLimit(const double& max)
      { maxLimit = max; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~FilterCode() {};


   protected:


         /** Checks that the value is within the given limits.
          * @param value     The value to be test
          *
          * @return
          *  True if check was OK.
          */
      virtual bool checkValue(const double& value) const
      { return ( (value>=minLimit) && (value<=maxLimit) ); };

         /// Minimum value allowed for input data (in meters).
      double minLimit;

         /// Maximum value allowed for input data (in meters).
      double maxLimit;


   }; // End of class 'FilterCode'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_FilterCodeHPP
