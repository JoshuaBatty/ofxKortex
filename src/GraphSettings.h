//
//  GraphSettings.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once

//- Common Code For All Addons needed just to ease inclusion as separate files in user code
#include <imgui.h>
#undef IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_PLACEMENT_NEW
#undef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <stdlib.h> // qsort
//---------------------------------------------------

#if !defined(alloca)
#ifdef _WIN32
#include <malloc.h>     // alloca
#elif (defined(__FreeBSD__) || defined(FreeBSD_kernel) || defined(__DragonFly__)) && !defined(__GLIBC__)
#include <stdlib.h>     // alloca. FreeBSD uses stdlib.h unless GLIBC
#else
#include <alloca.h>     // alloca
#endif
#endif

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
//static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
//static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }


#if (defined(_MSC_VER) && !defined(snprintf))
#   define snprintf _snprintf
#endif //(defined(_MSC_VER) && !defined(snprintf))
//---------------------------------------------------

// some constants
#   ifndef IMGUINODE_MAX_NAME_LENGTH
#   define IMGUINODE_MAX_NAME_LENGTH 32
#   endif //IMGUINODE_MAX_NAME_LENGTH
#   ifndef IMGUINODE_MAX_INPUT_SLOTS
#   define IMGUINODE_MAX_INPUT_SLOTS 8
#   endif //IMGUINODE_MAX_INPUT_SLOTS
#   ifndef IMGUINODE_MAX_OUTPUT_SLOTS
#   define IMGUINODE_MAX_OUTPUT_SLOTS 8
#   endif //IMGUINODE_MAX_OUTPUT_SLOTS
#   ifndef IMGUINODE_MAX_SLOT_NAME_LENGTH
#   define IMGUINODE_MAX_SLOT_NAME_LENGTH 12
#   endif //IMGUINODE_MAX_SLOT_NAME_LENGTH
#   ifndef IMGUIFIELDINFO_MAX_LABEL_LENGTH
#   define IMGUIFIELDINFO_MAX_LABEL_LENGTH 32
#   endif //IMGUIFIELDINFO_MAX_LABEL_LENGTH
#   ifndef IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH
#   define IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH 64
#   endif //IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH
//---------------------------------------------------

// Helpers macros to generate 32-bits encoded colors
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
//---------------------------------------------------
