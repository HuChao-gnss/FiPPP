#ifndef GnssRinex_HPP
#define GnssRinex_HPP

#include <vector>
#include <list>
#include <map>

#include "CommonTime.hpp"
#include "YDSTime.hpp"
#include "RinexDatum.hpp"
#include "DataStructures.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "DataStructures.hpp"
#include "SatelliteSystem.hpp"
#include "FreqConsts.hpp"
//#include "Variable.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
   /// Defines a header containing the source and epoch of data, plus extra
   /// data extracted from a Rinex file
   /// only keep useful data
   struct gnssRinexHeader 
   {
      gnssRinexHeader()
      {
          enuRotateMatrix.resize(3, 3);
          neuRotateMatrix.resize(3, 3);
      };

      /// Destructor
      virtual ~gnssRinexHeader() {};

      CommonTime    epoch, beginEpoch, endEpoch;
      short         epochFlag;
      SourceID      source;
      string        antType;
      Triple        antennaDeltaHEN;
      Triple        antennaPosition;
      SysTypesMap   mapObsTypes;

      // added by ragnwang
      Triple    rcvPos; // spp result
      Triple    refPos;
      string    system;
      double    interval;
      std::map<TypeID, double> dcdtr;
      Vector<double> dops;  // GDOP, PDOP, HDOP, VDOP
      // ecef2enu rotate matrix; Note, converted to 'enu'
      Matrix<double> enuRotateMatrix;   // enu = dxyz * enuRotateMatrix
      // ecef2neu rotate matrix; Note, converted to 'neu'
      Matrix<double> neuRotateMatrix;   // neu = dxyz * neuRotateMatrix
      ////////////////////////////////////////////
      std::map<RinexSatID/*SatID*/, int> glonassFreqNo;

      // satellite-indexed types
      std::map<RinexSatID/*SatID*/, TypeIDVec> satTypes;
      std::map<RinexSatID/*SatID*/, TypeIDVec> satShortTypes;

   }; // End of struct 'sourceRinexHeader'

   struct gnssRinex : gnssData<gnssRinexHeader, satTypeValueMap>
   {
        /// Default constructor
       gnssRinex() {};

        /** Adds 'Rx3ObsData' object data to this structure.
         *
         * @param gds     gnssRinex object containing data to be added.
         */
       gnssRinex(const Rinex3ObsData& rxData )
       {
           (*this).header.epoch = rxData.time;
           (*this).header.beginEpoch = rxData.auxHeader.beginEpoch;
           (*this).header.endEpoch = rxData.auxHeader.endEpoch;
           (*this).header.epochFlag = rxData.epochFlag;
           (*this).header.source  = SourceID(rxData.auxHeader.fileSys, rxData.auxHeader.markerName);
           (*this).header.antType = rxData.auxHeader.antType;
           (*this).header.antennaDeltaHEN = rxData.auxHeader.antennaDeltaHEN;
           (*this).header.antennaPosition = rxData.auxHeader.antennaPosition;
           (*this).header.interval = rxData.auxHeader.interval;
           for (auto& isys : rxData.auxHeader.mapObsTypes)
           {
               SysTypesMap::iterator sysType = (*this).header.mapObsTypes.find(isys.first);
               if (sysType == (*this).header.mapObsTypes.end())
               {
                   TypeIDVec typeIdVec;
                   sysType = (*this).header.mapObsTypes.emplace(isys.first, typeIdVec).first;
               }
               for (auto& itype : isys.second)
               {
                   TypeID typeId(itype.asString() + isys.first[0]);
                   sysType->second.push_back(typeId);
               }
           }

           //(*this).header.mapObsTypes   = rxData.header.mapObsTypes;
           (*this).header.glonassFreqNo = rxData.auxHeader.glonassFreqNo;

           satTypeValueMap satTypeValue;
           for (auto& isat : rxData.obs)
           {
               RinexSatID satId = isat.first;
               satTypeValueMap::iterator satDatas = satTypeValue.find(isat.first);
               if (satDatas == satTypeValue.end())
               {
                   typeValueMap typeValMap;
                   satDatas = satTypeValue.emplace(isat.first, typeValMap).first;
                   (*this).header.satTypes[isat.first].clear();
                   (*this).header.satShortTypes[isat.first].clear();
               }
               typeValueMap typeValue;
               std::string shortSys(1, asChar(isat.first.system));
               SysTypesMap::iterator satTypes = (*this).header.mapObsTypes.find(shortSys);
               if (satTypes == (*this).header.mapObsTypes.end())
               {
                   continue;
               }

               for (int i = 0 ;i < satTypes->second.size();++i)
               {
                   if (isat.second[i].dataBlank == true)
                   {
                       continue;
                   }
                   std::string longTypeStr = satTypes->second[i].asString();
				   double obsData = isat.second[i].data;
				   if (longTypeStr[0] == 'L')
				   {
					   double welve = getWavelength(isat.first.system, int(longTypeStr[1]) - 48);
					   obsData = obsData * welve;
				   }
                   satDatas->second.emplace(satTypes->second[i], obsData);
                   (*this).header.satTypes[isat.first].push_back(satTypes->second[i]);
                   
				   TypeID shortType = TypeID(longTypeStr.substr(0, 2) + longTypeStr.substr(3, 1));
                   auto isExist = find((*this).header.satShortTypes[isat.first].begin(), (*this).header.satShortTypes[isat.first].end(), shortType);
                   if (isExist == (*this).header.satShortTypes[isat.first].end())
                   {
                       (*this).header.satShortTypes[isat.first].push_back(shortType);
                   }
               }
           }
// 
//            (*this).header.satTypes      = rxData.satTypes;
//            (*this).header.satShortTypes = rxData.satShortTypes;
           (*this).body = satTypeValue;
       };

        /** Adds 'Rx3ObsData' object data to this structure.
         *
         * @param gds     gnssRinex object containing data to be added.
         */
       gnssRinex& addRxData( const Rinex3ObsData& rxData )
       {
		   (*this).header.epoch = rxData.time;
		   (*this).header.epochFlag = rxData.epochFlag;
		   (*this).header.source = SourceID(rxData.auxHeader.fileSys, rxData.auxHeader.markerName);
		   (*this).header.antType = rxData.auxHeader.antType;
		   (*this).header.antennaDeltaHEN = rxData.auxHeader.antennaDeltaHEN;
		   (*this).header.antennaPosition = rxData.auxHeader.antennaPosition;
		   for (auto& isys : rxData.auxHeader.mapObsTypes)
		   {
			   SysTypesMap::iterator sysType = (*this).header.mapObsTypes.find(isys.first);
			   if (sysType == (*this).header.mapObsTypes.end())
			   {
				   TypeIDVec typeIdVec;
				   sysType = (*this).header.mapObsTypes.emplace(isys.first, typeIdVec).first;
			   }
			   for (auto& itype : isys.second)
			   {
                   TypeID typeID(itype.asString() + isys.first);
				   sysType->second.push_back(typeID);
			   }
		   }

		   //(*this).header.mapObsTypes   = rxData.header.mapObsTypes;
		   (*this).header.glonassFreqNo = rxData.auxHeader.glonassFreqNo;

		   satTypeValueMap satTypeValue;
		   for (auto& isat : rxData.obs)
		   {
			   RinexSatID satId = isat.first;
			   satTypeValueMap::iterator satDatas = satTypeValue.find(isat.first);
			   if (satDatas == satTypeValue.end())
			   {
				   typeValueMap typeValMap;
				   satDatas = satTypeValue.emplace(isat.first, typeValMap).first;
				   (*this).header.satTypes[isat.first].clear();
				   (*this).header.satShortTypes[isat.first].clear();
			   }
			   typeValueMap typeValue;
			   std::string shortSys = to_string(asChar(isat.first.system));
			   SysTypesMap::iterator satTypes = (*this).header.mapObsTypes.find(shortSys);
			   if (satTypes == (*this).header.mapObsTypes.end())
			   {
				   continue;
			   }

			   for (int i = 0; i < satTypes->second.size(); ++i)
			   {
				   if (isat.second[i].dataBlank == true)
				   {
					   continue;
				   }

				   satDatas->second.emplace(satTypes->second[i], isat.second[i].data);
				   (*this).header.satTypes[isat.first].push_back(satTypes->second[i]);

				   std::string longTypeStr = satTypes->second[i].asString();
				   TypeID shortType = TypeID(longTypeStr.substr(0, 2) + longTypeStr.substr(3, 1));
				   auto isExist = find((*this).header.satShortTypes[isat.first].begin(), (*this).header.satShortTypes[isat.first].end(), shortType);
				   if (isExist == (*this).header.satShortTypes[isat.first].end())
				   {
					   (*this).header.satShortTypes[isat.first].push_back(shortType);
				   }
			   }
		   }
		   (*this).body = satTypeValue;

           return (*this);

       }

       virtual std::ostream& dump(std::ostream& s, int detail=1) const
       {
           s << YDSTime((*this).header.epoch) << " ";
           s << (*this).header.epochFlag << " ";
           s << (*this).header.source << " ";
           s << (*this).header.antType << " ";
           s << (*this).header.antennaDeltaHEN << " ";
           s << (*this).header.antennaPosition << " ";
           s << endl;
           if(detail)
           {
               s << "mapObsTypes" << endl;
               for(auto sys=(*this).header.mapObsTypes.begin(); 
                        sys!=(*this).header.mapObsTypes.end(); sys++)
               {
                   s << (*sys).first << " ";
                   for(auto type=(*sys).second.begin();type!=(*sys).second.end();type++)
                   {
                       s << (*type) << " ";
                   }
                   s << endl;
               }
               s << "satTypes:" << endl;
               for(auto satIt=(*this).header.satTypes.begin();
                        satIt!=(*this).header.satTypes.end(); 
                        satIt++)
               {
                   s << (satIt)->first << " ";
                   for(auto type=(*satIt).second.begin();type!=(*satIt).second.end();type++)
                   {
                       s << (*type) << " ";
                   }
                   s << endl;
               }
           }

           (*this).body.dump(s, detail);
           return s;
       };

                /// Return the number of satellites available.
        size_t numSats() const
        { return (*this).body.size(); }

        SatIDSet getSatID() const
        {
            SatIDSet satSet;
            satSet = (*this).body.getSatID();
            return satSet;
        };


        gnssRinex& keepOnlyTypeID(TypeID& type)
        {
            (*this).body.keepOnlyTypeID(type);
            return (*this);
        };

        gnssRinex& keepOnlyTypeID(TypeIDSet& typeSet)
        {
            (*this).body.keepOnlyTypeID(typeSet);
            return (*this);
        };

          // Return a satTypeValueMap with only these types of data.
          // @param typeSet Set (TypeIDSet) containing the types of data
          //                to be extracted.
        inline satValueMap extractTypeID(const TypeID& type) const
        {
            satValueMap theMap;
            for( satTypeValueMap::const_iterator it = (*this).body.begin();
                 it != (*this).body.end();
                 ++it )
            {

                typeValueMap::const_iterator itObs( (*it).second.find(type) );
                if( itObs != (*it).second.end() )
                {
                    theMap[(*it).first] = (*itObs).second;
                }
            }

            return theMap;

        }  // End of method 'satTypeValueMap::extractTypeID()'

        /** Return the data value (double) corresponding to provided SatID
          *  and TypeID.
          *
          * @param satellite     Satellite to be looked for.
          * @param type          Type to be looked for.
          */
        double getValue( const SatID& satellite,
                         const TypeID& type ) const
            noexcept(false)
        { return (*this).body.getValue( satellite, type ); }


        inline Vector<double> getVectorOfTypeID(const TypeID& type) const
        {
            Vector<double> dataVec;
            dataVec = (*this).body.getVectorOfTypeID(type);
            return dataVec;
        };

        Matrix<double> getMatrixOfTypes(const TypeIDSet& typeSet) const
        { return ( (*this).body.getMatrixOfTypes(typeSet) ); }

        Matrix<double> getMatrixOfTypes(const TypeIDVec& typeVec) const
        {
            return ( (*this).body.getMatrixOfTypes(typeVec) );
        }

         /** Modifies this object, adding one vector of data with this type,
          *  one value per satellite.
          *
          * If type already exists, data is overwritten. If the number of
          * values does not match with the number of satellites, a
          * NumberOfSatsMismatch exception is thrown.
          *
          * Given that dataVector does not store information about the
          * satellites the values correspond to, the user is held responsible
          * for having the data values stored in dataVector in the proper order
          * regarding the SatIDs in this object.
          *
          * @param type          Type of data to be added.
          * @param dataVector    GPSTk Vector containing the data to be added.
          */
        gnssRinex& insertTypeIDVector( const TypeID& type,
                                              const Vector<double> dataVector )
            noexcept(false)
        { (*this).body.insertTypeIDVector(type, dataVector); return (*this); }

       ~gnssRinex() {};

   };

   typedef std::map<CommonTime, gnssRinex> epochRinexMap;

   /// Defines a header containing the source and epoch of data, plus extra
   /// data extracted from a Rinex file
   struct sourceRinexHeader 
   {
      sourceRinexHeader()
      {};

      /// Destructor
      virtual ~sourceRinexHeader() {};

      /// to 
      CommonTime epoch;
      SourceIDSet  sourceSet;
      std::map<SourceID, short> sourceEpochFlag;
      std::map<SourceID, string> sourceAntType;
      std::map<SourceID, Triple> sourceAntennaDeltaHEN;
      std::map<SourceID, Triple> sourceAntennaPosition;
      std::map<SourceID, SysTypesMap> sourceMapObsTypes;

      // satellite-indexed types
      std::map<SourceID, std::map<RinexSatID/*SatID*/, int>> sourceGlonassFreqNo;
      std::map<SourceID, std::map<RinexSatID/*SatID*/, TypeIDVec>> sourceSatTypes;
      std::map<SourceID, std::map<RinexSatID/*SatID*/, TypeIDVec>> sourceSatShortTypes;


   }; // End of struct 'sourceRinexHeader'


     /// GNSS data structure consisting in a map with CommonTime as keys, and
     /// sourceDataMap as elements.
   struct  sourceRinex : gnssData<sourceRinexHeader, sourceDataMap>
   {

        /// Default constructor
       sourceRinex() {};

        /** Adds 'Rx3ObsData' object data to this structure.
         *
         * @param gds     gnssRinex object containing data to be added.
         */
       sourceRinex& addGnssRinex( const gnssRinex& gRin )
       {
           // header
           (*this).header.epoch = gRin.header.epoch;
           (*this).header.sourceSet.insert(gRin.header.source);
           (*this).header.sourceEpochFlag[gRin.header.source] = gRin.header.epochFlag;
           (*this).header.sourceAntType[gRin.header.source] = gRin.header.antType;
           (*this).header.sourceAntennaDeltaHEN[gRin.header.source] = gRin.header.antennaDeltaHEN;
           (*this).header.sourceAntennaPosition[gRin.header.source] = gRin.header.antennaPosition;
           (*this).header.sourceMapObsTypes[gRin.header.source] = gRin.header.mapObsTypes;
           (*this).header.sourceGlonassFreqNo[gRin.header.source] = gRin.header.glonassFreqNo;
           (*this).header.sourceSatTypes[gRin.header.source] = gRin.header.satTypes;
           (*this).header.sourceSatShortTypes[gRin.header.source] = gRin.header.satShortTypes;

           // data
           (*this).body[gRin.header.source] = gRin.body; 
           return *this;
       };

        /** Get a set with all the SourceID's in this data structure.
         *
         * @warning If current 'sourceRinex' is big, this could be a very
         * costly operation.
         */
       SourceIDSet getSourceIDSet( void ) const
       {
           // SourceID set to be returned
           SourceIDSet toReturn;

           sourceDataMap sdMap( (*this).body );

            // Iterate through all items in the sourceRinex
           for(sourceDataMap::const_iterator it = sdMap.begin();
               it != sdMap.end();
               ++it)
           {
              // Add current SourceID to 'toReturn'
              toReturn.insert( (*it).first );

           }  // End of 'for( sourceRinex::const_iterator it = gdMap.begin(); ...'

           return toReturn;
       };

        /** Get a set with all the SatID's in this data structure.
         *
         * @warning If current 'sourceRinex' is big, this could be a very
         * costly operation.
         */
       SatIDSet getSatIDSet( void ) const
       {

            // SatID set to be returned
           SatIDSet toReturn;

           sourceDataMap sdMap( (*this).body );

           // Then, iterate through corresponding 'sourceDataMap'
           for(sourceDataMap::const_iterator sdmIter = sdMap.begin();
               sdmIter != sdMap.end();
               ++sdmIter)
           {

               // Finally, iterate through corresponding 'satTypeValueMap'
               for(satTypeValueMap::const_iterator stvmIter =
                                                     (*sdmIter).second.begin();
                   stvmIter != (*sdmIter).second.end();
                   ++stvmIter)
               {

                   // Add current SatID to 'toReturn'
                   toReturn.insert( (*stvmIter).first );

               }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

           }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

           return toReturn;
       };

        /// Destructor.
       virtual ~sourceRinex() {};

   };    // End of 'sourceRinex'

}

#endif
