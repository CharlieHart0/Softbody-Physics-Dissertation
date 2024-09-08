#include "SpringConstraint.h"
#include "GameObject.h"

NCL::CSC8503::SpringConstraint::SpringConstraint(SpringType t_type, GameObject* t_objA, GameObject* t_objB, float t_distance, float t_springConstant, float t_tolerance, float t_dampingFactor)
{
	m_objectA = t_objA;
	m_objectB = t_objB;
	m_springConstant = t_springConstant;
	m_dampingFactor = t_dampingFactor;
	m_idealDistance = t_distance;
	m_distanceTolerance = t_tolerance;
	m_springType = t_type;
}

NCL::CSC8503::SpringConstraint::~SpringConstraint()
{
}

void NCL::CSC8503::SpringConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePos = m_objectA->GetTransform().GetPosition() - m_objectB->GetTransform().GetPosition();
	Vector3 relPosNormalised = relativePos.Normalised();

	float currentDistance = relativePos.Length();
	// in extension, negative, in compression, positive
	float offset = m_idealDistance - currentDistance;
	if (m_springType == Compression && offset > (0 + m_distanceTolerance))
	{
		
	}
	else if(m_springType == Extension && offset < (0- m_distanceTolerance))
	{
		
	}
	else
	{
		return;
	}
	
	Vector3 force = relPosNormalised.Scaled(m_springConstant * offset * -1);
	
	Vector3 velocityDifference = m_objectA->GetPhysicsObject()->GetLinearVelocity() - m_objectB->GetPhysicsObject()->GetLinearVelocity();

	Vector3 dampingForce = relPosNormalised.Scaled(Vector3::Dot(velocityDifference, relPosNormalised) * m_dampingFactor);
	
	force += dampingForce;

	m_objectA->GetPhysicsObject()->AddForce(-(force));
	m_objectB->GetPhysicsObject()->AddForce(force);

}
