// Dear ImGui: standalone example application for Win32 + OpenGL 3
// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// This is provided for completeness, however it is strongly recommended you use OpenGL with SDL or GLFW.
#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "AudioManager.h"
#include "TextureLoader.h"
#include "Win32Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Window.h>
#include <GL/GL.h>
#include "GameUI.h"
#include"GameObject.h"
#include "AudioManager.h"
// #include "PlayerCharacter.h"
#include <tchar.h>
#define IMGUI_FONT_QUALITY 4.5f
NCL::Win32Code::MessageFeedback ImguiInput(void* data);

// Data stored per platform window
struct WGL_WindowData { HDC hDC; };

// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;
static int              g_Width;
static int              g_Height;

// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
void ResetDeviceWGL();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
/*
void Initialize(Window* window) {
};*/
// Main code

NCL::CSC8503::GameUI* NCL::CSC8503::GameUI::p_self = nullptr;
/*
void GameUI::Initialize(Window* window){
    gameWindow = window;
}*/

/*
float coolTime;
float getCoolTime(float coolTime) {
    return coolTime;
}

void setCoolTime(float c) {
    coolTime = c;
};*/
NCL::CSC8503::GameUI::GameUI(TutorialGame* g, Window* window)
{
    //VolumeEngine = new AudioManager();
    game = g;
    gameWindow = window;
    //Font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\BAUHS93.ttf", 30);
    // Win32Code::Win32Window* win32 = (Win32Code::Win32Window*)Window::GetWindow();
    Win32Code::Win32Window* win32 = dynamic_cast<Win32Code::Win32Window*>(Window::GetWindow());
    // Window* window = TutorialGame::getGameWindow();
    // Win32Code::Win32Window* win32 = dynamic_cast<Win32Code::Win32Window*>(TutorialGame::getGameWindow())->GetHandle();
    if (!win32) return;

    
    //LONG_PTR Style = GetWindowLongPtr(hwnd, GWL_STYLE);
    //Style = Style & ~WS_CAPTION & ~WS_SYSMENU & ~WS_SIZEBOX;
    //SetWindowLongPtr(hwnd, GWL_STYLE, Style);
    
    // std::cout << "ImguiProcessInput work ready!" << std::endl;
    win32->ImGuiMsgFunc(ImguiInput);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\BRLNSR.TTF", 16.0f * IMGUI_FONT_QUALITY);
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    HWND hwnd = win32->GetHandle();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 330");

    //bool show_demo_window = true;
   // bool show_another_window = false;        
   // if (show_demo_window)ImGui::ShowDemoWindow(&show_demo_window);
    // ImGui_ImplWin32_InitForOpenGL(hwnd);
    // ImGui_ImplOpenGL3_Init("#version 330");
    

    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    // bool show_demo_window = true;
    // bool show_another_window = false;
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    /**/
    /**//*
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        /*
        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        // 渲染
        // glViewport(0, 0, g_Width, g_Height);
        // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // glClear(GL_COLOR_BUFFER_BIT);
       //  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // 刷新
        // Present
        // ::SwapBuffers(g_MainWindow.hDC);

        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplWin32_Shutdown();
        // ImGui::DestroyContext();

       //  CleanupDeviceWGL(hwnd, &g_MainWindow);
        wglDeleteContext(g_hRC);
        //  ::DestroyWindow(hwnd);
        //::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }*/
}

GameUI::~GameUI() {
   //delete VolumeEngine;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
int gameUpdateState = 1;
//bool Mainmenu = true;
bool Pausemenu = false;
bool Playingmenu = false;
bool Optionmenu = false;
bool Win = false;
bool Lose = false;
bool gameStart = false;
float timer = 240.0f;
float volume = 50.0f;
float ViewingDistance = 25.0f;
float MainMenuReturnFlag = false;
float PauseMenuReturnFlag = false;
void GameUI::UpdateGameUI(float dt) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    /*if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        gameStart = true;
    }*/
    
    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::I)&& DrawPhysicsPositionControl==false) {
        DrawPhysicsPositionControl = true;
    }
    else if(Window::GetKeyboard()->KeyDown(KeyboardKeys::O) && DrawPhysicsPositionControl == true){

        DrawPhysicsPositionControl = false;
    }
    if(!Mainmenu&&!Pausemenu){
        if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
            Pausemenu = true;
            Playingmenu = false;
        }
    }
    if (gameUpdateState) {
       
        if (Pausemenu && !Mainmenu) {

            soundEngine->setVolume(UpdateBackgroundVolume);
            game->setFlag(0);
            DrawPauseMenu();
            Window::GetWindow()->ShowOSPointer(true);

        }
        else if (Mainmenu) {
            soundEngine->setVolume(BackgroundVolume);
            DrawMainMenu();
            Window::GetWindow()->ShowOSPointer(true);
        }
        else if (Playingmenu) {
            soundEngine->setVolume(BackgroundVolume);
            game->setViewingDistance(ViewingDistance / 25.0);
            if (DrawPhysicsPositionControl) {
                DrawPhysicsPositionMenu();
            }
            DrawPlayingMenu();
            DrawDashMenu();
            DrawTimeCounter();
            //gameWindow->ShowOSPointer(false);
            game->setFlag(1);
            Window::GetWindow()->ShowOSPointer(false);
        }
        else if (Optionmenu) {
            soundEngine->setVolume(UpdateBackgroundVolume);
            DrawOptionMenu();

        }

        
    }
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //if(Mainmenu){}
    //DrawMainMenu();
    //DrawPauseMenu();
    //DrawPlayingMenu();


}
// Helper functions

