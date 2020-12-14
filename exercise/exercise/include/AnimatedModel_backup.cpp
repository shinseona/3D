#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include "AnimatedModel.h"
#include "GameWorld.h"
#include "Helper.h"
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_operation.hpp>
using std::cout;
using std::endl;
using std::pair;
using std::map;
using std::list;
using std::vector;




AnimatedModel::AnimatedModel(string filePath) {
	modelTransformations = new map<string, mat4>();
	path = filePath;
	directory = path.substr(0, path.find('/', 0));
	initModel(filePath);
}
AnimatedModel::AnimatedModel(){
	modelTransformations = new map<string, mat4>();
}
AnimatedModel::~AnimatedModel() {
	for(int a =0; a < textures.size(); ++a)
	glDeleteTextures(1, &textures[a].id);
}

void AnimatedModel::extractBoneInformation(const aiScene* scene, aiNode* node, ModelBone* &upgrNode) {
	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);
	aiNode* parent = node->mParent;
	string parentName = parent == NULL ? "NO ROOT" : parent->mName.C_Str();

	cout << "--Node '"+nodeName+"' Transformation--\n";
	h.showMatrix(nodeTransformation);

	//iterate all meshes to save bone information and vertex weights
	unsigned int* meshesIndexes = node->mMeshes;
	int nrMeshes = node->mNumMeshes;
	for (int nrMesh = 0; nrMesh < nrMeshes; ++nrMesh) {
		aiMesh* mesh = scene->mMeshes[meshesIndexes[nrMesh]];

		aiBone** bones = mesh->mBones;
		if (bones !=NULL) {
			int nrBones = mesh->mNumBones;
			for (int nrBone = 0; nrBone < nrBones; ++nrBone) {
				aiBone* bone = bones[nrBone];
				unsigned int boneId = boneInfo.size();
				string boneName = bone->mName.C_Str();
				mat4 boneOffset = h.aiMatrix4x4ToGlm(bone->mOffsetMatrix);

				cout << "--Bone '" << boneName << "' Offset--\n";
				h.showMatrix(boneOffset);

				//mapping bone name to bone id
				BoneInfo info;
				info.BoneOffset = boneOffset;
				info.boneId = boneId;
				if (boneIds.find(boneName) == boneIds.end()) {
					boneInfo.push_back(info);
					boneIds.insert(pair<string, int>(boneName, boneId));
				}

				//add vertex weights and bone ids
				int nrWeights = bone->mNumWeights;
				for (int nrWeight = 0; nrWeight < nrWeights; ++nrWeight) {
					aiVertexWeight vertexWeight = bone->mWeights[nrWeight];
					float weight = vertexWeight.mWeight;
					uint vertexId = vertexWeight.mVertexId;
					meshes.at(nrMesh).addBoneData(vertexId, weight, boneId);
				}
			}
		}
	}

	//check if node is bone
	aiNodeAnim** channels = scene->mAnimations[0]->mChannels;
	unsigned int nrChannels = scene->mAnimations[0]->mNumChannels;
	bool isBone = false;
	for (int a = 0; a < nrChannels; ++a) {
		string name(channels[a]->mNodeName.C_Str());
		if (name.compare(nodeName) == 0) {
			isBone = true;
			break;
		}
	}

	//check if is root bone
	if (isBone && upgrNode == NULL) {
		ModelBone* newRoot = new ModelBone();
		newRoot->transformation = nodeTransformation;
		newRoot->name = nodeName;
		newRoot->parent = NULL;
		upgrNode = newRoot;
	}
	//for each child do the same operations
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		if (isBone) {
			aiNode* child = node->mChildren[nrChild];
			ModelBone* upgrChild = new ModelBone();
			upgrChild->transformation = h.aiMatrix4x4ToGlm(child->mTransformation);
			upgrChild->parent = upgrNode;
			upgrChild->name = child->mName.C_Str();
			upgrNode->children.push_back(upgrChild);
			extractBoneInformation(scene, node->mChildren[nrChild], upgrChild);
		}
		else
			extractBoneInformation(scene, node->mChildren[nrChild], upgrNode);
	}
}

