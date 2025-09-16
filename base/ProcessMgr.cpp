#include "ProcessMgr.h"
#include "mstd/log.h"
#include "mstd/system.h"

ProcessMgr::ProcessMgr():mstd::ThreadMgr<ProcessMgr>("ProcessMgr") {

}

ProcessMgr::~ProcessMgr() {

}

int ProcessMgr::Process1(int a, int b) {
	auto [result, ok] = postFunc([this](int a, int b) {
		mstd::sleep(3000);
		return a + b;
	}, a, b);

	if (!ok) return -1;

	return result.get();
}

void ProcessMgr::Process2(int a) {
	auto add = [this](int a) {
		mstd::sleep(3000);
		LOGFMTI("a=%d", a);
	};

	if (auto [_, ok] = postFunc(add, a); !ok) {
		LOGFMTE("ERROR");
		return;
	}
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