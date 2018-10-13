#ifndef _COMMONFILES_QUANTTECHSTOCKQUOTATION_QUANTTECHSTOCKQUOTATIONCLISPI_H_
#define _COMMONFILES_QUANTTECHSTOCKQUOTATION_QUANTTECHSTOCKQUOTATIONCLISPI_H_
class CStockTick;
class MQuantTechStockQuotationCliSpi
{
public:
	virtual void Log(const char *) = 0;
	virtual void QuantTechStockQuotationCliSpiOnTick(const CStockTick *) = 0;
};
#endif