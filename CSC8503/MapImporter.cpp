#include "MapImporter.h"
#include "TutorialGame.h"
#include "Checkpoint.h"
#include "Assets.h"
#include "AABBVolume.h"
#include "SphereVolume.h"
#include "../CSC8503CoreClasses/RenderObject.h"
#include <algorithm>



using namespace NCL::CSC8503;

void MapImporter::loadMapFromJSON(std::string fileName, GameWorld& world, TutorialGame& tutorialGame, Vector3 offset) {


	rapidjson::Document jsonDoc;

	loadJsonDoc(fileName, &jsonDoc);

	loadGameObject(jsonDoc, offset, world, tutorialGame, nullptr);


}

void MapImporter::loadJsonDoc(std::string filename, rapidjson::Document* doc)
{
	std::ifstream jsonFile(NCL::Assets::MAPSDIR + filename);
	std::string line;
	std::string jsonStr;
	if (jsonFile.fail()) {
		std::cout << NCL::Assets::MAPSDIR + filename;
		throw std::exception("Unable to Open Map File");
	}
	else {
		while (std::getline(jsonFile, line)) {
			jsonStr += (line += '\n');
		}
		jsonFile.close();
	}


	doc->Parse(jsonStr.data());
}

void MapImporter::loadGameObject(rapidjson::Value& jVal, Vector3 offset, GameWorld& world, TutorialGame& tutorialGame, NCL::CSC8503::Transform* parent)
{
	//TODO add support for gameobjects with children?


	std::vector<std::string> tags;

	if (jVal.HasMember("tags")) {
		rapidjson::GenericArray arr = jVal["tags"].GetArray();
		for (rapidjson::Value* i = arr.begin(); i != arr.end(); i++) {
			tags.push_back(i->GetString());
		}
	}
	GameObject* obj = nullptr;

	if (tags.size() != 0) {
		if (std::find(tags.begin(), tags.end(), "respawntrigger") != tags.end()){
			obj = createRespawnTrigger(jVal, world, tutorialGame);
			obj->GetTransform().SetPosition(obj->GetTransform().GetPosition() + offset);
		}
		else if (std::find(tags.begin(), tags.end(), "checkpoint") != tags.end()){
			obj = createCheckpoint(jVal, world, tutorialGame);
			obj->GetTransform().SetPosition(obj->GetTransform().GetPosition() + offset);
		}	
	}
	else {
		obj = new GameObject();

		//TODO this should be set in the level file tbh, or maybe even change how jump detection works
		obj->SetType(GameObject::Floor);
		jValsetTransform(jVal["transform"], obj, offset, parent);

		if (jVal.HasMember("collider")) { jValsetBoundingVolume(jVal["collider"], obj); }
		if (jVal.HasMember("mesh")) { jValsetRenderObject(jVal["mesh"], obj, tutorialGame); }
		if (jVal.HasMember("physicsObject")) { jValsetPhysicsObject(jVal, obj); }
		else { setDefaultPhysicsObject(obj); }
	}


	if (jVal.HasMember("children")) {
		rapidjson::GenericArray<false, rapidjson::Value> childrenArray = jVal["children"].GetArray();

		for (const auto& val : jVal["children"].GetArray()) {
			loadGameObject((rapidjson::Value&)val, offset, world, tutorialGame, &(obj->GetTransform()));
		}
	}
	
	world.AddGameObject(obj);
	
}

Vector3 MapImporter::jValtoVec3(rapidjson::Value& jVal)
{
	Vector3 vec = Vector3(0, 0, 0);

	vec.x = jValtoFloat(jVal, "x");
	vec.y = jValtoFloat(jVal, "y");
	vec.z = jValtoFloat(jVal, "z");

	return vec;
}

float MapImporter::jValtoFloat(rapidjson::Value& jVal, const char* key)
{
	if (jVal.HasMember(key)) {
		return jVal[key].GetFloat();
	}
	else {
		throw std::exception("JSON member requested does not exist");
	}
	
}

void MapImporter::jValsetTransform(rapidjson::Value& jVal, GameObject* obj, Vector3 offset, Transform* parent)
{
	Vector3 parentPosition = Vector3(0, 0, 0);
	Vector3 parentScale = Vector3(1, 1, 1);
	Quaternion parentOrientation = Quaternion();

	if (parent != nullptr) {
		parentPosition = parent->GetPosition();
		parentScale = parent->GetScale();
		parentOrientation = parent->GetOrientation();
	}
	else {
		parentPosition = offset;
	}

	obj->GetTransform().SetPosition(
		jValtoVec3(jVal["w_position"]) + offset
	).SetScale(
		jValtoVec3(jVal["w_scale"])
	).SetOrientation(
		Quaternion::EulerAnglesToQuaternion(jValtoVec3(jVal["l_rotation"]))
	);

}

