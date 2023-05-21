#pragma once

#include <glm.hpp>

namespace Celes::Math {

	bool CE_API DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

}
