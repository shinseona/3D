

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <glm/mat4x4.hpp>
#include "IModel.h"
#include "Helper.h"
#include "GameWorld.h"
using std::map;
using namespace Assimp;
using namespace glm;

/*  Functions   */
// constructor, expects a filepath to a 3D model.
IModel::IModel() {
	loaded = false;
}
IModel::IModel(ModelProperty modelProperty, bool gamma) : property(modelProperty),gammaCorrection(gamma){
}

IModel::~IModel() {
	for (int a = 0; a < textures.size(); ++a)
		glDeleteTextures(1, &textures[a].id);
	textureIndexes.clear();
	textures_loaded.clear();
	textures.clear();
}

/*Getters----------------*/
glm::mat4 IModel::getMatrix() {
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
float IModel::getWidth() { return width; }
float IModel::getHeight() { return height; }
float IModel::getDepth() { return depth; }
glm::vec3 IModel::getVolume() {
	return glm::vec3(
		width,
		height,
		depth
	);
}
glm::vec3 IModel::getCoords() {

	return vec3(x, y, z);
}
const ModelProperty IModel::getProperty() { return property; }

bool IModel::isSelected() {
	return selected;
}
glm::mat4 IModel::getRotationMatrix() {
	return rotationMatrix;
}
bool IModel::isLoaded() { return loaded; }
/*------------------------*/

/*Setters--------------------*/
void IModel::setSelected(bool value) { selected = value; }
void IModel::setCoords(vec3 coord) {
	x = coord.x;
	y = coord.y;
	z = coord.z;
}
/*---------------------------*/

/*Interface--------------------*/
void IModel::translate(float x, float y, float z) {
	//matrix = glm::translate(matrix, glm::vec3(x, y, z));
	this->x += x;
	this->y += y;
	this->z += z;
	matrix[3][0] = this->x;
	matrix[3][1] = this->y;
	matrix[3][2] = this->z;
	Helper h;
	vec3 currCoord = vec3(x, y, z);
	vec2 tile = h.toTileCoords(currCoord, GameWorld::getInstance().getWorldOrigin());
	cout << "Player coords: " << this->x << ", " << this->y << ", " << this->z << endl;
	cout << "Player TILE: " << tile.x << ", " << tile.y << endl;
}
void IModel::goTo(float x, float y, float z) {
	this->x = matrix[3][0] = x;
	this->y = matrix[3][1] = y;
	this->z = matrix[3][2] = z;

	cout << "GoTo worked: " << this->x << ", " << this->y << ", " << this->z << endl;
}
void IModel::goTo(vec3 position) {
	this->x = matrix[3][0] = position.x;
	this->y = matrix[3][1] = position.y;
	this->z = matrix[3][2] = position.z;
}
mat4 IModel::fakeTranslate(float x, float y, float z) {//TODO remove the function
	return glm::translate(matrix, glm::vec3(x, y, z));
}
void IModel::scale(float x, float y, float z) {
	//TODO implement scale
	matrix = glm::scale(matrix, vec3(x, y, z));
	width *= x;
	height *= y;
	depth *= z;
	recalculateMeshVolumes(x, y, z);
}
void IModel::rotateRad(float rad, int axis) {
	assert(axis <= 3 && axis >= 1);

	if (axis == 1)
		rotationMatrix = glm::rotate(rotationMatrix, rad, vec3(1.0f, 0.0f, 0.0f));
	else if (axis == 2)
		rotationMatrix = glm::rotate(rotationMatrix, rad, vec3(0.0f, 1.0f, 0.0f));
	else
		rotationMatrix = glm::rotate(rotationMatrix, rad, vec3(0.0f, 0.0f, 1.0f));
}
void IModel::rotateDegree(float degree, int axis) {
	assert(axis <= 3 && axis >= 1);

	if (axis == 1)
		rotationMatrix = glm::rotate(rotationMatrix, radians(degree), vec3(1.0f, 0.0f, 0.0f));
	else if (axis == 2)
		rotationMatrix = glm::rotate(rotationMatrix, radians(degree), vec3(0.0f, 1.0f, 0.0f));
	else
		rotationMatrix = glm::rotate(rotationMatrix, radians(degree), vec3(0.0f, 0.0f, 1.0f));
}
/*----------------------------*/


/*Initialization-------------*/
void IModel::extractData(ModelProperty property, bool gamma) {
	this->property = property;
	gammaCorrection = gamma;
	load(property.path);
}
/*---------------------------*/


/*Utilities----------*/
void IModel::recalculateMeshVolumes(float x, float y, float z) {
	//for (int a = 0; a < meshes.size(); ++a) {
	//	meshes[a].calculateVolume();
	//}
	//TODO implement updating coords and params of meshes
}
unsigned int IModel::loadTextureFromFile(const string path, const string &directory, bool gamma)
{

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	string matPath = directory + "/";
	matPath += path;
	cout << "Texture Path: " << matPath.c_str() << endl;

	unsigned char *data = stbi_load(matPath.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Model::Error: Can't load " << matPath << " texture." << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
vector<Texture> IModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	cout << "Nr. textures: " << mat->GetTextureCount(type) << endl;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		aiReturn ret = mat->GetTexture(type, i, &str);

		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (textures_loaded[j].path == str.C_Str()) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = loadTextureFromFile(str.C_Str(), this->property.texFolder);
			texture.type = typeName;
			texture.path = string(str.C_Str());
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}
/*---------------------*/