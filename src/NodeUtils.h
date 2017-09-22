//
//  NodeDrawFunctions.h
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once 
#include "GraphSettings.h"

namespace NGE_Draw {
    // This methods are a subset of the ones already present in imguihelper.h, copied here to enable stand alone usage. Scoped for better isolation (in case of multi .cpp chaining and/or usage in amalgamation engines).
    static void ImDrawListPathFillAndStroke(ImDrawList *dl, const ImU32 &fillColor, const ImU32 &strokeColor, bool strokeClosed, float strokeThickness, bool antiAliased)    {
        if (!dl) return;
        if ((fillColor & IM_COL32_A_MASK) != 0) dl->AddConvexPolyFilled(dl->_Path.Data, dl->_Path.Size, fillColor, antiAliased);
        if ((strokeColor& IM_COL32_A_MASK)!= 0 && strokeThickness>0) dl->AddPolyline(dl->_Path.Data, dl->_Path.Size, strokeColor, strokeClosed, strokeThickness, antiAliased);
        dl->PathClear();
    }
    static void ImDrawListPathArcTo(ImDrawList *dl, const ImVec2 &centre, const ImVec2 &radii, float amin, float amax, int num_segments)  {
        if (!dl) return;
        if (radii.x == 0.0f || radii.y==0) dl->_Path.push_back(centre);
        dl->_Path.reserve(dl->_Path.Size + (num_segments + 1));
        for (int i = 0; i <= num_segments; i++)
        {
            const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
            dl->_Path.push_back(ImVec2(centre.x + cosf(a) * radii.x, centre.y + sinf(a) * radii.y));
        }
    }
    static void ImDrawListAddCircle(ImDrawList *dl, const ImVec2 &centre, float radius, const ImU32 &fillColor, const ImU32 &strokeColor, int num_segments, float strokeThickness, bool antiAliased)   {
        if (!dl) return;
        const ImVec2 radii(radius,radius);
        const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
        ImDrawListPathArcTo(dl,centre, radii, 0.0f, a_max, num_segments);
        ImDrawListPathFillAndStroke(dl,fillColor,strokeColor,true,strokeThickness,antiAliased);
    }
    
