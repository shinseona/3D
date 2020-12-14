
#define GL_GLEXT_PROTOTYPES
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>
#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <sstream>
#include <cfloat>
#include "shader.h"
#include "camera.h"
#include "IModel.h"
#include "SimpleModel.h"
#include "AlgRayBoxAABB.h"
#include "AlgRayCasting.h"
#include "AlgBoxBoxAABB.h"
#include "GameWorld.h"
#include "AnimatedModel.h"
#include "Ammo.h"
#include "Enemy.h"
//#include <Cairo\cairo.h>
#include "CollideTesting.h"
#include "ProgressBar.h"
#include "Properties.h"
#include "DrawingEngine.h"
using glm::vec3;
using glm::mat4;
using std::stringstream;
using namespace std::this_thread;

//constructors
GameWorld::GameWorld() {}
GameWorld::GameWorld(const GameWorld& g) {}

//Event listeners------------------
void GameWorld::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GameWorld& g = GameWorld::getInstance();
	float deltaTime = g.currentDeltaTime;
	Player* player = g.player;

	float velocity = 0.125;
	vec3 up(0.0f, 1.0f, 0.0f);
	vec3 down(0.0f, -1.0f, 0.0f);
	vec3 right(1.0f, 0.0f, 0.0f);
	vec3 left(-1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		g.camera->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		g.camera->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		g.camera->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		g.camera->processKeyboard(RIGHT, deltaTime);
	}
	float pas = 2.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		g.player->translate(0.0, 0.0, -pas);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		g.player->translate(0.0, 0.0, pas);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		g.player->translate(pas, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		g.player->translate(-pas, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		g.camera->translate(0, 0.5f, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		g.camera->translate(0, -0.5f, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		g.drawGrid = !g.drawGrid;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
	//	mat4 matrix = g.model->getRotationMatrix();
	//	matrix = rotate(matrix, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
		g.player->getModel().rotateDegree(45.0f,1);
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		g.player->getModel().rotateDegree(45.0f, 2);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		g.player->getModel().rotateDegree(45.0f, 3);
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		g.player->getModel().setAnimation("PistolShotAnim", true);
	}
	
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		ModelProperty property;
		property.path = "face2.png";
		property.texFolder = "models/textures/armors";
		g.player->getModel().changeTexture(ModelPart::HEAD, property);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		ModelProperty property;
		property.path = "armor2.png";
		property.texFolder = "models/textures/armors";
		g.player->getModel().changeTexture(ModelPart::ARMOR, property);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		ModelProperty property;
		property.path = "below2.png";
		property.texFolder = "models/textures/armors";
		g.player->getModel().changeTexture(ModelPart::BELOW, property);
	}
}
void GameWorld::framebuffer_sizeCallback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	GameWorld& g = GameWorld::getInstance();
	g.windowWidth = width;
	g.windowHeight = height;
	pair<double, double> p = make_pair(width, height);
	for (Observer o : g.observers) {
		o.update(g, p);
	}
}
void  GameWorld::mouseCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (window == NULL) {
		cout << "ERROR::mouseCursorCallback: Null window!\n";
		return;
	}

	GameWorld& g = GameWorld::getInstance();
	if (g.firstMouse)
	{
		g.lastX = xpos;
		g.lastY = ypos;
		g.firstMouse = false;
	}

	double xoffset = xpos - g.lastX;
	double yoffset = g.lastY - ypos; // reversed since y-coordinates go from bottom to top

	g.lastX = xpos;
	g.lastY = ypos;
	g.camera->processMouseMovement((float)xoffset, (float)yoffset);


	mouseButtonCallback(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
}
void GameWorld::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	GameWorld& g = GameWorld::getInstance();
	g.camera->processMouseScroll((float)yoffset);
}
void GameWorld::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		GameWorld& g = GameWorld::getInstance();
		double viewportX, viewportY;
		Helper h;
		glfwGetCursorPos(window, &viewportX, &viewportY);

		g.selectedEnemy = NULL;
		g.textManager->blockDraw("EnemyInfo");

		//calculate vector origin & direction
		g.rayCasting->calculate(viewportX, viewportY, g.windowWidth, g.windowHeight, g.projection, g.camera->getViewMatrix());
		vec3 origin = g.rayCasting->getOrigin();
		vec3 direction = g.rayCasting->getDirection();

		//g.terrainAttacked(origin, direction);
		//g.terrainAttacked(vec3(0,0,0), vec3(100,40,100));

		//-------- detecting colliding
		//detect map object colliding
		map<double, StaticModel*> collidedMapObjects = g.checkRayWithMapObjs(origin, direction);
		//detect colliding with enemies
		map<double, Enemy*> collidedEnemies = g.checkRayWithEnemies(origin, direction);
		//detect Player colliding
		pair<bool, double> collidedWithPlayer = g.checkRayWithPlayer(origin, direction);
		//detect Terrain colliding
		bool collidedWithMap = g.checkRayWithTerrain(origin, direction);
		//-----------End colliding detection

		//--------SELECTING
		//select selected object\map\player\enemy
		double minTNear = 1000.0f;
		int type = -1;
		for (pair<double, Enemy*> pair : collidedEnemies) {//check colliding with enemies
			double tNear = pair.first;
			if (tNear < minTNear) {
				minTNear = tNear;
				type = 0;
			}
		}
		for (pair<double, StaticModel*> pair : collidedMapObjects) {//check colliding with mapObjects
			double tNear = pair.first;
			if (tNear < minTNear) {
				minTNear = tNear;
				type = 1;
			}
		}
		if (collidedWithPlayer.second < minTNear) {//collided with player
			type = 2;
		}
		if (type == -1 && collidedWithMap)//collided with terrain
			type = 3;

		cout << "Type: " << type << endl;
		switch (type) {
		case 0://collided with enemy
			//collidedEnemies.at(minTNear)->getModel().setSelected(true);
			g.enemyAttacked(collidedEnemies.at(minTNear));
			break;
		case 1://collided with map object
			collidedMapObjects.at(minTNear)->setSelected(true);
			break;
		case 2://collided with player
			g.player->getModel().setSelected(true);
			break;
		case 3://collided with map
			break;
		case -1://no collide
			break;
		}


		/*
		if (type == 3) {//collided with MAP. Search path for player
			cout << "Is collided with Terrain" << endl;
			vec3 point = g.helper.binSearch(origin, origin + 40.0f*direction, 0, 30, 1.0f);
			vec2 tile = g.helper.toTileCoords(point, g.getWorldOrigin());
			cout << "Clicked on Coord " << point.x << ", " << point.z << endl;
			cout << "Clicked on tile " << tile.x << ", " << tile.y << endl;

			//path finding
			time_t currTime;
			time(&currTime);
			srand(currTime%UINT_MAX);
			int** matrix;
			int cols = g.gameMap->getHeight(), rows = g.gameMap->getWidth();
			matrix = new int*[rows];
			for (int a = 0; a < rows; ++a) {
				matrix[a] = new int[cols];
				for (int b = 0; b < cols; ++b) {
					matrix[a][b] = 0;// rand() % 2;
				}
			}
			vector<Tile> tiles = g.pathfinder->searchPath(g.gameMap->getGridPatency(),
				rows,
				cols,
				g.player->getTile(),
				Tile(tile));
			for (Tile t : tiles) {
				cout << "Tile: " << t.getX() << ", " << t.getY() << endl;
			}
			if (tiles.size() != 0) {//if path found
				vector<unsigned int> indices;
				vector<Vertex> vertices;
				vec3 currPos;
				Vertex v;
				vec3 worldOrigin = g.getWorldOrigin();
				cout << "WorldORIGIN: ";
				h.showVector(worldOrigin);
				cout << "TerrainVolume: ";
				h.showVector(g.gameMap->getTerrainVolume());

				for (Tile t : tiles) { 
					currPos = g.helper.toWorldCoords(t, worldOrigin,g.gameMap->getTerrainVolume());
					cout << "CurrPosition: " << currPos.x << ", " << currPos.z << endl;
					v.Position = currPos;
				//	v.Position.x -= 1;
					v.Position.y = 3;// += 10;
			//		v.Position.z += 1;
					vertices.insert(vertices.begin(), v);
				}

				g.pathLine->updateVertices(vertices);
				g.player->setRoute(tiles, false);
			}
			else
				cout << "No path" << endl;//not path between these tiles

		}
		*/

		//collided with MAP. Search path for player
		if(false)
		if (type == 3) {
			cout << "Is collided with Terrain" << endl;
			vec3 point = g.helper.binSearch(origin, origin + 40.0f*direction, 0, 30, 1.0f);
			vec2 tile = g.helper.toTileCoords(point, g.getWorldOrigin()- g.gameMap->getTerrainVolume());
			cout << "Clicked on Coord " << point.x << ", " << point.z << endl;
			cout << "Clicked on tile " << tile.x << ", " << tile.y << endl;

			//path finding
			
			vector<Tile> tiles = g.pathfinder->searchPath(g.gameMap->getGridPatency(),
				g.gameMap->getHeight(),
				g.gameMap->getWidth(),
				g.player->getTile(),
				Tile(tile));
			for (Tile t : tiles) {
				cout << "Tile: " << t.getX() << ", " << t.getY() << endl;
			}

			if (tiles.size() != 0) {//if path found
				vector<unsigned int> indices;
				vector<Vertex> vertices;
				vec3 currPos;
				Vertex v;
				vec3 worldOrigin = g.getWorldOrigin();
				cout << "WorldORIGIN: ";
				h.showVector(worldOrigin);
				cout << "TerrainVolume: ";
				h.showVector(g.gameMap->getTerrainVolume());

				for (Tile t : tiles) {
					currPos = g.helper.toWorldCoords(t, worldOrigin, g.gameMap->getTerrainVolume());
					cout << "CurrPosition: " << currPos.x << ", " << currPos.z << endl;
					v.Position = currPos;
					v.Position.x -= 1;
					v.Position.y = 10;// += 10;
					v.Position.z -= 1;
					vertices.insert(vertices.begin(), v);
				}

				g.pathLine->updateVertices(vertices);
				g.player->setRoute(tiles, true);
			}
			else
				cout << "No path" << endl;//not path between these tiles

		}

		//update ray indices and vertices(for line/ray drawing)
		vector<unsigned int> indices;
		vector<Vertex> vertices;
		Vertex v;
		v.Position = origin;
		vertices.push_back(v);
		v.Position = origin + direction*80.0f;
		vertices.push_back(v);
		g.mouseRay->updateVertices(vertices);
	}
}
//---------------------------------

