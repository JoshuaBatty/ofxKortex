//
//  OutputNode.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once

#include "Node.h"
#include "NodeTypes.h"

class OutputNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef OutputNode ThisClass;
    OutputNode() : Base() {}
    static const int TYPE = MNT_OUTPUT_NODE;
    
    // No field values in this class
    
    virtual const char* getTooltip() const {return "OutputNode tooltip.";}
    virtual const char* getInfo() const {return "OutputNode info.\n\nThis is supposed to display some info about this node.";}
    virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut,ImU32& defaultTitleBgColorOut,float& defaultTitleBgColorGradientOut) const {
        // [Optional Override] customize Node Title Colors [default values: 0,0,-1.f => do not override == use default values from the Style()]
        defaultTitleTextColorOut = IM_COL32(230,180,180,255);defaultTitleBgColorOut = IM_COL32(40,55,55,200);defaultTitleBgColorGradientOut = 0.025f;
    }
    virtual bool canBeCopied() const {return false;}
    
public:
    
    // create:
    static ThisClass* Create(const ImVec2& pos) {
        // 1) allocation
        // MANDATORY (NodeGraphEditor::~NodeGraphEditor() will delete these with ImGui::MemFree(...))
        // MANDATORY even with blank ctrs. Reason: ImVector does not call ctrs/dctrs on items.
        ThisClass* node = (ThisClass*) ImGui::MemAlloc(sizeof(ThisClass));IM_PLACEMENT_NEW (node) ThisClass();
        
        // 2) main init
        node->init("OutputNode",pos,"ch1;ch2;ch3;ch4","",TYPE);
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        
        // 4) set (or load) field values
        
        return node;
    }
    
    // casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    
protected:
    bool render(float /*nodeWidth*/)   {
        ImGui::Text("There can be a single\ninstance of this class.\nTry and see if it's true!");
        return false;
    }
};
