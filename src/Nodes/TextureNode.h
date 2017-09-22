//
//  TextureNode.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once

#include "GraphSettings.h"
#include "Node.h"
#include "NodeTypes.h"

#ifdef IMGUI_USE_AUTO_BINDING
class TextureNode : public ImGui::Node {
protected:
    typedef Node Base;  //Base Class
    typedef TextureNode ThisClass;
    TextureNode() : Base() {}
    virtual ~TextureNode() {if (textureID) {ImImpl_FreeTexture(textureID);}}
    static const int TYPE = MNT_TEXTURE_NODE;
    static const int TextBufferSize =
#   ifndef NO_IMGUIFILESYSTEM
    ImGuiFs::MAX_PATH_BYTES;
#   else
    2049;
#   endif
    
    ImTextureID textureID;
    char imagePath[TextBufferSize];				// field 1 (= the only one that is copied/serialized/handled by the Node)
    char lastValidImagePath[TextBufferSize];    // The path for which "textureID" was created
    bool startBrowseDialogNextFrame;
#   ifndef NO_IMGUIFILESYSTEM
    ImGuiFs::Dialog dlg;
#   endif //NO_IMGUIFILESYSTEM
    
    virtual const char* getTooltip() const {return "TextureNode tooltip.";}
    virtual const char* getInfo() const {return "TextureNode info.\n\nThis is supposed to display some info about this node.";}
    virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut,ImU32& defaultTitleBgColorOut,float& defaultTitleBgColorGradientOut) const {
        // [Optional Override] customize Node Title Colors [default values: 0,0,-1.f => do not override == use default values from the Style()]
        defaultTitleTextColorOut = IM_COL32(220,220,220,255);defaultTitleBgColorOut = IM_COL32(105,105,0,255);defaultTitleBgColorGradientOut = -1.f;
    }
    
public:
    
    // create (main method of this class):
    static ThisClass* Create(const ImVec2& pos) {
        // 1) allocation
        // MANDATORY (NodeGraphEditor::~NodeGraphEditor() will delete these with ImGui::MemFree(...))
        // MANDATORY even with blank ctrs.  Reason: ImVector does not call ctrs/dctrs on items.
        ThisClass* node = (ThisClass*) ImGui::MemAlloc(sizeof(ThisClass));
        IM_PLACEMENT_NEW(node) ThisClass();
        
        // 2) main init
        node->init("TextureNode",pos,"","r;g;b;a",TYPE);
        node->baseWidthOverride = 150.f;    // (optional) default base node width is 120.f;
        
        // 3) init fields ( this uses the node->fields variable; otherwise we should have overridden other virtual methods (to render and serialize) )
        FieldInfo* f=NULL;
#	ifndef NO_IMGUIFILESYSTEM
        f=&node->fields.addFieldTextEditAndBrowseButton(&node->imagePath[0],TextBufferSize,"Image Path:","A valid image path: press RETURN to validate or browse manually.",ImGuiInputTextFlags_EnterReturnsTrue,(void*) node);
#	else	//NO_IMGUIFILESYSTEM
        f=&node->fields.addFieldTextEdit(&node->imagePath[0],TextBufferSize,"Image Path:","A valid image path: press RETURN to validate or browse manually.",ImGuiInputTextFlags_EnterReturnsTrue,(void*) node);
#	endif //NO_IMGUIFILESYSTEM
        f->editedFieldDelegate = &ThisClass::StaticEditFieldCallback;   // we set an "edited callback" to this node field. It is fired soon (as opposed to other "outer" edited callbacks), but we have chosen: ImGuiInputTextFlags_EnterReturnsTrue.
        node->startBrowseDialogNextFrame = false;
        
        // 4) set (or load) field values
        node->textureID = 0;
        node->imagePath[0] = node->lastValidImagePath[0] = '\0';
        
        return node;
    }
    
    // helper casts:
    inline static ThisClass* Cast(Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    inline static const ThisClass* Cast(const Node* n) {return Node::Cast<ThisClass>(n,TYPE);}
    
protected:
    bool render(float nodeWidth)   {
        const bool changed = Base::render(nodeWidth);
#	ifndef NO_IMGUIFILESYSTEM
        const char* filePath = dlg.chooseFileDialog(startBrowseDialogNextFrame,dlg.getLastDirectory(),".jpg;.jpeg;.png;.gif;.tga;.bmp");
        if (strlen(filePath)>0) {
            //fprintf(stderr,"Browsed..: %s\n",filePath);
            strcpy(imagePath,filePath);
            processPath(imagePath);
        }
#	endif //NO_IMGUIFILESYSTEM
        startBrowseDialogNextFrame = false;
        //----------------------------------------------------
        // draw textureID:
        ImGui::Image(reinterpret_cast<void*>(textureID),ImVec2(nodeWidth,nodeWidth));
        //----------------------------------------------------
        return changed;
    }
    
    void processPath(const char* filePath)  {
        if (!filePath || strcmp(filePath,lastValidImagePath)==0) return;
        if (!ValidateImagePath(filePath)) return;
        if (textureID) {ImImpl_FreeTexture(textureID);}
        textureID = ImImpl_LoadTexture(filePath);
        if (textureID) strcpy(lastValidImagePath,filePath);
    }
    
    // When the node is loaded from file or copied from another node, only the text field (="imagePath") is copied, so we must recreate "textureID":
    void onCopied() {
        processPath(imagePath);
    }
    void onLoaded() {
        processPath(imagePath);
    }
    //bool canBeCopied() const {return false;}	// Just for testing... TO REMOVE!
    
    void onEditField(FieldInfo& /*f*/,int widgetIndex) {
        //fprintf(stderr,"TextureNode::onEditField(\"%s\",%i);\n",f.label,widgetIndex);
        if (widgetIndex==1)         startBrowseDialogNextFrame = true;  // browsing button pressed
        else if (widgetIndex==0)    processPath(imagePath);             // text edited (= "return" pressed in out case)
    }
    static void StaticEditFieldCallback(FieldInfo& f,int widgetIndex) {
        reinterpret_cast<ThisClass*>(f.userData)->onEditField(f,widgetIndex);
    }
    
    static bool ValidateImagePath(const char* path) {
        if (!path || strlen(path)==0) return false;
        
        // TODO: check extension
        
        FILE* f = ImFileOpen(path,"rb");
        if (f) {fclose(f);f=NULL;return true;}
        
        return false;
    }
    
};
#endif //IMGUI_USE_AUTO_BINDING
