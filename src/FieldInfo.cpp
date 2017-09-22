//
//  FieldInfo.cpp
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#include "FieldInfo.h"



/*
 bool FieldInfo::copyFrom(const FieldInfo &f) {
 if (!isCompatibleWith(f)) return false;
 void* myOldPdata = pdata;
 *this = f;
 pdata = myOldPdata;
 return copyPDataValueFrom(f);
 }
 */
bool FieldInfo::copyPDataValueFrom(const FieldInfo &f) {
    if (!isCompatibleWith(f) || (!pdata || !f.pdata)) return false;
    if (type==FT_CUSTOM)    {
        if (!copyFieldDelegate) return false;
        else return copyFieldDelegate(*this,f);
    }
    switch (type) {
        case FT_INT:
        case FT_ENUM:
        case FT_UNSIGNED:
        case FT_FLOAT:
        case FT_DOUBLE:
        case FT_COLOR:
        {
            const int numElements = numArrayElements<=0 ? 1 : numArrayElements;
            const size_t elemSize = ((type==FT_INT||type==FT_ENUM)?sizeof(int):
                                     type==FT_UNSIGNED?sizeof(unsigned):
                                     (type==FT_FLOAT || type==FT_COLOR)?sizeof(float):
                                     type==FT_DOUBLE?sizeof(double):
                                     0);
            memcpy(pdata,f.pdata,numElements*elemSize);
        }
        break;
        case FT_BOOL: *((bool*)pdata) = *((bool*)f.pdata);
        break;
        case FT_TEXTLINE: memcpy(pdata,f.pdata,precision < f.precision ? precision : f.precision);
        break;
        case FT_STRING: memcpy(pdata,f.pdata,precision < f.precision ? precision : f.precision);
        break;
        default:
        //IM_ASSERT(true); // copyPDataValueFrom(...) not defined for this type [we can probably just skip this]
        return false;
    }
    return true;
}
#if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
bool FieldInfo::serialize(ImGuiHelper::Serializer& s) const   {
    const char* fieldName = label;
    const int ft = this->type;
    switch (type) {
        case FT_INT:
        case FT_ENUM:
        return s.save((ImGui::FieldType) ft,(const int*)pdata,fieldName,numArrayElements,precision);
        case FT_BOOL: {
            return s.save((const bool*)pdata,fieldName,numArrayElements);
        }
        case FT_UNSIGNED:
        return s.save((const unsigned*)pdata,fieldName,numArrayElements,precision);
        case FT_DOUBLE:
        return s.save((const double*)pdata,fieldName,numArrayElements,(precision>=0 && needsRadiansToDegs) ? (precision+3) : precision);
        case FT_FLOAT:
        case FT_COLOR:
        return s.save((ImGui::FieldType) ft,(const float*)pdata,fieldName,numArrayElements,(precision>=0 && needsRadiansToDegs) ? (precision+3) : precision);
        case FT_STRING:
        return s.save((const char*)pdata,fieldName,precision);
        case FT_TEXTLINE:
        return s.saveTextLines((const char*)pdata,fieldName);
        case FT_CUSTOM:
        if (!serializeFieldDelegate) return false;
        return serializeFieldDelegate(s,*this);
        default:
        //IM_ASSERT(true); // copyPDataValueFrom(...) not defined for this type [we can probably just skip this]
        return false;
    }
    return false;
}
#endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
static bool fieldInfoParseCallback(ImGuiHelper::FieldType ft,int numArrayElements,void* pValue,const char* name,void* userPtr)    {
    FieldInfo& fi = *((FieldInfo*)(userPtr));
    if (strcmp(fi.label,name)!=0) {
        fprintf(stderr,"fieldInfoParseCallback Error: \"%s\"!=\"%s\"\n",fi.label,name);
        return true;    // true = stop parsing
    }
    const FieldType type = ft;
    switch (type) {
        case FT_INT:
        case FT_ENUM:
        case FT_UNSIGNED:
        case FT_FLOAT:
        case FT_DOUBLE:
        case FT_COLOR:
        case FT_BOOL:
        {
            const int numElements = numArrayElements<=0 ? 1 : numArrayElements;
            const size_t elemSize = ((type==FT_INT||type==FT_ENUM)?sizeof(int):
                                     type==FT_UNSIGNED?sizeof(unsigned):
                                     (type==FT_FLOAT || type==FT_COLOR)?sizeof(float):
                                     type==FT_DOUBLE?sizeof(double):
                                     type==FT_BOOL?sizeof(bool):
                                     0);
            memcpy(fi.pdata,pValue,numElements*elemSize);
            break;
        }
        break;
        case FT_STRING: {
            const char* txt = (const char*)pValue;
            int len = (int) strlen(txt);
            if (fi.precision>0 && fi.precision<len+1) len = fi.precision-1;
            char* dst = (char*) fi.pdata;
            strncpy(dst,(const char*)pValue,len+1);
            dst[len]='\0';
        }
        break;
        case FT_TEXTLINE: { // This is called more than once for multiple lines, but we just use a single line here
            const char* txt = (const char*)pValue;
            int len = (int) strlen(txt);
            if (fi.precision>0 && fi.precision<len+1) len = fi.precision-1;
            char* dst = (char*) fi.pdata;
            strncpy(dst,(const char*)pValue,len+1);
            dst[len]='\0';
        }
        break;
        case FT_CUSTOM:    {
            if (fi.deserializeFieldDelegate) fi.deserializeFieldDelegate(fi,type,numArrayElements,pValue,name);
        }
        break;
        default:
        //IM_ASSERT(true); // copyPDataValueFrom(...) not defined for this type [we can probably just skip this]
        //return false;
        break;
    }
    return true;    // true = done parsing
}

