#pragma once
#include<type_traits>
#include"AINode.h"
#include"Proposition.h"
#define theGlobalArea GlobalArea::getInstance()

#define concept_of(type) theGlobalArea.findNounByPtrAddress(runtime_of(type))
#define verb_of(name)    theGlobalArea.findVerbByName(#name)

class GlobalArea
{
private:
	GlobalArea(){}
public:
	GlobalArea(GlobalArea&) = delete;
	static GlobalArea& getInstance()
	{
		static GlobalArea obj;
		return obj;
	}

	vector<AIConceptNode*> nouns;
	vector<AITemporalOrderNode*> verbs;
	vector<ActionAndEffect> relationships_action_effect;
	vector<ConditionAndAction> conditionBeforeDoings;
	
	AIConceptNode* findNounByName(string name)
	{
		for (auto each : nouns)
			if (each->name == name)
				return each;
		return NULL;
	}

	AITemporalOrderNode* findVerbByName(string name)
	{
		for (auto each : verbs)
			if (each->name == name)
				return each;
		return NULL;
	}
	vector<AIConceptNode*> findNounsByClass(AIConceptNode* cls)
	{
		vector<AIConceptNode*> results;
		for (auto each : nouns)
			if (*each < *cls)
				results.push_back(each);
		return results;
	}
	vector<AIConceptNode*> findNounsByClassWithRestriction(AIConceptNode* cls,Sentence abstractCmd)
	{
		int where = abstractCmd.findAny();
		vector<AIConceptNode*> results;
		for (auto each : nouns)
		{
			if (*each < *cls)
			{
				Sentence temp_cmd = abstractCmd;
				temp_cmd.args[where] = NounDescription(each);
				if(temp_cmd.checkBeforeExecute()==true)
					results.push_back(each);
			}
		}
		return results;
	}
	vector<AIConceptNode*> findNounsByClassWithRestriction(Sentence abstractCmd)
	{
		int where = abstractCmd.findAny();
		if (-1 == where)
			return{};
		AIConceptNode* cls = abstractCmd.args[where].ptr;
		return findNounsByClassWithRestriction(cls, abstractCmd);
	}
	AIConceptNode* findNounByPtrAddress(void* ptr)
	{
		for (AIConceptNode* noun : nouns)
			if (ptr == noun->ptr)
				return noun;
		return NULL;
	}
	AITemporalOrderNode* findVerbByPtrAddress(void* ptr)
	{
		for (auto verb : verbs)
			if (ptr == verb->funcPtr)
				return verb;
		return NULL;
	}
	map< AIConceptNode*,map<string, AIConceptNode*>> getCurrentStatuses_jsons()
	{
		map< AIConceptNode*, map<string, AIConceptNode*>> jsons;
		for (AIConceptNode* noun : nouns)
		{
			jsons.insert({ noun,noun->toJson() });
		}
		return jsons;
	}

	vector<Sentence> findActionsByGoal(Proposition goal)
	{
		vector<Sentence> actions;
		for (ActionAndEffect& each : relationships_action_effect)
		{
			auto& effect_concrete = each.effect.toConcrete(each.action.args);

			if (effect_concrete >= goal)
			{
				Sentence convertedAction = each.action;//拷贝一份
				for (unsigned index_effect_arg = 0; index_effect_arg < each.effect.args.size(); index_effect_arg++)
				{
					NounDescription& arg_effect = each.effect.args[index_effect_arg];
					if (paraIndex_NounDescriptionType == arg_effect.descriptionType)
					{
						int index_action_argToBeReplaced = arg_effect.index;//原版的“效果”命题里面记录的是参数下标
																			//反正目标一定是具体的
						NounDescription& valueFromGoal = goal.args[index_effect_arg];
						//遇到参数[N]，必须替换
						convertedAction.args[index_action_argToBeReplaced] = valueFromGoal;
					}
				}
				actions.push_back(convertedAction);//each.action);
			}
		}
		
		//复审
		vector<Sentence> return_actions;
		for (Sentence& action : actions)
		{
			//参数不许有重复
			if (!action.verb->allowSameArgs)
			{
				if (action.hasSameArgs())
					continue;//不允许，直接跳过
			}

			if (action.isAllConcrete())//如果不含有模糊抽象的表述（如任意一个XX类型对象），就直接放行
			{
				return_actions.push_back(action);
				continue;
			}
			//找出合适的替换项
			vector<AIConceptNode*> suitableChoices = theGlobalArea.findNounsByClassWithRestriction(action);
			//逐个代入替换，得到所有可行的结果
			vector<Sentence> afterReplace = action.replaceAbstractItem_muliti(suitableChoices);
			for (Sentence& each_afterReplace : afterReplace)
			{
				//参数不许有重复
				if (!each_afterReplace.verb->allowSameArgs)
				{
					//没有重复，则放行
					if (each_afterReplace.hasSameArgs()==false)
						return_actions.push_back(each_afterReplace);
				}
				else
					return_actions.push_back(each_afterReplace);
			}
		}
		return return_actions;
	}
	vector<ActionAndEffect> findActionAndEffectsByAction(Sentence& action)
	{
		vector<ActionAndEffect> results;
		for (auto& each : relationships_action_effect)
		{
			if (action <= each.action)
				results.push_back(each);
		}
		return results;//返回一个无效的结构体
	}
	vector<Proposition> findConditionsOfAction(Sentence action)
	{
		vector<Proposition> results;
		for (auto& each : theGlobalArea.conditionBeforeDoings)
			if (each.action >= action)
				results.push_back(each.condition());
		return results;
	}
};

template<typename T>
AIConceptNode& createNewRefNodeAndPutItsDataTypeIntoAbsNodeArray(T& obj)
{
	AIConceptNode& node=_newRefNode<decay<T>::type>(obj);
	const int typeID = theClasses.getRuntimeClassID<decay<T>::type>();
	if (Classes::start_jsonSupportedClassID <= typeID && typeID <= Classes::end_jsonSupportedClassID)
		node.name = *(string*)&obj;
	if (typeID == theClasses.getRuntimeClassID<Class>())
		node.name = ((Class*)&obj)->runtimeName;

	if (typeid(T).name() == typeid(Class).name())
		return node;//到此结束

	AIConceptNode* conceptOfDataType = theGlobalArea.findNounByPtrAddress(runtime_of(decay<T>::type));
	if (conceptOfDataType)
	{
		node.absNodes.push_back(conceptOfDataType);
		conceptOfDataType->concreteNodes.push_back(&node);
	}
	else if(typeid(T).name()!=typeid(Class).name())
	{
		DEBUG_MSG("%s 没有概念结点！", typeid(T).name());
	}
	return node;
}


#define createNewRefNode(obj) createNewRefNodeAndPutItsDataTypeIntoAbsNodeArray<decltype(obj)>(obj)