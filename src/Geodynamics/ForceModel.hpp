
/**
* @file ForceModel.hpp
* Force Model is a simple interface which allows uniformity among the various force
* models.
*/

#ifndef FORCE_MODEL_HPP
#define FORCE_MODEL_HPP


#include "Vector.hpp"
#include "Matrix.hpp"
#include "DataStructures.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    typedef std::map<SatID, Matrix<double>> satMatrixMap;

    /**
     * Force Model is a simple interface which allows uniformity among the various force
     * models.
     */
    class ForceModel
    {
    public:
        /// Constructor
        ForceModel()
        {
            satAcc.clear();
            satPartialR.clear();
            satPartialV.clear();
            satPartialP.clear();
            satPartialEGM.clear();
            satPartialSRP.clear();
        };


        /// Default destructor
        virtual ~ForceModel() {};


        /** Compute acceleration (and related partial derivatives).
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits )
        {};


        /// Return the force model name
        inline virtual std::string forceModelName() const
        { return "ForceModel"; };


        /// Get acceleration, a
        virtual Vector<double> getAcceleration(const SatID& sat) const
            noexcept(false)
        {
            satVectorMap::const_iterator it( satAcc.find(sat) );

            if( it != satAcc.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to position, da_dr
        virtual Matrix<double> dA_dR(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialR.find(sat) );

            if( it != satPartialR.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to velocity, da_dv
        virtual Matrix<double> dA_dV(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialV.find(sat) );

            if( it != satPartialV.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to force model parameters, da_dp
        virtual Matrix<double> dA_dP(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialP.find(sat) );

            if( it != satPartialP.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to EGM coefficients, da_dEGM
        virtual Matrix<double> dA_dEGM(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialEGM.find(sat) );

            if( it != satPartialEGM.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to SRP coefficients, da_dSRP
        virtual Matrix<double> dA_dSRP(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialSRP.find(sat) );

            if( it != satPartialSRP.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get partials of acceleration to Empirical Force, da_dEMP
        virtual Matrix<double> dA_dEMP(const SatID& sat) const
            noexcept(false)
        {
            satMatrixMap::const_iterator it( satPartialEMP.find(sat) );

            if( it != satPartialEMP.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Get coefficient matrix of equation of variation
        Matrix<double> getCoeffMatOfEOV(const SatID& sat) const
            noexcept(false)
        {
            Matrix<double> da_dr, da_dv, da_dp;

            satMatrixMap::const_iterator it;

            it = satPartialR.find(sat);
            if( it != satPartialR.end() )
            {
                da_dr = (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }

            it = satPartialV.find(sat);
            if( it != satPartialV.end() )
            {
                da_dv = (*it).second;
            }
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }

            it = satPartialP.find(sat);
            if( it != satPartialP.end() )
            {
                da_dp = (*it).second;
            }
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }


            //////////////////////////////////////////////////
            //             |                     |          //
            //             | 0      I      0     |          //
            //             |                     |          //
            //    cMat  =  | da/dr  da/dv  da/dp |          //
            //             |                     |          //
            //             | 0      0      0     |          //
            //             |                     |          //
            //////////////////////////////////////////////////

            int numOfParam = da_dp.cols();
            Matrix<double> cMat(6+numOfParam,6+numOfParam, 0.0);

            // identify part
            cMat(0,3) = 1.0; cMat(1,4) = 1.0; cMat(2,5) = 1.0;

            // da_dr
            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    cMat(i+3,j+0) = da_dr(i,j);
                }
            }

            // da_dv
            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    cMat(i+3,j+3) = da_dv(i,j);
                }
            }

            // da_dp
            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<numOfParam; ++j)
                {
                    cMat(i+3,j+6) = da_dp(i,j);
                }
            }

            return cMat;

        }  // End of method 'ForceModel::getCoeffMatOfEOV()'


    protected:

        /// Acceleration
        satVectorMap satAcc;

        /// Partial derivative of acceleration wrt position
        satMatrixMap satPartialR;

        /// Partial derivative of acceleration wrt velocity
        satMatrixMap satPartialV;

        /// Partial derivative of acceleration wrt dynamic parameters
        satMatrixMap satPartialP;

        /// Partial derivatives of acceleration wrt EGM coefficients
        satMatrixMap satPartialEGM;

        /// Partial derivatives of acceleration wrt SRP coefficients
        satMatrixMap satPartialSRP;

        /// Partial derivatives of acceleration wrt Empirical Force
        satMatrixMap satPartialEMP;


        Vector<double> a;         // 3

                 /// Partial derivative of acceleration wrt position
        Matrix<double> da_dr;      // 3*3

           /// Partial derivative of acceleration wrt velocity
        Matrix<double> da_dv;      // 3*3

        /// Partial derivative of acceleration wrt Cd
        Matrix<double> da_dcd;      // 3*1

    }; // End of class 'ForceModel'

    // @}

}  // End of namespace 'gpstk'

#endif  // FORCE_MODEL_HPP
