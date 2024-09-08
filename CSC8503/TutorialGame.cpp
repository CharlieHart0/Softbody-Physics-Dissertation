#include "DebugOptions.h"
#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "TriggerBox.h"
#include"RendererBase.h"
#include"GameUI.h"
#include "Checkpoint.h"
#include "Softbody.h"
#include "SpringConstraint.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "../CSC8503CoreClasses/NavigationGrid.h"

#include <stdint.h>
#include <random>

#include "../stb-master/stb_image.h"
#include "MapImporter.h"

//_CRT_SECURE_NO_WARNINGS

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#define SOFTBODY_TEST_SPAWN_POWER 7

using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

TutorialGame::TutorialGame() {
	win = new bool;
	*win = false;
	timer = new double;
	CoolDownTimer = new double;
	*timer = 240;
	*CoolDownTimer = 6.0;
	flag = new bool;
	*flag = false;
	ViewingDistance = new float;
	world = new GameWorld();
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	InitialiseAssets();	
}

bool GameState = true;
void TutorialGame::InitUI() {

	menuStateMachine = new StateMachine();

	mainmenuState = new State([&](float dt)->void {
		freeCam = true;
		
		world->GetMainCamera()->SetPosition(Vector3(-71, 104, 90));
		world->GetMainCamera()->SetPitch(-10);//-45
		world->GetMainCamera()->SetYaw(180);//270
		});

	gamePlayingState = new State([&](float dt)->void {
		freeCam = false;
		});

	gamePauseState = new State([&](float dt)->void {
		freeCam = true;
		});

	gameOverState = new State([&](float dt)->void {
		freeCam = false;
		});

	menuStateMachine->AddState(mainmenuState);
	menuStateMachine->AddState(gamePlayingState);
	menuStateMachine->AddState(gamePauseState);
	menuStateMachine->AddState(gameOverState);


	stMMGame = new StateTransition(mainmenuState, gamePlayingState, [&](void)->bool {

		return Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) || true;
		});

	stGameGameover = new StateTransition(gamePlayingState, gameOverState, [&](void)->bool {
		return PlayerMe->GetHasWonGame() || *timer < 0.0f;
		});

	stGameGamePause = new StateTransition(gamePlayingState, gamePauseState, [&](void)->bool {
		return Window::GetKeyboard()->KeyPressed(KeyboardKeys::P);
		});

	stGamePauseGame = new StateTransition(gamePauseState, gamePlayingState, [&](void)->bool {
		return Window::GetKeyboard()->KeyPressed(KeyboardKeys::P);
		});

	menuStateMachine->AddTransition(stGameGameover);
	menuStateMachine->AddTransition(stMMGame);
	menuStateMachine->AddTransition(stGameGamePause);
	menuStateMachine->AddTransition(stGamePauseGame);
}

void NCL::CSC8503::TutorialGame::initialise(Window*window){
	GameWindow = window;
}
/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	srand((unsigned int)time(NULL));

	InitUI();

	useGravity = true;
	physics->UseGravity(useGravity);
	cylinderMesh = renderer->LoadMesh("cylinder.msh");
	cubeMesh = renderer->LoadMesh("cube.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");
	charMesh = renderer->LoadMesh("cube.msh");
	
	stateMesh = renderer->LoadMesh("Keeper.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	
	doorTex = renderer->LoadTexture("door.png");
	basicTex = renderer->LoadTexture("checkerboard.png");
	
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");
	animationShader = renderer->LoadShader("skinning.vert", "scene.frag");

	InitCamera();
	InitWorld();

	AddPlayerToWorld(startPoint);
	
}


void NCL::CSC8503::TutorialGame::WinGame()
{	
	setWin(true);
	// write your code here for when the game has been won
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete cylinderMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete stateMesh;
	delete basicTex;
	delete basicShader;
	delete flag;
	delete physics;
	delete renderer;
	delete world;

	delete menuStateMachine;


}