map<double, Enemy*> GameWorld::checkRayWithEnemies(vec3 origin, vec3 direction) {
	map<double, Enemy*> collidedEnemies;
	for (int nrEnemy = 0; nrEnemy < enemies.size(); ++nrEnemy) {
		Enemy* e = enemies.at(nrEnemy);
		UnitModel& m = e->getModel();
		vec3 volume = m.getVolume();
		vec3 coords = m.getCoords();
		coords -= volume / 2.0f;
		m.setSelected(false);
		bool collidedWithEnemy = rayboxCollision->isCollided(coords, volume, origin, direction);
		if (collidedWithEnemy) {
			cout << "Is collided with " << "enemy" << endl;
			double tnear = rayboxCollision->getTNear();
			collidedEnemies.insert(make_pair(tnear, e));
		}
	}
	return collidedEnemies;
}
map<double, StaticModel*> GameWorld::checkRayWithMapObjs(vec3 origin, vec3 direction) {
	map<double, StaticModel*> collidedMapObjects;
	for (StaticModel*& m : gameMap->getModels()) {
		vec3 volume = m->getVolume();
		vec3 coords = m->getCoords();
		coords -= volume / 2.0f;

		m->setSelected(false);
		bool collidedWithMapObject = rayboxCollision->isCollided(coords, volume, origin, direction);
		if (collidedWithMapObject) {
			cout << "Is collided with obj " << m->getProperty().name << endl;
			double tnear = rayboxCollision->getTNear();
			collidedMapObjects.insert(make_pair(tnear, m));
		}
	}
	return collidedMapObjects;
}
pair<bool, double> GameWorld::checkRayWithPlayer(vec3 origin, vec3 direction) {
	pair<bool, double> result;
	UnitModel& m = player->getModel();
	vec3 volume = m.getVolume();
	vec3 coords = m.getCoords();
	coords -= volume / 2.0f;
	m.setSelected(false);
	bool collidedWithPlayer = rayboxCollision->isCollided(coords, volume, origin, direction);
	double playerTNear = 1000.0;
	if (collidedWithPlayer) {
		playerTNear = rayboxCollision->getTNear();
		cout << "Is collided with " << "player" << endl;
	}
	result.first = collidedWithPlayer;
	result.second = playerTNear;
	return result;
}
bool GameWorld::checkRayWithTerrain(vec3 origin, vec3 direction) {
	//detect terrain coliding
	vec3 coords = gameMap->getTerrainCoords();
	vec3 volume = gameMap->getTerrainVolume();
	coords -= volume / 2.0f;
	bool collidedWithMap = rayboxCollision->isCollided(coords, volume, origin, direction);
	if (collidedWithMap) {
		cout << "Is collided with " << "map" << endl;
	}

	return false;// collidedWithMap;
}


