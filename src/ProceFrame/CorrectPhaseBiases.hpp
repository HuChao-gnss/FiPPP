#pragma ident "$$"

#ifndef GPSTK_CorrectPhaseBiases_HPP
#define GPSTK_CorrectPhaseBiases_HPP


#include "BiasDataReader.hpp"
#include "gnssData.hpp"

namespace gpstk
{

   /** Only read DLR/IGG daily MGEX DCB file, not DLR week update file
    *
    *
    */
   class CorrectPhaseBiases 
   {
      public :


           // Default constructr
		 CorrectPhaseBiases()    
			  : pBiasStore(NULL)
         {};

           /** Return a satTypeValueMap object, adding the new data generated
            *  when calling this object.
            *
            * @param  time     Epoch corresponding to the data.
            * @param  gData    Data object holding the data.
            */
         virtual satTypeValueMap& Process( const CommonTime& time,
                                           std::map<RinexSatID, TypeIDVec>& satTypes,
                                           satTypeValueMap& gData)
            noexcept(false);


         /** Returns a gnssRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
         virtual void Process(gnssRinex& gRin)
            noexcept(false)
         {
             Process(gRin.header.epoch, gRin.header.satTypes, gRin.body);
         };

         virtual void setBiasStore(BiasDataReader& dcbStore)
         {
            pBiasStore = &dcbStore;
         };

           // Returns a string identifying this object.
         virtual std::string getClassName () const;


           // Default deconstructr
         ~CorrectPhaseBiases(){};


      private:

		 // Object that reads and stores MGEX DCB data
		 BiasDataReader* pBiasStore;
         
   }; // End of class 'CorrectPhaseBiases'

      // @}

}; // end of namespace gpstk

#endif   // GPSTK_CorrectPhaseBiases_HPP
