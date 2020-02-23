

#include "SettingsMenu.xaml.h"
#include "OptionSelector.xaml.h"

#include <NsCore/ReflectionImplement.h>
#include <NsGui/IntegrationAPI.h>


using namespace Menu3D;
using namespace Noesis;


////////////////////////////////////////////////////////////////////////////////////////////////////
SettingsMenu::SettingsMenu()
{
    InitializeComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SettingsMenu::InitializeComponent()
{
    GUI::LoadComponent(this, "SettingsMenu.xaml");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(Menu3D::SettingsMenu)
{
    NsMeta<TypeId>("Menu3D.SettingsMenu");
}
