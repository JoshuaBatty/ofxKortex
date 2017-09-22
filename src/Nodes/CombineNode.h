//
//  CombineNode.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once 

#include "Node.h"
#include "NodeTypes.h"

class CombineNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef CombineNode ThisClass;
    CombineNode() : Base() {}
    static const int TYPE = MNT_COMBINE_NODE;
    
    float fraction;
    
    virtual const char* getTooltip() const {return "CombineNode tooltip.";}
    virtual const char* getInfo() const {return "CombineNode info.\n\nThis is supposed to display some info about this node.";}
    /*virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut,ImU32& defaultTitleBgColorOut,float& defaultTitleBgColorGradientOut) const {
     // [Optional Override] customize Node Title Colors [default values: 0,0,-1.f => do not override == use default values from the Style()]
     defaultTitleTextColorOut = IM_COL32(220,220,220,255);defaultTitleBgColorOut = IM_COL32(0,75,0,255);defaultTitleBgColorGradientOut = -1.f;
     }*/
    
public:
    
    // create:
    static ThisClass* Create(const ImVec2& pos) {
        // 1) allocation
        // MANDATORY (NodeGraphEditor::~NodeGraphEditor() will delete these with ImGui::MemFree(...))
        // MANDATORY even with blank ctrs. Reason: ImVector does not call ctrs/dctrs on items.
        ThisClass* node = (ThisClass*) ImGui::MemAlloc(sizeof(ThisClass));IM_PLACEMENT_NEW(node) ThisClass();
        
        // 2) main init
        node->init("CombineNode",pos,"in1;in2","out",TYPE);
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        node->fields.addField(&node->fraction,1,"Fraction","Fraction of in1 that is mixed with in2",2,0,1);
        
        // 4) set (or load) field values
        node->fraction = 0.5f;
        
        return node;
    }
    
    // casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    
    
};
