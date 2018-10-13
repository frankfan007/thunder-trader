#ifndef _LIST_CTRL_COMMON_H_
#define _LIST_CTRL_COMMON_H_

#define SEPERATOR_STRING _T( "$" )

#define EDIT_BOX_STYLE      WS_CHILD | WS_VISIBLE |WS_TABSTOP | WS_BORDER | \
                            ES_LEFT
#define LIST_CONTROL_STYLE  LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | \
                            LVS_EX_ONECLICKACTIVATE

#define ID_EDIT             100
#define ID_COMBO            110
#define COMBO_HEIGHT        100
#define MAX_CHAR_COUNT      255
#define COLOR_BLACK         RGB(0, 0, 0)
#define COLOR_WHITE         RGB(255, 255, 255)
#define COLUMN_ALL          -1
#define ROW_ALL             -1

struct SEditableCell
{
    int nRow;
    int nCol;
    bool operator ==(SEditableCell& stEditable)
    {
        bool bRet = false;
        if ((stEditable.nRow == nRow || stEditable.nRow == ROW_ALL) &&
            (stEditable.nCol == nCol || stEditable.nCol == COLUMN_ALL))
        {
            bRet = true;
        }
        return bRet;
    }
};

struct SComboCell
{
    int nRow;
    int nCol;
    CString csItem;
    bool operator ==(SComboCell& stCombo)
    {
        bool bRet = false;
        if ((stCombo.nRow == nRow || stCombo.nRow == -1) &&
           ( stCombo.nCol == nCol || stCombo.nCol == -1))
        {
            bRet = true;
        }
        return bRet;
    }
};

struct SColumnInfo
{
    CString csColumnHeader;
    int nWidth;
};

struct SColoredRow
{
    int mnRow;
    COLORREF mcolTextColor;
    COLORREF mcolBkColor;
    SColoredRow()
    {
        mnRow = -1;
        mcolTextColor = COLOR_BLACK;
        mcolBkColor = COLOR_WHITE;
    }
};

#endif