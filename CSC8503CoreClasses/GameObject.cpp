#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include "../CSC8503CoreClasses/NavigationPath.h"
#include "../CSC8503CoreClasses/NavigationGrid.h"

#include "../NCLCoreClasses/Window.h"
#include"../CSC8503CoreClasses/GameWorld.h"

using namespace NCL::CSC8503;

void NCL::CSC8503::GameObject::beHit()
{
	this->selfColor = this->GetRenderObject()->GetColour();
	
	ifBeHit = true;
	hitTime = std::chrono::steady_clock::now();
	this->GetRenderObject()->SetColour(Vector4(1,0,0,1));//set it to red
}

void NCL::CSC8503::GameObject::afterHit()
{
	Vector4 colour = this->selfColor;
//	this->GetRenderObject()->SetColour(colour);
	ifBeHit = false;
	
	this->GetRenderObject()->SetColour(Vector4(1,1,1,1));

	
}

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}
