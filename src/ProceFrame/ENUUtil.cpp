
//=========================                    
// class to convert xyz2neu and neu2xyz
//=========================                    

// gpstk
#include "ENUUtil.hpp"
#include <cmath>


namespace gpstk
{

    using std::sin;
    using std::cos;

    NEUUtil::NEUUtil(const double refLatRad,
        const double refLonRad)
    {
        compute(refLatRad, refLonRad);
    }

    //
    void NEUUtil::compute(const double refLat,
        const double refLon)
    {
        rotMat.resize(3, 3);
        rotMat(0, 0) = -sin(refLat) * cos(refLon);
        rotMat(0, 1) = -sin(refLat) * sin(refLon);
        rotMat(0, 2) = cos(refLat);

        rotMat(1, 0) = -sin(refLon);
        rotMat(1, 1) = cos(refLon);
        rotMat(1, 2) = 0.0;

        rotMat(2, 0) = cos(refLat) * cos(refLon);
        rotMat(2, 1) = cos(refLat) * sin(refLon);
        rotMat(2, 2) = sin(refLat);

        rotMatNEU2XYZ.resize(3, 3);

        rotMatNEU2XYZ(0, 0) = -sin(refLat) * cos(refLon);
        rotMatNEU2XYZ(0, 1) = -sin(refLon);
        rotMatNEU2XYZ(0, 2) = cos(refLat) * cos(refLon);

        rotMatNEU2XYZ(1, 0) = -sin(refLat) * sin(refLon);
        rotMatNEU2XYZ(1, 1) = cos(refLon);
        rotMatNEU2XYZ(1, 2) = cos(refLat) * sin(refLon);

        rotMatNEU2XYZ(2, 0) = cos(refLat);
        rotMatNEU2XYZ(2, 1) = 0.0;
        rotMatNEU2XYZ(2, 2) = sin(refLat);

    }

    void NEUUtil::updatePosition(const double refLatRad,
        const double refLonRad)
    {
        compute(refLatRad, refLonRad);
    }


    gpstk::Vector<double> NEUUtil::convertToNEU(const gpstk::Vector<double>& inV) const
    {
        gpstk::Vector<double> outV(3);

        if (inV.size() != 3)
        {
            gpstk::Exception e("Incompatible dimensions for Vector");
            GPSTK_THROW(e);
        }
        outV = rotMat * inV;
        return(outV);
    }

    gpstk::Triple NEUUtil::convertToNEU(const gpstk::Triple& inVec) const
    {
        gpstk::Vector<double> v(3);
        v[0] = inVec[0];
        v[1] = inVec[1];
        v[2] = inVec[2];

        gpstk::Vector<double> vOut = convertToNEU(v);
        gpstk::Triple outVec(vOut[0], vOut[1], vOut[2]);
        return(outVec);
    }

    gpstk::Vector<double> NEUUtil::convertToXYZ(const gpstk::Vector<double>& inV) const
    {
        gpstk::Vector<double> outV(3);

        if (inV.size() != 3)
        {
            gpstk::Exception e("Incompatible dimensions for Vector");
            GPSTK_THROW(e);
        }
        outV = rotMatNEU2XYZ * inV;
        return(outV);
    }

    gpstk::Triple NEUUtil::convertToXYZ(const gpstk::Triple& inVec) const
    {
        gpstk::Vector<double> v(3);
        v[0] = inVec[0];
        v[1] = inVec[1];
        v[2] = inVec[2];

        gpstk::Vector<double> vOut = convertToXYZ(v);
        gpstk::Triple outVec(vOut[0], vOut[1], vOut[2]);
        return(outVec);
    }

}     // end namespace gpstk
