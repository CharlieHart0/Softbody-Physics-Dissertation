#include "Window.h"
#include "DebugOptions.h"
#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "GameUI.h"
#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>


void GameUI();

//pathfinding
vector<Vector3> testNodes;

void TestPathfinding() {
	NavigationGrid grid("TestGrid2.txt");
	NavigationPath outPath;

	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);

	bool found = grid.FindPath(startPos, endPos, outPath);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); i++) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		//Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}


/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	if (!DEBUG_PRINT_TO_CONSOLE) {
		std::cout.setstate(std::ios_base::failbit);
	}
	Window*w = Window::CreateGameWindow("Soft body physics Demonstration - Charlie Hart", 1280, 720,false);

	if (!w->HasInitialised()) {
		return -1;
	}	

	//ISoundEngine* engine = createIrrKlangDevice();
	//if (!engine) {
	//	std::cout << "Error initializing irrKlang." << std::endl;
	//	return 1;
	//}

	//ISound* music = engine->play2D("../Assets/Sounds/Main.mp3", true);
	AudioManager audioManager;
	audioManager.playBackgroundMusic();

	w->ShowOSPointer(true);
	w->LockMouseToWindow(true);
	// while (!Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		TutorialGame* g = new TutorialGame(); 
		NCL::CSC8503::GameUI* ui = new NCL::CSC8503::GameUI(g, w);
		ui->setSoundEngine(&audioManager);

		g->setRendererUI(ui);
		w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

		
		while (w->UpdateWindow() ) {
			float dt = w->GetTimer()->GetTimeDeltaSeconds();
			if (dt > 0.1f) {
				// std::cout << "Skipping large time delta" << std::endl;
				continue; //must have hit a breakpoint or something to have a 1 second frame time!
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
				w->ShowConsole(true);
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
				w->ShowConsole(false);
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
				w->SetWindowPosition(0, 0);
			}

			//w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

			g->UpdateGame(dt);
			
		// }
	}

	Window::DestroyGameWindow();
}