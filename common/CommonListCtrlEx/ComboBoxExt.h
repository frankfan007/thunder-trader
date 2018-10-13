/**
 *
 * ComboBoxExt.h - This file contains the definition of the class CComboBoxExt.
                   This class is derived from CComboBox and used for the class
                   CListControlEx.
 *
 * @author :    Shine Kumar
 * @version:    1.0     Date:  2004-10-29
 **/

#ifndef _COMBOBOXEXT_H_
#define _COMBOBOXEXT_H_

class CComboBoxExt : public CComboBox
{
public:
    // Constructor
    CComboBoxExt();
    // Destructor
    ~CComboBoxExt();

protected:
    // Used for setting the text in the combo
    // as the text in the list control when
    //focus is lost
    void OnKillFocusCombo();
    // Used for setting the text in the combo
    // as the text in the list control when
    //selection is changed is lost
    void OnSelChangeCombo();
    DECLARE_MESSAGE_MAP()
};

#endif // ifndef _COMBOBOXEXT_H_
