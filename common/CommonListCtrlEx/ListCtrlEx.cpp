 /**
 *
 * ListCtrlEx.cpp *-Implementation of the class CListCtrlEx.
 * @author:     Shine Kumar
 * @version:    1.00    Date: 3/26/2004
 */

#include "stdafx.h"
#include "EditEx.h"
#include "ListCtrlEx.h"
#include "ComboBoxExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/*  *************************************************************************** */
/** 
@fn:		CListCtrlEx
@brief:		Constructor    
@param      Nil
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
CListCtrlEx::CListCtrlEx()
{
    // Initialize members
    mnCurrentRowNo    = 0;
    mnCurrentColumnNo = 0;
    mpeditListEdit    = NULL;
    mpcomboListCombo  = NULL;
}

/*  *************************************************************************** */
/** 
@fn:		~CListCtrlEx
@brief:		Destructor    
@param      Nil
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
CListCtrlEx::~CListCtrlEx()
{
}

/*  *************************************************************************** */
/** 
@fn:		SetEditBox
@brief:		This function will set a cell of a list control editable. It adds
            the index of the row and column,which is the parameters passed,into the array
            m_carEditableItems.
@param      int nItemIndex_i
                The index of the item(row). If this is -1, it means the entire cells in
                the row is editable.
@param      int nSubItemIndex_i
                The index of the sub item(column). If this is -1, it means the entire 
                cells in the column is editable.
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::SetEditBox(int nItemIndex_i, int nSubItemIndex_i)
{
    SEditableCell stEditableCell;
    // Initialize members of Editable cells
    stEditableCell.nRow = nItemIndex_i;
    stEditableCell.nCol = nSubItemIndex_i;
    // Add the cell info into the array
    mastEditableItems.Add(stEditableCell);    
    return;
}

/*  *************************************************************************** */
/** 
@fn:		SetComboBox
@brief:		This function will set combo box in a
@param      int nRow_i
                The row to be added into the map
@param      int nCol_i
                The row to be added into the map
@param      CStringArray& csarValues_i
                The String values to be added into the map
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::SetComboBox(int nRow_i,
                              int nCol_i,
                              CStringArray& csarValues_i)
{

	CString csComboValues;
	AddComboStrings(csComboValues, csarValues_i);
    SComboCell stCombo;
    stCombo.nRow = nRow_i;
    stCombo.nCol = nCol_i;
    stCombo.csItem = csComboValues;
    mastComboItems.Add(stCombo);
    return;
}

/*  *************************************************************************** */
/** 
@fn:		Clear
@brief:		This function will delete the contents of the given column
@param      int nCoulmn_i
                The column that should be cleared
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::Clear(int nCoulmn_i)
{
    for ( int nIndex = 0; nIndex < GetItemCount(); ++nIndex)
    {
        CString csText = _T("");
        SetItemText(nIndex, nCoulmn_i, csText);
    }
    return;
}

/*  *************************************************************************** */
/** 
@fn:		OnVScroll
@brief:		The framework calls this member function when the user clicks the
            window's vertical scroll bar. This is used here to hide the edit
            box while scrolling the list control.
@param      UINT nSBCode_i
                Specifies a scroll-bar code that indicates the user's scrolling
                request.
@param      UINT nPos_i
                Contains the current scroll-box position if the scroll-bar code
                is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise not used.
                Depending on the initial scroll range, nPos may be negative and
                should be cast to an int if necessary.
@param      CScrollBar* pScrollBar_i
                If the scroll message came from a scroll-bar control, contains
                a pointer to the control. If the user clicked a window's
                scroll bar, this parameter is NULL. The pointer may be temporary
                and should not be stored for later use.
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::OnVScroll(UINT nSBCode_i,
                            UINT nPos_i,
                            CScrollBar* pScrollBar_i)
{
    if (GetFocus() != this)
    {
        // Set the focus to the list
        SetFocus();
    }
    CListCtrl::OnVScroll(nSBCode_i, nPos_i, pScrollBar_i);
    return;
}

/*  *************************************************************************** */
/** 
@fn:		OnVScroll
@brief:		The framework calls this member function when the user clicks the
            window's horizontal scroll bar. This is used here to hide the edit
            box while scrolling the list control.
@param      UINT nSBCode_i
                Specifies a scroll-bar code that indicates the user's scrolling
                request.
@param      UINT nPos_i
                Contains the current scroll-box position if the scroll-bar code
                is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise not used.
                Depending on the initial scroll range, nPos may be negative and
                should be cast to an int if necessary.
@param      CScrollBar* pScrollBar_i
                If the scroll message came from a scroll-bar control, contains
                a pointer to the control. If the user clicked a window's
                scroll bar, this parameter is NULL. The pointer may be temporary
                and should not be stored for later use.
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::OnHScroll(UINT nSBCode_i,
                            UINT nPos_i,
                            CScrollBar* pScrollBar_i )
{
    if (GetFocus() != this)
    {
        // Set the focus to the list
        SetFocus();
    }
    CListCtrl::OnHScroll(nSBCode_i, nPos_i, pScrollBar_i);
    return;
}

/*  *************************************************************************** */
/** 
@fn:		OnLButtonDown
@brief:		This function will display an edit box when the user click on a
            cell.The edit box will be displayed only if the  clicked column is
            editable. The editable column are stored in a Array.If the clicked
            column is found then the edit box is displayed.
@@param     nFlags_i 
                Indicates whether various virtual keys are down
@param      Point_i
                Specifies the x- and y-coordinate of the cursor. These
                coordinates are always relative to the upper-left corner
                of the window.
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::OnLButtonDown(UINT nFlags_i, CPoint Point_i )
{
    CListCtrl::OnLButtonDblClk(nFlags_i, Point_i);
    LVHITTESTINFO lvHitTestInfo;
    lvHitTestInfo.pt = Point_i;
    // Get the information about the selected sub item
    int nError = SubItemHitTest(&lvHitTestInfo);
    if (-1 != nError)
    {
        CRect cRect;
        // Get the boundary of the selected cell
        GetSubItemRect(lvHitTestInfo.iItem,
                       lvHitTestInfo.iSubItem,
                       LVIR_LABEL,
                       cRect);
        mnCurrentRowNo = lvHitTestInfo.iItem;
        mnCurrentColumnNo = lvHitTestInfo.iSubItem;
        // Modify the rectangle to adjust the size
        if ( 0 == lvHitTestInfo.iSubItem)
        {
            cRect.right = cRect.left + GetColumnWidth(0);
        }
        // Get the current font
        // Is the Selected column is editable?
        CString csComboStrings = _T("");
        if (IsComboSet(mnCurrentRowNo, mnCurrentColumnNo, csComboStrings)) 
        {
            DisplayComboBox(cRect, csComboStrings);
        }
        else if (IsEditable(mnCurrentRowNo, mnCurrentColumnNo))
        {
            DisplayEditBox(cRect);
        }
    }
    return;
}

/*  *************************************************************************** */
/** 
@fn:		OnRButtonDown
@brief:		This function will be invoked when the Right button of mouse is
            clicked on the list control. It is used to display an edit box in
            those cells in which combo boxes are set
@@param     nFlags_i 
                Indicates whether various virtual keys are down
@param      Point_i
                Specifies the x- and y-coordinate of the cursor. These
                coordinates are always relative to the upper-left corner
                of the window.
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::OnRButtonDown(UINT nFlags_i, CPoint Point_i)
{

    CListCtrl::OnRButtonDown(nFlags_i, Point_i);
    LVHITTESTINFO lvHitTestInfo;
    lvHitTestInfo.pt = Point_i;
    // Get the information about the selected sub item
    int nError = SubItemHitTest(&lvHitTestInfo);
    if (-1 != nError)
    {
        CRect cRect;
        // Get the boundary of the selected cell
        GetSubItemRect(lvHitTestInfo.iItem,
                       lvHitTestInfo.iSubItem,
                       LVIR_BOUNDS, cRect);
        mnCurrentRowNo = lvHitTestInfo.iItem;
        mnCurrentColumnNo = lvHitTestInfo.iSubItem;
        CString csComboStrings = _T( "" );;
        if (IsComboSet(mnCurrentRowNo, mnCurrentColumnNo, csComboStrings))
        {
            DisplayEditBox(cRect);          
        }
    }   
    return;
} 

/*  *************************************************************************** */
/** 
@fn:		OnCustomDraw
@brief:		This function is used to change the color of individual rows. We
            should add the rows needed to change color into the member array
            and the it will change the color.
@param      NMHDR* pNMHDR_i
                Notification Header
@param      LRESULT* pResult_i
                Result of Custom Draw
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR_i, LRESULT* pResult_i)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR_i);
    *pResult_i = CDRF_DODEFAULT;
    COLORREF crText(COLOR_BLACK);
    COLORREF crBack(COLOR_WHITE);
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult_i = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        for (int nIndex = 0; nIndex < masColoredRows.GetSize(); ++nIndex)
        {
            if (pLVCD->nmcd.dwItemSpec == masColoredRows[nIndex].mnRow)
            {
                crText = masColoredRows[nIndex].mcolTextColor;
                crBack = masColoredRows[nIndex].mcolBkColor;
            }
        }
        // Store the color back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crText;
        pLVCD->clrTextBk = crBack;

        // Tell Windows to paint the control itself.
        *pResult_i = CDRF_NOTIFYSUBITEMDRAW;
    }
    return;
}

/*  *************************************************************************** */
/** 
@fn:		HideEditBox
@brief:		This function will add the given no to the m_carColoredRows. Later it will
            be used for coloring items
@param      int nRow_i
                Row to be added
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::AddColoredRow(int nItemIndex,
                                COLORREF colTextColor,
                                COLORREF colBkColor)
{
    int nIndex;
    SColoredRow stColored;
    if (true == SearchForColoredItem(nItemIndex, nIndex))
    {
        stColored.mnRow = nItemIndex;
        stColored.mcolTextColor = colTextColor;
        stColored.mcolBkColor = colBkColor;
        masColoredRows.SetAt(nIndex, stColored);
    }
    else
    {
        stColored.mnRow = nItemIndex;
        stColored.mcolTextColor = colTextColor;
        stColored.mcolBkColor = colBkColor;
        // Add the info into the array.
        masColoredRows.Add(stColored);
    }
    RedrawWindow();
    return;
}

/*  *************************************************************************** */
/** 
@fn:		HideEditBox
@brief:		Hides the edit box        
@param      Nil
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::HideEditBox()
{
    if (NULL != mpeditListEdit)
    {
        // Hide the edit box
        mpeditListEdit->ShowWindow(SW_HIDE);
        delete mpeditListEdit;
        mpeditListEdit = NULL;
    }
    return;
}

/*  *************************************************************************** */
/** 
@fn:		DisplayEditBox
@brief:		This function will display an edit box in the second column of the current
            selected row.            
@param      CRect& crRect_i
                The length and width of the edit box will be set using this object
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::DisplayEditBox(CRect& crRect_i)
{
    // Is edit box there?
    if (NULL != mpeditListEdit)
    {
        // Kill the focus of edit box
        SetFocus();
        // Delete edit box
        delete mpeditListEdit;
        mpeditListEdit = NULL;
    }
    CFont* pFont = GetFont();
    TCHAR tcText[MAX_CHAR_COUNT];
    // Create edit box
    mpeditListEdit = new CEditEx;
    mpeditListEdit->Create(EDIT_BOX_STYLE,
                           crRect_i,
                           this,
                           ID_EDIT);
    // Show edit box
    mpeditListEdit->ShowWindow(SW_SHOW);
    // Set memory for the text in edit box
    memset(tcText, 0, MAX_CHAR_COUNT);
    // Get the text in the selected cell
    GetItemText(mnCurrentRowNo,
                mnCurrentColumnNo,
                tcText,
                MAX_CHAR_COUNT);
    // Set the font for edit box text
    mpeditListEdit->SetFont(pFont);
    // Add the text of the selected cell into the edit box
    mpeditListEdit->SetWindowText(tcText);
    // Select the edit box text
    mpeditListEdit->SetSel(0, -1); // 0, -1 combination is for selecting 
                                   // the entire text
    mpeditListEdit->SetFocus();
    return;
}

/*  *************************************************************************** */
/** 
@fn:		DisplayComboBox
@brief:		This function will display a combo box with the measurements of
            the given CRect object. The combo bob will be displayed in the
            row corresponding to the value of mnCurrentRowNo and in the second
            column
@param:		crRect_i
                The length and width of the combo box will be set using this
                object
@param:     csValues_i
                The strings that should be displayed in the combo box
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::DisplayComboBox( CRect& crRect_i, CString csValues_i )
{
    // Is already combo box is there?
    if( mpcomboListCombo )
    {
        // Kill the focus of combo box
        SetFocus();
        // Delete combo box
        delete mpcomboListCombo;
        mpcomboListCombo = NULL;
    }
    // Get the current font
    CFont* pFont = GetFont();
    // Create combo box
    mpcomboListCombo = new CComboBoxExt;
    crRect_i.bottom += COMBO_HEIGHT;
    mpcomboListCombo->Create(WS_CHILD |
                             WS_VISIBLE |
                             WS_TABSTOP |
                             CBS_DROPDOWNLIST |
                             WS_VSCROLL |
                             WS_HSCROLL,
                             crRect_i,
                             this,
                             ID_COMBO);

    // Get the Values needed to display in the combo box and add into the combo
    // box
    CStringArray csarValues;
    Tokenize(csValues_i,SEPERATOR_STRING,csarValues );
    for( int nIndex = 0 ;nIndex < csarValues.GetSize(); nIndex++ )
    {
        mpcomboListCombo->AddString( csarValues.GetAt( nIndex ));
    }
    // Show the combo box Set font and Set focus
    mpcomboListCombo->SetFont( pFont );
    //mpcomboListCombo->ShowDropDown( SW_SHOW );
    mpcomboListCombo->SetFocus();
    CString csCurrentItem = GetItemText( mnCurrentRowNo, mnCurrentColumnNo );
    // Select the current string in the combo box
    if( CB_ERR == mpcomboListCombo->SelectString( -1, csCurrentItem ))
    {
        mpcomboListCombo->SetCurSel ( mpcomboListCombo->AddString( csCurrentItem ));
    }
    return;
}

/*  *************************************************************************** */
/** 
@fn:		IsEditable
@brief:		The purpose of this function is to findout whether a column of a
            list control is editable. The editable columns are stored in the
            m_carEditableItems this function will search the array to find out
            whether a column of a list control is editable.
@param:		nSubItemIndex_i
                
@return:	bool
                True if editable; Otherwise False
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
bool CListCtrlEx::IsEditable( int nItemIndex_i, int nSubItemIndex_i )
{
    bool bReturn =  false;
    SEditableCell stEditable;
    stEditable.nRow = nItemIndex_i;
    stEditable.nCol = nSubItemIndex_i;
    // Taking each columns from the array
    for (int nIndex = 0; nIndex < mastEditableItems.GetSize(); ++nIndex)
    {
        // Check whether editable
        if (ROW_ALL == nItemIndex_i &&
            COLUMN_ALL == nSubItemIndex_i)
        {
            bReturn = true;
            break;
        }
		if (ROW_ALL == nItemIndex_i &&
           (mastEditableItems[nIndex].nCol) == nSubItemIndex_i)
		{
            bReturn = true;
            break;
		}
		
		if ((mastEditableItems[nIndex].nRow) == nItemIndex_i &&
            COLUMN_ALL == nSubItemIndex_i)
		{
            bReturn = true;
            break;
		}
        if (stEditable == mastEditableItems.GetAt(nIndex))
        {
            bReturn = true;
            break;
        }
    }
    return bReturn;
}


/**
 * This function will check the map object for the given row and if found
 * return true with the string for the matched row. Otherwise it return false
 *
 * @param       nRow_i    - The row for searching
 * @param       csValue_o - The string corresponding to the matched row
 * @return      bool      - true if searching is success else false
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 * @serial      Nil
 * @deprecated  Nil
 */