    inline static void GetVerticalGradientTopAndBottomColors(ImU32 c,float fillColorGradientDeltaIn0_05,ImU32& tc,ImU32& bc)  {
        if (fillColorGradientDeltaIn0_05==0) {tc=bc=c;return;}
        
        static ImU32 cacheColorIn=0;static float cacheGradientIn=0.f;static ImU32 cacheTopColorOut=0;static ImU32 cacheBottomColorOut=0;
        if (cacheColorIn==c && cacheGradientIn==fillColorGradientDeltaIn0_05)   {tc=cacheTopColorOut;bc=cacheBottomColorOut;return;}
        cacheColorIn=c;cacheGradientIn=fillColorGradientDeltaIn0_05;
        
        const bool negative = (fillColorGradientDeltaIn0_05<0);
        if (negative) fillColorGradientDeltaIn0_05=-fillColorGradientDeltaIn0_05;
        if (fillColorGradientDeltaIn0_05>0.5f) fillColorGradientDeltaIn0_05=0.5f;
        
        
        // New code:
        //#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
        const int fcgi = fillColorGradientDeltaIn0_05*255.0f;
        const int R = (unsigned char) (c>>IM_COL32_R_SHIFT);    // The cast should reset upper bits (as far as I hope)
        const int G = (unsigned char) (c>>IM_COL32_G_SHIFT);
        const int B = (unsigned char) (c>>IM_COL32_B_SHIFT);
        const int A = (unsigned char) (c>>IM_COL32_A_SHIFT);
        
        int r = R+fcgi, g = G+fcgi, b = B+fcgi;
        if (r>255) r=255;if (g>255) g=255;if (b>255) b=255;
        if (negative) bc = IM_COL32(r,g,b,A); else tc = IM_COL32(r,g,b,A);
        
        r = R-fcgi; g = G-fcgi; b = B-fcgi;
        if (r<0) r=0;if (g<0) g=0;if (b<0) b=0;
        if (negative) tc = IM_COL32(r,g,b,A); else bc = IM_COL32(r,g,b,A);
        
        // Old legacy code (to remove)... [However here we lerp alpha too...]
        /*// Can we do it without the double conversion ImU32 -> ImVec4 -> ImU32 ?
         const ImVec4 cf = ColorConvertU32ToFloat4(c);
         ImVec4 tmp(cf.x+fillColorGradientDeltaIn0_05,cf.y+fillColorGradientDeltaIn0_05,cf.z+fillColorGradientDeltaIn0_05,cf.w+fillColorGradientDeltaIn0_05);
         if (tmp.x>1.f) tmp.x=1.f;if (tmp.y>1.f) tmp.y=1.f;if (tmp.z>1.f) tmp.z=1.f;if (tmp.w>1.f) tmp.w=1.f;
         if (negative) bc = ColorConvertFloat4ToU32(tmp); else tc = ColorConvertFloat4ToU32(tmp);
         tmp=ImVec4(cf.x-fillColorGradientDeltaIn0_05,cf.y-fillColorGradientDeltaIn0_05,cf.z-fillColorGradientDeltaIn0_05,cf.w-fillColorGradientDeltaIn0_05);
         if (tmp.x<0.f) tmp.x=0.f;if (tmp.y<0.f) tmp.y=0.f;if (tmp.z<0.f) tmp.z=0.f;if (tmp.w<0.f) tmp.w=0.f;
         if (negative) tc = ColorConvertFloat4ToU32(tmp); else bc = ColorConvertFloat4ToU32(tmp);*/
        
        cacheTopColorOut=tc;cacheBottomColorOut=bc;
    }
    inline static ImU32 GetVerticalGradient(const ImVec4& ct,const ImVec4& cb,float DH,float H)    {
        IM_ASSERT(H!=0);
        const float fa = DH/H;
        const float fc = (1.f-fa);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(
                                              ct.x * fc + cb.x * fa,
                                              ct.y * fc + cb.y * fa,
                                              ct.z * fc + cb.z * fa,
                                              ct.w * fc + cb.w * fa)
                                       );
    }
    static void ImDrawListAddConvexPolyFilledWithVerticalGradient(ImDrawList *dl, const ImVec2 *points, const int points_count, ImU32 colTop, ImU32 colBot, bool anti_aliased,float miny,float maxy)
    {
        if (!dl) return;
        if (colTop==colBot)  {
            dl->AddConvexPolyFilled(points,points_count,colTop,anti_aliased);
            return;
        }
        const ImVec2 uv = GImGui->FontTexUvWhitePixel;
        anti_aliased &= GImGui->Style.AntiAliasedShapes;
        //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug
        
        int height=0;
        if (miny<=0 || maxy<=0) {
            const float max_float = 999999999999999999.f;
            miny=max_float;maxy=-max_float;
            for (int i = 0; i < points_count; i++) {
                const float h = points[i].y;
                if (h < miny) miny = h;
                else if (h > maxy) maxy = h;
            }
        }
        height = maxy-miny;
        const ImVec4 colTopf = ImGui::ColorConvertU32ToFloat4(colTop);
        const ImVec4 colBotf = ImGui::ColorConvertU32ToFloat4(colBot);
        
        
        if (anti_aliased)
        {
            // Anti-aliased Fill
            const float AA_SIZE = 1.0f;
            
            const ImVec4 colTransTopf(colTopf.x,colTopf.y,colTopf.z,0.f);
            const ImVec4 colTransBotf(colBotf.x,colBotf.y,colBotf.z,0.f);
            const int idx_count = (points_count-2)*3 + points_count*6;
            const int vtx_count = (points_count*2);
            dl->PrimReserve(idx_count, vtx_count);
            
            // Add indexes for fill
            unsigned int vtx_inner_idx = dl->_VtxCurrentIdx;
            unsigned int vtx_outer_idx = dl->_VtxCurrentIdx+1;
            for (int i = 2; i < points_count; i++)
            {
                dl->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); dl->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+((i-1)<<1)); dl->_IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx+(i<<1));
                dl->_IdxWritePtr += 3;
            }
            
            // Compute normals
            ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
            for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
            {
                const ImVec2& p0 = points[i0];
                const ImVec2& p1 = points[i1];
                ImVec2 diff = p1 - p0;
                diff *= ImInvLength(diff, 1.0f);
                temp_normals[i0].x = diff.y;
                temp_normals[i0].y = -diff.x;
            }
            
            for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
            {
                // Average normals
                const ImVec2& n0 = temp_normals[i0];
                const ImVec2& n1 = temp_normals[i1];
                ImVec2 dm = (n0 + n1) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                dm *= AA_SIZE * 0.5f;
                
                // Add vertices
                //_VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
                //_VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
                dl->_VtxWritePtr[0].pos = (points[i1] - dm); dl->_VtxWritePtr[0].uv = uv; dl->_VtxWritePtr[0].col = GetVerticalGradient(colTopf,colBotf,points[i1].y-miny,height);        // Inner
                dl->_VtxWritePtr[1].pos = (points[i1] + dm); dl->_VtxWritePtr[1].uv = uv; dl->_VtxWritePtr[1].col = GetVerticalGradient(colTransTopf,colTransBotf,points[i1].y-miny,height);  // Outer
                dl->_VtxWritePtr += 2;
                
                // Add indexes for fringes
                dl->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx+(i1<<1)); dl->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+(i0<<1)); dl->_IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx+(i0<<1));
                dl->_IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx+(i0<<1)); dl->_IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx+(i1<<1)); dl->_IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx+(i1<<1));
                dl->_IdxWritePtr += 6;
            }
            dl->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
        }
        else
        {
            // Non Anti-aliased Fill
            const int idx_count = (points_count-2)*3;
            const int vtx_count = points_count;
            dl->PrimReserve(idx_count, vtx_count);
            for (int i = 0; i < vtx_count; i++)
            {
                //_VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                dl->_VtxWritePtr[0].pos = points[i]; dl->_VtxWritePtr[0].uv = uv; dl->_VtxWritePtr[0].col = GetVerticalGradient(colTopf,colBotf,points[i].y-miny,height);
                dl->_VtxWritePtr++;
            }
            for (int i = 2; i < points_count; i++)
            {
                dl->_IdxWritePtr[0] = (ImDrawIdx)(dl->_VtxCurrentIdx); dl->_IdxWritePtr[1] = (ImDrawIdx)(dl->_VtxCurrentIdx+i-1); dl->_IdxWritePtr[2] = (ImDrawIdx)(dl->_VtxCurrentIdx+i);
                dl->_IdxWritePtr += 3;
            }
            dl->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
        }
    }
    static void ImDrawListPathFillWithVerticalGradientAndStroke(ImDrawList *dl, const ImU32 &fillColorTop, const ImU32 &fillColorBottom, const ImU32 &strokeColor, bool strokeClosed = false, float strokeThickness = 1.0f, bool antiAliased = true, float miny=-1.f, float maxy=-1.f)    {
        if (!dl) return;
        if (fillColorTop==fillColorBottom) dl->AddConvexPolyFilled(dl->_Path.Data,dl->_Path.Size, fillColorTop, antiAliased);
        else if ((fillColorTop & IM_COL32_A_MASK) != 0 || (fillColorBottom & IM_COL32_A_MASK) != 0) ImDrawListAddConvexPolyFilledWithVerticalGradient(dl, dl->_Path.Data, dl->_Path.Size, fillColorTop, fillColorBottom, antiAliased,miny,maxy);
        if ((strokeColor& IM_COL32_A_MASK)!= 0 && strokeThickness>0) dl->AddPolyline(dl->_Path.Data, dl->_Path.Size, strokeColor, strokeClosed, strokeThickness, antiAliased);
        dl->PathClear();
    }
    static void ImDrawListAddRectWithVerticalGradient(ImDrawList *dl, const ImVec2 &a, const ImVec2 &b, const ImU32 &fillColorTop, const ImU32 &fillColorBottom, const ImU32 &strokeColor, float rounding = 0.0f, int rounding_corners = 0x0F,float strokeThickness = 1.0f,bool antiAliased = true) {
        if (!dl || (((fillColorTop & IM_COL32_A_MASK) == 0) && ((fillColorBottom & IM_COL32_A_MASK) == 0) && ((strokeColor & IM_COL32_A_MASK) == 0)))  return;
        if (rounding==0.f || rounding_corners==0) {
            dl->AddRectFilledMultiColor(a,b,fillColorTop,fillColorTop,fillColorBottom,fillColorBottom); // Huge speedup!
            if ((strokeColor& IM_COL32_A_MASK)!= 0 && strokeThickness>0.f) {
                dl->PathRect(a, b, rounding, rounding_corners);
                dl->AddPolyline(dl->_Path.Data, dl->_Path.Size, strokeColor, true, strokeThickness, antiAliased);
                dl->PathClear();
            }
        }
        else    {
            dl->PathRect(a, b, rounding, rounding_corners);
            ImDrawListPathFillWithVerticalGradientAndStroke(dl,fillColorTop,fillColorBottom,strokeColor,true,strokeThickness,antiAliased,a.y,b.y);
        }
    }
    static void ImDrawListAddRectWithVerticalGradient(ImDrawList *dl, const ImVec2 &a, const ImVec2 &b, const ImU32 &fillColor, float fillColorGradientDeltaIn0_05, const ImU32 &strokeColor, float rounding = 0.0f, int rounding_corners = 0x0F,float strokeThickness = 1.0f,bool antiAliased = true)   {
        ImU32 fillColorTop,fillColorBottom;GetVerticalGradientTopAndBottomColors(fillColor,fillColorGradientDeltaIn0_05,fillColorTop,fillColorBottom);
        ImDrawListAddRectWithVerticalGradient(dl,a,b,fillColorTop,fillColorBottom,strokeColor,rounding,rounding_corners,strokeThickness,antiAliased);
    }
} // namespace


