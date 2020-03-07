#include"stdafx.h"
#include"Proposition.h"
#include"global.h"

AIConceptNode* convertIfIsObjAndPropertyStr(AIConceptNode* p_concept)
{
	if (p_concept->isConst && runtime_of(string) == p_concept->type)
	{
		vector<string> strArr_objName_propertyName = split(p_concept->strValue, "->");
		if (strArr_objName_propertyName.size() < 2)
		{
			DEBUG_MSG("%s格式不符合：对象名->属性名", p_concept->strValue.c_str());
			return NULL;
		}
		string objName = strArr_objName_propertyName[0];
		string propertyName = strArr_objName_propertyName[1];
		AIConceptNode* obj = theGlobalArea.findNounByName(objName);
		if (NULL == obj)
		{
			DEBUG_MSG("%s箭头左边的对象名不存在！", p_concept->strValue.c_str());
			return NULL;
		}
		map<string, AIConceptNode*> json = obj->toJson();
		if (json.find(propertyName) == json.end())
		{
			DEBUG_MSG("%s没有%s这个属性。", objName.c_str(), propertyName.c_str());
			return NULL;
		}
		else
			p_concept = json[propertyName];
	}
	return p_concept;
}

bool pred_increased(vector<AIConceptNode*> args) 
{ 
	//下面这行代码没有实际意义，但如果删除它的话，会导致release模式下（由于编译器优化导致）运行结果错误
	int& a = *new int; a = args.size(); delete &a;

	return false; 
}
bool pred_decreased(vector<AIConceptNode*> args) 
{ 
	//下面这行代码没有实际意义，但如果删除它的话，会导致release模式下（由于编译器优化导致）运行结果错误
	int& b = *new int;b = args.capacity();delete &b;

	return false; 
}
bool pred_propertyIs(vector<AIConceptNode*> args)
{
	AIConceptNode& subject = *args[0];//主语，必须是一个对象，不能是基本型常量
	string propertyName = args[1]->strValue;//参数：属性名
	AIConceptNode& valueShouldBe = *args[2];//应该等于什么值
	map<string, AIConceptNode*> json = subject.toJson();//把主语对象拆解成JSON数组
	if (json.find(propertyName) == json.end())//如果没有这个属性，返回FALSE
		return false;
	else
	{
		AIConceptNode& actualValue = *json[propertyName];//实际属性值
		return actualValue == valueShouldBe;//注意：对于基本型常量，这里比较的是内容而不是内存地址
	}
}
bool pred_memberHas(vector<AIConceptNode*> args)
{
	//主语（待定），可能是一个对象，也可能是形如【对象名->属性名】的字符串
	AIConceptNode* p_subject = convertIfIsObjAndPropertyStr(args[0]);
	//如果是后者，就要把它的属性值提取出来
	if (NULL == p_subject)
		return false;
	if (p_subject->isConst&& runtime_of(void*) == p_subject->type)
	{
		void* address = p_subject->ptrAddressValue;
		AIConceptNode* nodeWhichAddressMatched = theGlobalArea.findNounByPtrAddress(address);
		if (NULL == nodeWhichAddressMatched)
		{
			DEBUG_MSG("%s的某个void*型属性值所指向的内存地址，并没有在全局变量中登记",
				p_subject->toString().c_str());
			return false;
		}
		else
			p_subject = nodeWhichAddressMatched;
	}
	//转换后的主语
	AIConceptNode& subject = *p_subject;

	string propertyName = args[1]->strValue;
	
	//举例：拥有元素(地铁5号线,"经过站点","我->当前位置")
	AIConceptNode* node_elementSholdBeExist = convertIfIsObjAndPropertyStr(args[2]);
	if (NULL == node_elementSholdBeExist)
		return false;
	void* elementShouldBeExist = node_elementSholdBeExist->ptr;
	map<string, AIConceptNode*> json = subject.toJson();
	if (json.find(propertyName) == json.end())
		return false;

	vector<void*> arr = json[propertyName]->ptrArrayValue;
	if (stdFindExist(arr, elementShouldBeExist))
		return true;

	if (node_elementSholdBeExist->isAbstract() == false)
		return false;

	AIConceptNode& type_shouldBelongsTo = *node_elementSholdBeExist;

	for (void* p : arr)
	{
		AIConceptNode* node = theGlobalArea.findNounByPtrAddress(p);
		if (node == NULL)
			continue;
		if (*node < type_shouldBelongsTo)
			return true;
	}

	return false;
}

