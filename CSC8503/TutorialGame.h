#pragma once
#include "GameTechRenderer.h"
#include "MeshMaterial.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "StateGameObject.h"
#include "PlayerCharacter.h"
#include "Win32Window.h"
#include "Window.h"


#define PROFILING_TEST_DURATION_MS 10000
#define PROFILING_TEST_INTERVAL_MS 100
#define PROFILING_PERFORM_TESTS true

namespace NCL {
	namespace CSC8503 {
		class NavigationGrid;
		class PlayerDetectorBox;
		class GameUI;
		class TutorialGame {
		public:

			TutorialGame();
			~TutorialGame(); 
			
			virtual void UpdateGame(float dt);
			void setWin(bool winGame) {
				*win = winGame;
			}
			bool getWin() {
				return *win;
			}
			void setFlag(bool uiFlag) {
				*flag = uiFlag;
			}
			bool getFlag() {
				return *flag;
			}
			void setTimer(double GetTimer) {
				*timer = GetTimer;
			}
			double getTimer() {
				return *timer;
			}
			void setCoolDownTimer(double GetCoolDownTimer) {
				*CoolDownTimer = GetCoolDownTimer;
			}
			double getCoolDownTimer() {
				return *CoolDownTimer;
			}
			void setViewingDistance(float ViewingDistancePosition) {
				*ViewingDistance = ViewingDistancePosition;
			}
			float getViewingDistance() {
				return *ViewingDistance;
			}
			float* ViewingDistance;
			bool* win;
			double* CoolDownTimer;
			double* timer;
			bool* flag;
			int TrueCount = 0;
			int FalseCount = 0;
			void initialise(Window* window);
			Window* GameWindow;
			void setRendererUI(GameUI* guiptr);
			Vector3 startPoint = Vector3(0, -17, 155);
			Vector3 spawnPoint1 = Vector3(0, -17, 360);
			Vector3 spawnPoint2 = Vector3(0, -17, 530);
			Vector3 spawnPoint3 = Vector3(0, -17, 690);

			MeshGeometry* cylinderMesh = nullptr;
			MeshGeometry* capsuleMesh = nullptr;
			MeshGeometry* cubeMesh = nullptr;
			MeshGeometry* sphereMesh = nullptr;

		
			
			TextureBase* basicTex = nullptr;
			ShaderBase* basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry* charMesh = nullptr;
			MeshGeometry* enemyMesh = nullptr;
			MeshGeometry* bonusMesh = nullptr;
			MeshGeometry* stateMesh = nullptr;


			GameObject* AddTriggerBox(const Vector3& position, const Vector3& dim);
			GameObject* AddCheckpoint(const Vector3& position, const Vector3& dim);
			GameObject* AddRespawnTrigger(const Vector3& position, const Vector3& dim);
			
			void DrawDebugAxis();
			float physicsTimeMsec = 0;
			float renderTimeMsec = 0;
		protected:
			void InitUI();
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			
			void BridgeConstraintTest();

			void DoProfiling(float t_dt);
			void ProfilingPass(float& t_lastProfile, float t_currTime);
			float profilingStart = 0;
			float profilingLastTime = 0;
			double profilingAlwaysTimer = 0;

			void SpawnTestSoftBodies(Vector3 t_pos, float t_power);

			void addToRunningTotal(float& t_prevAvg, unsigned int& t_prevCount, float t_newVal)
			{
				t_prevAvg = 
					(t_prevCount +
						(t_newVal / t_prevAvg)
					) /
					(
						(t_prevCount + 1) /
						t_prevAvg
					);

				t_prevCount++;

			}

			float profRenderTimesAvg = 0;
			unsigned int profRenderTimesCount = 0;

			float profPhysTimesAvg = 0;
			unsigned int profPhysTimesCount = 0;

			float profGPUMEMAvg = 0;
			unsigned int profGPUMEMCount = 0;


			void InitCheckpoints();

			void WinGame();

			PlayerDetectorBox* goalPlayerDetectorBox;

			

			void InitDefaultFloor();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			
			
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			PlayerCharacter* AddPlayerToWorld(const Vector3& position);
			
			std::vector<PlayerCharacter*> players;
			PlayerCharacter* PlayerMe; // the player that our local user is controlling
			bool freeCam = false;
			float cooldown = 0.0f;
			StateMachine* menuStateMachine;
			State* mainmenuState;
			State* gamePlayingState;
			State* gamePauseState;
			State* gameOverState;
			StateTransition* stMMGame;
			StateTransition* stGameGameover;
			StateTransition* stGameGamePause;
			StateTransition* stGamePauseGame;



			

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* enemy = nullptr;

			

			TextureBase* doorTex = nullptr;
			
			ShaderBase* animationShader = nullptr;

			//Coursework Meshes
			MeshMaterial* charMat = nullptr;
			MeshAnimation* charIdle = nullptr;
			MeshAnimation* charWalk = nullptr;
			MeshAnimation* charRun = nullptr;
			
			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
			GameUI* gameUI;
			friend class GameUI;
			GameObject* objClosest = nullptr;

			GameObject* disc1 = nullptr;
			GameObject* disc2 = nullptr;
			GameObject* disc3 = nullptr;
			GameObject* longCube1 = nullptr;
			GameObject* longCube2 = nullptr;
			GameObject* longCube3 = nullptr;
			GameObject* longCube4 = nullptr;
			GameObject* hammer = nullptr;
			GameObject* slide = nullptr;
		};
	}
}

