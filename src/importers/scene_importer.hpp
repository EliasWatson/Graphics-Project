#ifndef H_SCENE_IMPORTER
#define H_SCENE_IMPORTER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

#include "../scene.hpp"

bool importScene(scene* internalScene, std::string path);

#endif