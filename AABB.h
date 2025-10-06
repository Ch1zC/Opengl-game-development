#pragma once

#include "structs.h"
#include <glm/glm.hpp>

class AABB_class {
public:
	bool checkAABBCollision(const AABB&, const AABB&);
	AABB calculateWorldAABB(const AABB&, const glm::mat4&);
};