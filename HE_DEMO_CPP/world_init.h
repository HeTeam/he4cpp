#pragma once
#include"reflect.h"

void setRuntimeNameAndJsonGenerator()
{
	TypeLogger& tyLog = TypeLogger::getInstance();
	int index_basic;
	string staticName;
	REGIST_BASIC_TYPE(bool, "布尔值");
	REGIST_BASIC_TYPE(int, "整数");
	REGIST_BASIC_TYPE(double, "小数");
	REGIST_BASIC_TYPE(void*, "指针");
	REGIST_BASIC_TYPE(vector<void*>, "指针数组");
	REGIST_BASIC_TYPE(string, "字符串");
	REGIST_BASIC_TYPE(Class, "类型");

	for (unsigned idx = 0; idx<Classes::howManyClasses_onlyStatic; idx++)
	{
		string staticTypeName = theClasses.staticNames[idx];//staticNameAndOffsetMapForEachType.first;
		pair<string/*编译期类名*/, map<string/*动态属性名*/, int>> staticNameAndOffsetMapForEachType;
		staticNameAndOffsetMapForEachType = { staticTypeName, tyLog.map_offset[staticTypeName] };
		map<string, int>& offsetMapForEachType = staticNameAndOffsetMapForEachType.second;
		map<string, string>& fieldNamesForEachType = tyLog.map_staticFieldTypeName[staticTypeName];
		int index = theClasses.findByStaticTypeName(staticTypeName);
		if (-1 == index)
		{
			DEBUG_MSG("未注册类%s", staticTypeName.c_str());
			continue;
		}
		Class* thisClass = &theClasses.types[index];
		thisClass->setOffsetsAndFieldType(offsetMapForEachType, fieldNamesForEachType);
		thisClass->staticName = staticTypeName;
		thisClass->runtimeName = tyLog.map_staticAndRuntimeName[staticTypeName];
		AIConceptNode& conceptNodeOfThisDataType = createNewRefNode(*thisClass);
		theGlobalArea.nouns.push_back(&conceptNodeOfThisDataType);
	}

	AIConceptNode* waterContainer = new AIConceptNode();
	waterContainer->name = "盛水的容器";
	waterContainer->insertConcreteNode(concept_of(Cup));
	waterContainer->insertConcreteNode(concept_of(DrinkingFountain));
	waterContainer->insertConcreteNode(concept_of(Kettle));
	theGlobalArea.nouns.push_back(waterContainer);
}

void registAllFunctions()
{
	_registFunc<howManyFunctions_static - 1>();
	TheFuncLogger& logger = TheFuncLogger::getInstance();
	for (int i = 0; i < howManyFunctions_static; i++)
	{
		auto newVerbNode = new AITemporalOrderNode;
		newVerbNode->name = logger.names[i];
		newVerbNode->funcPtr = logger.pointers[i];
		for (string& nameOfEachParaType : logger.paraTypesOfAllFuncs[i])
		{
			int index = theClasses.findByStaticTypeName(nameOfEachParaType);
			if (-1 == index)
			{
				DEBUG_MSG("未注册类%s", nameOfEachParaType.c_str());
				continue;
			}
			Class* thisClass = &theClasses.types[index];
			AIConceptNode* clsNode = theGlobalArea.findNounByPtrAddress(thisClass);
			if (NULL == clsNode)
			{
				DEBUG_MSG("类%s没有对应的概念结点", nameOfEachParaType.c_str());
				continue;
			}
			newVerbNode->paraTypes.push_back(clsNode);
		}
		newVerbNode->paraNames = logger.paraNamesForVerbs[i];
		theGlobalArea.verbs.push_back(newVerbNode);
	}

	auto waterContainer = theGlobalArea.findNounByName("盛水的容器");
	verb_of(获取热水)->paraTypes = verb_of(获取冷水)->paraTypes
		= { concept_of(Cup),waterContainer };
	verb_of(获取热水)->paraNames = verb_of(获取冷水)->paraNames = { "目标杯子","水源" };

	auto download = new AITemporalOrderNode;
	download->name = "下载";
	download->paraTypes = verb_of(缓存视频)->paraTypes;
	download->paraNames = { "手机","要下载的内容" };
	download->insertConcreteNode(verb_of(缓存视频));
	download->insertConcreteNode(verb_of(下载音乐));
	download->insertConcreteNode(verb_of(保存网络图片));

	theGlobalArea.verbs.push_back(download);
}

