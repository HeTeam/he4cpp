#pragma once
#include"reflect.h"

void setRuntimeNameAndJsonGenerator()
{
	TypeLogger& tyLog = TypeLogger::getInstance();
	int index_basic;
	string staticName;
	REGIST_BASIC_TYPE(bool, "����ֵ");
	REGIST_BASIC_TYPE(int, "����");
	REGIST_BASIC_TYPE(double, "С��");
	REGIST_BASIC_TYPE(void*, "ָ��");
	REGIST_BASIC_TYPE(vector<void*>, "ָ������");
	REGIST_BASIC_TYPE(string, "�ַ���");
	REGIST_BASIC_TYPE(Class, "����");

	for (unsigned idx = 0; idx<Classes::howManyClasses_onlyStatic; idx++)
	{
		string staticTypeName = theClasses.staticNames[idx];//staticNameAndOffsetMapForEachType.first;
		pair<string/*����������*/, map<string/*��̬������*/, int>> staticNameAndOffsetMapForEachType;
		staticNameAndOffsetMapForEachType = { staticTypeName, tyLog.map_offset[staticTypeName] };
		map<string, int>& offsetMapForEachType = staticNameAndOffsetMapForEachType.second;
		map<string, string>& fieldNamesForEachType = tyLog.map_staticFieldTypeName[staticTypeName];
		int index = theClasses.findByStaticTypeName(staticTypeName);
		if (-1 == index)
		{
			DEBUG_MSG("δע����%s", staticTypeName.c_str());
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
	waterContainer->name = "ʢˮ������";
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
				DEBUG_MSG("δע����%s", nameOfEachParaType.c_str());
				continue;
			}
			Class* thisClass = &theClasses.types[index];
			AIConceptNode* clsNode = theGlobalArea.findNounByPtrAddress(thisClass);
			if (NULL == clsNode)
			{
				DEBUG_MSG("��%sû�ж�Ӧ�ĸ�����", nameOfEachParaType.c_str());
				continue;
			}
			newVerbNode->paraTypes.push_back(clsNode);
		}
		newVerbNode->paraNames = logger.paraNamesForVerbs[i];
		theGlobalArea.verbs.push_back(newVerbNode);
	}

	auto waterContainer = theGlobalArea.findNounByName("ʢˮ������");
	verb_of(��ȡ��ˮ)->paraTypes = verb_of(��ȡ��ˮ)->paraTypes
		= { concept_of(Cup),waterContainer };
	verb_of(��ȡ��ˮ)->paraNames = verb_of(��ȡ��ˮ)->paraNames = { "Ŀ�걭��","ˮԴ" };

	auto download = new AITemporalOrderNode;
	download->name = "����";
	download->paraTypes = verb_of(������Ƶ)->paraTypes;
	download->paraNames = { "�ֻ�","Ҫ���ص�����" };
	download->insertConcreteNode(verb_of(������Ƶ));
	download->insertConcreteNode(verb_of(��������));
	download->insertConcreteNode(verb_of(��������ͼƬ));

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

	DECLARE_OBJECTS(Station, ����, ����Ŷ�, ���˺�, Сʯ��, ��ӡ��ͨ��·��, �ϼ���, ���, ��亱�, ����·ʯ��·��, \
	����·����ֿ�, ��԰·����·��, ��԰·����·��, ��԰��, �ϼҶ�, ��Ҷ�, ��Ҷ�·�Ŷ�·��, ����, ��, �𳵶�վ, �·�, ����, ����, ����·, \
		����·, ����, 𮽭·, ������, ����·, ����, �ܼ���, ��ˮ, ����·, ������, ����㳡, \
		����һ·, ������·, ����·, �ɺ�·, ӯ��·, Ǯ�����ͳ�, Ǯ��·, �촺�㳡, ����·, ������·, �кӱ�·, ����·, ������, ������, \
		ѧԺ·, �Ŵ�·, ��̶·, ����, Ϻ����, ����, �����, ����, �Ÿ���, ���, ˮ����, ��ׯ, ��ҽҩ��ѧ, ��Ҷ�, ����, Ƽˮ��, \
		����, �ػ��ֹ���վ, ��Ҷ�, ��Ҷ�·����, ˮ��Է, �Ŷ�·����·��, �Ŷ�·ʯ����·��, ��������������, \
		���»�԰, ���ׯ, �����´�, �ν���, ��һ·ѧԺ·��, �㽭���̴�ѧ, ��԰�´�, �̹�·��԰ͤ, \
		������, ���ֹ㳡��, Ī��Ӫ, ��������, ������, �³��������, ���·Ǯ��·��, ��������, \
		����Ͻ��, ����, �峣, ����, ��ʦ���ǰ, ����·);
	DECLARE_OBJECTS(Subway, ����2����, ����4����, ����5����);
	DECLARE_OBJECTS(Bus, ����98BM·, ����B2�߷���);
	DECLARE_OBJECTS(Water, ��ˮ, ��ˮ);
	DECLARE_OBJECTS(Coffee, һ���ȿ���, һ���俧��);
	DECLARE_OBJECTS(Kettle, ��ˮ��1, ��ˮ��2, ��ˮ��3, ��ˮ��4, ��ˮ��5);
	DECLARE_OBJECTS(Person, ��, ����, ����, ����, ����);
	DECLARE_OBJECTS(Phone, �ҵ�Oppo, �ҵĻ�Ϊ, �����յ�iPhoneX, ��������iPhone, �����յ�WinPhone);
	DECLARE_OBJECTS(Cup, ����1, ����2, ����3, ����4, ����5, ����6, ����7);
	DECLARE_OBJECTS(Solute, ȸ�����ȳ��, ��˹�������ȳ��, �������ȳ��, ����Ͻ����ܰ׿��ȳ��);
	DECLARE_OBJECTS(DrinkingFountain, ��ˮ��1, ��ˮ��2, ��ˮ��3);
	DECLARE_OBJECTS(Paper, ���Ծ�, ��˾��ͬ, �ҵı�ҵ֤��, �ⷿЭ��);
	DECLARE_OBJECTS(Copier, ��ӡ��1, ��ӡ��2, ��ӡ��3, ��ӡ��4, ��ӡ��5);

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

		����2����.stations = { &����,&�ܼ���,&��ˮ,&����·,&������,&����㳡,&����һ·,&������·,&����·,&�ɺ�·,&ӯ��·,&Ǯ�����ͳ�,\
			&Ǯ��·,&�촺�㳡,&����·,&������·,&�кӱ�·,&����·,&������,&������,&ѧԺ·,&�Ŵ�·,\
			&��̶·,&����,&����,&Ϻ����,&����,&�����,&��Ҷ�,&����,&�Ÿ���,&��� };
		����4����.stations = { &��,&�𳵶�վ,&�·�,&����,&����,&Ǯ��·,&����·,&��������,&����·,&����,&𮽭·,&������,&����·,&ˮ����,&��ׯ,&��ҽҩ��ѧ,&��Ҷ�,&���� };
		����5����.stations = { &����,&����Ŷ�,&���˺�,&����,&Ƽˮ��,&����,&����Ͻ��,&����,&�峣,&����,&��ʦ���ǰ,&����· };
		����B2�߷���.stations = { &�ػ��ֹ���վ,&��Ҷ�,&��Ҷ�·����,&ˮ��Է,&�Ŷ�·����·��,&�Ŷ�·ʯ����·��,&��������������,&����,&���»�԰,&���ׯ,&�����´�,&�ν���,&��һ·ѧԺ·��,&�㽭���̴�ѧ,&��԰�´�,&�̹�·��԰ͤ,&������,&���ֹ㳡��,&Ī��Ӫ,&��������,&������,&�³��������,&���·Ǯ��·��,&�������� };
		����98BM·.stations = { &���˺�,&Сʯ��,&��ӡ��ͨ��·��,&�ϼ���,&���,&��亱�,&����·ʯ��·��,&����·����ֿ�,&��԰·����·��,&��԰·����·��,&��԰��,&�ϼҶ�,&��Ҷ�,&��Ҷ�·�Ŷ�·��,&����� };

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

		��ˮ��2.content = { &��ˮ };
		����3.content = { &��ˮ };
		��.location = &����;
		for (auto drinkingFountainNode : theGlobalArea.findNounsByClass(concept_of(DrinkingFountain)))
		{
			auto drinkingFountain = (DrinkingFountain*)drinkingFountainNode->ptr;
			drinkingFountain->content = { &��ˮ,&��ˮ };
		}
	}
};

Water* theHotWater() { return &theWorld.��ˮ; }
Water* theColdWater() { return &theWorld.��ˮ; }
Coffee* theHotCoffee() { return &theWorld.һ���ȿ���; }
Coffee* theColdCoffee() { return &theWorld.һ���俧��; }
Person* __self() { return &theWorld.��; }