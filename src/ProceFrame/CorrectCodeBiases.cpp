#pragma ident "$Id$"

#include "CorrectCodeBiases.hpp"
#include "GNSSconstants.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

      // Returns a string identifying this object.
   string CorrectCodeBiases::getClassName() const
   { return "CorrectCodeBiases"; }

   
      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CorrectCodeBiases::Process( const CommonTime& time,
                                                std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                                satTypeValueMap& gData )
      noexcept(false)
   {
	   try
	   {
		   if (!obsCorr)
			   return gData;
		   SatIDSet satRejectedSet;

		   // Loop through all the satellites
		   for(auto it = gData.begin(); it != gData.end(); ++it)
		   {

			   /*SatID*/
               RinexSatID sat = it->first;
               SatelliteSystem sys(sat.system);
               TypeIDVec typeVec = satTypes[sat];
               TypeIDVec goodTypes = typeVec;
               // typeID for dcb correct
               vector<string> typeDcbVec;
               vector<TypeID> obsTypeDcbVec;
               for(int i=0; i<typeVec.size(); i++)
               {
                   TypeID obsType = typeVec[i];

                   // only correct code biases
                   string typeStr = obsType.asString();
                   if(typeStr[0] == 'C')
                   {
                       // now, let's get the osb type1 and type2
                       string type1 = typeStr.substr(0,3);
                       if (dcbCorr)
                       {    /* get type id for dcb corr */
                           typeDcbVec.push_back(type1);
                           obsTypeDcbVec.push_back(obsType);
                           continue;
                       }
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

                       double codeBias;
                       try
                       {
                           codeBias = (*pBiasStore).getSatBias(sat, type1, type2, time);

                           if(debug)
                           {
                               cout << "sat:" << sat 
                                    << "type1:" << type1 
                                    << "type2:" << type2 
                                    << "bias:" << codeBias 
                                    << endl;
                           }

                           // convert ns to m
                           codeBias = codeBias * 1.0E-9 * C_MPS ;

                           if(debug)
                           {
                               cout << "codeBias:" << codeBias << endl;
                           }

                           // correct code obs with the bias;
                           (*it).second[obsType] -= codeBias;
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

                   //
               }

               // correct dcb:
               if (dcbCorr)
               {
                   double dcb(0.), dcb1j(0.0), dcb12(0.0);
                   string f1, f2, comb, fc; // fc: first channel
                   map<string, double> dcbValues;
                   if (typeDcbVec.size() != 0)
                   {
                       try
                       {
						   // get dcb values.
						   bool bdsf(bdsfopt);
						   if(sys==SatelliteSystem::BeiDou&&sat.id<=18)  bdsf = false;
						   dcbValues = pBiasStore->getSatDCB(sat, typeDcbVec, time, bdsf);
						   //get first two frequency channel
						   getFirTChnel(sys, obsTypeDcbVec, f1, f2);
						   // get dcb12
						   comb = asChar(sys) + f1 + f2;
						   dcb12 = dcbValues[comb];

						   // for loop all the obsType
						   for (auto it_type = obsTypeDcbVec.begin();
							   it_type != obsTypeDcbVec.end();
							   it_type++)
						   {
							   TypeID obsType(*it_type);
							   string typeStr(obsType.asString().substr(1, 2));
							   // get dcb12
							   if (sys == SatelliteSystem::BeiDou&&bdsfopt&&sat.id>=19)
							   {    // alpha13DCB13 + beta14DCB14 - DCB1j
								   // 15268

								   double dcb13(dcbValues["C" + f1 + "2I"]);
								   double dcb14(dcbValues["C" + f1 + "6I"]);
								   comb = "C" + f1 + typeStr;
								   if (dcbValues.find(comb) == dcbValues.end()
									   && f1 != typeStr)
								   {
									   // remove code from satTypeValueData
									   TypeID obsTypeL("L" + obsType.asString().substr(1, 3));
									   (*it).second.removeTypeID(obsType);
									   (*it).second.removeTypeID(obsTypeL);

									   // remove code type from goodTypes
									   vector<TypeID>::iterator typeIt
										   = find(goodTypes.begin(), goodTypes.end(), obsType);

									   if (typeIt != goodTypes.end())
									   {
										   goodTypes.erase(typeIt);
									   }

									   vector<TypeID>::iterator typeItL
										   = find(goodTypes.begin(), goodTypes.end(), obsTypeL);
									   if (typeItL != goodTypes.end())
									   {
										   goodTypes.erase(typeItL);
									   }
									   continue;
								   }
								   // get DCB1j
								   dcb1j = dcbValues[comb];
								   // calc dcb
								   (*it).second[obsType] +=
									   (APHA_BDS_L2L6 * dcb13 + BETA_BDS_L2L6 * dcb14 - dcb1j) * 1.0E-9 * C_MPS;

							   }	// end of if (sys == SatelliteSystem::BeiDou && bdsfopt)
							   else
							   {
								   double beta(
									   sys == SatelliteSystem::GPS ? BETA_GPS_L1L2 :
									   (sys == SatelliteSystem::Galileo ? BETA_GAL_L1L5 :
										   (sys == SatelliteSystem::BeiDou ? BETA_BDS_L2L6 : 0))
								   );

								   comb = asChar(sys) + f1 + typeStr;
								   // get dcb1j
								   if (dcbValues.find(comb) == dcbValues.end()
									   && f1 != typeStr)
								   {
									   // remove code from satTypeValueData
									   TypeID obsTypeL("L" + obsType.asString().substr(1, 3));
									   (*it).second.removeTypeID(obsType);
									   (*it).second.removeTypeID(obsTypeL);

									   // remove code type from goodTypes
									   vector<TypeID>::iterator typeIt
										   = find(goodTypes.begin(), goodTypes.end(), obsType);
									   if (typeIt != goodTypes.end())
									   {
										   goodTypes.erase(typeIt);
									   }

									   vector<TypeID>::iterator typeItL
										   = find(goodTypes.begin(), goodTypes.end(), obsTypeL);
									   if (typeItL != goodTypes.end())
									   {
										   goodTypes.erase(typeItL);
									   }
									   continue;
								   }
								   dcb1j = dcbValues[comb];
								   (*it).second[obsType] += (beta * dcb12 - dcb1j) * 1.0E-9 * C_MPS;
							   }  // end of if (sys == SatelliteSystem::BeiDou && bdsfopt)

						   }    // end of for loop all the type
                       }
                       catch (Exception& u)
                       {
                           // Unable to get basic DCB value
                           // remove type
                           if (u.getErrorId()==1)
                           {
							   for (auto itType = obsTypeDcbVec.begin();
								   itType != obsTypeDcbVec.end();
								   ++itType)
							   {
								   TypeID obsTypeP(*itType);
								   TypeID obsTypeL("L" + obsTypeP.asString().substr(1, 3));
								   // remove obs type
								   (*it).second.removeTypeID(obsTypeP);
								   (*it).second.removeTypeID(obsTypeL);
								   // remove code type from goodTypes
								   vector<TypeID>::iterator typeIt
									   = find(goodTypes.begin(), goodTypes.end(), obsTypeP);
								   if (typeIt != goodTypes.end())
								   {
									   goodTypes.erase(typeIt);
								   }

								   vector<TypeID>::iterator typeItL
									   = find(goodTypes.begin(), goodTypes.end(), obsTypeL);
								   if (typeItL != goodTypes.end())
								   {
									   goodTypes.erase(typeItL);
								   }
							   }    // end of for loop
							   // remove sat
							   satRejectedSet.insert(sat);
							   continue;
                           }// end of if(u.getErrorId() == 1)
						   else
						   {
							   cerr << u.getLocation();
							   cerr << u.getText() << endl;
						   }
                       }
                           
                   }    // end of if (typeDcbVec.size() != 0)
               }    // end of if(dcbCorr)

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

   }  // End of method 'CorrectCodeBiases::Process()'


   // obstain first frequency channel
   void CorrectCodeBiases::getFirTChnel(const SatelliteSystem sys,
       const TypeIDVec& typeVec,
       string& a, string& b)
   {
       // for loop the types
       for (auto itType = typeVec.begin();
           itType != typeVec.end();
           ++itType)
       {
           string type(itType->asString());
           string f(type.substr(1, 1));

           if (sys == SatelliteSystem::BeiDou && !bdsfopt)
           {
               if (f == "2")   a = "2"+type.substr(2, 1);
               if (f == "6")   b = "6"+type.substr(2, 1);
           }
           else if (sys == SatelliteSystem::GPS)
           {
               if (f == "1")   a = "1"+type.substr(2, 1);
               if (f == "2")   b = "2"+type.substr(2, 1);
           }
           else if (sys == SatelliteSystem::Galileo || (sys == SatelliteSystem::BeiDou && bdsfopt))
           {
               if (f == "1")   a = "1"+type.substr(2, 1);
               if (f == "5")   b = "5"+type.substr(2, 1);
           }
       }    // end of for loop the types
	   // check the a and b value.
	   if (a==""&&b=="")
	   {
		   Exception e;
		   e.setErrorId(1);
		   e.addText("This satellite cannot determine its \
						first and second frequency channel");
		   GPSTK_THROW(e);
	   }
	   else if (a==""&&b!="")
	   {
		   if (sys==SatelliteSystem::BeiDou && !bdsfopt)
		   {
			   a = "2" + b.substr(b.size()-1,1);
		   }
		   else
		   {
			   a = "1" + b.substr(b.size()-1,1);
		   }
	   }	// end of if(a==""&&b=="")
   }    // end of getFirChnel


}  // End of namespace gpstk

