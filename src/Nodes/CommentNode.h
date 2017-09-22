//
//  CommentNode.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once

#include "Node.h"
#include "NodeTypes.h"

class CommentNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef CommentNode ThisClass;
    CommentNode() : Base() {}
    static const int TYPE = MNT_COMMENT_NODE;
    static const int TextBufferSize = 128;
    
    char comment[TextBufferSize];			    // field 1
    char comment2[TextBufferSize];			    // field 2
    char comment3[TextBufferSize];			    // field 3
    char comment4[TextBufferSize];			    // field 4
    bool flag;                                  // field 5
    
    virtual const char* getTooltip() const {return "CommentNode tooltip.";}
    virtual const char* getInfo() const {return "CommentNode info.\n\nThis is supposed to display some info about this node.";}
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
        node->init("CommentNode",pos,"","",TYPE);
        node->baseWidthOverride = 200.f;    // (optional) default base node width is 120.f;
        
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        node->fields.addFieldTextEdit(		&node->comment[0],TextBufferSize,"Single Line","A single line editable field",ImGuiInputTextFlags_EnterReturnsTrue);
        node->fields.addFieldTextEditMultiline(&node->comment2[0],TextBufferSize,"Multi Line","A multi line editable field",ImGuiInputTextFlags_AllowTabInput,50);
        node->fields.addFieldTextEditMultiline(&node->comment3[0],TextBufferSize,"Multi Line 2","A multi line read-only field",ImGuiInputTextFlags_ReadOnly,50);
        node->fields.addFieldTextWrapped(      &node->comment4[0],TextBufferSize,"Text Wrapped ReadOnly","A text wrapped field");
        node->fields.addField(&node->flag,"Flag","A boolean field");
        
        // 4) set (or load) field values
        strcpy(node->comment,"Initial Text Line.");
        strcpy(node->comment2,"Initial Text Multiline.");
        static const char* tiger = "Tiger, tiger, burning bright\nIn the forests of the night,\nWhat immortal hand or eye\nCould frame thy fearful symmetry?";
        strncpy(node->comment3,tiger,TextBufferSize);
        static const char* txtWrapped = "I hope this text gets wrapped gracefully. But I'm not sure about it.";
        strncpy(node->comment4,txtWrapped,TextBufferSize);
        node->flag = true;
        
        return node;
    }
    
    // helper casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
};
