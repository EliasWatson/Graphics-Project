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
    glm::vec4 scale;

    std::vector<int> meshIndices;
    
    entity* parent;
    std::vector<entity*> children;

    entity() { }

    void setModelMatrix(glm::mat4 inMat);
    glm::mat4 applyLocalTransform(glm::mat4 inMat);
    void updateWorldPosition(glm::mat4 inMat);

    void renderShadowmap(scene* s, glm::mat4 parentMat);
    void render(scene* s, camera* cam, glm::mat4 parentMat);
    void renderGUI();

    void killChild(entity* child);
    void destroy();

private:
    glm::mat4 modelMatrix;
};

#endif