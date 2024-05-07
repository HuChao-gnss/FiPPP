#pragma ident "$Id$"

/**
 * @file DataStructures.hpp
 * Set of several data structures to be used by other GPSTk classes.
 */

#ifndef GPSTK_DATASTRUCTURES_HPP
#define GPSTK_DATASTRUCTURES_HPP


#include <utility>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "StringUtils.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "GNSSconstants.hpp"
#include "TypeID.hpp"
#include "SatID.hpp"
#include "SourceID.hpp"
#include "RinexSatID.hpp"
using namespace std;

namespace gpstk
{

      /** @defgroup DataStructures GPSTk data structures
       *
       * This is a set of several data structures to be used by other
       * GPSTk classes.
       *
       * Each data structure is composed of a header and a body. The header
       * contains the information that is common to all the data stored in
       * the structure, and the body contains the data themselves along with
       * the information (indexes) necessary to access them.
       *
       * In this regard, four basic indexes are considered enough to
       * completely identify any GNSS value:
       *
       *  \li Epoch (CommonTime)
       *  \li Satellite (SatID)
       *  \li Type of value (TypeID)
       *
       * Moreover, all the GNSS data structures have two main parts:
       *
       *  \li Header: Containing the indexes that are common to all the values
       *              (sometimes with some extra information).
       *
       *  \li Body: Containing the GNSS values themselves, organized in
       *            std::maps.
       *
       * The general idea is to use the GNSS data structures like WHITE BOXES
       * that are able to carry all the important data around in an easy way,
       * in order to do something like the following to process GNSS data:
       *
       */

      //@{


      // First, we must declare some important exception objects


      /// Thrown when attempting to access a value and the corresponding TypeID
      /// does not exist in the map.
      /// @ingroup exceptiongroup

       /// Set containing TypeID objects.
    typedef std::set<TypeID> TypeIDSet;
    typedef std::vector<TypeID> TypeIDVec;

       /// Set containing SatID objects.
    typedef std::set<SatID> SatIDSet;

    typedef std::map<CommonTime, double> epochValueMap;

    typedef std::set<SourceID> SourceIDSet;

       /// Map holding sat with corresponding Vector<double>.
    typedef std::map< SatID, Vector<double> > satVectorMap;

       /// Map holding TypeID with corresponding numeric value.
    struct typeValueMap : std::map<TypeID, double>
    {

         /// Return the number of different types available.
        inline size_t numTypes() const
        { return (*this).size(); }


         /// Return a TypeIDSet with all the data types present in
         /// this object.
        TypeIDSet getTypeID() const;


         /// Return a typeValueMap with only this type of data.
         /// @param type Type of value to be extracted.
        typeValueMap extractTypeID(const TypeID& type) const;


         /// Return a typeValueMap with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data to
         ///                be extracted.
        typeValueMap extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
        typeValueMap& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
        typeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be removed.
        typeValueMap& removeTypeID(const TypeID& type)
        { (*this).erase(type); return (*this); }


         /// Modifies this object, removing these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
        typeValueMap& removeTypeID(const TypeIDSet& typeSet);


         /** Return the data value (double) corresponding to provided type.
          *
          * @param type       Type of value to be looked for.
          */
        double getValue(const TypeID& type) const
            noexcept(false);


         /// Return a reference to the data value (double) with
         /// corresponding type.
         /// @param type Type of value to be looked for.
        double& operator()(const TypeID& type)
            noexcept(false);


         /// Convenience output method
        virtual std::ostream& dump( std::ostream& s,
                                    int mode = 0 ) const;


         /// Destructor.
        virtual ~typeValueMap() {};

    };  // End typeValueMap

      /// Map holding SourceID with corresponding numeric value.
    struct sourceValueMap : std::map<SourceID, double>
    {

         /// Return the number of satellites available.
        size_t numSources() const
        { return (*this).size(); }


         /// Return a SourceIDSet with all the satellites present in this object.
        SourceIDSet getSourceID() const;


         /// Return a Vector with all the sources present in this object.
        Vector<SourceID> getVectorOfSourceID() const;


         /// Return a sourceValueMap with only this source.
         /// @param source Source to be extracted.
        sourceValueMap extractSourceID(const SourceID& source) const;


         /// Return a sourceValueMap with only these sources.
         /// @param sourceSet Set (SourceIDSet) containing the sources to
         ///               be extracted.
        sourceValueMap extractSourceID(const SourceIDSet& sourceSet) const;


