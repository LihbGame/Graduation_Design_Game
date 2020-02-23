#ifndef __BUTTONS_APP__
#define __BUTTONS_APP__
#include <NsCore/Noesis.h>
#include <NsCore/RegisterComponent.h>
#include <NsApp/EmbeddedXamlProvider.h>
#include <NsApp/EmbeddedTextureProvider.h>
#include <NsApp/EmbeddedFontProvider.h>
#include <NsApp/Application.h>
#include <NsApp/ApplicationLauncher.h>
#include "ViewModel.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "MenuDescription.xaml.h"
#include "MainMenu.xaml.h"
#include "StartMenu.xaml.h"
#include "SettingsMenu.xaml.h"
#include "PasswordBox.xaml.h"
#include "OptionSelector.xaml.h"
#include "MultiplierConverter.h"

#include "BackClouds.png.bin.h"
#include "FrontClouds.png.bin.h"
#include "FrontTrees.png.bin.h"
#include "LakeAndMountains.png.bin.h"
#include "SkyAndSun.jpg.bin.h"
#include "Koala.jpg.bin.h"
#include "weblysleekuisb.ttf.bin.h"
#include "weblysleekuisl.ttf.bin.h"
#include "SoundTrack.mp3.bin.h"
#include "WaterDropBig.mp3.bin.h"
#include "WaterDropSmall.mp3.bin.h"


#include "App.xaml.bin.h"
#include "MenuResources.xaml.bin.h"
#include "MenuDescription.xaml.bin.h"
#include "MainWindow.xaml.bin.h"
#include "MainMenu.xaml.bin.h"
#include "SettingsMenu.xaml.bin.h"
#include "PasswordBox.xaml.bin.h"
#include "StartMenu.xaml.bin.h"
#include "OptionSelector.xaml.bin.h"
#include <NsCore/EnumConverter.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/ReflectionImplementEnum.h>

using namespace Menu3D;
using namespace Noesis;
using namespace NoesisApp;

extern int life;

namespace Menu3D
{
    class Name : public NotifyPropertyChangedBase
    {
    public:
        const char* GetFirst() const
        {
            return _first.c_str();
        }

        void SetFirst(const char* first)
        {
            if (_first != first)
            {
                _first = first;
                OnPropertyChanged("First");
            }
        }

        const char* GetLast() const
        {
            return _last.c_str();
        }

        void SetLast(const char* last)
        {
            if (_last != last)
            {
                _last = "10/100";
                OnPropertyChanged("Last");
            }
        }

    private:
        NsString _first="100/100";
        NsString _last="100/100";

        NS_IMPLEMENT_INLINE_REFLECTION(Name, NotifyPropertyChangedBase)
        {
            NsMeta<TypeId>("Name");
            NsProp("First", &Name::GetFirst, &Name::SetFirst);
            NsProp("Last", &Name::GetLast, &Name::SetLast);
        }
    };



}


class AppLauncher final : public ApplicationLauncher
{
private:
    void RegisterComponents() const override
    {
        NsRegisterComponent<Menu3D::App>();
        NsRegisterComponent<Menu3D::MainWindow>();
        NsRegisterComponent<Menu3D::MenuDescription>();
        NsRegisterComponent<Menu3D::MainMenu>();
        NsRegisterComponent<Menu3D::StartMenu>();
        NsRegisterComponent<Menu3D::SettingsMenu>();
        NsRegisterComponent<Menu3D::OptionSelector>();
        NsRegisterComponent<Menu3D::Name>();
        NsRegisterComponent<EnumConverter<Menu3D::State>>();
        NsRegisterComponent<MultiplierConverter>();
    }

    Ptr<XamlProvider> GetXamlProvider() const override
    {
        EmbeddedXaml xamls[] =
        {
            { "App.xaml", App_xaml, sizeof(App_xaml) },
            { "MenuResources.xaml", MenuResources_xaml, sizeof(MenuResources_xaml) },
            { "MainWindow.xaml", MainWindow_xaml, sizeof(MainWindow_xaml) },
            { "MenuDescription.xaml", MenuDescription_xaml, sizeof(MenuDescription_xaml) },
            { "MainMenu.xaml", MainMenu_xaml, sizeof(MainMenu_xaml) },
            { "StartMenu.xaml", StartMenu_xaml, sizeof(StartMenu_xaml) },
            { "SettingsMenu.xaml", SettingsMenu_xaml, sizeof(SettingsMenu_xaml) },
            { "PasswordBox.xaml", PasswordBox_xaml, sizeof(PasswordBox_xaml) },
            { "OptionSelector.xaml", OptionSelector_xaml, sizeof(OptionSelector_xaml) },
            { "Sounds/SoundTrack.mp3", SoundTrack_mp3, sizeof(SoundTrack_mp3) },
            { "Sounds/WaterDropBig.mp3", WaterDropBig_mp3, sizeof(WaterDropBig_mp3) },
            { "Sounds/WaterDropSmall.mp3", WaterDropSmall_mp3, sizeof(WaterDropSmall_mp3) }
        };

        return *new EmbeddedXamlProvider(xamls, NS_COUNTOF(xamls));
    }

    Ptr<FontProvider> GetFontProvider() const override
    {
        EmbeddedFont fonts[] =
        {
            { "Fonts", weblysleekuisb_ttf, sizeof(weblysleekuisb_ttf) },
            { "Fonts", weblysleekuisl_ttf, sizeof(weblysleekuisl_ttf) }
        };

        return *new EmbeddedFontProvider(fonts, NS_COUNTOF(fonts));
    }

    Ptr<TextureProvider> GetTextureProvider() const override
    {
        EmbeddedTexture textures[] =
        {
            { "Images/BackClouds.png", BackClouds_png, sizeof(BackClouds_png) },
            { "Images/FrontClouds.png", FrontClouds_png, sizeof(FrontClouds_png) },
            { "Images/FrontTrees.png", FrontTrees_png, sizeof(FrontTrees_png) },
            { "Images/LakeAndMountains.png", LakeAndMountains_png, sizeof(LakeAndMountains_png) },
            { "Images/SkyAndSun.jpg", SkyAndSun_jpg, sizeof(SkyAndSun_jpg) },
            { "Koala.jpg", Koala_jpg, sizeof(Koala_jpg) }
        };

        return *new EmbeddedTextureProvider(textures, NS_COUNTOF(textures));
    }
};

#endif