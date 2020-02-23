

#include "PasswordBox.xaml.h"
#include "OptionSelector.xaml.h"

#include <NsCore/ReflectionImplement.h>
#include <NsGui/IntegrationAPI.h>


using namespace Menu3D;
using namespace Noesis;


////////////////////////////////////////////////////////////////////////////////////////////////////
PasswordBox::PasswordBox()
{
    InitializeComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PasswordBox::InitializeComponent()
{
    GUI::LoadComponent(this, "PasswordBox.xaml");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(Menu3D::PasswordBox)
{
    NsMeta<TypeId>("Menu3D.PasswordBox");
}