void AnimatedModel::initModel(string filePath) {
	Helper h;
	cout << "AnimatedModel: Loading model '" << filePath << "'..." << endl;
	Importer importer;

	//	aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices; // calculate tangents and bitangents if possible
	//-	aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
										  //aiProcess_ValidateDataStructure  | // perform a full validation of the loader's output
	//	aiProcess_Triangulate | // Ensure all verticies are triangulated (each 3 vertices are triangle)
	////	aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space (by default right-handed, for OpenGL)
	////	aiProcess_SortByPType | // ?
	////	aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
	////	aiProcess_RemoveRedundantMaterials | // remove redundant materials
	//	aiProcess_FindDegenerates | // remove degenerated polygons from the import
	//	aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
	//	aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
	//	aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
	//	aiProcess_FindInstances; // search for instanced meshes and remove them by references to one master
	////-	aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
	////-	aiProcess_OptimizeMeshes | // join small meshes, if possible;
	//	aiProcess_PreTransformVertices; //-- fixes the transformation issue.
	//	aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
	//	0;

	unsigned int flags = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenNormals;
	const aiScene* scene = importer.ReadFile(filePath, flags);
	if (!scene || scene->mFlags&& AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "Model::'" << filePath << "':" << importer.GetErrorString() << endl;
		loaded = false;//set error
		return;
	}

	//---extracting model information
	cout << "Model: Processing nodes...\n";
	//save global inverse matrix
	m_GlobalInverseTransform = inverse(h.aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));
	//process all nodes
	processNode(scene->mRootNode, scene);
	cout << "Model: Node processed\n";

	//---extract animation information
	cout << "Model: Adding bone information in vertices...\n";	
	//if(false)
	if (scene->mAnimations != NULL) {
		animData = new AnimData();
		aiAnimation** animations = scene->mAnimations;
		aiAnimation* animation = animations[0];
		animData->ticksPerSecond = animation->mTicksPerSecond;
		animData->numChannels = animation->mNumChannels;
		animData->duration = animation->mDuration;
		animData->name = animation->mName.C_Str();

		if (animations[0]->mNumChannels > 0) {
			aiNodeAnim** channels = animation->mChannels;
			for (int nrChannel = 0; nrChannel < animation->mNumChannels; ++nrChannel) {
				aiNodeAnim* channel = channels[nrChannel];
				string channelName(channel->mNodeName.C_Str());

				AnimChannel* boneData = new AnimChannel();
				boneData->nrPosKeys = channel->mNumPositionKeys;
				boneData->nrRotKeys = channel->mNumRotationKeys;
				boneData->nrScalKeys = channel->mNumScalingKeys;
				for (int a = 0; a < boneData->nrPosKeys; ++a) {
					float time = channel->mPositionKeys[a].mTime;
					vec3 value = h.assimpVec3ToGlmVec3(channel->mPositionKeys[a].mValue);
					boneData->posKeys.push_back(PositionKey(value, time));
				}
				for (int a = 0; a < boneData->nrScalKeys; ++a) {
					float time = channel->mScalingKeys[a].mTime;
					vec3 value = h.assimpVec3ToGlmVec3(channel->mScalingKeys[a].mValue);
					boneData->scalKeys.push_back(ScalingKey(value,time));
				}
				for (int a = 0; a < boneData->nrPosKeys; ++a) {
					float time = channel->mRotationKeys[a].mTime;
					quat value = h.assimpQuatToGlmQuat(channel->mRotationKeys[a].mValue);
					boneData->rotKeys.push_back(RotationKey(value, time));
				}
				boneData->name = channelName;
			//	animData->channels.push_back(boneData);
			}
		}
	}
	cout << "Model: Bone information added.\n";

	//---extract bones information
	cout << "Model: Extracting animation information...\n";
	extractBoneInformation(scene, scene->mRootNode,rootBone);
	cout << "Model: Animation information extracted.\n";

	//---apply all node transformations, except bones
	//init matrices
	matrix = mat4(1.0f, 0.0, 0.0, 0.0,
		0.0f, 1.0, 0.0, 0.0,
		0.0f, 0.0, 1.0, 0.0,
		1.0f, 1.0, 1.0, 1.0);
	rotationMatrix = mat4(1.0f);
	for (auto it = modelTransformations->begin(); it != modelTransformations->end(); ++it) {
		pair<string, mat4> transformation = (pair<string, mat4>)*it;
		string nodeName = transformation.first;

		//if (boneIds.find(nodeName) == boneIds.end()) {//isn't bone
			mat4 nodeTransformation = transformation.second;
			for (int a = 0; a < 4; ++a) {
				for (int b = 0; b < 4; ++b) {
					if (nodeTransformation[a][b] != 0)
						matrix[a][b] *= nodeTransformation[a][b];
				}
			}
			cout << "Matrix[initial]: " << endl;
			h.showMatrix(nodeTransformation);
		//}
	}
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
	cout << "Matrix[final]: " << endl;
	h.showMatrix(matrix);

	//---fire to mesh that it has new information in vertices(boneIds and boneWeights)
	for (BoneMesh mesh : meshes) {
		mesh.update();
	}

	cout << "Model: Loaded" << endl;


	//Showing information about model, information and bones
	cout << "||||||||||||||STARTING SHOWING...|||||||||||||||||\n";
	cout << "*** Animation Data(animData)\n";
	cout << "Duration: " << animData->duration << endl;
	cout << "TicksPerSecond: " << animData->ticksPerSecond << endl;
	cout << "Num. channels: " << animData->numChannels << endl;

	vector<AnimChannel*> channels;// animData->channels;
	for (int nrChannel = 0; nrChannel < channels.size(); ++nrChannel) {
		AnimChannel channel = *channels.at(nrChannel);
		cout << "* Channel " << nrChannel << "\n";
		cout << " Name: " << channel.name << endl;
		cout << " nrPosKeys " << channel.nrPosKeys << endl;
		cout << " nrRotKeys " << channel.nrRotKeys << endl;
		cout << " nrScalKeys " << channel.nrScalKeys << endl;
		cout << "- Pos keys\n";
		for (int nrKey = 0; nrKey < channel.nrPosKeys; ++nrKey) {
			double time = channel.posKeys[nrKey].time;
			vec3 value = channel.posKeys[nrKey].value;
			cout << " Key "<< nrKey <<": "<< time <<", (" << value.x << " " << value.y << " " << value.z << ")"<< endl;
		}
		cout << "- Rot keys\n";
		for (int nrKey = 0; nrKey < channel.nrRotKeys; ++nrKey) {
			double time = channel.rotKeys[nrKey].time;
			quat value = channel.rotKeys[nrKey].value;
			cout << " Key " << nrKey << ": " << time << ", (" << value.x << " " << value.y << " " << value.z << ")" << endl;
		}
		cout << "- Scal keys\n";
		for (int nrKey = 0; nrKey < channel.nrScalKeys; ++nrKey) {
			double time = channel.scalKeys[nrKey].time;
			vec3 value = channel.scalKeys[nrKey].value;
			cout << " Key " << nrKey << ": " << time << ", (" << value.x << " " << value.y << " " << value.z << ")" << endl;
		}
	}
	cout << "*** Bone info(m_BoneInfo)\n";
	for (int a = 0; a < boneInfo.size(); ++a) {
		BoneInfo info = boneInfo.at(a);
		cout << "- BoneInfo " << a << endl;
		cout << " BoneId: " << info.boneId << endl;
		cout << " BoneOffset: " << endl;
		h.showMatrix(info.BoneOffset);
		cout << " FinalTransformation: " << endl;
		h.showMatrix(info.FinalTransformation);
	}

	cout << "*** Bone mapping(m_BoneMapping)\n";
	int counter = 0;
	for (pair<string, int> pair : boneIds) {
		cout << "- BoneMapping " << counter++ << endl;
		cout << " BoneId: " << pair.second << endl;
		cout << " BoneName: " << pair.first << endl;
	}

	vector<BoneVertex> verts = meshes.at(0).getVertices();
	cout << "Vertices: \n";
	for (int a = 0; a < verts.size(); ++a) {
		BoneVertex v = verts.at(a);
		cout << "Position: ";
		h.showVector(v.position);
		cout << "boneIds: ";
		h.showVector(v.boneIds);
		cout << "boneWeights: ";
		h.showVector(v.boneWeights);
		cout << "--------\n";
	}
	cout << "||||||||||||||SHOWING STOPPED|||||||||||||||||\n";
}


