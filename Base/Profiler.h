#pragma once

#include "StringUtils.h"
#include "Singleton.h"

#include "../OS_Base/System.h"

#include <map>
#include <thread>

//--------------------------------------------------------------------------------
class ProfileBlock
{
public:
	ProfileBlock(void* in_sectionID) : m_SectionID((s8*)in_sectionID) { m_i64StartTime = OS::GetTickCount(); }
	~ProfileBlock();

	void StartBlock(ProfileBlock* in_pChild);
	bool StopBlock();

private:
	void Stop();

	double m_fBlockTimeMs = 0.f;
	s8* m_SectionID = nullptr;

	s64 m_i64StartTime = 0;
	s64 m_i64StopTime = 0;
	
	std::vector<ProfileBlock*> m_vChildren;
	ProfileBlock* m_pActiveChild = nullptr;
};

//--------------------------------------------------------------------------------
class Profiler : public Singleton<Profiler>
{
	MAKE_SINGLETON(Profiler);

public:
	void StartBlock(void* in_sectionID, std::thread::id in_threadID);
	void EndBlock(std::thread::id in_threadID);

protected:
	virtual void Initialize() {};

private:

	Profiler();
	virtual ~Profiler();

	std::map<std::thread::id, std::vector<ProfileBlock*>> m_vBlocks;
	std::map<std::thread::id, ProfileBlock*> m_vActiveBlocks;
};

//--------------------------------------------------------------------------------
class AutoProfileBlock
{
public:
	AutoProfileBlock(void* in_sectionID)
	{
		Profiler::GetInstance()->StartBlock(in_sectionID, std::this_thread::get_id());
	}

	~AutoProfileBlock()
	{
		Profiler::GetInstance()->EndBlock(std::this_thread::get_id());
	}
};

#ifdef _DEBUG
#define PROFILE_BLOCK AutoProfileBlock __autoprofileobj( __FUNCTION__ );
#else
#define PROFILE_BLOCK AutoProfileBlock __autoprofileobj( __FUNCTION__ );
#endif
