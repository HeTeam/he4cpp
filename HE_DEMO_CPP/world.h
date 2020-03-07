#pragma once

#include"reflect.h"
#pragma warning(disable:4103)

#pragma pack(1)
BEGIN_CLASS_DECLARE(Bus, 公交车)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer, stations, 途径站)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Subway, 地铁)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer, stations, 途径站)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Station, 车站)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer, busesAndSubways, 经过的地铁以及公交车)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Person, 人)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(double, money, 账户余额)
		__property(int, age, 年龄)
		__property(bool, gender, 性别)
		__property(double, height, 身高)
		__property(void*, location, 当前位置)//可以是车站，也可以是在公交、地铁上
	END_FIELD_DECLARE;
	Person();
END_CLASS_DECLARE();

//#pragma pack(1)
//BEGIN_CLASS_DECLARE(OperatingSystem, 操作系统)
//	BEGIN_FIELD_DECLARE
//		__firstProperty(string, name, 名称)
//		__property(int, releaseDate, 发行年份)
//		__property(string, developer, 开发者)//开发者可以是公司，也可以是个人
//		__property(string, usage_type, 用途)
//	END_FIELD_DECLARE;
//END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Phone, 手机)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(int, electricPercent, 电量)
		__property(double, memoryUsageRate, 内存占用率)
		__property(int, totalMemory, 内存大小)
		__property(int, restStorage, 剩余存储空间)
		__property(double,storageUsageRate,存储空间使用率)
		__property(int, totalStorage, 存储空间大小);
	END_FIELD_DECLARE;
	Phone();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Cup,杯子)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer,content,内容)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Kettle, 烧水壶)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer,content,内容)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(DrinkingFountain, 饮水机)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer, content, 内容)
	END_FIELD_DECLARE;
	//DrinkingFountain();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Solute, 咖啡冲剂)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Copier, 复印机)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(VectorOfPointer,papersToBePrinted,复印原件)
		__property(VectorOfPointer, papersPrinted, 复印件)
		__property(int,num,复印份数)
		__property(string,recentUsedTime,最近一次使用时间)
	END_FIELD_DECLARE;
	Copier() { num = 0; }
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Paper, 纸张)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
		__property(string,textContent,文本内容)
	END_FIELD_DECLARE;
	Paper();
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Water, 水)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
	END_FIELD_DECLARE;
END_CLASS_DECLARE();

#pragma pack(1)
BEGIN_CLASS_DECLARE(Coffee, 咖啡溶液)
	BEGIN_FIELD_DECLARE
		__firstProperty(string, name, 名称)
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
__function_auto(bool, 充电, Phone& 手机);
__function_auto(bool,获取冷水,Cup& 目标杯子, void* 来源, int 水源类型);
__function_auto(bool,获取热水, Cup& 目标杯子, void* 来源, int 水源类型);
__function_auto(bool,通电,Kettle& 烧水壶);
__function_auto(bool,搅拌,Cup& 杯子);
__function_auto(bool,放入杯中,Cup& 目标杯子,Solute& 冲剂);
__function_auto(bool, 乘坐公交, Bus& 公交线路, Station& 目的地);
__function_auto(bool, 乘坐地铁, Subway& 地铁线路);
__function_auto(bool, 下车并出站, Station& 地铁站);
__function_auto(bool, 放进复印机, Copier& 复印机, Paper& 要复印的纸张);
__function_auto(bool, 设置份数并复印, Copier& 复印机, int 份数);
__function_auto(bool, 取出复印件, Copier& 复印机);
__function_auto(bool, 取出复印原件, Copier& 复印机, Paper& 要取走的纸张);
__function_auto(bool, 转账, Person& 收款方, Person& 转出方, double 金额);
__function_auto(bool, 下载音乐, Phone& 手机, string 歌名);
__function_auto(bool, 保存网络图片, Phone& 手机, string 图片名);
__function_auto(bool, 缓存视频, Phone& 手机, string 视频地址);
END_FUNCTIONS_DECLARE

//__function_auto(bool, 安装系统, Phone&, OperatingSystem&);
//__function_auto(ReturnTrue, 下载音乐, Phone&, string fileName);
//__function_auto(ReturnTrue, 下载视频, Phone&, string fileName);
//__function_auto(ReturnTrue, 下载文档, Phone&, string fileName);

