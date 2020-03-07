#pragma once

#include"AINode.h"
#include<vector>
#include<map>
#include<set>
using namespace std;

bool pred_increased(vector<AIConceptNode*>);
bool pred_decreased(vector<AIConceptNode*>);
bool pred_propertyIs(vector<AIConceptNode*>);
bool pred_memberHas(vector<AIConceptNode*>);

typedef bool(*PredicatePointer)(vector<AIConceptNode*>);

inline string predicate_toString(PredicatePointer ptr)
{
	if (pred_increased == ptr)
		return "增大";
	if (pred_decreased == ptr)
		return "减小";
	if (pred_propertyIs == ptr)
		return "值等于或变为";
	if (pred_memberHas == ptr)
		return "新增或拥有元素";

	return "<未知谓词>";
}
inline PredicatePointer string_to_predicate(string str)
{
	if ("增大" == str)
		return pred_increased;
	if ("减小" == str)
		return pred_decreased;
	if ("值等于或变为" == str)
		return pred_propertyIs;
	if ("新增或拥有元素" == str)
		return pred_memberHas;

	return NULL;
}

//static AIConceptNode* lambdaVar_ptrOfThisArg;

class Proposition
{
public:
	PredicatePointer predicate = NULL;
	vector<NounDescription> args;
	//AITemporalOrderNode* parent = NULL;;
	
	Proposition(){}
	Proposition(PredicatePointer predicate,vector<AIConceptNode*> args_conceptNode)
	{
		this->predicate = predicate;
		for (AIConceptNode* conceptNode : args_conceptNode)
			this->args.push_back(NounDescription(conceptNode));
	}
	Proposition(PredicatePointer predicate, vector<NounDescription> args)
	{
		this->predicate = predicate;
		this->args = args;
	}
	Proposition(string str);
	Proposition(string str, vector<string> paraNames);
	/*Proposition toConcrete(vector<AIConceptNode*>& inputArgs)
	{
		Sentence temp_as_sentence;
		temp_as_sentence.args = this->args;
		Sentence abstract_as_sentence = temp_as_sentence.toConcrete(inputArgs);
		Proposition result;
		result.predicate = this->predicate;
		result.args = abstract_as_sentence.args;
		return result;
	}*/

	bool isValid() { return NULL!=this->predicate; }

	Proposition toConcrete(vector<NounDescription> inputArgs)
	{
		Proposition concreteProposition;
		//concreteStep.parent = this->parent;
		concreteProposition.predicate = this->predicate;
		for (NounDescription& originalArg : this->args)
		{
			if (concrete_NounDescriptionType == originalArg.descriptionType)
				concreteProposition.args.push_back(originalArg);
			else if (paraIndex_NounDescriptionType == originalArg.descriptionType)
			{
				NounDescription concreteOne;
				concreteOne.descriptionType = inputArgs[originalArg.index].descriptionType; //原来是concrete_NounDescriptionType;
				concreteOne.ptr = inputArgs[originalArg.index].ptr;//暂时不考虑垃圾回收机制
				concreteProposition.args.push_back(concreteOne);
			}
			else if (class_NounDescriptionType == originalArg.descriptionType)
			{
				concreteProposition.args.push_back(originalArg);
			}
		}
		return concreteProposition;
	}
	Proposition toConcrete(vector<AIConceptNode*> inputArgs)
	{
		vector<NounDescription> descriptions;
		for (AIConceptNode* arg : inputArgs)
			descriptions.push_back(NounDescription(arg));
		return toConcrete(descriptions);
	}

