#include "precompiled.h"

#include "Streamable.h"

StreamableObjectFactory s_StreamableObjectFactory;

bool StreamableObjectFactory::StreamableObject::HasChildNamed(const std_string& in_ChildName)
{
	for (auto it : vChildren)
	{
		if (it->name == in_ChildName)
			return true;

		if (it->HasChildNamed(in_ChildName))
			return true;
	}
	return false;
}

void* StreamableObjectFactory::CreateStreamableObject(const std_string& in_objectName, const std_string& in_baseClass)
{
	auto it = m_mStreamableObjects.find(in_objectName);
	if (it == m_mStreamableObjects.end())
	{
		AssertMsg(false, L("Class [%s] is not registered (did you forget DEFINE_PROPERTIES(%s);?)"), in_objectName.c_str(), in_objectName.c_str());
		return nullptr;
	}

	Assert(it->second != nullptr);
	Assert(it->second->pCreationFunc != nullptr);

	if (in_baseClass != L("") && in_baseClass != in_objectName)
	{
		auto parentIt = m_mStreamableObjects.find(in_baseClass);
		if (parentIt == m_mStreamableObjects.end())
		{
			AssertMsg(false, L("Inexistant base class [%s]"), in_baseClass.c_str());
			return nullptr;
		}

		if (!parentIt->second->HasChildNamed(in_objectName))
		{
			AssertMsg(false, L("Object class [%s] is not derived from [%s]"), in_objectName.c_str(), in_baseClass.c_str());
			return nullptr;
		}
	}

	return it->second->pCreationFunc();
}

void StreamableObjectFactory::RegisterStreamableObject(const std_string& in_objName, ObjCreationFunction in_creationFunc, const std_string& in_parentName)
{
	auto objIt = m_mStreamableObjects.find(in_objName);

	StreamableObject* pNewObject;
	if (objIt == m_mStreamableObjects.end())
	{
		pNewObject = new StreamableObject;
		pNewObject->name = in_objName;
		m_mStreamableObjects.insert(std::make_pair(in_objName, pNewObject));
	}
	else
	{
		Assert(objIt->second == nullptr);
		Assert(objIt->second->pCreationFunc == nullptr);

		pNewObject = objIt->second;
	}
	pNewObject->pCreationFunc = in_creationFunc;

	if (in_parentName != L(""))
	{
		auto parentIt = m_mStreamableObjects.find(in_parentName);
		if(parentIt == m_mStreamableObjects.end())
		{ 
			StreamableObject* pNewParent = new StreamableObject;
			pNewObject->name = in_parentName;
			pNewParent->vChildren.push_back(pNewObject);
			m_mStreamableObjects.insert(std::make_pair(in_parentName, pNewParent));
		}
		else
		{
			parentIt->second->vChildren.push_back(pNewObject);
		}
	}
}