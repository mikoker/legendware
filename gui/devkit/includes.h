#pragma once

#ifndef INCLUDES_H
#define INCLUDES_H

#include <iostream>
#include <string>
#include <random>
#include <algorithm>

#include <windows.h>
#include <tlhelp32.h>

/* d3d9 */
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/* third-party */
#include "../third-party/imgui/imgui.h"
#include "../third-party/imgui/imgui_internal.h"

#include "../third-party/imgui/impl/imgui_impl_dx9.h"
#include "../third-party/imgui/impl/imgui_impl_win32.h"

#include "utils/utils.h"
#include "graphics/d3d9.h"

#endif // !INCLUDES_H