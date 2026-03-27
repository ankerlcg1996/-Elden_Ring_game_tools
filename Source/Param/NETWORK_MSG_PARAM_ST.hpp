#pragma once

#include <cstddef>
#include <elden-x/paramdef/NETWORK_MSG_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_NETWORK_MSG_PARAM_ST_FIELDS(X) \\
    X(priority, offsetof(::er::paramdef::network_msg_param_st, priority)) \\
    X(forcePlay, offsetof(::er::paramdef::network_msg_param_st, forcePlay)) \\
    X(pad1, offsetof(::er::paramdef::network_msg_param_st, pad1)) \\
    X(normalWhite, offsetof(::er::paramdef::network_msg_param_st, normalWhite)) \\
    X(umbasaWhite, offsetof(::er::paramdef::network_msg_param_st, umbasaWhite)) \\
    X(berserkerWhite, offsetof(::er::paramdef::network_msg_param_st, berserkerWhite)) \\
    X(sinnerHeroWhite, offsetof(::er::paramdef::network_msg_param_st, sinnerHeroWhite)) \\
    X(normalBlack, offsetof(::er::paramdef::network_msg_param_st, normalBlack)) \\
    X(umbasaBlack, offsetof(::er::paramdef::network_msg_param_st, umbasaBlack)) \\
    X(berserkerBlack, offsetof(::er::paramdef::network_msg_param_st, berserkerBlack)) \\
    X(forceJoinBlack, offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack)) \\
    X(forceJoinUmbasaBlack, offsetof(::er::paramdef::network_msg_param_st, forceJoinUmbasaBlack)) \\
    X(forceJoinBerserkerBlack, offsetof(::er::paramdef::network_msg_param_st, forceJoinBerserkerBlack)) \\
    X(sinnerHunterVisitor, offsetof(::er::paramdef::network_msg_param_st, sinnerHunterVisitor)) \\
    X(redHunterVisitor, offsetof(::er::paramdef::network_msg_param_st, redHunterVisitor)) \\
    X(guardianOfBossVisitor, offsetof(::er::paramdef::network_msg_param_st, guardianOfBossVisitor)) \\
    X(guardianOfForestMapVisitor, offsetof(::er::paramdef::network_msg_param_st, guardianOfForestMapVisitor)) \\
    X(guardianOfAnolisVisitor, offsetof(::er::paramdef::network_msg_param_st, guardianOfAnolisVisitor)) \\
    X(rosaliaBlack, offsetof(::er::paramdef::network_msg_param_st, rosaliaBlack)) \\
    X(forceJoinRosaliaBlack, offsetof(::er::paramdef::network_msg_param_st, forceJoinRosaliaBlack)) \\
    X(redHunterVisitor2, offsetof(::er::paramdef::network_msg_param_st, redHunterVisitor2)) \\
    X(npc1, offsetof(::er::paramdef::network_msg_param_st, npc1)) \\
    X(npc2, offsetof(::er::paramdef::network_msg_param_st, npc2)) \\
    X(npc3, offsetof(::er::paramdef::network_msg_param_st, npc3)) \\
    X(npc4, offsetof(::er::paramdef::network_msg_param_st, npc4)) \\
    X(battleRoyal, offsetof(::er::paramdef::network_msg_param_st, battleRoyal)) \\
    X(npc5, offsetof(::er::paramdef::network_msg_param_st, npc5)) \\
    X(npc6, offsetof(::er::paramdef::network_msg_param_st, npc6)) \\
    X(npc7, offsetof(::er::paramdef::network_msg_param_st, npc7)) \\
    X(npc8, offsetof(::er::paramdef::network_msg_param_st, npc8)) \\
    X(npc9, offsetof(::er::paramdef::network_msg_param_st, npc9)) \\
    X(npc10, offsetof(::er::paramdef::network_msg_param_st, npc10)) \\
    X(npc11, offsetof(::er::paramdef::network_msg_param_st, npc11)) \\
    X(npc12, offsetof(::er::paramdef::network_msg_param_st, npc12)) \\
    X(npc13, offsetof(::er::paramdef::network_msg_param_st, npc13)) \\
    X(npc14, offsetof(::er::paramdef::network_msg_param_st, npc14)) \\
    X(npc15, offsetof(::er::paramdef::network_msg_param_st, npc15)) \\
    X(npc16, offsetof(::er::paramdef::network_msg_param_st, npc16)) \\
    X(forceJoinBlack_B, offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_B)) \\
    X(normalWhite_Npc, offsetof(::er::paramdef::network_msg_param_st, normalWhite_Npc)) \\
    X(forceJoinBlack_Npc, offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_Npc)) \\
    X(forceJoinBlack_B_Npc, offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_B_Npc)) \\
    X(forceJoinBlack_C_Npc, offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_C_Npc)) \\
    X(unknown_0xa4, offsetof(::er::paramdef::network_msg_param_st, unknown_0xa4)) \\
    X(unknown_0xa8, offsetof(::er::paramdef::network_msg_param_st, unknown_0xa8)) \\
    X(unknown_0xac, offsetof(::er::paramdef::network_msg_param_st, unknown_0xac)) \\
    X(unknown_0xb0, offsetof(::er::paramdef::network_msg_param_st, unknown_0xb0)) \\
    X(unknown_0xb4, offsetof(::er::paramdef::network_msg_param_st, unknown_0xb4)) \\
    X(pad2_new, offsetof(::er::paramdef::network_msg_param_st, pad2_new))

