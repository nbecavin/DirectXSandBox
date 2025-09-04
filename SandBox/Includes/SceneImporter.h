#pragma once

class SceneImporter
{
public:
	void LoadScene(std::string& filepath);
	void LoadScene(const char* filepath) { LoadScene(std::string(filepath)); }
};
