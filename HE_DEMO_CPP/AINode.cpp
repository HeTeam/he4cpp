#include"stdafx.h"
#include"AINode.h"
#include"global.h"
#include"AIHeader.h"

string __concept_string(AIConceptNode* node)
{
	string str;
	str += node->name;
	str += "(";
	str += node->type->runtimeName;
	str += "):";
	map<string, AIConceptNode*> json = node->toJson();
	string jsonString = json_toString(json);
	str += jsonString;

	json_GC(json);

	return str;
}

string concept_string(AIConceptNode* pNode)
{
	if (pNode->ptr&&pNode->type)
		return __concept_string(pNode);
	else
	{
		vector<string> strArr;
		for (auto eachConcrete : pNode->concreteNodes)
			strArr.push_back(eachConcrete->name);
		return pNode->name + ": {" + strJoin(strArr, ",") + "}";
	}
}

void json_GC(map<string, AIConceptNode*> json)
{
	for (auto& eachPair : json)
	{
		AIConceptNode* node = eachPair.second;
		if (node->isConst)
			delete node;
	}
}

bool conceptNodeCopy(AIConceptNode& dest, AIConceptNode& src)
{
	if (false == (dest.isConst&&src.isConst))
		return false;

	dest.integerValue = src.integerValue;
	dest.realValue = src.realValue;
	dest.strValue = src.strValue;
	dest.type = src.type;
	
	if (runtime_of(int) == src.type)
	{
		dest.ptr = &dest.integerValue;
		return true;
	}
	else if (runtime_of(double) == src.type)
	{
		dest.ptr = &dest.realValue;
		return true;
	}
	else if (runtime_of(string) == src.type)
	{
		dest.ptr = &dest.strValue;
		return true;
	}
	else if (runtime_of(void*) == src.type)
	{
		dest.ptr = &dest.ptrAddressValue;//ע������ط���Ҫ����&
		return true;
	}
	else if (runtime_of(vector<void*>) == src.type)
	{
		dest.ptr = &dest.ptrArrayValue;
		return true;
	}
	
	return false;
}

vector<AIConceptNode*> AIConceptNode::getAllAbstractNodes()
{
	if (this->isConst)
		return{theGlobalArea.findNounByPtrAddress(this->type)};

	vector<AIConceptNode*> results;
	for (AIConceptNode* abs : absNodes)
	{
		pushBackIfNew(results, abs);
		for (AIConceptNode* moreAbs : abs->getAllAbstractNodes())
			pushBackIfNew(results, moreAbs);
	}
	return results;
}

map<string, AIConceptNode*> AIConceptNode::toJson()
{
	if (this->type == NULL)
		return map<string, AIConceptNode*>();

	map<string, AIConceptNode*> json;
	Person aPerson;
	for (auto& pair_name_offset : this->type->map_offset)
	{
		string fieldName = pair_name_offset.first;
		int offset = pair_name_offset.second;
		Class* fieldType = this->type->map_fieldType[fieldName];
		if (runtime_of(int) == fieldType)
		{
			auto valueFromStruct = *(int*)((char*)ptr + offset);
			json.insert({ fieldName,new AIConceptNode(valueFromStruct) });
		}
		else if (runtime_of(bool) == fieldType)
		{
			auto valueFromStruct = *(bool*)((char*)ptr + offset);
			json.insert({ fieldName,new AIConceptNode(valueFromStruct) });
		}
		else if (runtime_of(double) == fieldType)
		{
			auto valueFromStruct = *(double*)((char*)ptr + offset);
			json.insert({ fieldName,new AIConceptNode(valueFromStruct) });
		}
		else if (runtime_of(string) == fieldType)
		{
			auto valueFromStruct = *(string*)((char*)ptr + offset);
			json.insert({ fieldName,new AIConceptNode(valueFromStruct) });
		}
		else if (runtime_of(vector<void*>) == fieldType)
		{
			auto valueFromStruct = *(vector<void*>*)((char*)ptr + offset);
			json.insert({ fieldName,new AIConceptNode(valueFromStruct) });
		}
		else if (runtime_of(void*) == fieldType)
		{
			void* valueFromStruct = *(void**)((char*)ptr + offset);
			AIConceptNode* globalConcept = theGlobalArea.findNounByPtrAddress(valueFromStruct);
			if(globalConcept)
				json.insert({ fieldName,globalConcept });
		}
	}
	return json;
}

bool AIConceptNode::operator< (AIConceptNode& other)
{
	if (*this == other)
		return false;

	if (this->isConst)
	{
		AIConceptNode& thisType = *theGlobalArea.findNounByPtrAddress(this->type);
		return thisType <= other;
	}

	for (AIConceptNode* p_eachAbs : this->absNodes)
	{
		AIConceptNode& eachAbs = *p_eachAbs;
		if (eachAbs == other)
			return true;
		if (eachAbs < other)
			return true;
	}
	
	return false;
}

