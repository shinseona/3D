#pragma once
#ifndef DRAWINGMANAGER_H_
#define DRAWINGMANAGER_H_

#include <vector>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/glm.hpp>
#include "Enemy.h"
#include "Player.h"
#include "Map.h"
#include "SimpleModel.h"
#include "UnitModel.h"
#include "AnimatedModel.h"
#include "Shader.h"
#include "Observer.h"
#include "Observable.h"
using std::vector;
using std::pair;

class DrawingEngine : public Observer {
private:
	mat4 projectionMatrix;
	mat4 viewMatrix;
	//shaders
	Shader *ourShader, *gridShader, *lineShader, *primitiveScreenShader, *animShader, *outlineAnimShader;
	Shader* outlineShader, *colorShader, *mouseShader;

	void drawWithOutline(AnimatedModel* model, vec3 color = vec3(0.1, 0.1, 0.9)) {
		glEnable(GL_STENCIL_TEST);//enable stencil test
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//if depth&stencip passed -> replace value
		glStencilFunc(GL_ALWAYS, 1, 0xFF);//always write 1 
		glStencilMask(0xFF);//allow write to stencil
		gridShader->use();
		animShader->setMat4("projection", projectionMatrix);
		animShader->setMat4("view", viewMatrix);
		animShader->setMat4("model", model->getMatrix());
		model->draw(animShader->get());
		//drawing border
		glStencilMask(0xFF);//here we need to put 0x00 mask to restrict writing in stencil, but it working only with 0xFF...
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//draw only diference between these models(scalling difference)
		glDisable(GL_DEPTH_TEST);
		//glStencilMask(0);
		mat4 m = glm::scale(model->getMatrix(), glm::vec3(1.08f, 1.001f, 1.08f));
		outlineAnimShader->use();
		outlineAnimShader->setMat4("projection", projectionMatrix);
		outlineAnimShader->setMat4("view", viewMatrix);
		outlineAnimShader->setMat4("model", m);
		outlineAnimShader->setVec3("color", color);
		model->draw(outlineAnimShader->get());

		//clearing
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//set default operation(in any case keep current values)
		glStencilFunc(GL_ALWAYS, 0, 0x0);//aw
		glClear(GL_STENCIL_BUFFER_BIT);
		glClearStencil(0);//clearing stencil, writing with 0 values
		glDisable(GL_STENCIL_TEST);//disable stencil buffer
		glStencilMask(0x00);//restrict writing in stencil buffer
		glEnable(GL_DEPTH_TEST);//enabline depth testing
		glDepthMask(0xFF);
		//glDepthMask(0xFF);
	}
	void drawWithOutline(UnitModel& model, vec3 color = vec3(0.0f,0.0f,0.0f)) {
		glEnable(GL_STENCIL_TEST);//enable stencil test

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//if depth&stencip passed -> replace value
												  //push to stencil
		glStencilFunc(GL_ALWAYS, 1, 0xFF);//always write 1 
		glStencilMask(0xFF);//allow write to stencil
		animShader->use();
		animShader->setMat4("projection", projectionMatrix);
		animShader->setMat4("view", viewMatrix);
		animShader->setMat4("model", model.getMatrix());
		model.draw(animShader->get());
		//drawing border
		glStencilMask(0xFF);//here we need to put 0x00 mask to restrict writing in stencil, but it working only with 0xFF...
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//draw only diference between these models(scalling difference)
		glDisable(GL_DEPTH_TEST);
		//glStencilMask(0);
		mat4 m = glm::scale(model.getMatrix(), glm::vec3(1.08f, 1.001f, 1.08f));
		outlineAnimShader->use();
		outlineAnimShader->setMat4("projection", projectionMatrix);
		outlineAnimShader->setMat4("view", viewMatrix);
		outlineAnimShader->setMat4("model", m);
		outlineAnimShader->setVec3("color", color);
		model.draw(outlineAnimShader->get());
		//clearing
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//set default operation(in any case keep current values)
		glStencilFunc(GL_ALWAYS, 0, 0x0);//aw
		glClear(GL_STENCIL_BUFFER_BIT);
		glClearStencil(0);//clearing stencil, writing with 0 values
		glDisable(GL_STENCIL_TEST);//disable stencil buffer
		glStencilMask(0x00);//restrict writing in stencil buffer
		glEnable(GL_DEPTH_TEST);//enabline depth testing
		glDepthMask(0xFF);
	}
	void drawWithOutline(StaticModel* model, vec3 color = vec3(0.1, 0.1, 0.9)) {
		glEnable(GL_STENCIL_TEST);//enable stencil test

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//if depth&stencip passed -> replace value
												  //push to stencil
		glStencilFunc(GL_ALWAYS, 1, 0xFF);//always write 1 
		glStencilMask(0xFF);//allow write to stencil
		gridShader->use();
		gridShader->setMat4("projection", projectionMatrix);
		gridShader->setMat4("view", viewMatrix);
		gridShader->setMat4("model", model->getMatrix());
		//	model->draw(gridShader->get());

			//drawing border
		glStencilMask(0xFF);//here we need to put 0x00 mask to restrict writing in stencil, but it working only with 0xFF...
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//draw only diference between these models(scalling difference)
		glDisable(GL_DEPTH_TEST);
		//glStencilMask(0);
		mat4 m = glm::scale(model->getMatrix(), glm::vec3(1.08f, 1.001f, 1.08f));
		outlineShader->use();
		outlineShader->setMat4("projection", projectionMatrix);
		outlineShader->setMat4("view", viewMatrix);
		outlineShader->setMat4("model", m);
		outlineShader->setVec3("color", color);
		//	model->draw(outlineShader->get());

			//clearing
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//set default operation(in any case keep current values)
		glStencilFunc(GL_ALWAYS, 0, 0x0);//aw
		glClear(GL_STENCIL_BUFFER_BIT);
		glClearStencil(0);//clearing stencil, writing with 0 values
		glDisable(GL_STENCIL_TEST);//disable stencil buffer
		glStencilMask(0x00);//restrict writing in stencil buffer
		glEnable(GL_DEPTH_TEST);//enable depth testing
		glDepthMask(0xFF);
	}
public:
	DrawingEngine(double windowWidth, double windowHeight) : Observer("DrawingEngine") {
		vector<unsigned int> indices;
		vector<Vertex> vertices;
		Vertex v;

		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
		cout << "DrawingManager: Initializing...";
		//compile shaders
		ourShader = new Shader("vsTextureShader.glsl", "fsTextureShader.glsl", SHADERCOMPLEXITY_FULL);
		gridShader = new Shader("vsGridShader.glsl", "fsGridShader.glsl", SHADERCOMPLEXITY_FULL);
		lineShader = new  Shader("vsLineShader.glsl", "fsLineShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		primitiveScreenShader = new  Shader("vsPrimitiveScreenShader.glsl", "fsPrimitiveScreenShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		animShader = new  Shader("vsAnimShader.glsl", "fsAnimShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		outlineAnimShader = new  Shader("vsOutlineAnimShader.glsl", "fsOutlineAnimShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		outlineShader = new  Shader("vsOutlineShader.glsl", "fsOutlineShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		colorShader = new  Shader("vsColorShader.glsl", "fsColorShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		mouseShader = new  Shader("vsMouseShader.glsl", "fsMouseShader.glsl", SHADERCOMPLEXITY_SIMPLE);
		cout << "DrawingManager: Initialized.";
	}
	~DrawingEngine() {
		delete ourShader;
		delete gridShader;
		delete lineShader;
		delete primitiveScreenShader;
		delete animShader;
		delete outlineAnimShader;
		delete outlineShader;
		delete colorShader;
		delete mouseShader;
	}

	//drawing
	void drawEnemies(const vector<Enemy*>& enemies) {
		bool existsOutlined = true;
		for (int a = 0; a < enemies.size(); ++a) {
			UnitModel& m = enemies.at(a)->getModel();
			if (m.isSelected()) {
				drawWithOutline(m, vec3(0.4, 0.2, 0.9));
				existsOutlined = true;
			}
			else {
				if (existsOutlined) {
					animShader->use();
					animShader->setMat4("projection", projectionMatrix);
					animShader->setMat4("view", viewMatrix);
					existsOutlined = false;
				}
				animShader->setMat4("model", m.getMatrix());
				m.draw(animShader->get());
			}
		}
	}
	void drawPlayer(Player* const player) {
		if (player->getModel().isSelected()) {
			vec3 color = vec3(0.9f, 0.2f, 0.1f);
			drawWithOutline(player->getModel(), color);
		}
		else {
			animShader->use();
			animShader->setMat4("projection", projectionMatrix);
			animShader->setMat4("view", viewMatrix);
			animShader->setMat4("model", player->getModel().getMatrix());
			player->getModel().draw(animShader->get());
		}
	}
	void drawTerrain(Map*& const map, bool drawGrid = false) {
		gridShader->use();
		gridShader->setMat4("projection", projectionMatrix);
		gridShader->setMat4("view", viewMatrix);
		gridShader->setMat4("model", map->getTerrainMatrix());
		gridShader->setFloat("drawGrid", (drawGrid ? 1.0f : 0.0f));
		StaticModel*& terrain = map->getTerrain();
		map->getTerrain()->draw(gridShader);
	}
	void drawMapObjects(vector<StaticModel*>& objects) {
		for (StaticModel*& object : objects) {
			if (object->isSelected())
				drawWithOutline(object);
			else {
				gridShader->use();
				gridShader->setMat4("projection", projectionMatrix);
				gridShader->setMat4("view", viewMatrix);
				gridShader->setMat4("model", object->getMatrix());
				object->draw(gridShader);
			}
		}
	}
	void drawMouse(SimpleModel* mouseModel, double screenX, double screenY) {
		screenX = screenX / (windowWidth / 2.0f) - 1.0f;
		screenY = screenY / (windowHeight / 2.0f) - 1.0f;
		screenY *= -1;//inverse Y coord
		vec3 min = vec3(screenX - 0.05, screenY - 0.05, 0.0f);
		vec3 max = vec3(screenX + 0.05f, screenY + 0.05f, 0.0f);
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

		mouseModel->updateVertices(vertices);
		mouseShader->use();
		mouseModel->draw(mouseShader->get(), GL_TRIANGLES);
	}
	void drawCube(SimpleModel* cube) {
		colorShader->use();
		colorShader->setMat4("projection", projectionMatrix);
		colorShader->setMat4("view", viewMatrix);
		colorShader->setMat4("model", mat4(1.0));
		colorShader->setVec4("aColor", vec4(0.4, 0.3f, 0.2, 0.5f));
		cube->draw(colorShader->get(), GL_TRIANGLES);
	}
	void drawLine(SimpleModel*& line) {
		lineShader->use();
		lineShader->setMat4("projection", projectionMatrix);
		lineShader->setMat4("view", viewMatrix);
		lineShader->setMat4("model", mat4(1.0f));
		line->draw(lineShader->get(), GL_LINE_STRIP);
		//GLuint vao, vbo, ibo;
		//glGenVertexArrays(1, &vao);
		//glGenBuffers(1, &vbo);
		//glGenBuffers(1, &ibo);

		//glBindVertexArray(vao);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		////vertex coord
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		////glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size());
		//glDrawElements(GL_LINE_STRIP, (GLsizei)indices.size(), GL_UNSIGNED_INT, (void*)0);

		//glBindVertexArray(0);
	}
	void drawText(SimpleModel* text) {
		mouseShader->use();
		text->draw(mouseShader->get(), GL_TRIANGLES);
	}
	void drawGUI(SimpleModel& model, bool textured) {
		if (textured) {
			mouseShader->use();
			model.draw(mouseShader->get(), GL_TRIANGLES);
		}
		else {
			primitiveScreenShader->use();
			primitiveScreenShader->setVec3("color", vec3(0.9, 0.2, 0.2));
			model.draw(primitiveScreenShader->get(), GL_TRIANGLES);
		}
	}
	void drawAmmos(list<Ammo*>& ammos) {
		gridShader->use();
		gridShader->setMat4("projection", projectionMatrix);
		gridShader->setMat4("view", viewMatrix);
		for (Ammo*& ammo : ammos) {
			StaticModel& model = ammo->getModel();
		//	model.rotateDegree(180.0f,2);
			gridShader->setMat4("model", model.getMatrix());
			model.draw(gridShader);
		}
	}

	//other
	void setProjectionMatrix(mat4 projection) {
		this->projectionMatrix = projection;
	}
	void setViewMatrix(mat4 view) {
		this->viewMatrix = view;
	}
};
#endif