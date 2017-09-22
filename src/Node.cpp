//
//  Node.cpp
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#include "Node.h"

using namespace ImGui;

void Node::init(const char *name, const ImVec2 &pos, const char *inputSlotNamesSeparatedBySemicolons, const char *outputSlotNamesSeparatedBySemicolons, int _nodeTypeID/*,float currentWindowFontScale*/) {
    /*if (currentWindowFontScale<0)   {
     ImGuiWindow* window = ImGui::GetCurrentWindow();
     currentWindowFontScale = window ? window->FontWindowScale  : 0.f;
     }*/
    strncpy(Name, name, IMGUINODE_MAX_NAME_LENGTH); Name[IMGUINODE_MAX_NAME_LENGTH-1] = '\0'; Pos = /*currentWindowFontScale==0.f?*/pos/*:pos/currentWindowFontScale*/;
    InputsCount = ProcessSlotNamesSeparatedBySemicolons<IMGUINODE_MAX_INPUT_SLOTS>(inputSlotNamesSeparatedBySemicolons,InputNames);
    OutputsCount = ProcessSlotNamesSeparatedBySemicolons<IMGUINODE_MAX_OUTPUT_SLOTS>(outputSlotNamesSeparatedBySemicolons,OutputNames);
    typeID = _nodeTypeID;
    user_ptr = NULL;userID=-1;
    startEditingTime = 0;
    isOpen = true;
}