void NCL::CSC8503::GameUI::DrawPhysicsPositionMenu() {
    {
        if (Playingmenu) {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            ImGui::SetNextWindowSize(ImVec2(500, 500));
            ImGui::SetNextWindowPos(ImVec2(10, 50));

            ImGui::Begin("debug menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
            ImGui::SetWindowFontScale(1.5f / IMGUI_FONT_QUALITY);
            ImGui::SetCursorPos(ImVec2(15, 130));
            ImGui::Text("Player x:%.2f", game->PlayerMe->GetTransform().GetPosition().x);
            ImGui::SetCursorPos(ImVec2(15, 180));
            ImGui::Text("Player y:%.2f", game->PlayerMe->GetTransform().GetPosition().y);
            ImGui::SetCursorPos(ImVec2(15, 230));
            ImGui::Text("Player z:%.2f", game->PlayerMe->GetTransform().GetPosition().z);

            //std::cout << PlayerMe->GetTransform().GetPosition().x << std::endl;
            //std::cout << PlayerMe->GetTransform().GetPosition().y << std::endl;
            //std::cout << PlayerMe->GetTransform().GetPosition().z << std::endl;
            ImGui::SetCursorPos(ImVec2(15, 280));
            ImGui::Text("Frame Time:%.2f ms", gameWindow->GetTimer()->GetTimeDeltaMSec());
            ImGui::SetCursorPos(ImVec2(15, 330));
            ImGui::Text("Frame Rate:%.2f fps", 1/gameWindow->GetTimer()->GetTimeDeltaSeconds());

            ImGui::SetCursorPos(ImVec2(15, 400));
            ImGui::Text("Render Time:%.2f ms", game->renderTimeMsec);

            ImGui::SetCursorPos(ImVec2(15, 450));
            ImGui::Text("Physics Time:%.2f ms", game->physicsTimeMsec);

            ImGui::End();

            
            game->DrawDebugAxis();
            /*
            if (timer <= 60) {
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Timer: %.2f", timer);

                // ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
                // ImGui::Text("Timer: %.2f", timer);
                // ImGui::PopStyleColor();
            }
            if (timer > 60) {
                ImGui::Text("Timer: %.2f", timer);
            }
            timer -= ImGui::GetIO().DeltaTime;*/
        }
    }
}
void NCL::CSC8503::GameUI::DrawMainMenu() {

    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplWin32_NewFrame();
    //ImGui::NewFrame();
    {

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (Mainmenu) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(gameWindow->GetScreenSize().x, gameWindow->GetScreenSize().y));
            ImGui::Begin("Main Menu", &Mainmenu, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowFontScale(5.0f / IMGUI_FONT_QUALITY);
            
            ImGui::SetCursorPos(ImVec2(380, 70));
            ImGui::Text("Softbody Physics");
            ImGui::SetWindowFontScale(2.0f / IMGUI_FONT_QUALITY);
            ImGui::SetCursorPos(ImVec2(570, 250));
            if (ImGui::Button("Start", ImVec2(200, 100))) {
                soundEngine->playMenuSound();
                Mainmenu = false;
                Playingmenu = true;
            }
            ImGui::SetCursorPos(ImVec2(570, 500));
            if (ImGui::Button("Quit", ImVec2(200, 100))) {
                soundEngine->playMenuSound();
                ExitProcess(0);
            }
            ImGui::SetCursorPos(ImVec2(1170, 0));
            ImGui::SetWindowFontScale(1.0f / IMGUI_FONT_QUALITY);
            if (ImGui::Button("Options", ImVec2(100, 50))) {
                soundEngine->playMenuSound();
                Optionmenu = true; 
                MainMenuReturnFlag = true;
                Mainmenu = false;

            }
            ImGui::End();
        }
    }
    //ImGui::EndFrame();
    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void NCL::CSC8503::GameUI::DrawPauseMenu() {
    {

        if (Pausemenu) {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(1280, 720));
            ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowFontScale(5.0f / IMGUI_FONT_QUALITY);
            ImGui::SetCursorPos(ImVec2(580, 70));
            ImGui::Text("Pause");
            // std::cout << "1" << std::endl;
            ImGui::SetWindowFontScale(2.0f / IMGUI_FONT_QUALITY);
            ImGui::SetCursorPos(ImVec2(570, 200));
            if (ImGui::Button("Resume", ImVec2(200, 70))) {
                soundEngine->playMenuSound();
                Pausemenu = false;
                Playingmenu = true;
            }
            ImGui::SetCursorPos(ImVec2(570, 300));
            if (ImGui::Button("Options", ImVec2(200, 70))) {
                soundEngine->playMenuSound();
                Optionmenu = true;
                PauseMenuReturnFlag = true;
                Pausemenu = false;

            }
            ImGui::SetCursorPos(ImVec2(570, 400));
            if (ImGui::Button("Back to Menu", ImVec2(200, 70))) {
                soundEngine->playMenuSound();
                Mainmenu = true;
                Pausemenu = false;
                Playingmenu = false;
            }
            ImGui::SetCursorPos(ImVec2(1170, 0));
            if (ImGui::Button("Quit", ImVec2(100, 50))) {
                soundEngine->playMenuSound();
                ExitProcess(0);
            }
            ImGui::End();
        }
    }
}
void NCL::CSC8503::GameUI::DrawTimeCounter() {
    return;
}
void NCL::CSC8503::GameUI::DrawOptionMenu() {
    if (Optionmenu) {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1280, 720));
        ImGui::Begin("settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
        ImGui::SetNextItemWidth(300);
        //ImGui::SetNextHeight(300);
        ImGui::SetCursorPos(ImVec2(1170, 0));
        ImGui::SetWindowFontScale(1.0f / IMGUI_FONT_QUALITY);
        if (ImGui::Button("Back", ImVec2(100, 50))) {
            soundEngine->playMenuSound();
            if (PauseMenuReturnFlag) {
                Pausemenu = true;
                MainMenuReturnFlag = false;
            }
            if (MainMenuReturnFlag) {
                Mainmenu = true;
                MainMenuReturnFlag = false;
            }
    }
        ImGui::SetWindowFontScale(2.5f / IMGUI_FONT_QUALITY);
        ImGui::SetCursorPos(ImVec2(20, 100));
        ImGui::SliderFloat("Volume", &volume, 0.5f, 100.0f);
        BackgroundVolume = volume / 100.0f;
        // std::cout << volume << std::endl;
        //VolumeEngine->setVolume(volume);
        //// VolumeEngine->setSoundVolume(volume);
        //std::cout << VolumeEngine->getVolume() << std::endl;
        // VolumnEngine->setSoundVolume(volume);
        ImGui::SetCursorPos(ImVec2(20, 180));
        ImGui::SliderFloat("ViewingDistance", &ViewingDistance, 0.5f, 100.0f);
        // std::cout << ViewingDistance << std::endl;
        ImGui::End();
    }
}

void NCL::CSC8503::GameUI::DrawPlayingMenu() {
    return;
}
bool countStart = true;
// float  cooldownTime = 1.0f;
void NCL::CSC8503::GameUI::DrawDashMenu() {
    return;
}

void NCL::CSC8503::GameUI::DrawWinMenu() {
    return;
}

void NCL::CSC8503::GameUI::DrawLoseMenu() {
    return;
}

bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            g_Width = LOWORD(lParam);
            g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


//reference from https ://github.com/bell-wilding/CSC8508-EngineeringGamingSolutionsTeamProject and Microsoft Learn

NCL::Win32Code::MessageFeedback ImguiInput(void* data) {

    ImGuiIO& io = ImGui::GetIO();
    RAWINPUT* MouseInput = (RAWINPUT*)data;

    if (MouseInput->header.dwType == RIM_TYPEMOUSE)
    {
            if (MouseInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
            {
                
                io.MouseDown[0] = true;
            }
            else if (MouseInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
            {
                io.MouseDown[0] = false;
            }
    }
    return{ false,false };
}