string AIConceptNode::toString()
{
	if ("" != this->name)
		return this->name;

	if (&theClasses.types[Classes::start_jsonSupportedClassID] <= type&&
		type <= &theClasses.types[Classes::end_jsonSupportedClassID])
		return *(string*)ptr;

	if (isConst)
	{
		if (runtime_of(int) == type|| runtime_of(bool) == type)
			return to_string(integerValue);
		if (runtime_of(double) == type)
			return to_string(realValue);
		if (runtime_of(string) == type)
			return "\""+strValue+ "\"";
		if (runtime_of(void*) == type)
		{
			AIConceptNode* conceptNodeOfThisPtr = theGlobalArea.findNounByPtrAddress(ptrAddressValue);
			if (conceptNodeOfThisPtr)
				return conceptNodeOfThisPtr->name;
			else
				return to_string((unsigned long long)ptrAddressValue);
		}
		if (runtime_of(vector<void*>)== type)
		{
			string strResult = "{";
			for (unsigned i=0;i<ptrArrayValue.size();i++)
			{
				void* each = ptrArrayValue[i];
				AIConceptNode* conceptNodeOfThisPtr = theGlobalArea.findNounByPtrAddress(each);
				if (conceptNodeOfThisPtr)
					strResult+= conceptNodeOfThisPtr->name;
				else
					strResult+=to_string((unsigned long long)ptrAddressValue);
				if (ptrArrayValue.size() - 1 != i)
					strResult += ",";
			}
			strResult += "}";
			return strResult;
		}
	}
	else
	{
		AIConceptNode* conceptNodeOfThisPtr = theGlobalArea.findNounByPtrAddress(this->ptr);
		if (conceptNodeOfThisPtr)
			return conceptNodeOfThisPtr->name;
		else
			return to_string((unsigned long long)ptr);
	}

	return "";
}

void AITemporalOrderNode::setParaTypes(vector<Class*> paraTypes)
{
	for (Class* paraType : paraTypes)
	{
		AIConceptNode* node = theGlobalArea.findNounByPtrAddress(paraType);
		if (!node)
			DEBUG_MSG("��paraTypes��Ա������������˿�ָ�룡");
		this->paraTypes.push_back(node);
	}
}

int Sentence::findAny()
{
	vector<AIConceptNode*> basicTypes = { concept_of(string),concept_of(int),concept_of(double),
		concept_of(void*),concept_of(vector<void*>) };
	for (int i = 0; i < args.size(); i++)
		if (class_NounDescriptionType == args[i].descriptionType &&false==stdFindExist(basicTypes,args[i].ptr) )
			return i;
	return -1;
}

bool Sentence::checkBeforeExecute()
{
	for (auto& cond : theGlobalArea.findConditionsOfAction(*this))
	{
		auto concreteCondition = cond.toConcrete(this->args);
		if (concreteCondition.isAllConcrete() == false)
			continue;
		if (concreteCondition.judge() == false)
			return false;
	}
	return true;
}

void* Sentence::execute(vector<AIConceptNode*>& inputArgs)
{
	if (this->isAllConcrete()) //�������漰�Ĳ���ȫ�Ǿ����
	{
		if (this->checkBeforeExecute() == false)
			return (void*)false;
		vector<AIConceptNode*> conArgs;
		for (NounDescription& nounDes : this->args)
			conArgs.push_back(nounDes.ptr);
		return verb->execute(conArgs);
	}
	else
	{
		Sentence concreteSentence = this->toConcrete(inputArgs);
		if (concreteSentence.isAllConcrete()==false)//����������������滻
		{
			vector<AIConceptNode*> choices = theGlobalArea.findNounsByClassWithRestriction(concreteSentence);
			if (choices.empty())
				return false;
			concreteSentence=concreteSentence.replaceAbstractItem(randomChoose(choices));
		}

		if (concreteSentence.checkBeforeExecute() == false)
			return (void*)false;
		return concreteSentence.execute(inputArgs);
	}
}

