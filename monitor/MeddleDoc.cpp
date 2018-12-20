#include "stdafx.h"
#include "MeddleDoc.h"

IMPLEMENT_DYNCREATE(CMeddleDoc, CDocument)
CMeddleDoc::CMeddleDoc()
{
}


CMeddleDoc::~CMeddleDoc()
{
}

void CMeddleDoc::Serialize(CArchive & ar)
{
    if (ar.IsStoring())
    {
        // TODO: 在此添加存储代码
    }
    else
    {
        // TODO: 在此添加加载代码
    }
}
