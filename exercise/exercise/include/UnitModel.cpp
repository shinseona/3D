#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <algorithm>
#include <iterator>
#include "UnitModel.h"
#include "Helper.h"
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "KeyFrame.h"
#include <exception>

using std::cout;
using std::endl;
using std::pair;
using std::map;
using std::list;
using std::vector;
using std::stringstream;
using namespace std;

//constructors and operators
UnitModel::UnitModel(const ModelProperty prop) : IModel(prop, false) {
	cout << "UnitModel::UnitModel: Nr. meshes1: " << meshes.size() << endl;
	load(property.path);
	cout << "UnitModel::UnitModel: Nr. meshes2: " << meshes.size() << endl;
}
UnitModel::UnitModel() : IModel() {}
UnitModel::UnitModel(const UnitModel& m) {
	std::cout << "UnitModel::UnitModel(m)1\n";
	this->modelParts.insert(m.modelParts.begin(), m.modelParts.end());
	for(pair<string,Animation*> p: m.animations)
		this->animations.insert(p);
	currAnimation = m.currAnimation;
	loop = m.loop;
	needUpdate = m.needUpdate;
	this->meshes.insert(this->meshes.end(), m.meshes.begin(), m.meshes.end());
	this->textures.insert(this->textures.end(), m.textures.begin(), m.textures.end());
	weaponMesh = m.weaponMesh;
	
	std::cout << "UnitModel::UnitModel(m)2\n";
}
UnitModel::~UnitModel() {
	std::cout << "UnitModel::~UnitModel1\n";
	std::cout << "UnitModel::~UnitModel2\n";
	cout << "Nr. meshes: " << meshes.size() << endl;
	for (BoneMesh* mesh : meshes) {
		cout << "Nr. vertices: " << mesh->getVertices().size() << endl;
		if (mesh != NULL)
			delete mesh;
		cout << "Deleted" << endl;
	}

	std::cout << "UnitModel::~UnitModel3\n";
	meshes.clear();
	if (weaponMesh != NULL)
		delete weaponMesh;
	std::cout << "UnitModel::~UnitModel4\n";
	for (pair<string, Animation*> anim : animations) {
		delete anim.second;
	}
	animations.clear();
	std::cout << "UnitModel::~UnitModel5\n";
}
UnitModel& UnitModel::operator=(const UnitModel& model) {
	cout << " UnitModel::operator\n";
	cout << "-Model.Nr.meshes: " << model.meshes.size() << endl;
	//	cout << "-Model.Nr.vertices: " << model.meshes.at(0)->getVertices().size() << endl;
	for (int a = 0; a < model.meshes.size(); ++a) {
		meshes.push_back(model.meshes.at(a));
	}
	cout << "-This.Nr.meshes: " << meshes.size() << endl;
	//	cout << "-This.Nr.vertices: " << meshes.at(0)->getVertices().size() << endl;
	this->textures.insert(this->textures.end(), model.textures.begin(), model.textures.end());
	loaded = model.loaded;

	property = model.property;
	width = model.width;
	height = model.height;
	depth = model.depth;
	x = model.x;
	y = model.y;
	z = model.z;
	matrix = model.matrix;
	currAnimation = model.currAnimation;
	animations = model.animations;
	loop = model.loop;
	meshes = model.meshes;
	this->modelParts.insert(model.modelParts.begin(), model.modelParts.end());
	needUpdate = model.needUpdate;
	rotationMatrix = model.rotationMatrix;
	selected = model.selected;
	cout << " UnitModel::operator\n";
	return *this;
}
UnitModel* UnitModel::operator=(const UnitModel* model) {
	cout << " UnitModel::operator=1\n";
	loaded = model->loaded;
	meshes = model->meshes;
	textures = model->textures;
	this->modelParts.insert(model->modelParts.begin(), model->modelParts.end());

	property = model->property;
	width = model->width;
	height = model->height;
	depth = model->depth;
	x = model->x;
	y = model->y;
	z = model->z;
	matrix = model->matrix;
	cout << "UnitModel::operator=2\n";
	return this;
}

