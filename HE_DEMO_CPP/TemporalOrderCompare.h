#pragma once
#include"Proposition.h"

class ConclusionsLogger_ActionOnly :public ConclusionsLogger
{
public:
	int insert(Sentence& action)
	{
		return ConclusionsLogger::insert(ActionAndEffect(action, Proposition()));
	}
	int insert(vector<Sentence>& actions)
	{
		vector<ActionAndEffect> actionsAndEffects;//强制转换
		for (auto& action : actions)
			actionsAndEffects.push_back(ActionAndEffect(action, Proposition()));
		return ConclusionsLogger::insert(actionsAndEffects);
	}
	vector<Sentence> getValidCommands()
	{
		vector<Sentence> results;
		auto actionsAndEffecs = ConclusionsLogger::getValidConclusions();
		for (auto& each : actionsAndEffecs)
			results.push_back(each.action);
		return results;
	}
	bool has(Sentence& action) { return !findByAction(action).empty(); }

	vector<Proposition> findEffectsByAction(Sentence& action) = delete;
};

//如时序长度或动词不一致，则返回空数组
vector<Sentence> compareAmongTemporalOrders(vector<vector<Sentence>> temporalOrders);
void replaceItem(vector<Sentence>& temporalOrder, AIConceptNode* from_typeNode,int to_index);
void replaceItems(vector<Sentence>& temporalOrder, map<AIConceptNode*,int> fromTos);