/**
 *
 * EditEx.h - This file contains the definition of the class CEditEx.This class
   is derived from CEdit and is used by the class CListCtrlEx.
 * @author:     Shine Kumar
 * @version:    1.00    Date: 3/8/2004
 */
//#include "stdafx.h"
#ifndef _EDITEX_H_
#define _EDITEX_H_



class CEditEx : public CEdit
{
public:
    // Constructor
    CEditEx();
    // Destructor
    virtual ~CEditEx();

protected:
    // This function is invoked when the edit box is lost its focus.
    // When the focus is lost the Text in this edit box is placed as the List
    // control text of class CListCtrlEx
    void OnKillFocus( CWnd* pNewWnd_i );

    DECLARE_MESSAGE_MAP()
};

#endif // #ifndef _EDITEX_H_