int atktime = 0;
void TutorialGame::UpdateGame(float dt) {

	//Debug::DrawLine(Vector3(0,0,0),Vector3(1,0,0), Vector4(1,0,0,1));
	//Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector4(0, 1, 0, 1));
	//Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector4(0, 0, 1, 1));

	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L)) {

		world->GetMainCamera()->SetPitch(0);
	}

	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!
		
		
			world->GetMainCamera()->SetPosition(camPos);
		
			
			world->GetMainCamera()->SetPitch(angles.x);
			world->GetMainCamera()->SetYaw(angles.y);
	}

	UpdateKeys();

	/*if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}
	*/

	menuStateMachine->Update(dt);
	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	//Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));


	//SelectObject();
	//MoveSelectedObject();

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::C)) {
		freeCam = !freeCam;
	}

	if (!freeCam) {
		//spin character for testing camera
		//PlayerMe->GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), 30.0f * (float)(Window::GetTimer()->GetTotalTimeSeconds())   ));
		Camera* cam = world->GetMainCamera();
		Vector3 camOffset = Vector3(0, 0, 0);
		Quaternion playerRot = PlayerMe->GetTransform().GetOrientation();

		if (getFlag() == 0) {
			freeCam = false;
			cam->SetPosition(Vector3(-71, 104, 90));
			cam->SetPitch(-10);//-45
			cam->SetYaw(180);//270
			
		}
		if (getFlag() == 1) {
			freeCam = true;

			float pitchRadians = cam->GetPitch() * (atan(1) * 4.0f / 180.0f);
			float yawRadians = cam->GetYaw() * (atan(1) * 4.0f / 180.0f);

			camOffset.x = sin(yawRadians) * cos(pitchRadians) * 8.0f;
			camOffset.y = std::clamp(sin(pitchRadians) * -8.0f, -6.0f, 6.0f);
			camOffset.z = cos(yawRadians) * cos(pitchRadians) * 8.0f;

			if (camOffset.y >= 6.0f || camOffset.y <= -6.0f) {
				camOffset.x = sin(yawRadians) * sqrtf(28);
				camOffset.z = cos(yawRadians) * sqrtf(28);
				cam->SetPitch((camOffset.y > 0.0f) ? -49.06f : 49.06f);
			}

			Vector3 playerPos = PlayerMe->GetTransform().GetPosition();
			Vector3 camNewPos = playerPos + (camOffset * getViewingDistance());
			/*Vector3 playerToCamDir = camNewPos - playerPos;
			Vector3 rayPos = playerPos + (playerToCamDir.Normalised() * 2.0f);

			Ray playerToCamRay = Ray(rayPos, playerToCamDir);
			RayCollision objsBetweenPlayerAndCamera;
			
			world->Raycast(playerToCamRay, objsBetweenPlayerAndCamera, true);
			if (objsBetweenPlayerAndCamera.rayDistance < 6.0f) {
				camNewPos = playerPos + (playerToCamDir.Normalised() * objsBetweenPlayerAndCamera.rayDistance);
			}*/

			
				cam->SetPosition(camNewPos);
			
			
		}


		// move camera closer to player if something is in the way
		Vector3 playerToCam = (world->GetMainCamera()->GetPosition() - PlayerMe->GetTransform().GetPosition());
		Ray ray = Ray(PlayerMe->GetTransform().GetPosition() + playerToCam.Normalised().Scaled(0.8f), playerToCam.Normalised());
		RayCollision cameraCollision;
		if (world->Raycast(ray, cameraCollision, true)) {
			if ((PlayerMe->GetTransform().GetPosition() - cameraCollision.collidedAt).LengthSquared() <
				(playerToCam).LengthSquared()) {

				world->GetMainCamera()->SetPosition(Vector3::Lerp(PlayerMe->GetTransform().GetPosition(), cameraCollision.collidedAt, 0.8f));
			}
		}

		PlayerMe->moveSelfPlayer(dt, cam->GetYaw());

	}
	else {

	}

	setTimer(*timer);
	if (*timer < 0.0) {
		//TODO end state
	}


	world->UpdateWorld(dt);
	gameUI->UpdateGameUI(dt);
	
	renderer->Update(dt);

	physicsTimeMsec = Window::GetTimer()->GetTotalTimeMSec();
	physics->Update(dt);
	physicsTimeMsec = Window::GetTimer()->GetTotalTimeMSec() - physicsTimeMsec;
	
	float prevRenderTimeMsec = Window::GetTimer()->GetTotalTimeMSec();
	renderer->Render();
	renderTimeMsec = Window::GetTimer()->GetTotalTimeMSec() - prevRenderTimeMsec;

	DoProfiling(dt);

	Debug::UpdateRenderables(dt);
}



void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}
void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	InitDefaultFloor();


	SpawnTestSoftBodies(Vector3(0, -11.5, 170), SOFTBODY_TEST_SPAWN_POWER);

	/*GameObject* sphere1 = AddSphereToWorld(Vector3(0, -13, 170), 0.5f, 10.0f);
	world->AddGameObject(sphere1);
	sphere1->GetPhysicsObject()->SetConstraints(ConstrainAll);

	GameObject* sphere2 = AddSphereToWorld(Vector3(-2, -13, 170), 0.5f, 10.0f);
	world->AddGameObject(sphere2);

	world->AddConstraint(new SpringConstraint(Compression, sphere1, sphere2,2, 0.5f, 0.05f));
	world->AddConstraint(new SpringConstraint(Extension, sphere1, sphere2, 2, 0.5f, 0.05f));*/

}

