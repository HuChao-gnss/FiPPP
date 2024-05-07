#pragma ident "$Id$"



#ifndef STRING_PROCESS_HPP
#define STRING_PROCESS_HPP

#include <string>
#include <vector>

namespace gpstk
{
	void stringSplit(const std::string& str, std::vector<std::string>& res, const char split = ';');
	void stringStitch(const std::vector<std::string> strs, std::string& res, const char split='\\');
	void pathSplitType(const std::string& path, std::vector<std::string>& res, char& split);
	void pathSplit(const std::string& path, std::vector<std::string>& res);
	void pathType(const std::string& path, char& split);
}

#endif   // STRING_PROCESS_HPP
