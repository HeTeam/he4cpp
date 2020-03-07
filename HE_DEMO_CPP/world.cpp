#include"stdafx.h"
#include"world.h"
#include"AIHeader.h"
#include"AINode.h"
#include"global.h"

template<typename FT>
class UniFunctionGenerator
{
public:
	template<int paraNum>
	static void* howToDo(FT func, vector<AIConceptNode*> args) { return (void*)false; }

	template<>
	static void* howToDo<0>(FT func, vector<AIConceptNode*> args) { return (void*)func(); }

	template<>
	static void* howToDo<1>(FT func, vector<AIConceptNode*> args)
	{
		return (void*)func(*(decay<_getParaTypeAt(FT, 0)>::type*)args[0]->ptr);
	}

	template<>
	static void* howToDo<2>(FT func, vector<AIConceptNode*> args)
	{
		return (void*)func(*(decay<_getParaTypeAt(FT, 0)>::type*)args[0]->ptr,
			*(decay<_getParaTypeAt(FT, 1)>::type*)args[1]->ptr);
	}

	template<>
	static void* howToDo<3>(FT func, vector<AIConceptNode*> args)
	{
		return (void*)func(*(decay<_getParaTypeAt(FT, 0)>::type*)args[0]->ptr,
			*(decay<_getParaTypeAt(FT, 1)>::type*)args[1]->ptr,
			*(decay<_getParaTypeAt(FT, 2)>::type*)args[2]->ptr);
	}
};

#define DECLARE_UNI_FUNC(funcName) void* uni_##funcName(vector<AIConceptNode*> args){\
						using funcTy=decltype(funcName);\
						return UniFunctionGenerator<funcTy>::howToDo<_getNumOfArgs(funcTy)>(funcName,args);}


bool 充电(Phone& phone)
{
	if (phone.electricPercent < 96)
		phone.electricPercent += 4;
	else
		phone.electricPercent = 100;
	return true;
}

bool 通电(Kettle& kettle)
{
	if (kettle.content.empty())
		return false;
	auto where_coldWater = stdFind(kettle.content, theColdWater());
	if (kettle.content.end() != where_coldWater)
		kettle.content[where_coldWater - kettle.content.begin()] = theHotWater();
	return true;
}

bool 放入杯中(Cup& cup, Solute& coffeeSolute)
{
	pushBackIfNew(cup.content, &coffeeSolute);
	return true;
}

bool 搅拌(Cup& cup)
{
	if (stdFindExist(cup.content, theHotWater()) && cup.content.size() >= 2)
	{
		cup.content = { theHotCoffee() };
	}
	else if (stdFindExist(cup.content, theColdWater()) && cup.content.size() >= 2)
	{
		cup.content = { theColdCoffee() };
	}
	return true;
}

bool 获取热水(Cup& dest, void* src, TypeOfWaterSource typeOfSource)
{
	if (Cup_WaterSource == typeOfSource)
	{
		Cup* src_cup = (Cup*)src;
		if (stdFindExist(src_cup->content, theHotWater()))
		{
			pushBackIfNew(dest.content, theHotWater());
			return true;
		}
		else
			return false;
	}
	if (Kettle_WaterSource == typeOfSource)
	{
		Kettle* src_kettle = (Kettle*)src;
		if (stdFindExist(src_kettle->content, theHotWater()))
		{
			pushBackIfNew(dest.content, theHotWater());
			return true;
		}
		else
			return false;
	}
	if (DrinkingFountain_WaterSource == typeOfSource)
	{
		pushBackIfNew(dest.content, theHotWater());
		return true;
	}

	return false;
}

bool 获取冷水(Cup& dest, void* src, TypeOfWaterSource typeOfSource)
{
	if (Cup_WaterSource == typeOfSource)
	{
		Cup* src_cup = (Cup*)src;
		if (!stdFindExist(src_cup->content, theHotWater()))//假定：热水+冷水==热水
		{
			pushBackIfNew(dest.content, theColdWater());
			return true;
		}
		else
			return true;
	}
	if (Kettle_WaterSource == typeOfSource)
	{
		Kettle* src_kettle = (Kettle*)src;
		if (stdFindExist(src_kettle->content, theColdWater()))
		{
			pushBackIfNew(dest.content, theColdWater());
			return true;
		}
		else
			return false;
	}
	if (DrinkingFountain_WaterSource == typeOfSource)
	{
		pushBackIfNew(dest.content, theColdWater());
		return true;
	}

	return false;
}

bool 乘坐公交(Bus& bus, Station& dest)
{
	Station* myLocation = (Station*)__self()->location;
	if (stdFindExist(bus.stations, myLocation)&&stdFindExist(bus.stations,&dest))
	{
		__self()->location = &dest;
		return true;
	}
	return false;
}

bool 乘坐地铁(Subway& subway)
{
	Station* myLocation = (Station*)__self()->location;
	if (stdFindExist(subway.stations, myLocation))
	{
		__self()->location = &subway;
		return true;
	}
	return false;
}

