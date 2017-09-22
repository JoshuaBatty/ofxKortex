//
//  ofxKortex.cpp
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#include "ofxKortex.h"


//--------------------------------------------------------------
void TestNodeGraphEditor()  {
    
    static ImGui::NodeGraphEditor nge;
    if (nge.isInited())	{
        // This adds entries to the "add node" context menu
        nge.registerNodeTypes(MyNodeTypeNames,MNT_COUNT,MyNodeFactory,NULL,-1); // last 2 args can be used to add only a subset of nodes (or to sort their order inside the context menu)
        // The line above can be replaced by the following two lines, if we want to use only an active subset of the available node types:
        //const int optionalNodeTypesToUse[] = {MNT_COMPLEX_NODE,MNT_COMMENT_NODE,MNT_OUTPUT_NODE};
        //nge.registerNodeTypes(MyNodeTypeNames,MNT_COUNT,MyNodeFactory,optionalNodeTypesToUse,sizeof(optionalNodeTypesToUse)/sizeof(optionalNodeTypesToUse[0]));
        nge.registerNodeTypeMaxAllowedInstances(MNT_OUTPUT_NODE,1); // Here we set the max number of allowed instances of the output node (1)
        
        // Optional: starting nodes and links (TODO: load from file instead):-----------
        ImGui::Node* colorNode = nge.addNode(MNT_COLOR_NODE,ImVec2(40,50));
        ImGui::Node* complexNode =  nge.addNode(MNT_COMPLEX_NODE,ImVec2(40,150));
        ImGui::Node* combineNode =  nge.addNode(MNT_COMBINE_NODE,ImVec2(275,80)); // optionally use e.g.: ImGui::CombineNode::Cast(combineNode)->fraction = 0.8f;
        ImGui::Node* outputNode =  nge.addNode(MNT_OUTPUT_NODE,ImVec2(520,140));
        // Return values can be NULL (if node types are not registered or their instance limit has been already reached).
        //nge.overrideNodeName(combineNode,"CombineNodeCustomName");  // Test only (to remove)
        //nge.overrideNodeInputSlots(combineNode,"in1;in2;in3;in4");  // Test only (to remove)
        //ImU32 bg = IM_COL32(0,128,0,255);nge.overrideNodeTitleBarColors(combineNode,NULL,&bg,NULL);  // Test only (to remove)
        //nge.overrideNodeTitleBarColors(complexNode,NULL,&bg,NULL);  // Test only (to remove)
        // addLink(...) should be robust enough to handle NULL nodes, so we don't check it.
        nge.addLink(colorNode, 0, combineNode, 0);
        nge.addLink(complexNode, 1, combineNode, 1);
        nge.addLink(complexNode, 0, outputNode, 1);
        nge.addLink(combineNode, 0, outputNode, 0);
        //-------------------------------------------------------------------------------
        //nge.load("nodeGraphEditor.nge");  // Please note than if the saved graph has nodes out of our active subset, they will be displayed as usual (it's not clear what should be done in this case: hope that's good enough, it's a user's mistake).
        //-------------------------------------------------------------------------------
        nge.show_style_editor = true;
        nge.show_load_save_buttons = true;
        // optional load the style (for all the editors: better call it in InitGL()):
        //NodeGraphEditor::Style::Load(NodeGraphEditor::GetStyle(),"nodeGraphEditor.style");
        //--------------------------------------------------------------------------------
    }
    nge.render();
    
}
