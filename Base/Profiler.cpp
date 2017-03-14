#include "precompiled.h"

#include "Profiler.h"
#include "../OS_Base/System.h"

//--------------------------------------------------------------------------------
ProfileBlock::~ProfileBlock() 
{
	for (auto i : m_vChildren)
		delete i;
	m_vChildren.clear();

	delete m_pActiveChild;
	m_pActiveChild = nullptr;
}

//--------------------------------------------------------------------------------
void ProfileBlock::StartBlock(ProfileBlock* in_pChild)
{
	if (m_pActiveChild)
		m_pActiveChild->StartBlock(in_pChild);
	else
		m_pActiveChild = in_pChild;
}

//--------------------------------------------------------------------------------
bool ProfileBlock::StopBlock()
{
	if (m_pActiveChild)
	{
		if (m_pActiveChild->StopBlock())
		{
			m_vChildren.push_back(m_pActiveChild);
			m_pActiveChild = nullptr;
		}
		return false;
	}
	else
	{
		Stop();
		return true;
	}
}

//--------------------------------------------------------------------------------
void ProfileBlock::Stop() 
{ 
	m_i64StopTime = OS::GetTickCount(); 

	m_fBlockTimeMs = (double(m_i64StopTime - m_i64StartTime) / OS::GetTickFrequency()) * 1000.f;
}

//--------------------------------------------------------------------------------
Profiler::Profiler()
{

}

//--------------------------------------------------------------------------------
Profiler::~Profiler()
{
	for (auto it : m_vBlocks)
		for (auto blockIt : it.second)
			delete blockIt;

	for (auto it : m_vActiveBlocks)
		delete it.second;
}

//--------------------------------------------------------------------------------
void Profiler::StartBlock(void* in_sectionID, std::thread::id in_threadID)
{
	ProfileBlock* newBlock = new ProfileBlock(in_sectionID);

	if (m_vActiveBlocks[in_threadID] == nullptr)
		m_vActiveBlocks[in_threadID] = newBlock;
	else
		m_vActiveBlocks[in_threadID]->StartBlock(newBlock);
}

//--------------------------------------------------------------------------------
void Profiler::EndBlock(std::thread::id in_threadID)
{
	if (m_vActiveBlocks[in_threadID]->StopBlock())
	{
		m_vBlocks[in_threadID].push_back(m_vActiveBlocks[in_threadID]);
		m_vActiveBlocks[in_threadID] = nullptr;
	}
}