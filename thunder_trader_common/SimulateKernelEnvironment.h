#pragma once
class MSimulateKernelEnvironment
{
public:
	virtual void OnUpdateProgress(unsigned int cur, unsigned int overall) = 0;
	virtual void OnBackTestFinished() = 0;
};