#pragma ident "$Id: ragn wang 2023-03-14"

#ifndef FindFile_HPP
#define FindFile_HPP

#include <set>
#include <map>
#include <iostream>


#include "TypeID.hpp"
#include "CommonTime.hpp"
#include "YDSTime.hpp"
#include "GPSWeekZcount.hpp"
#include "StringProcess.hpp"

using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp
{
	class FindFile
	{
	public:

		//default
		FindFile() {};

		FindFile(const std::string& dir):path(dir)
		{
			fileMap.clear();
			pObsSet.clear();
			traverseDir();
		};

		void setPath(const std::string& dir)
		{
			path = dir;
		}

		// traverse folder to get all the file added by ragn wang
		virtual void traverseDir();
		// find file
		virtual bool findObsFile(std::string& obs);
		virtual bool findEphFile(const CommonTime& epoch, std::string& p);
		virtual bool findBiaFile(const CommonTime& epoch, std::string& bia);
		virtual bool findDCBFile(const CommonTime& epoch, std::string& dcb);
		virtual bool findSnxFile(const CommonTime& epoch, std::string& snx);

		virtual bool findSGGIfcb(const CommonTime& epoch, std::vector<std::string>& ifcb);
		virtual bool findSp3File(const CommonTime& epoch, std::vector<std::string>& sp3Vec);
		virtual bool findClkFile(const CommonTime& epoch, std::vector<std::string>& clkVec);
		virtual bool findErpFile(const CommonTime& epoch, std::vector<std::string>& erpVec);
		// find upd
		virtual bool findUPDFile(const CommonTime& epoch, std::map<TypeID, std::string>& updMap);

//		// get obset
//		virtual std::set<std::string> getObsSet()
//		{
//			if (fileMap.find(TypeID::OBSF) != fileMap.end())
//				return fileMap[TypeID::OBSF];
//		};

		// add has processed obs file
		virtual void addPObs(const std::string& obs)
		{
			pObsSet.insert(obs);
		};

		virtual ~FindFile() {};



	private:
		// input dir path
		std::string path;
		// had be processed obs file
		std::set<std::string> pObsSet;
		//
		std::map<TypeID, std::set<std::string>> fileMap;
	};
}


#endif // !FINDFILE_HPP