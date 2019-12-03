#pragma once

class Texture
{
private:
	void CheckError();
public:
	Texture(char* imagePath);
	~Texture();
	void Bind();
	unsigned image;
	unsigned _textureID;
};
