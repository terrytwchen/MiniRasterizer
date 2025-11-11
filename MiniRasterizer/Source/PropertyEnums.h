/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include <string>
#include <map>

// These enums are for UI only

enum class BlinnPhongProperty
{
    AmbientX_Red, AmbientY_Green, AmbientZ_Blue,
    DiffuseX_Red, DiffuseY_Green, DiffuseZ_Blue,
    SpecularX_Red, SpecularY_Green, SpecularZ_Blue,
    Smoothness
};

enum class ToonProperty
{
    AmbientX_Red, AmbientY_Green, AmbientZ_Blue,
    BaseColorX_Red, BaseColorY_Green, BaseColorZ_Blue,
    RimColorX_Red, RimColorY_Green, RimColorZ_Blue,
    DiffuseSoftness,
    RimWidth,
    RimSoftness,
    RimDirection
};

inline BlinnPhongProperty GetPropertyEnum(const std::string& prop)
{
    static const std::map<std::string, BlinnPhongProperty> propertyMap = {
        {"AmbientX_Red", BlinnPhongProperty::AmbientX_Red},
        {"AmbientY_Green", BlinnPhongProperty::AmbientY_Green},
        {"AmbientZ_Blue", BlinnPhongProperty::AmbientZ_Blue},
        {"DiffuseX_Red", BlinnPhongProperty::DiffuseX_Red},
        {"DiffuseY_Green", BlinnPhongProperty::DiffuseY_Green},
        {"DiffuseZ_Blue", BlinnPhongProperty::DiffuseZ_Blue},
        {"SpecularX_Red", BlinnPhongProperty::SpecularX_Red},
        {"SpecularY_Green", BlinnPhongProperty::SpecularY_Green},
        {"SpecularZ_Blue", BlinnPhongProperty::SpecularZ_Blue},
        {"Smoothness", BlinnPhongProperty::Smoothness}
    };
    return propertyMap.at(prop);
}

inline ToonProperty GetToonPropertyEnum(const std::string& prop)
{
    static const std::map<std::string, ToonProperty> propertyMap = {
        {"AmbientX_Red", ToonProperty::AmbientX_Red},
        {"AmbientY_Green", ToonProperty::AmbientY_Green},
        {"AmbientZ_Blue", ToonProperty::AmbientZ_Blue},
        {"BaseColorX_Red", ToonProperty::BaseColorX_Red},
        {"BaseColorY_Green", ToonProperty::BaseColorY_Green},
        {"BaseColorZ_Blue", ToonProperty::BaseColorZ_Blue},
        {"DiffuseSoftness", ToonProperty::DiffuseSoftness},
        {"RimColorX_Red", ToonProperty::RimColorX_Red},
        {"RimColorY_Green", ToonProperty::RimColorY_Green},
        {"RimColorZ_Blue", ToonProperty::RimColorZ_Blue},
        {"RimWidth", ToonProperty::RimWidth},
        {"RimSoftness", ToonProperty::RimSoftness},
        {"RimDirection", ToonProperty::RimDirection }
    };
    return propertyMap.at(prop);
}