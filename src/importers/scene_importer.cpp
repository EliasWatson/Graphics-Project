#include "scene_importer.hpp"

#include "../util.hpp"
#include "texture_importer.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <vector>

bool processNode(scene* internalScene, aiNode* node, const aiScene* tempScene, entity* parent);
bool processCamera(scene* internalScene, aiCamera* inCamera, const aiScene* tempScene);
bool processLight(scene* internalScene, aiLight* inLight, const aiScene* tempScene);
bool processMaterial(scene* internalScene, aiMaterial* inMaterial, const aiScene* tempScene);
bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, texture::type textureType);
bool processMesh(scene* internalScene, aiMesh* inMesh, const aiScene* tempScene);

bool importScene(scene* internalScene, std::string path) {
    Assimp::Importer importer;
    const aiScene* tempScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!tempScene || tempScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !tempScene->mRootNode) {
        fprintf(stderr, "[!] Model Loader: %s\n", importer.GetErrorString());
        return true;
    }
    internalScene->baseDirectory = path.substr(0, path.find_last_of('/') + 1);

    bool returnValue = false;

    for(unsigned int i = 0; i < tempScene->mNumCameras; ++i) {
        returnValue = returnValue || processCamera(internalScene, tempScene->mCameras[i], tempScene);
    }

    for(unsigned int i = 0; i < tempScene->mNumLights; ++i) {
        returnValue = returnValue || processLight(internalScene, tempScene->mLights[i], tempScene);
    }

    for(unsigned int i = 0; i < tempScene->mNumMaterials; ++i) {
        returnValue = returnValue || processMaterial(internalScene, tempScene->mMaterials[i], tempScene);
    }

    returnValue = returnValue || processNode(internalScene, tempScene->mRootNode, tempScene, nullptr);

    return returnValue;
}

bool processNode(scene* internalScene, aiNode* node, const aiScene* tempScene, entity* parent) {
    bool returnValue = false;

    // Create entity
    entity* e = nullptr;
    if(parent == nullptr) {
        internalScene->rootNode = entity();
        e = &internalScene->rootNode;
    } else {
        parent->children.push_back(entity());
        e = &parent->children[parent->children.size() - 1];
    }

    // Copy transform
    e->modelMatrix = convertMat4(node->mTransformation);

    // Handle camera node
    if(strcmp(node->mName.C_Str(), "Camera") == 0) {
        glm::mat4 t_mat = e->modelMatrix;
        glm::vec3 pos = t_mat[3];
        internalScene->cameras[internalScene->mainCamera].pos = pos;
    } else if (strcmp(node->mName.C_Str(), "Point") == 0) {
        internalScene->lights[0].pos = e->modelMatrix[3];
    }

    // Import meshes
    for(unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = tempScene->mMeshes[node->mMeshes[i]];
        returnValue = returnValue || processMesh(internalScene, mesh, tempScene);
        e->meshIndices.push_back(internalScene->meshes.size() - 1);
    }

    // Process children
    for(unsigned int i = 0; i < node->mNumChildren; ++i) {
        returnValue = returnValue || processNode(internalScene, node->mChildren[i], tempScene, e);
    }

    return returnValue;
}

bool processCamera(scene* internalScene, aiCamera* inCamera, const aiScene* tempScene) {
    camera cam;

    cam.fov = glm::degrees(inCamera->mHorizontalFOV);
    cam.aspect = inCamera->mAspect;
    cam.clipNear = inCamera->mClipPlaneNear;
    cam.clipFar = inCamera->mClipPlaneFar;

    internalScene->cameras.push_back(cam);
    return false;
}

bool processLight(scene* internalScene, aiLight* inLight, const aiScene* tempScene) {
    light l;

    // TODO: Different light types
    // TODO: Light strengths

    l.pos = convertVec3to4(inLight->mPosition);
    l.ambient = convertVec3to4(inLight->mColorAmbient);
    l.diffuse = convertVec3to4(inLight->mColorDiffuse);
    l.specular = convertVec3to4(inLight->mColorSpecular);

    internalScene->lights.push_back(l);
    return false;
}

bool processMaterial(scene* internalScene, aiMaterial* inMaterial, const aiScene* tempScene) {
    material mat(internalScene->shaderProgram);

    mat.textures = std::vector<texture>();
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_DIFFUSE, texture::ALBEDO);
    // processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_SPECULAR, "specular");
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_NORMALS, texture::NORMAL);

    internalScene->materials.push_back(mat);
    return false;
}

bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, texture::type textureType) {
    bool returnValue = false;

    for(unsigned int i = 0; i < inMaterial->GetTextureCount(type); ++i) {
        aiString str;
        inMaterial->GetTexture(type, i, &str);

        texture tex;
        returnValue = returnValue || importTexture(&tex, baseDirectory + std::string(str.C_Str()), textureType);

        textures->push_back(tex);
    }

    return returnValue;
}

bool processMesh(scene* internalScene, aiMesh* inMesh, const aiScene* tempScene) {
    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<int> indices;

    // Process vertices
    for(unsigned int i = 0; i < inMesh->mNumVertices; ++i) {
        positions.push_back(inMesh->mVertices[i].x);
        positions.push_back(inMesh->mVertices[i].y);
        positions.push_back(inMesh->mVertices[i].z);

        normals.push_back(inMesh->mNormals[i].x);
        normals.push_back(inMesh->mNormals[i].y);
        normals.push_back(inMesh->mNormals[i].z);

        tangents.push_back(inMesh->mTangents[i].x);
        tangents.push_back(inMesh->mTangents[i].y);
        tangents.push_back(inMesh->mTangents[i].z);

        if(inMesh->mTextureCoords[0]) {
            uvs.push_back(inMesh->mTextureCoords[0][i].x);
            uvs.push_back(inMesh->mTextureCoords[0][i].y);
        } else {
            uvs.push_back(0.0f);
            uvs.push_back(0.0f);
        }
    }

    // Process indices
    for(unsigned int i = 0; i < inMesh->mNumFaces; ++i) {
        aiFace face = inMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Load into VBOs
    GLuint vao[1];
    GLuint vbo[5];

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(5, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), &positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tangents.size(), &tangents[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Construct mesh instance
    mesh m;
    m.materialIndex = inMesh->mMaterialIndex;
    m.vbo = std::vector<GLuint>(vbo, vbo + 4);
    m.ebo = vbo[4];
    m.vertexCount = (GLsizei) indices.size();

    internalScene->meshes.push_back(m);
    return false;
}