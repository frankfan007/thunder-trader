#ifndef _QFCOMPRETRADESYSTEM_ORDERREFRESOLVE_H_
#define _QFCOMPRETRADESYSTEM_ORDERREFRESOLVE_H_

template<int N>
struct PowerOfTwo {
	static const long long val = 2 * PowerOfTwo<N - 1>::val;
};
template<>
struct PowerOfTwo<0> {
	static const long long val = 1 ;
};

#define _MaskFor1Bit 0x1
#define _MaskFor2Bit 0x3
#define _MaskFor3Bit 0x7
#define _MaskFor4Bit 0xF
#define _MaskFor5Bit 0x1F
#define _MaskFor6Bit 0x3F
#define _MaskFor7Bit 0x7F
#define _MaskFor8Bit 0xFF

#define _StrategyCustomBitCount 3
#define _MaskForStrategyCustom _MaskFor3Bit

#define _StrategyIDBitCount 8
#define _MaskForStrategyID _MaskFor8Bit

#define _OrderDirectionBitCount 1
#define _MaskForOrderDirection _MaskFor1Bit

#define _OrderOffsetBitCount 2
#define _MaskForOrderOffset _MaskFor2Bit

#define _SystemNumberBitCount 3
#define _MaskForSystemNumber _MaskFor3Bit

#define _AccountNumberBitCount 4
#define _MaskForAccountNumber _MaskFor4Bit



#define _MaxStrategyCustom (PowerOfTwo<_StrategyCustomBitCount>::val-1)
#define _MaxStrategyID (PowerOfTwo<_StrategyIDBitCount>::val-1)
#define _MaxOrderDirection (PowerOfTwo<_OrderDirectionBitCount>::val-1)
#define _MaxOrderOffset (PowerOfTwo<_OrderOffsetBitCount>::val-1)
#define _MaxSystemNumber (PowerOfTwo<_SystemNumberBitCount>::val-1)
#define _MaxAccountNumber (PowerOfTwo<_AccountNumberBitCount>::val-1)



#define _OrderRef2StrategyCustomPart(OrderRef)	( (OrderRef) & _MaskForStrategyCustom)
#define _OrderRef2StrategyIDPart(OrderRef)		(((OrderRef)>>(_StrategyCustomBitCount)) & _MaskForStrategyID)
#define _OrderRef2OrderDirectionPart(OrderRef)	(((OrderRef)>>(_StrategyCustomBitCount+_StrategyIDBitCount)) & _MaskForOrderDirection)
#define _OrderRef2OrderOffsetPart(OrderRef)		(((OrderRef)>>(_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount)) & _MaskForOrderOffset)
#define _OrderRef2SystemNumberPart(OrderRef)	(((OrderRef)>>(_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount)) & _MaskForSystemNumber)
#define _OrderRef2AccountNumberPart(OrderRef)	(((OrderRef)>>(_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount+_SystemNumberBitCount)) & _MaskForAccountNumber)

#define _OrderRef2OrderIncreasePart(OrderRef)	(((OrderRef)>>(_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount+_SystemNumberBitCount+_AccountNumberBitCount)))

#define _StrategyCustom2OrderRefPart(StrategyCustom)	((StrategyCustom)	&	_MaskForStrategyCustom)
#define _StrategyID2OrderRefPart(StrategyID)			(((StrategyID)		&	_MaskForStrategyID)		<< (_StrategyCustomBitCount))
#define _OrderDirection2OrderRefPart(OrderDirection)	(((OrderDirection)	&	_MaskForOrderDirection)	<< (_StrategyCustomBitCount+_StrategyIDBitCount))
#define _OrderOffset2OrderRefPart(OrderOffset)			(((OrderOffset)		&	_MaskForOrderOffset)	<< (_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount))
#define _SystemNumberPart2OrderRefPart(SystemPart)		(((SystemPart)		&	_MaskForSystemNumber)	<< (_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount))

#define _AccountNumberPart2OrderRefPart(AccountPart)	(((AccountPart)		&	_MaskForAccountNumber)	<< (_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount+_SystemNumberBitCount))




#define _OrderIncreasePart2OrderRefPart(OrderIncreasePart)	(((OrderIncreasePart)) << (_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount+_SystemNumberBitCount+_AccountNumberBitCount))

#define _OrderRefBase(StrategyCustom,StrategyID,OrderDirection,OrderOffset,AccountPart,SystemPart) \
			(_StrategyCustom2OrderRefPart(StrategyCustom)+_StrategyID2OrderRefPart(StrategyID)+_OrderDirection2OrderRefPart(OrderDirection) \
			+_OrderOffset2OrderRefPart(OrderOffset)+_SystemNumberPart2OrderRefPart(SystemPart)+_AccountNumberPart2OrderRefPart(AccountPart))

#define _OrderRef(OrderNumber,Base) (((OrderNumber)<<(_StrategyCustomBitCount+_StrategyIDBitCount+_OrderDirectionBitCount+_OrderOffsetBitCount+_SystemNumberBitCount)+_AccountNumberBitCount)+(Base))

#endif