Proposition::Proposition(string str,vector<string> paraNames)
{
	vector<string> strArr = split(str, "(");
	string predicateName = strArr[0];
	this->predicate = string_to_predicate(predicateName);
	string str_allArgs = split(strArr[1], ")")[0];
	vector<string> strArr_allArgs = split(str_allArgs, ",");
	vector<AIConceptNode*> args;
	for (string eachStr : strArr_allArgs)
	{
		if (strContains(eachStr, "参数["))
		{
			string whatInsideTheBrackets = split(split(eachStr, "[")[1], "]")[0];
			int index;
			auto iter=stdFind(paraNames, whatInsideTheBrackets);
			if (paraNames.end() == iter)//不是一个参数名
				index = atoi(whatInsideTheBrackets.c_str());
			else
				index = iter - paraNames.begin();
			NounDescription nounDes;
			nounDes.descriptionType = paraIndex_NounDescriptionType;
			nounDes.index = index;
			this->args.push_back(nounDes);
		}
		else
		{
			AIConceptNode* noun = theGlobalArea.findNounByName(eachStr);
			if (noun)
				this->args.push_back(noun);
			else
			{
				if (isString(eachStr)) //字符串
					this->args.push_back(new AIConceptNode(removeQuotation(eachStr)));
				else if (isPtrArrayStr(eachStr))//指针数组
				{
					vector<void*> ptrArr = toPtrArr(eachStr);
					this->args.push_back(new AIConceptNode(ptrArr));
				}
				else
				{
					if (strContains(eachStr, "."))//小数
						this->args.push_back(new AIConceptNode(atof(eachStr.c_str())));
					else //整数
						this->args.push_back(new AIConceptNode(atoi(eachStr.c_str())));
				}
			}
		}
	}
}

Proposition::Proposition(string str)
{
	*this=Proposition(str, vector<string>());
}

vector<Proposition> getConclusionsByComparingBeforeAndAfter(AIConceptNode* thisObject,
	map<string, AIConceptNode*> before, map<string, AIConceptNode*> after)
{
	vector<Proposition> conclusions;
	for (auto& eachPair : before)
	{
		string propertyName = eachPair.first;
		AIConceptNode* beforeNode = eachPair.second;
		AIConceptNode* afterNode = after[propertyName];
		Class* dataType = beforeNode->type;
		bool isConst = beforeNode->isConst;
		if (isConst)
		{
			if (runtime_of(int) == dataType || runtime_of(double) == dataType)
			{
				if (runtime_of(int) == dataType)
				{
					int before_number = beforeNode->integerValue;
					int after_number = afterNode->integerValue;
					if (before_number < after_number)
						conclusions.push_back(Proposition(pred_increased, ConArr{ thisObject, new AIConceptNode(propertyName) }));
					else if (before_number > after_number)
						conclusions.push_back(Proposition(pred_decreased, ConArr{ thisObject, new AIConceptNode(propertyName) }));
				}
				else
				{
					double before_number = beforeNode->realValue;
					double after_number = afterNode->realValue;
					if (before_number < after_number)
						conclusions.push_back(Proposition(pred_increased, ConArr{ thisObject, new AIConceptNode(propertyName) }));
					else if (before_number > after_number)
						conclusions.push_back(Proposition(pred_decreased, ConArr{ thisObject, new AIConceptNode(propertyName) }));
				}
			}
			else if (runtime_of(vector<void*>) == dataType)
			{
				vector<void*>& before_array = *(vector<void*>*)beforeNode->ptr;
				vector<void*>& after_array = *(vector<void*>*)afterNode->ptr;
				for (void* after_each : after_array)
				{
					static void* _after_each;
					_after_each = after_each;
					auto iter = std::find_if(before_array.begin(), before_array.end(), [](void* before_each) {
						return before_each == _after_each;
					});
					bool isNewOne = (iter == before_array.end());
					if (isNewOne)
					{
						AIConceptNode* noun = theGlobalArea.findNounByPtrAddress(after_each);
						if (noun)
							conclusions.push_back(Proposition(pred_memberHas, ConArr{ thisObject, new AIConceptNode(propertyName), noun }));
					}
				}
			}
			else if (runtime_of(void*) == dataType)
			{
				void* before_ptr = beforeNode->ptr;
				void* after_ptr = afterNode->ptr;
				if (before_ptr != after_ptr)
				{
					AIConceptNode* noun = theGlobalArea.findNounByPtrAddress(after_ptr);
					if (noun)
						conclusions.push_back(Proposition(pred_propertyIs, ConArr{ thisObject, new AIConceptNode(propertyName), noun }));
				}
			}
		}
		else
		{
			if (beforeNode != afterNode)
			{
				conclusions.push_back(Proposition(pred_propertyIs, ConArr{ thisObject, new AIConceptNode(propertyName), afterNode }));
			}
		}
	}
	return conclusions;
}

