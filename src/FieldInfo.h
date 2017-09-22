//
//  FieldInfo.hpp
//  ImGui_NodeGraphTest
//
//  Created by Joshua Batty on 22/9/17.
//
//

#pragma once 

#include "GraphSettings.h"
#include "NodeUtils.h"

#   ifndef IMGUIHELPER_H_
// To make it compatible without serialization, we must still
// clone the FieldType enum from imguihelper.h...
// (Mmmh, this might be a problem if another addons will do the same in the future...)

// IMPORTANT: FT_INT,FT_UNSIGNED,FT_FLOAT,FT_DOUBLE,FT_BOOL support from 1 to 4 components.
enum FieldType {
    FT_INT=0,
    FT_UNSIGNED,
    FT_FLOAT,
    FT_DOUBLE,
    //--------------- End types that support 1 to 4 array components ----------
    FT_STRING,      // an arbitrary-length string (or a char blob that can be used as custom type)
    FT_ENUM,        // serialized/deserialized as FT_INT
    FT_BOOL,
    FT_COLOR,       // serialized/deserialized as FT_FLOAT (with 3 or 4 components)
    FT_TEXTLINE,    // a (series of) text line(s) (separated by '\n') that are fed one at a time in the Deserializer callback
    FT_CUSTOM,      // a custom type that is served like FT_TEXTLINE (=one line at a time).
    FT_COUNT
};
#   endif //IMGUIHELPER_H_

class FieldInfo {
public:
    int type;                   // usually one of the ImGui::FieldTypes in imguihelper.h
    void* pdata;                // ptr to a variable of type "type" (or to an array of "types")
    char label[IMGUIFIELDINFO_MAX_LABEL_LENGTH];
    char tooltip[IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH];
    // in case of FT_STRING max number of characters, in case of FT_FLOAT or FT_DOUBLE the number of decimals to be displayed (experiment for other types and see)
    int precision;
    // used only for FT_INT, FT_UNSIGNED, FT_FLOAT, FT_DOUBLE
    int numArrayElements;       // up to 4
    double minValue,maxValue;
    bool needsRadiansToDegs;    // optional for FT_FLOAT and FT_DOUBLE only
    // used only for FT_ENUM (internally it uses FT_INT, pdata must point to an int):
    int numEnumElements;
    typedef bool (*TextFromEnumDelegate)(void*, int, const char**); // userData is the first param
    TextFromEnumDelegate  textFromEnumFunctionPointer;  // used only when type==FT_ENUM, otherwise set it to NULL. The method is used to convert an int to a char*.
    void* userData;          // passed to textFromEnumFunctionPointer when type==FT_ENUM (useful if you want to share the same TextFromEnumDelegate for multiple enums). Otherwise set it to NULL or use it as you like.
    typedef int (*GetNumEnumElementsDelegate)(void*); // userData is the first param
    GetNumEnumElementsDelegate getNumEnumElementsFunctionPointer;  // used OPTIONALLY only when type==FT_ENUM, otherwise set it to NULL. The method overrides the value of 'numEnumElements'.
    typedef void (*EditedFieldDelegate)(FieldInfo& field,int widgetIndex);  // widgetIndex is always zero
    EditedFieldDelegate editedFieldDelegate;
    // used only for FT_CUSTOM
    typedef bool (*RenderFieldDelegate)(FieldInfo& field);
    RenderFieldDelegate renderFieldDelegate;
    typedef bool (*CopyFieldDelegate)(FieldInfo& fdst,const FieldInfo& fsrc);
    CopyFieldDelegate copyFieldDelegate;
    
