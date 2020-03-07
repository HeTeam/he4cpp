#include"stdafx.h"
#include"AIHeader.h"
#include"global.h"
#include"time.h"

std::vector<std::string> split(string s, string delim)
{
	std::vector<std::string> v;
	string c = delim;
	std::string::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.emplace_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != len)
		v.emplace_back(s.substr(pos1));

	return v;
}

std::string trim(std::string str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return str;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		return str.substr(pos, pos2 - pos + 1);
	}
	return str.substr(pos);
}

string randString()
{
	char str[6 + 1] = { 0 };
	for (int i = 0; i < 6; i++)
	{
		char ch = rand() % 26 + 'a';
		str[i] = ch;
	}
	return str;
}

string strJoin(vector<string> arr, string separator)
{
	string s;
	for (unsigned i = 0; i < arr.size(); i++)
	{
		s += arr[i];
		if (i + 1 < arr.size())
			s += separator;
	}
	return s;
}

vector<void*> toPtrArr(string str)
{
	if(isPtrArrayStr(str))
		str = removeBraces(str);
	vector<void*> results;
	vector<string> elementNames = split(removeBraces(str), ",");
	for (string each : elementNames)
	{
		each = trim(each);
		if ("" == each)
			continue;
		AIConceptNode* noun=theGlobalArea.findNounByName(each);
		if (noun)
			results.push_back(noun->ptr);
		else
			results.push_back(NULL);
	}
	return results;
}

string getCurrentTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	tm out_tm;
	localtime_s(&out_tm, &timep);
	//strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &out_tm);
	return tmp;
}
