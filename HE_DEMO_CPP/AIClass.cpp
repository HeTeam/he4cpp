#include"stdafx.h"
#include"AIClass.h"
#include"Classes.h"

void Class::setOffsetsAndFieldType(map<string, int>& inputOffsets, map<string, string> inputTypes)
{
	for (auto& eachPair : inputOffsets)
	{
		string thisFieldRuntimeName = eachPair.first;
		if ("Ãû³Æ" == thisFieldRuntimeName)
			continue;
		string staticNameOfTypeOfThisField = inputTypes[thisFieldRuntimeName];
		int index = Classes::getInstance().findByStaticTypeName(staticNameOfTypeOfThisField);
		if (index == -1)
			continue;
		map_fieldType.insert({ thisFieldRuntimeName,&Classes::getInstance().types[index] });
		map_offset.insert(eachPair);
	}
}