//----init
void UnitModel::load(string const &filePath) {
	Helper h;
	loaded = true;
	cout << "UnitModel: Loading model '" << filePath << "'..." << endl;
	Importer importer;
	unsigned int flags = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_LimitBoneWeights;
	const aiScene* scene = importer.ReadFile(filePath, flags);
	if (!scene) {
		cout << "UnitModel::Error::(" << filePath << "): Null scene after reading the file!" << endl;
		loaded = false;
	}
	else if (scene->mFlags&& AI_SCENE_FLAGS_INCOMPLETE) {
		cout << "UnitModel::Error::(" << filePath << "): Incomplete flags!" << endl;
		loaded = false;
	}
	else if (!scene->mRootNode) {
		cout << "UnitModel::Error::(" << filePath << "): Null root node!" << endl;
		loaded = false;
	}
	if (!loaded) return;

	//---init
	matrix = mat4(1.0f);
	//---------------------

	//---process skin
	meshes.resize(scene->mNumMeshes);
	cout << "Scene.nrMeshes: " << meshes.size() << endl;
	processNode(scene->mRootNode, scene);
	if (weaponMesh != NULL)
		meshes.resize(scene->mNumMeshes - 1);
	cout << "UnitModel: Model '" << filePath << "' loaded." << endl;

	rotateDegree(180, 1);
	weaponModelMatrix = glm::rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	//---------------------
}
void UnitModel::draw(Shader& animShader) {
	if (currAnimation != NULL) {
		map<int, mat4>* matrices = currAnimation->getMatrices();
		for (pair<int, mat4> mat : *matrices) {
			unsigned int boneIdx = mat.first;
			mat4 boneTransform = mat.second;
			stringstream ss;
			ss << "boneMatrices[" << boneIdx << "]";
			GLuint bonesUniformIndex = glGetUniformLocation(animShader.getId(), ss.str().c_str());
			animShader.setMat4(bonesUniformIndex, boneTransform);
		}
	}
	else cout << "Null animations!\n";

	for (BoneMesh*& m : meshes) {
		if (m != NULL)
			m->draw(animShader);
	}


	if (currAnimation != NULL && weaponMesh != NULL) {
		mat4 boneTransform = currAnimation->weaponFinalTransformation;
		GLuint bonesUniformIndex = glGetUniformLocation(animShader.getId(), "boneMatrices[0]");
		animShader.setMat4(bonesUniformIndex, boneTransform);
		animShader.setMat4("model", weaponModelMatrix);
		weaponMesh->draw(animShader);
	}
	else
		cout << "WeaponMesh==NULL!!!\n";
}
void UnitModel::processNode(aiNode* node, const aiScene* scene) {
	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);

