#ifndef RAB_MATERIAL_HLSLI
#define RAB_MATERIAL_HLSLI

static const float kMinRoughness = 0.05f;

struct RAB_Material
{
    float3 diffuseAlbedo;
    float3 specularF0;
    float roughness;
};

RAB_Material RAB_EmptyMaterial()
{
    RAB_Material material = (RAB_Material)0;

    return material;
}

float3 GetDiffuseAlbedo(RAB_Material material)
{
    return material.diffuseAlbedo;
}

float3 GetSpecularF0(RAB_Material material)
{
    return material.specularF0;
}

float GetRoughness(RAB_Material material)
{
    return material.roughness;
}

RAB_Material GetGBufferMaterial(
    int2 pixelPosition,
    PlanarViewConstants view,
    Texture2D<uint> diffuseAlbedoTexture, 
    Texture2D<uint> specularRoughTexture)
{
    RAB_Material material = RAB_EmptyMaterial();

    if (any(pixelPosition >= view.viewportSize))
        return material;

    material.diffuseAlbedo = Unpack_R11G11B10_UFLOAT(diffuseAlbedoTexture[pixelPosition]).rgb;
    float4 specularRough = Unpack_R8G8B8A8_Gamma_UFLOAT(specularRoughTexture[pixelPosition]);
    material.roughness = specularRough.a;
    material.specularF0 = specularRough.rgb;

    return material;
}

RAB_Material RAB_GetGBufferMaterial(
    int2 pixelPosition,
    bool previousFrame)
{
    if(previousFrame)
    {
        return GetGBufferMaterial(
            pixelPosition,
            g_Const.prevView,
            t_PrevGBufferDiffuseAlbedo,
            t_PrevGBufferSpecularRough);
    }
    else
    {
        return GetGBufferMaterial(
            pixelPosition,
            g_Const.view,
            t_GBufferDiffuseAlbedo,
            t_GBufferSpecularRough);
    }
}

// Compares the materials of two surfaces, returns true if the surfaces
// are similar enough that we can share the light reservoirs between them.
// If unsure, just return true.
bool RAB_AreMaterialsSimilar(RAB_Material a, RAB_Material b)
{
    const float roughnessThreshold = 0.5;
    const float reflectivityThreshold = 0.25;
    const float albedoThreshold = 0.25;

    if (!RTXDI_CompareRelativeDifference(a.roughness, b.roughness, roughnessThreshold))
        return false;

    if (abs(calcLuminance(a.specularF0) - calcLuminance(b.specularF0)) > reflectivityThreshold)
        return false;
    
    if (abs(calcLuminance(a.diffuseAlbedo) - calcLuminance(b.diffuseAlbedo)) > albedoThreshold)
        return false;

    return true;
}

#endif // RAB_MATERIAL_HLSLI