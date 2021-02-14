#include "model_importer.hpp"

#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <stdio.h>
#include <stdlib.h>

#define ERROR { success = false; break; }

typedef std::tuple<int, int, int> triplet;

struct triplet_hash : public std::unary_function<triplet, std::size_t> {
    std::size_t operator()(const triplet& k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
};

void consumeLine(FILE* file) {
    int c;
    while((c = fgetc(file)) != EOF) {
        if(c == '\n') break;
    }
}

bool importModel(const char* path, mesh* model) {
    bool success = true;

    std::vector<float> position;
    std::vector<float> uv;
    std::vector<float> normal;

    FILE* file = fopen(path, "r");

    // Parse v, vt, & vn
    char type[3];
    float x, y, z;
    while(success) {
        if(fscanf(file, "%2s ", type) <= 0) ERROR;
        if(type[0] == 's' || type[0] == 'f') break;

        // Skip lines that we don't support
        if(type[0] != 'v') {
            consumeLine(file);
            continue;
        }

        switch(type[1]) {
            case 0:
                if(fscanf(file, "%f %f %f\n", &x, &y, &z) <= 0) ERROR;
                position.push_back(x);
                position.push_back(y);
                position.push_back(z);
                break;

            case 't':
                if(fscanf(file, "%f %f\n", &x, &y) <= 0) ERROR;
                uv.push_back(x);
                uv.push_back(y);
                break;

            case 'n':
                if(fscanf(file, "%f %f %f\n", &x, &y, &z) <= 0) ERROR;
                normal.push_back(x);
                normal.push_back(y);
                normal.push_back(z);
                break;
        }
    }

    std::unordered_map<const std::tuple<int, int, int>, int, triplet_hash> vertex_map;
    std::vector<float> final_position;
    std::vector<float> final_uv;
    std::vector<float> final_normal;
    std::vector<int> final_index;

    // Parse f
    int currentIndex = 0;
    int p[3], u[3], n[3];
    while(success) {
        if(type[0] == 'f') {
            if(fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &p[0], &u[0], &n[0], &p[1], &u[1], &n[1], &p[2], &u[2], &n[2]) <= 0) ERROR;
            for(int i = 0; i < 3; ++i) {
                std::tuple<int, int, int> index = std::make_tuple(p[i], u[i], n[i]);
                auto iter = vertex_map.find(index);

                if(iter == vertex_map.end()) {
                    for(int j = 0; j < 3; ++j) final_position.push_back(position[((p[i]-1) * 3) + j]);
                    for(int j = 0; j < 2; ++j) final_uv.push_back(uv[((u[i]-1) * 2) + j]);
                    for(int j = 0; j < 3; ++j) final_normal.push_back(normal[((n[i]-1) * 3) + j]);
                    vertex_map.emplace(index, currentIndex);
                    final_index.push_back(currentIndex);
                    currentIndex++;
                } else {
                    final_index.push_back(iter->second);
                }
            }
        } else {
            // Skip lines that we don't support
            consumeLine(file);
        }

        if(fscanf(file, "%2s ", type) <= 0) break;
    }

    fclose(file);

    if(!success) return false;

    // Load data
    GLuint vao[1];
    GLuint vbo[4];

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(4, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * final_position.size(), &final_position[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * final_uv.size(), &final_uv[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * final_normal.size(), &final_normal[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * final_index.size(), &final_index[0], GL_STATIC_DRAW);

    // Construct mesh instance
    *model = mesh();
    model->vbo = std::vector<GLuint>(vbo, vbo + 3);
    model->vboIndices = vbo[3];
    model->vertexCount = final_index.size();

    return true;
}