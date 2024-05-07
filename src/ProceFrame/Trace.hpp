#pragma ident "$ragnwang$"
#ifndef FIPPP_TRACE_HPP
#define FIPPP_TRACE_HPP

#include <iostream>
#include <set>

#include "Matrix.hpp"
#include "Vector.hpp"
#include "Triple.hpp"

using namespace gpstk;
using namespace std;

namespace fippp
{

	template<typename T>
	void traceMatrix(std::ofstream* traceStream, std::string name, const Matrix<T>& matrix)
	{
		if (traceStream)
		{
			(*traceStream) << name << endl;
			(*traceStream) << matrix << endl;
		}
	}

	template<typename T>
	void traceVector(std::ofstream* traceStream, std::string name, const Vector<T>& vec)
	{
		if (traceStream)
		{
			(*traceStream) << name << endl;
			//(*traceStream) << endl;
			for (auto itData = vec.begin();
				itData != vec.end();
				++itData)
			{
				(*traceStream) << " "
					<< *itData << endl;
			}
		}
	}

	//void traceTriple(std::ofstream* traceStream, std::string name, const Triple& tri)
	//{
	//	if (traceStream)
	//	{
	//		(*traceStream) << name << endl;
	//		(*traceStream) << tri << endl;
	//	}
	//}

	template<typename T>
	void traceSet(std::ofstream* traceStream, std::string name, const std::set<T>& sets)
	{
		if (traceStream != NULL)
		{
			(*traceStream) << name << endl;
			for (auto itData = sets.begin();
				itData != sets.end();
				++itData)
			{
				(*traceStream) << " "
					<< *itData << endl;
			}
		}
	}


	template<typename T>
	void traceT(std::ofstream* traceStream, const std::string& name, const T& t)
	{
		if (traceStream != NULL)
		{
			(*traceStream) << name << endl;
			(*traceStream) << fixed
				<< setprecision(8)
				<<t << endl;
		}
	}
	//template<typename T>
	//void trace(std::ofstream* traceStream, const std::string& name, const T& t)
	//{
	//	if (traceStream != NULL)
	//	{
	//		//auto type_ = typeid(t).name;
	//		//if (type_ == gpstk::Triple)
	//		//	traceTriple(traceStream, name, t);
	//		//else if (type_ == gpstk::Matrix)
	//		//	traceMatrix(traceStream, name, t);
	//		//else if (type_ == gpstk::Vector)
	//		//	traceVector(traceStream, name, t);
	//		//else if (type_ == std::set)
	//		//	traceSet(traceStream, name, t);
	//		//else
	//		//	traceT(traceStream, name, t);
	//	}
	//}
	
}

#endif // !FIPPP_TRACE_HPP

