
#include <math.h>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "MatrixFunctors.hpp"
#include "Equation.hpp"
#include "EquSys.hpp"
#include "DataStructures.hpp"

#include "ComputeDerivative.hpp"
#include "ComputeTropModel.hpp"
#include "ComputeIonoDelay.hpp"

#include "ComputeSPP.hpp"
#include "Counter.hpp"
#include "YDSTime.hpp"

using namespace fippp;
using namespace gpstk;
using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{
    std::string ComputeSPP::getClassName() const
    {
        return "ComputeSPP"; 
    }


    void ComputeSPP::defineLinearList()
    {
        try
        {
            TypeIDSet commonOptTypes;
            commonOptTypes.insert(TypeID::gravDelay);

            LinearCombinations linearComb;

            gnssLinearCombination c1PrefitOfGPS = linearComb.c1PrefitOfGPS;
            gnssLinearCombination c2PrefitOfGPS = linearComb.c2PrefitOfGPS;
            gnssLinearCombination c5PrefitOfGPS = linearComb.c5PrefitOfGPS;

            // add prefit of GPS code to linearList
            linearListGPS.push_back(c1PrefitOfGPS);
            linearListGPS.push_back(c2PrefitOfGPS);
            linearListGPS.push_back(c5PrefitOfGPS);

            for(auto it=linearListGPS.begin(); it!=linearListGPS.end(); it++)
            {
                gnssLinearCombination codeComb = (*it);

                if(debug)
                {
                    cout << getClassName() << "GPS" << codeComb.header << endl;
                }

                // insert common optional types
                (*it).addOptionalType(commonOptTypes);

                string prefitTypeStr = (*it).header.asString();
                string obsTypeStr = prefitTypeStr.substr(6,3);
                string rcvCorrTypeStr = "rcvCorr" + obsTypeStr;

                if(debug)
                {
                    cout << "prefitTypeStr:" << prefitTypeStr << endl;
                    cout << "obsTypeStr:" << obsTypeStr << endl;
                    cout << "rcvCorrTypeStr:" << rcvCorrTypeStr << endl;
                }

                TypeID rcvCorrType = TypeID(rcvCorrTypeStr);

                // insert rcvCorrType into optionalTypes;
                (*it).addOptionalType(rcvCorrType);

                string satPCenterTypeStr = "satPCenter" + obsTypeStr;
                TypeID satPCenterType = TypeID(satPCenterTypeStr);
                (*it).addOptionalType(satPCenterType);

            }

            // inset sysCombs
            sysCombs[SatelliteSystem::GPS] = linearListGPS;

            ///
            /// GLONASS
            ///
            gnssLinearCombination c1PrefitOfGLO = linearComb.c1PrefitOfGLO;
            gnssLinearCombination c2PrefitOfGLO = linearComb.c2PrefitOfGLO;
            gnssLinearCombination c3PrefitOfGLO = linearComb.c3PrefitOfGLO;
            gnssLinearCombination c4PrefitOfGLO = linearComb.c4PrefitOfGLO;
            gnssLinearCombination c6PrefitOfGLO = linearComb.c6PrefitOfGLO;

            linearListGLO.push_back(c1PrefitOfGLO);
            linearListGLO.push_back(c2PrefitOfGLO);
            linearListGLO.push_back(c3PrefitOfGLO);
            linearListGLO.push_back(c4PrefitOfGLO);
            linearListGLO.push_back(c6PrefitOfGLO);

            for(auto it=linearListGLO.begin(); it!=linearListGLO.end(); it++)
            {
                gnssLinearCombination codeComb = (*it);
                if(debug)
                {
                    cout << getClassName() << "GLO:" << codeComb.header << endl;
                }

                (*it).addOptionalType(commonOptTypes);

                string prefitTypeStr = (*it).header.asString();
                string obsTypeStr = prefitTypeStr.substr(6,3);
                string rcvCorrTypeStr = "rcvCorr" + obsTypeStr;
                TypeID rcvCorrType = TypeID(rcvCorrTypeStr);

                if(debug)
                {
                    cout << "prefitTypeStr:" << prefitTypeStr << endl;
                    cout << "obsTypeStr:" << obsTypeStr << endl;
                    cout << "rcvCorrTypeStr:" << rcvCorrTypeStr << endl;
                }

                (*it).addOptionalType(rcvCorrType);

                string satPCenterTypeStr = "satPCenter" + obsTypeStr;
                TypeID satPCenterType = TypeID(satPCenterTypeStr);
                (*it).addOptionalType(satPCenterType);
            }

            sysCombs[SatelliteSystem::Glonass] = linearListGLO;

            gnssLinearCombination c1PrefitOfGAL = linearComb.c1PrefitOfGAL;
            gnssLinearCombination c5PrefitOfGAL = linearComb.c5PrefitOfGAL;
            gnssLinearCombination c6PrefitOfGAL = linearComb.c6PrefitOfGAL;
            gnssLinearCombination c7PrefitOfGAL = linearComb.c7PrefitOfGAL;
            gnssLinearCombination c8PrefitOfGAL = linearComb.c8PrefitOfGAL;

            linearListGAL.push_back(c1PrefitOfGAL);
            linearListGAL.push_back(c5PrefitOfGAL);
            linearListGAL.push_back(c7PrefitOfGAL);
            linearListGAL.push_back(c8PrefitOfGAL);
            linearListGAL.push_back(c6PrefitOfGAL);

            for(auto it=linearListGAL.begin(); it!=linearListGAL.end(); it++)
            {
                gnssLinearCombination codeComb = (*it);
                if(debug)
                {
                    cout << getClassName() << "GAL:" << codeComb.header << endl;
                }
                (*it).addOptionalType(commonOptTypes);

                string prefitTypeStr = (*it).header.asString();
                string obsTypeStr = prefitTypeStr.substr(6,3);
                string rcvCorrTypeStr = "rcvCorr" + obsTypeStr;
                TypeID rcvCorrType = TypeID(rcvCorrTypeStr);

                if(debug)
                {
                    cout << "prefitTypeStr:" << prefitTypeStr << endl;
                    cout << "obsTypeStr:" << obsTypeStr << endl;
                    cout << "rcvCorrTypeStr:" << rcvCorrTypeStr << endl;
                }

                (*it).addOptionalType(rcvCorrType);

                string satPCenterTypeStr = "satPCenter" + obsTypeStr;
                TypeID satPCenterType = TypeID(satPCenterTypeStr);
                (*it).addOptionalType(satPCenterType);
            }

            sysCombs[SatelliteSystem::Galileo] = linearListGAL;

            gnssLinearCombination c1PrefitOfBDS = linearComb.c1PrefitOfBDS;
            gnssLinearCombination c2PrefitOfBDS = linearComb.c2PrefitOfBDS;
            gnssLinearCombination c5PrefitOfBDS = linearComb.c5PrefitOfBDS;
            gnssLinearCombination c6PrefitOfBDS = linearComb.c6PrefitOfBDS;
            gnssLinearCombination c7PrefitOfBDS = linearComb.c7PrefitOfBDS;
            gnssLinearCombination c8PrefitOfBDS = linearComb.c8PrefitOfBDS;

            linearListBDS.push_back(c1PrefitOfBDS);
            linearListBDS.push_back(c2PrefitOfBDS);
            linearListBDS.push_back(c5PrefitOfBDS);
            linearListBDS.push_back(c6PrefitOfBDS);
            linearListBDS.push_back(c7PrefitOfBDS);
            linearListBDS.push_back(c8PrefitOfBDS);

            for(auto it=linearListBDS.begin(); it!=linearListBDS.end(); it++)
            {
                gnssLinearCombination codeComb = (*it);
                if(debug)
                {
                    cout << getClassName() << "BDS:" << codeComb.header << endl;
                }

                (*it).addOptionalType(commonOptTypes);
                string prefitTypeStr = (*it).header.asString();
                string obsTypeStr = prefitTypeStr.substr(6,3);
                string rcvCorrTypeStr = "rcvCorr" + obsTypeStr;
                TypeID rcvCorrType = TypeID(rcvCorrTypeStr);
                if(debug)
                {
                    cout << "prefitTypeStr:" << prefitTypeStr << endl;
                    cout << "obsTypeStr:" << obsTypeStr << endl;
                    cout << "rcvCorrTypeStr:" << rcvCorrTypeStr << endl;
                }
                (*it).addOptionalType(rcvCorrType);

                string satPCenterTypeStr = "satPCenter" + obsTypeStr;
                TypeID satPCenterType = TypeID(satPCenterTypeStr);
                (*it).addOptionalType(satPCenterType);
            }

            sysCombs[SatelliteSystem::BeiDou] = linearListBDS;


            // add prefit of QZSS
            gnssLinearCombination c6PrefitOfQZS = linearComb.c6PrefitOfQZS;
            gnssLinearCombination c5PrefitOfQZS = linearComb.c5PrefitOfQZS;
            gnssLinearCombination c2PrefitOfQZS = linearComb.c2PrefitOfQZS;
            gnssLinearCombination c1PrefitOfQZS = linearComb.c1PrefitOfQZS;

            linearListQZS.push_back(c6PrefitOfQZS);
            linearListQZS.push_back(c5PrefitOfQZS);
            linearListQZS.push_back(c2PrefitOfQZS);
            linearListQZS.push_back(c1PrefitOfQZS);

            for(auto it=linearListQZS.begin(); it!=linearListQZS.end(); it++)
            {
                gnssLinearCombination codeComb = (*it);

                if(debug)
                {
                    cout << getClassName() << "QZS:" << codeComb.header << endl;
                }
                (*it).addOptionalType(commonOptTypes);

                string prefitTypeStr = (*it).header.asString();
                string obsTypeStr = prefitTypeStr.substr(6,3);
                string rcvCorrTypeStr = "rcvCorr" + obsTypeStr;
                TypeID rcvCorrType = TypeID(rcvCorrTypeStr);
                if(debug)
                {
                    cout << "prefitTypeStr:" << prefitTypeStr << endl;
                    cout << "obsTypeStr:" << obsTypeStr << endl;
                    cout << "rcvCorrTypeStr:" << rcvCorrTypeStr << endl;
                }
                (*it).addOptionalType(rcvCorrType);

                string satPCenterTypeStr = "satPCenter" + obsTypeStr;
                TypeID satPCenterType = TypeID(satPCenterTypeStr);
                (*it).addOptionalType(satPCenterType);
            }

            sysCombs[SatelliteSystem::QZSS] = linearListQZS;

            // set sysCombs into computeCombs
            computeCombs.setSysCombs(sysCombs);
        }
        catch(Exception& e)
        {
            cerr << getClassName() << e << endl;
        }

    }

    void ComputeSPP::defineEquations(void)
    {

        // warning:
        // must clear equation system for each epoch
        equSys.clearEquations();

        //////////////////////////////////////////////////
        // for spp, only dx/dy/dz/dt are estimated, 
        // tropopsheric delay will 
        // be ignored.
        //////////////////////////////////////////////////

        // source-indexed variable
        Variable dx( TypeID::dX );
        Variable dy( TypeID::dY );
        Variable dz( TypeID::dZ );

        // vertical ionospheric delays 
        Variable iono( TypeID::iono, &ionoStoModel, true, true, false);

        VariableSet commonVarSet;
        commonVarSet.insert(dx);
        commonVarSet.insert(dy);
        commonVarSet.insert(dz);

        Variable cdtGPS( TypeID::dcdtGPS );
        Variable cdtGAL( TypeID::dcdtGAL );
        Variable cdtBDS( TypeID::dcdtBDS );
        Variable cdtGLO( TypeID::dcdtGLO );
        Variable cdtQZS( TypeID::dcdtQZS );

        // GPS Equation
        Equation equC1GPS( TypeID::prefitC1G, SatelliteSystem::GPS   );
        Equation equC2GPS( TypeID::prefitC2G, SatelliteSystem::GPS   );
        Equation equC5GPS( TypeID::prefitC5G, SatelliteSystem::GPS   );

        // GAL
        Equation equC1GAL( TypeID::prefitC1E, SatelliteSystem::Galileo );
        Equation equC5GAL( TypeID::prefitC5E, SatelliteSystem::Galileo );
        Equation equC7GAL( TypeID::prefitC7E, SatelliteSystem::Galileo );
        Equation equC8GAL( TypeID::prefitC8E, SatelliteSystem::Galileo );
        Equation equC6GAL( TypeID::prefitC6E, SatelliteSystem::Galileo );

        // BDS
        Equation equC1BDS( TypeID::prefitC1C, SatelliteSystem::BeiDou );
        Equation equC2BDS( TypeID::prefitC2C, SatelliteSystem::BeiDou);
        Equation equC5BDS( TypeID::prefitC5C, SatelliteSystem::BeiDou);
        Equation equC6BDS( TypeID::prefitC6C, SatelliteSystem::BeiDou);
        Equation equC7BDS( TypeID::prefitC7C, SatelliteSystem::BeiDou);
        Equation equC8BDS( TypeID::prefitC8C, SatelliteSystem::BeiDou);

        // GLO
        Equation equC1GLO( TypeID::prefitC1R, SatelliteSystem::Glonass );
        Equation equC2GLO( TypeID::prefitC2R, SatelliteSystem::Glonass);
        Equation equC3GLO( TypeID::prefitC3R, SatelliteSystem::Glonass);
        Equation equC4GLO( TypeID::prefitC4R, SatelliteSystem::Glonass);
        Equation equC6GLO( TypeID::prefitC6R, SatelliteSystem::Glonass);
        
        // now, let's insert variables into all equations
        Equation equC1QZS( TypeID::prefitC1J, SatelliteSystem::QZSS );
        Equation equC2QZS( TypeID::prefitC2J, SatelliteSystem::QZSS );
        Equation equC5QZS( TypeID::prefitC5J, SatelliteSystem::QZSS );
        Equation equC6QZS( TypeID::prefitC6J, SatelliteSystem::QZSS );

        //
        // add variables into equation
        //

        // cdt for GPS/GAL/BDS

        equC5GPS.addVarSet(commonVarSet);
        equC2GPS.addVarSet(commonVarSet);
        equC1GPS.addVarSet(commonVarSet);

        equC6GLO.addVarSet(commonVarSet);
        equC4GLO.addVarSet(commonVarSet);
        equC3GLO.addVarSet(commonVarSet);
        equC2GLO.addVarSet(commonVarSet);
        equC1GLO.addVarSet(commonVarSet);

        equC6BDS.addVarSet(commonVarSet);
        equC8BDS.addVarSet(commonVarSet);
        equC7BDS.addVarSet(commonVarSet);
        equC5BDS.addVarSet(commonVarSet);
        equC2BDS.addVarSet(commonVarSet);
        equC1BDS.addVarSet(commonVarSet);

        equC6GAL.addVarSet(commonVarSet);
        equC8GAL.addVarSet(commonVarSet);
        equC7GAL.addVarSet(commonVarSet);
        equC5GAL.addVarSet(commonVarSet);
        equC1GAL.addVarSet(commonVarSet);

        equC6QZS.addVarSet(commonVarSet);
        equC5QZS.addVarSet(commonVarSet);
        equC2QZS.addVarSet(commonVarSet);
        equC1QZS.addVarSet(commonVarSet);

        // add clock-bias into equations
        equC6BDS.addVariable(cdtBDS, 1.0);
        equC8BDS.addVariable(cdtBDS, 1.0);
        equC7BDS.addVariable(cdtBDS, 1.0);
        equC5BDS.addVariable(cdtBDS, 1.0);
        equC2BDS.addVariable(cdtBDS, 1.0);
        equC1BDS.addVariable(cdtBDS, 1.0);
        equC6GLO.addVariable(cdtGLO, 1.0);
        equC4GLO.addVariable(cdtGLO, 1.0);
        equC3GLO.addVariable(cdtGLO, 1.0);
        equC2GLO.addVariable(cdtGLO, 1.0);
        equC1GLO.addVariable(cdtGLO, 1.0);
        equC6GAL.addVariable(cdtGAL, 1.0);
        equC8GAL.addVariable(cdtGAL, 1.0);
        equC7GAL.addVariable(cdtGAL, 1.0);
        equC5GAL.addVariable(cdtGAL, 1.0);
        equC1GAL.addVariable(cdtGAL, 1.0);
        equC5GPS.addVariable(cdtGPS, 1.0);
        equC2GPS.addVariable(cdtGPS, 1.0);
        equC1GPS.addVariable(cdtGPS, 1.0);

        equC6QZS.addVariable(cdtQZS, 1.0);
        equC5QZS.addVariable(cdtQZS, 1.0);
        equC2QZS.addVariable(cdtQZS, 1.0);
        equC1QZS.addVariable(cdtQZS, 1.0);
        // estimate ionosphere variable
        opts.frqs = 1;
        if(opts.frqs > 2)
        {

            // gps
            Variable ionoMapC1G(TypeID::ionoMapC1G);
            Variable ionoMapC2G(TypeID::ionoMapC2G);
            Variable ionoMapC5G(TypeID::ionoMapC5G);

            equC1GPS.addVariable(iono, ionoMapC1G);
            equC2GPS.addVariable(iono, ionoMapC2G);
            equC5GPS.addVariable(iono, ionoMapC5G);

            // gal
            Variable ionoMapC1E(TypeID::ionoMapC1E);
            Variable ionoMapC5E(TypeID::ionoMapC5E);
            Variable ionoMapC6E(TypeID::ionoMapC6E);
            Variable ionoMapC8E(TypeID::ionoMapC8E);
            Variable ionoMapC7E(TypeID::ionoMapC7E);

            equC1GAL.addVariable(iono, ionoMapC1E);
            equC5GAL.addVariable(iono, ionoMapC5E);
            equC6GAL.addVariable(iono, ionoMapC6E);
            equC8GAL.addVariable(iono, ionoMapC8E);
            equC7GAL.addVariable(iono, ionoMapC7E);

            // bds
            Variable ionoMapC1C(TypeID::ionoMapC1C);
            Variable ionoMapC2C(TypeID::ionoMapC2C);
            Variable ionoMapC5C(TypeID::ionoMapC5C);
            Variable ionoMapC6C(TypeID::ionoMapC6C);
            Variable ionoMapC8C(TypeID::ionoMapC8C);
            Variable ionoMapC7C(TypeID::ionoMapC7C);

            equC1BDS.addVariable(iono, ionoMapC1C);
            equC2BDS.addVariable(iono, ionoMapC2C);
            equC5BDS.addVariable(iono, ionoMapC5C);
            equC6BDS.addVariable(iono, ionoMapC6C);
            equC8BDS.addVariable(iono, ionoMapC8C);
            equC7BDS.addVariable(iono, ionoMapC7C);

            // glonass
            Variable ionoMapC1R(TypeID::ionoMapC1R);
            Variable ionoMapC2R(TypeID::ionoMapC2R);
            Variable ionoMapC3R(TypeID::ionoMapC3R);
            Variable ionoMapC4R(TypeID::ionoMapC4R);
            Variable ionoMapC6R(TypeID::ionoMapC6R);

            equC6GLO.addVariable(iono, ionoMapC6R);
            equC4GLO.addVariable(iono, ionoMapC4R);
            equC3GLO.addVariable(iono, ionoMapC3R);
            equC2GLO.addVariable(iono, ionoMapC2R);
            equC1GLO.addVariable(iono, ionoMapC1R);

            // qzss
            Variable ionoMapC1J(TypeID::ionoMapC1J);
            Variable ionoMapC2J(TypeID::ionoMapC2J);
            Variable ionoMapC5J(TypeID::ionoMapC5J);
            Variable ionoMapC6J(TypeID::ionoMapC6J);

            equC6QZS.addVariable(iono, ionoMapC6J);
            equC5QZS.addVariable(iono, ionoMapC5J);
            equC2QZS.addVariable(iono, ionoMapC2J);
            equC1QZS.addVariable(iono, ionoMapC1J);
        }


        //
        // insert all equations into equation system 
        //
        if(opts.system.find("G") != string::npos)
        {
            if (opts.frqs>=1)
            {
                equSys.addEquation(equC1GPS);
            }
            if (opts.frqs>=2)
            {
                equSys.addEquation(equC2GPS);
            }
            if (opts.frqs>=3)
            {
                equSys.addEquation(equC5GPS);
            }
            
        }

        if(opts.system.find("E") != string::npos)
        {
            if (opts.frqs >= 1)
            {
                equSys.addEquation(equC1GAL);
            }
            if (opts.frqs >= 2)
            {
                equSys.addEquation(equC5GAL);
            }
            if (opts.frqs >= 3)
            {
                equSys.addEquation(equC7GAL);
            }
            if (opts.frqs >= 4)
            {
                equSys.addEquation(equC8GAL);
            }
            if (opts.frqs >= 5)
            {
                equSys.addEquation(equC6GAL);
            }
        }

        if(opts.system.find("R") != string::npos)
        {
            equSys.addEquation(equC6GLO);
            equSys.addEquation(equC4GLO);
            equSys.addEquation(equC3GLO);
            equSys.addEquation(equC2GLO);
            equSys.addEquation(equC1GLO);
        }

        if(opts.system.find("C") != string::npos)
        {
            if (opts.frqs >= 1)
            {
                if (opts.bdsfopt)
                    equSys.addEquation(equC1BDS);
                else
                    equSys.addEquation(equC2BDS);
            }
            if (opts.frqs >= 2)
            {
                if(opts.bdsfopt)
                    equSys.addEquation(equC5BDS);
                else
                    equSys.addEquation(equC6BDS);
            }
            if (opts.frqs >= 3)
            {
                if(opts.bdsfopt)
                    equSys.addEquation(equC2BDS);
                else
                    equSys.addEquation(equC1BDS);
            }
            if (opts.frqs >= 4)
            {
                if(opts.bdsfopt)
                    equSys.addEquation(equC6BDS);
                else
                    equSys.addEquation(equC5BDS);
            }
            if (opts.frqs >= 5)
            {
                equSys.addEquation(equC8BDS);
            }
        }

        if(opts.system.find("J") != string::npos)
        {
            equSys.addEquation(equC6QZS);
            equSys.addEquation(equC5QZS);
            equSys.addEquation(equC2QZS);
            equSys.addEquation(equC1QZS);
        }

    } // end of 'defineEquations()'


    gnssRinex& ComputeSPP::Process(gnssRinex& gRin)
    {

        // define equation systems
        defineEquations();

        CommonTime time = gRin.header.epoch;
        satTypeValueMap& gData = gRin.body;
        Triple currentPos;
        if (gRin.header.refPos.mag() != 0.)
            currentPos = gRin.header.refPos;
        else if (gRin.header.antennaPosition.mag() != 0.)
            currentPos = gRin.header.antennaPosition;
        else
            currentPos = Triple(100., 100., 100.);

        if(debug)
        {
            cout << "currentPos:" << currentPos << endl;
        }

        double delta;
        int iter(0);
        while(true)
        {
            if(debug)
            {
                cout << "currentPos:" << currentPos << endl;
            }

            computeDerivative.setCoordinates(currentPos);
            computeDerivative.Process(gRin);

            computeTrop.setAllParameters(time, currentPos, 0);
            computeTrop.Process(gRin);

            computeIono.setInitialRxPosition(currentPos);
            computeIono.Process(gRin);

            computeCombs.Process(gRin);

            if (debug)
            {
                gRin.dump(cout, 1);
            }


            Triple deltaPos = Solve(gRin);

            currentPos = currentPos + deltaPos;

            delta = deltaPos.mag();
            iter++;
            if(delta < 0.0001 || iter > 11 )
            {
                break;
            }
        }   // End of while true
        
        if(firstTime)
        {
            firstTime = false;
        }
        // added by ragnwang
        if (std::find(solution.begin(), solution.end(), TypeID::dcdtGPS)!=solution.end())
            gRin.header.dcdtr.insert(make_pair(TypeID::dcdtGPS, getSolution(TypeID::dcdtGPS, solution)));
		if (std::find(solution.begin(), solution.end(), TypeID::dcdtBDS)!=solution.end())
			gRin.header.dcdtr.insert(make_pair(TypeID::dcdtGPS, getSolution(TypeID::dcdtBDS, solution)));
		if (std::find(solution.begin(), solution.end(), TypeID::dcdtGAL)!=solution.end())
			gRin.header.dcdtr.insert(make_pair(TypeID::dcdtGPS, getSolution(TypeID::dcdtGAL, solution)));
        gRin.header.rcvPos = currentPos;
        return gRin;
    }   // end of gnssRinex& ComputeSPP::Process(gnssRinex& gRin)

    Position ComputeSPP::Solve(gnssRinex& gRin)
    {

        sourceRinex sRin;
        sRin.addGnssRinex(gRin);

        equSys.Prepare(sRin);

        currentUnkSet = equSys.getVarUnknowns();

        if(debug)
        {
            cout << "Unknowns:" << endl;
            for(auto it=currentUnkSet.begin(); it!= currentUnkSet.end(); it++)
            {
                cout << (*it) << endl;
            }
        }

        std::set<Equation> desSet = equSys.getDescripEqus();
        if(debug)
        {
            cout << "desSet" << endl;
            for(auto it = desSet.begin(); it!= desSet.end(); it++)
            {
                cout << (*it) << endl;
            }
        }

        std::set<Equation> equSet = equSys.getCurrentEquationsSet();
        if(debug)
        {
            cout << "equSet" << endl;
            for(auto it = equSet.begin(); it!= equSet.end(); it++)
            {
                cout << (*it) << endl;
            }
        }

        Vector<double> prefit = equSys.getPrefitsVector();
        Matrix<double> hMatrix = equSys.getGeometryMatrix();
        Matrix<double> hT = transpose(hMatrix);

        if(debug)
        {
            cout << "prefit:" << endl;
            cout << prefit << endl;

            cout << "hMatrix:" << endl;
            cout << hMatrix<< endl;

        }


        try
        {
            covMatrix = hT * hMatrix;
            covMatrix = inverseChol( covMatrix );
        }
        catch(...)
        {
            InvalidSolver e("Unable to invert matrix covMatrix");
            GPSTK_THROW(e);
        }

        solution = covMatrix * hT * prefit;

        Vector<double> postfit;
        postfit = prefit - hMatrix* solution;

        Triple delta;
        double dx = getSolution(TypeID::dX, solution);
        double dy = getSolution(TypeID::dY, solution);
        double dz = getSolution(TypeID::dZ, solution);

        delta[0] = dx;
        delta[1] = dy;
        delta[2] = dz;

        return delta;

    }


    double ComputeSPP::getSolution( const TypeID& type,
                                    const Vector<double>& stateVec ) const
      noexcept(false)
    {
          // Declare an varIterator for 'stateMap' and go to the first element
       auto varIt = currentUnkSet.begin();
       while( (*varIt).getType() != type )
       {
          // If the same type is not found, throw an exception
          if( varIt == currentUnkSet.end() )
          {
              InvalidRequest e(getClassName() + " Type not found in solution vector.");
              GPSTK_THROW(e);
          }

          varIt++;
       }

          // Else, return the corresponding value
       return stateVec( (*varIt).getNowIndex() );

    }  // End of method 'SolverGeneral::getSolution()'   

} // end of namespace gpstk
