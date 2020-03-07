#pragma once

#include"reflect.h"
#pragma warning(disable:4103)

#pragma pack(1)
BEGIN_CLASS_DECLARE(Bus, ������)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer, stations, ;��վ)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Subway, ����)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer, stations, ;��վ)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Station, ��վ)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer, busesAndSubways, �����ĵ����Լ�������)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Person, ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(double, money, �˻����)
		__property(int, age, ����)
		__property(bool, gender, �Ա�)
		__property(double, height, ���)
		__property(void*, location, ��ǰλ��)//�����ǳ�վ��Ҳ�������ڹ�����������
	END_FIELD_DECLARE;
	Person();
END_CLASS_DECLARE();

//#pragma pack(1)
//BEGIN_CLASS_DECLARE(OperatingSystem, ����ϵͳ)
//	BEGIN_FIELD_DECLARE
//		__firstProperty(string, name, ����)
//		__property(int, releaseDate, �������)
//		__property(string, developer, ������)//�����߿����ǹ�˾��Ҳ�����Ǹ���
//		__property(string, usage_type, ��;)
//	END_FIELD_DECLARE;
//END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Phone, �ֻ�)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(int, electricPercent, ����)
		__property(double, memoryUsageRate, �ڴ�ռ����)
		__property(int, totalMemory, �ڴ��С)
		__property(int, restStorage, ʣ��洢�ռ�)
		__property(double,storageUsageRate,�洢�ռ�ʹ����)
		__property(int, totalStorage, �洢�ռ��С);
	END_FIELD_DECLARE;
	Phone();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Cup,����)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer,content,����)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Kettle, ��ˮ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer,content,����)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(DrinkingFountain, ��ˮ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer, content, ����)
	END_FIELD_DECLARE;
	//DrinkingFountain();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Solute, ���ȳ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Copier, ��ӡ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(VectorOfPointer,papersToBePrinted,��ӡԭ��)
		__property(VectorOfPointer, papersPrinted, ��ӡ��)
		__property(int,num,��ӡ����)
		__property(string,recentUsedTime,���һ��ʹ��ʱ��)
	END_FIELD_DECLARE;
	Copier() { num = 0; }
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Paper, ֽ��)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
		__property(string,textContent,�ı�����)
	END_FIELD_DECLARE;
	Paper();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Water, ˮ)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Coffee, ������Һ)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, ����)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#define TypeOfWaterSource int
#define Unknown_WaterSource   0
#define Cup_WaterSource   1
#define DrinkingFountain_WaterSource 2
#define Kettle_WaterSource 3

#define test_printParaTypes(theFuncType,index) FuncRegister<theFuncType,index>::print<_getNumOfArgs(theFuncType)>()

Water* theHotWater();
Water* theColdWater();
Coffee* theHotCoffee();
Coffee* theColdCoffee();
Person* __self();

BEGIN_FUNCTIONS_DECLARE
__function_auto(bool, ���, Phone& �ֻ�);
__function_auto(bool,��ȡ��ˮ,Cup& Ŀ�걭��, void* ��Դ, int ˮԴ����);
__function_auto(bool,��ȡ��ˮ, Cup& Ŀ�걭��, void* ��Դ, int ˮԴ����);
__function_auto(bool,ͨ��,Kettle& ��ˮ��);
__function_auto(bool,����,Cup& ����);
__function_auto(bool,���뱭��,Cup& Ŀ�걭��,Solute& ���);
__function_auto(bool, ��������, Bus& ������·, Station& Ŀ�ĵ�);
__function_auto(bool, ��������, Subway& ������·);
__function_auto(bool, �³�����վ, Station& ����վ);
__function_auto(bool, �Ž���ӡ��, Copier& ��ӡ��, Paper& Ҫ��ӡ��ֽ��);
__function_auto(bool, ���÷�������ӡ, Copier& ��ӡ��, int ����);
__function_auto(bool, ȡ����ӡ��, Copier& ��ӡ��);
__function_auto(bool, ȡ����ӡԭ��, Copier& ��ӡ��, Paper& Ҫȡ�ߵ�ֽ��);
__function_auto(bool, ת��, Person& �տ, Person& ת����, double ���);
__function_auto(bool, ��������, Phone& �ֻ�, string ����);
__function_auto(bool, ��������ͼƬ, Phone& �ֻ�, string ͼƬ��);
__function_auto(bool, ������Ƶ, Phone& �ֻ�, string ��Ƶ��ַ);
END_FUNCTIONS_DECLARE

//__function_auto(bool, ��װϵͳ, Phone&, OperatingSystem&);
//__function_auto(ReturnTrue, ��������, Phone&, string fileName);
//__function_auto(ReturnTrue, ������Ƶ, Phone&, string fileName);
//__function_auto(ReturnTrue, �����ĵ�, Phone&, string fileName);

