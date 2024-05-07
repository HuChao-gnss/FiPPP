

#ifndef ComputeDerivative_HPP
#define ComputeDerivative_HPP


#include "XvtStore.hpp"
#include "Position.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

    class ComputeDerivative 
    {
    public:

        // given initial nominal position, compute the partial ComputeDerivatives for
        // observables, including dx/dy/dz/dcdt, and the elevation, azimuth,
        // etc.
        ComputeDerivative()
           : minElev(10.0)
        {
            nominalPos = Position(0.0,0.0,0.0,Position::Cartesian,NULL);
        };

         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
        ComputeDerivative(const Position& staCoordinates);


        satTypeValueMap& Process( const CommonTime& time,
                                  satTypeValueMap& gData )
            noexcept(false);

        /** Return a satTypeValueMap object, adding the new data generated
         *  when calling a modeling object.
         */
        virtual gnssRinex& Process(gnssRinex& gRin)
            noexcept(false)
        {
            Process(gRin.header.epoch, gRin.body);
            computeDOPs(gRin);
            return gRin;
        };

        void computeDOPs(gnssRinex& gRin)
            noexcept(false);

        /// Method to set satellite elevation cut-off angle. By default, it
        /// is set to 10 degrees.
        virtual ComputeDerivative& setCoordinates(Triple& coord)
        { 
            nominalPos = coord; 
            return (*this); 
        };


        /// Method to set satellite elevation cut-off angle. By default, it
        /// is set to 10 degrees.
        virtual ComputeDerivative& setMinElev(double newElevation)
        { 
            minElev = newElevation; 
            return (*this); 
        };


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor.
        virtual ~ComputeDerivative() {};


    protected:

         /// The elevation cut-off angle for accepted satellites.
         /// By default it is set to 10 degrees.
        double minElev;

         /// Station position
        Position nominalPos;

    }; // End of class 'ComputeDerivative'

      //@}

}  // End of namespace gpstk

#endif   // ComputeDerivative_HPP
