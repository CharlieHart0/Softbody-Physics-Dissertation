/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include <algorithm>
using namespace NCL;
using namespace Maths;


Vector3::Vector3(const Vector2& v2, float newZ) : x(v2.x), y(v2.y), z(newZ) {
}

Vector3::Vector3(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) {
}

float NCL::Maths::Vector3::angleBetween(Vector3 b)
{
	return acos(Vector3::Dot(*this, b) / (this->Length() * b.Length()));
}

Vector3 NCL::Maths::Vector3::rotatedByQuaternion(Quaternion& q)
{
	Vector3 qVec = Vector3(q.x, q.y, q.z);

	return (qVec * (Vector3::Dot(qVec, *this) * 2.0f) +
		(*this * (q.w * q.w - Vector3::Dot(qVec, qVec))) +
		(Vector3::Cross(qVec, *this) * 2.0f * q.w));

}

 Vector3 Vector3::Clamp(const Vector3& input, const Vector3& mins, const Vector3& maxs) {
	return Vector3(
		std::clamp(input.x, mins.x, maxs.x),
		std::clamp(input.y, mins.y, maxs.y),
		std::clamp(input.z, mins.z, maxs.z)
	);
}

 Vector3 Vector3::ProjectedOnto(Vector3& t_other)
 {
	 float units =  Vector3::Dot(*this, t_other) / t_other.Length();
	 *this = this->Normalised() * units;
	 return *this;
 }