template<typename T> inline static T GetRadiansToDegs() {
    static T factor = T(180)/(3.1415926535897932384626433832795029);
    return factor;
}
template<typename T> inline static T GetDegsToRadians() {
    static T factor = T(3.1415926535897932384626433832795029)/T(180);
    return factor;
}

static bool UseSlidersInsteadOfDragControls = false;

inline static float ImVec2Dot(const ImVec2& S1,const ImVec2& S2) {return (S1.x*S2.x+S1.y*S2.y);}

// Based on: http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
inline static float GetSquaredDistancePointSegment(const ImVec2& P,const ImVec2& S1,const ImVec2& S2) {
    const float l2 = (S1.x-S2.x)*(S1.x-S2.x)+(S1.y-S2.y)*(S1.y-S2.y);
    if (l2 < 0.0000001f) return (P.x-S2.x)*(P.x-S2.x)+(P.y-S2.y)*(P.y-S2.y);   // S1 == S2 case
    ImVec2 T(S2 - S1);
    const float tf = ImVec2Dot(P - S1, T)/l2;
    const float minTf = 1.f < tf ? 1.f : tf;
    const float t = 0.f > minTf ? 0.f : minTf;
    T = S1 + T*t;  // T = Projection on the segment
    return (P.x-T.x)*(P.x-T.x)+(P.y-T.y)*(P.y-T.y);
}

