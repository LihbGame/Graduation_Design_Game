#pragma once

#include <NsCore/Noesis.h>
#include <NsCore/Ptr.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsCore/ReflectionDeclareEnum.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>


namespace Menu3D
{
    class PlayerState : public NoesisApp::NotifyPropertyChangedBase
    {
    public:
        const char* GetRed() const;

        void SetRed(const char* RED);

        const char* GetBlue() const;

        void SetBlue(const char* BLUE);

    private:
        NsString _RED;
        NsString _BLUE;



        NS_IMPLEMENT_INLINE_REFLECTION(PlayerState, NotifyPropertyChangedBase)
        {
            NsMeta<TypeId>("PlayerState");
            NsProp("RED", &PlayerState::GetRed, &PlayerState::SetRed);
            NsProp("BLUE", &PlayerState::GetBlue, &PlayerState::SetBlue);
        }
    };
}

