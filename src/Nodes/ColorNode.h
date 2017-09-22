//
//  ColorNode.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once
#include "Node.h"
#include "NodeTypes.h"

class ColorNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef ColorNode ThisClass;
    ColorNode() : Base() {}
    static const int TYPE = MNT_COLOR_NODE;
    
    ImVec4 Color;       // field
    
    // Support static method for enumIndex (the signature is the same used by ImGui::Combo(...))
    static bool GetTextFromEnumIndex(void* ,int value,const char** pTxt) {
        if (!pTxt) return false;
        static const char* values[] = {"APPLE","LEMON","ORANGE"};
        static int numValues = (int)(sizeof(values)/sizeof(values[0]));
        if (value>=0 && value<numValues) *pTxt = values[value];
        else *pTxt = "UNKNOWN";
        return true;
    }
    
    virtual const char* getTooltip() const {return "ColorNode tooltip.";}
    virtual const char* getInfo() const {return "ColorNode info.\n\nThis is supposed to display some info about this node.";}
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
        ThisClass* node = (ThisClass*) ImGui::MemAlloc(sizeof(ThisClass));IM_PLACEMENT_NEW (node) ThisClass();
        
        // 2) main init
        node->init("ColorNode",pos,"","r;g;b;a",TYPE);
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        node->fields.addFieldColor(&node->Color.x,true,"Color","color with alpha");
        
        // 4) set (or load) field values
        node->Color = ImColor(255,255,0,255);
        
        return node;
    }
    
    
    // casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
};
