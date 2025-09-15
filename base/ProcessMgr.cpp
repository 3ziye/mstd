#include "ProcessMgr.h"
#include "mstd/log.h"
#include "mstd/system.h"

ProcessMgr::ProcessMgr() :mstd::ThreadMgr<ProcessMgr>("ProcessMgr") {

}

ProcessMgr::~ProcessMgr() {

}

int ProcessMgr::Process1(int a, int b) {
	std::future<int> result = postFunc([this](int a, int b) {
		mstd::sleep(3000);
		return a + b;
	}, a, b);

	return result.get();
}

int ProcessMgr::Process2(int a, int b) {
	auto add = [this](int a, int b) {
		mstd::sleep(3000);
		return a + b;
	};

	std::future<int> result = postFunc(add, a, b);
	return result.get();
}

bool ProcessMgr::onInit() {
	LOGFMTI("OnInit");
	return true;
}

void ProcessMgr::onUninit() {
	LOGFMTI("OnUninit");
}

void ProcessMgr::onIdle() {

}

void ProcessMgr::onTimer(const uint32_t timer_id) {
	LOGFMTI("OnTimer %d", timer_id);
}