#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	class MeshMaterial;
	class MeshAnimation;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetMaterial(MeshMaterial* m) {
				mat = m;
			}

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			MeshMaterial* GetMat() const {
				return mat;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			Vector4			colour;

			void setAnimated(bool ani) { animated = ani; }

			bool isAnimated() const { return animated; }

			void clearAnimations() { animations.clear(); }

			void addAnimation(MeshAnimation* newAni) { animations.emplace_back(newAni); }

			void addAnimations(std::vector<MeshAnimation*> newAnis);

			void setCurrentAnimation(int index) { 
				if (!animated) { return; }
				currentAnimation = animations[index]; 
			}

			MeshAnimation* GetCurrentAnimation() const { return currentAnimation; }

			void setIsEvilAsset(bool eAsset) { isEvilAsset = eAsset; }

			bool getIsEvilAsset() const { return isEvilAsset; }

			Vector3 getOffsetPos() const { return offsetPos; }

			void setOffsetPos(Vector3 pos) { offsetPos = pos; }

		protected:
			MeshGeometry*   mesh;
			MeshMaterial*   mat;
			TextureBase*	texture;
			ShaderBase*		shader;
			Transform*		transform;

			Vector3 offsetPos;
			MeshAnimation*	currentAnimation;
			std::vector<MeshAnimation*> animations;
			bool			animated;

			bool			isEvilAsset;
		};
	}
}
