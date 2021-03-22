#include "scene_importer.hpp"

#include "../util.hpp"
#include "texture_importer.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

bool processNode(scene* internalScene, aiNode* node, const aiScene* tempScene, entity* parent, int options);
bool processCamera(scene* internalScene, aiCamera* inCamera, const aiScene* tempScene, int options);
bool processLight(scene* internalScene, aiLight* inLight, const aiScene* tempScene, int options);
bool processMaterial(scene* internalScene, aiMaterial* inMaterial, const aiScene* tempScene, int options);
bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, texture::type textureType, int options);
bool processMesh(scene* internalScene, aiMesh* inMesh, const aiScene* tempScene, int options);

// TODO: Wrap in a struct to avoid globals
std::unordered_map<std::string, camera*> cameraNames;
std::unordered_map<std::string, light*> lightNames;

bool importScene(scene* internalScene, std::string path, int options) {
    Assimp::Importer importer;
    const aiScene* tempScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if(!tempScene || tempScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !tempScene->mRootNode) {
        fprintf(stderr, "[!] Model Loader: %s\n", importer.GetErrorString());
        return true;
    }
    internalScene->baseDirectory = path.substr(0, path.find_last_of('/') + 1);

    cameraNames.clear();
    lightNames.clear();

    bool returnValue = false;

    for(unsigned int i = 0; i < tempScene->mNumCameras; ++i) {
        returnValue = returnValue || processCamera(internalScene, tempScene->mCameras[i], tempScene, options);
    }

    for(unsigned int i = 0; i < tempScene->mNumLights; ++i) {
        returnValue = returnValue || processLight(internalScene, tempScene->mLights[i], tempScene, options);
    }

    for(unsigned int i = 0; i < tempScene->mNumMaterials; ++i) {
        returnValue = returnValue || processMaterial(internalScene, tempScene->mMaterials[i], tempScene, options);
    }

    returnValue = returnValue || processNode(internalScene, tempScene->mRootNode, tempScene, nullptr, options);

    return returnValue;
}

bool processNode(scene* internalScene, aiNode* node, const aiScene* tempScene, entity* parent, int options) {
    bool returnValue = false;

    // Create entity
    entity* e = new entity();
    e->parent = parent;

    if(parent == nullptr) {
        internalScene->rootNode = e;
    } else {
        parent->children.push_back(e);
    }

    // Set name
    e->name = std::string(node->mName.C_Str());

    // Copy transform
    e->setModelMatrix(convertMat4(node->mTransformation));

    // Handle camera & light nodes
    if(cameraNames.count(e->name) > 0) cameraNames.at(e->name)->parentEntity = (options & SCENE_IMPORT_CAMERA_PARENT) ? e->parent : e;
    if(lightNames.count(e->name) > 0) lightNames.at(e->name)->parentEntity = e;

    // Import meshes
    for(unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = tempScene->mMeshes[node->mMeshes[i]];
        returnValue = returnValue || processMesh(internalScene, mesh, tempScene, options);
        e->meshIndices.push_back(internalScene->meshes.size() - 1);
    }

    // Process children
    for(unsigned int i = 0; i < node->mNumChildren; ++i) {
        returnValue = returnValue || processNode(internalScene, node->mChildren[i], tempScene, e, options);
    }

    return returnValue;
}

bool processCamera(scene* internalScene, aiCamera* inCamera, const aiScene* tempScene, int options) {
    camera cam;

    cam.fov = glm::degrees(inCamera->mHorizontalFOV);
    cam.aspect = inCamera->mAspect;
    cam.clipNear = inCamera->mClipPlaneNear;
    cam.clipFar = inCamera->mClipPlaneFar;

    internalScene->cameras.push_back(cam);
    cameraNames.emplace(std::string(inCamera->mName.C_Str()), &internalScene->cameras[internalScene->cameras.size() - 1]);

    return false;
}

bool processLight(scene* internalScene, aiLight* inLight, const aiScene* tempScene, int options) {
    light l;

    // TODO: Different light types
    // TODO: Light strengths

    l.ambient = convertVec3to4(inLight->mColorAmbient);
    l.diffuse = convertVec3to4(inLight->mColorDiffuse);
    l.specular = convertVec3to4(inLight->mColorSpecular);

    internalScene->lights.push_back(l);
    lightNames.emplace(std::string(inLight->mName.C_Str()), &internalScene->lights[internalScene->lights.size() - 1]);

    return false;
}

bool processMaterial(scene* internalScene, aiMaterial* inMaterial, const aiScene* tempScene, int options) {
    material mat(internalScene->shaderProgram);

    mat.textures = std::vector<texture>();
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_DIFFUSE, texture::ALBEDO, options);
    //processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_DIFFUSE_ROUGHNESS, texture::ROUGHNESS, options);
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_NORMALS, texture::NORMAL, options);

    internalScene->materials.push_back(mat);
    return false;
}

bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, texture::type textureType, int options) {
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

bool processMesh(scene* internalScene, aiMesh* inMesh, const aiScene* tempScene, int options) {
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