void AnimatedModel::processNode(aiNode* node, const aiScene* scene) {
	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);
	//modelTransformations->insert(make_pair(nodeName,nodeTransformation));

	for (int a = 0; a < 4; ++a) {
		for (int b = 0; b < 4; ++b) {
			//if (nodeTransformation[a][b] != 0)
			//	matrix[a][b] *= nodeTransformation[a][b];
		}
	}
	cout << "NodeName: " << node->mName.C_Str() << endl;
	cout << "NodeTransformation:\n";
	h.showMatrix(nodeTransformation);
	matrix *= nodeTransformation;

	//process all meshes
	unsigned int nrMeshes = node->mNumMeshes;
	for (int a = 0; a < nrMeshes; ++a) {
		aiMesh* mesh = scene->mMeshes[a];
		BoneMesh processedMesh = processMesh(mesh, scene);
		vec3 params = processedMesh.getVolume();
		vec3 coords = processedMesh.getCoords();
		if (params.x > width) width = params.x;
		if (params.y > height) height = params.y;
		if (params.z > depth) depth = params.z;
		if (coords.x < x) x = coords.x;
		if (coords.y < y) y = coords.y;
		if (coords.z < z) z = coords.z;
		meshes.push_back(processedMesh);
	}

	for (int a = 0; a < node->mNumChildren; ++a) {
		processNode(node->mChildren[a], scene);
	}
}

BoneMesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<BoneVertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	//cout << "MeshName--: " << mesh->mName.C_Str() << endl;

	/*Read vertices*/
	unsigned int nrVertices = mesh->mNumVertices;
	for (int a = 0; a < nrVertices; ++a) {
		BoneVertex vertex;
		vec3 tmpVector;

		//position
		tmpVector.x = mesh->mVertices[a].x;
		tmpVector.y = mesh->mVertices[a].y;
		tmpVector.z = mesh->mVertices[a].z;
		vertex.position = tmpVector;

		if (mesh->HasNormals()) {
			tmpVector.x = mesh->mNormals[a].x;
			tmpVector.y = mesh->mNormals[a].y;
			tmpVector.z = mesh->mNormals[a].z;
			vertex.normal = tmpVector;
		}

		//textures(only first texture)
		if (mesh->mTextureCoords[0]) {
			vec2 texCoord;
			texCoord.x = mesh->mTextureCoords[0][a].x;
			texCoord.y = mesh->mTextureCoords[0][a].y;
			vertex.texCoords = texCoord;
		}

		vertices.push_back(vertex);
	}

	/*Read faces*/
	unsigned int nrFaces = mesh->mNumFaces;
	for (int a = 0; a < nrFaces; ++a) {
		aiFace face = mesh->mFaces[a];
		for (int b = 0; b < face.mNumIndices; ++b) {
			indices.push_back(face.mIndices[b]);
		}
	}
	/*Read materials
	In shaders we need to name textures in next mode:
	1. texture_diffuseN, where N is number of texture from 1 to MAX_SAMPLER_NUMBER;
	2. texture_specularN, 1<=N<=MAX_SAMPLER_NUMBER
	3. texture_normalN, 1<=N<=MAX_SAMPLER_NUMBER
	*/
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material) {
		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	/*BoneVertex vert;

	//no bones
	if (mesh->mNumBones == 0) return BoneMesh(vertices, indices, textures);

	
	//- processing assimp bones;
	//- extracting bone ids and bone weights to weightsMap;
	
	multimap<int, pair<int, float>> weightsMap;
	int nrBones = mesh->mNumBones;
	for (int boneNumber = 0; boneNumber < nrBones; ++boneNumber) {
		aiBone* bone = mesh->mBones[boneNumber];
		processBoneInfo(mesh, bone, boneNumber, weightsMap);
	}
	//assign bone weights and ids to vertices
	vec4 weights(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 ids(0.0f, 0.0f, 0.0f, 0.0f);
	int counter = 0;
	for (pair<int, pair<int, float>> value : weightsMap) {
		int boneNumber = value.first;
		pair<int, float> vertexInfo = value.second;
		vec4 vertex = vertices.at(vertexInfo.first).boneWeights;
		for (int a = 0; a < MAX_BONES; ++a) {
			if (vertex[a] == 0.0) {
				vertices.at(vertexInfo.first).boneWeights[a] = vertexInfo.second;
				vertices.at(vertexInfo.first).boneIds[a] = boneNumber;
				break;
			}
		}
	}*/

	BoneMesh boneMesh(vertices, indices, textures);
	return boneMesh;
}