	Proposition toAbstract(vector<NounDescription> argsOfAction)
	{
		Proposition result;
		result.predicate = predicate;
		//result.parent = parent;
		result.args = nounDescriptions_toAbstract(this->args, argsOfAction);
		return result;
	}
	string toString(vector<string> paraNamesOfParentVerb) const
	{
		string str = predicate_toString(predicate) + "(";

		vector<string> strArr_args;
		for (auto arg : args)
			strArr_args.push_back(arg.toString(paraNamesOfParentVerb));

		str += strJoin(strArr_args, ",");
		str += ")";
		return str;
	}
	string toString() const
	{
		return toString(vector<string>());
	}
	bool isAllConcrete()
	{
		for (NounDescription& arg : args)
			if (arg.descriptionType != concrete_NounDescriptionType)
				return false;
		return true;
	}
	bool judge()
	{
		if (!isAllConcrete())
		{
			DEBUG_MSG("这不是一个具体命题！");
			return false;
		}
		vector<AIConceptNode*> args_conceptNode;
		for (NounDescription& arg : args)
			args_conceptNode.push_back(arg.ptr);

		return predicate(args_conceptNode);
	}
	bool operator==(const Proposition& other)const
	{
		if (this->predicate != other.predicate)
			return false;
		if (this->args.size() != other.args.size())
			return false;
		for (unsigned i = 0; i < args.size(); i++)
			if ( this->args[i] != other.args[i])
				return false;
		return true;
	}
	bool operator!=(const Proposition& other)const { return !(*this == other); }
	bool operator> (Proposition& other)
	{
		Sentence this_as_sentence, other_as_sentence;
		//this_as_sentence.verb = (AITemporalOrderNode*)this->predicate;
		this_as_sentence.args = this->args;
		//other_as_sentence.verb = (AITemporalOrderNode*)other.predicate;
		other_as_sentence.args = other.args;
		if (this->predicate == other.predicate)
			other_as_sentence.verb = this_as_sentence.verb = new AITemporalOrderNode;
		else
			return false;

		bool cmpReturn= this_as_sentence > other_as_sentence;
		delete this_as_sentence.verb;
		return cmpReturn;
	}
	bool operator>=(Proposition& other)
	{
		return *this > other || *this == other;
	}
};

class ActionAndEffect
{
public:
	Sentence action;
	Proposition effect;
	ActionAndEffect() {}
	ActionAndEffect(Sentence action, Proposition effect)
	{
		this->action = action;
		this->effect = effect;
	}
	bool operator==(const ActionAndEffect& other)const
	{
		return this->action == other.action&&this->effect == other.effect;
	}
	bool operator!=(const ActionAndEffect& other)const {return !(*this == other);}
	bool operator<(const ActionAndEffect& other)const 
	{ 
		return this->toString()<other.toString();
	}
	bool operator<=(const ActionAndEffect& other) const
	{
		return *this == other || *this < other;
	}
	string toString() const 
	{ 
		vector<string> paraNames_action;
		if (action.verb->isParaNamesValid())
			paraNames_action = action.verb->paraNames;
		return action.toString() + "===>" + effect.toString(paraNames_action);
	}
	bool isValid() { return action.isValid() && effect.isValid(); }
};

class ConditionAndAction:public ActionAndEffect
{
public:
	Proposition& condition() { return effect; }//改名
	ConditionAndAction(){}
	ConditionAndAction(Proposition condition, Sentence action)
	{
		this->effect = condition;
		this->action = action;
	}
	string toString() const
	{
		vector<string> paraNames_action;
		if (action.verb->isParaNamesValid())
			paraNames_action = action.verb->paraNames;
		return action.toString() + "<===" + effect/*其实是条件*/.toString(paraNames_action);
	}
};

