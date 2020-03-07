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


bool ���(Phone& phone)
{
	if (phone.electricPercent < 96)
		phone.electricPercent += 4;
	else
		phone.electricPercent = 100;
	return true;
}

bool ͨ��(Kettle& kettle)
{
	if (kettle.content.empty())
		return false;
	auto where_coldWater = stdFind(kettle.content, theColdWater());
	if (kettle.content.end() != where_coldWater)
		kettle.content[where_coldWater - kettle.content.begin()] = theHotWater();
	return true;
}

bool ���뱭��(Cup& cup, Solute& coffeeSolute)
{
	pushBackIfNew(cup.content, &coffeeSolute);
	return true;
}

bool ����(Cup& cup)
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

bool ��ȡ��ˮ(Cup& dest, void* src, TypeOfWaterSource typeOfSource)
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

bool ��ȡ��ˮ(Cup& dest, void* src, TypeOfWaterSource typeOfSource)
{
	if (Cup_WaterSource == typeOfSource)
	{
		Cup* src_cup = (Cup*)src;
		if (!stdFindExist(src_cup->content, theHotWater()))//�ٶ�����ˮ+��ˮ==��ˮ
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

bool ��������(Bus& bus, Station& dest)
{
	Station* myLocation = (Station*)__self()->location;
	if (stdFindExist(bus.stations, myLocation)&&stdFindExist(bus.stations,&dest))
	{
		__self()->location = &dest;
		return true;
	}
	return false;
}

bool ��������(Subway& subway)
{
	Station* myLocation = (Station*)__self()->location;
	if (stdFindExist(subway.stations, myLocation))
	{
		__self()->location = &subway;
		return true;
	}
	return false;
}

bool �³�����վ(Station& station)
{
	vector<AIConceptNode*> _allSubways = theGlobalArea.findNounsByClass(concept_of(Subway));
	vector<void*> allSubways;
	for (auto each : _allSubways)
		allSubways.push_back((Subway*)each->ptr);
	if (stdFindExist(allSubways, __self()->location))//����ȷ�����������ڵ�����
	{
		Subway* currentSubway = (Subway*)__self()->location;
		if (stdFindExist(currentSubway->stations, &station))//���������Ƿ񾭹�Ŀ�ĵ�
		{
			__self()->location = &station;//�³�
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool �Ž���ӡ��(Copier& copier, Paper& paper)
{
	copier.papersToBePrinted.insert(copier.papersToBePrinted.begin(), &paper);
	return true;
}

bool ���÷�������ӡ(Copier& copier, int num)
{
	copier.num = num;
	for (int i = 0; i < copier.num; i++)
	{
		Paper* newPaper = new Paper;
		if (copier.papersToBePrinted.size() > 0)
		{
			newPaper->textContent = ((Paper*)(copier.papersToBePrinted[0]))->textContent;
			AIConceptNode& node = createNewRefNode(*newPaper);
			node.name = ((Paper*)(copier.papersToBePrinted[0]))->name + "_��ӡ��" + to_string(i + 1);
			theGlobalArea.nouns.push_back(&node);
		}
		copier.papersPrinted.push_back(newPaper);
	}
	copier.recentUsedTime = getCurrentTime();
	return true;
}

bool ȡ����ӡ��(Copier& copier)
{
	if (copier.papersPrinted.empty())
		return false;
	
	copier.papersPrinted.clear();
	return true;
}

bool ȡ����ӡԭ��(Copier& copier,Paper& paper)
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

bool ת��(Person& dest, Person& from, double value)
{
	dest.money += value;
	from.money -= value;
	return true;
}

bool ������Ƶ(Phone& phone, string videoName)
{
	phone.restStorage -= (rand() % 90 + 8)*1024;//����8����97MB
	phone.storageUsageRate = 
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

bool ��������(Phone& phone, string musicName)
{
	phone.restStorage -= (rand() % 9 + 2)*1024;//����2����10MB
	phone.storageUsageRate =
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

bool ��������ͼƬ(Phone& phone, string pictureName)
{
	phone.restStorage -= (rand() % 900 +100);//����0.1����1MB
	phone.storageUsageRate =
		double(phone.totalStorage - phone.restStorage) / (double)phone.totalStorage;
	return true;
}

DECLARE_UNI_FUNC(���);
DECLARE_UNI_FUNC(ͨ��);
DECLARE_UNI_FUNC(����);
DECLARE_UNI_FUNC(���뱭��);
DECLARE_UNI_FUNC(��������);
DECLARE_UNI_FUNC(��������);
DECLARE_UNI_FUNC(�³�����վ);
DECLARE_UNI_FUNC(�Ž���ӡ��);
DECLARE_UNI_FUNC(���÷�������ӡ);
DECLARE_UNI_FUNC(ȡ����ӡ��);
DECLARE_UNI_FUNC(ȡ����ӡԭ��);
DECLARE_UNI_FUNC(ת��);
DECLARE_UNI_FUNC(������Ƶ);
DECLARE_UNI_FUNC(��������);
DECLARE_UNI_FUNC(��������ͼƬ);

void* uni_��ȡ��ˮ(vector<AIConceptNode*> args) 
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

	return (void*)��ȡ��ˮ(dest, src, typeOfWaterSource);
}
void* uni_��ȡ��ˮ(vector<AIConceptNode*> args) 
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

	return (void*)��ȡ��ˮ(dest, src, typeOfWaterSource);
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
	this->totalMemory = pow(2, rand() % 3) * 1024 * 1024;//1����4G
	this->totalStorage = pow(2, rand() % 4) * 1024 * 1024;//1����8G
	this->storageUsageRate= double(rand() % 40) / 100.0 + 0.25;
	this->restStorage = (1.0 - storageUsageRate)*totalStorage;
}

//DrinkingFountain::DrinkingFountain()
//{
//	this->content = { theHotWater(),theColdWater() };//Ĭ����ˮ����������ˮ����ˮ
//}