Sentence Sentence::toConcrete(vector<AIConceptNode*>& inputArgs)
{
	Sentence concreteStep;
	//concreteStep.parent = this->parent;
	concreteStep.verb = this->verb;
	for (NounDescription& originalArg : this->args)
	{
		if (concrete_NounDescriptionType == originalArg.descriptionType)
			concreteStep.args.push_back(originalArg);
		else if (paraIndex_NounDescriptionType == originalArg.descriptionType)
		{
			NounDescription concreteOne;
			concreteOne.descriptionType = concrete_NounDescriptionType;
			concreteOne.ptr = inputArgs[originalArg.index];//��ʱ�������������ջ���
			concreteStep.args.push_back(concreteOne);
		}
		else if (class_NounDescriptionType == originalArg.descriptionType)
		{
			vector<AIConceptNode*> basicSingleTypes_canRandomlyGenerated = { concept_of(string),concept_of(int),
				concept_of(double) /*,concept_of(void*),concept_of(bool)*/ };
			if (stdFindExist(basicSingleTypes_canRandomlyGenerated, originalArg.ptr))
			{
				NounDescription concreteOne;
				concreteOne.descriptionType = concrete_NounDescriptionType;
				if (concept_of(int) == originalArg.ptr)
					concreteOne.ptr = new AIConceptNode(rand() % 100 + 1);//1��100
				if (concept_of(double) == originalArg.ptr)
					concreteOne.ptr = new AIConceptNode((rand() % 1000 + 1.0) / 10.0);//1.0��100.0
				if (concept_of(string) == originalArg.ptr)
					concreteOne.ptr = new AIConceptNode(randString());
				concreteStep.args.push_back(concreteOne);
			}
			else
				concreteStep.args.push_back(originalArg);
		}
	}
	return concreteStep;
}

vector<Sentence> AITemporalOrderNode::toConcreteCommands(vector<AIConceptNode*>& inputArgs)
{
	vector<Sentence> concreteSteps;
	for (Sentence& originalStep : steps)
	{
		Sentence concreteStep = originalStep.toConcrete(inputArgs);
		concreteSteps.push_back(concreteStep);
	}
	return concreteSteps;
}

void* AITemporalOrderNode::execute(vector<AIConceptNode*>& inputArgs)
{
	if (getType() == funcPtr_VerbType)
	{
		if (isArgTypeMatch(inputArgs, this->paraTypes))
		{
			if (Sentence(this, inputArgs).checkBeforeExecute() == false)
				return (void*)false;//�����Լ�� δͨ��
			return funcPtr(inputArgs);
		}
		else
		{
			DEBUG_MSG("%s:�������Ͳ�ƥ��", this->name.c_str());
			return (void*)false;
		}
	}
	else if (getType() == instructionList_VerbType)
	{
		auto concreteSteps = toConcreteCommands(inputArgs);
		//if (Sentence::checkAllBeforeExecute(concreteSteps) == false)
		//	return (void*)false;//�����Լ�� δͨ��

		for (Sentence& step : concreteSteps)
		{
			if(step.checkBeforeExecute()==false)
				return (void*)false;//�����Լ��δͨ������ִ��ֱ�ӷ���false
			void* ret_step = step.execute();
			if ((void*)false == ret_step)//ִ�к�õ���ʧ�ܵĽ��
				return (void*)false;
		}
		return (void*)true;
	}
	else if(getType()== abstract_VerbType)
	{
		vector<AITemporalOrderNode*> choices;
		for (AITemporalOrderNode* concreteVerb : this->concreteNodes)
		{
			if (concreteVerb->getType() != abstract_VerbType)
				choices.push_back(concreteVerb);
		}
		if (choices.empty())
			return (void*)false;

		return randomChoose(choices)->execute(inputArgs);
	}

	return (void*)false;
}

NounDescription::NounDescription(Class* cls)
{
	this->ptr = theGlobalArea.findNounByPtrAddress(cls);
	this->descriptionType = class_NounDescriptionType;
}

string Sentence::toString() const
{
	if (!verb)
		return "<�������>";
	vector<string> strArray = { verb->name };
	for (NounDescription arg : args)
		strArray.push_back(arg.toString());
	return strJoin(strArray, "|");
}

string Sentence::toString(vector<string> parentParaNames)
{
	if (!verb)
		return "<�������>";
	vector<string> strArray = { verb->name };
	for (NounDescription arg : args)
		strArray.push_back(arg.toString(parentParaNames));
	return strJoin(strArray, "|");
}

bool isArgTypeMatch(vector<AIConceptNode*> args, vector<AIConceptNode*> paraTypes)
{
	if (args.size() != paraTypes.size())
		return false;

	for (unsigned i = 0; i < args.size(); i++)
	{
		AIConceptNode& arg = *args[i];
		AIConceptNode& paraType = *paraTypes[i];
		if (false == (arg <= paraType))
			return false;
	}

	return true;
}

string json_toString(map<string, AIConceptNode*> json)
{
	string str = "{";
	unsigned count = 0;
	for (auto& eachPair : json)
	{
		str += eachPair.first + ":" + eachPair.second->toString();
		if (count + 1 < json.size())
			str += ",";
		count++;
	}
	str += "}";
	return str;
}

