#ifndef H_SCENE_IMPORTER
#define H_SCENE_IMPORTER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

#include "../scene.hpp"

#define SCENE_IMPORT_CAMERA_PARENT (1)

bool importScene(scene* internalScene, std::string path, int options);

#endif