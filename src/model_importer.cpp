#include "model_importer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
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

bool importModel(const char* path, mesh* model) {
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