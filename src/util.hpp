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
        mat[0][1], mat[0][2], mat[0][3], mat[0][4],
        mat[1][1], mat[1][2], mat[1][3], mat[1][4],
        mat[2][1], mat[2][2], mat[2][3], mat[2][4],
        mat[3][1], mat[3][2], mat[3][3], mat[3][4]
    );
}

#endif