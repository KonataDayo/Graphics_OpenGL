#pragma once
#include <glew.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

class Shader;
class IndexBuffer;
class VertexArray;

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
private:

public:
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
};
