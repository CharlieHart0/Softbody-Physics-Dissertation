#include "TriggerBox.h"
#include "PlayerCharacter.h"

class Checkpoint : public TriggerBox {
public:
	void OnCollisionBegin(GameObject* other);
    void OnCollisionEnd(GameObject* other) { return; };
};

class RespawnTrigger : public TriggerBox {
public:
	void OnCollisionBegin(GameObject* other);
 	void OnCollisionEnd(GameObject* other) { return; };
};