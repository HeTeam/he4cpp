#pragma once
#include"AIClass.h"
#include"Classes.h"
#include"AIHeader.h"
#include<string>
#include<vector>
#include<type_traits>
#include<algorithm>
using namespace std;

//#define __concept_string(node) (node->name+"（"+node->type->runtimeName+"）:"+json_toString(node->toJson()))
//#undef concept_string

//概念结点
class AIConceptNode
{
public:
	string name;//名称
	void* ptr=NULL;//指向存储其具体内容的内存单元的指针
	Class* type=NULL;//类型（编程意义上的）
	bool isConst=false;//是否为常量，如果是，以后比较的时候就只要比较其内容即可

	//BEGIN：常量值字段
	int integerValue=0;
	double realValue=0;
	string strValue;
	vector<void*> ptrArrayValue;
	void* ptrAddressValue=NULL;
	//END：BEGIN：常量值字段

	vector<AIConceptNode*> absNodes, concreteNodes;//抽象上级概念，具象下级概念

	vector<AIConceptNode*> getAllAbstractNodes();
	bool isAbstract() { return concreteNodes.size() > 0; }
	bool isInstanceOf(AIConceptNode* cls) //是否为cls类的具象实例
	{
		if (absNodes.empty())
			return false;
		if (stdFindExist(absNodes, cls))
			return true;

		for (auto absNode : absNodes)
			if (absNode->isInstanceOf(cls))
				return true;
		
		return false;
	}

	AIConceptNode(){}
	AIConceptNode(int value)
	{
		integerValue = value;
		ptr = &integerValue;
		type = runtime_of(int);
		isConst = true;
	}
	AIConceptNode(bool value) 
	{ 
		integerValue = (value?1:0);
		ptr = &integerValue;
		type = runtime_of(bool);
		isConst = true;
	}
	AIConceptNode(double value)
	{
		realValue = value;
		ptr = &realValue;
		type = runtime_of(double);
		isConst = true;
	}
	AIConceptNode(string value)
	{
		strValue = value;
		ptr = &strValue;
		type = runtime_of(string);
		isConst = true;
	}
	AIConceptNode(const char* str){ AIConceptNode(string(str)); }
	AIConceptNode(vector<void*> value)
	{
		ptrArrayValue = value;
		ptr = &ptrArrayValue;
		type = runtime_of(vector<void*>);
		isConst = true;
	}
	string toString();
	map<string, AIConceptNode*> toJson();
	bool operator== (AIConceptNode& other)//比较两个概念结点是否相等
	{
		if (this == &other)
			return true;
		if (this->type != other.type)
			return false;
		if (this->isConst != other.isConst)
			return false;
		if (isConst) //只有常量，才需要比较具体内容
		{
			if (runtime_of(int) == type)
				return this->integerValue == other.integerValue;
			else if (runtime_of(double) == type)
				return this->realValue == other.realValue;
			else if (runtime_of(string) == type)
				return this->strValue == other.strValue;
			else if (runtime_of(void*) == type)
				return this->ptrAddressValue == other.ptrAddressValue;
			else if (runtime_of(vector<void*>) == type) //两个数组如果内容相同但顺序不同，也算相等
			{
				if (this->ptrArrayValue.size() != other.ptrArrayValue.size())
					return false;

				for (void* eachOfThisArray : this->ptrArrayValue)
					if (!stdFindExist(other.ptrArrayValue, eachOfThisArray))
						return false;

				return true;
			}
		}

		return false;
	};
	bool operator< (AIConceptNode& other);//判断此概念是否为other概念集合的真子集
	bool operator> (AIConceptNode& other)//判断此概念是否真包含other概念集合
	{
		return other<*this;
	}
	bool operator>= (AIConceptNode& other)//判断此概念是否包含other概念集合
	{
		return *this==other||*this>other;
	}
	bool operator<= (AIConceptNode& other)//判断此概念是否为other概念集合的子集
	{
		return *this == other || *this<other;
	}
	inline bool belongsTo(AIConceptNode& other) { return *this < other; }
	inline bool equalsOrBelongsTo(AIConceptNode& other) { return *this <= other; }
	inline bool contains(AIConceptNode& other) { return *this > other; }
	inline bool equalsOrContains(AIConceptNode& other) { return *this >= other; }
	AIConceptNode(AIConceptNode& other) = delete;//禁止拷贝