const char* FieldInfo::deserialize(const ImGuiHelper::Deserializer& d,const char* start)    {
    if (type<ImGui::FT_COUNT) return d.parse(fieldInfoParseCallback,(void*)this,start);
    IM_ASSERT(true);	// Parse Custom type
    return start;
}
#endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#endif //NO_IMGUIHELPER_SERIALIZATION
FieldInfo &FieldInfoVector::addField(int *pdata, int numArrayElements, const char *label, const char *tooltip, int precision, int lowerLimit, int upperLimit, void *userData)   {
    IM_ASSERT(pdata && numArrayElements<=4);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_INT,(void*) pdata,label,tooltip,precision,numArrayElements,(double)lowerLimit,(double)upperLimit);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addField(unsigned *pdata, int numArrayElements, const char *label, const char *tooltip, int precision, unsigned lowerLimit, unsigned upperLimit, void *userData)   {
    IM_ASSERT(pdata && numArrayElements<=4);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_UNSIGNED,(void*) pdata,label,tooltip,precision,numArrayElements,(double)lowerLimit,(double)upperLimit);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addField(float *pdata, int numArrayElements, const char *label, const char *tooltip, int precision, float lowerLimit, float upperLimit, void *userData, bool needsRadiansToDegs)   {
    IM_ASSERT(pdata && numArrayElements<=4);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_FLOAT,(void*) pdata,label,tooltip,precision,numArrayElements,(double)lowerLimit,(double)upperLimit,needsRadiansToDegs);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addField(double *pdata, int numArrayElements, const char *label, const char *tooltip, int precision, double lowerLimit, double upperLimit, void *userData, bool needsRadiansToDegs)   {
    IM_ASSERT(pdata && numArrayElements<=4);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_DOUBLE,(void*) pdata,label,tooltip,precision,numArrayElements,(double)lowerLimit,(double)upperLimit,needsRadiansToDegs);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addField(char *pdata, int textLength, const char *label, const char *tooltip, int flags, bool multiline,float optionalHeight, void *userData,bool isSingleEditWithBrowseButton)   {
    IM_ASSERT(pdata);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(/*multiline ?*/ FT_STRING/* : FT_TEXTLINE*/,(void*) pdata,label,tooltip,textLength,flags,isSingleEditWithBrowseButton ? -500 : 0,(double)optionalHeight,multiline);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addFieldEnum(int *pdata, int numEnumElements, FieldInfo::TextFromEnumDelegate textFromEnumFunctionPtr, const char *label, const char *tooltip, void *userData)   {
    IM_ASSERT(pdata && numEnumElements>0 && textFromEnumFunctionPtr);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_ENUM,(void*) pdata,label,tooltip,0,0,0,1,false,numEnumElements,textFromEnumFunctionPtr,userData);
    return f;
}
FieldInfo &FieldInfoVector::addFieldEnum(int *pdata, FieldInfo::GetNumEnumElementsDelegate getNumEnumElementsFunctionPtr, FieldInfo::TextFromEnumDelegate textFromEnumFunctionPtr, const char *label, const char *tooltip, void *userData)   {
    IM_ASSERT(pdata && getNumEnumElementsFunctionPtr && textFromEnumFunctionPtr);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_ENUM,(void*) pdata,label,tooltip,0,0,0,1,false,-1,textFromEnumFunctionPtr,userData,getNumEnumElementsFunctionPtr);
    return f;
}
// 2 overloads of addFieldEnum(...) follow here. Both are based on the code in imgui.cpp
static bool NGE_Enum_Items_ArrayGetter(void* data, int idx, const char** out_text)   {
    const char* const* items = (const char* const*)data;
    if (out_text) *out_text = items[idx];
    return true;
}
FieldInfo &FieldInfoVector::addFieldEnum(int *pdata, int numEnumElements, const char* const* items, const char *label, const char *tooltip)   {
    IM_ASSERT(pdata && numEnumElements>0);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_ENUM,(void*) pdata,label,tooltip,0,0,0,1,false,numEnumElements,NGE_Enum_Items_ArrayGetter,(void*)items);
    return f;
}
static bool NGE_Enum_Items_SingleStringGetter(void* data, int idx, const char** out_text)   {
    // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)  {
        if (idx == items_count) break;
        p += strlen(p) + 1;
        items_count++;
    }
    if (!*p)        return false;
    if (out_text)   *out_text = p;
    return true;
}
FieldInfo &FieldInfoVector::addFieldEnum(int *pdata,const char* items_separated_by_zeros, const char *label, const char *tooltip)   {
    IM_ASSERT(pdata);
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)  {p += strlen(p) + 1;items_count++;}
    IM_ASSERT(items_count>0);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_ENUM,(void*) pdata,label,tooltip,0,0,0,1,false,items_count,NGE_Enum_Items_SingleStringGetter,(void*) items_separated_by_zeros);
    return f;
}
FieldInfo &FieldInfoVector::addField(bool *pdata, const char *label, const char *tooltip, void *userData)   {
    IM_ASSERT(pdata);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_BOOL,(void*) pdata,label,tooltip);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addFieldColor(float *pdata, bool useAlpha, const char *label, const char *tooltip, int precision, void *userData)   {
    IM_ASSERT(pdata);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_COLOR,(void*) pdata,label,tooltip,precision,useAlpha?4:3,0,1);
    f.userData = userData;
    return f;
}
FieldInfo &FieldInfoVector::addFieldCustom(FieldInfo::RenderFieldDelegate renderFieldDelegate,FieldInfo::CopyFieldDelegate copyFieldDelegate, void *userData
                                           //-------------------------------------------------------------------------------
#       if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#       ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
                                           ,FieldInfo::SerializeFieldDelegate serializeFieldDelegate,
#       endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#       ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
                                           FieldInfo::DeserializeFieldDelegate deserializeFieldDelegate
#       endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#       endif //NO_IMGUIHELPER_SERIALIZATION
//--------------------------------------------------------------------------------
)   {
    IM_ASSERT(renderFieldDelegate);
    push_back(FieldInfo());
    FieldInfo& f = (*this)[size()-1];
    f.init(FT_CUSTOM);
    f.renderFieldDelegate=renderFieldDelegate;
    f.copyFieldDelegate=copyFieldDelegate;
    f.userData = userData;
    //-------------------------------------------------------------------------------
#       if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#       ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
    f.serializeFieldDelegate=serializeFieldDelegate;
#       endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#       ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
    f.deserializeFieldDelegate=deserializeFieldDelegate;
#       endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#       endif //NO_IMGUIHELPER_SERIALIZATION
    //--------------------------------------------------------------------------------
    
    return f;
}