void MapImporter::jValsetBoundingVolume(rapidjson::Value& jVal, GameObject* obj)
{
	std::string type = jVal["type"].GetString();
	
	//TODO support bounding volumes with offset centre?

	if (type == "box") {
		NCL::AABBVolume* volume = new NCL::AABBVolume(jValtoVec3(jVal["fullDimensions"])/2);
		obj->SetBoundingVolume((NCL::CollisionVolume*)volume);
	}
	else if (type == "sphere") {
		NCL::SphereVolume* volume = new NCL::SphereVolume(jValtoFloat(jVal,"radius"));
		obj->SetBoundingVolume((NCL::CollisionVolume*)volume);
	}
	else {
		throw std::exception("Invalid collider type!");
	}

}

void MapImporter::jValsetRenderObject(rapidjson::Value& jVal, GameObject* obj,TutorialGame& tutorialGame)
{
	std::string type = jVal.GetString();

	//TODO support other meshes such as player, rhino etc.

	if (type == "Cube") {
		obj->SetRenderObject(new RenderObject(&(obj->GetTransform()),tutorialGame.cubeMesh, tutorialGame.basicTex, tutorialGame.basicShader));
	}
	else if (type == "Sphere") {
		obj->SetRenderObject(new RenderObject(&(obj->GetTransform()), tutorialGame.sphereMesh, tutorialGame.basicTex, tutorialGame.basicShader));
	}
	else {
		throw std::exception("Invalid mesh type!");
	}
}

void MapImporter::jValsetPhysicsObject(rapidjson::Value& jVal, GameObject* obj)
{

	obj->SetPhysicsObject(new PhysicsObject(&(obj->GetTransform()), obj->GetBoundingVolume()));

	if (!jVal["physicsObject"].HasMember("inverseMass")) { 
		throw std::exception("PhysicsObject in Map JSON has no inverse mass!"); }

	if (jVal["physicsObject"]["inverseMass"].IsString()) {
		if (jVal["physicsObject"]["inverseMass"].GetString() == "INF") {
			obj->GetPhysicsObject()->SetInverseMass(999999); // if inverse mass is INF, we want the normal mass to be near zero.
		}
	}
	else {
		obj->GetPhysicsObject()->SetInverseMass(jVal["physicsObject"]["inverseMass"].GetFloat());
	}

	if (jVal["physicsObject"].HasMember("constraints")) {
		obj->GetPhysicsObject()->SetConstraints((PhysicsObjectConstraints)jVal["physicsObject"]["constraints"].GetUint());
	}

	if (jVal["collider"]["type"].GetString() == "box") {
		obj->GetPhysicsObject()->InitCubeInertia();
	}
	else if (jVal["collider"]["type"].GetString() == "sphere") {
		obj->GetPhysicsObject()->InitSphereInertia();
	}

}

void MapImporter::setDefaultPhysicsObject(GameObject* obj)
{
	NCL::AABBVolume* volume = new NCL::AABBVolume(Vector3(0,0,0));
	obj->SetBoundingVolume((NCL::CollisionVolume*)volume);

	obj->SetPhysicsObject(new PhysicsObject(&(obj->GetTransform()), obj->GetBoundingVolume()));
	obj->GetPhysicsObject()->SetInverseMass(0);
	obj->GetPhysicsObject()->InitCubeInertia();
	
}


GameObject* MapImporter::createRespawnTrigger(rapidjson::Value& jVal, GameWorld& world, TutorialGame& tutorialGame)
{
	RespawnTrigger* obj = (RespawnTrigger*)tutorialGame.AddRespawnTrigger(jValtoVec3(jVal["transform"]["w_position"]), jValtoVec3(jVal["collider"]["fullDimensions"]));

	return obj;
}

GameObject* MapImporter::createCheckpoint(rapidjson::Value& jVal, GameWorld& world, TutorialGame& tutorialGame)
{
	Checkpoint* obj = (Checkpoint*)tutorialGame.AddCheckpoint(jValtoVec3(jVal["transform"]["w_position"]), jValtoVec3(jVal["collider"]["fullDimensions"]));

	return obj;
}
