#pragma once

#include "mstd/threadmgr.h"
class ProcessMgr : public mstd::ThreadMgr<ProcessMgr>
{
public:
	ProcessMgr();
	~ProcessMgr();

	int Process1(int a, int b);
	void Process2(int a);
private:
	virtual bool onInit();
	virtual void onUninit();
	virtual void onIdle();
	virtual void onTimer(const uint32_t timer_id);
};

