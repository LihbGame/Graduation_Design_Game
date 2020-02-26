

#ifndef __MENU3D_STARTMENU_H__
#define __MENU3D_STARTMENU_H__


#include <NsCore/Noesis.h>
#include <NsGui/UserControl.h>


namespace Menu3D
{

////////////////////////////////////////////////////////////////////////////////////////////////////
class StartMenu final: public Noesis::UserControl
{
public:
    StartMenu();

private:
    void InitializeComponent();

    NS_DECLARE_REFLECTION(StartMenu, UserControl)
};

}


#endif
