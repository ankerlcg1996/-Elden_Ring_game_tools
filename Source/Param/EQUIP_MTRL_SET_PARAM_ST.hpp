#pragma once

#include <cstddef>
#include <elden-x/paramdef/EQUIP_MTRL_SET_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_EQUIP_MTRL_SET_PARAM_ST_FIELDS(X) \\
    X(materialId01, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId01)) \\
    X(materialId02, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId02)) \\
    X(materialId03, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId03)) \\
    X(materialId04, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId04)) \\
    X(materialId05, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId05)) \\
    X(materialId06, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId06)) \\
    X(pad_id, offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_id)) \\
    X(itemNum01, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum01)) \\
    X(itemNum02, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum02)) \\
    X(itemNum03, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum03)) \\
    X(itemNum04, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum04)) \\
    X(itemNum05, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum05)) \\
    X(itemNum06, offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum06)) \\
    X(pad_num, offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_num)) \\
    X(materialCate01, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate01)) \\
    X(materialCate02, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate02)) \\
    X(materialCate03, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate03)) \\
    X(materialCate04, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate04)) \\
    X(materialCate05, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate05)) \\
    X(materialCate06, offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate06)) \\
    X(pad_cate, offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_cate)) \\
    X(pad, offsetof(::er::paramdef::equip_mtrl_set_param_st, pad))

#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId01 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId01)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId02 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId02)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId03 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId03)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId04 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId04)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId05 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId05)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialId06 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialId06)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_pad_id offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_id)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum01 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum01)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum02 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum02)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum03 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum03)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum04 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum04)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum05 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum05)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_itemNum06 offsetof(::er::paramdef::equip_mtrl_set_param_st, itemNum06)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_pad_num offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_num)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate01 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate01)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate02 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate02)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate03 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate03)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate04 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate04)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate05 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate05)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_materialCate06 offsetof(::er::paramdef::equip_mtrl_set_param_st, materialCate06)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_pad_cate offsetof(::er::paramdef::equip_mtrl_set_param_st, pad_cate)
#define ERD_OFFSET_EQUIP_MTRL_SET_PARAM_ST_pad offsetof(::er::paramdef::equip_mtrl_set_param_st, pad)
