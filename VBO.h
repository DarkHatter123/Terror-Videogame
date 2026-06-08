#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<vector>
#include"Mesh.h"  // Añadir para conocer el tipo Vertex

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;

	// Constructor that generates a Vertex Buffer Object and links it to vertices (old version)
	VBO(GLfloat* vertices, GLsizeiptr size);

	// NEW: Constructor that accepts vector of Vertex
	VBO(std::vector<Vertex>& vertices);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif