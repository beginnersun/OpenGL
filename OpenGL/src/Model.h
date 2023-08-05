#pragma once
#include <glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"

#include "assimp/scene.h"           // Output data structure

using namespace std;
/*
 * ��������
*/
struct Vertex
{
	glm::vec3 Position; //λ��
	glm::vec3 Normal; //����
	glm::vec2 TexCoord; //��������
};

/*
 ��������
*/
struct Texture
{
	unsigned int id; //����id
	string type;     //��������
	string path;
};

class Mesh
{
public:
	/*��������*/
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

	//ģ������
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	bool gammaCorrection;

	Model(string const &path, bool gamma = false) :gammaCorrection(gamma) {
		loadModel(path);
	}

	void Draw(Shader shader);
private:

	//����
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

