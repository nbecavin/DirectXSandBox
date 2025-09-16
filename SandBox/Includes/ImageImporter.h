#pragma once
#include <filesystem>

class ImageImporter
{
public:
	bool LoadFromWIC(const std::filesystem::path& path, Bitmap* texture);
	bool LoadFromDDS(const std::filesystem::path& path, Bitmap* texture);
};
