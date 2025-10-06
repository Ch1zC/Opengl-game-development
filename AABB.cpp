#include "AABB.h"
#include "structs.h"

#include <glm/glm.hpp>
#include <vector>


bool AABB_class::checkAABBCollision(const AABB& a, const AABB& b) {
    // 在所有轴上都存在重叠，才算碰撞
    // return ture  = 重叠了
    // return false = 没重叠

    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

AABB AABB_class::calculateWorldAABB(const AABB& localAABB, const glm::mat4& modelMatrix) {
    // 计算物体AABB最大、最小点坐标


    // 获取本地AABB的8个角点
    std::vector<glm::vec3> corners = {
        // 后面四个角
        glm::vec3(localAABB.min.x, localAABB.min.y, localAABB.min.z), // 0: 后-左-下角
        glm::vec3(localAABB.max.x, localAABB.min.y, localAABB.min.z), // 1: 后-右-下角
        glm::vec3(localAABB.max.x, localAABB.max.y, localAABB.min.z), // 2: 后-右-上角
        glm::vec3(localAABB.min.x, localAABB.max.y, localAABB.min.z), // 3: 后-左-上角

        // 前面四个角
        glm::vec3(localAABB.min.x, localAABB.min.y, localAABB.max.z), // 4: 前-左-下角
        glm::vec3(localAABB.max.x, localAABB.min.y, localAABB.max.z), // 5: 前-右-下角
        glm::vec3(localAABB.max.x, localAABB.max.y, localAABB.max.z), // 6: 前-右-上角
        glm::vec3(localAABB.min.x, localAABB.max.y, localAABB.max.z)  // 7: 前-左-上角
    };

    // 使用模型矩阵变换第一个角点，并以此作为初始的世界最大最小点
    glm::vec3 worldMin = modelMatrix * glm::vec4(corners[0], 1.0f);
    glm::vec3 worldMax = worldMin;

    // 遍历剩余的七个角点
    for (size_t i = 1; i < corners.size(); ++i) {
        glm::vec3 worldCorner = modelMatrix * glm::vec4(corners[i], 1.0f);
        worldMin.x = std::min(worldMin.x, worldCorner.x);
        worldMin.y = std::min(worldMin.y, worldCorner.y);
        worldMin.z = std::min(worldMin.z, worldCorner.z);
        worldMax.x = std::max(worldMax.x, worldCorner.x);
        worldMax.y = std::max(worldMax.y, worldCorner.y);
        worldMax.z = std::max(worldMax.z, worldCorner.z);
    }

    return { worldMin, worldMax };
}