bool CListCtrlEx::IsComboSet(int nRow_i, int nCol_i, CString& csValue_o)
{
    bool bReturn =  false;
    SComboCell stCombo;
    stCombo.nRow = nRow_i;
    stCombo.nCol = nCol_i;
    // Taking each columns from the array
    for( int nIndex = 0; nIndex < mastComboItems.GetSize(); nIndex++ )
    {
        SComboCell stComboTemp = mastComboItems.GetAt( nIndex );      
        // Check whether editable
        if( stCombo == stComboTemp)
        {
            csValue_o = stComboTemp.csItem;
            bReturn = true;
            break;
        }
    }
    return bReturn;
}


/** 
 * This function will remove all item from the colored items array.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since		1.0
 * @serial      Nil
 * @deprecated  Nil
 */ 
void CListCtrlEx::RemoveAllColoredRows()
{
    masColoredRows.RemoveAll();
    return;
}

/*  *************************************************************************** */
/** 
@fn:		OnToolHitTest
@brief:		
@param:		CPoint& point
                Point to be tested.
@param:     RECT* cellrect
                To hold the bounding rect
@param:     int* col
                To hold the column index
@return:	int
                Row index on success, -1 otherwise
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
int CListCtrlEx::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	int row, col;
	RECT cellrect;
	row = CellRectFromPoint(point, &cellrect, &col );

	if ( row == -1 ) 
	{
		return -1;
	}

	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	pTI->rect = cellrect;

	return (int)pTI->uId;
}

/*  *************************************************************************** */
/** 
@fn:		CellRectFromPoint
@brief:		Determine the row, col and bounding rect of a cell
@param:		CPoint& point
                Point to be tested.
@param:     RECT* cellrect
                To hold the bounding rect
@param:     int* col
                To hold the column index
@return:	int
                Row index on success, -1 otherwise
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
int CListCtrlEx::CellRectFromPoint(CPoint& point, RECT* cellrect, int* col) const
{
	int colnum;
	int nColumnCount;
	CHeaderCtrl* pHeader;

	// If not REPORT style for the list, then use 1 column
	// Otherwise, find the column in the report style view.
	if (LVS_REPORT != (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK))
	{
		// Window is not Report style, so no headers. Deal with it!
		nColumnCount = 1;
	}
	else
	{
		// Get the number of columns in Report style.
		pHeader = (CHeaderCtrl*)GetDlgItem(0);
		nColumnCount = pHeader->GetItemCount();
	}
	
	// Get the top and bottom row visible
	int row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if (bottom > GetItemCount())
    {
		bottom = GetItemCount();
    }
	
	// Loop through the visible rows
	for (;row <= bottom; ++row)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect, rectLabel;
		if (LVS_REPORT != (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK))
		{
			// If not Report style, get label / icon boundaries.
			GetItemRect(row, &rect, LVIR_ICON);
			GetItemRect(row, &rectLabel, LVIR_LABEL);
		}
		else
		{
			// If Report style, get bounds of cell.
			GetItemRect(row, &rect, LVIR_BOUNDS);
		}

		if (rect.PtInRect(point) ||
			rectLabel.PtInRect(point))
		{
			bool InLabel = false;

			// Store which part user clicked on if in ICON view,
			// clicked the Label or the Icon?
			if (rect.PtInRect(point))
			{
				InLabel = false;
			}
			else if (rectLabel.PtInRect(point))
			{
				InLabel = true;
			}

			// Now find the column
			for (colnum = 0; colnum < nColumnCount; ++colnum)
			{
				// Getting column width
				int colwidth;
				if (LVS_REPORT != (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK))
				{
					colwidth = 100000; // don't count colwidth.
				}
				else
				{
					// If REPORT, check inside column width.
					colwidth = GetColumnWidth(colnum);
				}

				if ((point.x >= rect.left 
					&& point.x <= (rect.left + colwidth)) ||
					(point.x >= rectLabel.left
					&& point.x <= (rectLabel.left + colwidth)))
				{
					RECT rectClient;
					GetClientRect(&rectClient);
					if(NULL != col)
                    {
                        *col = colnum;
                    }
					rect.right = rect.left + colwidth;
					rectLabel.right = rectLabel.left + colwidth;

					// Make sure that the right extent does not exceed
					// the client area
					if (rect.right > rectClient.right) 
                    {
						rect.right = rectClient.right;
                    }
					if (rectLabel.right > rectClient.right)
                    {
						rectLabel.right = rectClient.right;
                    }
					if (!InLabel)
					{
						*cellrect = rect;
					}
					else
					{
						*cellrect = rectLabel;
					}
					return row;
				}
				rect.left += colwidth;
				rectLabel.left += colwidth;
			}
		}
	}

	return -1;
}

/*  *************************************************************************** */
/** 
@fn:		OnToolTipText
@brief:		Display the tooltip text whel mouse over on a cell. Modify this
            function to change the text displayed in the Tool Tip. Currently,
            it uses GetItemText() to just show the item's name.
@param:		UINT id
@param:		NMHDR* pNMHDR
@param:		LRESULT* pResult
@return:	BOOL
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
BOOL CListCtrlEx::OnToolTipText(UINT id,
                                NMHDR* pNMHDR,
                                LRESULT* pResult)
{
	// Need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;

	if (0 == nID)	  	// Notification in NT from automatically
    {
        return FALSE;   	// created tooltip
    }
	int row = ((nID-1) >> 10) & 0x3fffff ;
	int col = (nID-1) & 0x3ff;

	// Use Item's name as the tool tip. Change this for something different.
	// Like use its file size, etc.
	strTipText = GetItemText(row, col);

#ifndef _UNICODE

	if (pNMHDR->code == TTN_NEEDTEXTA)
    {
		lstrcpyn(pTTTA->szText, strTipText, 80);
    }
	else
    {
		_mbstowcsz(pTTTW->szText, strTipText, 80);
    }
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
    {
		_wcstombsz(pTTTA->szText, strTipText, 80);
    }
	else
    {
		lstrcpyn(pTTTW->szText, strTipText, 80);
    }
#endif
	*pResult = 0;

	return TRUE;    // message was handled
}

/*  *************************************************************************** */
/** 
@fn:		PreSubclassWindow
@brief:		
@param:		Nil
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	// Add initialization code
	EnableToolTips(TRUE);
    return;
}

/*  *************************************************************************** */
/** 
@fn:		GetCellInfo
@brief:		This function will return the current values of mnCurrentRowNo
            and mnCurrentColumnNo
@param:		int& nRow_o
                value of mnCurrentRowNo
@param:		int& nCol_o
                value of mnCurrentColumnNo
@return:	void
@pre:		Nil
@todo:		Nil
@warning:	Nil
@author:	Shine Kumar
@version:	1.0
@date:		13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::GetCellInfo(int& nRow_o, int& nCol_o)
{
    nRow_o = mnCurrentRowNo;
    nCol_o = mnCurrentColumnNo;
    return;
}
/*  *************************************************************************** */
/** 
@fn     :	AddComboStrings
@param  :	CString& csValues_o
@param  :	CStringArray& csarValues_i
@return :	void
@pre    :	Nil
@todo   :	Nil
@warning:	Nil
@author :	Shine Kumar
@version:	1.0
@date   :	13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::AddComboStrings(CString& csValues_o, CStringArray& csarValues_i)
{	
	csValues_o = _T(""); int nIndex;
	for (nIndex = 0; nIndex < csarValues_i.GetSize() - 1; ++nIndex)
	{
		csValues_o += csarValues_i[nIndex];
		csValues_o += SEPERATOR_STRING;
	}
	csValues_o += csarValues_i[nIndex];
	return;
}

/*  *************************************************************************** */
/** 
@fn     :	Tokenize
@brief  :		
@param  :	const CString& csToken_i
@param  :	WCHAR* pwcDelimit_i
@param  :	CStringArray& csarData_o
@return :	void
@pre    :	Nil
@todo   :	Nil
@warning:	Nil
@author :	Shine Kumar
@version:	1.0
@date   :	13-10-2005
*/
/*  *************************************************************************** */
void CListCtrlEx::Tokenize(const CString& csToken_i,
                           WCHAR* pwcDelimit_i,
                           CStringArray& csarData_o)
{
    csarData_o.RemoveAll();
    int nSize = csToken_i.GetLength() + 1;
    LPWSTR pwcToken = new WCHAR[nSize];
	wcscpy_s(pwcToken, nSize,csToken_i);
    CString csData = wcstok_s(pwcToken,pwcDelimit_i,NULL);
    while (false == csData.IsEmpty())
    {
        csarData_o.Add(csData);
        csData = wcstok_s(NULL, pwcDelimit_i,NULL);
    }
    delete []pwcToken;
    return;
}


bool CListCtrlEx::SearchForColoredItem(int nItem, int& nItemIndex)
{
    bool bReturn = false;
    nItemIndex = -1;
    for (int nIndex = 0; nIndex < masColoredRows.GetSize(); ++nIndex)
    {
        if (nItem == masColoredRows.GetAt(nIndex).mnRow)
        {

            nItemIndex = nIndex;
            bReturn = true;
            break;
        }
    }
    return bReturn;
}