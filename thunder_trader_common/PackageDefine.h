#pragma once

#include "StrategyData.h"

using namespace std;
using namespace StrategyData;

namespace TradeServiceNameSpace
{
	enum PacketType{																///数据包类型
		ReqShutdown,
		ReqUsernameList,
		RspUsernameList,
		ReqUsername,
		RspUsername,
		ReqInstrument,
		RspInstrument,
		ReqMeddleResponse,
		RspMeddleResponse,
		ReqStrategy,
		RspStrategy,
		ReqAddUsername,
		RspAddUsername,
		ReqDelUsername,
		RspDelUsername,
		ReqDeployStrategy,
		RspDeployStrategy,
		ReqCancelStrategy,
		RspCancelStrategy,
		ReqMeddle,
		RspMeddle,
		RspError,

	};
#define MaxUsernameNameLength 32
#define MaxPassWordLength 64
#define MaxFilePathLength 512
#define MaxInstrumentIDLength 32
#define MaxMeddleLength 128
#define MaxErrorMessageLength 128
#define MaxMeddleResponseLength 128
#define MaxMeddleResponsePerPacket 32

#ifdef WIN32
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#elif defined(__GNUC__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif



	PACK(struct CDataPacketHeader
	{
		unsigned int m_uDataLength;												//数据包长度
		PacketType m_enumType;													//数据包类型
		unsigned int m_uRequestID;
	});


	PACK(struct CReqShutdownPacket
	{
		CDataPacketHeader m_Header;
	});


	PACK(struct CRspErrorPacket
	{
		CDataPacketHeader m_Header;
		char m_strErrorMessage[MaxErrorMessageLength];
	});


	PACK(struct CReqUsernameListPacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
	});

	PACK(struct CRspUsernameListPacket
	{
		CDataPacketHeader m_Header;
		bool m_boolIsOnline;
		unsigned int m_uOverAllTickCount;
		unsigned int m_uCountOfUsername;
		char m_strUsernames[][MaxUsernameNameLength];
	});

	PACK(struct CReqUsernamePacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		char m_strUsername[MaxUsernameNameLength];
	});

	PACK(struct CRspUsernamePacket
	{
		CDataPacketHeader m_Header;
		unsigned int m_uStrategyCount;
		bool m_boolIsOnline;
		char m_strNetAddress[64];
		char m_strBrokerID[64];
		char m_strInvestorID[64];
		char m_strUsername[MaxUsernameNameLength];
		char m_strPassword[64];
		unsigned int m_arrayStrategies[];
	});


	PACK(struct CReqStrategyPacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		unsigned int m_uStrategyId;

	});

	PACK(struct CPerProfitInfo
	{
		TPriceType m_dbOpenLongWeightingSum_Closed;//已开仓多头加权和
		TVolumeType m_intOpenLongVolume_Closed;//已开仓多头手数
		TPriceType m_dbOpenLongWeightingSum_UnClosed;//已开仓多头加权和
		TVolumeType m_intOpenLongVolume_UnClosed;//已开仓多头手数
		TPriceType m_dbCloseLongWeightingSum;//已平仓多头手数
		TVolumeType m_intCloseLongVolume;//已平仓多头手数

		TPriceType m_dbOpenShortWeightingSum_Closed;//已开仓空头加权和
		TVolumeType m_intOpenShortVolume_Closed;//已开仓空头手数
		TPriceType m_dbOpenShortWeightingSum_UnClosed;//已开仓空头加权和
		TVolumeType m_intOpenShortVolume_UnClosed;//已开仓空头手数
		TPriceType m_dbCloseShortWeightingSum;//已平仓空头手数
		TVolumeType m_intCloseShortVolume;//已平仓空头手数
	});

	PACK(struct CRspStrategyPacket
	{
		CDataPacketHeader m_Header;
		unsigned int m_uStrategyId;
		char m_strUsername[MaxUsernameNameLength];
		char m_strBinPath[MaxFilePathLength];
		char m_strParPath[MaxFilePathLength];
		char m_arrayDataID[LB1_DataEnd][MaxInstrumentIDLength];
		CPerProfitInfo m_arrayProfitInfo[6];
	});


	PACK(struct CReqInstrumentPacket
	{
		CDataPacketHeader m_Header;
		char m_strInstrument[MaxInstrumentIDLength];
	});

	PACK(struct CRspInstrumentPacket
	{
		CDataPacketHeader m_Header;
		unsigned int m_uTickCountUntilNow;
		char m_strInstrument[MaxInstrumentIDLength];
	});
	PACK(struct CReqAddUsernamePacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		char m_strNetAddress[64];
		char m_strBrokerID[64];
		char m_strInvestorID[64];
		char m_strUsername[MaxUsernameNameLength];
		char m_strPassword[64];
	});


	PACK(struct CReqMeddleResponsePacket
	{
		CDataPacketHeader m_Header;
		unsigned int m_uStrategyId;
	});

	PACK(struct CRspMeddleResponsePacket
	{
		CDataPacketHeader m_Header;
		unsigned int m_uCountOfMeddleResponse;
		char m_arrayMeddleResponses[][MaxMeddleResponseLength];
	});

	PACK(struct CRspAddUsernamePacket
	{
		CDataPacketHeader m_Header;
		char m_strUsername[MaxUsernameNameLength];
		int m_intResult;
	});

	PACK(struct CReqDelUsernamePacket
	{
		CDataPacketHeader m_Header;
		char m_strUsername[MaxUsernameNameLength];
	});

	PACK(struct CRspDelUsernamePacket
	{
		CDataPacketHeader m_Header;
		char m_strUsername[MaxUsernameNameLength];
		int m_intResult;
	});

	PACK(struct CReqDeployStrategyPacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		char m_strUsername[MaxUsernameNameLength];
		char m_strBinPath[MaxFilePathLength];
		char m_strParPath[MaxFilePathLength];
		char m_arrayDataID[LB1_DataEnd][MaxInstrumentIDLength];
		char m_arrayExChangeID[LB1_DataEnd][10];
		unsigned int m_uParConfigTextLenght;
		char m_arrayParConfigText[];

	});

	PACK(struct CRspDeployStrategyPacket
	{
		CDataPacketHeader m_Header;
		int m_intResult;
		unsigned int m_uStrategyId;
		char m_strUsername[MaxUsernameNameLength];

	});

	PACK(struct CReqCancelStrategyPacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		unsigned int m_uStrategyId;
	});

	PACK(struct CRspCancelStrategyPacket
	{
		CDataPacketHeader m_Header;
		int m_intResult;
		unsigned int m_uStrategyId;
	});

	PACK(struct CReqMeddlePacket
	{
		CDataPacketHeader m_Header;
		char m_strCmdPassword[MaxPassWordLength];
		unsigned int m_uStrategyId;
		char m_strMeddle[MaxMeddleLength];
	});

	PACK(struct CRspMeddlePacket
	{
		CDataPacketHeader m_Header;
		int m_intResult;
		unsigned int m_uStrategyId;
	});
}


