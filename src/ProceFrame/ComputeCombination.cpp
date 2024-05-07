
/**
 * @file ComputeCombination.cpp
 * This class computes linear combinations of GDS data.
 */

#include "ComputeCombination.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

      // Return a string identifying this object.
    std::string ComputeCombination::getClassName() const
    { return "ComputeCombination"; }



      /* Return a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeCombination::Process( const CommonTime& time,
                                                  satTypeValueMap& gData )
        noexcept(false)
    {

        try
        {
            SatIDSet satRejectedSet;
            LinearCombList linearList;

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                // get combination coefficient
                linearList = systemCombs[(*it).first.system];
                // Loop through all the defined linear combinations
                for(auto pos = linearList.begin(); pos != linearList.end(); ++pos)
                {
                    double result(0.0);

                    bool valid( true );
                    
                    if(debug)
                        cout << pos->header << endl;

                    // Read the information of each linear combination
                    for(typeValueMap::const_iterator iter = pos->body.begin();
                        iter != pos->body.end();
                        ++iter)
                    {
                        double temp(0.0);

                        TypeID type(iter->first);

                        if(debug)
                            cout << type << endl;


                        // if found 
                        if( (*it).second.find(type) != (*it).second.end() )
                        {
                           temp = (*it).second[type];
                        }
                        else // not found
                        {
                           if((*pos).optionalTypes.find(type) != (*pos).optionalTypes.end())
                           {
                               temp = 0.0;
                           }
                           else
                           {
                               valid = false;
                               break;
                           }
                        }

                        if(debug)
                        {
                            cout << (*iter).second << endl;
                            cout << type << " value: " << temp << endl;
                        }

                        result = result + (*iter).second * temp;
                    }

                    if(debug)
                    {
                        cout << "valid:" << valid << endl;
                        cout << "result:" << result << endl;
                    }

                    // Store the result in the proper place
                    if( valid )
                        (*it).second[pos->header] = result;
                }
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

    }  // End of method 'ComputeCombination::Process()'



} // End of namespace gpstk
