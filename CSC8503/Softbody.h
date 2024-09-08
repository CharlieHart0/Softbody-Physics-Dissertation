#pragma once
#include "GameObject.h"
#include "TextureBase.h"
#include "ShaderBase.h"
#include <set>
using namespace NCL::CSC8503;
using namespace NCL::Rendering;

#define SOFTBODY_NODE_RADIUS 0.1f

#define SOFTBODY_SPRING_CONSTANT 20.0f
#define SOFTBODY_DAMPING_FACTOR 0.2f

#define SOFTBODY_SHOW_SPRINGS false
#define SOFTBODY_RENDER_DYNAMICALLY true

namespace NCL
{
	class MeshGeometry;
	namespace CSC8503
	{
		class Constraint;
	}
	
}

class SoftbodyNode : public GameObject
{
public:
	SoftbodyNode(float t_invMass, Vector3 t_pos, GameWorld& t_world);
	~SoftbodyNode();
protected:

};

class Softbody : public GameObject
{
public:
	Softbody(NCL::MeshGeometry* t_mesh,Vector3 t_pos,GameWorld& t_world,float t_scale = 1.0f,TextureBase* t_tex = nullptr, ShaderBase* t_shader = nullptr,std::string t_name = "");
	void UpdateObject();
	~Softbody();
protected:
	int FindIndexOfNodeAtPosition(Vector3 t_pos);
	bool DoesSpringSetExist(std::set<unsigned int> t_set);
	void AddSpringsBetweenPoints(unsigned int t_a, unsigned int t_b);
	unsigned int VertIndexToNodeIndex(unsigned int t_vertIndex);
	std::vector<SoftbodyNode*> m_nodes;
	std::map<unsigned int,std::vector<unsigned int>> m_nodeIndexToVertexIndices;
	std::map<std::set<unsigned int>, std::vector<Constraint*>> m_springs;
	GameWorld* m_world;
	NCL::MeshGeometry* m_dynamicMesh;
};

