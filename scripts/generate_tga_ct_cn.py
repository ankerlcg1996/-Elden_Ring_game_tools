# -*- coding: utf-8 -*-
from pathlib import Path
from collections import defaultdict, OrderedDict
import csv
import re


ROOT = Path(
    r"C:\Users\ankerlcg\Desktop\erdporject\ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring"
)
OUT_CSV = Path(r"C:\Users\ankerlcg\Desktop\erdporject\TGA_CT_FULL_FEATURES_CN.csv")
OUT_MD = Path(r"C:\Users\ankerlcg\Desktop\erdporject\TGA_CT_FULL_TREE_CN.md")

EXACT = {
    "Open - The Grand Archives - Elden Ring": "Open - The Grand Archives - Elden Ring（艾尔登法环总表）",
    "Camera": "相机",
    "Coordinates & Teleport": "坐标与传送",
    "Debug Stuff": "调试内容",
    "Event Flags": "事件标志",
    "Hero": "角色",
    "MiscWIP": "杂项（开发中）",
    "Param Mods": "参数修改",
    "Scripts": "脚本",
    "Statistics": "统计信息",
    "Table Tools": "表工具",
    "Table Info (FAQ)": "表信息（FAQ）",
    "Build Creation": "建号与发放",
    "Cheats": "作弊功能",
    "Functions": "函数工具",
    "Helpers": "辅助项",
    "Npc Menus": "NPC 菜单",
    "QoL": "体验优化",
    "Seamless Co-op": "无缝联机",
    "Character": "角色",
    "Misc": "杂项",
    "SaveLoad2": "存档读写2",
    "Session Info": "会话信息",
    "Character Flags": "角色标志位",
    "SpecialEffect": "特殊效果",
    "Special Effect": "特殊效果",
    "EquipItemData": "装备物品数据",
    "EquipMagicData": "已装备魔法数据",
    "Face Data": "面部数据",
    "SuperArmor": "霸体",
    "Quick Item": "快捷物品",
    "Targeted Npc Info": "当前目标 NPC 信息",
    "Last Hit Npc Info": "最后命中 NPC 信息",
    "Current Session": "当前会话",
    "CSDebugCam": "调试相机 CSDebugCam",
    "CSPersCam0": "普通相机 CSPersCam0",
    "CSPersCam1": "普通相机 CSPersCam1",
    "Reset": "重置",
    "FOV": "视野角 FOV",
    "Render Dist": "渲染距离",
    "NoDamage": "无伤",
    "NoDead": "不死",
    "NoFPConsumption": "不耗 FP",
    "NoGoodsConsume": "道具不消耗",
    "NoStaminaConsumption": "不耗精力",
    "No Phantom Finger Auto-Equip": "禁止自动装备幻影指头",
    "Invisible helmets (re-equip!)": "隐藏头盔（需重新装备）",
    "Permanent Lantern Toggle": "常亮提灯切换",
    "Warp out of uncleared mini-dungeons": "未清小地牢强制传出",
    "Save Request": "请求保存",
    "Reload Character": "重载角色",
    "Load Save Slot": "读取存档槽位",
    "Disable Auto-Save": "禁用自动保存",
    "Save Slot (Profile Index)": "存档槽位（档案索引）",
    "Free Camera": "自由相机",
    "Noclip": "穿墙",
    "Kill all mobs": "清空敌人",
    "Give Runes": "给予卢恩",
    "Spawn Item": "生成物品",
    "Custom FPS Limit": "自定义 FPS 上限",
    "Permanent Lantern Toggle": "常亮提灯切换",
    "Set CSDebugCam to CSPersCam1 values": "将 CSDebugCam 设置为 CSPersCam1 的值",
    "Address": "地址",
    "Name": "名称",
    "ID": "ID",
    "Start": "起始字段",
    "Effect": "效果",
    "Bullet": "子弹",
    "weight": "重量",
    "Weight": "重量",
    "Description": "说明",
    "Flags": "标志",
    "Dependencies": "依赖项",
    "Global Functions": "全局函数",
    "Address Helpers": "地址辅助",
    "ID Helpers": "ID 辅助",
    "Classes": "类",
    "BaseParamClass": "基础参数类",
    "Params": "参数表",
    "Special Params": "特殊参数",
    "Dropdowns": "下拉项",
    "STATE_INFO": "状态信息",
    "Open": "展开",
    "Close": "关闭",
    "Enable": "启用",
    "Disable": "禁用",
    "Example 1 Basic": "示例1：基础用法",
    "Example 2 Unequip all slots": "示例2：卸下全部槽位",
    "Example 3 Set specific slots": "示例3：设置指定槽位",
    "Example 4 Random items": "示例4：随机物品",
    " Add SpEffect": "添加 SpEffect",
    " Remove SpEffect": "移除 SpEffect",
}

