#include <NsApp/ProvidersApi.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/FileTextureProvider.h>
#include "GameGUI.h"
#include "app.h"
// All NoesisGUI API is under Noesis namespace
using namespace Noesis;
using namespace NoesisApp;
extern Menu3D::Name name;

//debug
extern std::string str;


//Before being able to render any XAML, Noesis must be initialized by invoking Noesis::GUI::Init()
//and optionally passing an error handler, log handler and memory allocator.
void LogHandler(const char* filename, uint32_t line, uint32_t level, const char* channel,
    const char* message)
{
    if (strcmp(channel, "") == 0)
    {
        // [TRACE] [DEBUG] [INFO] [WARNING] [ERROR]
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        const char* prefix = level < NS_COUNTOF(prefixes) ? prefixes[level] : " ";
        fprintf(stderr, "[NOESIS/%s] %s\n", prefix, message);
    }
}




GameGUI::GameGUI()
{
}

GameGUI::~GameGUI()
{
    // Free GUI resources and shutdown kernel
    //g_XamlView.Reset();
    //Noesis::GUI::Shutdown();
}

void GameGUI::Init(ID3D11DeviceContext* device_context, int g_Width, int g_Height)
{
    SetupKeyMap();

    // Loads XAML and creates a view with it
     xaml = Noesis::GUI::LoadXaml<FrameworkElement>("PasswordBox.xaml");
    g_XamlView = Noesis::GUI::CreateView(xaml.GetPtr());
    g_XamlView->SetSize(g_Width, g_Height);


    // Initializes renderer. This could be done in a render thread
    Ptr<RenderDevice> device = *new D3D11RenderDevice(device_context);
    g_XamlView->GetRenderer()->Init(device);
    FrameworkElement* h = xaml->FindName<FrameworkElement>("blue");
    h->SetWidth(40);
   

}

void GameGUI::PreRender()
{
    // Performs rendering operations. Note that the renderer associated to a view is intended
    // to be used in the render thread. In this simple application it is the main thread
    Noesis::IRenderer* renderer = g_XamlView->GetRenderer();

    // Applies changes to the render tree
    renderer->UpdateRenderTree();

    // Renders offscreen textures. This step must be done before binding the main render target
    if (renderer->NeedsOffscreen())
    {
       renderer->RenderOffscreen();
    }
}

void GameGUI::Render()
{
    
    Noesis::IRenderer* renderer = g_XamlView->GetRenderer();
    // Renders UI to active render target in the active viewport
    renderer->Render();
}

void GameGUI::Update(float dt)
{
    TextBlock* text = xaml->FindName<TextBlock>("NumBule");
    text->SetText(str.c_str());

    g_XamlView->Update(dt);
}



