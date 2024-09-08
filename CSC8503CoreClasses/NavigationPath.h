#pragma once
#include "Vector3.h"
#include <vector>
#include "../CSC8503CoreClasses/Debug.h"

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationPath		{
		public:
			NavigationPath() {}
			~NavigationPath() {}

			void	Clear() {
				waypoints.clear();
			}
			void	PushWaypoint(const Vector3& wp) {
				waypoints.emplace_back(wp);
			}
			bool	PopWaypoint(Vector3& waypoint) {
				if (waypoints.empty()) {
					return false;
				}
				waypoint = waypoints.back();
				waypoints.pop_back();
				return true;
			}
			void DrawPath() {
				for (int i = 1; i < waypoints.size(); i++) {
					Vector3 a = waypoints[(uint8_t)i - (uint8_t)1];
					Vector3 b = waypoints[i];

					Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
				}
			}
		protected:

			std::vector <Vector3> waypoints;
		};
	}
}

