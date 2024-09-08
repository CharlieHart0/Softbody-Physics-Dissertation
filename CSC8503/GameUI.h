#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

#include "PhysicsSystem.h"
#include "Win32Window.h"
#include"TutorialGame.h"
#include <Window.h>
#include"AudioManager.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame;
		class GameUI {
		public:
			bool debugPrinter=false;
			float x, y, z;
			// friend void TutorialGame();
			GameUI(TutorialGame* g,Window*window);
			~GameUI();
			void setSoundEngine(AudioManager* engine) { soundEngine = engine; }

			void UpdateGameUI(float dt);
			// void Initialize(Window* window);
			// void GameUI();
			// Window* gameWindow;
			bool DrawPhysicsPositionControl = false;
			void DrawPhysicsPositionMenu();
			void DrawMainMenu();
			void DrawTimeCounter();
			void DrawOptionMenu();
			void DrawVolumnMenu();
			void DrawPointerSpeedMenu();
			void DrawPauseMenu();
			void DrawPlayingMenu();
			void DrawDashMenu();
			void DrawWinMenu();
			float cooltime;
			void DrawLoseMenu();
			HWND hwnd;
			float BackgroundVolume=0.0f;
			float UpdateBackgroundVolume = 0.0f;
			float time = 0.5f;
			float timesinceCooldown = 5.0f;
			bool Mainmenu = true;
		protected:
			
			//ImFont* TitleFont;
			TutorialGame* game;
			Window* gameWindow;
			static GameUI* p_self;
			AudioManager* soundEngine;
		};
	};
}