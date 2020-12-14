#pragma once
#ifndef MODEL_H
#define MODEL_H
#pragma comment(lib, "glew32s.lib")

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifndef  STBI_LOADED_
	#define STBI_LOADED_
	#include <stb/stb_image.h>
#endif
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "shader.h"
#include "Properties.h"
#include <string>
#include <vector>
#include <map>
#include <gl/GL.h>
using namespace std;
using namespace Assimp;
using namespace glm;

class IModel{
protected:
	//---tmp, used in initialization
	/*Index of texture in mesh->mTextureCoords. If model has more than one material per mesh then
	Assimp split mesh in more meshes each contains one material. textureIndexes is uses for this.*/
	map<string, int> textureIndexes;
	/*textures_loaded is used for deny initialization the same texture more than one time*/
	vector<Texture> textures_loaded;
	//---
	/*  Model Data */
	vector<Texture> textures;
	bool gammaCorrection;
	glm::mat4 matrix;
	glm::mat4 rotationMatrix;
	vec3 scalingVector;
	float width = 0, height = 0, depth = 0;
	float x = 1000, y = 1000, z = 1000;
	bool selected = false;//if object is selected
	ModelProperty property;
	bool loaded = false;
	

	void recalculateMeshVolumes(float x, float y, float z);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	virtual void load(string const& path)=0;
	virtual void processNode(aiNode *node, const aiScene *scene)=0;
public:
	IModel();
	IModel(ModelProperty modelProperty, bool gamma = false);
	~IModel();

	//setters
	void setSelected(bool value);
	void setCoords(vec3 coord);

	//getters
	glm::mat4 getMatrix();
	glm::mat4 getRotationMatrix();
	float getWidth();
	float getHeight();
	float getDepth();
	glm::vec3 getVolume();
	glm::vec3 getCoords();
	const ModelProperty getProperty();
	bool isSelected();
	bool isLoaded();

	//controls
	unsigned int loadTextureFromFile(const string path, const string &directory, bool gamma=false);
	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	void goTo(float x, float y, float z);
	void goTo(vec3 position);
	mat4 fakeTranslate(float x, float y, float z);
	void extractData(ModelProperty property, bool gamma = false);//if used empty constructor
	void rotateRad(float rad, int axis);//@param axis : 1=x, 2=y, 3=z
	void rotateDegree(float degree, int axis);//@param axis : 1=x, 2=y, 3=z
};
#endif
