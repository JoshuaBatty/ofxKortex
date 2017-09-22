//
//  NodeTypes.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once 

enum MyNodeTypes {
    MNT_COLOR_NODE = 0,
    MNT_COMBINE_NODE,
    MNT_COMMENT_NODE,
    MNT_COMPLEX_NODE,
#   ifdef IMGUI_USE_AUTO_BINDING
    MNT_TEXTURE_NODE,
#   endif
    MNT_OUTPUT_NODE,    // One problem here when adding new values is backward compatibility with old saved files: they rely on the previously used int values (it should be OK only if we append new values at the end).
    MNT_COUNT
};
// used in the "add Node" menu (and optionally as node title names)
static const char* MyNodeTypeNames[MNT_COUNT] = {"Color","Combine","Comment","Complex"
#						ifdef IMGUI_USE_AUTO_BINDING
    ,"Texture"
#						endif
    ,"Output"
};