    //-------------------------------------------------------------------------------
#       if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#       ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
    typedef bool (*SerializeFieldDelegate)(ImGuiHelper::Serializer& s,const FieldInfo& src);
    SerializeFieldDelegate serializeFieldDelegate;
#       endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#       ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
    typedef bool (*DeserializeFieldDelegate)(FieldInfo& dst,int ft,int numArrayElements,const void* pValue,const char* name);
    DeserializeFieldDelegate deserializeFieldDelegate;
    // ------------------------------------------------------
#       endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#       endif //NO_IMGUIHELPER_SERIALIZATION
    //--------------------------------------------------------------------------------
    IMGUI_API bool render(int nodeWidth);
    
protected:
    FieldInfo() {}
    void init (int _type=FT_INT,void* _pdata=NULL,const char* _label=NULL,const char* _tooltip=NULL,
               int _precision=0,int _numArrayElements=0,double _lowerLimit=0,double _upperLimit=1,bool _needsRadiansToDegs=false,
               int _numEnumElements=0,TextFromEnumDelegate _textFromEnumFunctionPointer=NULL,void* _userData=NULL,GetNumEnumElementsDelegate _getNumEnumElementsFunctionPointer=NULL,
               RenderFieldDelegate _renderFieldDelegate=NULL,EditedFieldDelegate _editedFieldDelegate=NULL)
    {
        label[0]='\0';if (_label) {strncpy(label,_label,IMGUIFIELDINFO_MAX_LABEL_LENGTH);label[IMGUIFIELDINFO_MAX_LABEL_LENGTH-1]='\0';}
        tooltip[0]='\0';if (_tooltip) {strncpy(tooltip,_tooltip,IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH);tooltip[IMGUIFIELDINFO_MAX_TOOLTIP_LENGTH-1]='\0';}
        type = _type;
        pdata = _pdata;
        precision = _precision;
        numArrayElements = _numArrayElements;
        minValue = _lowerLimit;
        maxValue = _upperLimit;
        needsRadiansToDegs = _needsRadiansToDegs;
        numEnumElements = _numEnumElements;
        textFromEnumFunctionPointer = _textFromEnumFunctionPointer;
        userData = _userData;
        getNumEnumElementsFunctionPointer = _getNumEnumElementsFunctionPointer;
        renderFieldDelegate = _renderFieldDelegate;
        editedFieldDelegate = _editedFieldDelegate;
    }
    
    inline bool isCompatibleWith(const FieldInfo& f) const {
        return (type==f.type &&
                numArrayElements == f.numArrayElements);   // Warning: we can't use numArrayElements for other purposes when it's not used....
    }
    //bool copyFrom(const FieldInfo& f);
    IMGUI_API bool copyPDataValueFrom(const FieldInfo& f);
    //-------------------------------------------------------------------------------
#       if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#       ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
    IMGUI_API bool serialize(ImGuiHelper::Serializer& s) const;
#       endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#       ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
    IMGUI_API const char* deserialize(const ImGuiHelper::Deserializer& d,const char* start);
#       endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#       endif //NO_IMGUIHELPER_SERIALIZATION
    //--------------------------------------------------------------------------------
    
    friend class FieldInfoVector;
    friend class Node;
};

class FieldInfoVector : public ImVector < FieldInfo >    {
public:
    // Warning: returned reference might not stay valid for long in these methods
    IMGUI_API FieldInfo& addField(int* pdata,int numArrayElements=1,const char* label=NULL,const char* tooltip=NULL,int precision=0,int lowerLimit=0,int upperLimit=100,void* userData=NULL);
    IMGUI_API FieldInfo& addField(unsigned* pdata,int numArrayElements=1,const char* label=NULL,const char* tooltip=NULL,int precision=0,unsigned lowerLimit=0,unsigned upperLimit=100,void* userData=NULL);
    IMGUI_API FieldInfo& addField(float* pdata,int numArrayElements=1,const char* label=NULL,const char* tooltip=NULL,int precision=3,float lowerLimit=0,float upperLimit=1,void* userData=NULL,bool needsRadiansToDegs=false);
    IMGUI_API FieldInfo& addField(double* pdata,int numArrayElements=1,const char* label=NULL,const char* tooltip=NULL,int precision=3,double lowerLimit=0,double upperLimit=100,void* userData=NULL,bool needsRadiansToDegs=false);
    