template<typename T>
void setNamesAndCreateConceptNodes(string str)
{
	void* first = theWorld.getAddressOfFirst<T>();
	vector<string> strArr = split(str, ",");
	int num = strArr.size();
	for (int i = 0; i < num; i++)
	{
		int step = sizeof(T);
		string thisName = trim(strArr[i]);
		void* current = (char*)first + step*i;
		*(string*)current = thisName;
		AIConceptNode& newNode = createNewRefNode(*(T*)current);
		theGlobalArea.nouns.push_back(&newNode);
	}
}


class TheWorld
{
	TheWorld() {}
public:
	TheWorld(TheWorld&) = delete;
	static TheWorld& getInstance()
	{
		static TheWorld obj;
		return obj;
	}

	template<typename T>
	string getNamesString() { return ""; }

	template<typename T>
	void* getAddressOfFirst() { return NULL; }

	template<int index>
	void registObjectsOfType()
	{
		using T = Classes::GetTypeById<index>::_ty;
		REGIST_OBJECTS(T);
		registObjectsOfType<index - 1>();
	}

	template<>
	void registObjectsOfType<Classes::start_jsonSupportedClassID>()
	{
		const int index = Classes::start_jsonSupportedClassID;
		using T = Classes::GetTypeById<index>::_ty;
		REGIST_OBJECTS(T);
	}

	void registAllObjects()
	{
		registObjectsOfType<Classes::end_jsonSupportedClassID>();
	}

	DECLARE_OBJECTS(Station, 善贤, 拱宸桥东, 大运河, 小石桥, 杭印街通益路口, 严家桥, 短浜, 长浜北, 杭行路石祥路口, \
	杭行路祥庆街口, 祥园路杭行路口, 祥园路祥兴路口, 祥园桥, 严家斗, 徐家斗, 金家渡路古墩路口, 和睦, 彭埠, 火车东站, 新风, 新塘, 景芳, 江锦路, \
		城星路, 近江, 甬江路, 南星桥, 复兴路, 朝阳, 曹家桥, 潘水, 人民路, 杭发厂, 人民广场, \
		建设一路, 建设三路, 振宁路, 飞虹路, 盈丰路, 钱江世纪城, 钱江路, 庆春广场, 庆菱路, 建国北路, 中河北路, 凤起路, 武林门, 沈塘桥, \
		学院路, 古翠路, 丰潭路, 文新, 虾龙圩, 三墩, 墩祥街, 白洋, 杜甫村, 良渚, 水澄桥, 联庄, 中医药大学, 杨家墩, 浦沿, 萍水街, \
		三坝, 池华街公交站, 金家渡, 金家渡路西口, 水起苑, 古墩路三墩路口, 古墩路石祥西路口, 机动车辆管理所, \
		政新花园, 骆家庄, 益乐新村, 宋江村, 文一路学院路口, 浙江工商大学, 花园新村, 教工路花园亭, \
		八字桥, 武林广场北, 莫衙营, 潮鸣寺巷, 菜市桥, 新城隧道东口, 解放路钱江路口, 市民中心, \
		浙大紫金港, 蒋村, 五常, 永福, 杭师大仓前, 良睦路);
	DECLARE_OBJECTS(Subway, 地铁2号线, 地铁4号线, 地铁5号线);
	DECLARE_OBJECTS(Bus, 公交98BM路, 公交B2高峰线);
	DECLARE_OBJECTS(Water, 热水, 冷水);
	DECLARE_OBJECTS(Coffee, 一杯热咖啡, 一杯冷咖啡);
	DECLARE_OBJECTS(Kettle, 烧水壶1, 烧水壶2, 烧水壶3, 烧水壶4, 烧水壶5);
	DECLARE_OBJECTS(Person, 我, 李明, 张三, 李四, 王五);
	DECLARE_OBJECTS(Phone, 我的Oppo, 我的华为, 贾晓刚的iPhoneX, 赵先生的iPhone, 贾晓刚的WinPhone);
	DECLARE_OBJECTS(Cup, 杯子1, 杯子2, 杯子3, 杯子4, 杯子5, 杯子6, 杯子7);
	DECLARE_OBJECTS(Solute, 雀巢咖啡冲剂, 麦斯威尔咖啡冲剂, 福爵咖啡冲剂, 益昌老街速溶白咖啡冲剂);
	DECLARE_OBJECTS(DrinkingFountain, 饮水机1, 饮水机2, 饮水机3);
	DECLARE_OBJECTS(Paper, 考试卷, 公司合同, 我的毕业证书, 租房协议);
	DECLARE_OBJECTS(Copier, 复印机1, 复印机2, 复印机3, 复印机4, 复印机5);