vector<Texture> AnimatedModel::loadMaterialTextures(aiMaterial* material, aiTextureType texType, string texName) {
	vector<Texture> textures;
	unsigned int nrTextures = material->GetTextureCount(texType);

	for (int a = 0; a < nrTextures; ++a) {
		aiString texPath;
		aiReturn ret = material->GetTexture(texType, a, &texPath);
		if (ret != aiReturn_SUCCESS) {
			cout << "Model::texture: Something wrong with material.GetTexture()" << endl;
			continue;
		}

		unsigned int nrLoadedTextures = this->textures.size();
		bool alreadyLoaded = false;
		for (int b = 0; b < nrLoadedTextures; ++b) {
			if (strcmp(texPath.C_Str(), this->textures[b].path.C_Str()) == 0) {
				alreadyLoaded = true;
				textures.push_back(this->textures[b]);
				break;
			}
		}

		if (!alreadyLoaded) {
			Texture texture;
			texture.id = loadTextureFromFile(texPath.C_Str(), this->directory);
			texture.path = texPath;
			texture.type = texName;
			textures.push_back(texture);
			this->textures.push_back(texture);
		}
	}

	return textures;
}

unsigned int AnimatedModel::loadTextureFromFile(const string& path, const string& directory, bool gamma) {
	unsigned int id;
	glGenTextures(1, &id);


	int width, height, nrComponents;
	string matPath = string(directory+"/"+path);

	unsigned char* data = stbi_load(matPath.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		cout << "Can't load texture '" << matPath.c_str() << "'" << endl;
	}
	stbi_image_free(data);

	return id;
}

bool AnimatedModel::getStatus() {
	return loaded;
}

void AnimatedModel::draw(Shader shader) {
	for (BoneMesh m : meshes) {
		m.draw(shader);
	}
}

string AnimatedModel::getFilePath() { return path; }

mat4 AnimatedModel::getGlobalInvTransMatrix() {
	return globalInvTransMatrix;
}



int AnimatedModel::getBoneCount() {
	return boneIds.size();
}

mat4 AnimatedModel::getMatrix() { 
	//TranslationM * RotationM * ScalingM;
	mat4 scal = mat4(1.0f);
	mat4 pos = mat4(1.0f);
	scal[0][0] = matrix[0][0];
	scal[1][1] = matrix[1][1];
	scal[2][2] = matrix[2][2];

	pos[3][0] = matrix[3][0];
	pos[3][1] = matrix[3][1];
	pos[3][2] = matrix[3][2];
	return pos*rotationMatrix*scal;
}


void AnimatedModel::extractData(string const &path, bool gamma) {
	this->path = path;
	initModel(path);

}

void AnimatedModel::translate(float x, float y, float z) {
	matrix = glm::translate(matrix, glm::vec3(x, y, z));
	vec3 currCoord = matrix*vec4(1.0f);
	this->x = currCoord.x;
	this->y = currCoord.y;
	this->z = currCoord.z;
}
void AnimatedModel::translate(Tile tile) {
	matrix = glm::translate(matrix, glm::vec3(tile.getX(), 1.0f, tile.getY()));
	vec3 currCoord = matrix*vec4(1.0f);
	this->x = currCoord.x;
	this->y = currCoord.y;
	this->z = currCoord.z;
}

void AnimatedModel::goTo(float x, float y, float z) {
	matrix = glm::translate(mat4(1.0f), glm::vec3(x, y, z));
//	system("pause");
//	matrix *= rotationMatrix;// glm::rotate(matrix, radians(45.0f), vec3(1.0f, 1.0f, 0.0f));

	//matrix *= animatedIdentity;
	//matrix = matrix*animatedIdentity;
	vec3 currCoord = matrix*vec4(1.0f);
	this->x = x;
	this->y = y;
	this->z = z;
}

string AnimatedModel::getPath() { return path; }

AnimatedModel& AnimatedModel::operator=(const AnimatedModel& model) {
	loaded = model.loaded;
	meshes = model.meshes;
	textures = model.textures;

	globalInvTransMatrix = model.globalInvTransMatrix;
	directory = model.directory;
	path = model.path;
	width = model.width;
	height = model.height;
	depth = model.depth;
	x = model.x;
	y = model.y;
	z = model.z;
	matrix = model.matrix;
	rootBone = model.rootBone;
	animData = model.animData;
	rootBone = model.rootBone;
	m_GlobalInverseTransform = model.m_GlobalInverseTransform;
	boneIds = model.boneIds;
	boneInfo = model.boneInfo;
	return *this;
}

AnimatedModel* AnimatedModel::operator=(const AnimatedModel* model) {
	loaded = model->loaded;
	meshes = model->meshes;
	textures = model->textures;

	globalInvTransMatrix = model->globalInvTransMatrix;
	directory = model->directory;
	path = model->path;
	width = model->width;
	height = model->height;
	depth = model->depth;
	x = model->x;
	y = model->y;
	z = model->z;
	matrix = model->matrix;
	rootBone = model->rootBone;
	animData = model->animData;
	rootBone = model->rootBone;
	m_GlobalInverseTransform = model->m_GlobalInverseTransform;
	boneIds = model->boneIds;
	boneInfo =  model->boneInfo;
	return this;
}

void AnimatedModel::setMatrix(mat4 matrix) {
	this->matrix = matrix;
}

//@param axis : 1=x, 2=y, 3=z
void AnimatedModel::rotate(float degree, int axis) {
	assert(axis <= 3 && axis >= 1);
	if(axis==1)
		rotationMatrix = glm::rotate(mat4(1.0f), radians(degree), vec3(1.0f, 0.0f, 0.0f));
	else if (axis == 2)
		rotationMatrix = glm::rotate(mat4(1.0f), radians(degree), vec3(0.0f, 1.0f, 0.0f));
	else
		rotationMatrix = glm::rotate(mat4(1.0f), radians(degree), vec3(0.0f, 0.0f, 1.0f));
}

void AnimatedModel::setRotationMatrix(mat4 matrix) {
	rotationMatrix = matrix;
}

