#ifndef H_ENTITY
#define H_ENTITY

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct scene;
struct camera;

struct entity {
    std::string name;

    glm::vec4 pos, worldPos;
    glm::mat4 modelMatrix;

    std::vector<int> meshIndices;
    
    entity* parent;
    std::vector<entity*> children;

    entity() { }

    glm::mat4 applyLocalTransform(glm::mat4 inMat);
    void updateWorldPosition(glm::mat4 inMat);

    void render(scene* s, camera* cam, glm::mat4 parentMat);
    void renderGUI();

    void killChild(entity* child);
    void destroy();
};

#endif