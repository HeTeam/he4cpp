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
			DEBUG_MSG("%s��ʽ�����ϣ�������->������", p_concept->strValue.c_str());
			return NULL;
		}
		string objName = strArr_objName_propertyName[0];
		string propertyName = strArr_objName_propertyName[1];
		AIConceptNode* obj = theGlobalArea.findNounByName(objName);
		if (NULL == obj)
		{
			DEBUG_MSG("%s��ͷ��ߵĶ����������ڣ�", p_concept->strValue.c_str());
			return NULL;
		}
		map<string, AIConceptNode*> json = obj->toJson();
		if (json.find(propertyName) == json.end())
		{
			DEBUG_MSG("%sû��%s������ԡ�", objName.c_str(), propertyName.c_str());
			return NULL;
		}
		else
			p_concept = json[propertyName];
	}
	return p_concept;
}

bool pred_increased(vector<AIConceptNode*> args) 
{ 
	//�������д���û��ʵ�����壬�����ɾ�����Ļ����ᵼ��releaseģʽ�£����ڱ������Ż����£����н������
	int& a = *new int; a = args.size(); delete &a;

	return false; 
}
bool pred_decreased(vector<AIConceptNode*> args) 
{ 
	//�������д���û��ʵ�����壬�����ɾ�����Ļ����ᵼ��releaseģʽ�£����ڱ������Ż����£����н������
	int& b = *new int;b = args.capacity();delete &b;

	return false; 
}
bool pred_propertyIs(vector<AIConceptNode*> args)
{
	AIConceptNode& subject = *args[0];//���������һ�����󣬲����ǻ����ͳ���
	string propertyName = args[1]->strValue;//������������
	AIConceptNode& valueShouldBe = *args[2];//Ӧ�õ���ʲôֵ
	map<string, AIConceptNode*> json = subject.toJson();//������������JSON����
	if (json.find(propertyName) == json.end())//���û��������ԣ�����FALSE
		return false;
	else
	{
		AIConceptNode& actualValue = *json[propertyName];//ʵ������ֵ
		return actualValue == valueShouldBe;//ע�⣺���ڻ����ͳ���������Ƚϵ������ݶ������ڴ��ַ
	}
}
bool pred_memberHas(vector<AIConceptNode*> args)
{
	//�����������������һ������Ҳ���������硾������->�����������ַ���
	AIConceptNode* p_subject = convertIfIsObjAndPropertyStr(args[0]);
	//����Ǻ��ߣ���Ҫ����������ֵ��ȡ����
	if (NULL == p_subject)
		return false;
	if (p_subject->isConst&& runtime_of(void*) == p_subject->type)
	{
		void* address = p_subject->ptrAddressValue;
		AIConceptNode* nodeWhichAddressMatched = theGlobalArea.findNounByPtrAddress(address);
		if (NULL == nodeWhichAddressMatched)
		{
			DEBUG_MSG("%s��ĳ��void*������ֵ��ָ����ڴ��ַ����û����ȫ�ֱ����еǼ�",
				p_subject->toString().c_str());
			return false;
		}
		else
			p_subject = nodeWhichAddressMatched;
	}
	//ת���������
	AIConceptNode& subject = *p_subject;

	string propertyName = args[1]->strValue;
	
	//������ӵ��Ԫ��(����5����,"����վ��","��->��ǰλ��")
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
		if (strContains(eachStr, "����["))
		{
			string whatInsideTheBrackets = split(split(eachStr, "[")[1], "]")[0];
			int index;
			auto iter=stdFind(paraNames, whatInsideTheBrackets);
			if (paraNames.end() == iter)//����һ��������
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
				if (isString(eachStr)) //�ַ���
					this->args.push_back(new AIConceptNode(removeQuotation(eachStr)));
				else if (isPtrArrayStr(eachStr))//ָ������
				{
					vector<void*> ptrArr = toPtrArr(eachStr);
					this->args.push_back(new AIConceptNode(ptrArr));
				}
				else
				{
					if (strContains(eachStr, "."))//С��
						this->args.push_back(new AIConceptNode(atof(eachStr.c_str())));
					else //����
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
			if (this->insert(actionAndEffect) == 1)//�µ�
				count++;
		}
	}

	return count;
}

bool ConclusionsLogger_VerbCompare::insertCommandAndKnownEffect(Sentence cmd)
{
	vector<ActionAndEffect> actionAndEffects_known=theGlobalArea.findActionAndEffectsByAction(cmd);
	//����Щ��Ӧ��ϵ����Ϊ���ֽ��С����󻯡����������嶯���滻�ɳ��󶯴�
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

	for (auto& each_known : actionAndEffects_known)//��ȫ���������ѯ����
	{
		bool completelyNew = true;
		for (auto& eachPair : this->timesCount) //�Ѿ���¼��
		{
			auto& times = eachPair.second;
			auto& actionPart = eachPair.first.action;//�Ѿ���¼��
			auto effectPart = eachPair.first.effect;//�Ѿ���¼�ģ�����һ��

			if ( effectPart != each_known.effect )//��ɵ�Ӱ�첻һ����ֱ������
				continue;

			effectPart=effectPart.toConcrete(actionPart.args);//����Щʲô������[1]�����滻���������滻��ĳ��XXX

			if (each_known.action == actionPart)
			{
				times++;//����+1
				completelyNew = false;
			}
			else if (each_known.action < actionPart)
			{
				times++;//����������������иõ�
				completelyNew = false;
			}
		}
		if (completelyNew)
			this->timesCount.insert({ each_known ,1 });
	}

	//this->insert(actionAndEffect_known);
	return true;
}