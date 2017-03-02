#pragma once

#include "StringUtils.h"
#include "Singleton.h"

#include "../OS_Base/System.h"

#include <unordered_map>
#include <map>
#include <thread>

class ProfileBlock
{
public:
	ProfileBlock(void* in_sectionID) : m_SectionID((char*)in_sectionID) { m_i64StartTime = OS::GetTickCount(); }
	~ProfileBlock() {};

	void StartBlock(ProfileBlock* in_pChild);
	bool StopBlock();

private:
	void Stop();

	__int64 m_i64StartTime = 0;
	__int64 m_i64StopTime = 0;

	float m_fBlockTimeMs = 0.f;

	char* m_SectionID = nullptr;

	std::vector<ProfileBlock*> m_vChildren;
	ProfileBlock* m_pActiveChild = nullptr;
};

class Profiler : public Singleton<Profiler>
{
	MAKE_SINGLETON(Profiler);

public:
	void StartBlock(void* in_sectionID, std::thread::id in_threadID);
	void EndBlock(std::thread::id in_threadID);

protected:
	virtual void Initialize() {};

private:

	Profiler() {};
	virtual ~Profiler() {};

	std::map<std::thread::id, std::vector<ProfileBlock*>> m_vBlocks;
	std::map<std::thread::id, ProfileBlock*> m_vActiveBlocks;
};

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

#define PROFILE_BLOCK AutoProfileBlock __autoprofileobj( __FUNCTION__ );