void GameGUI::SetupKeyMap()
{
    memset(g_KeyMap, 0, sizeof(g_KeyMap));

    g_KeyMap[VK_BACK] = Key_Back;
    g_KeyMap[VK_TAB] = Key_Tab;
    g_KeyMap[VK_CLEAR] = Key_Clear;
    g_KeyMap[VK_RETURN] = Key_Return;
    g_KeyMap[VK_PAUSE] = Key_Pause;

    g_KeyMap[VK_SHIFT] = Key_LeftShift;
    g_KeyMap[VK_CONTROL] = Key_LeftCtrl;
    g_KeyMap[VK_MENU] = Key_LeftAlt;

    g_KeyMap[VK_ESCAPE] = Key_Escape;

    g_KeyMap[VK_SPACE] = Key_Space;
    g_KeyMap[VK_PRIOR] = Key_Prior;
    g_KeyMap[VK_NEXT] = Key_Next;
    g_KeyMap[VK_END] = Key_End;
    g_KeyMap[VK_HOME] = Key_Home;
    g_KeyMap[VK_LEFT] = Key_Left;
    g_KeyMap[VK_UP] = Key_Up;
    g_KeyMap[VK_RIGHT] = Key_Right;
    g_KeyMap[VK_DOWN] = Key_Down;
    g_KeyMap[VK_SELECT] = Key_Select;
    g_KeyMap[VK_PRINT] = Key_Print;
    g_KeyMap[VK_EXECUTE] = Key_Execute;
    g_KeyMap[VK_SNAPSHOT] = Key_Snapshot;
    g_KeyMap[VK_INSERT] = Key_Insert;
    g_KeyMap[VK_DELETE] = Key_Delete;
    g_KeyMap[VK_HELP] = Key_Help;

    g_KeyMap['0'] = Key_D0;
    g_KeyMap['1'] = Key_D1;
    g_KeyMap['2'] = Key_D2;
    g_KeyMap['3'] = Key_D3;
    g_KeyMap['4'] = Key_D4;
    g_KeyMap['5'] = Key_D5;
    g_KeyMap['6'] = Key_D6;
    g_KeyMap['7'] = Key_D7;
    g_KeyMap['8'] = Key_D8;
    g_KeyMap['9'] = Key_D9;

    g_KeyMap[VK_NUMPAD0] = Key_NumPad0;
    g_KeyMap[VK_NUMPAD1] = Key_NumPad1;
    g_KeyMap[VK_NUMPAD2] = Key_NumPad2;
    g_KeyMap[VK_NUMPAD3] = Key_NumPad3;
    g_KeyMap[VK_NUMPAD4] = Key_NumPad4;
    g_KeyMap[VK_NUMPAD5] = Key_NumPad5;
    g_KeyMap[VK_NUMPAD6] = Key_NumPad6;
    g_KeyMap[VK_NUMPAD7] = Key_NumPad7;
    g_KeyMap[VK_NUMPAD8] = Key_NumPad8;
    g_KeyMap[VK_NUMPAD9] = Key_NumPad9;

    g_KeyMap[VK_MULTIPLY] = Key_Multiply;
    g_KeyMap[VK_ADD] = Key_Add;
    g_KeyMap[VK_SEPARATOR] = Key_Separator;
    g_KeyMap[VK_SUBTRACT] = Key_Subtract;
    g_KeyMap[VK_DECIMAL] = Key_Decimal;
    g_KeyMap[VK_DIVIDE] = Key_Divide;

    g_KeyMap['A'] = Key_A;
    g_KeyMap['B'] = Key_B;
    g_KeyMap['C'] = Key_C;
    g_KeyMap['D'] = Key_D;
    g_KeyMap['E'] = Key_E;
    g_KeyMap['F'] = Key_F;
    g_KeyMap['G'] = Key_G;
    g_KeyMap['H'] = Key_H;
    g_KeyMap['I'] = Key_I;
    g_KeyMap['J'] = Key_J;
    g_KeyMap['K'] = Key_K;
    g_KeyMap['L'] = Key_L;
    g_KeyMap['M'] = Key_M;
    g_KeyMap['N'] = Key_N;
    g_KeyMap['O'] = Key_O;
    g_KeyMap['P'] = Key_P;
    g_KeyMap['Q'] = Key_Q;
    g_KeyMap['R'] = Key_R;
    g_KeyMap['S'] = Key_S;
    g_KeyMap['T'] = Key_T;
    g_KeyMap['U'] = Key_U;
    g_KeyMap['V'] = Key_V;
    g_KeyMap['W'] = Key_W;
    g_KeyMap['X'] = Key_X;
    g_KeyMap['Y'] = Key_Y;
    g_KeyMap['Z'] = Key_Z;

    g_KeyMap[VK_F1] = Key_F1;
    g_KeyMap[VK_F2] = Key_F2;
    g_KeyMap[VK_F3] = Key_F3;
    g_KeyMap[VK_F4] = Key_F4;
    g_KeyMap[VK_F5] = Key_F5;
    g_KeyMap[VK_F6] = Key_F6;
    g_KeyMap[VK_F7] = Key_F7;
    g_KeyMap[VK_F8] = Key_F8;
    g_KeyMap[VK_F9] = Key_F9;
    g_KeyMap[VK_F10] = Key_F10;
    g_KeyMap[VK_F11] = Key_F11;
    g_KeyMap[VK_F12] = Key_F12;
    g_KeyMap[VK_F13] = Key_F13;
    g_KeyMap[VK_F14] = Key_F14;
    g_KeyMap[VK_F15] = Key_F15;
    g_KeyMap[VK_F16] = Key_F16;
    g_KeyMap[VK_F17] = Key_F17;
    g_KeyMap[VK_F18] = Key_F18;
    g_KeyMap[VK_F19] = Key_F19;
    g_KeyMap[VK_F20] = Key_F20;
    g_KeyMap[VK_F21] = Key_F21;
    g_KeyMap[VK_F22] = Key_F22;
    g_KeyMap[VK_F23] = Key_F23;
    g_KeyMap[VK_F24] = Key_F24;

    g_KeyMap[VK_NUMLOCK] = Key_NumLock;
    g_KeyMap[VK_SCROLL] = Key_Scroll;
}

Noesis::Key* GameGUI::GetKeyMap()
{
	return g_KeyMap;
}

Noesis::Ptr<Noesis::IView> GameGUI::GetXamlView()
{
	return g_XamlView;
}
