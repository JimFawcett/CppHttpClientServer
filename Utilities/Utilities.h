#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally useful, helper classes             //
// ver 1.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  This class will be extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Utilities.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.3 : 01 Jan 2018
* - fixed indexing bugs in StringHelper::trim
* ver 1.2 : 22 Feb 2015
* - changed default underline char in Title(const std::string&, Char)
* ver 1.1 : 06 Feb 2015
* - fixed bug in split which turns a comma separated string into
*   a vector of tokens.
* - added comments
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <locale>

namespace Utilities
{
  class StringHelper
  {
  public:
    static void Title(const std::string& src, char underline = '=');
    static void title(const std::string& src);
    template<typename T>
    static std::basic_string<T> trim(const std::basic_string<T>& toTrim);
    template <typename T>
    static std::vector<std::basic_string<T>> split(const std::basic_string<T>& toSplit, T splitOn = ',');
  };

  /*--- remove whitespace from front and back of string argument ---*/

  template <typename T>
  std::basic_string<T> StringHelper::trim(const std::basic_string<T>& toTrim)
  {
    if (toTrim.size() == 0)
      return toTrim;
    std::basic_string<T> temp;
    std::locale loc;
    typename std::basic_string<T>::const_iterator iter = toTrim.begin();
    while (isspace(*iter, loc))
    {
      if (++iter == toTrim.end())
      {
        break;
      }
    }
    for (; iter != toTrim.end(); ++iter)
    {
      temp += *iter;
    }
    std::basic_string<T>::reverse_iterator riter;
    size_t pos = temp.size();
    for (riter = temp.rbegin(); riter != temp.rend(); ++riter)
    {
      --pos;
      if (!isspace(*riter, loc))
      {
        break;
      }
    }
    if(0 <= pos && pos < temp.size())
      temp.erase(++pos);
    return temp;
  }

  /*--- split sentinel separated strings into a vector of trimmed strings ---*/

  template <typename T>
  std::vector<std::basic_string<T>> StringHelper::split(const std::basic_string<T>& toSplit, T splitOn)
  {
    std::vector<std::basic_string<T>> splits;
    std::basic_string<T> temp;
    typename std::basic_string<T>::const_iterator iter;
    for (iter = toSplit.begin(); iter != toSplit.end(); ++iter)
    {
      if (*iter != splitOn)
      {
        temp += *iter;
      }
      else
      {
        splits.push_back(trim(temp));
        temp.clear();
      }
    }
    if (temp.length() > 0)
      splits.push_back(trim(temp));
    return splits;
  }

  template<typename T>
  class Tester
  {
  public:
    bool execute(T t,const std::string& name, std::ostream& out = std::cout);
  private:
    void check(bool result, std::ostream& out);
  };

  template <typename T>
  bool Tester<T>::execute(T t, const std::string& name, std::ostream& out)
  {
    bool result = false;
    try
    {
      out << "\n  test \"" << name << "\"";
      result = t();
    }
    catch (std::exception& ex)
    {
      out << "\n  " << ex.what();
    }
    check(result, out);
    return result;
  }

  template<typename T>
  void Tester<T>::check(bool result, std::ostream& out)
  {
    if (result)
      out << " passed\n";
    else
      out << " failed\n";
  }

  void putline();

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }
}
#endif