mat4 AnimatedModel::getRotationMatrix() {
	return rotationMatrix;
}
ModelBone* AnimatedModel::getRootBone() {
	return rootBone;
}

void AnimatedModel::CalcInterpolatedRotation2(mat4& Out, float AnimationTime, const AnimChannel* pNodeAnim)
{
	/*Helper h;
	if (pNodeAnim->nrRotKeys == 1) {
		Out = glm::toMat4(pNodeAnim->rotKeys[0].value);
		return;
	}
	else if (pNodeAnim->nrRotKeys == 0) return;

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	if (RotationIndex == -1) return;//no keyframe with this time

	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->nrRotKeys);
	float DeltaTime = pNodeAnim->rotKeys[NextRotationIndex].time - pNodeAnim->rotKeys[RotationIndex].time;
	float Factor = (AnimationTime - (float)pNodeAnim->rotKeys[RotationIndex].time) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const mat4& StartRotationQ = glm::mat4_cast(pNodeAnim->rotKeys[RotationIndex].value);
	const mat4& EndRotationQ = glm::toMat4(pNodeAnim->rotKeys[NextRotationIndex].value);

	Out = mix(StartRotationQ, EndRotationQ, Factor);
	Out = normalize(Out);*/
}



unsigned int AnimatedModel::FindRotation(float AnimationTime, const AnimChannel* pNodeAnim)
{
	assert(pNodeAnim->nrRotKeys > 0);

	for (uint i = 0; i < pNodeAnim->nrRotKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->rotKeys[i + 1].time) {
			return i;
		}
	}

	//assert(0);
	return -1;
}
unsigned int AnimatedModel::FindPosition(float AnimationTime, const AnimChannel* pNodeAnim)
{
	assert(pNodeAnim->nrPosKeys > 0);

	for (uint i = 0; i < pNodeAnim->nrPosKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->posKeys[i + 1].time) {
			return i;
		}
	}

	//assert(0);
	return -1;
}
unsigned int AnimatedModel::FindScaling(float AnimationTime, const AnimChannel* pNodeAnim)
{
	assert(pNodeAnim != NULL);
	assert(pNodeAnim->nrScalKeys > 0);

	for (uint i = 0; i < pNodeAnim->nrScalKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->scalKeys[i + 1].time) {
			return i;
		}
	}

	//assert(0);
	return -1;
}
void AnimatedModel::CalcInterpolatedRotation(quat& Out, float AnimationTime, const AnimChannel* pNodeAnim)
{
	Helper h;
	if (pNodeAnim->nrRotKeys == 1) {
		Out = pNodeAnim->rotKeys[0].value;
		return;
	}
	else if (pNodeAnim->nrRotKeys == 0) return;

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	if (RotationIndex == -1) return;//no keyframe with this time

	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->nrRotKeys);
	float DeltaTime = pNodeAnim->rotKeys[NextRotationIndex].time - pNodeAnim->rotKeys[RotationIndex].time;
	float Factor = (AnimationTime - (float)pNodeAnim->rotKeys[RotationIndex].time) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const quat& StartRotationQ = pNodeAnim->rotKeys[RotationIndex].value;
	const quat& EndRotationQ = pNodeAnim->rotKeys[NextRotationIndex].value;
	Out = mix(StartRotationQ, EndRotationQ, Factor);
	Out = normalize(Out);
}
void AnimatedModel::CalcInterpolatedPosition(vec3& Out, float AnimationTime, const AnimChannel* pNodeAnim) {
	Helper h;
	if (pNodeAnim->nrPosKeys == 1) {
		Out = pNodeAnim->posKeys[0].value;
		return;
	}
	else if (pNodeAnim->nrPosKeys == 0) return;

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	if (PositionIndex == -1) return;//no keyframe with this time

	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->nrPosKeys);
	float DeltaTime = pNodeAnim->posKeys[NextPositionIndex].time - pNodeAnim->posKeys[PositionIndex].time;
	float Factor = (AnimationTime - (float)pNodeAnim->posKeys[PositionIndex].time) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	vec3 StartPositionV = pNodeAnim->posKeys[PositionIndex].value;
	vec3 EndPositionV = pNodeAnim->posKeys[NextPositionIndex].value;

	Out = lerp(StartPositionV, EndPositionV, Factor);
	if (Out.x != 0 && Out.y != 0 && Out.z != 0)
		Out = normalize(Out);
}
void AnimatedModel::CalcInterpolatedScaling(vec3& Out, float AnimationTime, const AnimChannel* pNodeAnim) {
	Helper h;
	if (pNodeAnim->nrScalKeys == 1) {
		Out = pNodeAnim->scalKeys[0].value;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	if (ScalingIndex == -1) return;//no keyframe with this time

	uint NextScalingIndex = (ScalingIndex + 1);
	float key1 = pNodeAnim->scalKeys[ScalingIndex].time;
	float key2 = pNodeAnim->scalKeys[NextScalingIndex].time;
	assert(NextScalingIndex < pNodeAnim->nrScalKeys);
	float DeltaTime = key2 - key1;
	float Factor = (AnimationTime - key1) / DeltaTime;

	/*cout << "Delta time: " << DeltaTime << endl;
	cout << "Factor: " << Factor << endl;
	cout << "AnimationTime: " << AnimationTime << endl;
	cout << "Indexes: " << ScalingIndex << ", " << ScalingIndex + 1 << endl;
	cout << "Times: " << pNodeAnim->mScalingKeys[ScalingIndex].mTime << ", " << pNodeAnim->mScalingKeys[ScalingIndex + 1].mTime << endl;
	cout << "animTime-Times0: " << (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) << endl;
	cout << pNodeAnim->mScalingKeys[ScalingIndex].mTime << "---" << pNodeAnim->mScalingKeys[ScalingIndex + 1].mTime << endl;
	*/

	assert(Factor >= 0.0f && Factor <= 1.0f);
	vec3 StartScalingQ = pNodeAnim->scalKeys[ScalingIndex].value;
	vec3 EndScalingQ = pNodeAnim->scalKeys[NextScalingIndex].value;
	//	StartScalingQ = normalize(StartScalingQ);
	//	EndScalingQ = normalize(EndScalingQ);
	//	Out = normalize(Out);

	Out = lerp(StartScalingQ, EndScalingQ, Factor);
	Out = normalize(Out);
	/*cout << "Start scaling: ";
	showVector(StartScalingQ);
	cout << "End scaling: ";
	showVector(EndScalingQ);
	cout << "Interpolated scaling: ";
	showVector(Out);*/
}
const AnimChannel* AnimatedModel::FindNodeAnim(const AnimData animData, string NodeName) {
	/*bool found = false;
	for (int a = 0; a < animData.channels.size(); ++a) {
		const AnimChannel* channel = (animData.channels.at(a));
		string channelName = channel->name;
		if (channelName.compare(NodeName) == 0) {
			return channel;
		}
	}*/
	return NULL;
}
void AnimatedModel::ReadNodeHierarchy(const float AnimationTime, ModelBone* pNode, const mat4& ParentTransform)
{
	Helper h;
	string NodeName(pNode->name);
	/*const AnimData animData =  animData;
	cout << "Name: " << animData.name << endl;
	cout << "duration: " << animData.duration << endl;
	cout << "ticksPerSecond: " << animData.ticksPerSecond << endl;
	cout << "channels: " << animData.channels.size() << endl;
	for (int a = 0; a < animData.channels.size(); ++a) {
	AnimChannel channel = animData.channels.at(a);
	cout << "Channel name: " << channel.name << endl;
	aiVectorKey* pos = channel.posKeys;
	aiQuatKey* rot = channel.rotKeys;
	aiVectorKey* scal =  channel.scalKeys;
	if (pos == NULL)
	cout << "NULL VALUE111position!\n";
	else {
	double time = pos->mTime;
	aiVector3D val = pos->mValue;
	cout << "Position value: " << time << ", (" << val.x << " " << val.y << " " << val.z << ")" << endl;
	}
	if (rot == NULL)
	cout << "NULL rotation!\n";
	else {
	double time = rot->mTime;
	aiQuaternion val = rot->mValue;
	cout << "Rotation value: " << time << ", (" << val.x << " " << val.y << " " << val.z << ")" << endl;
	}
	if (scal == NULL)
	cout << "NULL scaling!\n";
	else {
	double time = scal->mTime;
	aiVector3D val = scal->mValue;
	cout << "Scaling value: " << time << ", (" << val.x << " " << val.y << " " << val.z << ")" << endl;
	}
	}*/
	mat4 NodeTransformation = pNode->transformation;
	const AnimChannel* animChannel = FindNodeAnim(*animData, NodeName);

	if (animChannel != NULL) {
		/*cout << "BoneName: " << animChannel->name << endl;
		cout << "NrPosKeys: " << animChannel->nrPosKeys << endl;
		cout << "NrRotKeys: " << animChannel->nrRotKeys << endl;
		cout << "NrScalKeys: " << animChannel->nrScalKeys << endl;*/

		// Interpolate scaling and generate scaling transformation matrix
		vec3 Scaling(1.0);
		CalcInterpolatedScaling(Scaling, AnimationTime, animChannel);
		//	Scaling = normalize(Scaling);
		mat4 ScalingM(1.0f);
		ScalingM =  scale(ScalingM, Scaling);

		/*cout << "ScalingM matrix: \n";
		showMatrix(ScalingM);*/

		// Interpolate rotation and generate rotation transformation matrix
		quat RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, animChannel);
		mat4 RotationM = mat4(RotationQ);
		RotationM = rotationMatrix;


		// Interpolate translation and generate translation transformation matrix
		vec3 Translation(1.0f);
		CalcInterpolatedPosition(Translation, AnimationTime, animChannel);
			Translation = normalize(Translation);
		mat4 TranslationM(1.0f);
		TranslationM = glm::translate(TranslationM, Translation);


		/*cout << "Translation vector: \n";
		showVector(Translation);
		cout << "Translation matrix: \n";
		showMatrix(TranslationM);*/


		// Combine the above transformations
		NodeTransformation = mat4(1.0f);// TranslationM *RotationM * ScalingM;
	}

	mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	/*cout << "ParentTransform matrix: \n";
	showMatrix(ParentTransform);
	cout << "NodeTransformation matrix: \n";
	showMatrix(NodeTransformation);
	cout << "GlobalTransformation matrix: \n";
	showMatrix(GlobalTransformation);
	cout << "------------\n";*/

	if (boneIds.find(NodeName) != boneIds.end()) {
		uint BoneIndex = boneIds[NodeName];
		//	cout << "This. bone index: " << BoneIndex << endl;
		//	scaleMat = rotate(scaleMat, radians(90.0f), vec3(1.0, 0, 0));
		 boneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform
			* GlobalTransformation
			*  boneInfo[BoneIndex].BoneOffset;
	}

	for (int a = 0; a < pNode->children.size(); ++a) {
		ReadNodeHierarchy(AnimationTime, pNode->children[a], GlobalTransformation);
	}
}