	void insertAbsNode(AIConceptNode* abs)
	{
		this->absNodes.push_back(abs);
		abs->concreteNodes.push_back(this);
	}
	void insertConcreteNode(AIConceptNode* con)
	{
		this->concreteNodes.push_back(con);
		con->absNodes.push_back(this);
	}
};

string __concept_string(AIConceptNode* node);
string concept_string(AIConceptNode* pNode);

template<typename T>
AIConceptNode& _newRefNode(T& ref)
{
	AIConceptNode& node = *new AIConceptNode();
	node.ptr = &ref;
	node.type = runtime_of(T);
	//还需要把数据类型对应的ConceptNode找出来，将其指针放进absNodes成员数组里，但这个过程不能写进此头文件
	return node;
}

/*
template<>
AIConceptNode& _newRefNode<Class>(Class& ref)
{
	AIConceptNode& node = *new AIConceptNode();
	node.ptr = &ref;
	node.type = runtime_of(Class);
	node.name = ref.runtimeName;
	//本来要Class类对应的ConceptNode找出来，将其指针放进absNodes成员数组里，但这会造成递归死循环
	//所以就不需要做这一步了
	return node;
}
*/

//暂时禁止创建指向概念结点的概念结点
template<>
AIConceptNode& _newRefNode<AIConceptNode>(AIConceptNode& ref);

//将src的内容拷贝到dest里面，如果它们两者当中有一个不是常量，则函数中断并返回FALSE
bool conceptNodeCopy(AIConceptNode& dest, AIConceptNode& src);

typedef void*(*FunctionPointer)(vector<AIConceptNode*>);

enum NounDescriptionType
{
	none_NounDescriptionType,//无意义
	concrete_NounDescriptionType,//具体对象或常量
	class_NounDescriptionType,//某类的任意一个对象
	paraIndex_NounDescriptionType//参数列表下标
};

#define NDs vector<NounDescription>
#define ConArr vector<AIConceptNode*>

//用于描述参数的结构体，在编程中，我们执行一个函数，传入的参数不一定是常量或固定的变量地址（全局变量）
//还可能是【上层函数的某一个参数】，例如
//void 外部函数(string[] args)
//{
//    内部函数(args[3]);
//}
//这里的args[3]表示第3号参数，它不是一个固定的对象，它的具体内容取决于外部函数所传入的参数数组args
//另外，对于本系统还有一种参数类型——指定类型的任意值，其具体取值是随机的
class NounDescription
{
public:
	NounDescriptionType descriptionType = none_NounDescriptionType;//描述的参数是什么类型的？
	AIConceptNode* ptr=NULL;//参数的值，如果是【指定类型的任意值】，那么ptr就指向这一“类型”所对应的概念结点
	int index = 0;//如果描述的参数是由外层函数传入的，需说明是第几个参数
	NounDescription() {}
	NounDescription(Class* cls);
	NounDescription(AIConceptNode* ptr)
	{
		this->ptr = ptr;
		descriptionType = concrete_NounDescriptionType;
	}
	NounDescription(int index)
	{
		this->index = index;
		descriptionType = paraIndex_NounDescriptionType;
	}
	bool isValid() { return none_NounDescriptionType != this->descriptionType; }
	string toString(vector<string> paraNamesOfParentVerb)
	{
		bool isParaNamesValid = !paraNamesOfParentVerb.empty();
		if (paraIndex_NounDescriptionType == descriptionType)
		{
			if(isParaNamesValid)
				return "参数[" + paraNamesOfParentVerb[index] + "]";
			else
				return "参数[" + to_string(index) + "]";
		}
		else if (ptr && concrete_NounDescriptionType == descriptionType)
			return ptr->toString();
		else if (ptr && class_NounDescriptionType == descriptionType)
			return "any$" + ptr->toString();//指定类型的任意一个对象
		else
			return "<错误参数>";
	}
	string toString()
	{
		return toString(vector<string>());
	}
	//NounDescriptionType toConcrete(vector<AIConceptNode*> paraTypes);

	//如果是具体的某个对象（假设其类型为T），就把描述泛化成：T类型的任意一个对象
	NounDescription toClass();
	//argsFromConcreteAction是外层传入的参数，如果本结构体里的具体对象正好和外层参数中的某个对象一致，
	//则记录下标位置，并将本结构体的类型变更为【参数列表下标】
	NounDescription toIndex(vector<NounDescription>& argsFromConcreteAction)
	{
		auto iter = stdFind(argsFromConcreteAction, *this);
		if (argsFromConcreteAction.end() == iter)
			return NounDescription();
		int index = iter - argsFromConcreteAction.begin();
		return NounDescription(index);
	}
	