inline static float GetSquaredDistanceToBezierCurve(const ImVec2& point,const ImVec2& p1,const ImVec2& p2,const ImVec2& p3,const ImVec2& p4)   {
    static const int num_segments = 2;   // Num Sampling Points In between p1 and p4
    static bool firstRun = true;
    static ImVec4 weights[num_segments];
    
    if (firstRun)    {
        // static init here
        IM_ASSERT(num_segments>0);    // This are needed for computing distances: cannot be zero
        firstRun = false;
        for (int i = 1; i <= num_segments; i++) {
            float t = (float)i/(float)(num_segments+1);
            float u = 1.0f - t;
            weights[i-1].x=u*u*u;
            weights[i-1].y=3*u*u*t;
            weights[i-1].z=3*u*t*t;
            weights[i-1].w=t*t*t;
        }
    }
    
    float minSquaredDistance=FLT_MAX,tmp;   // FLT_MAX is probably in <limits.h>
    ImVec2 L = p1,tp;
    for (int i = 0; i < num_segments; i++)  {
        const ImVec4& W=weights[i];
        tp.x = W.x*p1.x + W.y*p2.x + W.z*p3.x + W.w*p4.x;
        tp.y = W.x*p1.y + W.y*p2.y + W.z*p3.y + W.w*p4.y;
        
        tmp = GetSquaredDistancePointSegment(point,L,tp);
        if (minSquaredDistance>tmp) minSquaredDistance=tmp;
        L=tp;
    }
    tp = p4;
    tmp = GetSquaredDistancePointSegment(point,L,tp);
    if (minSquaredDistance>tmp) minSquaredDistance=tmp;
    
    return minSquaredDistance;
}

static ImVec2 gNodeGraphEditorWindowPadding(8.f,8.f);
inline static void NodeGraphEditorSetTooltip(const char* tooltip) {
    ImGuiStyle& igStyle = ImGui::GetStyle();
    igStyle.WindowPadding.x = gNodeGraphEditorWindowPadding.x;
    igStyle.WindowPadding.y = gNodeGraphEditorWindowPadding.y;
    ImGui::SetTooltip("%s",tooltip);
    igStyle.WindowPadding.x = igStyle.WindowPadding.y = 0.f;
}

template < int IMGUINODE_MAX_SLOTS > inline static int ProcessSlotNamesSeparatedBySemicolons(const char* slotNamesSeparatedBySemicolons,char Names[IMGUINODE_MAX_SLOTS][IMGUINODE_MAX_SLOT_NAME_LENGTH]) {
    int Count = 0;
    const char *tmp = slotNamesSeparatedBySemicolons,*tmp2 = NULL;int length;
    if (tmp && strlen(tmp)>0)    {
        while ((tmp2=strchr(tmp,(int)';')) && Count<IMGUINODE_MAX_SLOTS)    {
            length = (int) (tmp2-tmp);if (length>=IMGUINODE_MAX_SLOT_NAME_LENGTH) length=IMGUINODE_MAX_SLOT_NAME_LENGTH-1;
            strncpy(Names[Count],tmp, length);
            Names[Count][length] = '\0';
            ++Count;tmp = ++tmp2;
        }
        if (tmp && Count<IMGUINODE_MAX_SLOTS)    {
            length = (int) strlen(tmp);if (length>=IMGUINODE_MAX_SLOT_NAME_LENGTH) length=IMGUINODE_MAX_SLOT_NAME_LENGTH-1;
            strncpy(Names[Count],tmp, length);
            Names[Count][length] = '\0';
            ++Count;
        }
    }
    return Count;
}

