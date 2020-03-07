#include "stdafx.h"
#include"AINode.h"
#include"Classes.h"
#include"Proposition.h"
#include"global.h"
#include<iostream>
#include"AIHeader.h"
#include"TemporalOrderCompare.h"
#include"reflect_init.h"
#include"world_init.h"
using namespace std;

#define concept_named(varName) theGlobalArea.findNounByPtrAddress(&theWorld.varName)

#define ConArr vector<AIConceptNode*>

#define PAUSE_FOR_READING system("pause")
//#define PAUSE_FOR_READING

int main()
{
	theClasses._regist<Classes::howManyClasses_onlyStatic - 1>();//ע��������Ļ�����Ϣ����С����������������
	theClasses.regist_jsonSupportedClasses();//����֧��JSON�����࣬��Ҫ��¼��������Ե����ơ������Լ��洢λ��
	setRuntimeNameAndJsonGenerator();//��ÿ�������������¼���������Զ����ɡ�JSON��������
	theWorld.init();//TheWorld��һ��ģ���ⲿ��������ĵ����࣬����洢�˶��ֶ���ִ��init���ע����Щ������Ϣ
	registAllFunctions();//���ͺ����ʶ�ע�����ˣ�����ע�ắ����Ϣ
	cout << "�����ʼ����ɣ�\n\n";

	/***********************��ʼ��д��Ԥ��֪ʶ***********************/

	//�����������X�����ҵĵ�ǰλ�þ��ǵ���X���ڵ���X�ϣ�
	ActionAndEffect knowledge_takeOnSubway;
	knowledge_takeOnSubway.action = Sentence("��������|any$����");
	knowledge_takeOnSubway.effect = Proposition("ֵ���ڻ��Ϊ(��,\"��ǰλ��\",����[0])");
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeOnSubway);

	//������³����߳�A����վ�����ҵĵ�ǰλ�þ���Aվ
	ActionAndEffect knowledge_takeOff;
	knowledge_takeOff.action = Sentence("�³�����վ|any$��վ");
	knowledge_takeOff.effect = Proposition("ֵ���ڻ��Ϊ(��,\"��ǰλ��\",����[0])");
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeOff);

	//����ҳ�������Xǰ��A�أ����ҵĵ�ǰλ�ñ��ΪAվ
	ActionAndEffect knowledge_takeBus;
	knowledge_takeBus.action = Sentence("��������|any$������|any$��վ");
	knowledge_takeBus.effect = Proposition("ֵ���ڻ��Ϊ(��,\"��ǰλ��\",����[Ŀ�ĵ�])",
		knowledge_takeBus.action.verb->paraNames);
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeBus);

	//Ҫ��������X��ǰ�������ǵ���X��;��վ�������ҵĵ�ǰλ��
	ConditionAndAction conditionBefore_takeOnSubway;
	conditionBefore_takeOnSubway.action = Sentence("��������|any$����");
	conditionBefore_takeOnSubway.condition() = Proposition("������ӵ��Ԫ��(����[0],\";��վ\",\"��->��ǰλ��\")");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeOnSubway);

	//Ҫ��Aվ�³���ǰ�������ǵ�ǰ�����ĵ�������Aվ
	ConditionAndAction conditionBefore_takeOff;
	conditionBefore_takeOff.action = Sentence("�³�����վ|any$��վ");
	conditionBefore_takeOff.condition() = Proposition("������ӵ��Ԫ��(\"��->��ǰλ��\",\";��վ\",����[0])");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeOff);

	//Ҫ����������X��ǰ��������X�����������ҵ�ǰ��λ��
	ConditionAndAction conditionBefore_takeBus_1;
	conditionBefore_takeBus_1.action = Sentence("��������|any$������|any$��վ");
	conditionBefore_takeBus_1.condition() = Proposition("������ӵ��Ԫ��(����[������·],\";��վ\",\"��->��ǰλ��\")", 
		conditionBefore_takeBus_1.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeBus_1);
	const static size_t index_takeBus_1 = theGlobalArea.conditionBeforeDoings.size() - 1;
	auto getCondtionBeforeTakeBus1 = []()->ConditionAndAction& {
		return theGlobalArea.conditionBeforeDoings[index_takeBus_1];
	};

	//Ҫ����������ǰ��A�أ�ǰ�����������������ڵĹ���·�߾���A��
	ConditionAndAction conditionBefore_takeBus_2;
	conditionBefore_takeBus_2.action = conditionBefore_takeBus_1.action;
	conditionBefore_takeBus_2.condition() = Proposition("������ӵ��Ԫ��(����[������·],\";��վ\",����[Ŀ�ĵ�])",
		conditionBefore_takeBus_2.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeBus_2);
	const static size_t index_takeBus_2 = theGlobalArea.conditionBeforeDoings.size() - 1;
	auto getCondtionBeforeTakeBus2 = []()->ConditionAndAction& {
		return theGlobalArea.conditionBeforeDoings[index_takeBus_2];
	};

	//Ҫ��ĳ���������ȡ��ˮ��ǰ����������ˮԴ������Ҫ����ˮ
	ConditionAndAction conditionBefore_getHotWater;
	conditionBefore_getHotWater.action = Sentence("��ȡ��ˮ|any$����|any$ʢˮ������");
	conditionBefore_getHotWater.condition() = Proposition("������ӵ��Ԫ��(����[ˮԴ],\"����\",��ˮ)",
		conditionBefore_getHotWater.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_getHotWater);

	//Ҫ��ĳ���������ȡ��ˮ��ǰ����������ˮԴ������Ҫ����ˮ
	ConditionAndAction conditionBefore_getColdWater;
	conditionBefore_getColdWater.action = Sentence("��ȡ��ˮ|any$����|any$ʢˮ������");
	conditionBefore_getColdWater.condition() = Proposition("������ӵ��Ԫ��(����[ˮԴ],\"����\",��ˮ)",
		conditionBefore_getColdWater.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_getColdWater);

	//Ҫ����ˮ��ͨ�磬ǰ���Ǻ�����������ˮ
	ConditionAndAction conditionBefore_powerOn_kettle;
	conditionBefore_powerOn_kettle.action = Sentence("ͨ��|any$��ˮ��");
	conditionBefore_powerOn_kettle.condition() = Proposition("������ӵ��Ԫ��(����[0],\"����\",ˮ)");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_powerOn_kettle);

	/***********************������д��Ԥ��֪ʶ***********************/

	cout << "�����������ִ�в�������֮ǰ��Ҫ���������������Ԥ��֪ʶ����\n";
	for (auto& each : theGlobalArea.conditionBeforeDoings)
		cout << each.toString() << endl;
	cout << "\n";

	cout << "�����������ִ�в�������֮�����ɺ���Ӱ�죨Ԥ��֪ʶ����\n";
	for (auto& each : theGlobalArea.relationships_action_effect)
		cout << each.toString() << endl;
	cout << "\n";
	PAUSE_FOR_READING;

	/////////////////////////////////////////////////////////
	cout << "==============================================\n\n";
	cout << "��ʵ���������ʱ���㣨����/���ʣ��������ʾ���Ǻ����������������ǲ�������\n";
	for (AITemporalOrderNode* node : theGlobalArea.verbs)
		cout << node->toString_declaration() << endl;
	PAUSE_FOR_READING;
	cout << "==============================================\n\n";

	cout << "��ʵ��������¸����㣨���ʣ���������ʾ�������ǵ����ơ������Լ����ݣ�";
	for (AIConceptNode* node : theGlobalArea.nouns)
		cout << concept_string(node) << endl;
	PAUSE_FOR_READING;
	cout << "==============================================\n\n";

	/*ConditionAndAction condition_����;
	condition_����.action.verb = verb_of(����);
	condition_����.action.args.push_back(NounDescription());
	condition_����.action.args[0].descriptionType = class_NounDescriptionType;
	condition_����.action.args[0].ptr = concept_of(Cup);
	condition_����.effect = Proposition("������ӵ��Ԫ��(����[0],\"����\",��ˮ)");
	cout << condition_����.toString() << endl;

	system("pause");
	cout << "==============================================\n\n";*/

	Sentence command;
	ConclusionsLogger conLogger;//�����ϵ���ۼ�¼��
	cout << "������ĿA���ü����ͨ��ѧϰ���������ף����������תǮ����׵��˻�������٣��ҵ���������\n";
	cout << "����A1������������תǮ14.7Ԫ\n";
	command = Sentence(verb_of(ת��), ConArr{ concept_named(����),concept_named(����),new AIConceptNode(14.7) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ�\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����A2����������תǮ28.3Ԫ�����壺˵��ת�ʷ�����һ���̶�ֵ��\n";
	command = Sentence(verb_of(ת��), ConArr{ concept_named(����),concept_named(��),new AIConceptNode(28.3) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������2�εģ���\n";
	for (auto each : conLogger.timesCount)
		if(each.second>=2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����A3����������תǮ10.0Ԫ�����壺˵���տ����һ���̶�ֵ��\n";
	command = Sentence(verb_of(ת��), ConArr{ concept_named(����),concept_named(��),new AIConceptNode(10.0) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������3�εģ���\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout <<"��Щ���ۻᱻ�������������Ժ�ľ��ߺ�����ʹ��\n"<< endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

#if 0 //�޸��˹��ڵ����ĺ����������ע�͵�
	conLogger = ConclusionsLogger();//��ս��ۼ�¼��
	cout << "==============================================\n\n";
	cout << "������ĿB���ü����ͨ��ѧϰ���������ף������������ǰ���ص�X�����ҵĵ�ǰλ�û��ΪX\n";
	cout << "����B1����������1����ǰ����ѧ��\n";
	command = Sentence(verb_of(��������), ConArr{ concept_named(����1����),concept_named(��ѧ��) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ�\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����B2����������1����ǰ������֮�������壺˵��Ŀ�ĵز���һ���̶�ֵ��\n";
	command = Sentence(verb_of(��������), ConArr{ concept_named(����1����),concept_named(����֮��) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������2�εģ���\n";
	for (auto each : conLogger.timesCount)
		if (each.second >= 2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����B3����������2����ǰ����ǿ�������壺˵��������·�Ų���һ���̶�ֵ��\n";
	command = Sentence(verb_of(��������), ConArr{ concept_named(����2����),concept_named(��ǿ��) });
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������3�εģ���\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "��Щ���ۻᱻ�������������Ժ�ľ��ߺ�����ʹ��\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;
#endif

	conLogger = ConclusionsLogger();//��ս��ۼ�¼��
	cout << "==============================================\n\n";
	cout << "������ĿC���ü����ͨ��ѧϰ���������׵��á���ȡ��ˮ������������ʹĿ�걭����������ˮ\n";
	cout << "����C1������ˮ��2����ȡ����ˮ����ˮ�ӵ�����1����\n";
	command = Sentence("��ȡ��ˮ|����1|��ˮ��2");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ�\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����C2������ˮ��2�����ˮ���뱭��2��\n";
	command = Sentence("��ȡ��ˮ|����2|��ˮ��2"); 
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������2�εģ���\n";
	for (auto each : conLogger.timesCount)
		if (each.second >= 2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "����C3���ѱ���3�����ˮ���뱭��4��\n";
	command = Sentence("��ȡ��ˮ|����4|����3");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������3�εģ���\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "��Щ���ۻᱻ�������������Ժ�ľ��ߺ�����ʹ��\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	conLogger = ConclusionsLogger();//��ս��ۼ�¼��
	cout << "==============================================\n\n";
	cout << "������ĿD���ü����ͨ��ѧϰ���������ף�������Ƶ�ᵼ��ʣ��洢�ռ��С���洢�ռ�ʹ��������\n";
	command = Sentence("������Ƶ|�ҵĻ�Ϊ|\"av114514\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	command= Sentence("������Ƶ|�����յ�iPhoneX|\"av114514\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	command = Sentence("������Ƶ|�ҵĻ�Ϊ|\"sm240051\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������3�εģ���\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "��Щ���ۻᱻ�������������Ժ�ľ��ߺ�����ʹ��\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	conLogger = ConclusionsLogger();//��ս��ۼ�¼��
	cout << "==============================================\n\n";
	cout << "������ĿE���ü����ͨ��ѧϰ���������ף��������ֻᵼ��ʣ��洢�ռ��С���洢�ռ�ʹ��������\n";
	command = Sentence("��������|�ҵĻ�Ϊ|\"ǧ��֮��\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	command = Sentence("��������|�����յ�iPhoneX|\"��ɽ��\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	command = Sentence("��������|�����յ�WinPhone|\"���־���\"");
	conLogger.executeAndGetConclusions(command);//ִ����һ�������¼�½���
	cout << "���򱣴����¼�����ǰ��ı���״̬��ͨ���Ƚϣ��ó����½��ۣ���������ִ���������3�εģ���\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "��Щ���ۻᱻ�������������Ժ�ľ��ߺ�����ʹ��\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	ConclusionsLogger_VerbCompare conLogger_verbCmp;
	cout << "==============================================\n\n";
	cout << "���ʳ����ܲ��ԣ��ü��������֮ǰ���������������ɵó������ض������������ͣ��ᵼ��ʣ��洢�ռ��С���洢�ռ�ʹ��������\n";
	command = Sentence("������Ƶ|�ҵĻ�Ϊ|\"av233333\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	command = Sentence("��������|�ҵĻ�Ϊ|\"�������еİ�ο\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	command = Sentence("������Ƶ|�����յ�iPhoneX|\"av10001\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	for (auto each : conLogger_verbCmp.getValidConclusions())
	{
		cout << each.toString() << endl;
		theGlobalArea.relationships_action_effect.push_back(each);
	}
	PAUSE_FOR_READING;

	cout << "==============================================\n\n";
	cout << "���߹��ܲ���A��";
	cout << "�ҵ��������������˻������١�\n";
	cout << "�����ҳ��Ľ��������\n";
	auto methods_ZhangSan = theGlobalArea.findActionsByGoal(Proposition(pred_decreased, ConArr{ concept_named(����),new AIConceptNode(string("�˻����")) }));
	for (auto each : methods_ZhangSan)
		cout << each.toString() << endl;
	cout << "ִ�е�һ��������" << methods_ZhangSan[0].toString() << endl;
	methods_ZhangSan[0].execute();

	cout << endl;

	cout << "==============================================\n\n";
	cout << "���߹��ܲ���B��";
	cout << "�ҵ������ñ���5�����С���ˮ����\n";
	cout << "�����ҳ��Ľ��������\n";
	auto solutions = theGlobalArea.findActionsByGoal(Proposition("������ӵ��Ԫ��(����5,\"����\",��ˮ"));
	for (Sentence& solution : solutions)
	{
		vector<AIConceptNode*> suitableChoices = theGlobalArea.findNounsByClassWithRestriction(solution);
		vector<Sentence> afterReplace=solution.replaceAbstractItem_muliti(suitableChoices);
		for (Sentence& each_after : afterReplace)
			cout << each_after.toString() << "\n";
	}
	PAUSE_FOR_READING;

	//string inputStr = "��ȡ��ˮ|��ˮ��2|����3";
	//Sentence _command;
	//_command = Sentence("����|�ҵĻ�Ϊ|\"ǧ��֮��\"");
	////_command.args.push_back(NounDescription(new AIConceptNode(string("APTX4869"))));
	//_command.execute();

	cout << "\n\n";

	cout << "==============================================\n\n";
	cout << "ʱ��������A������������ʱ��\n";
	vector<Sentence> t1 = { Sentence("��������|����4����"),Sentence("�³�����վ|Ǯ�����ͳ�") };
	vector<Sentence> t2 = { Sentence("��������|����2����"),Sentence("�³�����վ|���") };
	vector<Sentence> t3 = { Sentence("��������|����5����"),Sentence("�³�����վ|���˺�") };
	cout << "ʱ��1:\n" << arrToString(t1) << "\n\n";
	cout << "ʱ��2:\n" << arrToString(t2) << "\n\n";
	cout << "ʱ��3:\n" << arrToString(t3) << "\n\n";
	vector<Sentence> finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "�����3��ʱ����бȽϣ��õ��ĳ���ʱ�����£�\n";
	cout << arrToString(finalResults) << "\n\n";
	PAUSE_FOR_READING;

	cout << "��ʱ���еĵ����滻Ϊ����[0]����վ�滻Ϊ����[1]���õ���\n";
	map<AIConceptNode*, int> replacementRules = {
		{concept_of(Subway),0},{concept_of(Station),1}
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	PAUSE_FOR_READING;

	auto newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "��������ǰ��ĳ��";
	newVerb->paraTypes = { concept_of(Subway),concept_of(Station) };
	newVerb->paraNames = { "������·","Ŀ��վ��" };
	cout << "�½�ʱ���㣬���֣�" + newVerb->name << "��   ��������" << strJoin(newVerb->paraNames, "��") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//��������
	cout << "���ڣ������������£�\n\n";
	cout << newVerb->toString_all() << endl;
	PAUSE_FOR_READING;

	cout << endl;
	vector<AIConceptNode*> actualArgs = { concept_named(����2����),concept_named(������) };
	vector<Sentence> concreteCmds= newVerb->toConcreteCommands(actualArgs);
	cout << "�������������2���ߣ�������  �õ���\n\n";
	cout << arrToString(concreteCmds) << endl;
	cout << endl;
	Sentence takeSubway2_ShentangBridge("��������ǰ��ĳ��|����2����|������");
	cout << "ִ�����" << takeSubway2_ShentangBridge.toString() << endl;
	takeSubway2_ShentangBridge();
	cout << "ִ�������Ժ��ҵĵ�ǰλ���ǣ�" << theGlobalArea.findNounByPtrAddress(theWorld.��.location)->name << endl;
	cout << "(�ɹ�����Ŀ�ĵء����ڻص���� ���ӣ�������һʵ�顣)\n";
	PAUSE_FOR_READING;
	cout << endl;
	theWorld.��.location = &theWorld.����;//�ص����

	void* returnValOfFunc;
	Sentence takeSubway4_Pengbu("��������ǰ��ĳ��|����4����|��");
	returnValOfFunc = takeSubway4_Pengbu();//ִ��������ȡ����ֵ
	cout << takeSubway4_Pengbu.toString() << endl;
	cout << "ִ����һ��䣬�����Ƿ���С�  ִ�еõ��ķ���ֵ�ǣ�" << returnValOfFunc << endl;
	cout << "ִ�������Ժ��ҵĵ�ǰλ���ǣ�" << theGlobalArea.findNounByPtrAddress(theWorld.��.location)->name << endl;
	cout << "(���ڵ���4���߸������������ӣ����Լ����ʵ���ϲ�û�гɹ�ִ���κ�һ�����)\n";
	PAUSE_FOR_READING;
	cout << endl;
	//theWorld.��.location = &theWorld.����;

	Sentence takeSubway5_Pengbu("��������ǰ��ĳ��|����5����|��");
	/*actualArgs = { concept_named(����5����),concept_named(��) };
	concreteCmds = newVerb->toConcreteCommands(actualArgs);
	bool checkResult= Sentence::checkAllBeforeExecute(concreteCmds);
	cout << "����Ҫִ�е�����ǣ�" << takeSubway5_Pengbu.toString() << "�������������ִ�гɹ�����Ϊ����5���߲���������\n";
	cout << "��ִ��ǰ���������һ������п������������������Ϊ��" << checkResult << endl;*/
	returnValOfFunc = takeSubway5_Pengbu();//ִ��������ȡ����ֵ
	cout << takeSubway5_Pengbu.toString() << endl;
	cout << "ִ����һ��䣬�����Ƿ���С�  ִ�еõ��ķ���ֵ�ǣ�" << returnValOfFunc << endl;
	cout << "ִ�������Ժ��ҵĵ�ǰλ���ǣ�" << theGlobalArea.findNounByPtrAddress(theWorld.��.location)->name << endl;
	cout << "(��һ��û���⣬���ҡ������˵���5���ߣ��������˵����Ժ�����೵������Ŀ�ĵء����������ԡ��ҡ������ڵ����ϣ�û�����κεط��³���)\n";
	cout << "�ڽ������������У��ҽ�ͨ���޸Ķ���ִ�е�ǰ��������ʹ�ó�����ִ����һ���ǰ��ֱ�ӽ����ж�Ϊ�������еĶ��������Ӷ���ֹ���ҡ���˵���5���ߡ�\n";
	cout << endl;
	theWorld.��.location = &theWorld.����;//�ص����
	PAUSE_FOR_READING;

	cout << "�½����������ͨ���ߡ�����Bus(������)��Subway(����)��Ϊ���¼�������\n";
	auto ������ͨ���� = new AIConceptNode;
	������ͨ����->name = "������ͨ����";
	������ͨ����->insertConcreteNode(concept_of(Bus));
	������ͨ����->insertConcreteNode(concept_of(Subway));
	theGlobalArea.nouns.push_back(������ͨ����);

	cout << "�½����󶯴ʡ�����������ͨ���ߡ������������������͡���������ǰ��ĳ�ء���Ϊ���¼����嶯�ʡ�\n";
	auto ����������ͨ���� = new AITemporalOrderNode;
	����������ͨ����->name = "����������ͨ����";
	����������ͨ����->paraTypes = { ������ͨ����,concept_of(Station) };
	����������ͨ����->paraNames = { "������ͨ��·","Ŀ�ĵ�" };
	����������ͨ����->insertConcreteNode(theGlobalArea.findVerbByName("��������"));
	����������ͨ����->insertConcreteNode(theGlobalArea.findVerbByName("��������ǰ��ĳ��"));
	theGlobalArea.verbs.push_back(����������ͨ����);
	cout << "������ʵĲ�����ʽ���£�\n" << ����������ͨ����->toString_declaration() << endl;
	PAUSE_FOR_READING;

	cout << endl;
	cout << "�ڳ����Ԥ��֪ʶ���棬���������ڳ���������ǰ���������������еġ�����������չΪ��������ͨ���ߡ���"<<\
		"���������������滻Ϊ������������ͨ���ߡ������ɴﵽЧ����\n";
	cout << "�滻ǰ��\n";
	cout << getCondtionBeforeTakeBus1().toString() << endl;
	cout << getCondtionBeforeTakeBus2().toString() << endl;

	getCondtionBeforeTakeBus1().action.verb = ����������ͨ����;
	getCondtionBeforeTakeBus1().action.args[0].ptr = ������ͨ����;
	getCondtionBeforeTakeBus2().action.verb = ����������ͨ����;
	getCondtionBeforeTakeBus2().action.args[0].ptr = ������ͨ����;
	cout << "�滻��Ĺ���\n";
	cout << getCondtionBeforeTakeBus1().toString() << endl;
	cout << getCondtionBeforeTakeBus2().toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;

	cout << "��ִ��һ�� " << takeSubway5_Pengbu.toString() << endl;
	takeSubway5_Pengbu();
	cout << "ִ�������Ժ��ҵĵ�ǰλ���ǣ�" << theGlobalArea.findNounByPtrAddress(theWorld.��.location)->name << endl;
	cout << "��˵���������ڡ��ҡ��ϵ���֮ǰ��Ԥ��������������������ҵ�Ŀ�ĵأ��Ӷ���ֹ�ˡ��ҡ���˵�������Ϊ��\n";
	cout << endl;
	PAUSE_FOR_READING;

	theWorld.����1.content.clear();
	theWorld.����2.content.clear();
	theWorld.����3.content.clear();
	theWorld.����4.content = { &theWorld.��ˮ };

	cout << "==============================================\n\n";
	cout << "ʱ��������B������������ʱ��\n";
	t1 = {Sentence("��ȡ��ˮ|����3|��ˮ��2"),Sentence("���뱭��|����3|��˹�������ȳ��"),Sentence("����|����3")};
	t2 = {Sentence("��ȡ��ˮ|����1|��ˮ��3"),Sentence("���뱭��|����1|��˹�������ȳ��"),Sentence("����|����1") };
	t3 = {Sentence("��ȡ��ˮ|����2|��ˮ��2"),Sentence("���뱭��|����2|ȸ�����ȳ��"),Sentence("����|����2") };
	cout << "ʱ��1:\n" << arrToString(t1) << "\n\n";
	cout << "ʱ��2:\n" << arrToString(t2) << "\n\n";
	cout << "ʱ��3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "�����3��ʱ����бȽϣ��õ��ĳ���ʱ�����£�\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "��ʱ���е�any$�����滻Ϊ����[0]��any$���ȳ���滻Ϊ����[1]���õ���\n";
	replacementRules = {
		{ concept_of(Cup),0 },{ concept_of(Solute),1 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";

	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "�ݿ���";
	newVerb->paraTypes = { concept_of(Cup),concept_of(Solute) };
	newVerb->paraNames = { "����","���ȳ��" };
	cout << "�½�ʱ���㣬���֣�" + newVerb->name << "��   ��������" << strJoin(newVerb->paraNames, "��") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//��������
	cout << "�ղŵ�����ָ����Ǻ��������ݡ����ڣ������������£�\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	theWorld.����1.content.clear();
	command = Sentence("�ݿ���|����1|�������ȳ��");
	cout << "ִ�����" << command.toString() << endl;
	void* ret=command();
	cout << "ִ�������Ժ󣬷���ֵ��" << ret << "������1��״̬Ϊ��\n";
	cout << json_toString(concept_named(����1)->toJson()) << endl;
	PAUSE_FOR_READING;

	cout << endl<<endl;
	cout << "==============================================\n";
	cout << "ʱ��������B������������ʱ��\n";
	t1 = { Sentence("�Ž���ӡ��|��ӡ��1|�ⷿЭ��"),Sentence("���÷�������ӡ|��ӡ��1|2"),
		Sentence("ȡ����ӡ��|��ӡ��1"),Sentence("ȡ����ӡԭ��|��ӡ��1|�ⷿЭ��") };
	t2 = { Sentence("�Ž���ӡ��|��ӡ��2|�ⷿЭ��"),Sentence("���÷�������ӡ|��ӡ��2|1"),
		Sentence("ȡ����ӡ��|��ӡ��2"),Sentence("ȡ����ӡԭ��|��ӡ��2|�ⷿЭ��") };
	t3 = { Sentence("�Ž���ӡ��|��ӡ��2|���Ծ�"),Sentence("���÷�������ӡ|��ӡ��2|3"),
		Sentence("ȡ����ӡ��|��ӡ��2"),Sentence("ȡ����ӡԭ��|��ӡ��2|���Ծ�") };
	cout << "ʱ��1:\n" << arrToString(t1) << "\n\n";
	cout << "ʱ��2:\n" << arrToString(t2) << "\n\n";
	cout << "ʱ��3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "�����3��ʱ����бȽϣ��õ��ĳ���ʱ�����£�\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "��ʱ���е�any$��ӡ���滻Ϊ����[0]��any$ֽ���滻Ϊ����[1]��any$�����滻Ϊ����[2]�õ���\n";
	replacementRules = {
		{ concept_of(Copier),0 },{ concept_of(Paper),1 },{ concept_of(int),2 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "��ӡ";
	newVerb->paraTypes = { concept_of(Copier),concept_of(Paper),concept_of(int) };
	newVerb->paraNames = { "��ӡ��","Ҫ��ӡ��ֽ��","����" };
	cout << "�½�ʱ���㣬���֣�" + newVerb->name << "��   ��������" << strJoin(newVerb->paraNames, "��") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//��������
	cout << "�ղŵ�����ָ����Ǻ��������ݡ����ڣ������������£�\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	
	command = Sentence("��ӡ|��ӡ��3|�ҵı�ҵ֤��|2");
	cout << "ִ�����" << command.toString() << endl;
	ret = command();
	cout << "ִ�������Ժ󣬷���ֵ��" << ret << "����������ģ�⻷�����ܹ�����Щֽ��\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Paper)))
		cout << node->name << endl;
	cout << endl;
	PAUSE_FOR_READING;

	cout << "�ճ������У���Ҫ�ñ��˰��Ҹ�ӡ���ϣ���ֻ��ת����Ҫ��ӡʲô���ϡ���ӡ���ݾ����ˣ������ĸ���ӡ����ӡ���ƺ��Ǹ����������\n";
	cout << "�ڽ�������ʵ���У��ҽ��򻯵�����������ü����ѡ������һ̨��ӡ�������и�ӡ��\n";
	cout << endl;

	t1 = { Sentence("��ӡ|��ӡ��2|�ⷿЭ��|1") };
	t2 = { Sentence("��ӡ|��ӡ��1|��˾��ͬ|1") };
	t3 = { Sentence("��ӡ|��ӡ��1|�ҵı�ҵ֤��|2") };
	cout << "ʱ��1:\n" << arrToString(t1) << "\n\n";
	cout << "ʱ��2:\n" << arrToString(t2) << "\n\n";
	cout << "ʱ��3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "�����3��ʱ����бȽϣ��õ��ĳ���ʱ�����£�\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "��ʱ���е�any$ֽ���滻Ϊ����[0]��any$�����滻Ϊ����[1]�õ���\n";
	replacementRules = {
		{ concept_of(Paper),0 },{ concept_of(int),1 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "�򻯰渴ӡ";
	newVerb->paraTypes = { concept_of(Paper),concept_of(int) };
	newVerb->paraNames = { "Ҫ��ӡ��ֽ��","����" };
	cout << "�½�ʱ���㣬���֣�" + newVerb->name << "��   ��������" << strJoin(newVerb->paraNames, "��") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//��������
	cout << "�ղŵ�����ָ����Ǻ��������ݡ����ڣ������������£�\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	command = Sentence("�򻯰渴ӡ|���Ծ�|2");
	cout << "ִ�����" << command.toString() << endl;
	ret = command();
	cout << "ִ�������Ժ󣬷���ֵ��" << ret << "����������ģ�⻷�����ܹ�����Щֽ��\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Paper)))
		cout << node->name << endl;
	cout << "������ӡ����״̬������ʾ��\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Copier)))
		cout << node->name+"��"<<json_toString(node->toJson()) << endl;
	cout << endl;

	return 0;
}