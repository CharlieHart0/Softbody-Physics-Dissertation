#pragma once
using namespace NCL::CSC8503;

#define PLAYER_DEFAULT_SPAWN_POS Vector3(0,-17,155)

struct TransformData {
	Vector3 position;
	Quaternion orientation;
};

class PlayerCharacter : public GameObject {
public:
	bool PushForward(GameWorld* world, float pushRange);
	float distanceBetween(Vector3 a, Vector3 b);//get distance between tow  points
	void moveSelfPlayer(float dt, float camYaw);
	int getNumKeys() { return numKeys; }
	void addKey() { numKeys++; }
	void pickupInvis() { invisTime += 10; }
	float getInvisTime() { return invisTime; }
	void PickupCollision(GameObject*);
	bool GetHasGoalItem() { return hasGoalItem; }
	void OnCollisionBegin(GameObject* otherObject);
	void UpdateObject();
	int getPoints() { return points; }
	void clearPoints() { points = 0; }
	void addPoints(int numbertoadd) { points += numbertoadd; }
	bool GetHasWonGame() { return hasWonGame; }
	void SetInitPosition(Vector3 l) { initialPosition = l; }
	bool Jump();
	bool Move();
	bool Backward();
	bool canAttack;
	bool passLevel1 = false;
	bool passLevel2 = false;
	bool passLevel3 = false;
	bool passLevel4 = false;
	bool win = false;

	PlayerCharacter();

	void SetRespawnPoint();
	void RespawnAtPoint();
protected:
	float yaw;
	bool jump = false;
	bool move = true;
	bool backward = false;
	int numKeys = 0;
	float invisTime = 0.0f;
	bool hasGoalItem = false;
	int points = 0;
	bool hasWonGame = false;
	Vector3 initialPosition = Vector3(0, 0, 0);
	TransformData respawnPoint;
};