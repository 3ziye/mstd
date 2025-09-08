
#ifndef __TIMEPIECE_H_
#define __TIMEPIECE_H_

namespace stdxx
{   ///  used for measuring and signaling the end of time intervals.
	class timepiece
	{
	public:
		timepiece();
		timepiece(const volatile timepiece&);
		~timepiece();
		timepiece & operator=(const volatile timepiece &);
		void start() volatile;              ///<  	Set the begin time.
		double calc() const volatile;       ///<    Get the intervals from the begin time.
	private:
		struct impl_t;
		impl_t * impl_;
	};

}//end namespace stdxx

#endif

