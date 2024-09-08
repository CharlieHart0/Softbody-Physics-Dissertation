#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "Window.h"
#include "Maths.h"
#include "Debug.h"
using namespace NCL;

typedef Vector2 Interval;
typedef Vector3 vec3;
typedef Matrix3 mat3;
typedef Transform mat43;



Interval get_interval( const vec3& obb_extents, const mat43 &obb_transform, const vec3& axis) {
    vec3 corners[8];

    vec3 C = obb_transform.GetPosition();	// OBB Center
    vec3 E = obb_extents;		// OBB Extents
    Matrix3 mat3( obb_transform.GetOrientation() )  ;

    const float* o = &mat3.array[0][0];
    vec3 A[] = {			// OBB Axis
        vec3(o[0], o[1], o[2]),
        vec3(o[3], o[4], o[5]),
        vec3(o[6], o[7], o[8]),
    };

    corners[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    corners[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    corners[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    corners[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    corners[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    corners[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    corners[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    corners[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

    Interval result;
    result[0] = result[1] = vec3::Dot( axis, corners[0]);

    for (int i = 1; i < 8; ++i) {
        float projection = vec3::Dot(axis, corners[i]);

        result[0] = (projection < result[0]) ? projection : result[0];
        result[1] = (projection > result[1]) ? projection : result[1];
    }

    return result;
};

bool overlap_on_axis(const vec3& obb1_extents, const mat43 &obb1_transform,  const vec3& obb2_extents, const mat43& obb2_transform, const vec3& axis) {
    Interval a = get_interval(obb1_extents, obb1_transform, axis);
    Interval b = get_interval(obb2_extents, obb2_transform, axis);
    return ((b[0] <= a[1]) && (a[0] <= b[1]));
};

// 计算OBB和OBB的相交信息
bool intersection(const vec3& obb1_extents, const mat43 & obb1_transform, const vec3& obb2_extents, const mat43 &oob2_transform ) {

    Matrix3 axis1(obb1_transform.GetOrientation());
    Matrix3 axis2(oob2_transform.GetOrientation());


	const float* o1 = &axis1.array[0][0];
	const float* o2 = &axis2.array[0][0];

	vec3 test[15] = {
        axis1.GetColumn(0),
        axis1.GetColumn(1),
        axis1.GetColumn(2),

        axis2.GetColumn(0),
        axis2.GetColumn(1),
        axis2.GetColumn(2),
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
        for (int j=0; j < 3; ++j) {
		    test[6 + i * 3 + j] = vec3::Cross(test[i], test[j]);
        }
	}

	for (int i = 0; i < 15; ++i) {
		if (!overlap_on_axis(obb1_extents, obb1_transform, obb2_extents, oob2_transform, test[i])) {
			return false; // Seperating axis found
		}
	}

	return true; // Seperating axis not found
}

float penetration_depth(const vec3& obb1_extents, const mat43 &obb1_transform, const vec3& obb2_extents, const mat43 &obb2_transform, const vec3& axis, bool* out_should_flip) {
	
    Interval i1 = get_interval(obb1_extents, obb1_transform, axis.Normalised() );
	Interval i2 = get_interval(obb2_extents, obb2_transform, axis.Normalised() );

	if (!((i2[0] <= i1[1]) && (i1[0] <= i2[1]))) {
		return 0.0f; // No penerattion
	}

	float len1 = i1[1] - i1[0];
	float len2 = i2[1] - i2[0];
	float min = fminf(i1[0], i2[0]);
	float max = fmaxf(i1[1], i2[1]);
	float length = max - min;

	if (out_should_flip != 0) {
		*out_should_flip = (i2[0] < i1[0]);
	}

	return (len1 + len2) - length;
}

struct line_t {
	vec3 start;
	vec3 end;

	inline line_t() {}
	inline line_t(const vec3& s, const vec3& e) :
		start(s), end(e) { }
} ;

struct plane_t {
	vec3 normal;
	float distance;

	inline plane_t() : normal(1, 0, 0) { }
	inline plane_t(const vec3& n, float d) :
		normal(n), distance(d) { }
};

inline float  CMP( float x, float y) {
    return 	(fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));
}

bool clip_to_plane(const plane_t& plane, const line_t& line, vec3* outPoint) {
	vec3 ab = line.end - line.start;

	float nA = vec3::Dot(plane.normal, line.start);
	float nAB = vec3::Dot(plane.normal, ab);

	if (CMP(nAB, 0)) {
		return false;
	}

	float t = (plane.distance - nA) / nAB;
	if (t >= 0.0f && t <= 1.0f) {
		if (outPoint != 0) {
			*outPoint = line.start + ab * t;
		}
		return true;
	}

	return false;
}

std::vector<plane_t> get_planes(const vec3& obb_extents, const mat43 &obb_transform ) {
	vec3 c = obb_transform.GetPosition();	// OBB Center
	vec3 e = obb_extents;		// OBB Extents
	const float* o = &Matrix3(  obb_transform.GetOrientation() ).array[0][0];
	vec3 a[] = {			// OBB Axis
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8]),
	};

	std::vector<plane_t> result;
	result.resize(6);

	result[0] = plane_t(a[0]        ,  vec3::Dot (a[0], (c + a[0] * e.x)));
	result[1] = plane_t(a[0] * -1.0f, -vec3::Dot (a[0], (c - a[0] * e.x)));
	result[2] = plane_t(a[1]        ,  vec3::Dot (a[1], (c + a[1] * e.y)));
	result[3] = plane_t(a[1] * -1.0f, -vec3::Dot (a[1], (c - a[1] * e.y)));
	result[4] = plane_t(a[2]        ,  vec3::Dot (a[2], (c + a[2] * e.z)));
	result[5] = plane_t(a[2] * -1.0f, -vec3::Dot (a[2], (c - a[2] * e.z)));

	return result;
}

bool point_In_OBB(const vec3& point, const vec3& obb_extents, const mat43 &obb_transform ) {
	vec3 dir = point - obb_transform.GetPosition() ;

    const float *o = &Matrix3( obb_transform.GetOrientation() ).array[0][0];

	for (int i = 0; i < 3; ++i) {
		const float* orientation = & o[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);

		float distance =  vec3::Dot (dir, axis);

		if (distance > obb_extents[i]) {
			return false;
		}
		if (distance < -obb_extents[i]) {
			return false;
		}
	}

	return true;
}

std::vector<vec3> clip_edges_to_OBB(const std::vector<line_t>& edges, const vec3& obb_extents, const mat43 &obb_transform ) {
	std::vector<vec3> result;
	result.reserve(edges.size() * 3);
	vec3 intersection;

	std::vector<plane_t> planes = get_planes(obb_extents, obb_transform );

	for (int i = 0; i < planes.size(); ++i) {
		for (int j = 0; j < edges.size(); ++j) {
			if (clip_to_plane(planes[i], edges[j], &intersection)) {
				if (point_In_OBB(intersection, obb_extents, obb_transform)) {
					result.push_back(intersection);
				}
			}
		}
	}

	return result;
}


std::vector<vec3> get_vertices(const vec3& obb_extents, const mat43 &obb_transform ) {
	std::vector<vec3> v;
	v.resize(8);

	vec3 C = obb_transform.GetPosition();	// OBB Center
	vec3 E = obb_extents;		// OBB Extents
	const float* o = &Matrix3(  obb_transform.GetOrientation() ).array[0][0];
	vec3 A[] = {			// OBB Axis
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8]),
	};

	v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	return v;
}

std::vector<line_t> get_edges(const vec3& obb_extents, const mat43 & obb_transform ) {
	std::vector<line_t> result;
	result.reserve(12);
	std::vector<vec3> v = get_vertices(obb_extents, obb_transform);

	int index[][2] = { // Indices of edges
		{ 6, 1 },{ 6, 3 },{ 6, 4 },{ 2, 7 },{ 2, 5 },{ 2, 0 },
		{ 0, 1 },{ 0, 3 },{ 7, 1 },{ 7, 4 },{ 4, 5 },{ 5, 3 }
	};

	for (int j = 0; j < 12; ++j) {
		result.push_back(line_t(
			v[index[j][0]], v[index[j][1]]
		));
	}

	return result;
}

bool CollisionDetection::OBBIntersection(const OBBVolume& volumeA, const Transform& obb1_transform,
	const OBBVolume& volumeB, const Transform& obb2_transform, CollisionInfo& collisionInfo) 
    
{
    const vec3 obb1_extents =  volumeA.GetHalfDimensions();
    const vec3 obb2_extents =  volumeB.GetHalfDimensions();

    float result_depth = FLT_MAX;

	Matrix3 axis1 = obb1_transform.GetOrientation();
	Matrix3 axis2 = obb2_transform.GetOrientation();

	Vector3 test[15] = {
		axis1.GetColumn(0),
		axis1.GetColumn(1),
		axis1.GetColumn(2),

		axis2.GetColumn(0),
		axis2.GetColumn(1),
		axis2.GetColumn(2),
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
        for (int j=0; j < 3; ++j) {
		    test[6 + i * 3 + j] =  Vector3::Cross(test[i], test[j]);
        }
	}


	Vector3* hit_normal = 0;
	bool should_flip;

	for (int i = 0; i < 15; ++i) {
		if (test[i].x < 0.000001f) test[i].x = 0.0f;
		if (test[i].y < 0.000001f) test[i].y = 0.0f;
		if (test[i].z < 0.000001f) test[i].z = 0.0f;
		if ( (test[i].LengthSquared()) < 0.001f )  {
			continue;
		}

		float depth = penetration_depth(obb1_extents, obb1_transform,  obb2_extents, obb2_transform, test[i], &should_flip);
		if (depth <= 0.0f) {
			return false;
		}
		else if (depth < result_depth) {
			if (should_flip) {
				test[i] = test[i] * -1.0f;
			}
			result_depth = depth;
			hit_normal = &test[i];
		}
	}

	if (hit_normal == 0) {
		return false;
	}



	vec3 axis = hit_normal->Normalised();

	std::vector<vec3> c1 = clip_edges_to_OBB(get_edges( obb2_extents, obb2_transform), obb1_extents, obb1_transform );
	std::vector<vec3> c2 = clip_edges_to_OBB(get_edges( obb1_extents, obb1_transform), obb2_extents, obb2_transform );



	std::vector<vec3> contacts;
    contacts.reserve(c1.size() + c2.size());
	contacts.insert(contacts.end(), c1.begin(), c1.end());
	contacts.insert(contacts.end(), c2.begin(), c2.end());

	Interval iterval = get_interval(obb1_extents, obb1_transform, axis);
	float distance = (iterval[1] - iterval[0])* 0.5f - result_depth * 0.5f;
	vec3 pointOnPlane = obb1_transform.GetPosition() + axis * distance;

    for ( int i = (int)contacts.size() - 1; i >= 0; --i) {

		vec3 contact = contacts[i];
		contacts[i] = contact + (axis * vec3::Dot(axis, pointOnPlane - contact));
		
		// This bit is in the "There is more" section of the book
		for ( int j = (int)contacts.size() - 1; j > i; --j) {
			if (  (contacts[j] - contacts[i]).LengthSquared() < 0.0001f) {
				contacts.erase(contacts.begin() + j);
				break;
			}
		}
	}    

    auto obb1_inv = obb1_transform.GetOrientation().Conjugate();
    auto obb2_inv = obb2_transform.GetOrientation().Conjugate();

    for ( int i=0; i<contacts.size(); ++i ) {
        vec3 contact = contacts[i];

		// Debug::DrawLine(contact, contact + axis  , Vector4(1, 0, 1, 1));

        vec3 a =  Vector3();
        vec3 b =  obb2_inv * (contact - obb2_transform.GetPosition())  ;

	    collisionInfo.AddContactPoint(a, b, axis, result_depth);

    }

	return true;
}

bool CollisionDetection::AABBOBBIntersection(	const AABBVolume& volumeA	 , const Transform& worldTransformA,
										const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) 
{
	return OBBIntersection( (const OBBVolume&) volumeA, worldTransformA, volumeB, worldTransformB , collisionInfo );
}

vec3 obb_closest_point(const vec3& obb_position, const mat3 &obb_axis, const vec3 &obb_extent, const vec3& point) {
	vec3 result = obb_position;
	vec3 dir = point - obb_position;

	const float *axis_float_ptr = (const float *) &obb_axis.array;

	for (int i = 0; i < 3; ++i) {
		const float* orientation = &axis_float_ptr[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);

		float distance = Vector3::Dot(dir, axis);

		if (distance > obb_extent[i]) {
			distance = obb_extent[i];
		}
		if (distance < -obb_extent[i]) {
			distance = -obb_extent[i];
		}

		result = result + (axis * distance);
	}

	return result;
}

bool  CollisionDetection::OBBSphereIntersection(const OBBVolume& obb, const Transform& obb_transform,
	const SphereVolume& sphere, const Transform& sphere_transform, CollisionInfo& collisionInfo) {
	
	float sphere_radius = sphere.GetRadius();
	vec3 obb_origin = obb_transform.GetPosition();
	vec3 sphere_origin = sphere_transform.GetPosition();

	vec3 closest_point = obb_closest_point(obb_origin, obb_transform.GetOrientation(), obb.GetHalfDimensions(), sphere_origin );

	float distanceSq =   (closest_point - sphere_origin ).LengthSquared() ;
	if (distanceSq > sphere_radius * sphere_radius) {
		return false;
	}

	vec3 normal =  ( sphere_origin - closest_point).Normalised();

	vec3 outside_point = sphere_origin - normal * sphere_radius;

	vec3 distance_vec = outside_point - closest_point;

	float distance = distance_vec.Length();

	vec3 contact =  ( closest_point + distance_vec * 0.5f );

	//Debug::DrawLine(contact, contact + normal * distance  , Vector4(1, 0, 1, 1));

	vec3 a =  Vector3(); 
	vec3 b =  sphere_transform.GetOrientation().Conjugate()  *  contact ;

	collisionInfo.AddContactPoint(a, b, normal, distance * 0.5f);

	return true;
}

