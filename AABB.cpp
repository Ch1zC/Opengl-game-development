#include "AABB.h"
#include "structs.h"

#include <glm/glm.hpp>
#include <vector>


bool AABB_class::checkAABBCollision(const AABB& a, const AABB& b) {
    // ���������϶������ص���������ײ
    // return ture  = �ص���
    // return false = û�ص�

    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

AABB AABB_class::calculateWorldAABB(const AABB& localAABB, const glm::mat4& modelMatrix) {
    // ��������AABB�����С������


    // ��ȡ����AABB��8���ǵ�
    std::vector<glm::vec3> corners = {
        // �����ĸ���
        glm::vec3(localAABB.min.x, localAABB.min.y, localAABB.min.z), // 0: ��-��-�½�
        glm::vec3(localAABB.max.x, localAABB.min.y, localAABB.min.z), // 1: ��-��-�½�
        glm::vec3(localAABB.max.x, localAABB.max.y, localAABB.min.z), // 2: ��-��-�Ͻ�
        glm::vec3(localAABB.min.x, localAABB.max.y, localAABB.min.z), // 3: ��-��-�Ͻ�

        // ǰ���ĸ���
        glm::vec3(localAABB.min.x, localAABB.min.y, localAABB.max.z), // 4: ǰ-��-�½�
        glm::vec3(localAABB.max.x, localAABB.min.y, localAABB.max.z), // 5: ǰ-��-�½�
        glm::vec3(localAABB.max.x, localAABB.max.y, localAABB.max.z), // 6: ǰ-��-�Ͻ�
        glm::vec3(localAABB.min.x, localAABB.max.y, localAABB.max.z)  // 7: ǰ-��-�Ͻ�
    };

    // ʹ��ģ�;���任��һ���ǵ㣬���Դ���Ϊ��ʼ�����������С��
    glm::vec3 worldMin = modelMatrix * glm::vec4(corners[0], 1.0f);
    glm::vec3 worldMax = worldMin;

    // ����ʣ����߸��ǵ�
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