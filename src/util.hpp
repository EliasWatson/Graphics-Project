#ifndef H_UTIL
#define H_UTIL

#include <assimp/scene.h>
#include <glm/glm.hpp>

#define COPY_VEC4(arr,vec) \
    arr[0] = vec[0]; \
    arr[1] = vec[1]; \
    arr[2] = vec[2]; \
    arr[3] = vec[3]

inline glm::vec3 convertVec3(aiVector3D vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::vec3 convertVec3(aiColor3D vec) {
    return glm::vec3(vec.r, vec.g, vec.b);
}

inline glm::vec4 convertVec3to4(aiVector3D vec) {
    return glm::vec4(vec.x, vec.y, vec.z, 1.0);
}

inline glm::vec4 convertVec3to4(aiColor3D vec) {
    return glm::vec4(vec.r, vec.g, vec.b, 1.0);
}

inline glm::mat4 convertMat4(aiMatrix4x4 mat) {
    return glm::mat4(
        glm::vec4(mat.a1, mat.a2, mat.a3, mat.a4),
        glm::vec4(mat.b1, mat.b2, mat.b3, mat.b4),
        glm::vec4(mat.c1, mat.c2, mat.c3, mat.c4),
        glm::vec4(mat.d1, mat.d2, mat.d3, mat.d4)
    );
}

#endif