    IMGUI_API FieldInfo& addFieldEnum(int* pdata,int numEnumElements,FieldInfo::TextFromEnumDelegate textFromEnumFunctionPtr,const char* label=NULL,const char* tooltip=NULL,void* userData=NULL);
    IMGUI_API FieldInfo& addFieldEnum(int* pdata,FieldInfo::GetNumEnumElementsDelegate getNumEnumElementsFunctionPtr,FieldInfo::TextFromEnumDelegate textFromEnumFunctionPtr,const char* label=NULL,const char* tooltip=NULL,void* userData=NULL);
    IMGUI_API FieldInfo& addFieldEnum(int *pdata, int numEnumElements, const char* const* items, const char *label=NULL, const char *tooltip=NULL);
    IMGUI_API FieldInfo& addFieldEnum(int *pdata,const char* items_separated_by_zeros, const char *label=NULL, const char *tooltip=NULL);     // separate items with \0, end item-list with \0\0
    IMGUI_API FieldInfo& addField(bool* pdata,const char* label=NULL,const char* tooltip=NULL,void* userData=NULL);
    IMGUI_API FieldInfo& addFieldColor(float* pdata,bool useAlpha=true,const char* label=NULL,const char* tooltip=NULL,int precision=3,void* userData=NULL);
    FieldInfo& addFieldTextEdit(char* pdata, int textLength=0, const char* label=NULL, const char* tooltip=NULL, int flags=ImGuiInputTextFlags_EnterReturnsTrue, void* userData=NULL) {
        return addField(pdata,textLength,label,tooltip,flags,false,-1.f,userData);
    }
    FieldInfo& addFieldTextEditMultiline(char* pdata, int textLength=0, const char* label=NULL, const char* tooltip=NULL, int flags=ImGuiInputTextFlags_EnterReturnsTrue, float optionalHeight=-1.f, void* userData=NULL) {
        return addField(pdata,textLength,label,tooltip,flags,true,optionalHeight,userData);
    }
    FieldInfo& addFieldTextWrapped(char* pdata, int textLength=0, const char* label=NULL, const char* tooltip=NULL,void* userData=NULL) {
        return addField(pdata,textLength,label,tooltip,-1,true,-1.f,userData);
    }
    FieldInfo& addFieldTextEditAndBrowseButton(char* pdata, int textLength=0, const char* label=NULL, const char* tooltip=NULL, int flags=ImGuiInputTextFlags_EnterReturnsTrue,void* userData=NULL) {
        return addField(pdata,textLength,label,tooltip,flags,false,-1.f,userData,true);
    }
    
    IMGUI_API FieldInfo& addFieldCustom(FieldInfo::RenderFieldDelegate renderFieldDelegate,FieldInfo::CopyFieldDelegate copyFieldDelegate,void* userData
                                        //-------------------------------------------------------------------------------
#       if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#       ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
                                        ,FieldInfo::SerializeFieldDelegate serializeFieldDelegate=NULL,
#       endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#       ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
                                        FieldInfo::DeserializeFieldDelegate deserializeFieldDelegate=NULL
#       endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#       endif //NO_IMGUIHELPER_SERIALIZATION
    //--------------------------------------------------------------------------------
    );
    
    void copyPDataValuesFrom(const FieldInfoVector& o)   {
        for (int i=0,isz=o.size()<size()?o.size():size();i<isz;i++) {
            const FieldInfo& of = o[i];
            FieldInfo& f = (*this)[i];
            f.copyPDataValueFrom(of);
        }
    }
    
    //-------------------------------------------------------------------------------
#   if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#    ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
    bool serialize(ImGuiHelper::Serializer& s) const {
        bool rt = true;
        for (int i=0,isz=size();i<isz;i++) {
            const FieldInfo& f = (*this)[i];
            rt|=f.serialize(s);
            // should I stop if rt is false ?
        }
        return rt;
    }
#   endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#   ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
    const char* deserialize(const ImGuiHelper::Deserializer& d,const char* start)   {
        const char* pend = start;
        for (int i=0,isz=size();i<isz;i++) {
            FieldInfo& f = (*this)[i];
            pend = f.deserialize(d,pend);
        }
        return pend;
    }
#   endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#   endif //NO_IMGUIHELPER_SERIALIZATION
    //--------------------------------------------------------------------------------
    
protected:
    IMGUI_API FieldInfo& addField(char* pdata, int textLength=0, const char* label=NULL, const char* tooltip=NULL, int flags=ImGuiInputTextFlags_EnterReturnsTrue, bool multiline=false,float optionalHeight=-1.f, void* userData=NULL,bool isSingleEditWithBrowseButton=false);
    friend class Node;
};
//-------------------------------------------------------------------------------------------