void NCL::CSC8503::TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5;
	int numLinks = 10;
	float maxDistance = 30;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(50, 500, 50);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++) {

		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;

	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void NCL::CSC8503::TutorialGame::DoProfiling(float t_dt)
{
	t_dt *= 1000; // convert from second to msec
	profilingAlwaysTimer += t_dt;

	if (profilingStart != 0)
	{
		//within profile time
		if (profilingAlwaysTimer > profilingStart && 
			profilingStart + PROFILING_TEST_DURATION_MS > profilingAlwaysTimer &&
			profilingAlwaysTimer - profilingLastTime > PROFILING_TEST_INTERVAL_MS)
		{
			//do a profile
			ProfilingPass(profilingLastTime, profilingAlwaysTimer);
		}
		else if (profilingAlwaysTimer > profilingStart && profilingStart + PROFILING_TEST_DURATION_MS < profilingAlwaysTimer)
		{
			//end profiling

			unsigned int totalcubes = std::pow(2, SOFTBODY_TEST_SPAWN_POWER);

			std::ofstream outfile("pow "+ std::to_string(SOFTBODY_TEST_SPAWN_POWER)+" render " + std::to_string(SOFTBODY_RENDER_DYNAMICALLY)+ ".txt");

			outfile << "X " << SOFTBODY_TEST_SPAWN_POWER << std::endl;
			outfile << "2^X - number of soft bodies spawned " << totalcubes << std::endl;
			outfile << "Soft Bodies rendered? " << SOFTBODY_RENDER_DYNAMICALLY << std::endl;
			outfile << "Render Time Avg " << profRenderTimesAvg << "ms" << std::endl;
			outfile << "Physics Time Avg " << profPhysTimesAvg << "ms" << std::endl;
			outfile << "GPU memory usage Avg (MB)" << (profGPUMEMAvg /1000) << "MB" << std::endl;

			outfile.close();

			std::cout << "Profiling complete!" << std::endl;

			profilingStart = 0;
			profilingLastTime = 0;

		}
	}
	else
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::COMMA))
		{
			// begin profiling and do an profile
			std::cout << "Profiling started!" << std::endl;
			profilingStart = profilingAlwaysTimer;
			ProfilingPass(profilingLastTime, profilingAlwaysTimer);
		}
	}

	
}

void NCL::CSC8503::TutorialGame::ProfilingPass(float& t_lastProfile, float t_currTime)
{
	
	if (profRenderTimesCount == 0)
	{
		profRenderTimesAvg = renderTimeMsec;
		profRenderTimesCount = 1;

		profPhysTimesAvg = physicsTimeMsec;
		profPhysTimesCount = 1;

		GLint total_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
			&total_mem_kb);

		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
			&cur_avail_mem_kb);

		profGPUMEMAvg = (int)(total_mem_kb - cur_avail_mem_kb);
		profGPUMEMCount = 1;
	}
	
	addToRunningTotal(profPhysTimesAvg, profPhysTimesCount, physicsTimeMsec);
	addToRunningTotal(profRenderTimesAvg,profRenderTimesCount,renderTimeMsec);


	t_lastProfile = t_currTime;
}

void NCL::CSC8503::TutorialGame::SpawnTestSoftBodies(Vector3 t_pos, float t_power)
{

	float spacing = 2;
	unsigned int totalSoftBodies = std::pow(2, t_power);
	unsigned int width = (unsigned int) std::ceil(std::sqrt(totalSoftBodies));

	for (unsigned int i = 0; i < totalSoftBodies; i++)
	{
		Vector3 pos = t_pos;
		pos.x += (i % width) * spacing;
		pos.z += std::floor(i / width) * spacing;

		GameObject* softBody = new Softbody(charMesh, pos, *world, 1.0f, basicTex, basicShader);
		world->AddGameObject(softBody);
	}

	
}

void NCL::CSC8503::TutorialGame::DrawDebugAxis()
{
	// Doesn't really matter what positive value this distance is set to, the below line will make it the same size after perspective divide
	float distanceFromCamera = 5;
	float sizeOfAxis = distanceFromCamera /5;

	Vector3 camPos = world->GetMainCamera()->GetPosition();
	Quaternion q = Quaternion::EulerAnglesToQuaternion(world->GetMainCamera()->GetPitch(), world->GetMainCamera()->GetYaw(), 0);
	Vector3 offsetPos = camPos + Vector3(0,0,distanceFromCamera).rotatedByQuaternion(q).Scaled(-1);


	Debug::DrawLine(offsetPos, offsetPos + Vector3(0, 0, sizeOfAxis), Vector4(0, 0, 1, 1));
	Debug::DrawLine(offsetPos, offsetPos + Vector3(0, sizeOfAxis, 0), Vector4(0, 1, 0, 1));
	Debug::DrawLine(offsetPos, offsetPos + Vector3(sizeOfAxis, 0, 0), Vector4(1, 0, 0, 1));
}





