#include <NsCore/ReflectionImplement.h>
#include <NsCore/ReflectionImplementEnum.h>
#include <NsCore/Log.h>
#include <NsGui/UIElement.h>
#include "GameUI_Notify.h"

using namespace Noesis;
using namespace NoesisApp;


const char* PlayerState::GetRed() const
{
    return _RED.c_str();
}

void PlayerState::SetRed(const char* RED)
{
    if (_RED != RED)
    {
        _RED = RED;
        OnPropertyChanged("RED");
    }
}

const char* PlayerState::GetBlue() const
{
    return _BLUE.c_str();
}

void PlayerState::SetBlue(const char* BLUE)
{
    if (_BLUE != BLUE)
    {
        _BLUE = BLUE;
        OnPropertyChanged("BLUE");
    }
}