vector<Proposition> getAllConclusionsByComparingBeforeAndAfter(
	map<AIConceptNode*, map<string, AIConceptNode*>> before,
	map<AIConceptNode*, map<string, AIConceptNode*>> after)
{
	vector<Proposition> results;
	for (auto eachPair : before)
	{
		AIConceptNode* eachConceptNoun = eachPair.first;
		auto json_before = eachPair.second;
		auto json_after = after[eachConceptNoun];
		auto each_conclusions = getConclusionsByComparingBeforeAndAfter(eachConceptNoun,json_before, json_after);
		results.insert(results.begin(), each_conclusions.begin(), each_conclusions.end());
	}
	return results;
}

int ConclusionsLogger::executeAndGetConclusions(Sentence cmd)
{
	int count = 0;
	if (cmd.isAllConcrete() == false)
		return 0;

	vector<Sentence> commands = generateAllValidExprOfAction(cmd);

	auto before_statuses=theGlobalArea.getCurrentStatuses_jsons();
	if (!cmd.execute())
		return 0;
	auto after_statuses = theGlobalArea.getCurrentStatuses_jsons();

	vector<Proposition> concreteEffects=getAllConclusionsByComparingBeforeAndAfter(before_statuses, after_statuses);
	vector<Proposition> allEffects;
	for (Proposition& each : concreteEffects)
	{
		vector<Proposition> effects = generateAllValidExprOfEffect(each,cmd.args);
		allEffects.insert(allEffects.end(), effects.begin(), effects.end());
	}

	for (Sentence& command : commands)
	{
		for (Proposition& effect : allEffects)
		{
			ActionAndEffect actionAndEffect(command, effect);
			if (this->insert(actionAndEffect) == 1)//新的
				count++;
		}
	}

	return count;
}

bool ConclusionsLogger_VerbCompare::insertCommandAndKnownEffect(Sentence cmd)
{
	vector<ActionAndEffect> actionAndEffects_known=theGlobalArea.findActionAndEffectsByAction(cmd);
	//对这些对应关系的行为部分进行“抽象化”处理，将具体动词替换成抽象动词
	vector<ActionAndEffect> actionAndEffects_abs;
	for (ActionAndEffect& each_old : actionAndEffects_known)
	{
		for (auto absVerb : each_old.action.verb->absNodes)
		{
			ActionAndEffect copied = each_old;
			copied.action.verb = absVerb;
			actionAndEffects_abs.push_back(copied);
		}
	}
	for (auto& eachAbs : actionAndEffects_abs)
		actionAndEffects_known.push_back(eachAbs);

	for (auto& each_known : actionAndEffects_known)//从全局数组里查询到的
	{
		bool completelyNew = true;
		for (auto& eachPair : this->timesCount) //已经记录的
		{
			auto& times = eachPair.second;
			auto& actionPart = eachPair.first.action;//已经记录的
			auto effectPart = eachPair.first.effect;//已经记录的，拷贝一份

			if ( effectPart != each_known.effect )//造成的影响不一样，直接跳过
				continue;

			effectPart=effectPart.toConcrete(actionPart.args);//把那些什么“参数[1]”都替换掉，可以替换成某类XXX

			if (each_known.action == actionPart)
			{
				times++;//次数+1
				completelyNew = false;
			}
			else if (each_known.action < actionPart)
			{
				times++;//抽象的描述，会命中该点
				completelyNew = false;
			}
		}
		if (completelyNew)
			this->timesCount.insert({ each_known ,1 });
	}

	//this->insert(actionAndEffect_known);
	return true;
}