void GameWorld::enemyAttacked(Enemy* enemy) {
	enemy->getModel().setSelected(true);
	textManager->allowDraw("EnemyInfo");
	ammoManager->addAmmo(1,
		player->getModel().getCoords(), 
		enemy->getModel().getCoords(),vec3(0.0,0.0,0.0));
	selectedEnemy = enemy;	
	cout << "Selected enemy: " << selectedEnemy->getName() << endl;
}
void GameWorld::terrainAttacked(vec3 origin, vec3 direction) {
	Helper h;
	vec3 vect1 = direction - origin;
	vec3 rotationDegrees(0.0f);

	vec3 vect2 = vec3(0.0, 0.0, 300.0f);
	vec3 vect3 = vec3(0.0f, 300.0, 0.0f);
	vec3 vect4 = vec3(300.0f, 0.0, 0.0f);
	float degree = h.calcDegreeBetween(vect1, vect2);
	float degree2 = h.calcDegreeBetween(vect1, vect4);

	rotationDegrees.y = degree;
	rotationDegrees.z = degree2;
	ammoManager->addAmmo(1,origin,direction,rotationDegrees);
	/*ModelProperty prop;
	prop.path = "models/ammo.dae";
	prop.texFolder = "models";
	ammoModel = new StaticModel(prop);
	AmmoProperty prop2;
	prop2.type = 0;
	ammo = new Ammo(*ammoModel, prop2, origin, direction);*/
	//system("pause");
}

class A {
public:
	int var1 = 45;
	A(int i){
		var1 = i;
		cout << "A constructor called" << endl;
	}
	~A() {
		cout << "A dectructor called" << endl;
	}
};
class B : public A {
public:
	int var2 = 45;

	B(int i) : A(312){
		var2 = i;
		cout << "B constructor called" << endl;
	}
	~B() {
		cout << "B dectructor called" << endl;
	}
};

void func() {

}

