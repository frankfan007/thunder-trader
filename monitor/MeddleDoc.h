#pragma once
#include "afxwin.h"
#include <string>
using namespace std;
class CMeddleDoc :
    public CDocument
{
    DECLARE_DYNCREATE(CMeddleDoc)
public:
    CMeddleDoc();
    ~CMeddleDoc();
    virtual void Serialize(CArchive& ar);
};