         /// Modifies this object, keeping only this source.
         /// @param source Source to be kept.
        sourceValueMap& keepOnlySourceID(const SourceID& source);


         /// Modifies this object, keeping only these sources.
         /// @param sourceSet Set (SourceIDSet) containing the sources to be kept.
        sourceValueMap& keepOnlySourceID(const SourceIDSet& sourceSet);


         /// Modifies this object, removing this source.
         /// @param source Source to be removed.
        sourceValueMap& removeSourceID(const SourceID& source)
        { (*this).erase(source); return (*this); }


         /// Modifies this object, removing the given sources.
         /// @param sourceSet Set (SourceIDSet) containing the sources to
         ///               be removed.
        sourceValueMap& removeSourceID(const SourceIDSet& sourceSet);


         /** Return the data value (double) corresponding to provided SourceID.
          *
          * @param source     Source to be looked for.
          */
        double getValue(const SourceID& source) const
            noexcept(false);


         /// Return a reference to the data value (double) with
         /// corresponding SourceID.
         /// @param source Source to be looked for.
        double& operator()(const SourceID& source)
            noexcept(false);


         /// Destructor.
        virtual ~sourceValueMap() {};

    };  // End of 'sourceValueMap'



      /// Map holding SatID with corresponding numeric value.
    struct satValueMap : std::map<SatID, double>
    {

         /// Return the number of satellites available.
        size_t numSats() const
        { return (*this).size(); }


         /// Return a SatIDSet with all the satellites present in this object.
        SatIDSet getSatID() const;


         /// Return a Vector with all the satellites present in this object.
        Vector<SatID> getVectorOfSatID() const;


         /// Return a satValueMap with only this satellite.
         /// @param satellite Satellite to be extracted.
        satValueMap extractSatID(const SatID& satellite) const;


         /// Return a satValueMap with only one satellite, identified by
         /// the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
        satValueMap extractSatID( const int& p,
                                  const SatelliteSystem& s ) const;


