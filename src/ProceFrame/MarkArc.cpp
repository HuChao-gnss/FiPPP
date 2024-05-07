#pragma ident "$Id$"

/**
 * @file MarkArc.cpp
 * This class keeps track of satellite arcs caused by cycle slips.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================

//
// Modify by Chao Hu, Ragn Wang.
//

#include "MarkArc.hpp"

#define debug 0

namespace gpstk
{

      // Return a string identifying this object.
    std::string MarkArc::getClassName() const
    { return "MarkArc"; }


      /* Method to get the arc changed epoch.
       * @param sat              Interested SatID.
       */
    CommonTime MarkArc::getArcChangedEpoch(const SatID& sat)
    {
        SatArcData::iterator it = m_satArcData.find(sat);
        if(it != m_satArcData.end())
        {
            return it->second.arcChangeTime;
        }
        else
        {
            return CommonTime::BEGINNING_OF_TIME;
        }

    }  // End of method 'MarkArc::getArcChangedEpoch()'


      /* Method to get the arc changed epoch.
       * @param source           Interested SourceID.
       * @param sat              Interested SatID.
       */
    CommonTime MarkArc::getArcChangedEpoch(const SourceID& source,
                                                const SatID& sat)
    {
        SatArcDataMap::iterator it = m_satArcDataMap.find(source);

        if(it != m_satArcDataMap.end())
        {
            m_satArcData = it->second;

            return getArcChangedEpoch( sat );
        }
        else
        {
            return CommonTime::BEGINNING_OF_TIME;
        }

    }  // End of method 'MarkArc::getArcChangedEpoch()'


      /* Return a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& MarkArc::Process( const CommonTime& epoch,
                                            satTypeValueMap& gData )
        noexcept(false)
    {
        try
        {
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for ( satTypeValueMap::iterator it = gData.begin();
                  it != gData.end();
                  ++it )
            {
                SatID sat = (*it).first;

                // Check if satellite currently has entries
                SatArcData::const_iterator itArc( m_satArcData.find( sat ) );
                if( itArc == m_satArcData.end() )
                {
                    // If it doesn't have an entry, insert one
                    m_satArcData[sat].arcChangeTime = CommonTime::BEGINNING_OF_TIME;
                    m_satArcData[sat].arcNum = 0.0;
                }

  
                // Check if there was a cycle slip
                // compatible with different cycle-slip method
                if ( (*it).second.find(TypeID::CSFlag) != (*it).second.end() )
                {
  
                    if(csDataStream!=NULL)
                    {
                        (*csDataStream) 
                            << sat  << " "
                            << YDSTime(epoch) << " "
                            << (*it).second[TypeID::CSFlag]<< " "
                            << endl;
                        
                    }
  
                    if((*it).second(TypeID::CSFlag) > 0 )
                    {

                        if (m_satArcData[sat].arcNum != 0.0)
                        {/* added by ragn wang for recording sat by cycle slip. 2023/02/28 */
                            std::cout
                            << sat << " "
                            << "Cycle Slips are detected! At time: "
                            << static_cast<CivilTime>(epoch)
                            << std::endl;
                            satRejectedSet.insert(sat);
                        }

                        // Increment the value of "TypeID::satArc"
                        m_satArcData[sat].arcNum = m_satArcData[sat].arcNum + 1.0;

                        // Update arc change epoch
                        m_satArcData[sat].arcChangeTime = epoch;
                        
                    }
  
                }
                // We will insert satellite arc number
                (*it).second[TypeID::satArc] = m_satArcData[sat].arcNum;

            }

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'MarkArc::Process()'


}  // End of namespace gpstk
