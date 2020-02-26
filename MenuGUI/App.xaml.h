
#ifndef __MENU3D_APP_H__
#define __MENU3D_APP_H__


#include <NsCore/Noesis.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsApp/Application.h>


namespace Menu3D
{

////////////////////////////////////////////////////////////////////////////////////////////////////
class App final: public NoesisApp::Application
{
    NS_DECLARE_REFLECTION(App, Application)
};

}


#endif
