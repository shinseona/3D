#include "Ground.h"
#include "FileManager.h"
#include "Controller.h"

void Ground::init()
{
	FileManager* filemgr = FileManager::instance();

	filemgr->loadObj(
		this,
		"ground.obj",
		"grass.bmp",
		"20171189_vs.shader",
		"20171189_fs.shader"
	);

	this->setScale(6, 0.3, 0.1);
}

void Ground::render()
{
	glUseProgram(this->programID);

	Controller* controller = Controller::instance();

	controller->MatrixInput();

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewMatrix = glm::lookAt(
		glm::vec3(0, 1, 10), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 3.5f, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 ModelMatrix = glm::mat4(1.0f);

	glm::mat4 scale = glm::mat4(1.0f);
	scale = glm::scale(scale, this->Objscale);

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * scale;

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

void Ground::update()
{

}

void Ground::setPosition(float x, float y, float z)
{
	ObjPosition = glm::vec3(x, y, z);
}
void Ground::setScale(float x, float y, float z)
{
	Objscale = glm::vec3(x, y, z);
}

void Ground::shutDown()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(MatrixID);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
}