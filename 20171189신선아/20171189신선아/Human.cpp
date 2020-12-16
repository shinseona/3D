#include "Human.h"
#include "FileManager.h"
#include "Controller.h"

void Human::init()
{
	FileManager* filemgr = FileManager::instance();

	filemgr->loadObj(
		this,
		"human.obj",
		"skin.bmp",
		"20171189_vs.shader",
		"20171189_fs.shader"
	);

	this->setPosition(0, -1.8f, 0);
	this->setScale(0.1, 0.1, 0.1);
}

void Human::render()
{
	glUseProgram(this->programID);

	Controller* controller = Controller::instance();

	controller->MatrixInput();

	glm::mat4 ProjectionMatrix = controller->getProjectionMatrix();
	glm::mat4 ViewMatrix = controller->getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0f);

	glm::mat4 MoveObject = glm::mat4(1.0f);
	MoveObject = glm::translate(MoveObject, this->ObjPosition);
	glm::mat4 scale = glm::mat4(1.0f);
	scale = glm::scale(scale, this->Objscale);

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * MoveObject * ModelMatrix* scale;

	glUniformMatrix4fv(this->MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->Texture);
	glUniform1i(this->TextureID, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Human::update()
{

}

void Human::setPosition(float x, float y, float z)
{
	ObjPosition = glm::vec3(x, y, z);
}
void Human::setScale(float x, float y, float z)
{
	Objscale = glm::vec3(x, y, z);
}

void Human::shutDown()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(MatrixID);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
}