inline vector<Sentence> generateAllValidExprOfAction(Sentence concreteForm)
{
	vector<Sentence> results;
	if (concreteForm.args.size() == 1)
	{
		results.push_back(concreteForm);
		//Sentence absForm = concreteForm;
		//NounDescription arg_abstract = concreteForm.args[0].toClass();
		vector<NounDescription> args_abstract;
		if (concreteForm.args[0].descriptionType == class_NounDescriptionType)
			args_abstract = { concreteForm.args[0] };
		else if(concreteForm.args[0].descriptionType == concrete_NounDescriptionType)
		{
			vector<AIConceptNode*> abstractNodes = 
				concreteForm.args[0].ptr->getAllAbstractNodes();
			for (AIConceptNode* abstractNode : abstractNodes)
			{
				NounDescription description;
				description.ptr = abstractNode;
				description.descriptionType = class_NounDescriptionType;
				args_abstract.push_back(description);
			}
		}

		//if(arg_abstract.isValid())
		//	absForm.args[0] = arg_abstract;
		//results.push_back(absForm);
		for (NounDescription arg_abstract : args_abstract)
		{
			Sentence absForm = concreteForm;
			absForm.args[0] = arg_abstract;
			results.push_back(absForm);
		}
	}
#if 0
	else if (concreteForm.args.size() == 2)
	{
		NounDescription arg0_concrete= concreteForm.args[0],arg1_concrete = concreteForm.args[1],
			arg0_abstract = arg0_concrete.toClass(),arg1_abstract = arg1_concrete.toClass();
		if (arg0_abstract.isValid() && !arg1_abstract.isValid())
		{
			results.push_back(Sentence(concreteForm.verb,NDs{arg0_concrete,arg1_concrete}));
			results.push_back(Sentence(concreteForm.verb,NDs{arg0_abstract, arg1_concrete}));
		}
		if (!arg0_abstract.isValid() && arg1_abstract.isValid())
		{
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_concrete,arg1_concrete }));
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_concrete, arg1_abstract}));
		}
		if (arg0_abstract.isValid() && arg1_abstract.isValid())
		{
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_concrete,arg1_concrete }));
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_concrete, arg1_abstract }));
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_abstract,arg1_concrete }));
			results.push_back(Sentence(concreteForm.verb, NDs{ arg0_abstract, arg1_abstract }));
		}
	}
	else if (concreteForm.args.size() == 3)
	{
		Sentence s_doubleArg = concreteForm;
		s_doubleArg.args.pop_back();
		Sentence s_singleArg = concreteForm;
		s_singleArg.args = { concreteForm.args[2] };

		vector<Sentence> combinations_doubleArg = generateAllValidExprOfAction(s_doubleArg);
		vector<Sentence> combinations_singleArg = generateAllValidExprOfAction(s_singleArg);

		for (Sentence& each_double : combinations_doubleArg)
		{
			for (Sentence& each_single : combinations_singleArg)
			{
				Sentence result = each_double;
				result.args.push_back(each_single.args[0]);
				results.push_back(result);
			}
		}

	}
#else
	else if (concreteForm.args.size()>1)
	{
		Sentence s_rest = concreteForm;
		s_rest.args.pop_back();
		Sentence s_tail = concreteForm;
		s_tail.args = { concreteForm.args[concreteForm.args.size()-1] };

		vector<Sentence> combinations_restArg = generateAllValidExprOfAction(s_rest);//递归
		vector<Sentence> combinations_tailArg = generateAllValidExprOfAction(s_tail);

		for (Sentence& each_rest : combinations_restArg)
		{
			for (Sentence& each_tail : combinations_tailArg)
			{
				Sentence result = each_rest;
				result.args.push_back(each_tail.args[0]);
				results.push_back(result);
			}
		}
	}
#endif
	
	//筛选掉过度抽象的
	if (concreteForm.args.size() == concreteForm.verb->paraTypes.size())
	{
		vector<Sentence> returnResults;
		auto mostAbstract = concreteForm.verb->getMostAbstractSentence();
		for (Sentence& result : results)
		{
			if (mostAbstract >= result)
				returnResults.push_back(result);
		}
		return returnResults;
	}
	else
		return results;
	
}
inline vector<Proposition> generateAllValidExprOfEffect(Proposition concreteEffect, vector<NounDescription>& argsFromConcreteAction)
{
	if (concreteEffect.isAllConcrete() == false)
		return vector<Proposition>();

	PredicatePointer predicate = concreteEffect.predicate;
	vector<Proposition> results;
	if (pred_decreased == predicate || pred_increased == predicate)
	{
		NounDescription subject_concrete = concreteEffect.args[0];
		NounDescription subject_abstract = subject_concrete.toIndex(argsFromConcreteAction);
		results.push_back(concreteEffect);
		if (subject_abstract.isValid())
		{
			Proposition abstractEffect = concreteEffect;
			abstractEffect.args[0] = subject_abstract;
			results.push_back(abstractEffect);
		}
	}
	else if (pred_memberHas == predicate || pred_propertyIs == predicate)
	{
		NounDescription subject_concrete = concreteEffect.args[0],
			subject_abstract = subject_concrete.toIndex(argsFromConcreteAction),
			propertyNameNode = concreteEffect.args[1],
			object_concrete = concreteEffect.args[2],
			object_abstract = object_concrete.toIndex(argsFromConcreteAction);
		if (subject_abstract.isValid() && !object_abstract.isValid())
		{
			results.push_back(Proposition(predicate, NDs{ subject_concrete, propertyNameNode, object_concrete }));
			results.push_back(Proposition(predicate, NDs{ subject_abstract, propertyNameNode, object_concrete }));
		}
		if (!subject_abstract.isValid() && object_abstract.isValid())
		{
			results.push_back(Proposition(predicate, NDs{ subject_concrete, propertyNameNode, object_concrete }));
			results.push_back(Proposition(predicate, NDs{ subject_concrete, propertyNameNode, object_abstract }));
		}
		if (subject_abstract.isValid() && object_abstract.isValid())
		{
			results.push_back(Proposition(predicate, NDs{ subject_concrete, propertyNameNode, object_concrete }));
			results.push_back(Proposition(predicate, NDs{ subject_concrete, propertyNameNode, object_abstract }));
			results.push_back(Proposition(predicate, NDs{ subject_abstract, propertyNameNode, object_concrete }));
			results.push_back(Proposition(predicate, NDs{ subject_abstract, propertyNameNode, object_abstract }));
		}

	}
	return results;
}


