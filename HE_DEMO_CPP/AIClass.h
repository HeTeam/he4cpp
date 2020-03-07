#pragma once
#include<map>
#include<string>
using namespace std;

class Class
{
	Class(Class&) = delete;
public:
	Class(){}
	int size=0;
	string staticName, runtimeName;
	map<string, int> map_offset;
	map<string, Class*> map_fieldType;
	void setOffsetsAndFieldType(map<string, int>& inputMap, map<string, string> inputTypes);
};