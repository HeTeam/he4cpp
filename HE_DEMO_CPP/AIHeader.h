#pragma once
#include<vector>
using namespace std;

#define DEBUG_MSG(format,...) {printf(format,__VA_ARGS__);printf("\n");}
#define stdFind(arr,element) std::find(arr.begin(),arr.end(),element)
#define stdFindExist(arr,element) (stdFind(arr,element)!=arr.end())
#define pushBackIfNew(arr,element) {if(!stdFindExist(arr,element)) arr.push_back(element);}

template<typename T>
std::vector<T> toObjectArray(std::vector<T*>& pointerArray)
{
	std::vector<T> results;
	for (T* ptr : pointerArray)
		results.push_back(*ptr);
	return results;
}

#define _toObjectArray(arr) toObjectArray<std::remove_pointer<decltype(arr)::value_type>::type>(arr)

template<typename T>
std::vector<T*> toPointerArray(std::vector<T>& objectArray)
{
	std::vector<T*> results;
	for (T& obj : objectArray)
		results.push_back(&obj);
	return results;
}
#define _toPointerArray(arr) toPointerArray<decltype(arr)::value_type>(arr)

#define addAllInto(arr1,arr2) {for(auto eachOne:arr2) (arr1).push_back(eachOne);}
#define cast_assign(a,b) a= (decltype(a))b
#define castAndAddAllInto(arr1,arr2)  { for(auto eachOne:arr2) (arr1).push_back( (decltype(arr1::value_type))eachOne ); }

#define strContains(s1,s2)  (string::npos!=s1.find(s2))
vector<std::string> split(string s, string delim);
string trim(string str);
string randString();
string strJoin(vector<string>, string separator);
inline bool isString(string& str)
{
	if (str.size() < 2)
		return false;
	if (str[0] == '\"' && '\"' == str[str.size() - 1])
		return true;
	return false;
}
inline string removeQuotation(string& str)
{
	return str.substr(1, str.size() - 2);
}
inline bool isPtrArrayStr(string& str)
{
	if (str.size() < 2)
		return false;
	if (str[0] == '{' && '}' == str[str.size() - 1])
		return true;
	return false;
}
inline string removeBraces(string& str)
{
	return str.substr(1, str.size() - 2);
}
vector<void*> toPtrArr(string str);

template<typename T>
T _randomChoose(vector<T> arr)
{
	return arr[rand() % arr.size()];
}

#define randomChoose(arr) _randomChoose<decltype(arr)::value_type>(arr)

template<typename T>
string _arrToString(vector<T> arr)
{
	string s;
	for (size_t i = 0; i < arr.size(); i++)
	{
		s += arr[i].toString();
		if (i != arr.size() - 1)
			s += "\n";
	}
	return s;
}

#define arrToString(arr) _arrToString<decltype(arr)::value_type>(arr)

string getCurrentTime();