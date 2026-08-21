#include "Renderer.h"
#include <iostream>
#include "IndexBuffer.h"
#include "Object.h"
#include "Shader.h"
#include "VertexArray.h"
#include "util/Mesh.h"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "):" << function
			<< " " << file << ": line " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	// DrawCall
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawMesh(util::Mesh& mesh, const Shader& shader) const
{
	shader.Bind();
	mesh.Draw();
}

void Renderer::DrawObject(Object& object, const Shader& shader) const
{
	shader.Bind();
	object.DrawObject();
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}
