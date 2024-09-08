#include "TriggerBox.h"

TriggerBox::TriggerBox()
{
	//addCollisionBeginFunction([this]() {this->collideEnter();} );
	//addCollisionEndFunction([this]() {this->collideExit(); });
}

TriggerBox::~TriggerBox()
{
}

std::size_t TriggerBox::addCollisionBeginFunction(std::function<void()> func)
{
	return addFunctionToVector(OnCollisionBeginFunctions, func);
}

std::size_t TriggerBox::addCollisionEndFunction(std::function<void()> func)
{
	return addFunctionToVector(OnCollisionEndFunctions, func);
}

void TriggerBox::removeCollisionEndFunction(std::size_t index)
{
	removeFunctionFromVec(OnCollisionEndFunctions, index);
}

void TriggerBox::removeCollisionBeginFunction(std::size_t index)
{
	removeFunctionFromVec(OnCollisionBeginFunctions, index);
}

void TriggerBox::collideEnter()
{
	std::cout << "Trigger box collision enter!\n";
}

void TriggerBox::collideExit()
{
	std::cout << "Trigger box collision exit!\n";
}

std::size_t TriggerBox::addFunctionToVector(std::vector<std::function<void()>>& vec, std::function<void()> func)
{
	if (auto i = std::find(vec.begin(), vec.end(), nullptr); i != vec.end()) {
		*i = func;
		return std::distance(vec.begin(), i);
	}
	else {
		vec.push_back(func);
		return vec.size() - 1;
	}
}

void TriggerBox::removeFunctionFromVec(std::vector<std::function<void()>>& vec, std::size_t index)
{
	vec[index] = nullptr;
}
