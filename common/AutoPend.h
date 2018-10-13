#ifndef _COMMONFILES_AUTOPEND_H_
#define _COMMONFILES_AUTOPEND_H_
#include <atomic>
class CAutoPend
{
	std::atomic_bool & m_IsPending;
public:
	CAutoPend(std::atomic_bool & _IsPending);
	~CAutoPend();
};
#endif