	bool operator ==(const NounDescription& other) const
	{
		if (this->descriptionType != other.descriptionType)
			return false;
		if (concrete_NounDescriptionType == descriptionType || class_NounDescriptionType == descriptionType)
			return *this->ptr == *other.ptr;
		if (paraIndex_NounDescriptionType == descriptionType)
			return this->index == other.index;
		return false;
	}
	bool operator!=(const NounDescription& other) const{ return !(*this == other); }
	bool operator >(const NounDescription& other) const //本结构体的描述，是否已经涵盖（并超越了）了other的描述范围
	{
		if (class_NounDescriptionType == this->descriptionType)
		{
			if (class_NounDescriptionType == other.descriptionType||
				concrete_NounDescriptionType==other.descriptionType)
				return *this->ptr > *other.ptr;
		}
		return false;
	}
	bool operator <(const NounDescription& other) const
	{
		return other > *this;
	}
	bool operator >=(const NounDescription& other) const { return *this == other || *this > other; }
	bool operator <=(const NounDescription& other) const { return *this == other || *this < other; }
};

class AITemporalOrderNode;

enum VerbType
{
	abstract_VerbType,//抽象的动词，没有具体的执行方法
	funcPtr_VerbType,//指向函数的指针，直接去执行这个函数就可以
	instructionList_VerbType//具体指令内容，存储在时序数组里面，要按顺序执行时序数组里的内容
};

class Sentence
{
public:
	AITemporalOrderNode* verb = NULL;//动词
	vector<NounDescription> args;//参数
	string toString()const;
	string toString(vector<string> parentParaNames);
	bool isValid() { return NULL != this->verb; }
	bool isAllConcrete() //参数是否具体
	{
		for (auto& arg : args)
			if (concrete_NounDescriptionType != arg.descriptionType)
				return false;
		return true;
	}
	Sentence toConcrete(vector<AIConceptNode*>& inputArgs);//如果不具体，就对参数做具象化处理
	void* execute(vector<AIConceptNode*>& inputArgs);//执行
	void* execute() //如果args已经是具体的参数，就不需要外界传入参数，自己就可以执行了
	{
		return execute(vector<AIConceptNode*>{});
	}
	bool checkBeforeExecute();
	/*static bool checkAllBeforeExecute(vector<Sentence>& commands)
	{
		for (Sentence& cmd : commands)
			if (cmd.checkBeforeExecute() == false)
				return false;
		return true;
	}*/

	int findAny();
	Sentence replaceAbstractItem(AIConceptNode* concreteItem)
	{
		Sentence copied = *this;
		if(findAny() >=0 )
			copied.args[findAny()] = NounDescription(concreteItem);
		return copied;
	}
	vector<Sentence> replaceAbstractItem_muliti(vector<AIConceptNode*> concreteItems)
	{
		if (-1 == findAny())
			return{ *this };
		vector<Sentence> results;
		for (auto concreteItem : concreteItems)
		{
			Sentence copied = *this;
			copied.args[findAny()] = NounDescription(concreteItem);
			results.push_back(copied);
		}
		return results;
	}
	bool hasSameArgs()
	{
		if (args.size() <= 1)
			return false;

		for (size_t i = 0; i < args.size() - 1; i++)
		{
			if (args[i].descriptionType != concrete_NounDescriptionType)
				continue;
			for (size_t j = i + 1; j < args.size(); j++)
			{
				if (args[j].descriptionType != concrete_NounDescriptionType)
					continue;
				//这回不考虑常量值相等的情况
				if (args[i].ptr->isConst || args[j].ptr->isConst)
					continue;
				if (args[i].ptr == args[j].ptr)
					return true;
			}
		}

		return false;
	}
	Sentence() {}
	Sentence(AITemporalOrderNode* verb, vector<NounDescription> args)
	{
		this->verb = verb;
		this->args = args;
	}
	Sentence(AITemporalOrderNode* verb, vector<AIConceptNode*> concreteArgs)
	{
		this->verb = verb;
		for (auto conArg : concreteArgs)
			this->args.push_back(NounDescription(conArg));
	}
	Sentence(string str);
	void* operator()() { return execute(); }
	bool operator==(const Sentence& other) const
	{
		if (this->verb != other.verb)
			return false;
		if (this->args.size() != other.args.size())
			return false;
		for (unsigned i = 0; i < args.size(); i++)
			if (this->args[i] != other.args[i])
				return false;
		return true;
	}
	bool operator<(const Sentence& other) const;
	bool operator>(const Sentence& other) const
	{
		return  other < *this;
	}
	bool operator<=(const Sentence& other) const
	{
		return *this == other || *this < other;
	}
	bool operator>=(const Sentence& other) const
	{
		return *this == other || *this > other;
	}
};


