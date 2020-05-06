#ifndef _CommonFiles_QuantFundHFTBackTestTypedefine_h_
#define _CommonFiles_QuantFundHFTBackTestTypedefine_h_
#include <vector>
#include <string>
#include "StrategyData.h"
#include "StrategyContext.h"
using namespace std;
typedef vector<
	pair<
		string/*Type*/,
		vector<
			std::tuple<
				string/*Name*/,
				TProbeColorType/*Color*/,
				std::shared_ptr<vector<unsigned int>>/*Index*/,
				std::shared_ptr<vector<float>>/*Value*/
			>/*Data*/
		>
	>
> TNormalGraphProbeInfoType;
typedef vector<
		vector<
			std::tuple<
				string/*Name*/,
				TProbeColorType/*Color*/,
				std::shared_ptr<vector<unsigned int>>/*Index*/,
				std::shared_ptr<vector<float>>/*Value*/
			>
		>
> TCandlesticksGraphProbeInfoType;

#endif