inline vector<ActionAndEffect> combineActionsAndEffects(vector<Sentence> actions, vector<Proposition> effects)
{
	vector<ActionAndEffect> results;
	for (Sentence& action : actions)
	{
		for (Proposition& effect : effects)
		{
			results.push_back(ActionAndEffect(action, effect));
		}
	}
	return results;
}


class ConclusionsLogger
{
public:
	int thresholdTimes = 3;
	map<ActionAndEffect, int>  timesCount;
	int insert(ActionAndEffect conclusion)
	{
		bool alreadyRecorded;

		alreadyRecorded = (timesCount.find(conclusion) != timesCount.end());
		if (alreadyRecorded)
		{
			//auto theOldOne = (*timesCount.find(conclusion)).first;
			//cout << "重复：\n" << "原：" << theOldOne.toString() << endl;
			//cout << "新：" << conclusion.toString() << endl;

			int oldCount = timesCount[conclusion];
			if (oldCount < 0)
				return -1;
			else
			{
				timesCount[conclusion]++;
				return oldCount + 1;
			}
		}
		else
		{
			//conclusions.insert(conclusion);
			timesCount.insert({ conclusion,1 });
			return 1;
		}
	}
	int insert(vector<ActionAndEffect>& conclusions)
	{
		int count = 0;
		for (ActionAndEffect& conclusion : conclusions)
			if (insert(conclusion) > 0)
				count++;
		return count;
	}
	int getMaxTimes()//最大次数
	{
		int max = 0;
		for (auto& eachPair : timesCount)
			if (eachPair.second > max)
				max = eachPair.second;
		return max;
	}
	vector<ActionAndEffect> getValidConclusions()
	{
		int _threshold = max(getMaxTimes(), this->thresholdTimes);
		vector<ActionAndEffect> results;
		for (auto eachPair : timesCount)
		{
			ActionAndEffect conclusion = eachPair.first;
			int times = eachPair.second;
			if (times >= _threshold)
				results.push_back(conclusion);
		}
		return results;
	}
	vector<ActionAndEffect> findByAction(Sentence action)
	{
		vector<ActionAndEffect> results;
		for (auto eachPair : timesCount)
			if (eachPair.first.action == action)
				results.push_back(eachPair.first);
		return results;
	}
	vector<Proposition> findEffectsByAction(Sentence& action)
	{
		vector<Proposition> results;
		for (auto eachPair : timesCount)
			if (eachPair.first.action == action)
				results.push_back(eachPair.first.effect);
		return results;
	}
	int executeAndGetConclusions(Sentence cmd);
};

class ConclusionsLogger_VerbCompare:public ConclusionsLogger
{
public:
	bool insertCommandAndKnownEffect(Sentence cmd);
	int executeAndGetConclusions(Sentence cmd) = delete;//不适用于动词抽象
};



vector<Proposition> getConclusionsByComparingBeforeAndAfter(AIConceptNode* thisObject,
	map<string, AIConceptNode*> before, map<string, AIConceptNode*> after);

vector<Proposition> getAllConclusionsByComparingBeforeAndAfter(
	map<AIConceptNode*, map<string, AIConceptNode*>> before,
	map<AIConceptNode*, map<string, AIConceptNode*>> after);