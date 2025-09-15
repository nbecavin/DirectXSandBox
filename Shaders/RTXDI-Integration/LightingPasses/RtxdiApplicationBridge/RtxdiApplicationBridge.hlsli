/***************************************************************************
 # Copyright (c) 2020-2023, NVIDIA CORPORATION.  All rights reserved.
 #
 # NVIDIA CORPORATION and its licensors retain all intellectual property
 # and proprietary rights in and to this software, related documentation
 # and any modifications thereto.  Any use, reproduction, disclosure or
 # distribution of this software and related documentation without an express
 # license agreement from NVIDIA CORPORATION is strictly prohibited.
 **************************************************************************/

/*
This header file is the bridge between the RTXDI resampling functions
and the application resources and parts of shader functionality.

The RTXDI SDK provides the resampling logic, and the application provides
other necessary aspects:
    - Material BRDF evaluation;
    - Ray tracing and transparent/alpha-tested material processing;
    - Light sampling functions and emission profiles.

The structures and functions that are necessary for SDK operation
start with the RAB_ prefix (for RTXDI-Application Bridge).

All structures defined here are opaque for the SDK, meaning that
it makes no assumptions about their contents, they are just passed
between the bridge functions.
*/

#ifndef RTXDI_APPLICATION_BRIDGE_HLSLI
#define RTXDI_APPLICATION_BRIDGE_HLSLI

#include "../../ShaderParameters.h"
#include "../../SceneGeometry.hlsli"

#include "RAB_Buffers.hlsli"

#include "RAB_LightInfo.hlsli"
#include "RAB_LightSample.hlsli"
#include "RAB_LightSampling.hlsli"
#include "RAB_Material.hlsli"
#include "RAB_RandomSamplerState.hlsli"
#include "RAB_RayPayload.hlsli"
#include "RAB_RTShaders.hlsli"
#include "RAB_SpatialHelpers.hlsli"
#include "RAB_Surface.hlsli"
#include "RAB_VisibilityTest.hlsli"

#endif // RTXDI_APPLICATION_BRIDGE_HLSLI
