#include "Softbody.h"
#include "MeshGeometry.h"
#include "SphereVolume.h"
#include "Debug.h"
#include "GameWorld.h"
#include "RenderObject.h"
#include "Window.h"
#include "SpringConstraint.h"
#include "OGLMesh.h"



using namespace NCL;

Softbody::Softbody(NCL::MeshGeometry* t_mesh, Vector3 t_pos, GameWorld& t_world, float t_scale , TextureBase* t_tex , ShaderBase* t_shader, std::string t_name)
{
	m_world = &t_world;
	transform.SetPosition(t_pos);

	if (SOFTBODY_RENDER_DYNAMICALLY)
	{
		m_dynamicMesh = new OGLMesh(t_mesh->GetFileName());
		m_dynamicMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
		m_dynamicMesh->UploadToGPU();

		if (t_tex != nullptr && t_shader != nullptr)
		{
			renderObject = new RenderObject(&transform, m_dynamicMesh, t_tex, t_shader);
		}
	}


	// add nodes
	std::vector<Vector3> nodePositions = t_mesh->GetPositionData();
	for (std::vector<Vector3>::iterator i = nodePositions.begin(); i != nodePositions.end(); i++)
	{
		(*i).Scale(t_scale);
		//TODO change mass 
		int index = FindIndexOfNodeAtPosition(transform.GetPosition() + (*i));

		//if there is a node at that position already
		if (index != -1)
		{
			m_nodeIndexToVertexIndices[index].push_back(std::distance(nodePositions.begin(), i));
			continue;
		}
		SoftbodyNode* node = new SoftbodyNode(3, transform.GetPosition() + (*i), t_world);
		m_nodes.push_back(node);
		m_nodeIndexToVertexIndices[m_nodes.size()-1].push_back(std::distance(nodePositions.begin(), i));
		
	}
	

	// add springs
	std::vector<unsigned int> indices = t_mesh->GetIndexData();
	for (std::vector<unsigned int>::iterator i = indices.begin(); i != indices.end();)
	{
		unsigned int triIndices[3][2];
		triIndices[0][0] = *i;
		triIndices[0][1] = *(i + 1);
		triIndices[1][0] = *(i + 1);
		triIndices[1][1] = *(i + 2);
		triIndices[2][0] = *(i + 2);
		triIndices[2][1] = *i;
		
		for (int i = 0; i < 3; i++)
		{
			//make sure we dont have duplicate nodes
			for (int j = 0; j < 2; j++)
			{
				int index = FindIndexOfNodeAtPosition(nodePositions[triIndices[i][j]]+ transform.GetPosition());
				if (index != -1)
				{
					triIndices[i][j] = index;
				}
			}

			// dont add a duplicate spring pair
			if (DoesSpringSetExist({triIndices[i][0],triIndices[i][1]}))
			{
				continue;
			}
			AddSpringsBetweenPoints(triIndices[i][0], triIndices[i][1]);
		}
		
		i += 3;
	}

	unsigned int extraSupports[12] = {0,4,4,3,3,7,7,0,5,6,2,1};
	for (int i = 0; i < 12; i += 2)
	{
		AddSpringsBetweenPoints(extraSupports[i], extraSupports[i + 1]);
	}

	
}

