#pragma once
#include<string>
#include<iostream>
#include<typeinfo>
#include<vector>
#include<map>
#include<functional>
#include<tuple>
#include<cstdarg>
using namespace std;

class TypeLogger
{
public:
	static TypeLogger& getInstance()
	{
		static TypeLogger obj;
		return obj;
	}
	map<string/*编译期类名*/, string/*运行时类名*/>  map_staticAndRuntimeName;
	map<string/*编译期类名*/, map<string/*动态属性名*/, int>> map_offset;
	map<string/*编译期类名*/, map<string/*动态属性名*/, string/*编译期类型名*/>> map_staticFieldTypeName;
}

#define theTypeLogger TypeLogger::getInstance()
#define LINE_TO_FIELD_ID    __LINE__ - fieldDeclarationBeginningLine - 1

#define BEGIN_FIELD_DECLARE enum{fieldDeclarationBeginningLine=__LINE__};\
template<int fieldId>\
class GetFieldInfo{public:\
	using type = bool;\
	enum{ size = sizeof(type) };\
	enum{ offset = 0 };\
	static string getName(){ return ""; }\
};


#define END_FIELD_DECLARE   enum{numberOfFields=LINE_TO_FIELD_ID};\
template<int fieldId>\
static void _regist()\
{\
	if(GetFieldInfo<fieldId>::offset>0){\
    string runtimeFieldName=GetFieldInfo<fieldId>::getName();\
    string staticFieldTypeName=typeid(GetFieldInfo<fieldId>::type).name();\
    theTypeLogger.map_staticFieldTypeName[getStaticTypeName()].insert({runtimeFieldName,staticFieldTypeName});\
	theTypeLogger.map_offset[getStaticTypeName()].insert({runtimeFieldName,GetFieldInfo<fieldId>::offset});\
	}\
	_regist<fieldId - 1>();\
}\
\
template<>\
static void _regist<0>()\
{\
	string runtimeFieldName=GetFieldInfo<0>::getName();\
    string staticFieldTypeName=typeid(GetFieldInfo<0>::type).name();\
    theTypeLogger.map_staticFieldTypeName[getStaticTypeName()].insert({runtimeFieldName,staticFieldTypeName});\
	theTypeLogger.map_offset[getStaticTypeName()].insert({runtimeFieldName,GetFieldInfo<0>::offset});\
}\
\
static void regist()\
{\
	_regist<numberOfFields - 1>();\
    theTypeLogger.map_staticAndRuntimeName.insert({getStaticTypeName(),getRuntimeName()});\
}


#define __property(typeName,fieldName,runtimeName) typeName fieldName;template<>class GetFieldInfo<LINE_TO_FIELD_ID>{public:\
	using type = decltype(fieldName);\
	enum{ size = sizeof(typeName) };enum{ offset = GetFieldInfo<LINE_TO_FIELD_ID-1>::offset + GetFieldInfo<LINE_TO_FIELD_ID-1>::size }; \
	static string getName(){ return #runtimeName; }\
};

#define __firstProperty(typeName,fieldName,runtimeName) typeName fieldName;template<>class GetFieldInfo<0>{public:\
	using type = decltype(fieldName);\
	enum{ size = sizeof(typeName) };enum{ offset = 0 }; \
	static string getName(){ return #runtimeName; }\
};
;