	bool initialized = false;
	void init()
	{
		if (initialized)
			return;

		vector<AIConceptNode*>& theNouns = theGlobalArea.nouns;

		registAllObjects();

		vector<AIConceptNode*> people = theGlobalArea.findNounsByClass(concept_of(Person));
		for (AIConceptNode* each : people)
		{
			Person* person = (Person*)each->ptr;
			person->location = randomChoose(theGlobalArea.findNounsByClass(concept_of(Station)))->ptr;
		}

		地铁2号线.stations = { &朝阳,&曹家桥,&潘水,&人民路,&杭发厂,&人民广场,&建设一路,&建设三路,&振宁路,&飞虹路,&盈丰路,&钱江世纪城,\
			&钱江路,&庆春广场,&庆菱路,&建国北路,&中河北路,&凤起路,&武林门,&沈塘桥,&学院路,&古翠路,\
			&丰潭路,&文新,&三坝,&虾龙圩,&三墩,&墩祥街,&金家渡,&白洋,&杜甫村,&良渚 };
		地铁4号线.stations = { &彭埠,&火车东站,&新风,&新塘,&景芳,&钱江路,&江锦路,&市民中心,&城星路,&近江,&甬江路,&南星桥,&复兴路,&水澄桥,&联庄,&中医药大学,&杨家墩,&浦沿 };
		地铁5号线.stations = { &善贤,&拱宸桥东,&大运河,&和睦,&萍水街,&三坝,&浙大紫金港,&蒋村,&五常,&永福,&杭师大仓前,&良睦路 };
		公交B2高峰线.stations = { &池华街公交站,&金家渡,&金家渡路西口,&水起苑,&古墩路三墩路口,&古墩路石祥西路口,&机动车辆管理所,&三坝,&政新花园,&骆家庄,&益乐新村,&宋江村,&文一路学院路口,&浙江工商大学,&花园新村,&教工路花园亭,&八字桥,&武林广场北,&莫衙营,&潮鸣寺巷,&菜市桥,&新城隧道东口,&解放路钱江路口,&市民中心 };
		公交98BM路.stations = { &大运河,&小石桥,&杭印街通益路口,&严家桥,&短浜,&长浜北,&杭行路石祥路口,&杭行路祥庆街口,&祥园路杭行路口,&祥园路祥兴路口,&祥园桥,&严家斗,&徐家斗,&金家渡路古墩路口,&墩祥街 };

		for (AIConceptNode* stationNode : theGlobalArea.findNounsByClass(concept_of(Station)))
		{
			auto station = (Station*)stationNode->ptr;
			for (AIConceptNode* subwayNode : theGlobalArea.findNounsByClass(concept_of(Subway)))
			{
				auto subway = (Subway*)subwayNode->ptr;
				if (stdFindExist(subway->stations, station))
					station->busesAndSubways.push_back(subway);
			}
			for (AIConceptNode* busNode : theGlobalArea.findNounsByClass(concept_of(Bus)))
			{
				auto bus = (Bus*)busNode->ptr;
				if (stdFindExist(bus->stations, station))
					station->busesAndSubways.push_back(bus);
			}
		}

		烧水壶2.content = { &热水 };
		杯子3.content = { &热水 };
		我.location = &三坝;
		for (auto drinkingFountainNode : theGlobalArea.findNounsByClass(concept_of(DrinkingFountain)))
		{
			auto drinkingFountain = (DrinkingFountain*)drinkingFountainNode->ptr;
			drinkingFountain->content = { &热水,&冷水 };
		}
	}
};

Water* theHotWater() { return &theWorld.热水; }
Water* theColdWater() { return &theWorld.冷水; }
Coffee* theHotCoffee() { return &theWorld.一杯热咖啡; }
Coffee* theColdCoffee() { return &theWorld.一杯冷咖啡; }
Person* __self() { return &theWorld.我; }