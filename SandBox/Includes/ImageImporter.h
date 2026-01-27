#pragma once
#include <filesystem>
class Bitmap;

class ImageImporter
{
public:
	bool LoadFromWIC(const std::filesystem::path& path, Bitmap* texture);
	bool LoadFromDDS(const std::filesystem::path& path, Bitmap* texture);
};
