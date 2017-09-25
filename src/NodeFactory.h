//
//  NodeFactory.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once
#include "GraphSettings.h"
#include "ColorNode.h"
#include "CombineNode.h"
#include "CommentNode.h"
#include "ComplexNode.h"
#include "OutputNode.h"
#include "TextureNode.h"

#include "LfoNode.h"

static ImGui::Node* MyNodeFactory(int nt,const ImVec2& pos,const ImGui::NodeGraphEditor& /*nge*/) {
    switch (nt) {
        case MNT_COLOR_NODE: return ColorNode::Create(pos);
        case MNT_COMBINE_NODE: return CombineNode::Create(pos);
        case MNT_COMMENT_NODE: return CommentNode::Create(pos);
        case MNT_COMPLEX_NODE: return ComplexNode::Create(pos);
        case MNT_LFO_NODE: return LfoNode::Create(pos);
        case MNT_OUTPUT_NODE: return OutputNode::Create(pos);
#   ifdef IMGUI_USE_AUTO_BINDING
        case MNT_TEXTURE_NODE: return TextureNode::Create(pos);
#   endif //IMGUI_USE_AUTO_BINDING
        default:
        IM_ASSERT(true);    // Missing node type creation
        return NULL;
    }
    return NULL;
}
