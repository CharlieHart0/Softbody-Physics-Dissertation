#include "GameObject.h"
#include "PlayerCharacter.h"
#include "RenderObject.h"
#include "MeshAnimation.h"
#include "../NCLCoreClasses/Window.h"
#include "../CSC8503CoreClasses/GameWorld.h"
#include "GameUI.h"
#include"TutorialGame.h"
#include "AudioManager.h"
using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

AudioManager audioManager;

bool PlayerCharacter::PushForward(GameWorld* world, float pushRange)
{
	//ray detection
	Vector3 rayPos;
	Vector3 rayDir;
	rayDir = this->GetTransform().GetOrientation() * Vector3(0, 0, -1);

	rayPos = this->GetTransform().GetPosition();
	RayCollision closestCollision;
	GameObject* ForwardObject;
	Ray r = Ray(rayPos, rayDir);
	if (world->Raycast(r, closestCollision, true, this)) {
		ForwardObject = (GameObject*)closestCollision.node;

		if (!ForwardObject->isPushable) { return false; }

		//get position caculate the distence between player and object
		//if it is in  player push range then push else just return false
		Vector3 playerPos = this->GetTransform().GetPosition();
		Vector3 forwardObjectPos = closestCollision.collidedAt;
		float distance = distanceBetween(playerPos, forwardObjectPos);//get ditance
		if (distance < pushRange && ForwardObject->objectType != Enemy) //if the target is not enemy and if it is  within the push range, then fush
		{
			//ForwardObject->setColor();
			//ForwardObject->beHit();
			ForwardObject->GetPhysicsObject()->SetLinearVelocity(rayDir * 6 + Vector3(0, 1, 0) * 6);

		}
		else
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}

}
float PlayerCharacter::distanceBetween(Vector3 a, Vector3 b)
{
	return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) + std::pow(b.z - a.z, 2));
}
void PlayerCharacter::moveSelfPlayer(float dt, float camYaw) {
	
	static float time = 0.5f;
	static float timesinceCooldown = 6.0f;
	static bool isaccelerating = false;
	static bool  rushSoundPlayed = false;
	
	float MoveSpeed = 60.0f;
	float RotateSpeed = 100.0f;
	float forwardsMovement = 0.0f;
	float forwardsMovement1 = 0.0f;
	float movement = 0.0f;
	float rotation = 0.0f;
	float cooltime = 6.0f;
	if (abs(GetPhysicsObject()->GetLinearVelocity().x) < 1.0f && abs(GetPhysicsObject()->GetLinearVelocity().z) < 1.0f) {
		GetRenderObject()->setCurrentAnimation(0);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), camYaw));
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * MoveSpeed);
		GetRenderObject()->setCurrentAnimation(1);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), camYaw - 180.0f));
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * MoveSpeed);
		GetRenderObject()->setCurrentAnimation(1);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), camYaw + 90.0f));
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * MoveSpeed);
		GetRenderObject()->setCurrentAnimation(1);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), camYaw - 90.0f));
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * MoveSpeed);
		GetRenderObject()->setCurrentAnimation(1);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F) && isaccelerating && !rushSoundPlayed) {
		audioManager.playRushSound();
		rushSoundPlayed = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F)) {
		if (timesinceCooldown >= cooltime) {
			if (!isaccelerating)
			{
				isaccelerating = true;
				time = 0.5f;
				timesinceCooldown = 0.0f;
			}
		}
	}
	if (isaccelerating)
	{
		GetRenderObject()->setCurrentAnimation(2);
		//forwardsMovement1++;
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * 500.0f);
		time -= dt;
		if (time <= 0.05f)
		{
			GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, 1) * 1500.0f);
		}
		if (time <= 0.0f) {
			isaccelerating = false;
			rushSoundPlayed = false;
		}
	}
	if (timesinceCooldown < cooltime) {
		timesinceCooldown += dt;

	}
	if (Backward()) {

		// if (GetPhysicsObject()->GetLinearVelocity().z > 1.0f) GetPhysicsObject()->SetLinearVelocity({ 0, 0, 0 });
		// if (GetPhysicsObject()->GetLinearVelocity().z > 35.0f) GetPhysicsObject()->SetLinearVelocity({ 0, 0, 0 });
		// if (GetPhysicsObject()->GetLinearVelocity().y > 35.0f) GetPhysicsObject()->SetLinearVelocity({ 0, 0, 0 });
		// GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, -2.0f));

		//GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, -0.5f));
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && Jump()) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 20, 0));
		audioManager.playJumpSound();
		jump = false;
	}

	// character is not rotated using torque, may result in unrealistic rotation
	/*Quaternion playerRot = (GetTransform().GetOrientation());
	if (rotation != 0) {
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), playerRot.ToEuler().y + (rotation * RotateSpeed * Window::GetTimer()->GetTimeDeltaSeconds())));
	}

	if (forwardsMovement != 0) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, forwardsMovement * MoveSpeed * Window::GetTimer()->GetTimeDeltaSeconds()).rotatedByQuaternion(playerRot));
	}
	if (forwardsMovement1 != 0) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, forwardsMovement * MoveSpeed * 4.0 * Window::GetTimer()->GetTimeDeltaSeconds()).rotatedByQuaternion(playerRot));
	}
	if (movement != 0) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, movement * MoveSpeed * Window::GetTimer()->GetTimeDeltaSeconds()).rotatedByQuaternion(playerRot));
	}
	if (movement != 0) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, movement * MoveSpeed * 4.0 * Window::GetTimer()->GetTimeDeltaSeconds()).rotatedByQuaternion(playerRot));
	}*/

	// add some extra drag in the X and Z component to the character, to stop them being so slippy
	Vector3 linVel = GetPhysicsObject()->GetLinearVelocity();
	Vector3 draggedVel = linVel * (1.0f - (0.99f * Window::GetTimer()->GetTimeDeltaSeconds()));
	draggedVel.y = linVel.y;
	GetPhysicsObject()->SetLinearVelocity(draggedVel);
	//TODO remove this cleanly
	//GetRenderObject()->GetCurrentAnimation()->UpdateCurrentFrame(dt);
}

