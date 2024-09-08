#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "PhysicsObject.h"
#include "../CSC8503CoreClasses/StateMachine.h"
#include "../CSC8503CoreClasses/State.h"
#include "../CSC8503CoreClasses/StateTransition.h"
#include <functional>
#include "../NCLCoreClasses/Vector4.h"

using std::vector;



namespace NCL::CSC8503 {
	
	class NavigationGrid;
	class GameWorld;
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject	{
	public:
		typedef std::function<void(GameObject*)> collisionFuncType;
		enum ObjectType {
			Wall,
			DefaultType,
			PickupJump,
			PickupInvisibility,
			PickupSpeed,
			PickupKey,
			PickupGoal,
			PlayerUser,
			PlayerOther,
			Door,
			Enemy,
			Floor,
			Level2Bot,
			Floor2,
			StartingLocation,
			Barrier,
			TrueDoor,
			Ball,
			ATKBall,
			Rhino,
			Goal,
			LongCube
		};
		std::chrono::steady_clock::time_point hitTime;
		Vector4 selfColor;
		void setColor(Vector4 color)
		{
			selfColor = color;
		}
		bool ifBeHit = false;
		void beHit();
		bool getIfBeHit()
		{
			return ifBeHit;
		}
		bool setIfBeHit(bool b) {
		ifBeHit = b;
		}
		void afterHit();
		GameObject(std::string name = "");
		~GameObject();
		ObjectType objectType = DefaultType;
		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void OnCollisionBegin(GameObject* otherObject) {
			//std::cout << "OnCollisionBegin event occured!\n";
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			//std::cout << "OnCollisionEnd event occured!\n";
		}

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

		void SetType(ObjectType t) {
			objectType = t;
		}

		ObjectType GetType() {
			return objectType;
		}

		// deleting objects mid collision causes issues with the physics system, so lets just throw it under the map,
		// far away enough that it's someone else's problem, and give it infinite mass to stop it from going anywhere
		void SendToShadowRealm() { 
			transform.SetPosition(Vector3(0, -10000, 0)); 
			GetPhysicsObject()->SetInverseMass(0.0f);
		}

		virtual void UpdateObject() {};


		void SetActive(bool a) {
			isActive = a;
		}
		
		bool isPushable = false;
	protected:
		Transform			transform;

		CollisionVolume*	boundingVolume;
		PhysicsObject*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;

		bool		isAnimated;
		bool		isActive;
		int			worldID;
		std::string	name;



		Vector3 broadphaseAABB;
	};
}

