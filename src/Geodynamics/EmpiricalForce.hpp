
/**
 * @file EmpiricalForce.hpp
 * Class to do Empirical Force calculation.
 */

#ifndef EMPIRICAL_FORCE_HPP
#define EMPIRICAL_FORCE_HPP

#include "ForceModel.hpp"


namespace gpstk
{
    /** @addtogroup */
    //@{

    /** Class to do Empirical Force calculation.
     *
     */
    class EmpiricalForce : public ForceModel
    {
    public:
        /// Default constructor
        EmpiricalForce()
            : sigmaR(0.0), sigmaT(0.0), sigmaN(0.0),
              tau(0.0)
        {};

        /// Default destructor
        virtual ~EmpiricalForce() {};


        /// Set process variance.
        EmpiricalForce& setProcessVariance(double var)
        {
            sigmaR = var;
            sigmaT = var;
            sigmaN = var;
            return (*this);
        };


        /// Set process variance.
        EmpiricalForce& setProcessVariance(double varR,
                                           double varT,
                                           double varN)
        {
            sigmaR = varR;
            sigmaT = varT;
            sigmaN = varN;
            return (*this);
        };


        /// Set correlation time.
        EmpiricalForce& setCorrelationTime(double time)
        { tau = time; return (*this); };


        /// Set previous time.
        EmpiricalForce& setPreviousTime(const CommonTime& time)
        { prevTime = time; return (*this); };


        /// Set previous force.
        EmpiricalForce& setPreviousForce(const satVectorMap& force)
        { prevForce = force; return (*this); };


        /** Compute acceleration (and related partial derivatives).
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits );

        /// Return the force model name
        inline virtual std::string modelName() const
        { return "EmpiricalForce"; }


    private:

        double sigmaR;
        double sigmaT;
        double sigmaN;

        double tau;

        CommonTime prevTime;
        satVectorMap prevForce;

    }; // End of class 'EmpiricalForce'

    // @}

}  // End of namespace 'gpstk'

#endif  // EMPIRICAL_FORCE_HPP
