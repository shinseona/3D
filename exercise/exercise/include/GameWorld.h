#pragma once
#ifndef GAMEWORLD_H_
#define GAMEWORLD_H_

#define GL_GLEXT_PROTOTYPES
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cfloat>
#include "shader.h"
#include "camera.h"
#include "StaticModel.h"
#include "AnimatedModel.h"
#include "UnitModel.h"
#include "SimpleModel.h"
#include "AlgRayBoxAABB.h"
#include "AlgRayCasting.h"
#include "AlgAStarPathfinder.h"
#include "Helper.h"
#include "player.h"
#include "Enemy.h"
#include "Map.h"
#include "AmmoManager.h"
#include "TextManager.h"
#include "ProgressBar.h"
#include "FileManager.h"
#include "DrawingEngine.h"
#include "Observer.h"
#include "Observable.h"
using glm::vec3;
using std::pair;
using glm::mat4;



class GameWorld : protected Observable {
private:
	// settings
	double windowWidth = 800;
	double windowHeight = 600;
	float worldOriginX, worldOriginY, worldOriginZ;
	FileManager* fileManager;
	// camera
	Camera* camera;
	double lastX = windowWidth / 2.0;
	double lastY = windowHeight / 2.0;
	bool firstMouse = true;
	// timing
	float currentDeltaTime;
	long long currentTime;
	long long lastTime;
	float FPS = 30.0f;// frames per second
	float FRAME_TIME = (1000.0f / FPS);//miliseconds
	//models/objects
	SimpleModel* mouseModel;
	SimpleModel* mouseRay;
	SimpleModel* centerLine;
	SimpleModel* pathLine;
	//objects settings
	bool drawGrid = true;
	//matrices
	mat4 projection, view;//world matrices
	mat4 clickedMatrix;//
	//window
	GLFWwindow* window;
	//other classes
	AlgRayCasting* rayCasting;
	AlgRayBoxAABB* rayboxCollision;
	Helper helper;
	AlgAStarPathfinder* pathfinder;
	//map
	Map* gameMap;
	AmmoManager* ammoManager;
	//Player
	Player* player;
	//drawing
	DrawingEngine* drawingEngine;
	//
	Enemy* selectedEnemy;
	TextManager* textManager;
	vector<Enemy*> enemies;
	SimpleModel* simpleCube;
	SimpleModel* enemyInfo;
	SimpleModel* mainPanel;
	SimpleModel* playerInfo;
	ProgressBar* bar;

	StaticModel* terrain;
	Shader* gridShader;
	Ammo* ammo;
	StaticModel* ammoModel;
	// process all inputs
	//-------------------
	//-realtime process input(not using)
	//void processInput(GLFWwindow *window);
	//-process keyboard
	static void  keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void framebuffer_sizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	//-------------------

	//game lifecycle
	int init();
	void finish();
	int run();
	void draw();
	void collide();
	void update(double time);
	void input();
	//creating
	SimpleModel* createGUI(string imagePath, double screenX, double screenY, double width, double height);
	SimpleModel* createSimpleCube(vec3 min, vec3 volume, string texturePath = "");
	SimpleModel* createLine(vector<Vertex> vertices, vector<unsigned int> indices, string texturePath = "");
	//ray colliding
	map<double, Enemy*> checkRayWithEnemies(vec3 origin, vec3 direction);
	map<double, StaticModel*>checkRayWithMapObjs(vec3 origin, vec3 direction);
	pair<bool, double> checkRayWithPlayer(vec3 origin, vec3 direction);
	bool checkRayWithTerrain(vec3 origin, vec3 direction);

	GameWorld();
	GameWorld::GameWorld(const GameWorld& g);
	GameWorld operator=(const GameWorld& g) {
		return *this;
	}
public:
	//controls
	void start();

	//getters
	static GameWorld &getInstance() {
		static GameWorld instance;
		return instance;
	}
	float getWorldOriginX() {
		return worldOriginX;
	}
	float getWorldOriginY() {
		return worldOriginY;
	}
	float getWorldOriginZ() {
		return worldOriginZ;
	}
	vec3 getWorldOrigin() {
		return vec3(worldOriginX, worldOriginY, worldOriginZ);
	}

	//testing
	void GameWorld::enemyAttacked(Enemy* enemy);
	void GameWorld::terrainAttacked(vec3 origin, vec3 direction);
};

#endif GAMEWORLD_H_