#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_priority offsetof(::er::paramdef::network_msg_param_st, priority)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forcePlay offsetof(::er::paramdef::network_msg_param_st, forcePlay)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_pad1 offsetof(::er::paramdef::network_msg_param_st, pad1)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_normalWhite offsetof(::er::paramdef::network_msg_param_st, normalWhite)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_umbasaWhite offsetof(::er::paramdef::network_msg_param_st, umbasaWhite)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_berserkerWhite offsetof(::er::paramdef::network_msg_param_st, berserkerWhite)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_sinnerHeroWhite offsetof(::er::paramdef::network_msg_param_st, sinnerHeroWhite)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_normalBlack offsetof(::er::paramdef::network_msg_param_st, normalBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_umbasaBlack offsetof(::er::paramdef::network_msg_param_st, umbasaBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_berserkerBlack offsetof(::er::paramdef::network_msg_param_st, berserkerBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBlack offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinUmbasaBlack offsetof(::er::paramdef::network_msg_param_st, forceJoinUmbasaBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBerserkerBlack offsetof(::er::paramdef::network_msg_param_st, forceJoinBerserkerBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_sinnerHunterVisitor offsetof(::er::paramdef::network_msg_param_st, sinnerHunterVisitor)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_redHunterVisitor offsetof(::er::paramdef::network_msg_param_st, redHunterVisitor)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_guardianOfBossVisitor offsetof(::er::paramdef::network_msg_param_st, guardianOfBossVisitor)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_guardianOfForestMapVisitor offsetof(::er::paramdef::network_msg_param_st, guardianOfForestMapVisitor)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_guardianOfAnolisVisitor offsetof(::er::paramdef::network_msg_param_st, guardianOfAnolisVisitor)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_rosaliaBlack offsetof(::er::paramdef::network_msg_param_st, rosaliaBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinRosaliaBlack offsetof(::er::paramdef::network_msg_param_st, forceJoinRosaliaBlack)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_redHunterVisitor2 offsetof(::er::paramdef::network_msg_param_st, redHunterVisitor2)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc1 offsetof(::er::paramdef::network_msg_param_st, npc1)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc2 offsetof(::er::paramdef::network_msg_param_st, npc2)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc3 offsetof(::er::paramdef::network_msg_param_st, npc3)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc4 offsetof(::er::paramdef::network_msg_param_st, npc4)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_battleRoyal offsetof(::er::paramdef::network_msg_param_st, battleRoyal)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc5 offsetof(::er::paramdef::network_msg_param_st, npc5)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc6 offsetof(::er::paramdef::network_msg_param_st, npc6)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc7 offsetof(::er::paramdef::network_msg_param_st, npc7)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc8 offsetof(::er::paramdef::network_msg_param_st, npc8)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc9 offsetof(::er::paramdef::network_msg_param_st, npc9)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc10 offsetof(::er::paramdef::network_msg_param_st, npc10)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc11 offsetof(::er::paramdef::network_msg_param_st, npc11)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc12 offsetof(::er::paramdef::network_msg_param_st, npc12)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc13 offsetof(::er::paramdef::network_msg_param_st, npc13)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc14 offsetof(::er::paramdef::network_msg_param_st, npc14)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc15 offsetof(::er::paramdef::network_msg_param_st, npc15)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_npc16 offsetof(::er::paramdef::network_msg_param_st, npc16)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBlack_B offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_B)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_normalWhite_Npc offsetof(::er::paramdef::network_msg_param_st, normalWhite_Npc)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBlack_Npc offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_Npc)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBlack_B_Npc offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_B_Npc)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_forceJoinBlack_C_Npc offsetof(::er::paramdef::network_msg_param_st, forceJoinBlack_C_Npc)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_unknown_0xa4 offsetof(::er::paramdef::network_msg_param_st, unknown_0xa4)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_unknown_0xa8 offsetof(::er::paramdef::network_msg_param_st, unknown_0xa8)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_unknown_0xac offsetof(::er::paramdef::network_msg_param_st, unknown_0xac)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_unknown_0xb0 offsetof(::er::paramdef::network_msg_param_st, unknown_0xb0)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_unknown_0xb4 offsetof(::er::paramdef::network_msg_param_st, unknown_0xb4)
#define ERD_OFFSET_NETWORK_MSG_PARAM_ST_pad2_new offsetof(::er::paramdef::network_msg_param_st, pad2_new)
