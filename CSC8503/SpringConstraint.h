#pragma once
#include "..\CSC8503CoreClasses\Constraint.h"

namespace NCL {
    namespace CSC8503     {
        enum SpringType
        {
            Compression = -1,
            Extension = 1
        };
        class GameObject;
        class SpringConstraint :
            public Constraint {
        public:
            SpringConstraint(SpringType t_type, GameObject* t_objA, GameObject* t_objB, float t_distance, float t_springConstant, float t_tolerance = 0.0f, float t_dampingFactor = 1.0f);
            ~SpringConstraint();

            void UpdateConstraint(float dt) override;
        protected:
            GameObject* m_objectA;
            GameObject* m_objectB;

            SpringType m_springType;

            float m_dampingFactor;
            float m_springConstant;
            float m_idealDistance;
            // spring does not begin to work until compressed this much past ideal distance, in order to avoid too much oscillation
            float m_distanceTolerance = 0.0f; 
        };
    }
}

