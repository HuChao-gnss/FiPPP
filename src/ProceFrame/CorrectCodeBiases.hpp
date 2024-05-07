#pragma ident "$ID$"

#ifndef GPSTK_CorrectCodeBiases_HPP
#define GPSTK_CorrectCodeBiases_HPP

#include "BiasDataReader.hpp"
#include "gnssData.hpp"

using namespace fippp;

namespace gpstk
{

   /** Only read DLR/IGG daily MGEX DCB file, not DLR week update file
    *
    *
    */
   class CorrectCodeBiases 
   {
      public :


           // Default constructr
		 CorrectCodeBiases()    
			  : pBiasStore(NULL),
             dcbCorr(false),
             bdsfopt(false)
         {};

           /** Return a satTypeValueMap object, adding the new data generated
            *  when calling this object.
            *
            * @param  time     Epoch corresponding to the data.
            * @param  gData    Data object holding the data.
            */
         virtual satTypeValueMap& Process( const CommonTime& time,
                                           std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
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


         virtual void setDCBStore(BiasDataReader& dcbStore)
         {
            pBiasStore = &dcbStore;
         };

         virtual void setDCBCorr(int corr)
         {
             obsCorr = (corr!=Options::NONE);
             dcbCorr = (corr==Options::DCB);
         }
         virtual void setBDSFf(bool bds3ff)
         {
             bdsfopt = bds3ff;
         }
           // Returns a string identifying this object.
         virtual std::string getClassName () const;


           // Default deconstructr
         ~CorrectCodeBiases(){};


      private:
            // Obtain the first frequency channel
          void getFirTChnel(const SatelliteSystem sys, 
              const TypeIDVec& typeVec, string& a, string& b);
		 // Object that reads and stores MGEX DCB data
          BiasDataReader* pBiasStore;
          bool obsCorr;
          bool dcbCorr;
          bool bdsfopt;
         
   }; // End of class 'CorrectCodeBiases'

      // @}

}; // end of namespace gpstk

#endif   // GPSTK_CorrectCodeBiases_HPP
