
/**
 * @file Relativity.hpp
 * Class to do Relativity calculation.
 */

#ifndef RELATIVITY_HPP
#define RELATIVITY_HPP

#include "ForceModel.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Relativity calculation.
     * see IERS Conventions 2010 Section 10.3 for more details.
     *
     */
    class Relativity : public ForceModel
    {
    public:
        /// Default constructor
        Relativity() {};

        /// Default destructor
        virtual ~Relativity() {};

        /** Compute acceleration (and related partial derivatives).
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits );

        /// Return the force model name
        inline virtual std::string modelName() const
        { return "Relativity"; }

    }; // End of class 'Relativity'

    // @}

}  // End of namespace 'gpstk'

#endif  // RELATIVITY_HPP
