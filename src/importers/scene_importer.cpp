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
bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, std::string typeName);
bool processMesh(scene* internalScene, aiMesh* inMesh, const aiScene* tempScene);

bool importScene(scene* internalScene, std::string path) {
    Assimp::Importer importer;
    const aiScene* tempScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!tempScene || tempScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !tempScene->mRootNode) {
        fprintf(stderr, "[!] Model Loader: %s\n", importer.GetErrorString());
        return true;
    }
    internalScene->baseDirectory = path.substr(0, path.find_last_of('/'));

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

    // cam.pos = convertVec3(inCamera->mPosition);
    cam.pos = glm::vec3(0.0, 1.0, -3.0); // TODO: Change
    cam.dir = glm::lookAt(
        cam.pos,
        convertVec3(inCamera->mLookAt),
        convertVec3(inCamera->mUp)
    );
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
    material mat;

    mat.textures = std::vector<texture>();
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_DIFFUSE, "albedo");
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_SPECULAR, "specular");
    processTextures(&mat.textures, internalScene->baseDirectory, inMaterial, aiTextureType_NORMALS, "normal");

    internalScene->materials.push_back(mat);
    return false;
}

bool processTextures(std::vector<texture>* textures, std::string baseDirectory, aiMaterial* inMaterial, aiTextureType type, std::string typeName) {
    bool returnValue = false;

    for(unsigned int i = 0; i < inMaterial->GetTextureCount(type); ++i) {
        aiString str;
        inMaterial->GetTexture(type, i, &str);

        texture tex;
        returnValue = returnValue || importTexture(&tex, baseDirectory + std::string(str.C_Str()), typeName);

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
    glGenBuffers(4, vbo);

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

/*
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <functional>

typedef std::tuple<int, int, int> triplet;

struct triplet_hash : public std::unary_function<triplet, std::size_t> {
    std::size_t operator()(const triplet& k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
};

bool importModel(mesh* model, const char* path) {
    // Setup loader config
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.mtl_search_path = "./";

    // Read data from file
    tinyobj::ObjReader reader;
    if(!reader.ParseFromFile(path, readerConfig)) {
        if(!reader.Error().empty()) {
            std::cerr << "[!] Model Loader: " << reader.Error();
        }
        return true;
    }

    // Check for warnings
    if(!reader.Warning().empty()) {
        std::cerr << "[*] Model Loader: " << reader.Warning();
    }

    // Get data
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    //auto& materials = reader.GetMaterials();

    // Load into vectors and handle vertices with the same index but different attributes
    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<int> indices;
    std::unordered_map<const std::tuple<int, int, int>, int, triplet_hash> indexMap;
    int currentIndex = 0;

    int numNonTriangle = 0;
    glm::vec3 boundMin( INFINITY,  INFINITY,  INFINITY);
    glm::vec3 boundMax(-INFINITY, -INFINITY, -INFINITY);

    for(size_t s = 0; s < shapes.size(); ++s) {
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // TODO: Triangulate quads and ngons
            if(fv != 3) {
                ++numNonTriangle;
                continue;
            }

            for(size_t v = 0; v < fv; ++v) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                triplet key = std::make_tuple(idx.vertex_index, idx.texcoord_index, idx.normal_index);

                auto iter = indexMap.find(key);
                if(iter == indexMap.end()) {
                    for(int i = 0; i < 3; ++i) {
                        float n = attrib.vertices[3*idx.vertex_index+i];

                        if(n < boundMin[i]) boundMin[i] = n;
                        if(n > boundMax[i]) boundMax[i] = n;

                        positions.push_back(n);
                    }
                    
                    for(int i = 0; i < 2; ++i) {
                        uvs.push_back(attrib.texcoords[2*idx.texcoord_index+i]);
                    }
                    
                    for(int i = 0; i < 3; ++i) {
                        normals.push_back(attrib.normals[3*idx.normal_index+i]);
                    }
                    
                    indices.push_back(currentIndex);
                    indexMap.emplace(key, currentIndex);
                    currentIndex++;
                } else {
                    indices.push_back(iter->second);
                }
            }

            index_offset += fv;
        }
    }

    if(numNonTriangle > 0) {
        std::cerr
        << "[*] Mesh Loader: "
        << numNonTriangle
        << " non-triangle faces in '"
        << path
        << "'"
        << std::endl;
    }

    // Load data
    GLuint vao[1];
    GLuint vbo[4];

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(4, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), &positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Construct mesh instance
    *model = mesh();
    model->vbo = std::vector<GLuint>(vbo, vbo + 3);
    model->vboIndices = vbo[3];
    model->vertexCount = indices.size();
    model->boundMin = boundMin;
    model->boundMax = boundMax;

    return true;
}
*/