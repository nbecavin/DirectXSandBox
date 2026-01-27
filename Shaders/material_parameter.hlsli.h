#pragma once

struct MaterialParameter
{
	Vec3f albedo;
	Vec3f normal;
	float roughness;
	float metallic;
	float opacity;
	float ao;
	Vec3f emission;

	U32 albedo_map;
	U32 normal_map;
	U32 roughnes_map;
	U32 metal_map;
};
