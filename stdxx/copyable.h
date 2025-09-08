
#ifndef __COPYABLE_H_
#define __COPYABLE_H_

namespace stdxx {

class copyable
{
public:
    copyable(const copyable&) = default;
	copyable& operator=(const copyable&) = default;
};

} // namespace stdxx

#endif
