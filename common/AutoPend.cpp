#include "AutoPend.h"



CAutoPend::CAutoPend(std::atomic_bool & _IsPending):m_IsPending(_IsPending)
{
	m_IsPending.store(true);
}


CAutoPend::~CAutoPend()
{
	m_IsPending.store(false);
}