#pragma region Add objects

/*

A single function to add a large immoveable cube to the bottom of our world

*/



GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(500, 2, 500);
	//OBBVolume* volume = new OBBVolume(floorSize);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));//floorMesh wallTex basicTex
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->SetType(GameObject::ObjectType::Floor);

	floor->GetPhysicsObject()->SetConstraints(ConstrainAll);
	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, doorTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}


PlayerCharacter* TutorialGame::AddPlayerToWorld(const Vector3& position) {

	auto  add_obb  = [&] (const Vector3& position, const Quaternion & rotation, const Vector3& dimensions, float inverseMass ) 
	{
		GameObject* cube = new GameObject();

		//AABBVolume* volume = new AABBVolume(dimensions);
		OBBVolume* volume = new OBBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform()
			.SetPosition(position)
			.SetScale(dimensions * 2)
			.SetOrientation(rotation);

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(cube);

		return cube;
	};

	auto  add_sphere  = [&] (const Vector3& position , const float radius, float inverseMass ) 
	{
		GameObject* cube = new GameObject();

		SphereVolume* volume = new SphereVolume(radius);
		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform()
			.SetPosition(position)
			.SetScale( {radius  , radius ,radius } );

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), sphereMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitSphereInertia();
		
		world->AddGameObject(cube);

		return cube;
	};

	float obb_radius = 5;
	int obb_count = 2;
	int obb_stacks = 2;
	float obb_deg = 360.f / obb_count;


	/*for ( int i=0; i<obb_count ; ++i ) {
		for ( int j=0; j<obb_stacks; ++j) {
			add_obb( 
			Vector3(0, 5.f + j * 2.f ,0) +	position + Quaternion::AxisAngleToQuaterion( {0,1,0 }, i * obb_deg ) * Vector3(obb_radius,0,0) 
				, Quaternion(), Vector3(1.f, 1.f, 1.f), 1.f );
			
			add_sphere( Vector3(0, 6.f + j * 2.f ,0) +	position + Quaternion::AxisAngleToQuaterion( {0,1,0 }, 180 + i * obb_deg ) * Vector3(obb_radius,0,0)
				 , 1.0f, 1.f );
		}

	}*/

	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	PlayerCharacter* character = new PlayerCharacter();
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize * 1.2f, meshSize))
		.SetPosition(position);

	if (PlayerMe == nullptr) {
		PlayerMe = character;
		character->SetType(GameObject::ObjectType::PlayerUser);
	}
	else {
		character->SetType(GameObject::ObjectType::PlayerOther);
	}


	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, animationShader));
	//character->GetRenderObject()->setOffsetPos(Vector3(0, -1.0f, 0));
	
	character->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	world->AddGameObject(character);


	players.push_back(character);



	return character;
}

GameObject* NCL::CSC8503::TutorialGame::AddTriggerBox(const Vector3& position, const Vector3& dim)
{
	GameObject* cube = new TriggerBox();

	AABBVolume* volume = new AABBVolume(dim);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dim * 2);

	if (DEBUG_SHOW_TRIGGER_BOXES) {
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->GetRenderObject()->colour = Vector4(1, 0, 1, 1);
	}
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetConstraints(ConstrainAll);
	cube->GetPhysicsObject()->SetIsTrigger(true);

	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::AddCheckpoint(const Vector3& position, const Vector3& dim)
{
	GameObject* cube = new Checkpoint();

	AABBVolume* volume = new AABBVolume(dim/2);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dim);

	if (DEBUG_SHOW_TRIGGER_BOXES) {
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->GetRenderObject()->colour = Vector4(1, 0, 0, 1);
	}
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetConstraints(ConstrainAll);
	cube->GetPhysicsObject()->SetIsTrigger(true);

	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::AddRespawnTrigger(const Vector3& position, const Vector3& dim)
{
	GameObject* cube = new RespawnTrigger();

	AABBVolume* volume = new AABBVolume(dim / 2);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dim);

	if (DEBUG_SHOW_TRIGGER_BOXES) {
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->GetRenderObject()->colour = Vector4(1, 1, 0, 1);
	}
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetConstraints(ConstrainAll);
	cube->GetPhysicsObject()->SetIsTrigger(true);

	world->AddGameObject(cube);

	return cube;
}


#pragma endregion



void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20, 150));
}


/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void NCL::CSC8503::TutorialGame::setRendererUI(GameUI* guiptr)
{
	renderer->ui = guiptr;
	gameUI = guiptr;
}