//	cout << "NodeName: " << nodeName << endl;

	//process all meshes
	unsigned int nrMeshes = node->mNumMeshes;
	unsigned int* meshIndexes = node->mMeshes;
	for (int nrMesh = 0; nrMesh < nrMeshes; ++nrMesh) {
		int meshId = meshIndexes[nrMesh];
		aiMesh* mesh = scene->mMeshes[meshId];
		string meshName(mesh->mName.C_Str());

		if (meshName != "WeaponMesh" && weaponMesh != NULL)
				meshId -= 1;

		BoneMesh* processedMesh = processMesh(mesh, scene, meshId);
//		cout << "BoneMesh: " << meshName <<", id:" << meshId<< endl;

		if (meshName != "WeaponMesh") {
			meshes.at(meshId) = processedMesh;

			/*cout << "meshes[nrMesh]::nrVertices: " << meshes[meshId]->vertices.size() << endl;
			cout << "meshes[nrMesh]::nrIndices: " << meshes[meshId]->indices.size() << endl;
			cout << "meshes[nrMesh]::nrTextures: " << meshes[meshId]->textures.size() << endl;*/

			vec3 params = meshes[meshId]->getVolume();
			vec3 coords = meshes[meshId]->getCoords();
			if (params.x > width) width = params.x;
			if (params.y > height) height = params.y;
			if (params.z > depth) depth = params.z;
			if (coords.x < x) x = coords.x;
			if (coords.y < y) y = coords.y;
			if (coords.z < z) z = coords.z;
		}
		else
			weaponMesh = processedMesh;
	}

	for (int a = 0; a < node->mNumChildren; ++a) {
		processNode(node->mChildren[a], scene);
	}
}
BoneMesh* UnitModel::processMesh(aiMesh* mesh, const aiScene* scene, int meshId) {
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
		if (mesh->HasTextureCoords(texIndex)) {
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
	cout << "-MaterialName: " << matName << endl;
	if (matName == "Armor-material")
		modelParts.insert(make_pair(meshId, ModelPart::ARMOR));
	else if (matName == "Head-material")
		modelParts.insert(pair<int, ModelPart>(meshId, ModelPart::HEAD));
	else if (matName == "Below-material")
		modelParts.insert(make_pair(meshId, ModelPart::BELOW));
	cout << "Nr. meshes: " << meshes.size() << endl;

	if (material) {
		cout << "-Processing material...\n";
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		if(diffuseMaps.size()!=0)
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		if (specularMaps.size() != 0)
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		if (normalMaps.size() != 0)
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		if (heightMaps.size() != 0)
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return new BoneMesh(vertices, indices, textures);
}

//setters
void UnitModel::setAnimation(string animName, bool loop) {
	if (!running)
		if (animations.size() != 0)
			if (animations.find(animName) != animations.end()) {
				this->needUpdate = true;
				this->loop = loop;
				currAnimation = animations.at(animName);
			}
}

//----controls
void UnitModel::update(float elapsedTime) {
	if (needUpdate)
		if (currAnimation != NULL && currAnimation->isLoaded()) {
			bool animationFinished = currAnimation->update(elapsedTime, loop);
			if (animationFinished) {
				needUpdate = false;
			}
		}
}
void UnitModel::addAnimation(string name, string path) {
	cout << "Meshes.size: " << meshes.size() << endl;
	cout << "weaponMesh.nrVertices: " << weaponMesh->getVertices().size() << endl;
	Animation* anim = new Animation(path, name, meshes, weaponMesh);
	if (anim->isLoaded()) {
		anim->clearChanges();
		animations.insert(make_pair(name, anim));
		currAnimation = anim;
	}
}
void UnitModel::addAnimation(Animation* anim) {
	if (anim != NULL && anim->isLoaded()) {
		anim->clearChanges();
		animations.insert(make_pair(anim->getName(), anim));
		currAnimation = anim;
	}
}
void UnitModel::goTo(float x, float y, float z) {
	this->x = matrix[3][0] = weaponModelMatrix[3][0] = x;
	//this->y = matrix[3][1] = y;
	this->z = matrix[3][2] = weaponModelMatrix[3][2] = z;
}
void UnitModel::translate(float x, float y, float z) {
	//matrix = glm::translate(matrix, glm::vec3(x, y, z));
	this->x += x;
	this->y += y;
	this->z += z;
	matrix[3][0] = weaponModelMatrix[3][0] = this->x;
	matrix[3][1] = weaponModelMatrix[3][1] = this->y;
	matrix[3][2] = weaponModelMatrix[3][2] = this->z;
}
void UnitModel::changeTexture(ModelPart modelPart, ModelProperty property) {
	for (pair<int, ModelPart> part : modelParts) {
		if (part.second == modelPart) {
			Texture texture;
			texture.type = "texture_diffuse";
			texture.path = property.path;
			texture.id = loadTextureFromFile(property.path, property.texFolder, gammaCorrection);

			BoneMesh*& mesh = meshes.at(part.first);
			if (mesh->textures.size() != 0) {
				mesh->textures.at(0) = texture;
			}
			break;
		}
	}
}