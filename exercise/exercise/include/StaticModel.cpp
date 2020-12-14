#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include "StaticModel.h"
#include "IModel.h"
//#include "AnimatedModel.h"
#include "Helper.h"
using std::map;
using namespace Assimp;
using namespace glm;

/*  Functions   */
// constructor, expects a filepath to a 3D model.
StaticModel::StaticModel() : IModel() {}
StaticModel::StaticModel(ModelProperty modelProperty, bool gamma) : IModel(modelProperty,gamma){
	load(modelProperty.path);
}
StaticModel::~StaticModel() {
	cout << "StaticModel::~StaticModel1\n";
	textureIndexes.clear();
	meshes.clear();
	cout << "StaticModel::~StaticModel2\n";
}

/*Getters----------------*/
vector<Vertex> StaticModel::getVertices() {
	vector<Vertex> vertices;
	for (Mesh*& m : meshes) {
		for (Vertex v : m->getVertices()) {
			vertices.insert(vertices.end(), v);
		}
	}
	return vertices;
}
vector<unsigned int> StaticModel::getIndices() {
	vector<unsigned int> indices;
	for (Mesh*& m : meshes) {
		for (unsigned i : m->getIndices()) {
			indices.insert(indices.end(), i);
		}
	}
	return indices;
}
/*------------------------*/

/*Interface--------------------*/
void StaticModel::draw(Shader*& shader){
	if (meshes.size() == 0)
		cout << "NNULL meshes" << endl;
	int index = 0;
	for (Mesh*& m : meshes) {
		//shader->setMat4("model", meshTransformations.at(index++));
		m->draw(shader);
	}
}
/*----------------------------*/

/*Initialization-------------*/
void StaticModel::load(string const &path)
{
	cout << "Model: Starting loading '" << path << "' model...\n";

	unsigned int processFlags = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace;


	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, processFlags);
	const char* str = importer.GetErrorString();
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "Model::Error: " << importer.GetErrorString() << endl;
		return;
	}
	else cout << "Model: Model readed from file.\n";

	//init matrices
	matrix = mat4(1.0f, 0.0, 0.0, 0.0,
		0.0f, 1.0, 0.0, 0.0,
		0.0f, 0.0, 1.0, 0.0,
		1.0f, 1.0, 1.0, 1.0);
	rotationMatrix = mat4(1.0f);
	scalingVector = vec3(1.0f, 1.0f, 1.0f);

	//process all nodes 
	processNode(scene->mRootNode, scene);

	//apply scaling
	if (matrix[0][0] != 0)
		width = width*matrix[0][0];
	if (matrix[1][1] != 0)
		height = height*matrix[1][1];
	if (matrix[2][2] != 0)
		depth = depth*matrix[2][2];

	//applying position
	x = matrix[3][0];
	y = matrix[3][1] = matrix[3][1] + height / 2.0f;//above terrain
	z = matrix[3][2];

	//rotationMatrix = glm::rotate(rotationMatrix, 45.0f, vec3(1.0f, 0.0f, 0.0f));
	cout << "Model: Model '" << path << "' loaded.\n";
}
void StaticModel::processNode(aiNode *node, const aiScene *scene) {
	Helper h;
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);

	for (int a = 0; a < 4; ++a) {
		for (int b = 0; b < 4; ++b) {
			if (nodeTransformation[a][b] != 0)
				matrix[a][b] *= nodeTransformation[a][b];
		}
	}

	unsigned int numMeshes = node->mNumMeshes;
	for (unsigned int nrMesh = 0; nrMesh < numMeshes; nrMesh++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[nrMesh]];
		Mesh* processedMesh = processMesh(mesh, scene);

		vec3 params = processedMesh->getVolume();
		vec3 coords = processedMesh->getCoords();
		if (params.x > width) width = params.x;
		if (params.y > height) height = params.y;
		if (params.z > depth) depth = params.z;
		if (coords.x < x) x = coords.x;
		if (coords.y < y) y = coords.y;
		if (coords.z < z) z = coords.z;

		meshes.push_back(processedMesh);
	}

	for (unsigned int nrChild = 0; nrChild < node->mNumChildren; nrChild++) {
		processNode(node->mChildren[nrChild], scene);
	}

}
Mesh* StaticModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	if (mesh->mVertices == NULL) cout << "Model: NULL mesh vertices!\n";
	if (mesh->mNormals == NULL) cout << "Model: NULL mesh normals!\n";
	if (mesh->mTextureCoords == NULL) cout << "Model: NULL mesh textureCoords!\n";
	if (mesh->mTangents == NULL) cout << "Model: NULL mesh Tangents!\n";
	if (mesh->mBitangents == NULL) cout << "Model: NULL mesh BiTangents!\n";

	//calculate texture index
	string meshName(mesh->mName.C_Str());
	int texIndex = 0;
	if (textureIndexes.find(meshName) == textureIndexes.end()) {
		textureIndexes.insert(make_pair(meshName, 1));
	}
	else {
		texIndex = textureIndexes.at(meshName);
		textureIndexes.at(meshName) += 1;
	}

	// Walk through each of the mesh's vertices
	for (unsigned int nrVertex = 0; nrVertex < mesh->mNumVertices; nrVertex++) {
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[nrVertex].x;
		vector.y = mesh->mVertices[nrVertex].y;
		vector.z = mesh->mVertices[nrVertex].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[nrVertex].x;
			vector.y = mesh->mNormals[nrVertex].y;
			vector.z = mesh->mNormals[nrVertex].z;
			vertex.Normal = vector;
		}
		else
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		// texture coordinates
		if (mesh->HasTextureCoords(texIndex)) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[texIndex][nrVertex].x;
			vec.y = mesh->mTextureCoords[texIndex][nrVertex].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		if (mesh->mTangents) {
			vector.x = mesh->mTangents[nrVertex].x;
			vector.y = mesh->mTangents[nrVertex].y;
			vector.z = mesh->mTangents[nrVertex].z;
			vertex.Tangent = vector;
		}

		// bitangent
		if (mesh->mBitangents) {
			vector.x = mesh->mBitangents[nrVertex].x;
			vector.y = mesh->mBitangents[nrVertex].y;
			vector.z = mesh->mBitangents[nrVertex].z;
			vertex.Bitangent = vector;
		}

		vertices.push_back(vertex);
	}

	for (unsigned int nrFace = 0; nrFace < mesh->mNumFaces; nrFace++) {
		aiFace face = mesh->mFaces[nrFace];
		for (unsigned int nrIndex = 0; nrIndex < face.mNumIndices; nrIndex++)
			indices.push_back(face.mIndices[nrIndex]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material == NULL) cout << "Model: NULL material!\n";
	else {
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}


	return new Mesh(vertices, indices, textures);
}
/*---------------------------*/

