#pragma ident "$Id$"

/**
 * @file DataStructures.cpp
 * Set of several data structures to be used by other GPSTk classes.
 */

#include "DataStructures.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{

      ////// typeValueMap //////


      // Return a TypeIDSet with all the data types present in
      // this object.
    TypeIDSet typeValueMap::getTypeID() const
    {

        TypeIDSet typeSet;

        for( typeValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            typeSet.insert( (*pos).first );
        }

        return typeSet;

    }  // End of method 'typeValueMap::getTypeID()'



      // Return a typeValueMap with only this type of data.
      // @param type Type of value to be extracted.
    typeValueMap typeValueMap::extractTypeID(const TypeID& type) const
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return extractTypeID(typeSet);

    }  // End of method 'typeValueMap::extractTypeID()'



      // Return a typeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data to
      //                be extracted.
    typeValueMap typeValueMap::extractTypeID(const TypeIDSet& typeSet) const
    {

        typeValueMap tvMap;

        for( TypeIDSet::const_iterator pos = typeSet.begin();
             pos != typeSet.end();
             ++pos )
        {

            typeValueMap::const_iterator itObs( (*this).find(*pos) );
            if( itObs != (*this).end() )
            {
                tvMap[ (*itObs).first ] = (*itObs).second;
            }
        }

        return tvMap;

    }  // End of method 'typeValueMap::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
    typeValueMap& typeValueMap::keepOnlyTypeID(const TypeID& type)
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return (keepOnlyTypeID(typeSet));

    }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
    typeValueMap& typeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
    {

        typeValueMap tvMap( (*this).extractTypeID(typeSet) );
        (*this) = tvMap;

        return (*this);

    }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
    typeValueMap& typeValueMap::removeTypeID(const TypeIDSet& typeSet)
    {

        for( TypeIDSet::const_iterator pos = typeSet.begin();
             pos != typeSet.end();
             ++pos )
        {
            (*this).erase(*pos);
        }

        return (*this);

    }  // End of method 'typeValueMap::removeTypeID()'



      /* Return the data value (double) corresponding to provided type.
       *
       * @param type       Type of value to be looked for.
       */
    double typeValueMap::getValue(const TypeID& type) const
        noexcept(false)
    {

        typeValueMap::const_iterator itObs( (*this).find(type) );
        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            string typeStr = asString(type);
            GPSTK_THROW(TypeIDNotFound(typeStr+"TypeID not found in map"));
        }

    }  // End of method 'typeValueMap::getValue()'



      // Return a reference to the data value (double) with
      // corresponding type.
      // @param type Type of value to be looked for.
    double& typeValueMap::operator()(const TypeID& type)
        noexcept(false)
    {

        typeValueMap::iterator itObs ( (*this).find(type) );

        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
        }

    }  // End of method 'typeValueMap::operator()'


      // Convenience output method for structure satTypeValueMap
    std::ostream& typeValueMap::dump( std::ostream& s, int mode ) const
    {

        for(typeValueMap::const_iterator itObs = (*this).begin();
            itObs != (*this).end();
            ++itObs)
        {

            if (mode==1)
            {
                s << (*itObs).first << " ";
            }

            s << setprecision(12) ;
            s << (*itObs).second << " ";

        }  // End of 'for( typeValueMap::const_iterator itObs = ...'

         // Let's return the 'std::ostream'
        return s;

    }  // End of method 'satTypeValueMap::dump()'

      ////// sourceValueMap //////


      // Return a SourceIDSet with all the sources present in this object.
    SourceIDSet sourceValueMap::getSourceID() const
    {

        SourceIDSet sourceSet;

        for( sourceValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            sourceSet.insert( (*pos).first );
        }

        return sourceSet;

    }  // End of method 'sourceValueMap::getSourceID()'



      // Return a Vector with all the sources present in this object.
    Vector<SourceID> sourceValueMap::getVectorOfSourceID() const
    {

        std::vector<SourceID> temp;

        for( sourceValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            temp.push_back( (*pos).first );
        }

        Vector<SourceID> result;
        result = temp;

        return result;

    }  // End of method 'sourceValueMap::getVectorOfSourceID()'



      // Return a sourceValueMap with only this source.
      // @param source Source to be extracted.
    sourceValueMap sourceValueMap::extractSourceID(const SourceID& source) const
    {

        SourceIDSet sourceSet;
        sourceSet.insert(source);

        return extractSourceID(sourceSet);

    }  // End of method 'sourceValueMap::extractSourceID()'



      // Return a sourceValueMap with only these sources.
      // @param sourceSet Set (SourceIDSet) containing the sources to
      //               be extracted.
    sourceValueMap sourceValueMap::extractSourceID(const SourceIDSet& sourceSet) const
    {

        sourceValueMap svMap;

        for( SourceIDSet::const_iterator pos = sourceSet.begin();
             pos != sourceSet.end();
             ++pos )
        {
            sourceValueMap::const_iterator itObs( (*this).find(*pos) );

            if( itObs != (*this).end() )
            {
                svMap[ (*itObs).first ] = (*itObs).second;
            }
        }

        return svMap;

    }  // End of method 'sourceValueMap::extractSourceID()'



      // Modifies this object, keeping only this source.
      // @param source Source to be kept.
    sourceValueMap& sourceValueMap::keepOnlySourceID(const SourceID& source)
    {

        SourceIDSet sourceSet;
        sourceSet.insert(source);

        return keepOnlySourceID(sourceSet);

    }  // End of method 'sourceValueMap::keepOnlySourceID()'



      // Modifies this object, keeping only these sources.
      // @param sourceSet Set (SourceIDSet) containing the sources to be kept.
    sourceValueMap& sourceValueMap::keepOnlySourceID(const SourceIDSet& sourceSet)
    {

        sourceValueMap svMap = extractSourceID(sourceSet);
        (*this) = svMap;

        return (*this);

    }  // End of method 'sourceValueMap::keepOnlySourceID()'



      // Modifies this object, removing these sources.
      // @param sourceSet Set (SourceIDSet) containing the sources to
      //               be removed.
    sourceValueMap& sourceValueMap::removeSourceID(const SourceIDSet& sourceSet)
    {

        for( SourceIDSet::const_iterator pos = sourceSet.begin();
             pos != sourceSet.end();
             ++pos )
        {
            (*this).erase(*pos);
        }

        return (*this);

    }  // End of method 'sourceValueMap::removeSourceID()'



      /* Return the data value (double) corresponding to provided SourceID.
       *
       * @param source     Source to be looked for.
       */
    double sourceValueMap::getValue(const SourceID& source) const
        noexcept(false)
    {

        sourceValueMap::const_iterator itObs( (*this).find(source) );
        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
        }

    }  // End of method 'sourceValueMap::getValue()'



      // Return a reference to the data value (double) with
      // corresponding SourceID.
      // @param source Source to be looked for.
    double& sourceValueMap::operator()(const SourceID& source)
        noexcept(false)
    {

        sourceValueMap::iterator itObs( (*this).find(source) );

        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
        }

    }  // End of method 'sourceValueMap::operator()'





      ////// satValueMap //////


      // Return a SatIDSet with all the satellites present in this object.
    SatIDSet satValueMap::getSatID() const
    {

        SatIDSet satSet;

        for( satValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            satSet.insert( (*pos).first );
        }

        return satSet;

    }  // End of method 'satValueMap::getSatID()'



      // Return a Vector with all the satellites present in this object.
    Vector<SatID> satValueMap::getVectorOfSatID() const
    {

        std::vector<SatID> temp;

        for( satValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            temp.push_back( (*pos).first );
        }

        Vector<SatID> result;
        result = temp;

        return result;

    }  // End of method 'satValueMap::getVectorOfSatID()'



      // Return a satValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
    satValueMap satValueMap::extractSatID(const SatID& satellite) const
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return extractSatID(satSet);

    }  // End of method 'satValueMap::extractSatID()'



      // Return a satValueMap with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
    satValueMap satValueMap::extractSatID( const int& p,
                                           const SatelliteSystem& s ) const
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return extractSatID(tempSatellite);

    }  // End of method 'satValueMap::extractSatID()'



      // Return a satValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
    satValueMap satValueMap::extractSatID(const SatIDSet& satSet) const
    {

        satValueMap svMap;

        for( SatIDSet::const_iterator pos = satSet.begin();
             pos != satSet.end();
             ++pos )
        {
            satValueMap::const_iterator itObs( (*this).find(*pos) );

            if( itObs != (*this).end() )
            {
                svMap[ (*itObs).first ] = (*itObs).second;
            }
        }

        return svMap;

    }  // End of method 'satValueMap::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
    satValueMap& satValueMap::keepOnlySatID(const SatID& satellite)
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return keepOnlySatID(satSet);

    }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
    satValueMap& satValueMap::keepOnlySatID( const int& p,
                                             const SatelliteSystem& s )
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return keepOnlySatID(tempSatellite);

    }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
    satValueMap& satValueMap::keepOnlySatID(const SatIDSet& satSet)
    {

        satValueMap svMap = extractSatID(satSet);
        (*this) = svMap;

        return (*this);

    }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be removed.
    satValueMap& satValueMap::removeSatID(const SatIDSet& satSet)
    {

        for( SatIDSet::const_iterator pos = satSet.begin();
             pos != satSet.end();
             ++pos )
        {
            (*this).erase(*pos);
        }

        return (*this);

    }  // End of method 'satValueMap::removeSatID()'



      /* Return the data value (double) corresponding to provided SatID.
       *
       * @param satellite     Satellite to be looked for.
       */
    double satValueMap::getValue(const SatID& satellite) const
        noexcept(false)
    {

        satValueMap::const_iterator itObs( (*this).find(satellite) );
        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

    }  // End of method 'satValueMap::getValue()'



      // Return a reference to the data value (double) with
      // corresponding SatID.
      // @param satellite Satellite to be looked for.
    double& satValueMap::operator()(const SatID& satellite)
        noexcept(false)
    {

        satValueMap::iterator itObs( (*this).find(satellite) );

        if ( itObs != (*this).end() )
        {
            return (*itObs).second;
        }
        else
        {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

    }  // End of method 'satValueMap::operator()'


      // Convenience output method for structure satValueMap
    std::ostream& satValueMap::dump( std::ostream& s, int mode ) const
    {

        for(satValueMap::const_iterator it = (*this).begin();
            it != (*this).end();
            ++it)
        {

            // First, print satellite (system and PRN)
            s << (*it).first << " " << (*it).second;
            s << endl;

        }  // End of 'for( satValueMap::const_iterator it = ...'

         // Let's return the 'std::ostream'
        return s;

    }  // End of method 'satValueMap::dump()'


      ////// satTypeValueMap //////


      /* Return the total number of data elements in the map.
       * This method DOES NOT suppose that all the satellites have
       * the same number of type values.
       */
    size_t satTypeValueMap::numElements() const
    {

        size_t numEle(0);

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            numEle = numEle + (*it).second.size();
        }

        return numEle;

    }  // End of method 'satTypeValueMap::numElements()'



      // Return a SatIDSet with all the satellites present in this object.
    SatIDSet satTypeValueMap::getSatID() const
    {

        SatIDSet satSet;

        for( satTypeValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            satSet.insert( (*pos).first );
        }

        return satSet;

    }  // End of method 'satTypeValueMap::getSatID()'



      // Return a Vector with all the satellites present in this object.
    Vector<SatID> satTypeValueMap::getVectorOfSatID() const
    {

        std::vector<SatID> temp;

        for( satTypeValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {
            temp.push_back( (*pos).first );
        }

        Vector<SatID> result;
        result = temp;

        return result;

    }  // End of method 'satTypeValueMap::getVectorOfSatID()'



      // Return a TypeIDSet with all the data types present in
      // this object. This does not imply that all satellites have these types.
    TypeIDSet satTypeValueMap::getTypeID() const
    {

        TypeIDSet typeSet;

        for( satTypeValueMap::const_iterator pos = (*this).begin();
             pos != (*this).end();
             ++pos )
        {

            for( typeValueMap::const_iterator it = (*pos).second.begin();
                 it != (*pos).second.end();
                 ++it )
            {
                typeSet.insert( (*it).first );
            }

        }

        return typeSet;

    }  // End of method 'satTypeValueMap::getTypeID()'



      // Return a satTypeValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
    satTypeValueMap satTypeValueMap::extractSatID(const SatID& satellite) const
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return extractSatID(satSet);

    }  // End of method 'satTypeValueMap::extractSatID()'



      // Return a satTypeValueMap with only one satellite, identified
      // by the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
    satTypeValueMap satTypeValueMap::extractSatID( const int& p,
                                                   const SatelliteSystem& s ) const
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return extractSatID(tempSatellite);

    }  // End of method 'satTypeValueMap::extractSatID()'



      // Return a satTypeValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
    satTypeValueMap satTypeValueMap::extractSatID(const SatIDSet& satSet) const
    {

        satTypeValueMap stvMap;

        for( SatIDSet::const_iterator pos = satSet.begin();
             pos != satSet.end();
             ++pos )
        {
            satTypeValueMap::const_iterator itObs( (*this).find(*pos) );
            if( itObs != (*this).end() )
            {
                stvMap[ (*itObs).first ] = (*itObs).second;
            }
        }

        return stvMap;

    }  // End of method 'satTypeValueMap::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
    satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatID& satellite)
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return keepOnlySatID(satSet);

    }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
    satTypeValueMap& satTypeValueMap::keepOnlySatID( const int& p,
                                                     const SatelliteSystem& s )
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return keepOnlySatID(tempSatellite);

    }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
    satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatIDSet& satSet)
    {

        satTypeValueMap stvMap( extractSatID(satSet) );
        (*this) = stvMap;

        return (*this);

    }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Return a satTypeValueMap with only this type of value.
      // @param type Type of value to be extracted.
    satTypeValueMap satTypeValueMap::extractTypeID(const TypeID& type) const
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return extractTypeID(typeSet);

    }  // End of method 'satTypeValueMap::extractTypeID()'



      // Return a satTypeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
    satTypeValueMap satTypeValueMap::extractTypeID(const TypeIDSet& typeSet) const
    {

        satTypeValueMap theMap;

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {

            typeValueMap tvMap( (*it).second.extractTypeID(typeSet) );
            if( tvMap.size() > 0 )
            {
                theMap[(*it).first] = tvMap;
            }

        }

        return theMap;

    }  // End of method 'satTypeValueMap::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
    satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeID& type)
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return keepOnlyTypeID(typeSet);

    }  // End of method 'satTypeValueMap::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
    satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
    {

        satTypeValueMap stvMap( extractTypeID(typeSet) );
        (*this) = stvMap;

        return (*this);

    }  // End of method 'satTypeValueMap::keepOnlyTypeID()'


      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
    satTypeValueMap& satTypeValueMap::removeSatID(const SatIDSet& satSet)
    {

        for( SatIDSet::const_iterator pos = satSet.begin();
             pos != satSet.end();
             ++pos )
        {
            (*this).erase(*pos);
        }

        return (*this);

    }  // End of method 'satTypeValueMap::removeSatID()'



      // Modifies this object, removing this type of data.
      // @param type Type of value to be removed.
    satTypeValueMap& satTypeValueMap::removeTypeID(const TypeID& type)
    {

        for( satTypeValueMap::iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            (*it).second.removeTypeID(type);
        }

        return (*this);

    }  // End of method 'satTypeValueMap::removeTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
    satTypeValueMap& satTypeValueMap::removeTypeID(const TypeIDSet& typeSet)
    {

        for( TypeIDSet::const_iterator pos = typeSet.begin();
             pos != typeSet.end();
             ++pos )
        {
            removeTypeID(*pos);
        }

        return (*this);

    }  // End of method 'satTypeValueMap::removeTypeID()'



      // Return a GPSTk::Vector containing the data values with this type.
      // @param type Type of value to be returned.
      // This method returns zero if a given satellite does not have this type.
    Vector<double> satTypeValueMap::getVectorOfTypeID(const TypeID& type) const
    {

         // Let's declare a STL vector
        std::vector<double> temp;

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {

            typeValueMap::const_iterator itObs( (*it).second.find(type) );
            if ( itObs != (*it).second.end() )
            {
                temp.push_back( (*itObs).second );
            }
            else
            {
                temp.push_back( 0.0 );
            }

        }

         // Let's declare a GPSTk Vector
        Vector<double> result;

         // Transform STL vector into GPSTk Vector
        result = temp;

        return result;

    }  // End of method 'satTypeValueMap::getVectorOfTypeID()'


      // Return a GPSTk::Vector containing the data values with this type.
      // @param type Type of value to be returned.
      // This method returns zero if a given satellite does not have this type.
    Vector<double> satTypeValueMap::getVectorOfTypeID(const TypeID& type,
                                                      SatelliteSystem sys) const
    {

         // Let's declare a STL vector
        std::vector<double> temp;

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            if((*it).first.system == sys)
            {
                typeValueMap::const_iterator itObs( (*it).second.find(type) );
                if ( itObs != (*it).second.end() )
                {
                    temp.push_back( (*itObs).second );
                }
                else
                {
                    temp.push_back( 0.0 );
                }
            }

        }

         // Let's declare a GPSTk Vector
        Vector<double> result;

         // Transform STL vector into GPSTk Vector
        result = temp;

        return result;

    }  // End of method 'satTypeValueMap::getVectorOfTypeID()'


      // Return a GPSTk::Matrix containing the data values in this set.
      // @param typeSet  TypeIDSet of values to be returned.
    Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDSet& typeSet) const
    {

         // First, let's create a Matrix<double> of the proper size
        Matrix<double> tempMat( (*this).numSats(), typeSet.size(), 0.0 );

        size_t numRow(0), numCol(0);

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            numCol=0;

            for( TypeIDSet::const_iterator pos = typeSet.begin();
                 pos != typeSet.end();
                 ++pos )
            {

                typeValueMap::const_iterator itObs( (*it).second.find(*pos) );
                if( itObs != (*it).second.end() )
                {
                    tempMat(numRow, numCol) = (*itObs).second;
                }

                ++numCol;
            }

            ++numRow;

        }

        return tempMat;

    }  // End of method 'satTypeValueMap::getMatrixOfTypes()'

      // Return a GPSTk::Matrix containing the data values in this set.
      // @param typeSet  TypeIDSet of values to be returned.
    Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDVec& typeVec) const
    {

         // First, let's create a Matrix<double> of the proper size
        Matrix<double> tempMat( (*this).numSats(), typeVec.size(), 0.0 );

        size_t numRow(0), numCol(0);

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            for( int i=0; i<typeVec.size(); i++ )
            {
                TypeID type = typeVec[i];
                auto typeIter = (*it).second.find(type);
                if( typeIter != (*it).second.end() )
                {
                    tempMat(numRow, i) = (*typeIter).second;
                }
            }

            ++numRow;
        }

        return tempMat;

    }  // End of method 'satTypeValueMap::getMatrixOfTypes()'


      // Return a GPSTk::Matrix containing the data values in this set.
      // @param typeSet  TypeIDSet of values to be returned.
    Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDSet& typeSet, 
                                                     SatelliteSystem sys) const
    {

         // First, let's create a Matrix<double> of the proper size
        Matrix<double> tempMat( (*this).numSats(sys), typeSet.size(), 0.0 );

        size_t numRow(0), numCol(0);

        for( satTypeValueMap::const_iterator it = (*this).begin();
             it != (*this).end();
             ++it )
        {
            if((*it).first.system == sys)
            {
                numCol=0;

                for( TypeIDSet::const_iterator pos = typeSet.begin();
                     pos != typeSet.end();
                     ++pos )
                {

                    typeValueMap::const_iterator itObs( (*it).second.find(*pos) );
                    if( itObs != (*it).second.end() )
                    {
                        tempMat(numRow, numCol) = (*itObs).second;
                    }

                    ++numCol;
                }
                ++numRow;
            }
        }

        return tempMat;

    }  // End of method 'satTypeValueMap::getMatrixOfTypes()'



      /* Modifies this object, adding one vector of data with this type,
       * one value per satellite.
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
    satTypeValueMap& satTypeValueMap::insertTypeIDVector( const TypeID& type,
                                                          const Vector<double> dataVector )
        noexcept(false)
    {

        if( dataVector.size() == (*this).numSats() )
        {
            size_t pos = 0;

            for( satTypeValueMap::iterator it = (*this).begin();
                 it != (*this).end();
                 ++it )
            {
                (*it).second[type] = dataVector[pos];
                ++pos;
            }

            return (*this);

        }
        else
        {
            GPSTK_THROW( NumberOfSatsMismatch(" Number of data values in vector \
and number of satellites do not match") );
        }

    }  // End of method 'satTypeValueMap::insertTypeIDVector()'



      /* Modifies this object, adding a matrix of data, one vector
       * per satellite.
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
    satTypeValueMap& satTypeValueMap::insertMatrix( const TypeIDSet& typeSet,
                                                    const Matrix<double> dataMatrix )
        noexcept(false)
    {

        if( dataMatrix.rows() != (*this).numSats() )
        {
            GPSTK_THROW( NumberOfSatsMismatch("Number of rows in matrix and \
number of satellites do not match") );
        }

        if( dataMatrix.cols() == typeSet.size() )
        {

            size_t pos(0);

            for( satTypeValueMap::iterator it = (*this).begin();
                 it != (*this).end();
                 ++it )
            {

                size_t idx(0);

                for( TypeIDSet::const_iterator itSet = typeSet.begin();
                     itSet != typeSet.end();
                     ++itSet )
                {
                    (*it).second[(*itSet)] = dataMatrix(pos,idx);
                    ++idx;
                }

                ++pos;

            }

            return (*this);

        }
        else
        {
            GPSTK_THROW( NumberOfTypesMismatch("Number of data values per row \
in matrix and number of types do not match") );
        }

    }  // End of method 'satTypeValueMap::insertMatrix()'


      /* Return the data value (double) corresponding to provided SatID
       * and TypeID.
       *
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       */
    double satTypeValueMap::getValue( const SatID& satellite,
                                      const TypeID& type ) const
        noexcept(false)
    {

        satTypeValueMap::const_iterator itObs( (*this).find(satellite) );
        if( itObs != (*this).end() )
        {
            return (*itObs).second.getValue( type );
        }
        else
        {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

    }  // End of method 'satTypeValueMap::getValue()'



      // Return a reference to the typeValueMap with corresponding SatID.
      // @param type Type of value to be looked for.
    typeValueMap& satTypeValueMap::operator()(const SatID& satellite)
    {

        satTypeValueMap::iterator itObs( (*this).find(satellite) );
        try{
            return (*itObs).second;
        }
        catch (...){
            cerr << (SatIDNotFound("SatID not found in map"));
        }
//        if( itObs != (*this).end() )
//        {
//            return (*itObs).second;
//        }
//        else
//        {
//            cerr << (SatIDNotFound("SatID not found in map"));
//            return (*itObs).second;
//        }

    }  // End of method 'satTypeValueMap::operator()'


      ////// Other stuff //////


      // Convenience output method for structure satTypeValueMap
    std::ostream& satTypeValueMap::dump( std::ostream& s, int mode ) const
    {

        for(satTypeValueMap::const_iterator it = (*this).begin();
            it != (*this).end();
            ++it)
        {

            // First, print satellite (system and PRN)
            s << (*it).first << " ";

            for(typeValueMap::const_iterator itObs = (*it).second.begin();
                itObs != (*it).second.end();
                ++itObs)
            {

                if (mode==1)
                {
                    s << (*itObs).first << " ";
                }

                s << setprecision(12) ;
                s << (*itObs).second << " ";

            }  // End of 'for( typeValueMap::const_iterator itObs = ...'

            s << endl;

        }  // End of 'for( satTypeValueMap::const_iterator it = ...'

         // Let's return the 'std::ostream'
        return s;

    }  // End of method 'satTypeValueMap::dump()'


      /* Return the data value (double) corresponding to provided SourceID,
       * SatID and TypeID.
       *
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       */
    double sourceDataMap::getValue( const SourceID& source,
                                    const SatID& satellite,
                                    const TypeID& type ) const
        noexcept(false)
    {

         // Look for the SourceID
        sourceDataMap::const_iterator itObs( (*this).find(source) );
        if( itObs != (*this).end() )
        {
            return (*itObs).second.getValue( satellite, type );
        }
        else
        {
            GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
        }

    }  // End of method 'sourceDataMap::getValue()'



      /* Get a set with all the SourceID's in this data structure.
       *
       * @warning If current 'sourceDataMap' is big, this could be a very
       * costly operation.
       */
    SourceIDSet sourceDataMap::getSourceIDSet( void ) const
    {

         // SourceID set to be returned
        SourceIDSet toReturn;

         // Then, iterate through corresponding 'sourceDataMap'
        for(sourceDataMap::const_iterator sdmIter = (*this).begin();
            sdmIter != (*this).end();
            ++sdmIter)
        {

            // Add current SourceID to 'toReturn'
            toReturn.insert( (*sdmIter).first );

        }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

        return toReturn;

    }  // End of method 'sourceDataMap::getSourceIDSet()'



      /* Get a set with all the SatID's in this data structure.
       *
       * @warning If current 'sourceDataMap' is big, this could be a very
       * costly operation.
       */
    SatIDSet sourceDataMap::getSatIDSet( void ) const
    {

         // SatID set to be returned
        SatIDSet toReturn;

         // Then, iterate through corresponding 'sourceDataMap'
        for(sourceDataMap::const_iterator sdmIter = (*this).begin();
            sdmIter != (*this).end();
            ++sdmIter)
        {

            // Finally, iterate through corresponding 'satTypeValueMap'
            for(satTypeValueMap::const_iterator stvmIter =
                                                   (*sdmIter).second.begin();
                stvmIter != (*sdmIter).second.end();
                stvmIter++)
            {

                // Add current SatID to 'toReturn'
                toReturn.insert( (*stvmIter).first );

            }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

        }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

        return toReturn;

    }  // End of method 'sourceDataMap::getSatIDSet()'




      // stream output for satTypeValueMap
    std::ostream& operator<<( std::ostream& s, const typeValueMap& tvMap )
    {

        tvMap.dump(s,1);
        return s;

    }  // End of 'operator<<'

      // stream output for satTypeValueMap
    std::ostream& operator<<( std::ostream& s, const satValueMap& svMap )
    {

        svMap.dump(s,1);
        return s;

    }  // End of 'operator<<'


      // stream output for satTypeValueMap
    std::ostream& operator<<( std::ostream& s, const satTypeValueMap& stvMap )
    {

        stvMap.dump(s,1);
        return s;

    }  // End of 'operator<<'


}  // End of namespace gpstk
