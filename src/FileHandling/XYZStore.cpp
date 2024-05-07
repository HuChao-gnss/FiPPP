

#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

#include "Exception.hpp"
#include "XYZStore.hpp"
#include "Xvt.hpp"
#include "Vector.hpp"
#include "MJD.hpp"
#include "MiscMath.hpp"
#include "YDSTime.hpp"
#include "StringUtils.hpp"

using namespace gpstk;

using namespace gpstk::StringUtils;

#define debug 0

void XYZStore::loadFile(std::string filename)
{
   ifstream input(filename.c_str());
   if (!input)
   {
      FileMissingException
      fe ( "Cannot Open xyz-formatted file: " + filename );
      GPSTK_THROW(fe);
   }
   
   bool ok(true);
   
   string line;
   
   while( input.peek() != EOF )
   {
      getline( input, line );

      if(debug)
          cout << "data line:" << line << endl;

      istringstream iss(line);

      string station;
      int year, doy;
      double second;
      double x, y, z, vx, vy, vz;
      iss >> station >> year >> doy >> second >> x >> y >> z >> vx >> vy >> vz;

      if(debug)
          cout << station << " "
               << year << " "
               << doy << " "
               << second << " "
               << x << " "
               << y << " "
               << z << " "
               << vx << " "
               << vy << " "
               << vz << " "
               << endl;


      YDSTime yds(year, doy, second);
      CommonTime epoch = yds.convertToCommonTime();
      
      Xvt xvt;
      xvt.x[0] = x;
      xvt.x[1] = y;
      xvt.x[2] = z;

      xvt.v[0] = vx;
      xvt.v[1] = vy;
      xvt.v[2] = vz;

      stationXvtData[station][epoch] = xvt;

   }
   
   input.close();
   
   if( !ok )
   {
      FileMissingException fe( filename + "is corrupted..." );
      GPSTK_THROW(fe);
   }
   
} // END of "loadPositionFile"

void XYZStore::loadFile(std::string fileName, std::string station, Triple& rcvPos)
{
    ifstream input(fileName.c_str());
    if (!input)
    {
        FileMissingException
            fe("Cannot Open xyz-formatted file: " + fileName);
        GPSTK_THROW(fe);
    }

    bool ok(true);
    bool inregion(false);
    int find;
    std::string line;
    Xvt xvt;
    int year(0), doy(0);
    double sec(0.0);

    while (input.peek() != EOF)
    {
        getline(input, line);
        if (std::string::npos != line.find("+SOLUTION/ESTIMATE"))
        {
            inregion = true;
        }
        if (std::string::npos != line.find("-SOLUTION/ESTIMATE"))
        {
            inregion = false;
            break;
        }
        if (inregion)
        {
            
            std::string sta = upperCase(line.substr(14, 4));
            find = line.find("STAX");
            if (line.substr(14, 4)==station && find != std::string::npos)
            {
                year = asInt("20" + line.substr(27, 2));
                doy = asInt(line.substr(30, 3));
                sec = asDouble(line.substr(34, 5));
                xvt.x[0] = asDouble(line.substr(47, 21));
                xvt.v[0] = asDouble(line.substr(69, 11));
            }
            find = line.find("STAY");
            if (line.substr(14, 4) == station && find != std::string::npos)
            {
                xvt.x[1] = asDouble(line.substr(47, 21));
                xvt.v[1] = asDouble(line.substr(69, 11));
            }
            find = line.find("STAZ");
            if (line.substr(14, 4) == station && find != std::string::npos)
            {
                xvt.x[2] = asDouble(line.substr(47, 21));
                xvt.v[2] = asDouble(line.substr(69, 11));
                break;
            }
        }   /// END of if (inregion)
    }   // END of while (input.peek() != EOF)

    input.close();

    if (xvt.x.mag() == 0.0 || xvt.x.size() == 0) ok = false;

    YDSTime yds(year, doy, sec);
    CommonTime epoch = yds.convertToCommonTime();
    stationXvtData[station][epoch] = xvt;
    rcvPos = xvt.x;

    if (!ok)
    {
        Exception e("cannot find " + station + " reference coords in\
the '" + fileName + "'");
        GPSTK_THROW(e);
    }
} // END of "loadPositionFile"


Triple XYZStore::getPos(std::string station, CommonTime& t)
{

    t.setTimeSystem(TimeSystem::Unknown);

    Triple Pos;
    string stationUpper = StringUtils::upperCase(station);
    string stationLower= StringUtils::lowerCase(station);
    auto itUpper = stationXvtData.find(stationUpper);
    auto itLower = stationXvtData.find(stationLower);
    if(itUpper != stationXvtData.end())
    {
        epochXvtMap epochXvt;
        epochXvt = (*itUpper).second;

        auto itUpper = epochXvt.begin();

        CommonTime refEpoch = (*itUpper).first;
        Xvt xvt = (*itUpper).second;

        Triple pos = xvt.x;

        return pos;
    }
    else if(itLower != stationXvtData.end())
    {
        epochXvtMap epochXvt;
        epochXvt = (*itLower).second;

        auto itLower = epochXvt.begin();

        CommonTime refEpoch = (*itLower).first;
        Xvt xvt = (*itLower).second;

        Triple pos = xvt.x;
        return pos;
    }
    else
    {
        InvalidRequest e(station + " not found.");
        GPSTK_THROW(e);
    }

} // END of "getPos"

Triple XYZStore::getVel(std::string station, CommonTime& t)
{
    // to be extended.
    Triple temp;
    return temp;
}

