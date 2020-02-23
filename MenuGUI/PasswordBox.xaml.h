  
#ifndef __MENU3D_PasswordBox_H__
#define __MENU3D_PasswordBox_H__



#include <NsGui/UserControl.h>
#include <NsCore/Noesis.h>


namespace Menu3D
{

////////////////////////////////////////////////////////////////////////////////////////////////////
class PasswordBox final: public Noesis::UserControl
{
public:
    PasswordBox();

private:
    void InitializeComponent();

   NS_DECLARE_REFLECTION(PasswordBox, UserControl)
};



};








#endif