//game lifecycle-------------
void GameWorld::start() { 
	int ret;
	ret = init();
	if (ret == 1)
	ret = run();
	finish();

	//init();
	//ModelProperty prop;
	//prop.name = "Hero";
	//prop.animated = true;
	//prop.path = "models/anim_man2.dae";
	//prop.texFolder = "models/textures/armors";
	////UnitModel* model = new UnitModel(prop);
	////StaticModel* model = new StaticModel(prop);
	//AnimatedModel* model = new AnimatedModel(prop);
	//cout << "0000000000000000000000000000000000000000\n";
	////Enemy* enemy = new Enemy(model, 0, 0);
	////Player* player = new Player(model,0,0);
	////Map* map = new Map(40, 40);
	////map->addObject(model);
	//system("pause");


	////////////////////////////////////////
	/*cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create_from_png("newImg.png");
	//surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,100, 20);
	cr = cairo_create(surface);
	
	//fill background
	cairo_rectangle(cr, 0, 0, 0.5, 0.5);
	cairo_set_source_rgb(cr, 0.2, 0.9, 0.2);
	cairo_fill(cr);

	//draw text
	cairo_set_source_rgb(cr, 0.2, 0.8, 0.3);
	cairo_select_font_face(cr, "Verdana",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 13);
	cairo_move_to(cr, 0, 10);
	cairo_show_text(cr, "2000/3670");

	cairo_surface_write_to_png(surface, "newImg.png");

	cairo_surface_destroy(surface);
	cairo_destroy(cr);*/

	//init
/*	Model model("models/ammo.dae");
	AmmoProperty property;
	map<int, Model> models;
	map<int, AmmoProperty> properties;
	ammoManager = new AmmoManager(models,properties);
	//create
	Ammo* ammo1 = new Ammo(model, property, vec3(0,0,0.0f), vec3(1000.0, 0, 0.0f));
	Ammo* ammo2 = new Ammo(model, property, vec3(0, 0, 0.0f), vec3(100.0, 0, 0.0f));
	Ammo* ammo3 = new Ammo(model, property, vec3(0, 0, 0.0f), vec3(0, 0, 0.0f));
	Ammo* ammo4 = new Ammo(model, property, vec3(0, 0, 0.0f), vec3(0, 0, 0.0f));
	Enemy* enemy1 = new Enemy(model, 0, 0);
	Enemy* enemy1 = new Enemy(model, 0, 0);
	Enemy* enemy1 = new Enemy(model, 0, 0);
	Enemy* enemy1 = new Enemy(model, 0, 0);
	ammoManager->addAmmo(ammo1);
	ammoManager->addAmmo(ammo2);
	ammoManager->addAmmo(ammo3);
	ammoManager->addAmmo(ammo4);
	//test
	CollideTesting* testing = new CollideTesting();
	
	testing->test(ammoManager, enemies);

	system("pause");
	*/
}
int GameWorld::init() {

	//* glfw: initialize, configure, create
	// ------------------------------[
	int ret = glfwInit();
	if (ret != GLFW_TRUE) {
		std::cout << "Failed to init GLFW!" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Application", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
		return -1;
	}
	glfwMakeContextCurrent(window);
	// --------------------]

	// glew: load all OpenGL function pointers
	// ---------------------------------------[
	if (ret = glewInit())
	{
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(1) << std::endl;
		system("pause");
		return -1;
	}
	// ---------------------------------------]

	glViewport(0, 0, 800, 600);
	glLineWidth(3);
	glColor3f(0.3, 0.4, 0.9);
	// configure global opengl state
	// -----------------------------[
	glEnable(GL_DEPTH_TEST);
	glColor3b(50, 50, 200);
	// -----------------------------]

	// tell GLFW to capture our mouse
	// ------------------------------[
	 //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// ------------------------------]

//	if (true) return -1;

	//load constants
	// -----------[
	cout << "Settings and Constants: loading..." << endl;
	fileManager = new FileManager("settings");
	if (!fileManager->isLoaded()) {
		cout << "Settings and Constants: " << fileManager->getError() << endl;
		system("pause");
		return -1;
	}
	cout << "Settings and Constants: loaded." << endl;
	// -----------]

	//init drawing engine
	drawingEngine = new DrawingEngine(windowWidth, windowHeight);
	addObserver(*drawingEngine);

	//Loading models-------------[
	vector<unsigned int> indices;
	vector<Vertex> vertices;
	Vertex v;
	std::cout << "Models: Loading models...\n";
	StaticModel* terrainModel = new StaticModel(fileManager->getModelProperty("Terrain"));
	UnitModel* playerModel = new UnitModel(fileManager->getModelProperty("Player"));
	playerModel->addAnimation("PistolShotAnim", "models/anim_man2.dae");
	//simple models(no texture, just points)
	//Init mouse
	indices.clear();
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(0);
	mouseModel = new SimpleModel(vector<Vertex>(), indices, "models/cursor_target.png");
	//init mouse ray
	indices.clear();
	indices.push_back(0);
	indices.push_back(1);
	mouseRay = new SimpleModel(vector<Vertex>(), indices);
	//init center line
	indices.clear();
	v.Position = vec3(0.0, -100.0, 0.0f);
	vertices.push_back(v);
	v.Position = vec3(0.0, 100.0, 0.0f);
	vertices.push_back(v);
	indices.push_back(0);
	indices.push_back(1);
	centerLine = new SimpleModel(vertices, indices);
	//init path line
	pathLine = new SimpleModel(vector<Vertex>(), vector<unsigned int>());
	std::cout << "Models: loaded.\n";
	//----------------]

	//set world origins---------------[
	vec3 terrainVolume = terrainModel->getVolume();
	vec3 terrainCoords = terrainModel->getCoords();
	int mapWidth = terrainVolume.z;//(int)(terrainVolume.z / 2);
	int mapHeight = terrainVolume.x;// (int)(terrainVolume.x / 2);
	worldOriginX = terrainCoords.x - terrainVolume.x*0.5f;
	worldOriginY = terrainCoords.y + terrainVolume.y;
	worldOriginZ = terrainCoords.z - terrainVolume.z*0.5f;
	cout << "Terrain volume: " << terrainVolume.x
		<< ", " << terrainVolume.y
		<< ", " << terrainVolume.z << endl;
	cout << "MapDimensions: " << mapWidth << ", " << mapHeight << endl;
	cout << "WorldOrigin: " << worldOriginX << ", " << worldOriginY << ", " << worldOriginZ << endl;
	//------------------------]

	//Init game elements-------------[
	//Init map
	cout << "Game elements: settings..." << endl;
	cout << "Map initializing" << endl;
	gameMap = new Map(mapWidth, mapHeight);
	gameMap->addObject(terrainModel);
	cout << "Map initialized." << endl;
	//Init player
	cout << "Player initializing" << endl;
	player = new Player(playerModel, 1, 1);
	cout << "Player initialized." << endl;
	//Init algorithms
	cout << "Algorithms initializing" << endl;
	rayCasting = new AlgRayCasting();
	rayboxCollision = new AlgRayBoxAABB();
	pathfinder = new AlgAStarPathfinder();
	cout << "Algorithms initialized." << endl;
	//Init camera
	cout << "Camera initializing" << endl;
	camera = new Camera(vec3(worldOriginX, 21, worldOriginZ));//
	cout << "Camera initialized." << endl;
	cout << "Game elements: setted." << endl;
	//Init mouse
	indices.clear();
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(0);
	mouseModel->updateIndices(indices);
	//Init Ammo Manager
	ammoManager = new AmmoManager(fileManager->getAmmoProperties());
	//----------------------]

	// set listeners(callbacks)
	// --------------------[
	cout << "Window callbacks: setting..." << endl;
	glfwSetFramebufferSizeCallback(window, framebuffer_sizeCallback);
	glfwSetCursorPosCallback(window, mouseCursorCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	cout << "Window callbacks: set." << endl;
	// --------------------]


	UnitModel* enemyModel = new UnitModel(fileManager->getModelProperty("Player"));
	UnitModel* enemyModel2 = new UnitModel(fileManager->getModelProperty("Player"));
	UnitModel* enemyModel3 = new UnitModel(fileManager->getModelProperty("Player"));
	enemyModel->addAnimation("PistolShotAnim", "models/anim_man2.dae");
	enemyModel2->addAnimation("PistolShotAnim", "models/anim_man2.dae");
	enemyModel3->addAnimation("PistolShotAnim", "models/anim_man2.dae");
	Enemy* enemy1 = new Enemy(enemyModel, 13, 3);
	Enemy* enemy2 = new Enemy(enemyModel2, 20, 20);
	Enemy* enemy3 = new Enemy(enemyModel3, 20, 12);
	enemy1->setName("Enemy1");
	enemy2->setName("Enemy2");
	enemy3->setName("Enemy3");
	enemies.push_back(enemy1);
	enemies.push_back(enemy2);
	enemies.push_back(enemy3);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	enemyInfo = createGUI("enemy_info.png", windowWidth - 200, windowHeight - 100, 200, 100);
	mainPanel = createGUI("main_panel.png", 50, 50, 230, 62);
	playerInfo = createGUI("player_Info.png", windowWidth - 255, 5, 250, 100);

	textManager = new TextManager();
	SimpleModel* verdanaFont = new SimpleModel(vertices, indices, "fonts/VerdanaFont.bmp");
	SimpleModel* verdanaFont2 = new SimpleModel(vertices, indices, "fonts/VerdanaFont.bmp");
	textManager->addFont("Verdana", verdanaFont);
	double enemyInfoTextPosX = (windowWidth - 185);
	double enemyInfoTextPosY = (windowHeight - 26);
	textManager->addText("EnemyInfo", "Hello there qwerty!", "Verdana", enemyInfoTextPosX, enemyInfoTextPosY, 20, 20, windowWidth, windowHeight);
	/*textManager->addText("EnemyInfo2", "Hello there1!", "Verdana", 0, 0, 20, 30, SCR_WIDTH, SCR_HEIGHT);
	textManager->addText("EnemyInfo3", "Hello there3!", "Verdana", 300, 300, 20, 20, SCR_WIDTH, SCR_HEIGHT);
	textManager->addText("EnemyInfo4", "Hello there4!", "Verdana", 200, 100, 30,20, SCR_WIDTH, SCR_HEIGHT);
	textManager->addText("EnemyInfo5", "Hello there5!", "Verdana", 100, 300, 10,10, SCR_WIDTH, SCR_HEIGHT);*/

	bar = new ProgressBar(windowWidth - 247, 62, 232, 28, windowWidth, windowHeight, 300);
	//animModel = new UnitModel(fileManager->getModelProperty("Player"));
	//animModel->addAnimation("PistolShotAnim", "models/anim_man2.dae");

	terrain = new StaticModel(fileManager->getModelProperty("Terrain"));
	gridShader = new Shader("vsGridShader.glsl", "fsGridShader.glsl", SHADERCOMPLEXITY_FULL);

	//TMP
//	animModel->rotateDegree(90, 2);
//	weapon->rotateDegree(90, 2);
	//camera = new Camera(vec3(-19.85, 20, -6.05));

	return 1;
}
int GameWorld::run() {
	Helper h;
	std::cout << "GameLoop started.\n";

	currentTime = h.getCurrentTime();
	while (!glfwWindowShouldClose(window)){
		lastTime = currentTime;
		currentTime = h.getCurrentTime();
		currentDeltaTime = currentTime - lastTime;

		update(currentDeltaTime);
		collide();
		input();
		draw();
	}
	std::cout << "GameLoop finished.\n";

	return 0;
}
void GameWorld::finish() {
	cout << "here1\n";
	//window
	if (window != NULL) {
		glfwTerminate();
		glfwDestroyWindow(window);
		//delete window;
	}
	cout << "here2\n";
	// file manager
	if(fileManager!=NULL)
		delete fileManager;
	// camera
	if (camera != NULL)
		delete camera;
	// simple models
	if (mouseModel != NULL)
		delete mouseModel;
	if (mouseRay != NULL)
		delete mouseRay;
	if (centerLine != NULL)
		delete centerLine;
	if (pathLine != NULL)
		delete pathLine;
	cout << "here3\n";
	//algorithm classes
	if (rayCasting != NULL)
		delete rayCasting;
	if (rayboxCollision != NULL)
		delete rayboxCollision;
	if (pathfinder != NULL)
		delete pathfinder;
	//map
	if (gameMap != NULL)
		delete gameMap;
	if (ammoManager != NULL)
		delete ammoManager;
	cout << "here4\n";
	//Player
	if (player != NULL)
		delete player;
	cout << "here40\n";
	//drawing
	if (drawingEngine != NULL)
		delete drawingEngine;
	cout << "here41\n";
	//enemies
	for (Enemy* e : enemies)
		delete e;
	cout << "here42\n";
	enemies.clear();
	cout << "here5\n";
	//other
	if (selectedEnemy != NULL)
		delete selectedEnemy;
	if (textManager != NULL)
		delete textManager;
	
	if (simpleCube != NULL)
		delete simpleCube;
	if (enemyInfo != NULL)
		delete enemyInfo;
	if (mainPanel != NULL)
		delete mainPanel;
	if (playerInfo != NULL)
		delete playerInfo;
	if (bar != NULL)
		delete bar;
	cout << "here6\n";
//	
}
//---------------------------

//creating---------
SimpleModel* GameWorld::createGUI(string imagePath, double screenX, double screenY, double width, double height) {
	vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);
	double startScreenX = screenX / (windowWidth / 2.0f) - 1.0f;
	double startScreenY = screenY / (windowHeight / 2.0f) - 1.0f;
	double endScreenX = (screenX + width) / (windowWidth / 2.0f) - 1.0f;
	double endScreenY = (screenY + height) / (windowHeight / 2.0f) - 1.0f;
	double normalizedImgWidth = endScreenX - startScreenX;
	double normalizedImgHeight = endScreenY - startScreenY;
	vec3 min = vec3(startScreenX, startScreenY, 0.0f);
	vec3 max = vec3(startScreenX + normalizedImgWidth, startScreenY + normalizedImgHeight, 0.0f);
	Vertex vert1, vert2, vert3, vert4;
	vector<Vertex> vertices;
	vert1.Position = vec3(min.x, max.y, min.z);
	vert2.Position = vec3(max.x, max.y, min.z);
	vert3.Position = vec3(max.x, min.y, min.z);
	vert4.Position = vec3(min.x, min.y, min.z);
	vert1.TexCoords = vec2(0.0f, 0.0f);
	vert2.TexCoords = vec2(1.0f, 0.0f);
	vert3.TexCoords = vec2(1.0f, 1.0f);
	vert4.TexCoords = vec2(0.0f, 1.0f);
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);
	vertices.push_back(vert4);

	return new SimpleModel(vertices, indices, imagePath);
}
SimpleModel* GameWorld::createSimpleCube(vec3 min, vec3 volume, string texturePath) {
	/*
	V4 <-----V3
	|     |
	V1->-----^V2
	*/
	float width = volume.x;
	float height = volume.y;
	float depth = volume.z;
	Vertex vert1, vert2, vert3, vert4;
	Vertex vert5, vert6, vert7, vert8;
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	vert1.Position = vec3(min.x, min.y, min.z);
	vert2.Position = vec3(min.x + width, min.y, min.z);
	vert3.Position = vec3(min.x + width, min.y + height, min.z);
	vert4.Position = vec3(min.x, min.y + height, min.z);
	vert5.Position = vec3(min.x, min.y, min.z + depth);
	vert6.Position = vec3(min.x + width, min.y, min.z + depth);
	vert7.Position = vec3(min.x + width, min.y + height, min.z + depth);
	vert8.Position = vec3(min.x, min.y + height, min.z + depth);
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);
	vertices.push_back(vert4);
	vertices.push_back(vert5);
	vertices.push_back(vert6);
	vertices.push_back(vert7);
	vertices.push_back(vert8);

	//--------
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);
	//back
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(4);
	//left
	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(0);
	//right
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(2);
	indices.push_back(1);
	//top
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(2);
	indices.push_back(3);
	//bottom
	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(0);

	return new SimpleModel(vertices, indices, texturePath);
}
SimpleModel* GameWorld::createLine(vector<Vertex> vertices, vector<unsigned int> indices, string texturePath) {
	return new SimpleModel(vertices, indices, texturePath);
}
//-----------------

