//
//  LfoNode.h
//  ofxKortex-example
//
//  Created by Joshua Batty on 25/9/17.
//
//

#pragma once

#include "Node.h"
#include "NodeTypes.h"

class LfoNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef LfoNode ThisClass;
    LfoNode() : Base() {}
    static const int TYPE = MNT_LFO_NODE;
    
    int enumIndex;      // field 1
    float speed;
    float amp;
    //float Value[3];     // field 2
    //ImVec4 Color;       // field 3
    
    // Support static method for enumIndex (the signature is the same used by ImGui::Combo(...))
    static bool GetTextFromEnumIndex(void* ,int value,const char** pTxt) {
        if (!pTxt) return false;
        static const char* values[] = {"SINE","TRIANGLE","SAWTOOTH","SQUARE"};
        static int numValues = (int)(sizeof(values)/sizeof(values[0]));
        if (value>=0 && value<numValues) *pTxt = values[value];
        else *pTxt = "UNKNOWN";
        return true;
    }
    
    virtual const char* getTooltip() const {return "LfoNode tooltip.";}
    virtual const char* getInfo() const {return "LfoNode info.\n\nThis is supposed to display some info about this node.";}
    virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut,ImU32& defaultTitleBgColorOut,float& defaultTitleBgColorGradientOut) const {
        // [Optional Override] customize Node Title Colors [default values: 0,0,-1.f => do not override == use default values from the Style()]
        defaultTitleTextColorOut = IM_COL32(220,220,220,255);defaultTitleBgColorOut = IM_COL32(125,35,0,255);defaultTitleBgColorGradientOut = -1.f;
    }
    
public:
    
    // create:
    static ThisClass* Create(const ImVec2& pos) {
        // 1) allocation
        // MANDATORY (NodeGraphEditor::~NodeGraphEditor() will delete these with ImGui::MemFree(...))
        // MANDATORY even with blank ctrs.  Reason: ImVector does not call ctrs/dctrs on items.
        ThisClass* node = (ThisClass*) ImGui::MemAlloc(sizeof(ThisClass));IM_PLACEMENT_NEW(node) ThisClass();
        
        // 2) main init
//        node->init("LfoNode",pos,"in1;in2;in3","out1;out2",TYPE);
        node->init("LfoNode",pos,"type;speed;amp;reset","out",TYPE);
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        node->fields.addFieldEnum(&node->enumIndex,4,&GetTextFromEnumIndex,"Type","Choose lfo waveform");
        node->fields.addField(&node->speed,1,"Speed","Frequency of LFO",3,0,10);
        node->fields.addField(&node->amp,1,"Amp","Amplitude of LFO",3,0,1);

        //node->fields.addField(&node->Value[0],3,"Angles","Three floats that are stored in radiant units internally",2,0,360,NULL,true);
        //node->fields.addFieldColor(&node->Color.x,true,"Color","color with alpha");
        
        // addFieldEnum(...) now has all the 3 overloads ImGui::Combo(...) has.
        // addFieldEnum(...) [1] (item_count + external callback)
        // addFieldEnum(...) [2] (items_count + item_names)
        //static const char* FruitNames[3] = {"APPLE","LEMON","ORANGE"};
        //node->fields.addFieldEnum(&node->enumIndex,3,FruitNames,"Fruit","Choose your favourite");
        // addFieldEnum(...) [3] (zero_separated_item_names)
        //node->fields.addFieldEnum(&node->enumIndex,"APPLE\0LEMON\0ORANGE\0\0","Fruit","Choose your favourite");
        
        
        // 4) set (or load) field values
        node->enumIndex = 0;
        node->speed = 1.0;
        node->amp = 1.0;
        //node->Value[0] = 0;node->Value[1] = 3.14f; node->Value[2] = 4.68f;
        //node->Color = ImColor(126,200,124,230);
        
        return node;
    }
    
    // helper casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
};