bool FieldInfo::render(int nodeWidth)   {
    FieldInfo& f = *this;
    ImGui::PushID((const void*) &f);
    static const int precisionStrSize = 16;
    static char precisionStr[precisionStrSize];
    int precisionLastCharIndex;
    const char* label = (/*f.label &&*/ f.label[0]!='\0') ? &f.label[0] : "##DummyLabel";
    if (f.type!=FT_UNSIGNED && f.type!=FT_INT)  {
        if (f.precision>0) {
            strcpy(precisionStr,"%.");
            snprintf(&precisionStr[2], precisionStrSize-2,"%ds",f.precision);
            precisionLastCharIndex = strlen(precisionStr)-1;
        }
        else {
            strcpy(precisionStr,"%s");
            precisionLastCharIndex = 1;
        }
    }
    
    float dragSpeed = (float)(f.maxValue-f.minValue)/200.f;if (dragSpeed<=0) dragSpeed=1.f;
    
    bool changed = false;int widgetIndex = 0;bool skipTooltip = false;
    switch (f.type) {
        case FT_DOUBLE: {
            precisionStr[precisionLastCharIndex]='f';
            const float minValue = (float) f.minValue;
            const float maxValue = (float) f.maxValue;
            const double rtd = f.needsRadiansToDegs ? GetRadiansToDegs<double>() : 1.f;
            const double dtr = f.needsRadiansToDegs ? GetDegsToRadians<double>() : 1.f;
            double* pField = (double*)f.pdata;
            float value[4] = {0,0,0,0};
            for (int vl=0;vl<f.numArrayElements;vl++) {
                value[vl] = (float) ((*(pField+vl))*rtd);
            }
            if (UseSlidersInsteadOfDragControls)   {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::SliderFloat2(label,value,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::SliderFloat3(label,value,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::SliderFloat4(label,value,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::SliderFloat(label,value,minValue,maxValue,precisionStr);break;
                }
            }
            else {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::DragFloat2(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::DragFloat3(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::DragFloat4(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::DragFloat(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                }
            }
            if (changed)    {
                for (int vl=0;vl<f.numArrayElements;vl++) {
                    *(pField+vl) = (double) value[vl] * dtr;
                }
            }
            
        }
        break;
        case FT_FLOAT: {
            precisionStr[precisionLastCharIndex]='f';
            const float minValue = (float) f.minValue;
            const float maxValue = (float) f.maxValue;
            const float rtd = f.needsRadiansToDegs ? GetRadiansToDegs<float>() : 1.f;
            const float dtr = f.needsRadiansToDegs ? GetDegsToRadians<float>() : 1.f;
            float* pField = (float*)f.pdata;
            float value[4] = {0,0,0,0};
            for (int vl=0;vl<f.numArrayElements;vl++) {
                value[vl] = (float) ((*(pField+vl))*rtd);
            }
            if (UseSlidersInsteadOfDragControls)   {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::SliderFloat2(label,value,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::SliderFloat3(label,value,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::SliderFloat4(label,value,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::SliderFloat(label,value,minValue,maxValue,precisionStr);break;
                }
            }
            else {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::DragFloat2(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::DragFloat3(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::DragFloat4(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::DragFloat(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                }
            }
            if (changed)    {
                for (int vl=0;vl<f.numArrayElements;vl++) {
                    *(pField+vl) = (float) value[vl]*dtr;
                }
            }
        }
        break;
        case FT_UNSIGNED: {
            strcpy(precisionStr,"%1.0f");
            const int minValue = (int) f.minValue;
            const int maxValue = (int) f.maxValue;
            unsigned* pField = (unsigned*) f.pdata;
            int value[4] = {0,0,0,0};
            for (int vl=0;vl<f.numArrayElements;vl++) {
                value[vl] = (int) *(pField+vl);
            }
            if (UseSlidersInsteadOfDragControls)   {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::SliderInt2(label,value,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::SliderInt3(label,value,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::SliderInt4(label,value,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::SliderInt(label,value,minValue,maxValue,precisionStr);break;
                }
            }
            else {
                if (dragSpeed<1.f) dragSpeed = 1.f;
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::DragInt2(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::DragInt3(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::DragInt4(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::DragInt(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                }
            }
            if (changed)    {
                for (int vl=0;vl<f.numArrayElements;vl++) {
                    *(pField+vl) = (unsigned) value[vl];
                }
            }
        }
        break;
        case FT_INT: {
            strcpy(precisionStr,"%1.0f");
            const int minValue = (int) f.minValue;
            const int maxValue = (int) f.maxValue;
            int* pField = (int*) f.pdata;
            int value[4] = {0,0,0,0};
            for (int vl=0;vl<f.numArrayElements;vl++) {
                value[vl] = (int) *(pField+vl);
            }
            if (UseSlidersInsteadOfDragControls)   {
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::SliderInt2(label,value,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::SliderInt3(label,value,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::SliderInt4(label,value,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::SliderInt(label,value,minValue,maxValue,precisionStr);break;
                }
            }
            else {
                if (dragSpeed<1.f) dragSpeed = 1.f;
                switch (f.numArrayElements)    {
                    case 2: changed = ImGui::DragInt2(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 3: changed = ImGui::DragInt3(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    case 4: changed = ImGui::DragInt4(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                    default: changed = ImGui::DragInt(label,value,dragSpeed,minValue,maxValue,precisionStr);break;
                }
            }
            if (changed)    {
                for (int vl=0;vl<f.numArrayElements;vl++) {
                    *(pField+vl) = (int) value[vl];
                }
            }
        }
        break;
        case FT_BOOL:   {
            bool * boolPtr = (bool*) f.pdata;
            changed|=ImGui::Checkbox(label,boolPtr);
        }
        break;
        case FT_ENUM: {
            if (f.getNumEnumElementsFunctionPointer) f.numEnumElements = f.getNumEnumElementsFunctionPointer(f.userData);
            changed|=ImGui::Combo(label,(int*) f.pdata,f.textFromEnumFunctionPointer,f.userData,f.numEnumElements,f.numEnumElements<12 ? f.numEnumElements : -1);
        }
        break;
        case FT_STRING: {
            char* txtField = (char*)  f.pdata;
            const int flags = f.numArrayElements;
            const float maxHeight =(float) (f.maxValue<0 ? 0 : f.maxValue);
            const bool multiline = f.needsRadiansToDegs;
            ImGui::Text("%s",label);
            float width = nodeWidth;
            if (flags<0) {
                //ImVec2 pos = ImGui::GetCursorScreenPos();
                const float startPos = ImGui::GetCursorPos().x + width;
                if (startPos>0)    {
                    ImGui::PushTextWrapPos(startPos);
                    ImGui::Text(txtField, width);
                    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255,255,0,255));
                    ImGui::PopTextWrapPos();
                }
            }
            else if (!multiline) {
                const bool addBrowseButton = (f.minValue==-500);
                float browseBtnWidth = 0;
                if (addBrowseButton) {
                    browseBtnWidth = ImGui::CalcTextSize("...").x + 10;
                    if (width-browseBtnWidth>0) width-=browseBtnWidth;
                }
                ImGui::PushItemWidth(width);
                changed|=ImGui::InputText("##DummyLabelInputText",txtField,f.precision,flags);
                ImGui::PopItemWidth();
                if (addBrowseButton)	{
                    if (/*f.tooltip &&*/ f.tooltip[0]!='\0' && ImGui::IsItemHovered()) NodeGraphEditorSetTooltip(f.tooltip);
                    skipTooltip = true;
                    ImGui::SameLine(0,4);
                    ImGui::PushItemWidth(browseBtnWidth);
                    if (ImGui::Button("...##DummyLabelInputTextBrowseButton")) {
                        changed = true;
                        widgetIndex = 1;
                    }
                    ImGui::PopItemWidth();
                    //if (ImGui::IsItemHovered()) NodeGraphEditorSetTooltip("Browse");
                }
            }
            else changed|=ImGui::InputTextMultiline("##DummyLabelInputText",txtField,f.precision,ImVec2(width,maxHeight),flags);
        }
        break;
        case FT_COLOR:  {
            float* pColor = (float*) f.pdata;
            if (f.numArrayElements==3) changed|=ImGui::ColorEdit3(label,pColor);//,ImGuiColorEditFlags_NoAlpha);
            else changed|=ImGui::ColorEdit4(label,pColor);//,ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
        }
        case FT_CUSTOM: {
            if (f.renderFieldDelegate) changed = f.renderFieldDelegate(f);
        }
        break;
        default:
        IM_ASSERT(true);    // should never happen
        break;
    }
    if (!skipTooltip && f.type!=FT_CUSTOM)  {
        if (/*f.tooltip &&*/ f.tooltip[0]!='\0' && ImGui::IsItemHovered()) NodeGraphEditorSetTooltip(f.tooltip);
    }
    if (changed && f.editedFieldDelegate) f.editedFieldDelegate(f,widgetIndex);
    ImGui::PopID();
    return changed;
}
