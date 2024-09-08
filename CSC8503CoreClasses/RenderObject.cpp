#include "RenderObject.h"
#include "MeshGeometry.h"
#include "../CSC8503/DebugOptions.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {

	if (!DEBUG_ALLOW_RENDEROBJ_NULLPTR_MESH && mesh == nullptr)
	{
		throw std::logic_error(__FUNCTION__ ": not allowed to have nullptr mesh!");
	}
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->mat		= nullptr;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	this->offsetPos = Vector3(0, 0, 0);
	this->currentAnimation = nullptr;
	this->animated	= false;
	this->isEvilAsset = false;
}

RenderObject::~RenderObject() {
	delete mesh, mat, texture, shader, transform, currentAnimation, animations;
}

void RenderObject::addAnimations(std::vector<MeshAnimation*> newAnis) {
	
	for(auto& ani: newAnis) {
		addAnimation(ani);
	}

}