#include"stdafx.h"
#include"TemporalOrderCompare.h"

vector<Sentence> compareAmongTemporalOrders(vector<vector<Sentence>> temporalOrders)
{
	if (temporalOrders.size() <= 1)
		return vector<Sentence>();

	int len = temporalOrders[0].size();
	for (size_t i = 1; i < temporalOrders.size(); i++)
	{
		if(temporalOrders[i].size()!=len)
			return vector<Sentence>();
	}

	for (size_t lineId = 0; lineId < len; lineId++)
	{
		AITemporalOrderNode* verb = temporalOrders[0][lineId].verb;
		for (size_t index_temporal = 1; index_temporal < temporalOrders.size(); index_temporal++)
		{
			AITemporalOrderNode* currentVerb = temporalOrders[index_temporal][lineId].verb;
			if(currentVerb!=verb)
				return vector<Sentence>();
		}
	}

	vector<ConclusionsLogger_ActionOnly> loggers(len);//每一行语句，建立一个比较器
	for (size_t lineId = 0; lineId < loggers.size(); lineId++)
	{
		ConclusionsLogger_ActionOnly& logger = loggers[lineId];
		for (vector<Sentence>& temporalOrder : temporalOrders)
		{
			Sentence& thisStep = temporalOrder[lineId];
			vector<Sentence> allValidCommands = generateAllValidExprOfAction(thisStep);
			logger.insert(allValidCommands);
		}
	}
	vector<Sentence> finalResults;
	for (auto& logger : loggers)
	{
		vector<Sentence> abstractSentences = logger.getValidCommands();
		if (abstractSentences.empty())
			return vector<Sentence>();
		else
			finalResults.push_back(abstractSentences[0]);
	}
	return finalResults;
}

void replaceItem(vector<Sentence>& temporalOrder, AIConceptNode* from_typeNode, int to_index)
{
	NounDescription from, to;
	from.descriptionType = class_NounDescriptionType;
	from.ptr = from_typeNode;
	to.descriptionType = paraIndex_NounDescriptionType;
	to.index = to_index;
	for (Sentence& step : temporalOrder)
	{
		for (NounDescription& item : step.args)
		{
			if (item == from)
				item = to;
		}
	}
}

void replaceItems(vector<Sentence>& temporalOrder, map<AIConceptNode*, int> fromTos)
{
	for (auto fromTo : fromTos)
	{
		replaceItem(temporalOrder, fromTo.first, fromTo.second);
	}
}