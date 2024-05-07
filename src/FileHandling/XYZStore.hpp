
#ifndef GPSTK_XYZStore_HPP
#define GPSTK_XYZStore_HPP

#include <iostream>
#include <cmath>
#include <vector>

#include "CommonTime.hpp"
#include "Triple.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "ReferenceSystem.hpp"
#include "MJD.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   typedef std::map<CommonTime, Xvt> epochXvtMap;
   typedef std::map<std::string, epochXvtMap> stationXvtMap;

   class XYZStore
   {
   public:
      
      XYZStore()
      {}
      
      virtual ~XYZStore()
      {}
      
      virtual void loadFile(std::string filename);
      virtual void loadFile(std::string fileName, std::string station, Triple& rcvPos);

      virtual Triple getPos(std::string station, CommonTime& t);
      virtual Triple getVel(std::string station, CommonTime& t);
      
   private:
   
      CommonTime epoch;
      
      double x, y, z, vx, vy, vz;
      
      Xvt xvt;
      
      stationXvtMap stationXvtData;
      
   }; // END of "class XYZStore"
   
} // END of "namespace gpstk"










#endif
