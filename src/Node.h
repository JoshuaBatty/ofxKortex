//
//  Node.hpp
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once 

#include "GraphSettings.h"
#include "NodeUtils.h"
#include "FieldInfo.h"

namespace ImGui	{

    class Node
    {
    public:
        virtual ~Node() {}
        mutable void* user_ptr;
        mutable int userID;
        inline const char* getName() const {return Name;}
        inline int getType() const {return typeID;}
        inline int getNumInputSlots() const {return InputsCount;}
        inline int getNumOutputSlots() const {return OutputsCount;}
        inline void setOpen(bool flag) {isOpen=flag;}
        
    protected:
        FieldInfoVector fields; // I guess you can just skip these at all and implement virtual methods... but it was supposed to be useful...
        // virtual methods
        virtual bool render(float nodeWidth) // should return "true" if the node has been edited and its values modified (to fire "edited callbacks")
        {
            bool nodeEdited = false;
            for (int i=0,isz=fields.size();i<isz;i++)   {
                FieldInfo& f = fields[i];
                nodeEdited|=f.render(nodeWidth);
            }
            return nodeEdited;
        }
        virtual const char* getTooltip() const {return NULL;}
        virtual const char* getInfo() const {return NULL;}
        virtual void onEdited() {}  // called (a few seconds) after the node has been edited
        virtual void onCopied() {}  // called after the node fileds has been copied from another node
        virtual void onLoaded() {}  // called after the node has been loaded (=deserialized from file)
        virtual bool canBeCopied() const {return true;}
        // called on a class basis to set the default colors different from the ones defined in NodeGraphEditor::GetStyle()
        // [but on an instance basis these colors can still be overridden using the protected fields defined below, or better NodeGraphEditor::overrideNodeTitleBarColors(...)]
        virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut,ImU32& defaultTitleBgColorOut,float& defaultTitleBgColorGradientOut) const {
            defaultTitleTextColorOut = defaultTitleBgColorOut = 0;  // 0 -> use values defined in NodeGraphEditor::GetStyle()
            defaultTitleBgColorGradientOut = -1;                    // -1 -> use value defined in NodeGraphEditor::GetStyle()
        }

        
        ImVec2  Pos, Size;
        char    Name[IMGUINODE_MAX_NAME_LENGTH];
        int     InputsCount, OutputsCount;
        char    InputNames[IMGUINODE_MAX_INPUT_SLOTS][IMGUINODE_MAX_SLOT_NAME_LENGTH];
        char    OutputNames[IMGUINODE_MAX_OUTPUT_SLOTS][IMGUINODE_MAX_SLOT_NAME_LENGTH];
        mutable float startEditingTime; // used for Node Editing Callbacks
        mutable bool isOpen;
        mutable bool isSelected;
        int typeID;
        float baseWidthOverride;
        bool mustOverrideName,mustOverrideInputSlots,mustOverrideOutputSlots;
        ImU32 overrideTitleTextColor,overrideTitleBgColor;  // 0 -> don't override
        float overrideTitleBgColorGradient;                 //-1 -> don't override
        bool isInEditingMode;
        
        Node() : Pos(0,0),Size(0,0),isSelected(false),baseWidthOverride(-1),mustOverrideName(false),mustOverrideInputSlots(false),mustOverrideOutputSlots(false),overrideTitleTextColor(0),overrideTitleBgColor(0),overrideTitleBgColorGradient(-1.f),isInEditingMode(false),parentNodeGraphEditor(NULL) {}
        IMGUI_API void init(const char* name, const ImVec2& pos,const char* inputSlotNamesSeparatedBySemicolons=NULL,const char* outputSlotNamesSeparatedBySemicolons=NULL,int _nodeTypeID=0/*,float currentWindowFontScale=-1.f*/);
        
        inline ImVec2 GetInputSlotPos(int slot_no,float currentFontWindowScale=1.f) const   { return ImVec2(Pos.x*currentFontWindowScale,           Pos.y*currentFontWindowScale + Size.y * ((float)slot_no+1) / ((float)InputsCount+1)); }
        inline ImVec2 GetOutputSlotPos(int slot_no,float currentFontWindowScale=1.f) const  { return ImVec2(Pos.x*currentFontWindowScale + Size.x,  Pos.y*currentFontWindowScale + Size.y * ((float)slot_no+1) / ((float)OutputsCount+1)); }
        inline const ImVec2 GetPos(float currentFontWindowScale=1.f) const {return ImVec2(Pos.x*currentFontWindowScale,Pos.y*currentFontWindowScale);}
        
        friend struct NodeLink;
        friend class NodeGraphEditor;
        
        // Helper static methods to simplify code of the derived classes
        // casts:
        template <typename T> inline static T* Cast(Node* n,int TYPE) {return ((n && n->getType()==TYPE) ? static_cast<T*>(n) : NULL);}
        template <typename T> inline static const T* Cast(const Node* n,int TYPE) {return ((n && n->getType()==TYPE) ? static_cast<const T*>(n) : NULL);}
        
    private:
        class NodeGraphEditor* parentNodeGraphEditor;
        
    protected:
        NodeGraphEditor& getNodeGraphEditor() {IM_ASSERT(parentNodeGraphEditor);return *parentNodeGraphEditor;}
        const NodeGraphEditor& getNodeGraphEditor() const {IM_ASSERT(parentNodeGraphEditor);return *parentNodeGraphEditor;}
    };

    //--------------------------------------------
    struct NodeLink
    {
        Node*  InputNode;   int InputSlot;
        Node*  OutputNode;  int OutputSlot;
        
        NodeLink(Node* input_node, int input_slot, Node* output_node, int output_slot) {
            InputNode = input_node; InputSlot = input_slot;
            OutputNode = output_node; OutputSlot = output_slot;
        }
        
        friend struct NodeGraphEditor;
    };
    
}	// namespace ImGui

