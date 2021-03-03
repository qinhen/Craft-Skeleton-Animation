#pragma once
#define CRAFT_ENGINE_USING_WORLD_SPACE_RIGHT_HAND
#define CRAFT_ENGINE_USING_DEVICE_SPACE_LEFT_HAND

float gCurrentTimeInSeconds = 0.0f;


#include "CraftEngine/gui/Gui.h"
using namespace CraftEngine;
using math::vec2;
using math::vec3;
using math::vec4;
using math::mat2;
using math::mat3;
using math::mat4;
using gui::Rect;
using gui::Size;
using gui::Point;
using gui::String;

#include "CraftEngine/graphics/Camera.h"
using graphics::Camera;
Camera gCamera;


#include "CraftEngine/graphics/Scene.h"
graphics::Scene gScene;
#include "RenderAssetsManager.h"
RenderAssetsManager gRenderAssetsManager;


float gAnimationSpeed = 100.0f;
int gAnimationIndex = 0;


int gModelID;
int gInstanceID;

bool gUsingAnimationShader = true;