//per frame work-------------------
void GameWorld::update(double timeElapsed) {
	//update units
	player->update(timeElapsed);
	//animModel->update(timeElapsed);

	//update animations
	if(ammo!=NULL)
		ammo->update(timeElapsed);

	//update ammos
	ammoManager->update(timeElapsed);
}
void GameWorld::input() {
	glfwPollEvents();
}
void GameWorld::collide() {
	AlgBoxBoxAABB checker;
	vector<Ammo*> toDelete;
	bool collided = false;

	list<Ammo*> ammos = ammoManager->getAmmos();
	for (auto it = ammos.begin(); it != ammos.end(); ++it) {
		Ammo* ammo = (Ammo*)*it;
		vec3 coordsBody1 = ammo->getModel().getCoords();
		vec3 volumeBody1 = ammo->getModel().getVolume();
		coordsBody1 -= volumeBody1 / 2.0f;

		//is collided with enemy?
		for (Enemy* enemy : enemies) {
			vec3 coordsBody2 = enemy->getModel().getCoords();
			vec3 volumeBody2 = enemy->getModel().getVolume();
			coordsBody2 -= volumeBody2 / 2.0f;
			bool collidedWithEnemy = checker.checkCollide(coordsBody1, coordsBody1 + volumeBody1,
				coordsBody2, coordsBody2 + volumeBody2);
			if (collidedWithEnemy) {
				cout << "Collided with enemy" << endl;
				toDelete.push_back(ammo);
				collided = true;

				enemy->damage(ammo->getProperty().damage);
			}
		}

		//is collided with player?
		vec3 coordsBody2 = player->getModel().getCoords();
		vec3 volumeBody2 = player->getModel().getVolume();
		coordsBody2 -= volumeBody2 / 2.0f;
		bool collidedWithPlayer = checker.checkCollide(coordsBody1, coordsBody1 + volumeBody1,
			coordsBody2, coordsBody2 + volumeBody2);
		if (collidedWithPlayer) {
			cout << "Collided with player" << endl;
			toDelete.push_back(ammo);
			collided = true;
			player->damage(40.0);
			bar->add(-40);
		}
	}

	if (collided) {
		for (Ammo* ammo : toDelete) {
			ammoManager->removeAmmo(ammo);
		}
	}


	//else
	//	cout << "Not collided\n";
}

