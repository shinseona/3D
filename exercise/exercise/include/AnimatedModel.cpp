#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <algorithm>
#include <iterator>
#include "AnimatedModel.h"
#include "Helper.h"
#include <glm/gtx/quaternion.hpp>
using std::cout;
using std::endl;
using std::pair;
using std::map;
using std::list;
using std::vector;
using std::stringstream;
using namespace std;

//constructors and operators
AnimatedModel::AnimatedModel(ModelProperty& const property) : IModel(property,false) {
	load(property.path);
}
AnimatedModel::AnimatedModel() : IModel(){
}
AnimatedModel::~AnimatedModel() {
	textureIndexes.clear();
	if (animation != NULL)
		delete animation;
	for (BoneMesh* mesh : meshes) {
		delete mesh;
	}
	meshes.clear();
}
AnimatedModel& AnimatedModel::operator=(AnimatedModel& model) {
	for (int a = 0; a < model.meshes.size(); ++a) {
		meshes.push_back(model.meshes.at(a));
	}
	textures = model.textures;
	loaded = model.loaded;
	property = model.property;
	width = model.width;
	height = model.height;
	depth = model.depth;
	x = model.x;
	y = model.y;
	z = model.z;
	matrix = model.matrix;
	return *this;
}
AnimatedModel* AnimatedModel::operator=(const AnimatedModel* model) {
	loaded = model->loaded;
	meshes = model->meshes;
	textures = model->textures;

	property = model->property;
	width = model->width;
	height = model->height;
	depth = model->depth;
	x = model->x;
	y = model->y;
	z = model->z;
	matrix = model->matrix;
	return this;
}

//----init
void AnimatedModel::load(string const &filePath) {
	Helper h;
	loaded = true;
	cout << "AnimatedModel: Loading model '" << filePath << "'..." << endl;
	Importer importer;
	unsigned int flags = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_LimitBoneWeights;
	const aiScene* scene = importer.ReadFile(filePath, flags);
	if (!scene) {
		cout << "AnimatedModel::Error::(" << filePath << "): Null scene after reading the file!" << endl;
		loaded = false;
	}
	else if (scene->mFlags&& AI_SCENE_FLAGS_INCOMPLETE) {
		cout << "AnimatedModel::Error::(" << filePath << "): Incomplete flags!" << endl;
		loaded = false;
	}
	else if (!scene->mRootNode) {
		cout << "AnimatedModel::Error::(" << filePath << "): Null root node!" << endl;
		loaded = false;
	}
	if (!loaded) return;

	//---init
	matrix = mat4(1.0f);
	//---------------------

	//---process skin
	meshes.resize(scene->mNumMeshes);
	processNode(scene->mRootNode, scene);
	cout << "AnimatedModel: Model '" << filePath << "' loaded." << endl;

	rotateDegree(180, 1);
	//---------------------
}
void AnimatedModel::processNode(aiNode* node, const aiScene* scene) {
	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);

	cout << "processNode::NodeName: " << nodeName << endl;

	//process all meshes
	unsigned int nrMeshes = node->mNumMeshes; 
	unsigned int* meshIndexes = node->mMeshes;
	for (int nrMesh = 0; nrMesh < nrMeshes; ++nrMesh) {
		int meshId = meshIndexes[nrMesh];
		aiMesh* mesh = scene->mMeshes[meshIndexes[nrMesh]];
		string meshName(mesh->mName.C_Str());

		BoneMesh* processedMesh = processMesh(mesh, scene);
		meshes.at(meshId) = processedMesh;
		vec3 params = meshes[meshId]->getVolume();
		vec3 coords = meshes[meshId]->getCoords();
		if (params.x > width) width = params.x;
		if (params.y > height) height = params.y;
		if (params.z > depth) depth = params.z;
		if (coords.x < x) x = coords.x;
		if (coords.y < y) y = coords.y;
		if (coords.z < z) z = coords.z;

		cout << "processNode::MeshName: " << meshName << endl;
	}

	for (int a = 0; a < node->mNumChildren; ++a) {
		processNode(node->mChildren[a], scene);
	}
}
BoneMesh* AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<BoneVertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	string meshName(mesh->mName.C_Str());
	int texIndex = 0;
	if (textureIndexes.find(meshName) == textureIndexes.end()) {
		textureIndexes.insert(make_pair(meshName, 1));
	}
	else {
		texIndex = textureIndexes.at(meshName);
		++textureIndexes.at(meshName);
	}

	//Read vertices
	unsigned int nrVertices = mesh->mNumVertices;
	for (int nrVertex = 0; nrVertex < nrVertices; ++nrVertex) {
		BoneVertex vertex;
		vec3 tmpVector;
		//position
		tmpVector.x = mesh->mVertices[nrVertex].x;
		tmpVector.y = mesh->mVertices[nrVertex].y;
		tmpVector.z = mesh->mVertices[nrVertex].z;
		vertex.position = tmpVector;
		//normals
		if (mesh->HasNormals()) {
			tmpVector.x = mesh->mNormals[nrVertex].x;
			tmpVector.y = mesh->mNormals[nrVertex].y;
			tmpVector.z = mesh->mNormals[nrVertex].z;
			vertex.normal = tmpVector;
		}
		// texture coordinates
		if (mesh->HasTextureCoords(texIndex)){
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[texIndex][nrVertex].x;
			vec.y = mesh->mTextureCoords[texIndex][nrVertex].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	//Read faces
	unsigned int nrFaces = mesh->mNumFaces;
	for (int a = 0; a < nrFaces; ++a) {
		aiFace face = mesh->mFaces[a];
		for (int b = 0; b < face.mNumIndices; ++b) {
			indices.push_back(face.mIndices[b]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString materialName;
	aiGetMaterialString(material, AI_MATKEY_NAME, &materialName);
	string matName(materialName.C_Str());
	cout << "processMesh::MaterialName: " << matName << endl;

	if (material) {
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return new BoneMesh(vertices, indices, textures);
}


//----controls
void AnimatedModel::update(float elapsedTime){
	if(needUpdate)
	if (animation != NULL && animation->isLoaded()) {
		bool animationFinished = animation->update(elapsedTime, loop);
		if (animationFinished) {
			needUpdate = false;
		}
	}
}
void AnimatedModel::draw(Shader animShader) {
	if (animation != NULL) {
		map<int, mat4>* matrices = animation->getMatrices();
		for (pair<int, mat4> mat : *matrices) {
			unsigned int boneIdx = mat.first;
			mat4 boneTransform = mat.second;
			stringstream ss;
			ss << "boneMatrices[" << boneIdx << "]";
			GLuint bonesUniformIndex = glGetUniformLocation(animShader.getId(), ss.str().c_str());
			animShader.setMat4(bonesUniformIndex, boneTransform);
		}
	}

	for (BoneMesh* m : meshes) {
		m->draw(animShader);
	}
}
