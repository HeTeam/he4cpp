#pragma once
#include"AIClass.h"
#include"world.h"
#include<string>
#include<typeinfo>
using namespace std;

#define runtime_of(cls)  (&(Classes::getInstance().types[Classes::getInstance().getRuntimeClassID<cls>()]))
#define __THIS_ID__  (__LINE__ - start_classes_define - 1)
#define DECLARE_TYPE(cls) template<>int getRuntimeClassID<cls>() { return __THIS_ID__; }; template<>struct GetTypeById < __THIS_ID__ > { using _ty = cls; };
#define theClasses Classes::getInstance()

class Classes
{
	Classes(){}
public:
	Classes(Classes&) = delete;
	static Classes& getInstance()
	{
		static Classes obj;
		return obj;
	}

	Class types[250];
	string  staticNames[250];
	//string  runtimeNames[250];

	template<typename T>
	int getRuntimeClassID()
	{
		return -1;
	}

	template<int id>
	struct GetTypeById
	{
		using _ty = void;
	};

	enum { start_classes_define = __LINE__ };
	DECLARE_TYPE(Class);
	DECLARE_TYPE(bool);
	DECLARE_TYPE(int);
	DECLARE_TYPE(double);
	DECLARE_TYPE(string);
	DECLARE_TYPE(void*);
	DECLARE_TYPE(vector<void*>);
	DECLARE_TYPE(Bus);enum{start_jsonSupportedClassID= __THIS_ID__};
	DECLARE_TYPE(Subway);
	DECLARE_TYPE(Station);
	DECLARE_TYPE(Person);
	DECLARE_TYPE(Phone); 
	DECLARE_TYPE(Cup);
	DECLARE_TYPE(Kettle);
	DECLARE_TYPE(DrinkingFountain);
	DECLARE_TYPE(Solute);
	DECLARE_TYPE(Copier); 
	DECLARE_TYPE(Paper); 
	DECLARE_TYPE(Water); 
	DECLARE_TYPE(Coffee); enum { end_jsonSupportedClassID = __THIS_ID__ };
	enum{howManyClasses_onlyStatic=__THIS_ID__};

	template<int id>
	void _regist()
	{
		types[id].size = sizeof(GetTypeById<id>::_ty);
		using whatType = GetTypeById<id>::_ty;
		staticNames[id] = typeid(whatType).name();
		_regist < id - 1 >();
	}

	template<>
	void _regist<0>()
	{
		types[0].size = sizeof(GetTypeById<0>::_ty);
		using whatType = GetTypeById<0>::_ty;
		staticNames[0] = typeid(whatType).name();
	}

	template<int id>
	void _regist_jsonSupportedClass()
	{
		using whichClass = GetTypeById<id>::_ty;
		whichClass::regist();
		_regist_jsonSupportedClass<id - 1>();
	}

	template<>
	void _regist_jsonSupportedClass<start_jsonSupportedClassID>()
	{
		using whichClass = GetTypeById<start_jsonSupportedClassID>::_ty;
		whichClass::regist();
	}

	void regist_jsonSupportedClasses()
	{
		_regist_jsonSupportedClass<end_jsonSupportedClassID>();
	}

	template<int id>
	bool isStaticNameOfType(string str)
	{
		using thisClass = GetTypeById<id>::_ty;
		return str == typeid(thisClass).name();
	}

	template<int id>
	int _findByStaticTypeName(string name)
	{
		return isStaticNameOfType<id>(name) ? id : _findByStaticTypeName<id - 1>(name);
	}

	template<>
	int _findByStaticTypeName<0>(string name)
	{
		return isStaticNameOfType<0>(name)?0:-1;
	}

	int findByStaticTypeName(string name)
	{
		for (int i = 0; i < 250; i++)
			if (staticNames[i] == name)
				return i;
		return -1;
		//return _findByStaticTypeName<howManyClasses_onlyStatic - 1>(name);
	}
};