vector<NounDescription> nounDescriptions_toAbstract(vector<NounDescription>& originals, vector<NounDescription>& inputArgs)
{
	vector<NounDescription> results;
	for (NounDescription& originalArg : originals)
	{
		NounDescription abstractArg;
		static AIConceptNode* lambdaVar_ptrOfThisArg;
		lambdaVar_ptrOfThisArg = originalArg.ptr;
		auto iter = std::find_if(inputArgs.begin(), inputArgs.end(), [](NounDescription& each)->bool {
			return each.ptr == lambdaVar_ptrOfThisArg;//���ֱ�ӱȽϵ�ַ�����ˣ�û�г�����
		});
		if (inputArgs.end() == iter)
			abstractArg = originalArg;
		else
		{
			int index = iter - inputArgs.begin();
			abstractArg.descriptionType = paraIndex_NounDescriptionType;
			abstractArg.index = index;
		}
		results.push_back(abstractArg);
	}
	return results;
}

vector<NounDescription> toDecriptions(vector<AIConceptNode*>& conceptNodes)
{
	vector<NounDescription> results;
	for (AIConceptNode* each : conceptNodes)
		results.push_back(NounDescription(each));
	return results;
}
vector<AIConceptNode*>  toConceptNodes(vector<NounDescription>& descriptions)
{
	vector<AIConceptNode*> results;
	for (NounDescription& each : descriptions)
		results.push_back(each.ptr);
	return results;
}

NounDescription NounDescription::toClass()
{
	NounDescription result;
	if (this->descriptionType == none_NounDescriptionType || NULL == ptr)
		return result;

	if (ptr->isConst)
	{
		result.descriptionType = class_NounDescriptionType;
		result.ptr = theGlobalArea.findNounByPtrAddress(ptr->type);
		return result;
	}
	else
	{
		if (ptr->absNodes.empty())
			return result;
		result.descriptionType = class_NounDescriptionType;
		result.ptr = ptr->absNodes[0];
		return result;
	}
}

bool AITemporalOrderNode::operator< (AITemporalOrderNode& other)
{
	if (*this == other)
		return false;

	for (auto p_eachAbs : this->absNodes)
	{
		auto& eachAbs = *p_eachAbs;
		if (eachAbs == other)
			return true;
		if (eachAbs < other)
			return true;
	}

	return false;
}

Sentence::Sentence(string str)
{
	vector<string> strArr = split(str, "|");//��|Ϊ�ָ��������ַ������зָ�
	string verbName = strArr[0];//��һ���Ƕ�����
	this->verb = theGlobalArea.findVerbByName(verbName);//�������ֲ��Ҷ���
	for (size_t i = 1; i < strArr.size(); i++)
	{
		if (isString(strArr[i]))
			this->args.push_back(NounDescription(
				new AIConceptNode(removeQuotation(strArr[i])) ) );
		else if (strContains(strArr[i], "any$"))
		{
			const int beginning= sizeof("any$")-1;
			string typeName = strArr[i].substr(beginning);
			AIConceptNode* typeNode = theGlobalArea.findNounByName(typeName);
			NounDescription description;
			description.descriptionType = class_NounDescriptionType;
			description.ptr = typeNode;
			this->args.push_back(description);
		}
		else
		{
			string conceptName = strArr[i];
			AIConceptNode* concept = theGlobalArea.findNounByName(conceptName);//�������ֲ�������
			if(concept)
				this->args.push_back(NounDescription(concept));//��װһ�㣬�ٷŽ�ȥ
			else
			{
				if (strContains(conceptName, "."))
					this->args.push_back(NounDescription(
						new AIConceptNode(atof(conceptName.c_str())) ) );
				else
					this->args.push_back(NounDescription(
						new AIConceptNode(atoi(conceptName.c_str()))));
			}
		}
	}
}

bool Sentence::operator<(const Sentence& other) const
{
	bool eq_or_belongs = ((this->verb == other.verb) || (this->verb->belongsTo(*other.verb)));
	if (false == eq_or_belongs)
		return false;
	if (this->args.size() != other.args.size())
		return false;

	bool completelySame = true;
	for (unsigned i = 0; i < args.size(); i++)
	{
		if (this->args[i] <= other.args[i])
		{
			if (this->args[i] != other.args[i]) //Ҳ���ǡ�
				completelySame = false;
			//����ǵ��ڣ���û��˼��
		}
		else
			return false;//�����ݵ�����
	}

	return (!completelySame) || (!(*this->verb == *other.verb));
}