void AnimatedModel::update(float elapsedTime) {
	//if (animData.duration != 0.0f) {
		currentFrameTime += elapsedTime;
		float timeInSeconds = currentFrameTime / 1000.0f;
		if (timeInSeconds >= animData->duration) {
			currentFrameTime = fmod(currentFrameTime,animData->duration*1000.0f);
		}

		float ticksPerSecond = animData->ticksPerSecond != 0 ? animData->ticksPerSecond : 25.0f;
		float timeInTicks = ticksPerSecond*timeInSeconds;
		float animationTime = animData->duration == 0 ? 0.0f : (float)fmod(timeInTicks, animData->duration);

		/*cout << "TimeInSeconds: " << timeInSeconds << endl;
		cout << "ticksPerSecond: " << ticksPerSecond << endl;
		cout << "timeInTicks: " << timeInTicks << endl;
		cout << "animationTime: " << animationTime << endl;
		cout << "duration: " <<  animData.duration << endl;
		cout << "---------------------" << endl;*/

		mat4 identity(1.0f);
		//if(player->hasRoute())
		ReadNodeHierarchy(animationTime, rootBone, identity);
//	}
}

///////////////////////PROCESSING//////////////////////////////////////
aiNode* FindNodeRecursivelyByName(aiNode* node, string name) {
	string currNodeName(node->mName.C_Str());
	cout << "Searching. CurrNodeName: " << currNodeName << endl;
	if (currNodeName.compare(name) == 0) return node;
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		FindNodeRecursivelyByName(node->mChildren[nrChild], name);
	}
}