PHRASE_MAP = OrderedDict(
    [
        ("Set CSDebugCam to CSPersCam1 values", "将 CSDebugCam 设置为 CSPersCam1 的值"),
        ("Scale Attack Power With Equip Load", "根据装备负重缩放攻击力"),
        ("Current Session", "当前会话"),
        ("Session Info", "会话信息"),
        ("Special Effect", "特殊效果"),
        ("SpecialEffect", "特殊效果"),
        ("Equip Magic Quantity List", "已装备魔法数量列表"),
        ("Equip Magic List", "已装备魔法列表"),
        ("Quick Item", "快捷物品"),
        ("Save Request Busy", "保存请求忙碌中"),
        ("read only", "只读"),
        ("Loading screen stuck fix", "加载界面卡死修复"),
        ("No Phantom Finger Auto-Equip", "禁止自动装备幻影指头"),
        ("Invisible helmets", "隐藏头盔"),
        ("re-equip", "重新装备"),
        ("Warp out of uncleared mini-dungeons", "未清小地牢强制传出"),
        ("Permanent Lantern Toggle", "常亮提灯切换"),
        ("Custom FPS Limit", "自定义 FPS 上限"),
        ("Render Dist", "渲染距离"),
        ("Save Slot", "存档槽位"),
        ("Profile Index", "档案索引"),
        ("Character Flags", "角色标志位"),
        ("Event Flags", "事件标志"),
        ("Coordinates", "坐标"),
        ("Teleport", "传送"),
        ("Build Creation", "建号与发放"),
        ("Seamless Co-op", "无缝联机"),
        ("Targeted Npc Info", "当前目标 NPC 信息"),
        ("Last Hit Npc Info", "最后命中 NPC 信息"),
        ("Address Helpers", "地址辅助"),
        ("ID Helpers", "ID 辅助"),
        ("Global Functions", "全局函数"),
        ("BaseParamClass", "基础参数类"),
        ("Special Params", "特殊参数"),
    ]
)