void GameWorld::draw() {
	//set projection and view with Camera values
	projection = perspective(radians(camera->getZoom()), (float)(windowWidth / windowHeight), 0.1f, 440.0f);
	view = camera->getViewMatrix();

	//clear window
	glClearColor(0.2, 0.75, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//---set main matrices
	drawingEngine->setProjectionMatrix(projection);
	drawingEngine->setViewMatrix(view);
	//-------------------

	//Draw main objects---------------
	//draw terrain
	drawingEngine->drawTerrain(gameMap);
	
	//draw enemies
	drawingEngine->drawEnemies(enemies);
	
	//other map objects
	drawingEngine->drawMapObjects(gameMap->getModels());
	
	//draw player
	drawingEngine->drawPlayer(player);
	//drawing ammos
	drawingEngine->drawAmmos(ammoManager->getAmmos());

	/*if (ammo != NULL) {
		cout << "Drawing ammos...1\n";
		gridShader->use();
		gridShader->setMat4("projection", projection);
		gridShader->setMat4("view", view);
		StaticModel& model = ammo->getModel();
		gridShader->setMat4("model", model.getMatrix());
		model.draw(gridShader);
		Helper h;
		h.showMatrix(model.getMatrix());
		cout << "Drawing ammos...2\n";
	}*/
	//---------------------------------
	//cout << "----------\n";
	

	/*if (true) {
		glfwSwapBuffers(window);
		return;
	}*/

	//Draw GUI--------------
//	drawingEngine->drawGUI(bar->getModel(),false);
	drawingEngine->drawGUI(*bar->getModel(),true);
	drawingEngine->drawGUI(*mainPanel,true);
	drawingEngine->drawGUI(*playerInfo,true);
	//texts
	for (pair<string, Text*> text : textManager->getTexts()) {
		if (textManager->canDraw(text.first))
			drawingEngine->drawText(text.second->getModel());
	}
	//enemy info
	if (selectedEnemy != NULL) {
		drawingEngine->drawGUI(*enemyInfo,true);

		string name = selectedEnemy->getName();
		string maxLife = std::to_string(selectedEnemy->getMaxLife());
		string currLife = std::to_string(selectedEnemy->getCurrLife());
		string maxArmor = std::to_string(selectedEnemy->getMaxArmor());
		string currArmor = std::to_string(selectedEnemy->getCurrArmor());
		string text = name + "\n";
		text += "Life: " + currLife + "/" + maxLife + "\n";
		text += "Armor: " + currArmor + "/" + maxArmor + "\n";
		textManager->updateText("EnemyInfo", text, windowWidth, windowHeight); 
	}
	//mouse
	double screenX, screenY;
	glfwGetCursorPos(window, &screenX, &screenY);
	drawingEngine->drawMouse(mouseModel, screenX, screenY);
	//-----------------------

	//other------------------
	//draw center line
	glLineWidth(8);
	glColor3f(0.3, 0.4, 0.9);
	drawingEngine->drawLine(centerLine);
	glLineWidth(3);
	glColor3f(0.3, 0.4, 0.9);
	//draw path
	drawingEngine->drawLine(pathLine);
	//draw raycasting vector
	drawingEngine->drawLine(mouseRay);
	//-----------------------


	glfwSwapBuffers(window);
}

void draw2() {
	////set projection and view with Camera values
	//projection = perspective(radians(camera->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 440.0f);
	//view = camera->getViewMatrix();

	////clear window
	//glClearColor(0.2, 0.75, 0.4, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	////draw terrain
	//gridShader->use();
	//gridShader->setMat4("projection", projection);
	//gridShader->setMat4("view", view);
	//gridShader->setMat4("model", gameMap->getTerrainMatrix());
	//gridShader->setFloat("drawGrid", (drawGrid ? 1.0f : 0.0f));//draw grid
	//gameMap->getTerrain().draw(gridShader->get());

	////draw animated model
	//animShader->use();
	//animShader->setMat4("projection", projection);
	//animShader->setMat4("view", view);
	//animShader->setMat4("model", animModel->getMatrix());
	//animModel->draw(animShader->get());

	////draw 
	//primitiveScreenShader->use();
	//primitiveScreenShader->setVec3("color", vec3(0.9, 0.2, 0.2));
	//bar->getModel()->draw(primitiveScreenShader->get(), GL_TRIANGLES);

	//mouseShader->use();
	//textManager->draw(mouseShader->get());
	//mainPanel->draw(mouseShader->get(), GL_TRIANGLES);
	//playerInfo->draw(mouseShader->get(), GL_TRIANGLES);

	//if (selectedEnemy != NULL) {
	//	mouseShader->use();
	//	enemyInfo->draw(mouseShader->get(), GL_TRIANGLES);

	//	string name = selectedEnemy->getName();
	//	string maxLife = std::to_string(selectedEnemy->getMaxLife());
	//	string currLife = std::to_string(selectedEnemy->getCurrLife());
	//	string maxArmor = std::to_string(selectedEnemy->getMaxArmor());
	//	string currArmor = std::to_string(selectedEnemy->getCurrArmor());
	//	string text = name + "\n";
	//	text += "Life: "+ currLife + "/" + maxLife + "\n";
	//	text += "Armor: "+currArmor + "/" + maxArmor + "\n";
	//	textManager->updateText("EnemyInfo", text,SCR_WIDTH,SCR_HEIGHT);
	//}

	////draw lines[test]
	//vector<unsigned int> v1;
	//vector<Vertex> v2;
	////drawLines(v2,v1);

	////draw mouse
	//double screenX, screenY;
	//glfwGetCursorPos(window, &screenX, &screenY);
	//drawMouse();

	////draw center line
	///*glLineWidth(8);
	//glColor3f(0.3, 0.4, 0.9);
	//vector<Vertex> vertices;
	//vector<unsigned int> indices;
	//Vertex v;
	//v.Position = vec3(0.0, -100.0, 0.0f);
	//vertices.push_back(v);
	//v.Position = vec3(0.0, 100.0, 0.0f);
	//vertices.push_back(v);
	//indices.push_back(0);
	//indices.push_back(1);
	//drawLines(vertices,indices);
	//glLineWidth(3);
	//glColor3f(0.3, 0.4, 0.9);*/
	////centerLine->draw(lineShader->get(), GL_LINES);

	////drawText("Hello there!", 10,10);


	////draw enemies
	//gridShader->use();
	//gridShader->setMat4("projection", projection);
	//gridShader->setMat4("view", view);
	//for (int a = 0; a < enemies.size(); ++a) {
	//	UnitModel m = enemies.at(a)->getModel(); //
	//	if (m.isSelected())
	//		drawWithOutline(&m,vec3(0.4,0.2,0.9));
	//	else {
	//		gridShader->use();
	//		gridShader->setMat4("projection", projection);
	//		gridShader->setMat4("view", view);
	//		gridShader->setMat4("model", m.getMatrix());
	//		m.draw(gridShader->get());
	//	}
	//}

	////other map objects
	//gridShader->setFloat("drawGrid", 0.0f);//dont draw grid
	//for (StaticModel& m : gameMap->getModels()) {
	//	if (m.isSelected())
	//		drawWithOutline(&m);
	//	else {
	//		gridShader->use();
	//		gridShader->setMat4("projection", projection);
	//		gridShader->setMat4("view", view);
	//		gridShader->setMat4("model", m.getMatrix());
	//		m.draw(gridShader->get());
	//	}
	//}

	////draw player
	//if (player->getModel().isSelected()) {
	//	vec3 color = vec3(0.9f, 0.2f, 0.1f);
	//	drawWithOutline(&player->getModel(),color);
	//}
	//else {
	//	gridShader->use();
	//	gridShader->setMat4("projection", projection);
	//	gridShader->setMat4("view", view);
	//	gridShader->setMat4("model", player->getModel().getMatrix());
	//	player->getModel().draw(gridShader->get());
	//}

	////drawing ammos
	//gridShader->use();
	//gridShader->setMat4("projection", projection);
	//gridShader->setMat4("view", view);
	//ammoManager->draw(gridShader->get());

	////draw raycasting vector
	//lineShader->use();
	//lineShader->setMat4("projection", projection);
	//lineShader->setMat4("view", view);
	//lineShader->setMat4("model", mat4(1.0f));
	//clickedLine->draw(lineShader->get(), GL_LINE_STRIP);

	////draw path
	//lineShader->use();
	//lineShader->setMat4("projection", projection);
	//lineShader->setMat4("view", view);
	//lineShader->setMat4("model", clickedMatrix);
	//pathLine->draw(lineShader->get(), GL_LINE_STRIP);


	//glfwSwapBuffers(window);
}
//--------------------------------
