#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "stb_image.h"
#include "Renderer.h"

static unsigned int loadTexture(const std::string& filepath)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	int height, width, component;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &component, 0);
	if (data)
	{
		unsigned int format = 0;
		if (component == 1)
			format = GL_RED;
		if (component == 3)
			format = GL_RGB;
		if (component == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);
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
		std::cout << "Texture failed to load" << std::endl;
		stbi_image_free(data);
	}
	return texture;
}

static unsigned int loadHDRTexture(const std::string& filepath)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	int height, width, component;

	// 使用 stbi_loadf 加载 HDR 图像，返回 float 数据
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(filepath.c_str(), &width, &height, &component, 0);

	if (data)
	{
		// 确定纹理格式
		unsigned int format = 0;
		unsigned int internalFormat = 0;

		if (component == 1) {
			format = GL_RED;
			internalFormat = GL_R32F;  // 单通道浮点
		}
		else if (component == 3) {
			format = GL_RGB;
			internalFormat = GL_RGB32F;  // 三通道浮点
		}
		else if (component == 4) {
			format = GL_RGBA;
			internalFormat = GL_RGBA32F;  // 四通道浮点
		}

		glBindTexture(GL_TEXTURE_2D, texture);

		// 使用 GL_FLOAT 作为数据类型，internalFormat 使用浮点格式
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
			format, GL_FLOAT, data);

		// HDR 环境贴图通常不需要 mipmap，但如果你需要可以保留
		// glGenerateMipmap(GL_TEXTURE_2D);

		// HDR 纹理的纹理参数设置
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 对于 HDR 环境贴图，考虑使用 GL_TEXTURE_WRAP_S/T 为 GL_CLAMP_TO_EDGE
		// 避免边缘出现接缝

		stbi_image_free(data);

		std::cout << "HDR texture loaded successfully: " << filepath << std::endl;
		std::cout << "Size: " << width << "x" << height << ", Channels: " << component << std::endl;
	}
	else
	{
		std::cout << "HDR texture failed to load: " << filepath << std::endl;
		// stbi_loadf 失败时不需要调用 stbi_image_free
		// 因为 data 已经是 nullptr
	}

	return texture;
}

static unsigned int loadCubemap(const std::vector<std::string>& faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels = 0;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};