WORD_MAP = {
    "no": "无",
    "damage": "伤害",
    "dead": "死亡",
    "fp": "FP",
    "stamina": "精力",
    "goods": "道具",
    "consume": "消耗",
    "item": "物品",
    "items": "物品",
    "runes": "卢恩",
    "save": "保存",
    "load": "读取",
    "slot": "槽位",
    "profile": "档案",
    "index": "索引",
    "current": "当前",
    "session": "会话",
    "net": "联机",
    "player": "玩家",
    "primary": "主",
    "secondary": "副",
    "tertiary": "第三",
    "right": "右手",
    "left": "左手",
    "wep": "武器",
    "arrow": "箭",
    "bolt": "弩箭",
    "armor": "胸甲",
    "gauntlet": "手甲",
    "leggings": "腿甲",
    "helmet": "头盔",
    "hair": "头发",
    "accessory": "护符",
    "quantity": "数量",
    "selected": "已选",
    "array": "数组",
    "idx": "索引",
    "special": "特殊",
    "effect": "效果",
    "active": "激活",
    "effects": "效果",
    "duration": "持续时间",
    "total": "总",
    "vertical": "纵向",
    "horizontal": "横向",
    "face": "脸部",
    "ratio": "比例",
    "nose": "鼻子",
    "forehead": "额头",
    "camera": "相机",
    "reset": "重置",
    "render": "渲染",
    "dist": "距离",
    "debug": "调试",
    "flags": "标志",
    "flag": "标志",
    "hero": "角色",
    "coordinates": "坐标",
    "teleport": "传送",
    "param": "参数",
    "params": "参数",
    "mods": "修改",
    "scripts": "脚本",
    "helpers": "辅助",
    "functions": "函数",
    "cheats": "作弊",
    "misc": "杂项",
    "tools": "工具",
    "character": "角色",
    "magic": "魔法",
    "equip": "装备",
    "equipment": "装备",
    "weight": "重量",
    "change": "变化",
    "rate": "倍率",
    "weapon": "武器",
    "protector": "防具",
    "accessory": "护符",
    "name": "名称",
    "id": "ID",
    "address": "地址",
    "start": "起始",
    "state": "状态",
    "info": "信息",
    "dropdowns": "下拉项",
    "open": "展开",
    "grand": "大书库",
    "archives": "档案",
    "elden": "艾尔登",
    "ring": "法环",
    "fov": "视野角",
    "x": "X",
    "y": "Y",
    "z": "Z",
    "example": "示例",
    "basic": "基础",
    "random": "随机",
    "specific": "指定",
    "unequip": "卸下",
    "all": "全部",
    "multiple": "多个",
    "apply": "应用",
    "remove": "移除",
    "toggle": "切换",
    "invisible": "隐藏",
    "helmets": "头盔",
    "phantom": "幻影",
    "finger": "指头",
    "auto": "自动",
    "co": "联机",
    "op": "合作",
    "loading": "加载",
    "screen": "界面",
    "stuck": "卡住",
    "fix": "修复",
    "read": "读取",
    "only": "仅",
    "busy": "忙碌",
    "request": "请求",
    "disable": "禁用",
    "enable": "启用",
    "super": "超级",
    "durability": "耐久",
    "recover": "恢复",
    "time": "时间",
    "break": "破坏",
    "light": "轻",
    "heavy": "重",
    "normal": "普通",
    "over": "超出",
    "map": "地图",
    "graces": "赐福",
    "summoning": "召唤",
    "pools": "池",
    "cookbooks": "食谱",
    "whetblades": "砥石刀",
    "arenas": "斗技场",
    "gestures": "手势",
    "ng": "周目",
    "free": "免费",
}

REGEX_RULES = [
    (re.compile(r"^NetPlayer\s*(\d+)$", re.I), lambda m: f"联机玩家{m.group(1)}"),
    (re.compile(r"^Accessory\s*(\d+)$", re.I), lambda m: f"护符槽位{m.group(1)}"),
    (re.compile(r"^Slot\s*(\d+)$", re.I), lambda m: f"槽位{m.group(1)}"),
    (re.compile(r"^Quantity\s*(\d+)$", re.I), lambda m: f"数量{m.group(1)}"),
    (re.compile(r"^(\d+)$"), lambda m: m.group(1)),
    (re.compile(r"^refId\s*(\d+)$", re.I), lambda m: f"引用ID {m.group(1)}"),
    (re.compile(r"^refId_(\d+)$", re.I), lambda m: f"引用ID {m.group(1)}"),
]


def split_words(text: str):
    text = text.replace("&", " & ")
    text = text.replace("(", " ( ").replace(")", " ) ")
    text = text.replace("[", " [ ").replace("]", " ] ")
    text = text.replace("/", " / ").replace("-", " - ").replace("_", " ")
    text = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", text)
    text = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", text)
    text = re.sub(r"\s+", " ", text).strip()
    return text.split(" ") if text else []


def translate_segment(seg: str) -> str:
    seg = seg.strip()
    if seg in EXACT:
        return EXACT[seg]

    for rx, fn in REGEX_RULES:
        m = rx.match(seg)
        if m:
            return fn(m)

    work = seg
    for src, dst in PHRASE_MAP.items():
        work = work.replace(src, dst)

    if any("\u4e00" <= ch <= "\u9fff" for ch in work):
        return work

    tokens = split_words(work)
    if not tokens:
        return seg

    translated = []
    for token in tokens:
        low = token.lower()
        if token in {"(", ")", "/", "-", "&", "[", "]"}:
            translated.append(token)
        elif low in WORD_MAP:
            translated.append(WORD_MAP[low])
        elif re.fullmatch(r"\d+", token):
            translated.append(token)
        elif token in {"ID", "FOV", "FP", "NPC", "QoL", "WIP", "NG", "XML", "CEA"}:
            translated.append(token)
        else:
            translated.append(token)

    result = " ".join(translated)
    result = re.sub(r"\s+([)\]])", r"\1", result)
    result = re.sub(r"([([[])\s+", r"\1", result)
    result = result.replace(" / ", "/").replace(" - ", "-").replace(" & ", " 与 ")
    result = re.sub(r"\s+", " ", result).strip()
    return result or seg


