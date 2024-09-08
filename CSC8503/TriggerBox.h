#pragma once
#include "..\CSC8503CoreClasses\GameObject.h"
#include <functional>
#include <set>

class TriggerBox :
    public NCL::CSC8503::GameObject
{
public:
	TriggerBox();
	~TriggerBox();

	void OnCollisionBegin(GameObject* otherObject) {
		for (std::function<void()> func : OnCollisionBeginFunctions) { func(); }
	}

	void OnCollisionEnd(GameObject* otherObject) {
		for (std::function<void()> func : OnCollisionEndFunctions) { func(); }
	}

	std::size_t addCollisionBeginFunction(std::function<void()> func);
	std::size_t addCollisionEndFunction(std::function<void()> func);

	void removeCollisionEndFunction(std::size_t index);
	void removeCollisionBeginFunction(std::size_t index);
protected:
	std::vector<std::function<void()>> OnCollisionBeginFunctions;
	std::vector<std::function<void()>> OnCollisionEndFunctions;

	void collideEnter();
	void collideExit();

	std::size_t addFunctionToVector(std::vector<std::function<void()>>& vec, std::function<void()> func);

	void removeFunctionFromVec(std::vector<std::function<void()>>& vec, std::size_t index);
};