class AITemporalOrderNode
{
public:
	string name;//名字
	FunctionPointer funcPtr = NULL;//函数指针
	bool allowSameArgs = false;
	vector<AIConceptNode*> paraTypes;//参数类型
	vector<string> paraNames;
	vector<Sentence> steps;//时序，解决问题的步骤
	vector<AITemporalOrderNode*> absNodes, concreteNodes;//同AIConceptNode
	inline bool isParaNamesValid()
	{
		return paraTypes.size() == paraNames.size();
	}
	VerbType getType()
	{
		if (funcPtr)
			return funcPtr_VerbType;
		else if (steps.size())
			return instructionList_VerbType;
		else
			return abstract_VerbType;
	}
	void setParaTypes(vector<AIConceptNode*> paraTypes) { this->paraTypes = paraTypes; }
	void setParaTypes(vector<Class*> paraTypes);
	string toString() { return name; }
	string toString_declaration()
	{
		string str = name;
		str += "(";
		for (unsigned i = 0; i < paraTypes.size(); i++)
		{
			str += paraTypes[i] -> name;
			if (isParaNamesValid())
				str += " "+paraNames[i];
			if (i + 1 < paraTypes.size())
				str += ",";
		}
		str += ")";
		return str;
	}
	string toString_content(bool needTab=false)
	{
		auto& arr = this->steps;
		string s;
		for (size_t i = 0; i < arr.size(); i++)
		{
			if (needTab)
				s += "    ";
			s += arr[i].toString( isParaNamesValid() ? this->paraNames : vector<string>());
			if (i != arr.size() - 1)
				s += "\n";
		}
		return s;
	}
	string toString_all()
	{
		auto& arr = this->steps;
		string s = this->toString_declaration();
		s += "\n";
		s += "{\n";
		s += toString_content(true);
		s += "\n";
		s += "}";
		return s;
	}
	vector<Sentence> toConcreteCommands(vector<AIConceptNode*>& inputArgs);
	void* execute(vector<AIConceptNode*>& inputArgs);
	void* operator()(vector<AIConceptNode*>& inputArgs) { return execute(inputArgs); }
	bool operator==(AITemporalOrderNode& other)
	{
		return this == &other;//对指令内容的比较，目前暂时用不到
	}
	bool operator< (AITemporalOrderNode& other);
	bool operator> (AITemporalOrderNode& other)
	{
		return other<*this;
	}
	bool operator>= (AITemporalOrderNode& other)
	{
		return *this == other || *this>other;
	}
	bool operator<= (AITemporalOrderNode& other)
	{
		return *this == other || *this<other;
	}
	inline bool belongsTo(AITemporalOrderNode& other) { return *this < other; }
	inline bool equalsOrBelongsTo(AITemporalOrderNode& other) { return *this <= other; }
	inline bool contains(AITemporalOrderNode& other) { return *this > other; }
	inline bool equalsOrContains(AITemporalOrderNode& other) { return *this >= other; }
	//AITemporalOrderNode(){}
	//AITemporalOrderNode(AITemporalOrderNode&) = delete;//禁止拷贝
	void insertAbsNode(AITemporalOrderNode* abs)
	{
		this->absNodes.push_back(abs);
		abs->concreteNodes.push_back(this);
	}
	void insertConcreteNode(AITemporalOrderNode* con)
	{
		this->concreteNodes.push_back(con);
		con->absNodes.push_back(this);
	}
	Sentence getMostAbstractSentence()
	{
		Sentence result;
		result.verb = this;
		for (AIConceptNode* type : this->paraTypes)
		{
			NounDescription description;
			description.ptr = type;
			description.descriptionType = class_NounDescriptionType;
			result.args.push_back(description);
		}
		return result;
	}
};

bool isArgTypeMatch(vector<AIConceptNode*> args, vector<AIConceptNode*> paraTypes);
string json_toString(map<string, AIConceptNode*> json);
void json_GC(map<string, AIConceptNode*> json);//垃圾回收

vector<NounDescription> nounDescriptions_toAbstract(vector<NounDescription>& originals, vector<NounDescription>& inputArgs);
vector<NounDescription> toDecriptions(vector<AIConceptNode*>&);
vector<AIConceptNode*>  toConceptNodes(vector<NounDescription>&);