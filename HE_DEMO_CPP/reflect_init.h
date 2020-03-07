#pragma once
#include"reflect.h"

template<int id>
void _registFunc()
{
	using FuncInfo = GetFuncById<id>;
	using FuncType = FuncInfo::type;
	FuncRegister<FuncType, id>::regist<_getNumOfArgs(FuncType)>();
	_registFunc<id - 1>();
}

template<>
void _registFunc<0>()
{
	using FuncInfo = GetFuncById<0>;
	using FuncType = FuncInfo::type;
	FuncRegister<FuncType, 0>::regist<_getNumOfArgs(FuncType)>();
}

#define REGIST_BASIC_TYPE(name_static,name_runtime) index_basic = theClasses.getRuntimeClassID<name_static>();\
staticName = typeid(name_static).name();\
theClasses.staticNames[index_basic] = staticName;\
theTypeLogger.map_staticAndRuntimeName.insert({staticName, name_runtime});\
theTypeLogger.map_offset.insert({staticName,map<string, int>()});\
theTypeLogger.map_staticFieldTypeName.insert({staticName,map<string,string>()});


#define theWorld TheWorld::getInstance()

#define DECLARE_OBJECTS(type,head,...) type head,__VA_ARGS__;\
			template<>string getNamesString<type>(){return string(#head)+","+string(#__VA_ARGS__);}\
            template<>void* getAddressOfFirst<type>(){return &head;}
#define REGIST_OBJECTS(type) setNamesAndCreateConceptNodes<type>(getNamesString<type>())
