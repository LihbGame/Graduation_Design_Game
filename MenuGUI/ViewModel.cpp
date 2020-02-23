
#include <NsCore/ReflectionImplement.h>
#include <NsCore/ReflectionImplementEnum.h>
#include <NsCore/Log.h>
#include <NsGui/UIElement.h>
#include <windows.h>
#include <stdlib.h>
#include "app.h"
#include "ViewModel.h"
#include <thread>
using namespace Noesis;
using namespace NoesisApp;




extern bool menu;
extern AppLauncher launcher;
extern HWND  hWnd;
extern HWND hWnd32;
////////////////////////////////////////////////////////////////////////////////////////////////////
ViewModel::ViewModel()
{
    _start.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnStart));
    _startCasual.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnStartCasual));
    _startNormal.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnStartNormal));
    _startExpert.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnStartExpert));
    _settings.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnSettings));
    _exit.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnExit));
    _back.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnBack));
    _fadeInCompleted.SetExecuteFunc(MakeDelegate(this, &ViewModel::OnFadeInCompleted));

    _state = State::Main;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetStart() const
{
    return &_start;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetStartCasual() const
{
    return &_startCasual;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetStartNormal() const
{
    return &_startNormal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetStartExpert() const
{
    return &_startExpert;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetSettings() const
{
    return &_settings;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetExit() const
{
    return &_exit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetBack() const
{
    return &_back;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DelegateCommand* ViewModel::GetFadeInCompleted() const
{
    return &_fadeInCompleted;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ViewModel::GetPlatform() const
{
#if defined(NS_PLATFORM_XBOX_ONE) || defined(NS_PLATFORM_NX)
    return "XBOX";
#elif defined(NS_PLATFORM_PS4)
    return "PS4";
#else
    return "PC";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnStart(BaseComponent*)
{
    SetState(State::Start);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnStartCasual(BaseComponent*)
{
    NS_LOG_INFO("Start Casual");
    
    ShowWindow(hWnd, SW_SHOW);
    ShowWindow(hWnd32, SW_HIDE);
    menu = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnStartNormal(BaseComponent*)
{
    NS_LOG_INFO("Start Normal");
   
    ShowWindow(hWnd, SW_SHOW);
    ShowWindow(hWnd32, SW_HIDE);

    menu = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnStartExpert(BaseComponent*)
{
    NS_LOG_INFO("Start Expert");
    ShowWindow(hWnd, SW_SHOW);
    ShowWindow(hWnd32, SW_HIDE);
    menu = false;

   /* if (menu)
    {
        std::this_thread::yield();
    }*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnSettings(BaseComponent*)
{
    SetState(State::Settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnExit(BaseComponent*)
{
    NS_LOG_INFO("Exiting game"); 
    NoesisApp::Application::Current()->Shutdown();
    ::_exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnBack(BaseComponent*)
{
    switch (_state)
    {
        case State::Main:
        {
            OnExit(nullptr);
            break;
        }
        case State::Start:
        case State::Settings:
        {
            SetState(State::Main);
            break;
        }
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::OnFadeInCompleted(BaseComponent* param)
{
    UIElement* target = (UIElement*)param;
    target->Focus();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
State ViewModel::GetState() const
{
    return _state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ViewModel::SetState(State value)
{
    if (_state != value)
    {
        _state = value;
        OnPropertyChanged("State");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(ViewModel)
{
    NsProp("Start", &ViewModel::GetStart);
    NsProp("StartCasual", &ViewModel::GetStartCasual);
    NsProp("StartNormal", &ViewModel::GetStartNormal);
    NsProp("StartExpert", &ViewModel::GetStartExpert);
    NsProp("Settings", &ViewModel::GetSettings);
    NsProp("Exit", &ViewModel::GetExit);
    NsProp("Back", &ViewModel::GetBack);
    NsProp("FadeInCompleted", &ViewModel::GetFadeInCompleted);
    NsProp("State", &ViewModel::GetState, &ViewModel::SetState);
    NsProp("Platform", &ViewModel::GetPlatform);
}

NS_IMPLEMENT_REFLECTION_ENUM(State)
{
    NsMeta<TypeId>("Menu3D.State");

    NsVal("Main", State::Main);
    NsVal("Start", State::Start);
    NsVal("Settings", State::Settings);
}