         /// Return a satValueMap with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be extracted.
        satValueMap extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
        satValueMap& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
        satValueMap& keepOnlySatID( const int& p,
                                    const SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        satValueMap& keepOnlySatID(const SatIDSet& satSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
        satValueMap& removeSatID(const SatID& satellite)
        { (*this).erase(satellite); return (*this); }


         /// Modifies this object, removing the given satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be removed.
        satValueMap& removeSatID(const SatIDSet& satSet);


         /** Return the data value (double) corresponding to provided SatID.
          *
          * @param satellite     Satellite to be looked for.
          */
        double getValue(const SatID& satellite) const
            noexcept(false);


         /// Return a reference to the data value (double) with
         /// corresponding SatID.
         /// @param satellite Satellite to be looked for.
        double& operator()(const SatID& satellite)
            noexcept(false);

         /// Convenience output method
        virtual std::ostream& dump( std::ostream& s,
                                    int mode = 0 ) const;

         /// Destructor.
        virtual ~satValueMap() {};

    };  // End of 'satValueMap'


      /// Map holding SatID with corresponding typeValueMap.
    struct satTypeValueMap : std::map<RinexSatID, typeValueMap>
    {

         /// Return the number of available satellites.
        size_t numSats() const
        { return (*this).size(); }


         /// Return the number of available satellites.
        size_t numSats(SatelliteSystem sys) const
        { 
            int nsat(0);
            for(auto it=(*this).begin(); it!=(*this).end(); it++)
            {
                if((*it).first.system == sys)
                {
                    nsat ++;
                }
            }
            return nsat;
        }

         /** Return the total number of data elements in the map.
          * This method DOES NOT suppose that all the satellites have
          * the same number of type values.
          */
        size_t numElements() const;


         /// Return a SatIDSet with all the satellites present in this object.
        SatIDSet getSatID() const;


         /// Return a Vector with all the satellites present in this object.
        Vector<SatID> getVectorOfSatID() const;


         /// Return a TypeIDSet with all the data types present in
         /// this object.  This does not imply that all satellites have
         /// these types.
        TypeIDSet getTypeID() const;


         /// Return a satTypeValueMap with only this satellite.
         /// @param satellite Satellite to be extracted.
        satTypeValueMap extractSatID(const SatID& satellite) const;


         /// Return a satTypeValueMap with only one satellite, identified
         /// by the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
        satTypeValueMap extractSatID( const int& p,
                                      const SatelliteSystem& s) const;


         /// Return a satTypeValueMap with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be extracted.
        satTypeValueMap extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
        satTypeValueMap& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
        satTypeValueMap& keepOnlySatID( const int& p,
                                        const SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        satTypeValueMap& keepOnlySatID(const SatIDSet& satSet);


         /// Return a satTypeValueMap with only this type of value.
         /// @param type Type of value to be extracted.
        satTypeValueMap extractTypeID(const TypeID& type) const;


         /// Return a satTypeValueMap with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be extracted.
        satTypeValueMap extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
        satTypeValueMap& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
        satTypeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
        satTypeValueMap& removeSatID(const SatID& satellite)
        { (*this).erase(satellite); return (*this); }


         /// Modifies this object, removing these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be removed.
        satTypeValueMap& removeSatID(const SatIDSet& satSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be removed.
        satTypeValueMap& removeTypeID(const TypeID& type);


         /// Modifies this object, removing these types of data.
         /// @param typeSet Set(TypeIDSet) containing the types
         ///               to be removed.
        satTypeValueMap& removeTypeID(const TypeIDSet& typeSet);


         /// Return a GPSTk::Vector containing the data values with this type.
         /// @param type Type of value to be returned.
         /// This method returns zero if a given satellite does not have
         /// this type.
        Vector<double> getVectorOfTypeID(const TypeID& type) const;

         /// Return a GPSTk::Vector containing the data values with this type.
         /// @param type Type of value to be returned.
         /// This method returns zero if a given satellite does not have
         /// this type.
        Vector<double> getVectorOfTypeID(const TypeID& type, 
                                         SatelliteSystem sys) const;


         /// Return a GPSTk::Matrix containing the data values in this set.
         /// @param typeSet  TypeIDSet of values to be returned.
        Matrix<double> getMatrixOfTypes(const TypeIDSet& typeSet) const;

        Matrix<double> getMatrixOfTypes(const TypeIDVec& typeVec) const;

        
         /// Return a GPSTk::Matrix containing the data values in this set.
         /// @param typeSet  TypeIDSet of values to be returned.
        Matrix<double> getMatrixOfTypes(const TypeIDSet& typeSet,
                                        SatelliteSystem sys) const;


         /** Modifies this object, adding one vector of data with this type,
          *  one value per satellite.
          *
          * If type already exists, data is overwritten. If the number of
          * values does not match with the number of satellites, a
          * NumberOfSatsMismatch exception is thrown.
          *
          * Given that dataVector does not store information about the
          * satellites the values correspond to, the user is held responsible
          * for having the data values stored in dataVector in the proper
          * order regarding the SatIDs in this object.
          *
          * @param type          Type of data to be added.
          * @param dataVector    GPSTk Vector containing the data to be added.
          */
        satTypeValueMap& insertTypeIDVector( const TypeID& type,
                                             const Vector<double> dataVector )
            noexcept(false);


         /** Modifies this object, adding a matrix of data, one vector
          *  per satellite.
          *
          * If types already exists, data is overwritten. If the number of
          * rows in matrix does not match with the number of satellites, a
          * NumberOfSatsMismatch exception is thrown. If the number of columns
          * in matrix does not match with the number of types in typeSet, a
          * NumberOfTypesMismatch exception is thrown.
          *
          * Given that dataMatrix does not store information about the
          * satellites and types the values correspond to, the user is held
          * responsible for having those data values stored in dataMatrix in
          * the proper order regarding the SatIDs in this object and the
          * provided typeSet.
          *
          * @param typeSet       Set (TypeIDSet) containing the types of data
          *                      to be added.
          * @param dataMatrix    GPSTk Matrix containing the data to be added.
          */
        satTypeValueMap& insertMatrix( const TypeIDSet& typeSet,
                                       const Matrix<double> dataMatrix )
            noexcept(false);

         /** Return the data value (double) corresponding to provided SatID
          *  and TypeID.
          *
          * @param satellite     Satellite to be looked for.
          * @param type          Type to be looked for.
          */
        double getValue( const SatID& satellite,
                         const TypeID& type ) const
            noexcept(false);


         /// Return a reference to the typeValueMap with corresponding SatID.
         /// @param type Type of value to be look for.
        typeValueMap& operator()(const SatID& satellite) ;


         /// Convenience output method
        virtual std::ostream& dump( std::ostream& s,
                                    int mode = 0 ) const;


         /// Destructor.
        virtual ~satTypeValueMap() {};

    };  // End of 'satTypeValueMap'


      /// GNSS data structure consisting in a map with SourceID as keys, and
      /// satTypeValueMap as elements.
    struct sourceDataMap : std::map<SourceID, satTypeValueMap>
    {

         /// Default constructor
        sourceDataMap() {};


         /** Return the data value (double) corresponding to provided SourceID,
          *  SatID and TypeID.
          *
          * @param source        Source to be looked for.
          * @param satellite     Satellite to be looked for.
          * @param type          Type to be looked for.
          */
        double getValue( const SourceID& source,
                         const SatID& satellite,
                         const TypeID& type ) const
            noexcept(false);


         /** Get a set with all the SourceID's in this data structure.
          *
          * @warning If current 'sourceDataMap' is big, this could be a very
          * costly operation.
          */
        SourceIDSet getSourceIDSet( void ) const;


         /** Get a set with all the SatID's in this data structure.
          *
          * @warning If current 'sourceDataMap' is big, this could be a very
          * costly operation.
          */
        SatIDSet getSatIDSet( void ) const;


         /// Destructor.
        virtual ~sourceDataMap() {};

    };    // End of 'sourceDataMap'

      /// stream output for data 
    std::ostream& operator<<( std::ostream& s, const typeValueMap& tvMap);
    std::ostream& operator<<( std::ostream& s, const satValueMap& svMap );
    std::ostream& operator<<( std::ostream& s, const satTypeValueMap& stvMap);

       ///
       /// Basic gnssData structure.
       ///
    template <class HEADER_CLASS, class BODY_CLASS>
    struct gnssData
    {

        /// Header.
        HEADER_CLASS header;

        /// Body.
        BODY_CLASS   body;

        /// Default constructor.
        gnssData() {}

        /// Common constructor.
        gnssData( const HEADER_CLASS& h,
                  const BODY_CLASS& b )
        {
            header = h;
            body = b;
        }


        /// Copy constructor.
        template<class H, class B>
        gnssData(const gnssData<H,B>& g)
        {
            header = g.header;
            body = g.body;
        }


        /// Destructor.
        virtual ~gnssData() {};

    };  // End of 'gnssData'


      /// Object defining the structure of a GNSS equation. The header is the
      /// prefit and the body is a TypeIDSet containing the unknowns.
    struct  gnssEquationDefinition : gnssData<TypeID, TypeIDSet>
    {

         /// Default constructor.
        gnssEquationDefinition() {};


         /// Common constructor.
        gnssEquationDefinition( const TypeID& h,
                                const TypeIDSet& b )
        {
            header = h;
            body   = b;
        }


         /// Destructor.
        virtual ~gnssEquationDefinition() {};


    };  // End of 'gnssEquationDefinition'



      /// Object defining the structure of a GNSS linear combination. The
      /// header is the result type and the body is a typeValueMap containing
      /// the GNSS data types to be combined plus corresponding coefficients.
    struct  gnssLinearCombination : gnssData<TypeID, typeValueMap>
    {

        /// Default constructor.
        gnssLinearCombination() 
        {
            optionalTypes.clear();
        };

        /// Common constructor.
        gnssLinearCombination( const TypeID& h,
                               const typeValueMap& b )
        {
            header = h;
            body   = b;
        }

        void addOptionalType(const TypeIDSet& typeSet)
        {
            for(auto it=typeSet.begin(); it!=typeSet.end(); it++)
            {
                optionalTypes.insert((*it));
            }
        };

        void addOptionalType(const TypeID& type)
        {
            optionalTypes.insert(type);
        };

        std::set<TypeID> optionalTypes;

        /// Destructor.
        virtual ~gnssLinearCombination() {};


    };  // End of 'gnssLinearCombination'


    /// List containing gnssLinearCombination objects.
    typedef std::list<gnssLinearCombination> LinearCombList;

    typedef std::map<CommonTime, satTypeValueMap> epochSatTypeValueMap;

    /// Map holding epoch with corresponding Vector<double>.
    typedef std::map< CommonTime, Vector<double> > epochVectorMap;

    /// Map holding epoch with corresponding Matrix<double>.
    typedef std::map< CommonTime, Matrix<double> > epochMatrixMap;

    struct EstData
    {
       EstData()
           : value(0.0), variance(1.0E+10)
       {}
       double value;
       double variance;
    };


    struct YawData
    {
        YawData()
            : type("UNK"), beta(0.0),
              event(0), nominal(0.0), modeled(0.0)
        {};

        std::string type;
        double beta;
        int event;
        double nominal;
        double modeled;
    };

    // Now, some useful type definitions
    typedef std::map<SatID,YawData> satYawDataMap;


      //@}

}  // End of namespace gpstk

#endif // GPSTK_DATASTRUCTURES_HPP