def collect_unique_entries():
    unique = {}
    for path in ROOT.rglob("*"):
        if path.is_file() and path.suffix.lower() in {".xml", ".cea"}:
            rel = path.relative_to(ROOT)
            key = str(rel.with_suffix("")).replace("\\", "/")
            unique.setdefault(key, set()).add(path.suffix.lower())
    return unique


class Node:
    __slots__ = ("name", "children", "leaf", "exts")

    def __init__(self, name=""):
        self.name = name
        self.children = {}
        self.leaf = False
        self.exts = set()


def build_tree(sorted_keys, unique):
    root_node = Node("ROOT")
    for key in sorted_keys:
        node = root_node
        for part in key.split("/"):
            node = node.children.setdefault(part, Node(part))
        node.leaf = True
        node.exts = unique[key]
    return root_node


def write_csv(sorted_keys, unique):
    with OUT_CSV.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["顶层模块", "相对路径", "中文路径", "原始名称", "中文名称", "深度", "来源扩展"])
        for key in sorted_keys:
            parts = key.split("/")
            translated_parts = [translate_segment(p) for p in parts]
            writer.writerow(
                [
                    parts[0] if parts else "",
                    key,
                    "/".join(translated_parts),
                    parts[-1] if parts else "",
                    translated_parts[-1] if translated_parts else "",
                    len(parts),
                    ",".join(sorted(unique[key])),
                ]
            )


def write_markdown(sorted_keys, unique):
    module_counts = defaultdict(int)
    for key in sorted_keys:
        module_counts[key.split("/")[0]] += 1

    root_node = build_tree(sorted_keys, unique)
    lines = []
    lines.append("# Elden-Ring-CT-TGA-master 全功能中文清单")
    lines.append("")
    lines.append("说明：")
    lines.append("- 这份清单基于 `CheatTable/CheatEntries/Open - The Grand Archives - Elden Ring` 目录生成。")
    lines.append("- 同一路径下的 `.xml` 与 `.cea` 已合并为一个功能条目，避免重复。")
    lines.append("- 为了保留完整层级，下面的树会把模块、子菜单、字段、脚本选项全部列出。")
    lines.append("- 低层参数名和内部字段名较多，中文采用“直译 + 保留原术语”的方式，便于你后续对照 DLL 和 CT。")
    lines.append("")
    lines.append("统计：")
    lines.append(f"- 去重后功能/子项条目总数：`{len(sorted_keys)}`")
    for top in sorted(module_counts.keys(), key=str.lower):
        lines.append(f"- `{top}`：`{module_counts[top]}` 项")
    lines.append("")
    lines.append(f"完整 CSV：`{OUT_CSV.name}`")
    lines.append("")

    def emit(node, depth=0):
        for name in sorted(node.children.keys(), key=lambda s: s.lower()):
            child = node.children[name]
            indent = "  " * depth
            translated = translate_segment(name)
            suffix = f"  [来源: {','.join(sorted(child.exts))}]" if child.leaf else ""
            lines.append(f"{indent}- `{name}` -> {translated}{suffix}")
            emit(child, depth + 1)

    emit(root_node)
    OUT_MD.write_text("\n".join(lines), encoding="utf-8")


def main():
    unique = collect_unique_entries()
    sorted_keys = sorted(unique.keys(), key=lambda s: [part.lower() for part in s.split("/")])
    write_csv(sorted_keys, unique)
    write_markdown(sorted_keys, unique)
    print(OUT_CSV)
    print(OUT_MD)
    print(f"unique_entries {len(sorted_keys)}")


if __name__ == "__main__":
    main()
