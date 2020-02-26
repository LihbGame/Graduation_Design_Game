
#ifndef __MENU3D_MAINWINDOW_H__
#define __MENU3D_MAINWINDOW_H__


#include <NsCore/Noesis.h>
#include <NsApp/Window.h>


namespace Menu3D
{

////////////////////////////////////////////////////////////////////////////////////////////////////
class MainWindow final: public NoesisApp::Window
{
public:
    MainWindow();

private:
    void InitializeComponent();
    void OnLoaded(Noesis::BaseComponent*, const Noesis::RoutedEventArgs&);

private:
    NS_DECLARE_REFLECTION(MainWindow, Window)
};


}


#endif