void PlayerCharacter::PickupCollision(GameObject* other) {

	switch (other->GetType()) {
	case PickupGoal:
		addPoints(200);
		hasGoalItem = true;
		break;
	case PickupKey:
		addPoints(50);
		addKey();
		break;
	case PickupInvisibility:
		addPoints(100);
		pickupInvis();
		break;
	}



	// deleting the pickup causes issues so lets just throw it into the void under the map
	switch (other->GetType()) {
	case PickupGoal:
	case PickupKey:
	case PickupInvisibility:
		other->SendToShadowRealm();

	}



}

bool PlayerCharacter::Jump() {
	return jump;
}
bool PlayerCharacter::Move() {
	return move;
}
bool PlayerCharacter::Backward() {
	return backward;
}
PlayerCharacter::PlayerCharacter()
{
	SetRespawnPoint();
	respawnPoint.position = PLAYER_DEFAULT_SPAWN_POS;
}
void PlayerCharacter::SetRespawnPoint()
{
	respawnPoint = { GetTransform().GetPosition(), GetTransform().GetOrientation() };
}
void PlayerCharacter::RespawnAtPoint()
{
	GetTransform().SetPosition(respawnPoint.position);
	GetTransform().SetOrientation(respawnPoint.orientation);

	GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
	GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
}
void PlayerCharacter::OnCollisionBegin(GameObject* otherObject) {

	PickupCollision(otherObject);

	if (otherObject->GetType() == Door && getNumKeys() > 0) {
		numKeys--;
		otherObject->SendToShadowRealm();
		addPoints(50);
	}
	else if (otherObject->GetType() == Enemy) {
		points -= 150;
		GetTransform().SetPosition(initialPosition);
		//std::cout << "Goat Attacked! respawning...\n";
	}
	else if (otherObject->GetType() == StartingLocation && hasGoalItem) {
		//end game
		//std::cout << "GAME WIN\n";
		hasWonGame = true;
	}
	else if (otherObject->GetType() == Floor || otherObject->GetType() == Floor2) {
		jump = true;
	}
	else {
		jump = false;
	}
	if (otherObject->GetType() == Barrier) {
		backward = true;
	}
	else {
		backward = false;
	}

	if (otherObject->GetType() == Ball) {
		canAttack = true;
		otherObject->SetBoundingVolume(nullptr);
		otherObject->SetActive(false);
	}
	if (otherObject->GetType() == ATKBall) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 50, 0));
		otherObject->SetBoundingVolume(nullptr);
		otherObject->SetActive(false);
		move = false;
	}
	if (otherObject->GetType() == Floor2)
	{
		otherObject->GetPhysicsObject()->SetInverseMass(1.0f);
	}
	else if (otherObject->GetType() == Rhino) {
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 30, 0));
		GetPhysicsObject()->ApplyLinearImpulse(otherObject->GetTransform().GetOrientation() * Vector3(0, 0, 80));
	}

	if (otherObject->GetType() == TrueDoor) {

	}
	else {

	}

	if (otherObject->GetType()==Goal)
	{
		win = true;
		//std::cout << "GAME WIN\n";
	}

	if (otherObject->GetType()==LongCube)
	{
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 30, 0));
		GetPhysicsObject()->ApplyLinearImpulse(otherObject->GetTransform().GetOrientation() * Vector3(0, 0, 30));
	}
}

void PlayerCharacter::UpdateObject()
{
	if (invisTime > 0.0f) {
		invisTime -= Window::GetTimer()->GetTimeDeltaSeconds();
	}

	// std::cout << GetTransform().GetPosition() << std::endl;
}
