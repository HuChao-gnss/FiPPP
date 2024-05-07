#pragma ident "$Id$"

/**
 * @file MarkArc.hpp
 * This class keeps track of satellite arcs caused by cycle slips.
 */

#ifndef MarkArc_HPP
#define MarkArc_HPP

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

#include "CommonTime.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This class keeps track of satellite arcs caused by cycle slips.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *   LICSDetector markCSLI;
       *   MarkArc markArc;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> markCSLI >> markArc;
       *   }
       * @endcode
       *
       * The "MarkArc" object will visit every satellite in the GNSS
       * data structure that is "gRin" and, if a cycle slip has happened
       * (indicated by the corresponding CS flag), it will increase the
       * value of the corresponding "TypeID::satArc" type.
       *
       * \warning Be aware that this class DOES NOT apply cycle slip detection
       * algorithms, so you MUST invoke CS detection objects BEFORE calling
       * MarkArc objects.
       *
       * \warning Objects in this class store their internal state, so you
       * MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       * @sa LICSDetector.hpp and MWCSDetector.hpp for CS detection classes.
       *
       */
    class MarkArc                         
    {
    public:

        /// Default constructor. It will only watch "TypeID::CSL1" flag.
        MarkArc()
            :csDataStream(NULL)
        {};

        /** Method to get the arc changed epoch.
         * @param sat              Interested SatID.
         */
        virtual CommonTime getArcChangedEpoch(const SatID& sat);


        /** Method to get the arc changed epoch.
         * @param source           Interested SourceID.
         * @param sat              Interested SatID.
         */
        virtual CommonTime getArcChangedEpoch( const SourceID& source,
                                               const SatID& sat );


        /** Return a satTypeValueMap object, adding the new data generated
         *  when calling this object.
         *
         * @param epoch     Time of observations.
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process( const CommonTime& epoch,
                                          satTypeValueMap& gData )
            noexcept(false);


        virtual void Process(gnssRinex& gRin)
            noexcept(false)
        {
            Process(gRin.header.epoch, gRin.body);
        }


        virtual void setCSStream(std::ofstream& stream)
        {
            csDataStream = &stream;
        };


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor
        virtual ~MarkArc() {};


    private:


        struct arcData
        {
            arcData()
                : arcNum(0),
                  arcChangeTime(CommonTime::BEGINNING_OF_TIME),
                  arcNew(false)
            {};

            double arcNum;
            CommonTime arcChangeTime;
            bool arcNew;
        };


        typedef std::map<SatID, arcData> SatArcData;
        typedef std::map<SourceID, SatArcData> SatArcDataMap;

        std::ofstream* csDataStream;

        /// Map holding information regarding every satellite
        SatArcData m_satArcData;

        /// Map holding information for sourceRinex
        SatArcDataMap m_satArcDataMap;

    }; // End of class 'MarkArc'

    //@}

}  // End of namespace gpstk

#endif   // MarkArc_HPP
