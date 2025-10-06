#pragma once
#include <iostream>
#include <vector>
#include "structs.h"

struct modelsLoader_struct {
	std::vector<float> vertices;
	unsigned int diffuseMap;
	AABB localAABB;
};

class ModelsLoader {

public:
	modelsLoader_struct load(std::string);

private:
	unsigned int loadTexture(const char*);
};