Softbody::~Softbody()
{
	delete m_dynamicMesh;
	for (std::vector<SoftbodyNode*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
	{
		delete *i;
	}
	// delete spring constraints
	for(std::map<std::set<unsigned int>, std::vector<Constraint*>>::iterator i = m_springs.begin(); i != m_springs.end(); i++){
		for (std::vector<Constraint*>::iterator j = (*i).second.begin(); j != (*i).second.end(); i++)
		{
			m_world->RemoveConstraint((*j),true);
		}
	}
	
}

int Softbody::FindIndexOfNodeAtPosition(Vector3 t_pos)
{
	if (m_nodes.size() == 0) {return -1;}
	for (int i = 0; i < m_nodes.size(); i++)
	{
		if (m_nodes[i]->GetTransform().GetPosition() == t_pos)
		{
			return i;
		}
	}
	return -1;
}

bool Softbody::DoesSpringSetExist(std::set<unsigned int> t_set)
{
	return m_springs.find(t_set) != m_springs.end();
}

void Softbody::AddSpringsBetweenPoints(unsigned int t_a, unsigned int t_b)
{
	std::vector<Constraint*> constraints;
	float distance = (m_nodes[t_a]->GetTransform().GetPosition() - m_nodes[t_b]->GetTransform().GetPosition()).Length();
	/*if (distance == 1.41421354f)
	{
		std::cout << triIndices[i][1] << ", " << triIndices[i][0] << std::endl;
	}*/
	constraints.push_back(
		new SpringConstraint(Compression, m_nodes[t_a], m_nodes[t_b], distance - 0.0f, SOFTBODY_SPRING_CONSTANT * 0.8f, 0, SOFTBODY_DAMPING_FACTOR)
	);
	constraints.push_back(
		new SpringConstraint(Extension, m_nodes[t_a], m_nodes[t_b], distance + 0.0f, SOFTBODY_SPRING_CONSTANT, 0, SOFTBODY_DAMPING_FACTOR)
	);
	m_springs[{t_a, t_b}] = constraints;
	m_world->AddConstraint(constraints[0]);
	m_world->AddConstraint(constraints[1]);
}

//returns the softbody node index of a vertex index. only returns first node index it finds.
unsigned int Softbody::VertIndexToNodeIndex(unsigned int t_vertIndex)
{
	for (std::map<unsigned int, std::vector<unsigned int>>::iterator i = m_nodeIndexToVertexIndices.begin(); i != m_nodeIndexToVertexIndices.end(); i++)
	{
		std::vector<unsigned int>::iterator foundIt =std::find((*i).second.begin(), (*i).second.end(), t_vertIndex);
		if (foundIt != (*i).second.end())
		{
			return (*i).first;
		}
		
	}
}

SoftbodyNode::SoftbodyNode(float t_invMass, Vector3 t_pos, GameWorld& t_world)
{
	transform.SetPosition(t_pos);
	boundingVolume = (NCL::CollisionVolume*) new NCL::SphereVolume(SOFTBODY_NODE_RADIUS);
	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(t_invMass);
	physicsObject->SetConstraints(ConstrainAll);


	t_world.AddGameObject(this);
}

void Softbody::UpdateObject()
{
	//draw debug lines for all springs
	if (renderObject == nullptr || SOFTBODY_SHOW_SPRINGS)
	{
		for (std::map<std::set<unsigned int>, std::vector<Constraint*>>::iterator i = m_springs.begin(); i != m_springs.end(); i++)
		{
			std::set<unsigned int>::iterator j = (*i).first.begin();
			SoftbodyNode* start = m_nodes[*j];
			j++;
			SoftbodyNode* end = m_nodes[*j];
			Debug::DrawLine(start->GetTransform().GetPosition(), end->GetTransform().GetPosition(), Vector4(1, 0, 1, 1));
		}
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::N))
	{
		for (std::vector<SoftbodyNode*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		{
			if ((*i)->GetPhysicsObject()->GetConstraints() == ConstrainAll)
			{
				(*i)->GetPhysicsObject()->SetConstraints(ConstrainNone);
			}
		}
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::M))
	{
		for (std::vector<SoftbodyNode*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		{
			if ((*i)->GetPhysicsObject()->GetConstraints() == ConstrainNone)
			{
				(*i)->GetPhysicsObject()->SetConstraints(ConstrainAll);
			}
		}
	}


	// this stuff should probably be done inside a shader instead
	if (renderObject != nullptr && SOFTBODY_RENDER_DYNAMICALLY)
	{
		// set dynamic mesh vertex positions
		for (std::map<unsigned int, std::vector<unsigned int>>::iterator i = m_nodeIndexToVertexIndices.begin(); i != m_nodeIndexToVertexIndices.end(); i++)
		{
			Vector3 nodePos = m_nodes[(*i).first]->GetTransform().GetPosition();
			Vector3 transformToNode = transform.GetPosition().to(nodePos);
			for (std::vector<unsigned int>::iterator j = (*i).second.begin(); j != (*i).second.end(); j++)
			{
				m_dynamicMesh->SetVertexPosition(*j, transformToNode);
			}
		}
		std::vector<unsigned int> indexData = m_dynamicMesh->GetIndexData();
		//calculate new normals
		for (std::vector<unsigned int>::iterator i = indexData.begin(); std::distance(indexData.begin(), i) < std::distance(indexData.begin(), indexData.end());)
		{
			Vector3 vertexPos[3];
			for (int j = 0; j < 3; j++)
			{
				vertexPos[j] = m_nodes[VertIndexToNodeIndex(*i)]->GetTransform().GetPosition();
				i++;
			}
			Vector3 normal = Vector3::Cross((vertexPos[1] - vertexPos[0]), (vertexPos[2] - vertexPos[0]));
			unsigned int lastIndex = std::distance(indexData.begin(), i);
			for (int j = 1; j < 4; j++)
			{
				m_dynamicMesh->SetVertexNormal(lastIndex - j, normal);
			}
		}

		m_dynamicMesh->UploadToGPU();
	}
}

SoftbodyNode::~SoftbodyNode()
{
}
