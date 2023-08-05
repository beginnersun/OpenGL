#pragma once
#include <glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"

#include "assimp/scene.h"           // Output data structure

using namespace std;
/*
 * 顶点数据
*/
struct Vertex
{
	glm::vec3 Position; //位置
	glm::vec3 Normal; //向量
	glm::vec2 TexCoord; //纹理坐标
};

/*
 纹理数据
*/
struct Texture
{
	unsigned int id; //纹理id
	string type;     //纹理类型
	string path;
};

class Mesh
{
public:
	/*网格数据*/
	vector<Vertex> vertices;
	vector<unsigned int>indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};


class Model {
public:

	//模型数据
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	bool gammaCorrection;

	Model(string const &path, bool gamma = false) :gammaCorrection(gamma) {
		loadModel(path);
	}

	void Draw(Shader shader);
private:

	//函数
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

