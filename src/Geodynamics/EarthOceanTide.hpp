
/**
* @file EarthOceanTide.hpp
* Class to do Earth Ocean Tide correction.
*
*/

#ifndef EARTH_OCEAN_TIDE_HPP
#define EARTH_OCEAN_TIDE_HPP

#include "ReferenceSystem.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Earth Ocean Tide correction
     * see IERS Conventions 2010 Section 6.3 for more details.
     */
    class EarthOceanTide
    {
    public:
        /// struct to hold Ocean Tide Data
        struct OceanTideData
        {
            int         n[6];
            std::string Darw;
            int         l;
            int         m;

            double      DelCp;
            double      DelSp;
            double      DelCm;
            double      DelSm;
        };

    public:
        /// Default constructor
        EarthOceanTide(int n=4, int m=4)
            : desiredDegree(n),
              desiredOrder(m),
              pRefSys(NULL)
        {}

        /// Default destructor
        ~EarthOceanTide() {}


        /// Load ocean tide file
        void loadFile(const std::string& file)
            noexcept(false);


        /// Set desired degree and order
        inline EarthOceanTide& setDesiredDegreeOrder(const int& n, const int& m)
        {
            if(n >= m)
            {
                desiredDegree  =  n;
                desiredOrder   =  m;
            }
            else
            {
                desiredDegree  =  n;
                desiredOrder   =  n;
            }

            return (*this);
        }


        /// Get desired degree and order
        inline void getDesiredDegreeOrder(int& n, int& m) const
        {
            n  =  desiredDegree;
            m  =  desiredOrder;
        }


        /// Set reference system
        inline EarthOceanTide& setReferenceSystem(ReferenceSystem& ref)
        {
            pRefSys = &ref;

            return (*this);
        }


        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        {
            return pRefSys;
        }


        /** Ocean tide to normalized earth potential coefficients.
         *
         * @param tt    TT
         * @return      correction to normalized Cnm and Snm
         */
        Matrix<double> getOceanTide(CommonTime tt);


    protected:

        /// Degree and Order of ocean tide model desired
        int desiredDegree;
        int desiredOrder;

        /// Reference System
        ReferenceSystem* pRefSys;

        /// Standard vector of Ocean Tide Data
        std::vector<OceanTideData> otDataVec;

    }; // End of class 'EarthOceanTide'

    // @}

}  // End of namespace 'gpstk'

#endif   // EARTH_OCEAN_TIDE_HPP
