
/**
 * @file ChooseOptimalObs.hpp
 * filter out satellite with observations out of bound.
 */

#ifndef ChooseOptimalObs_HPP
#define ChooseOptimalObs_HPP

#include "DataStructures.hpp"
#include "gnssData.hpp"
#include "Rinex3ObsHeader.hpp"

namespace gpstk
{

   class ChooseOptimalObs                         
   {
   public:

	   // Default constructor.
	   ChooseOptimalObs() 
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
          Process(gRin.header.mapObsTypes, gRin.header.satTypes,  gRin.body);
          return gRin;
      };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ChooseOptimalObs() {};


      /// This mapgives priority tracking codes for RINEX
      /// observations given the system and frequency;
      /// eg. priorityCodes['G']['1']="WCSLXPYMN"
      /// The only exception is there is no pseudorange (C) on GPS L1/L2 N (codeless)
      /// These tracking code characters are ORDERED, basically 'best' to 'worst'
      static CharCharStringMap priorityCodes;

      static CharCharStringMap init();


   }; // End of class 'ChooseOptimalObs'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_ChooseOptimalObs_HPP
