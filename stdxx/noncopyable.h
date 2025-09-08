
#ifndef __NONCOPYABLE_H_
#define __NONCOPYABLE_H_

namespace stdxx {

class noncopyable
{
public:
	// Not copyable or movable
    noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;

protected:
	// The move operations are implicitly disabled, but you can
	// spell that out explicitly if you want:
	noncopyable(noncopyable&&) = delete;
	noncopyable& operator=(noncopyable&&) = delete;
};

} // namespace stdxx

#endif