void showBones(ModelBone* bone) {
	cout << "Bone: " << bone->name << endl;
	for (ModelBone* child : bone->children) {
		showBones(child);
	}
}
void calcInverseTransforms(ModelBone* bone, mat4 parentMatrix){
	mat4 bindTransform = parentMatrix*bone->transformation;
	bone->inverseBindTransform = inverse(bindTransform);
	for (ModelBone* child : bone->children) {
		calcInverseTransforms(child, bindTransform);
	}
}
void AnimatedModel::initBoneTransform(aiNode* node) {
	Helper h;
	string currNodeName(node->mName.C_Str());
	
	if(boneIds.find(currNodeName)!=boneIds.end()){
		int nodeId = boneIds.at(currNodeName);
		mat4 transformation = h.aiMatrix4x4ToGlm(node->mTransformation);
		boneTransformations.insert(make_pair(nodeId,mat4(1.0f)));
	}
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		initBoneTransform(node->mChildren[nrChild]);
	}
}

void AnimatedModel::initPose(string filePath) {
	path = filePath;
	Helper h;
	loaded = true;
	cout << "AnimatedModel: Loading model '" << filePath << "'..." << endl;
	Importer importer;
	unsigned int flags = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenNormals;
	const aiScene* scene = importer.ReadFile(filePath, flags);
	if (!scene || scene->mFlags&& AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "Model::'" << filePath << "':" << importer.GetErrorString() << endl;
		loaded = false;
		return;
	}


	//---init
	modelTransformations = new map<string, mat4>();
	m_GlobalInverseTransform = inverse(h.aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));
	matrix = mat4(1.0f);
	//---------------------

	//---process skin
	cout << "StartMatrix: \n";
	h.showMatrix(matrix);
	processNode(scene->mRootNode, scene);
	cout << "FinalMatrix: \n";
	h.showMatrix(matrix);
	cout << "Skin processed----------------\n";
	//---------------------

	//---extract bone information
	if (scene->mAnimations != NULL) {
		if (scene->mAnimations[0]->mChannels != NULL) {
			//extract vertex weights and bone indices
			extractBoneInformation(scene, scene->mRootNode, rootBone);
			//calculate inverse transforms
			calcInverseTransforms(rootBone, mat4(1.0f));
		}
		else loaded = false;
	}
	else loaded = false;
	meshes.at(0).update();//notify mesh that vertices were update
	cout << "Bone information extracted-----------\n";
	//------------------

	//---extract animation information
	cout << "Model: Adding animation information...\n";
	if (scene->mAnimations != NULL) {
		animData = new AnimData();
		aiAnimation** animations = scene->mAnimations;
		aiAnimation* animation = animations[0];
		animData->ticksPerSecond = animation->mTicksPerSecond;
		animData->numChannels = animation->mNumChannels;
		animData->duration = animation->mDuration;
		animData->name = animation->mName.C_Str();

		vector<KeyFrame*> keyFrames;
		KeyFrame* keyFrame;
		if (animation->mNumChannels > 0) {
			aiNodeAnim** channels = animation->mChannels;
			int nrKeyFrames = channels[0]->mNumPositionKeys;
			for (int nrKeyFrame = 0; nrKeyFrame < nrKeyFrames; ++nrKeyFrame) {
				aiNodeAnim* channel = channels[0];
				double keyTime = channel->mRotationKeys[nrKeyFrame].mTime;
				map<int, BoneTransform> poses;
				for (int nrChannel = 0; nrChannel < animation->mNumChannels; ++nrChannel) {
					channel = channels[nrChannel];
					string channelName(channel->mNodeName.C_Str());
					quat rotKey = h.assimpQuatToGlmQuat(channel->mRotationKeys[nrKeyFrame].mValue);
					vec3 scalKey = h.assimpVec3ToGlmVec3(channel->mScalingKeys[nrKeyFrame].mValue);
					vec3 posKey = h.assimpVec3ToGlmVec3(channel->mPositionKeys[nrKeyFrame].mValue);
					BoneTransform transform;
					transform.rotation = rotKey;
					transform.scaling = scalKey;
					transform.position = posKey;
					cout << "ChannelName: " << channelName << endl;

					if (boneIds.find(channelName) != boneIds.end()) {
						cout <<"- Bone: "<< channelName << endl;
						cout << "BoneTransform\n";
						h.showMatrix(transform.toMatrix());
						int boneId = boneIds.at(channelName);
						poses.insert(make_pair(boneId, transform));
					}
				}
				keyFrame = new KeyFrame(keyTime, poses);
				keyFrames.push_back(keyFrame);
			}
			animData->keyFrames = keyFrames;

			//show info
			if(false)
			for (int nrKeyFrame = 0; nrKeyFrame < keyFrames.size(); ++nrKeyFrame) {
				KeyFrame* keyFrame = keyFrames.at(nrKeyFrame);
				cout << "Time: " << keyFrame->getTimeStamp() << endl;
				map<int, BoneTransform> transforms = keyFrame->getPoses();
				for (auto it = transforms.begin(); it != transforms.end(); ++it) {
					pair<int, BoneTransform> transform = (pair<int, BoneTransform>)*it;
					BoneTransform t = transform.second;
					cout << "- Bone ID: " << transform.first << endl;
					cout << "Position: ";
					h.showVector(t.position);
					cout << "Scaling: ";
					h.showVector(t.scaling);
					cout << "Rotation: ";
					h.showQuat(t.rotation);
				}
			}
		}
	}
	else loaded = false;
	
	//init bone transformation
	initBoneTransform(scene->mRootNode);

	/*vector<BoneVertex> vertexes = meshes.at(0).getVertices();
	for (int a = 0; a < vertexes.size(); ++a) {
		BoneVertex v = vertexes.at(a);
		cout << "Vertex "<<a<<"\n";
		cout << "- IDs:\n";
		h.showVector(v.boneIds);
		cout << "- Weights:\n";
		h.showVector(v.boneWeights);
	}*/

	cout << "Model: Animation information added.--------------\n";
}

