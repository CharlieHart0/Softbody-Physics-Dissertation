#pragma once
#include "../NCLCoreClasses/Matrix3.h"
using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

		enum PhysicsObjectConstraints {
			ConstrainNone = 0x00,
			ConstrainPositionX = 0x02,
			ConstrainPositionY = 0x04,
			ConstrainPositionZ = 0x08,
			ConstrainPositionXYZ = 0x0E,
			ConstrainRotationX = 0x10,
			ConstrainRotationY = 0x20,
			ConstrainRotationZ = 0x40,
			ConstrainRotation =	0x70,
			ConstrainAll = 0x7E
		};

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void SetConstraints(PhysicsObjectConstraints value);
			void SetConstraint(PhysicsObjectConstraints bit, bool value);

			PhysicsObjectConstraints GetConstraints();
			bool GetConstraint(PhysicsObjectConstraints bit);


			void InitCubeInertia();
			void InitSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			void ConstrainVelocities();

			void SetIsTrigger(bool value);
			bool GetIsTrigger();

		protected:
			const CollisionVolume* volume;
			Transform*		transform;
		
			bool isTrigger = false;

			float inverseMass;
			float elasticity;
			float friction;

			PhysicsObjectConstraints constraints = ConstrainNone;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

