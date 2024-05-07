#pragma ident "$Id$"

#include "CorrectPhaseBiases.hpp"
#include "GNSSconstants.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

      // Returns a string identifying this object.
   string CorrectPhaseBiases::getClassName() const
   { return "CorrectPhaseBiases"; }

   
      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CorrectPhaseBiases::Process( const CommonTime& time,
                                                std::map<RinexSatID, TypeIDVec>& satTypes,
                                                satTypeValueMap& gData )
      noexcept(false)
   {
	   try
	   {

		   SatIDSet satRejectedSet;

		   // Loop through all the satellites
		   for(auto it = gData.begin(); it != gData.end(); ++it)
		   {

			   SatID sat = it->first;
               TypeIDVec typeVec = satTypes[sat];
               TypeIDVec goodTypes = typeVec;

               if(debug)
               {
                   cout << "size:" << typeVec.size() << endl;
               }

               for(int i=0; i<typeVec.size(); i++)
               {
                   TypeID obsType = typeVec[i];

                   string typeStr = obsType.asString();

                   if(debug)
                   {
                       cout << "typeStr:" << typeStr << endl;
                   }

                   // only correct Phase biases
                   if(typeStr[0] == 'L')
                   {
                       // now, let's get the osb type1 and type2
                       string type1 = typeStr.substr(0,3);

                       // 3-length blank string
                       string type2(3,' ');

                       if(debug)
                       {
                           cout << getClassName() << " "
                                << sat << " "
                                << type1 << " "
                                << type2 << " "
                                << YDSTime(time) << " "
                                << endl;
                       }

                       double phaseBias;
                       try
                       {
                           phaseBias = (*pBiasStore).getSatBias(sat, type1, type2, time);

                           if(debug)
                           {
                               cout << "sat:" << sat 
                                    << "type1:" << type1 
                                    << "type2:" << type2 
                                    << "phase bias:" << phaseBias 
                                    << endl;
                           }

                           // convert ns to m
                           phaseBias = phaseBias * 1.0E-9 * C_MPS ;

                           if(debug)
                           {
                               cout << "phaseBias:" << phaseBias << endl;
                           }

                           // correct phase obs with the bias;
                           // according to the definition of "CODE" bias
                           // O(meas) = O(computed) + bias;
                           (*it).second[obsType] -= phaseBias;
                       }
                       catch(Exception& e)
                       {
                           // remove code from satTypeValueData
                           (*it).second.removeTypeID(obsType);

                           // remove code type from goodTypes
                           vector<TypeID>::iterator typeIt 
                               = find(goodTypes.begin(), goodTypes.end(), obsType);

                           if(typeIt != goodTypes.end())
                           {
                              goodTypes.erase(typeIt);
                           }
                       }

                       // now, let's correct biases for current code type
                   }
               }

               // updat satTypes
               satTypes[sat] = goodTypes;

		   }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

		   // Let's remove satellites without all TypeID's
		   gData.removeSatID(satRejectedSet);

		   return gData;

	   }
	   catch(Exception& u)
	   {

		   // Throw an exception if something unexpected happens
		   ProcessingException e( getClassName() + ":"
			   + u.what() );

		   GPSTK_THROW(e);

	   }

   }  // End of method 'CorrectPhaseBiases::Process()'


}  // End of namespace gpstk

