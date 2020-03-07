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
	theClasses._regist<Classes::howManyClasses_onlyStatic - 1>();//注册所有类的基础信息（大小、编译期类型名）
	theClasses.regist_jsonSupportedClasses();//对于支持JSON化的类，需要记录其各个属性的名称、类型以及存储位置
	setRuntimeNameAndJsonGenerator();//把每个类的中文名记录下来，并自动生成“JSON生成器”
	theWorld.init();//TheWorld是一个模拟外部物质世界的单例类，里面存储了多种对象，执行init便可注册这些对象信息
	registAllFunctions();//类型和名词都注册完了，现在注册函数信息
	cout << "程序初始化完成！\n\n";

	/***********************开始：写入预置知识***********************/

	//如果乘坐地铁X，则我的当前位置就是地铁X（在地铁X上）
	ActionAndEffect knowledge_takeOnSubway;
	knowledge_takeOnSubway.action = Sentence("乘坐地铁|any$地铁");
	knowledge_takeOnSubway.effect = Proposition("值等于或变为(我,\"当前位置\",参数[0])");
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeOnSubway);

	//如果我下车并走出A地铁站，则我的当前位置就是A站
	ActionAndEffect knowledge_takeOff;
	knowledge_takeOff.action = Sentence("下车并出站|any$车站");
	knowledge_takeOff.effect = Proposition("值等于或变为(我,\"当前位置\",参数[0])");
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeOff);

	//如果我乘坐公交X前往A地，则我的当前位置变更为A站
	ActionAndEffect knowledge_takeBus;
	knowledge_takeBus.action = Sentence("乘坐公交|any$公交车|any$车站");
	knowledge_takeBus.effect = Proposition("值等于或变为(我,\"当前位置\",参数[目的地])",
		knowledge_takeBus.action.verb->paraNames);
	theGlobalArea.relationships_action_effect.push_back(knowledge_takeBus);

	//要乘坐地铁X，前提条件是地铁X的途径站里面有我的当前位置
	ConditionAndAction conditionBefore_takeOnSubway;
	conditionBefore_takeOnSubway.action = Sentence("乘坐地铁|any$地铁");
	conditionBefore_takeOnSubway.condition() = Proposition("新增或拥有元素(参数[0],\"途径站\",\"我->当前位置\")");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeOnSubway);

	//要在A站下车，前提条件是当前乘坐的地铁经过A站
	ConditionAndAction conditionBefore_takeOff;
	conditionBefore_takeOff.action = Sentence("下车并出站|any$车站");
	conditionBefore_takeOff.condition() = Proposition("新增或拥有元素(\"我->当前位置\",\"途径站\",参数[0])");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeOff);

	//要乘坐公交车X，前提条件是X公交车经过我当前的位置
	ConditionAndAction conditionBefore_takeBus_1;
	conditionBefore_takeBus_1.action = Sentence("乘坐公交|any$公交车|any$车站");
	conditionBefore_takeBus_1.condition() = Proposition("新增或拥有元素(参数[公交线路],\"途径站\",\"我->当前位置\")", 
		conditionBefore_takeBus_1.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeBus_1);
	const static size_t index_takeBus_1 = theGlobalArea.conditionBeforeDoings.size() - 1;
	auto getCondtionBeforeTakeBus1 = []()->ConditionAndAction& {
		return theGlobalArea.conditionBeforeDoings[index_takeBus_1];
	};

	//要乘坐公交车前往A地，前提条件是我现在所在的公交路线经过A地
	ConditionAndAction conditionBefore_takeBus_2;
	conditionBefore_takeBus_2.action = conditionBefore_takeBus_1.action;
	conditionBefore_takeBus_2.condition() = Proposition("新增或拥有元素(参数[公交线路],\"途径站\",参数[目的地])",
		conditionBefore_takeBus_2.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_takeBus_2);
	const static size_t index_takeBus_2 = theGlobalArea.conditionBeforeDoings.size() - 1;
	auto getCondtionBeforeTakeBus2 = []()->ConditionAndAction& {
		return theGlobalArea.conditionBeforeDoings[index_takeBus_2];
	};

	//要从某个容器里获取热水，前提是容器（水源）里面要有热水
	ConditionAndAction conditionBefore_getHotWater;
	conditionBefore_getHotWater.action = Sentence("获取热水|any$杯子|any$盛水的容器");
	conditionBefore_getHotWater.condition() = Proposition("新增或拥有元素(参数[水源],\"内容\",热水)",
		conditionBefore_getHotWater.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_getHotWater);

	//要从某个容器里获取冷水，前提是容器（水源）里面要有冷水
	ConditionAndAction conditionBefore_getColdWater;
	conditionBefore_getColdWater.action = Sentence("获取冷水|any$杯子|any$盛水的容器");
	conditionBefore_getColdWater.condition() = Proposition("新增或拥有元素(参数[水源],\"内容\",冷水)",
		conditionBefore_getColdWater.action.verb->paraNames);
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_getColdWater);

	//要给烧水壶通电，前提是壶里面必须得有水
	ConditionAndAction conditionBefore_powerOn_kettle;
	conditionBefore_powerOn_kettle.action = Sentence("通电|any$烧水壶");
	conditionBefore_powerOn_kettle.condition() = Proposition("新增或拥有元素(参数[0],\"内容\",水)");
	theGlobalArea.conditionBeforeDoings.push_back(conditionBefore_powerOn_kettle);

	/***********************结束：写入预置知识***********************/

	cout << "现在输出的是执行部分命令之前需要满足的限制条件（预置知识）：\n";
	for (auto& each : theGlobalArea.conditionBeforeDoings)
		cout << each.toString() << endl;
	cout << "\n";

	cout << "现在输出的是执行部分命令之后会造成何种影响（预置知识）：\n";
	for (auto& each : theGlobalArea.relationships_action_effect)
		cout << each.toString() << endl;
	cout << "\n";
	PAUSE_FOR_READING;

	/////////////////////////////////////////////////////////
	cout << "==============================================\n\n";
	cout << "本实验包含以下时序结点（函数/动词），左边显示的是函数名，括号里面是参数类型\n";
	for (AITemporalOrderNode* node : theGlobalArea.verbs)
		cout << node->toString_declaration() << endl;
	PAUSE_FOR_READING;
	cout << "==============================================\n\n";

	cout << "本实验包含以下概念结点（名词），现在显示的是它们的名称、类型以及内容：";
	for (AIConceptNode* node : theGlobalArea.nouns)
		cout << concept_string(node) << endl;
	PAUSE_FOR_READING;
	cout << "==============================================\n\n";

	/*ConditionAndAction condition_搅拌;
	condition_搅拌.action.verb = verb_of(搅拌);
	condition_搅拌.action.args.push_back(NounDescription());
	condition_搅拌.action.args[0].descriptionType = class_NounDescriptionType;
	condition_搅拌.action.args[0].ptr = concept_of(Cup);
	condition_搅拌.effect = Proposition("新增或拥有元素(参数[0],\"内容\",热水)");
	cout << condition_搅拌.toString() << endl;

	system("pause");
	cout << "==============================================\n\n";*/

	Sentence command;
	ConclusionsLogger conLogger;//因果关系结论记录器
	cout << "测试项目A：让计算机通过学习案例，明白：如果甲向乙转钱，则甲的账户余额会减少，乙的余额会增加\n";
	cout << "案例A1：李四向张三转钱14.7元\n";
	command = Sentence(verb_of(转账), ConArr{ concept_named(张三),concept_named(李四),new AIConceptNode(14.7) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论：\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例A2：我向张三转钱28.3元（意义：说明转帐方不是一个固定值）\n";
	command = Sentence(verb_of(转账), ConArr{ concept_named(张三),concept_named(我),new AIConceptNode(28.3) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于2次的）：\n";
	for (auto each : conLogger.timesCount)
		if(each.second>=2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例A3：我向王五转钱10.0元（意义：说明收款方不是一个固定值）\n";
	command = Sentence(verb_of(转账), ConArr{ concept_named(王五),concept_named(我),new AIConceptNode(10.0) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于3次的）：\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout <<"这些结论会被保存起来，供以后的决策和推理使用\n"<< endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

#if 0 //修改了关于地铁的函数，这段先注释掉
	conLogger = ConclusionsLogger();//清空结论记录器
	cout << "==============================================\n\n";
	cout << "测试项目B：让计算机通过学习案例，明白：如果乘坐地铁前往地点X，则我的当前位置会变为X\n";
	cout << "案例B1：乘坐地铁1号线前往科学馆\n";
	command = Sentence(verb_of(乘坐地铁), ConArr{ concept_named(地铁1号线),concept_named(科学馆) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论：\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例B2：乘坐地铁1号线前往世界之窗（意义：说明目的地不是一个固定值）\n";
	command = Sentence(verb_of(乘坐地铁), ConArr{ concept_named(地铁1号线),concept_named(世界之窗) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于2次的）：\n";
	for (auto each : conLogger.timesCount)
		if (each.second >= 2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例B3：乘坐地铁2号线前往华强北（意义：说明地铁线路号不是一个固定值）\n";
	command = Sentence(verb_of(乘坐地铁), ConArr{ concept_named(地铁2号线),concept_named(华强北) });
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于3次的）：\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "这些结论会被保存起来，供以后的决策和推理使用\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;
#endif

	conLogger = ConclusionsLogger();//清空结论记录器
	cout << "==============================================\n\n";
	cout << "测试项目C：让计算机通过学习案例，明白调用【获取热水】函数，将会使目标杯子里面有热水\n";
	cout << "案例C1：从饮水机2里面取得热水，把水加到杯子1里面\n";
	command = Sentence("获取热水|杯子1|饮水机2");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论：\n";
	for (auto each : conLogger.timesCount)
		cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例C2：把烧水壶2里面的水倒入杯子2里\n";
	command = Sentence("获取热水|杯子2|烧水壶2"); 
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于2次的）：\n";
	for (auto each : conLogger.timesCount)
		if (each.second >= 2)
			cout << each.first.toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	cout << "案例C3：把杯子3里面的水倒入杯子4里\n";
	command = Sentence("获取热水|杯子4|杯子3");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于3次的）：\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "这些结论会被保存起来，供以后的决策和推理使用\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	conLogger = ConclusionsLogger();//清空结论记录器
	cout << "==============================================\n\n";
	cout << "测试项目D：让计算机通过学习案例，明白：缓存视频会导致剩余存储空间减小，存储空间使用率升高\n";
	command = Sentence("缓存视频|我的华为|\"av114514\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	command= Sentence("缓存视频|贾晓刚的iPhoneX|\"av114514\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	command = Sentence("缓存视频|我的华为|\"sm240051\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于3次的）：\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "这些结论会被保存起来，供以后的决策和推理使用\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	conLogger = ConclusionsLogger();//清空结论记录器
	cout << "==============================================\n\n";
	cout << "测试项目E：让计算机通过学习案例，明白：下载音乐会导致剩余存储空间减小，存储空间使用率升高\n";
	command = Sentence("下载音乐|我的华为|\"千里之外\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	command = Sentence("下载音乐|贾晓刚的iPhoneX|\"南山南\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	command = Sentence("下载音乐|贾晓刚的WinPhone|\"极乐净土\"");
	conLogger.executeAndGetConclusions(command);//执行这一命令，并记录下结论
	cout << "程序保存了事件发生前后的变量状态，通过比较，得出如下结论（仅输出出现次数不少于3次的）：\n";
	for (auto each : conLogger.getValidConclusions())
		cout << each.toString() << endl;
	cout << "这些结论会被保存起来，供以后的决策和推理使用\n" << endl;
	for (auto each : conLogger.getValidConclusions())
		theGlobalArea.relationships_action_effect.push_back(each);
	PAUSE_FOR_READING;

	ConclusionsLogger_VerbCompare conLogger_verbCmp;
	cout << "==============================================\n\n";
	cout << "动词抽象功能测试：让计算机根据之前的两个案例，归纳得出：下载东西（任意类型）会导致剩余存储空间减小，存储空间使用率升高\n";
	command = Sentence("缓存视频|我的华为|\"av233333\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	command = Sentence("下载音乐|我的华为|\"整座城市的安慰\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	command = Sentence("缓存视频|贾晓刚的iPhoneX|\"av10001\"");
	conLogger_verbCmp.insertCommandAndKnownEffect(command);
	for (auto each : conLogger_verbCmp.getValidConclusions())
	{
		cout << each.toString() << endl;
		theGlobalArea.relationships_action_effect.push_back(each);
	}
	PAUSE_FOR_READING;

	cout << "==============================================\n\n";
	cout << "决策功能测试A：";
	cout << "我的需求：让张三的账户余额减少。\n";
	cout << "程序找出的解决方法：\n";
	auto methods_ZhangSan = theGlobalArea.findActionsByGoal(Proposition(pred_decreased, ConArr{ concept_named(张三),new AIConceptNode(string("账户余额")) }));
	for (auto each : methods_ZhangSan)
		cout << each.toString() << endl;
	cout << "执行第一个方案：" << methods_ZhangSan[0].toString() << endl;
	methods_ZhangSan[0].execute();

	cout << endl;

	cout << "==============================================\n\n";
	cout << "决策功能测试B：";
	cout << "我的需求：让杯子5里面有【热水】。\n";
	cout << "程序找出的解决方法：\n";
	auto solutions = theGlobalArea.findActionsByGoal(Proposition("新增或拥有元素(杯子5,\"内容\",热水"));
	for (Sentence& solution : solutions)
	{
		vector<AIConceptNode*> suitableChoices = theGlobalArea.findNounsByClassWithRestriction(solution);
		vector<Sentence> afterReplace=solution.replaceAbstractItem_muliti(suitableChoices);
		for (Sentence& each_after : afterReplace)
			cout << each_after.toString() << "\n";
	}
	PAUSE_FOR_READING;

	//string inputStr = "获取冷水|饮水机2|杯子3";
	//Sentence _command;
	//_command = Sentence("下载|我的华为|\"千里之外\"");
	////_command.args.push_back(NounDescription(new AIConceptNode(string("APTX4869"))));
	//_command.execute();

	cout << "\n\n";

	cout << "==============================================\n\n";
	cout << "时序抽象测试A，有下面三条时序：\n";
	vector<Sentence> t1 = { Sentence("乘坐地铁|地铁4号线"),Sentence("下车并出站|钱江世纪城") };
	vector<Sentence> t2 = { Sentence("乘坐地铁|地铁2号线"),Sentence("下车并出站|良渚") };
	vector<Sentence> t3 = { Sentence("乘坐地铁|地铁5号线"),Sentence("下车并出站|大运河") };
	cout << "时序1:\n" << arrToString(t1) << "\n\n";
	cout << "时序2:\n" << arrToString(t2) << "\n\n";
	cout << "时序3:\n" << arrToString(t3) << "\n\n";
	vector<Sentence> finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "程序对3条时序进行比较，得到的抽象时序如下：\n";
	cout << arrToString(finalResults) << "\n\n";
	PAUSE_FOR_READING;

	cout << "将时序中的地铁替换为参数[0]，车站替换为参数[1]，得到：\n";
	map<AIConceptNode*, int> replacementRules = {
		{concept_of(Subway),0},{concept_of(Station),1}
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	PAUSE_FOR_READING;

	auto newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "乘坐地铁前往某地";
	newVerb->paraTypes = { concept_of(Subway),concept_of(Station) };
	newVerb->paraNames = { "地铁线路","目标站点" };
	cout << "新建时序结点，名字：" + newVerb->name << "，   参数名：" << strJoin(newVerb->paraNames, "，") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//保存起来
	cout << "现在，函数内容如下：\n\n";
	cout << newVerb->toString_all() << endl;
	PAUSE_FOR_READING;

	cout << endl;
	vector<AIConceptNode*> actualArgs = { concept_named(地铁2号线),concept_named(沈塘桥) };
	vector<Sentence> concreteCmds= newVerb->toConcreteCommands(actualArgs);
	cout << "代入参数：地铁2号线，沈塘桥  得到：\n\n";
	cout << arrToString(concreteCmds) << endl;
	cout << endl;
	Sentence takeSubway2_ShentangBridge("乘坐地铁前往某地|地铁2号线|沈塘桥");
	cout << "执行命令：" << takeSubway2_ShentangBridge.toString() << endl;
	takeSubway2_ShentangBridge();
	cout << "执行命令以后，我的当前位置是：" << theGlobalArea.findNounByPtrAddress(theWorld.我.location)->name << endl;
	cout << "(成功到达目的地。现在回到起点 三坝，进行下一实验。)\n";
	PAUSE_FOR_READING;
	cout << endl;
	theWorld.我.location = &theWorld.三坝;//回到起点

	void* returnValOfFunc;
	Sentence takeSubway4_Pengbu("乘坐地铁前往某地|地铁4号线|彭埠");
	returnValOfFunc = takeSubway4_Pengbu();//执行这句命令，取返回值
	cout << takeSubway4_Pengbu.toString() << endl;
	cout << "执行这一语句，不管是否可行。  执行得到的返回值是：" << returnValOfFunc << endl;
	cout << "执行命令以后，我的当前位置是：" << theGlobalArea.findNounByPtrAddress(theWorld.我.location)->name << endl;
	cout << "(由于地铁4号线根本不经过三坝，所以计算机实际上并没有成功执行任何一条命令。)\n";
	PAUSE_FOR_READING;
	cout << endl;
	//theWorld.我.location = &theWorld.三坝;

	Sentence takeSubway5_Pengbu("乘坐地铁前往某地|地铁5号线|彭埠");
	/*actualArgs = { concept_named(地铁5号线),concept_named(彭埠) };
	concreteCmds = newVerb->toConcreteCommands(actualArgs);
	bool checkResult= Sentence::checkAllBeforeExecute(concreteCmds);
	cout << "现在要执行的语句是：" << takeSubway5_Pengbu.toString() << "，这条命令不可能执行成功，因为地铁5号线不经过彭埠。\n";
	cout << "在执行前，程序对这一命令进行可行性评估，评估结果为：" << checkResult << endl;*/
	returnValOfFunc = takeSubway5_Pengbu();//执行这句命令，取返回值
	cout << takeSubway5_Pengbu.toString() << endl;
	cout << "执行这一语句，不管是否可行。  执行得到的返回值是：" << returnValOfFunc << endl;
	cout << "执行命令以后，我的当前位置是：" << theGlobalArea.findNounByPtrAddress(theWorld.我.location)->name << endl;
	cout << "(第一步没问题，“我”坐上了地铁5号线，但是上了地铁以后发现这班车不经过目的地【彭埠】，所以“我”滞留在地铁上，没有在任何地方下车。)\n";
	cout << "在接下来的试验中，我将通过修改动作执行的前提条件，使得程序在执行这一语句前，直接将其判定为【不可行的动作】，从而阻止“我”搭乘地铁5号线。\n";
	cout << endl;
	theWorld.我.location = &theWorld.三坝;//回到起点
	PAUSE_FOR_READING;

	cout << "新建概念【公共交通工具】，将Bus(公交车)和Subway(地铁)设为其下级具象概念。\n";
	auto 公共交通工具 = new AIConceptNode;
	公共交通工具->name = "公共交通工具";
	公共交通工具->insertConcreteNode(concept_of(Bus));
	公共交通工具->insertConcreteNode(concept_of(Subway));
	theGlobalArea.nouns.push_back(公共交通工具);

	cout << "新建抽象动词【乘坐公共交通工具】，将【乘坐公交】和【乘坐地铁前往某地】设为其下级具体动词。\n";
	auto 乘坐公共交通工具 = new AITemporalOrderNode;
	乘坐公共交通工具->name = "乘坐公共交通工具";
	乘坐公共交通工具->paraTypes = { 公共交通工具,concept_of(Station) };
	乘坐公共交通工具->paraNames = { "公共交通线路","目的地" };
	乘坐公共交通工具->insertConcreteNode(theGlobalArea.findVerbByName("乘坐公交"));
	乘坐公共交通工具->insertConcreteNode(theGlobalArea.findVerbByName("乘坐地铁前往某地"));
	theGlobalArea.verbs.push_back(乘坐公共交通工具);
	cout << "这个动词的参数形式如下：\n" << 乘坐公共交通工具->toString_declaration() << endl;
	PAUSE_FOR_READING;

	cout << endl;
	cout << "在程序的预置知识里面，有两条关于乘坐公交的前提条件，将动作中的【公交车】拓展为【公共交通工具】，"<<\
		"将【乘坐公交】替换为【乘坐公共交通工具】，即可达到效果。\n";
	cout << "替换前：\n";
	cout << getCondtionBeforeTakeBus1().toString() << endl;
	cout << getCondtionBeforeTakeBus2().toString() << endl;

	getCondtionBeforeTakeBus1().action.verb = 乘坐公共交通工具;
	getCondtionBeforeTakeBus1().action.args[0].ptr = 公共交通工具;
	getCondtionBeforeTakeBus2().action.verb = 乘坐公共交通工具;
	getCondtionBeforeTakeBus2().action.args[0].ptr = 公共交通工具;
	cout << "替换后的规则：\n";
	cout << getCondtionBeforeTakeBus1().toString() << endl;
	cout << getCondtionBeforeTakeBus2().toString() << endl;
	cout << endl;
	PAUSE_FOR_READING;

	cout << "再执行一次 " << takeSubway5_Pengbu.toString() << endl;
	takeSubway5_Pengbu();
	cout << "执行命令以后，我的当前位置是：" << theGlobalArea.findNounByPtrAddress(theWorld.我.location)->name << endl;
	cout << "这说明：程序在“我”上地铁之前就预先推理出这班地铁不经过我的目的地，从而阻止了“我”搭乘地铁的行为。\n";
	cout << endl;
	PAUSE_FOR_READING;

	theWorld.杯子1.content.clear();
	theWorld.杯子2.content.clear();
	theWorld.杯子3.content.clear();
	theWorld.杯子4.content = { &theWorld.热水 };

	cout << "==============================================\n\n";
	cout << "时序抽象测试B，有下面三条时序：\n";
	t1 = {Sentence("获取热水|杯子3|饮水机2"),Sentence("放入杯中|杯子3|麦斯威尔咖啡冲剂"),Sentence("搅拌|杯子3")};
	t2 = {Sentence("获取热水|杯子1|饮水机3"),Sentence("放入杯中|杯子1|麦斯威尔咖啡冲剂"),Sentence("搅拌|杯子1") };
	t3 = {Sentence("获取热水|杯子2|烧水壶2"),Sentence("放入杯中|杯子2|雀巢咖啡冲剂"),Sentence("搅拌|杯子2") };
	cout << "时序1:\n" << arrToString(t1) << "\n\n";
	cout << "时序2:\n" << arrToString(t2) << "\n\n";
	cout << "时序3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "程序对3条时序进行比较，得到的抽象时序如下：\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "将时序中的any$杯子替换为参数[0]，any$咖啡冲剂替换为参数[1]，得到：\n";
	replacementRules = {
		{ concept_of(Cup),0 },{ concept_of(Solute),1 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";

	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "泡咖啡";
	newVerb->paraTypes = { concept_of(Cup),concept_of(Solute) };
	newVerb->paraNames = { "杯子","咖啡冲剂" };
	cout << "新建时序结点，名字：" + newVerb->name << "，   参数名：" << strJoin(newVerb->paraNames, "，") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//保存起来
	cout << "刚才的三条指令，就是函数体内容。现在，函数内容如下：\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	theWorld.杯子1.content.clear();
	command = Sentence("泡咖啡|杯子1|福爵咖啡冲剂");
	cout << "执行命令：" << command.toString() << endl;
	void* ret=command();
	cout << "执行命令以后，返回值：" << ret << "，杯子1的状态为：\n";
	cout << json_toString(concept_named(杯子1)->toJson()) << endl;
	PAUSE_FOR_READING;

	cout << endl<<endl;
	cout << "==============================================\n";
	cout << "时序抽象测试B，有下面三条时序：\n";
	t1 = { Sentence("放进复印机|复印机1|租房协议"),Sentence("设置份数并复印|复印机1|2"),
		Sentence("取出复印件|复印机1"),Sentence("取出复印原件|复印机1|租房协议") };
	t2 = { Sentence("放进复印机|复印机2|租房协议"),Sentence("设置份数并复印|复印机2|1"),
		Sentence("取出复印件|复印机2"),Sentence("取出复印原件|复印机2|租房协议") };
	t3 = { Sentence("放进复印机|复印机2|考试卷"),Sentence("设置份数并复印|复印机2|3"),
		Sentence("取出复印件|复印机2"),Sentence("取出复印原件|复印机2|考试卷") };
	cout << "时序1:\n" << arrToString(t1) << "\n\n";
	cout << "时序2:\n" << arrToString(t2) << "\n\n";
	cout << "时序3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "程序对3条时序进行比较，得到的抽象时序如下：\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "将时序中的any$复印机替换为参数[0]，any$纸张替换为参数[1]，any$整数替换为参数[2]得到：\n";
	replacementRules = {
		{ concept_of(Copier),0 },{ concept_of(Paper),1 },{ concept_of(int),2 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "复印";
	newVerb->paraTypes = { concept_of(Copier),concept_of(Paper),concept_of(int) };
	newVerb->paraNames = { "复印机","要复印的纸张","份数" };
	cout << "新建时序结点，名字：" + newVerb->name << "，   参数名：" << strJoin(newVerb->paraNames, "，") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//保存起来
	cout << "刚才的三条指令，就是函数体内容。现在，函数内容如下：\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	
	command = Sentence("复印|复印机3|我的毕业证书|2");
	cout << "执行命令：" << command.toString() << endl;
	ret = command();
	cout << "执行命令以后，返回值：" << ret << "，现在整个模拟环境中总共有这些纸：\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Paper)))
		cout << node->name << endl;
	cout << endl;
	PAUSE_FOR_READING;

	cout << "日常生活中，我要让别人帮我复印资料，我只需转告他要复印什么资料、复印几份就行了，【用哪个复印机复印】似乎是个多余参数。\n";
	cout << "在接下来的实验中，我将简化掉这个参数，让计算机选择【任意一台复印机】进行复印。\n";
	cout << endl;

	t1 = { Sentence("复印|复印机2|租房协议|1") };
	t2 = { Sentence("复印|复印机1|公司合同|1") };
	t3 = { Sentence("复印|复印机1|我的毕业证书|2") };
	cout << "时序1:\n" << arrToString(t1) << "\n\n";
	cout << "时序2:\n" << arrToString(t2) << "\n\n";
	cout << "时序3:\n" << arrToString(t3) << "\n\n";
	finalResults = compareAmongTemporalOrders({ t1,t2,t3 });
	cout << "程序对3条时序进行比较，得到的抽象时序如下：\n";
	cout << arrToString(finalResults) << "\n\n";
	cout << "将时序中的any$纸张替换为参数[0]，any$整数替换为参数[1]得到：\n";
	replacementRules = {
		{ concept_of(Paper),0 },{ concept_of(int),1 }
	};
	replaceItems(finalResults, replacementRules);
	cout << arrToString(finalResults) << "\n\n";
	newVerb = new AITemporalOrderNode;
	newVerb->steps = finalResults;
	newVerb->name = "简化版复印";
	newVerb->paraTypes = { concept_of(Paper),concept_of(int) };
	newVerb->paraNames = { "要复印的纸张","份数" };
	cout << "新建时序结点，名字：" + newVerb->name << "，   参数名：" << strJoin(newVerb->paraNames, "，") << "\n";
	theGlobalArea.verbs.push_back(newVerb);//保存起来
	cout << "刚才的三条指令，就是函数体内容。现在，函数内容如下：\n\n";
	cout << newVerb->toString_all() << endl;
	cout << endl;
	PAUSE_FOR_READING;
	command = Sentence("简化版复印|考试卷|2");
	cout << "执行命令：" << command.toString() << endl;
	ret = command();
	cout << "执行命令以后，返回值：" << ret << "，现在整个模拟环境中总共有这些纸：\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Paper)))
		cout << node->name << endl;
	cout << "各个打印机的状态如下所示：\n";
	for (AIConceptNode* node : theGlobalArea.findNounsByClass(concept_of(Copier)))
		cout << node->name+"："<<json_toString(node->toJson()) << endl;
	cout << endl;

	return 0;
}