void AnimatedModel::updatePose(map<int, mat4> currentPose, ModelBone* bone, mat4 parentTransform) {
	
	cout << "BoneName: " << bone->name << endl;

	if (boneIds.find(bone->name) == boneIds.end()) {
		cout << "This ID not exists\n";
	}
	int boneId = boneIds.at(bone->name);
	cout << "B1\n";
	mat4 currLocalTransform = currentPose.at(boneId);
	cout << "B2\n";
	mat4 globalTransform = parentTransform*currLocalTransform;
	cout << "B3\n";
	boneTransformations.at(boneId) =  globalInvTransMatrix * globalTransform * boneInfo.at(boneId).BoneOffset;

	for (ModelBone* childBone : bone->children) {
		updatePose(currentPose, childBone, globalTransform);
	} 
	
}

KeyFrame** AnimatedModel::getNextAndPrevFrames(double animTime) {
	cout << "CheckPoint\n";
	vector<KeyFrame*> frames = animData->keyFrames;
	KeyFrame* prevFrame = frames[0];
	KeyFrame* nextFrame = frames[0];
	cout << "CheckPoint1\n";
	for (int nrFrame = 1; nrFrame < frames.size(); ++nrFrame) {
		nextFrame = frames[nrFrame];
		if (nextFrame->getTimeStamp() > animTime)
			break;
		prevFrame = nextFrame;
	}
	cout << "CheckPoint2\n";
	KeyFrame** prevAndNext = new KeyFrame*[2];
	prevAndNext[0] = prevFrame;
	prevAndNext[1] = nextFrame;
	cout << "CheckPoint3\n";
	return prevAndNext;
}

void AnimatedModel::updateModel(float elapsedTime) {
	cout << "Here1\n";
	elapsedTime /= 1000;
	Helper h1;
	if (!loaded) { cout << "Animation of '" << path << "' not loaded!\n";  return; }
	Helper h;
	currentFrameTime += elapsedTime;
	if (currentFrameTime > animData->duration)
		currentFrameTime = fmod(currentFrameTime, animData->duration);
	KeyFrame** frames = getNextAndPrevFrames(currentFrameTime);
	KeyFrame* prevFrame = frames[0];
	KeyFrame* nextFrame = frames[1];
	cout << "Here2\n";
	float totalTime = nextFrame->getTimeStamp() - prevFrame->getTimeStamp();
	float currentTime = currentFrameTime - prevFrame->getTimeStamp();
	float progress = currentTime / totalTime;
	cout << "Here3\n";
	//calculate current pose
	map<int, mat4> currentPose;
	for (auto iterBoneId = boneIds.begin(); iterBoneId != boneIds.end(); ++iterBoneId) {
		int boneId = ((pair<string, int>)*iterBoneId).second;
		BoneTransform prevTransform = prevFrame->getPoses().at(boneId);
		BoneTransform nextTransform = nextFrame->getPoses().at(boneId);
		BoneTransform currentTransform = BoneTransform::interpolate(prevTransform, nextTransform, progress);
		mat4 currentTransformationMatrix = currentTransform.toMatrix();
		currentPose.insert(make_pair(boneId, currentTransformationMatrix));
	}

	printf("PrevTimeStamp: %.5f\n\
		NextTimeStamp: %.5f\n\
		AnimDuration: %.5f\n\
		ElapsedTime: %.5f\n\
		TotalTime: %.5f\n\
		CurrentTime: %.5f \n\
		Progress: %.5f\n", prevFrame->getTimeStamp(), nextFrame->getTimeStamp(),
		animData->duration, elapsedTime, totalTime, currentTime, progress);
	cout << "-------------\n";
	
	//update current pose
	updatePose(currentPose, rootBone, mat4(1.0f));
	cout << "Pose updated\n";

	/*boneInfo.size();
	boneIds.size();
	cout << "Bone transformations\n";
	for (auto it = boneTransformations.begin(); it != boneTransformations.end(); ++it) {
		pair<int, mat4> value = (pair<int, mat4>)*it;
		cout << "Bone " << value.first <<" transformation"<< endl;
		h.showMatrix(value.second);
	}*/

	
}

///////////////////////-------------------------PROCESSING------------------------//////////////////////////////////////

bool AnimatedModel::isSelected() {
	return selected;
}
void AnimatedModel::setSelected(bool value) { selected = value; }

vec3 AnimatedModel::getVolume() {
	return vec3(width, height, depth);
}

vec3 AnimatedModel::getCoords() {
	return vec3(x, y, z);
}