using VectorOfPointer = vector < void* >;
#define BEGIN_CLASS_DECLARE(staticName,runtimeName) class staticName{public:static string getStaticTypeName(){ return typeid(staticName).name(); }\
	static string getRuntimeName(){return #runtimeName;}
#define END_CLASS_DECLARE()  };

#define defineVarWithName(cls,varName) cls varName; strcpy((char*)&varName,#varName);


#define BEGIN_FUNCTIONS_DECLARE enum{functionsDeclarationBeginningLine=__LINE__};
#define THIS_FUNC_ID  (__LINE__-functionsDeclarationBeginningLine-1)
#define END_FUNCTIONS_DECLARE   enum{howManyFunctions_static=THIS_FUNC_ID};

class AIConceptNode;
typedef void*(*__uniFP)(vector<AIConceptNode*>);

template<int id>
struct GetFuncById
{
	using type = void;
	static __uniFP uniFuncPtr() { return NULL; }
	static const bool isAuto = true;
	static string getName() { return ""; }
	static string getDeclaration() { return ""; }
};

#define __function_auto(returnType,name,...) returnType name(__VA_ARGS__);\
			void* uni_##name(vector<AIConceptNode*>);\
			template<>struct GetFuncById<THIS_FUNC_ID> { \
				using type = decltype(name); \
				static __uniFP uniFuncPtr() { return &uni_##name;}\
				static const bool isAuto = true;\
				static string getName(){return #name;}\
				static string getDeclaration(){return #__VA_ARGS__;}\
			};
#define __function __function_auto
//#define __function(returnType,name,...) returnType name(__VA_ARGS__);\
//			void* uni_##name(vector<AIConceptNode*>);\
//			template<>struct GetFuncById<THIS_FUNC_ID> { \
//				using type = decltype(name); \
//				static __uniFP uniFuncPtr() { return &uni_##name;}\
//				static const bool isAuto = false;\
//				static string getName(){return #name;}\
//			};

/*4.静态反射机制，在编译期获知一个函数接收什么类型的参数*/
template<typename T>
struct function_traits;
// R为返回类型
// ...Args 为输入参数类型，个数不限
template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
	static const size_t nargs = sizeof...(Args);
	// 返回类型
	typedef R result_type;

	// 输入参数类型,i为从0开始的参数类型索引
	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
};


#define _getReturnTypeOf(ty)  function_traits<std::function<ty>>::result_type
#define _getParaTypeAt(ty,index) function_traits<std::function<ty>>::arg<index>::type
#define _getNumOfArgs(ty) function_traits<std::function<ty>>::nargs
//查询f函数的第index个参数（从0开始计数）是什么类型的
#define getParaTypeAt(f,index) function_traits<std::function<decltype(f)>>::arg<index>::type
//查询函数f总共接收多少个参数
#define getNumOfArgs(f) function_traits<std::function<decltype(f)>>::nargs
//将函数f的第index个参数由通用指针型(void*)转化为特定类型，如果不能在编译期转型，那就在运行时转化
#define castArgAt(f,index) cast<getParaTypeAt(f,index)>(args[index])
//如果f函数的第index个参数要求接收的是变量具体内容，而传入的是该变量的指针，则需对其解引用
#define derefArgAt(f,index) *(getParaTypeAt(f,index)*)(args[index])
//声明一个函数接受的每个参数分别代表什么意义（参数名）
#define declareParaNames(f,...) struct Function_##f##{enum{##__VA_ARGS__##,paraCount};};

#define __tyNameOf(ty) typeid(ty).name()

#define theFuncLogger TheFuncLogger::getInstance()
class TheFuncLogger
{
	TheFuncLogger() {}
	TheFuncLogger(TheFuncLogger&) = delete;
public:
	static TheFuncLogger& getInstance()
	{
		static TheFuncLogger obj;
		return obj;
	}
	vector<string> paraTypesOfAllFuncs[250];
	string names[250];
	__uniFP pointers[250];
	vector<string> paraNamesForVerbs[250];
};

template<typename FT, int index>
class FuncRegister
{
public:
	static void regist_common()
	{
		theFuncLogger.names[index] = GetFuncById<index>::getName();
		theFuncLogger.pointers[index] = GetFuncById<index>::uniFuncPtr();
		vector<string> strArr = split(GetFuncById<index>::getDeclaration(), ",");
		for (string typeAndParaName : strArr)
		{
			typeAndParaName = trim(typeAndParaName);
			vector<string> temp = split(typeAndParaName, " ");
			string paraName = temp[temp.size() - 1];
			theFuncLogger.paraNamesForVerbs[index].push_back(paraName);
		}
	};

	template<int paraNum>
	static void regist()
	{
		regist_common();
	}

	template<>
	static void regist<1>()
	{
		regist_common();
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 0)));
	}

	template<>
	static void regist<2>()
	{
		regist_common();
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 0)));
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 1)));
	}

	template<>
	static void regist<3>()
	{
		regist_common();
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 0)));
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 1)));
		theFuncLogger.paraTypesOfAllFuncs[index].push_back(__tyNameOf(_getParaTypeAt(FT, 2)));
	}
};
