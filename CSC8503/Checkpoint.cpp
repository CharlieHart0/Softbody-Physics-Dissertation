#include "Checkpoint.h"

void Checkpoint::OnCollisionBegin(GameObject* other)
{
	if (other->GetType() != PlayerUser) {
		return;
	}
	((PlayerCharacter*)other)->SetRespawnPoint();
}

void RespawnTrigger::OnCollisionBegin(GameObject* other)
{
	if (other->GetType() != PlayerUser) {
		return;
	}
	((PlayerCharacter*)other)->RespawnAtPoint();
}
