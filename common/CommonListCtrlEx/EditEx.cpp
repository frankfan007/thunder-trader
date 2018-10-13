/**
 * EditEx.cpp -To be used by the class CListCtrlEx.To implement the OnKillfocus
 * message map.
 * @author:     Shine Kumar
 * @version:    1.00    Date: 3/8/2004
*/


#include "stdafx.h"
#include "EditEx.h"
//#include "resource.h"
#include "ListCtrlEx.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * CEditEx Constructor
 *
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 * @serial      Nil
 * @deprecated  Nil
 */
CEditEx::CEditEx ()
{
    // Nothing is doing here
}


/**
 * CEditEx Destructor
 *
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 * @serial      Nil
 * @deprecated  Nil
 */
CEditEx::~CEditEx()
{

}


//The ClassWizard will add member initialization here
BEGIN_MESSAGE_MAP( CEditEx, CEdit )
    ON_WM_KILLFOCUS( )
END_MESSAGE_MAP()


/** 
 * This function is used to set the text in the edit ctrl as athe text in the
 * listctrl when the focus of the Edit control is lost.
 * 
 * @param       pNewWnd_i - window pointer
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since		1.0
 * @serial      Nil
 * @deprecated  Nil
 */ 
 void CEditEx::OnKillFocus( CWnd* pNewWnd_i )
{
    CEdit::OnKillFocus( pNewWnd_i );
    CString csText = _T( "" );
    // Get the text
    GetWindowText( csText );
    CListCtrlEx* pList = static_cast<CListCtrlEx*>(GetParent());
    int nRow = -1;
    int nCol = -1;
    pList->GetCellInfo( nRow, nCol );
     // Set the text into the current editable cell of the List Ctrl.
    pList->SetItemText( nRow, nCol, csText );
    // Hide the edit box
    ShowWindow( SW_HIDE );
    // Delete the edit box
    PostMessage( WM_DESTROY, 0, 0 );
    return;
}


