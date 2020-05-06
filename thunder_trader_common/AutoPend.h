#pragma once
#include <atomic>
class CAutoPend
{
	std::atomic_bool & m_IsPending;
public:
	CAutoPend(std::atomic_bool & _IsPending);
	~CAutoPend();
};
