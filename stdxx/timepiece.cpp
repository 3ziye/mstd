#include "platform_define.h"
#include "timepiece.h"

#ifdef STDXX_WINDOWS
	#include <windows.h>
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
	#include <sys/time.h>
#endif

namespace stdxx
{
		//class timepiece
		struct timepiece::impl_t
		{
#if defined(STDXX_WINDOWS)
			LARGE_INTEGER beg_timestamp;
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			struct timeval beg_timestamp;
#endif
		};

		timepiece::timepiece()
			: impl_(new impl_t)
		{}

		timepiece::timepiece(const volatile timepiece& rhs)
			: impl_(new impl_t(*rhs.impl_))
		{}

		timepiece::~timepiece()
		{
			delete impl_;
		}

		timepiece & timepiece::operator=(const volatile timepiece & rhs)
		{
			if(this != &rhs)
				*impl_ = *rhs.impl_;

			return *this;
		}

		void timepiece::start() volatile
		{
#if defined(STDXX_WINDOWS)
			::QueryPerformanceCounter(&impl_->beg_timestamp);
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			struct timezone tz;
			::gettimeofday(&impl_->beg_timestamp, &tz);
#endif
		}

		double timepiece::calc() const volatile
		{
#if defined(STDXX_WINDOWS)
			LARGE_INTEGER li;
			::QueryPerformanceCounter(&li);

			__int64 diff = li.QuadPart - impl_->beg_timestamp.QuadPart;

			LARGE_INTEGER freq;
			::QueryPerformanceFrequency(&freq);

			return double(diff)/double(freq.QuadPart) * 1000;
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv, &tz);
			return static_cast<double>(tv.tv_sec - impl_->beg_timestamp.tv_sec) * 1000 + static_cast<double>(tv.tv_usec - impl_->beg_timestamp.tv_usec) / 1000;
#endif
		}
	//end class timepiece

}//end namespace stdxx
