#pragma ident "$Id$"

#include "StringProcess.hpp"
#include <sstream>
#include <iostream>

namespace gpstk {

	void stringSplit(const std::string& str, std::vector<std::string>& res, const char split)
	{
		if (str == "")		return;
		//add split key at the end of the input string
		std::string strs = str + split;
		std::istringstream strStream(strs);
		std::string token;
		while (getline(strStream, token, split))	//
		{
			res.push_back(token);
		}
	}
	/// <summary>
	/// get the split marker and split the path
	/// </summary>
	/// <param name="path"> I path to split</param>
	/// <param name="res"> I split result</param>
	/// <param name="split"> O split marker </param>
	void pathSplitType(const std::string& path, std::vector<std::string>& res, char& split)
	{
		size_t pos1 = path.find_last_of("\\");
		size_t pos2 = path.find_last_of("/");
		if ((pos1==std::string::npos&&pos2==std::string::npos)||
			(pos1!=std::string::npos&&pos2!=std::string::npos))
		{
			std::cout << "Please check the path carefully!" << std::endl;
			std::cout << "  " <<
				path << std::endl;
			exit(-1);
		}
		if (pos1 != std::string::npos)
			split = *path.substr(pos1, 1).c_str();
		if (pos2 != std::string::npos)
			split = *path.substr(pos2, 1).c_str();
		//add split key at the end of the input string
		std::string strs = path + split;
		std::istringstream strStream(strs);
		std::string token;
		while (getline(strStream, token, split))	//
		{
			res.push_back(token);
		}			
	}

	void pathSplit(const std::string& path, std::vector<std::string>& res)
	{
		size_t pos1 = path.find_last_of("\\");
		size_t pos2 = path.find_last_of("/");
		if ((pos1 == std::string::npos && pos2 == std::string::npos) ||
			(pos1 != std::string::npos && pos2 != std::string::npos))
		{
			std::cout << "Please check the path carefully!" << std::endl;
			std::cout << "  " <<
				path << std::endl;
			exit(-1);
		}
		char splitType;
		if (pos1 != std::string::npos)
			splitType = *path.substr(pos1, 1).c_str();
		if (pos2 != std::string::npos)
			splitType = *path.substr(pos2, 1).c_str();
		//add split key at the end of the input string
		std::string strs = path + splitType;
		std::istringstream strStream(strs);
		std::string token;
		while (getline(strStream, token, splitType))	//
		{
			res.push_back(token);
		}
	}

	void pathType(const std::string& path, char& split)
	{
		size_t pos1 = path.find_last_of("\\");
		size_t pos2 = path.find_last_of("/");
		if ((pos1 == std::string::npos && pos2 == std::string::npos) ||
			(pos1 != std::string::npos && pos2 != std::string::npos))
		{
			std::cout << "Please check the path carefully!" << std::endl;
			std::cout << "  " <<
				path << std::endl;
			exit(-1);
		}
		if (pos1 != std::string::npos)
			split = *path.substr(pos1, 1).c_str();
		if (pos2 != std::string::npos)
			split = *path.substr(pos2, 1).c_str();
	}

	void stringStitch(const std::vector<std::string> strs, std::string& res, const char split)
	{
		if (strs.size() == 0) return;
		res = "";
		for (auto itStr = strs.begin();
			itStr != strs.end();
			++itStr)
		{
			res += *itStr + split;
		}
	}

}
