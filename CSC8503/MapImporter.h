#pragma once
#include "../rapidjson/include/rapidjson/document.h"
#include <string>
#include "../NCLCoreClasses/Vector3.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Transform.h"
#include "GameWorld.h"
#include "TutorialGame.h"


using namespace NCL::Maths;
using namespace NCL::CSC8503;



class MapImporter {
public:
	static void loadMapFromJSON(std::string fileName, GameWorld& world,TutorialGame& tutorialGame,Vector3 offset = Vector3(0, 0, 0));
protected:
	static void loadJsonDoc(std::string filename, rapidjson::Document* doc);

	static void loadGameObject(rapidjson::Value& jVal, Vector3 offset, GameWorld& world, TutorialGame& tutorialGame, Transform* parent = nullptr);

	static Vector3 jValtoVec3(rapidjson::Value& jVal);

	static float jValtoFloat(rapidjson::Value& jVal, const char* key);

	static void jValsetTransform(rapidjson::Value& jVal, GameObject* obj, Vector3 offset = Vector3(0,0,0), Transform* parent = nullptr);

	static void jValsetBoundingVolume(rapidjson::Value& jVal, GameObject* obj);

	static void jValsetRenderObject(rapidjson::Value& jVal, GameObject* obj, TutorialGame& tutorialGame);

	static void jValsetPhysicsObject(rapidjson::Value& jVal, GameObject* obj);

	static void setDefaultPhysicsObject(GameObject* obj);

	static GameObject* createRhino(rapidjson::Value& jVal, GameWorld& world, TutorialGame& tutorialGame, Vector3 offset);

	static GameObject* createRespawnTrigger(rapidjson::Value& jVal, GameWorld& world, TutorialGame& tutorialGame);

	static GameObject* createCheckpoint(rapidjson::Value& jVal, GameWorld& world, TutorialGame& tutorialGame);
	
};