bool 下车并出站(Station& station)
{
	vector<AIConceptNode*> _allSubways = theGlobalArea.findNounsByClass(concept_of(Subway));
	vector<void*> allSubways;
	for (auto each : _allSubways)
		allSubways.push_back((Subway*)each->ptr);
	if (stdFindExist(allSubways, __self()->location))//首先确保我现在是在地铁上
	{
		Subway* currentSubway = (Subway*)__self()->location;
		if (stdFindExist(currentSubway->stations, &station))//这辆地铁是否经过目的地
		{
			__self()->location = &station;//下车
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool 放进复印机(Copier& copier, Paper& paper)
{
	copier.papersToBePrinted.insert(copier.papersToBePrinted.begin(), &paper);
	return true;
}

bool 设置份数并复印(Copier& copier, int num)
{
	copier.num = num;
	for (int i = 0; i < copier.num; i++)
	{
		Paper* newPaper = new Paper;
		if (copier.papersToBePrinted.size() > 0)
		{
			newPaper->textContent = ((Paper*)(copier.papersToBePrinted[0]))->textContent;
			AIConceptNode& node = createNewRefNode(*newPaper);
			node.name = ((Paper*)(copier.papersToBePrinted[0]))->name + "_复印件" + to_string(i + 1);
			theGlobalArea.nouns.push_back(&node);
		}
		copier.papersPrinted.push_back(newPaper);
	}
	copier.recentUsedTime = getCurrentTime();
	return true;
}

bool 取出复印件(Copier& copier)
{
	if (copier.papersPrinted.empty())
		return false;
	
	copier.papersPrinted.clear();
	return true;
}

bool 取出复印原件(Copier& copier,Paper& paper)
{
	auto where_paper = stdFind(copier.papersToBePrinted, &paper);
	if (copier.papersToBePrinted.end() == where_paper)
		return false;
	else
	{
		copier.papersToBePrinted.erase(where_paper);
		return true;
	}
}

bool 转账(Person& dest, Person& from, double value)
{
	dest.money += value;
	from.money -= value;
	return true;
}

bool 缓存视频(Phone& phone, string videoName)
{
	phone.restStorage -= (rand() % 90 + 8)*1024;//消耗8——97MB
	phone.storageUsageRate = 
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

bool 下载音乐(Phone& phone, string musicName)
{
	phone.restStorage -= (rand() % 9 + 2)*1024;//消耗2——10MB
	phone.storageUsageRate =
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

bool 保存网络图片(Phone& phone, string pictureName)
{
	phone.restStorage -= (rand() % 900 +100);//消耗0.1——1MB
	phone.storageUsageRate =
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

DECLARE_UNI_FUNC(充电);
DECLARE_UNI_FUNC(通电);
DECLARE_UNI_FUNC(搅拌);
DECLARE_UNI_FUNC(放入杯中);
DECLARE_UNI_FUNC(乘坐公交);
DECLARE_UNI_FUNC(乘坐地铁);
DECLARE_UNI_FUNC(下车并出站);
DECLARE_UNI_FUNC(放进复印机);
DECLARE_UNI_FUNC(设置份数并复印);
DECLARE_UNI_FUNC(取出复印件);
DECLARE_UNI_FUNC(取出复印原件);
DECLARE_UNI_FUNC(转账);
DECLARE_UNI_FUNC(缓存视频);
DECLARE_UNI_FUNC(下载音乐);
DECLARE_UNI_FUNC(保存网络图片);

void* uni_获取冷水(vector<AIConceptNode*> args) 
{ 
	Cup& dest = *(Cup*)(args[0]->ptr);
	void* src = args[1]->ptr;
	int typeOfWaterSource = Unknown_WaterSource;
	
	if (args[1]->type == runtime_of(DrinkingFountain))
		typeOfWaterSource = DrinkingFountain_WaterSource;
	if (args[1]->type == runtime_of(Kettle))
		typeOfWaterSource = Kettle_WaterSource;
	if (args[1]->type == runtime_of(Cup))
		typeOfWaterSource = Cup_WaterSource;

	return (void*)获取冷水(dest, src, typeOfWaterSource);
}
void* uni_获取热水(vector<AIConceptNode*> args) 
{ 
	Cup& dest = *(Cup*)(args[0]->ptr);
	void* src = args[1]->ptr;
	int typeOfWaterSource = Unknown_WaterSource;

	if (args[1]->type == runtime_of(DrinkingFountain))
		typeOfWaterSource = DrinkingFountain_WaterSource;
	if (args[1]->type == runtime_of(Kettle))
		typeOfWaterSource = Kettle_WaterSource;
	if (args[1]->type == runtime_of(Cup))
		typeOfWaterSource = Cup_WaterSource;

	return (void*)获取热水(dest, src, typeOfWaterSource);
}

Paper::Paper()
{
	for (int i = 0; i < 3; i++)
	{
		this->textContent += to_string((rand() % 100+1) * 39);
		this->textContent += randString();
	}
}

Person::Person()
{
	age = rand() % 10 + 20;
	money = (rand() % 10000)/7.1 + 2333.3;
	gender = ((rand() % 3!=0)?true:false);
	height = 165 + (rand() % 120+2) / 9.1;
}

Phone::Phone()
{
	this->electricPercent = rand() % 30 + 30;
	this->memoryUsageRate = double(rand() % 50) / 100.0 + 0.2;
	//this->system = (OperatingSystem*)randomChoose(
	//	theGlobalArea.findNounsByClass(concept_of(OperatingSystem)))->ptr;
	this->totalMemory = pow(2, rand() % 3) * 1024 * 1024;//1——4G
	this->totalStorage = pow(2, rand() % 4) * 1024 * 1024;//1——8G
	this->storageUsageRate= double(rand() % 40) / 100.0 + 0.25;
	this->restStorage = (1.0 - storageUsageRate)*totalStorage;
}

//DrinkingFountain::DrinkingFountain()
//{
//	this->content = { theHotWater(),theColdWater() };//默认饮水机里面有冷水和热水
//}