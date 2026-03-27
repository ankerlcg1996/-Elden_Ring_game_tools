// 

using System.ComponentModel;

namespace TarnishedTool.Enums.ParamEnums.SpEffect;

public enum StateInfo : ushort
{
    None = 0,
    Poison = 2,
    Unknown3 = 3,
    [Description("Durability Damage")] DurabilityDamage = 4,
    [Description("Scarlet Rot")] ScarletRot = 5,
    [Description("Blood Loss")] BloodLoss = 6,
    Ghost = 7,
    [Description("Enemy Sight Reduction")] EnemySightReduction = 8,

    [Description("Tranquil Walk of Peace")]
    TranquilWalkOfPeace = 9,
    [Description("Remove Poison")] RemovePoison = 10,
    [Description("Remove Scarlet Rot")] RemoveScarletRot = 11,
    [Description("Remove Blood Loss")] RemoveBloodLoss = 12,
    [Description("Remove All Status")] RemoveAllStatus = 13,
    [Description("Humanity Stolen")] HumanityStolen = 14,
    Telescope = 15,
    [Description("Warp to Grace")] WarpToGrace = 16,
    Revival = 17,
    [Description("Dispel Black Phantom")] DispelBlackPhantom = 19,
    Unknown22 = 22,
    OnReviveMagic = 23,
    [Description("Disables Spell Usage")] DisablesSpellUsage = 24,
    [Description("Right-hand Buff VFX")] RightHandBuffVfx = 28,
    [Description("Body Buff VFX")] BodyBuffVfx = 29,

    [Description("Ghost Parameter Change")]
    GhostParameterChange = 31,
    [Description("Middle of Paralysis")] MiddleOfParalysis = 32,
    [Description("Giant Slime Freezing")] GiantSlimeFreezing = 34,
    Unknown35 = 35,
    Unknown36 = 36,
    Unknown37 = 37,
    [Description("Sound Feed")] SoundFeed = 39,
    [Description("Greater Body Buff VFX")] GreaterBodyBuffVfx = 40,
    [Description("Flash Sweat")] FlashSweat = 41,
    [Description("HP Recovery")] HpRecovery = 42,
    Unknown43 = 43,
    Unknown44 = 44,
    Unknown45 = 45,

    [Description("Modify Target Priority")]
    ModifyTargetPriority = 46,
    [Description("Disable Fall Damage")] DisableFallDamage = 47,
    [Description("Increase Damage")] IncreaseDamage = 48,
    [Description("Increase Defense")] IncreaseDefense = 49,

    [Description("HP/FP/Stamina Recovery")]
    HpFpStaminaRecovery = 50,
    [Description("Pledge Effect Test")] PledgeEffectTest = 52,

    [Description("Modify Enemy Listen Reduction")]
    ModifyEnemyListenReduction = 54,
    HostDeath = 55,

    [Description("Point Light Source Equipped")]
    PointLightSourceEquipped = 58,

    [Description("Your Message Was Rated")]
    YourMessageWasRated = 59,
    [Description("Magic Buff VFX")] MagicBuffVfx = 60,
    [Description("Magic Weapon Buff VFX")] MagicWeaponBuffVfx = 61,
    [Description("Fire Weapon Buff VFX")] FireWeaponBuffVfx = 62,

    [Description("Enchanted Weapon Buff VFX")]
    EnchantedWeaponBuffVfx = 64,
    Unknown65 = 65,
    [Description("Modify Item Discovery")] ModifyItemDiscovery = 66,
    [Description("Tears of Denial VFX")] TearsOfDenialVfx = 69,

    [Description("Is Dead Test Condition")]
    IsDeadTestCondition = 70,
    [Description("Spell Power Boost")] SpellPowerBoost = 71,
    [Description("Green Blossom VFX")] GreenBlossomVfx = 75,
    [Description("Modify Rune Gain")] ModifyRuneGain = 76,
    Unknown78 = 78,
    Unknown79 = 79,
    Unknown91 = 91,

    [Description("Applies Chameleon Effect")]
    AppliesChameleonEffect = 95,

    [Description("Applies Dragon Form Effect")]
    AppliesDragonFormEffect = 96,
    [Description("MP Detection")] MpDetection = 98,

    [Description("MP Wait for Cooperation")]
    MpWaitForCooperation = 99,
    [Description("MP Cooperation")] MpCooperation = 100,
    [Description("MP Cooperation Sent")] MpCooperationSent = 101,
    [Description("Wax Slow Down")] WaxSlowDown = 102,

    [Description("Used for Evil Eye Effect")]
    UsedForEvilEyeEffect = 103,

    [Description("MP Wait for Cooperation")]
    MpWaitForCooperation104 = 104,
    [Description("MP Cooperation LV 1")] MpCooperationLv1 = 105,
    [Description("MP Cooperation LV 2")] MpCooperationLv2 = 106,
    [Description("MP Cooperation LV 3")] MpCooperationLv3 = 107,

    [Description("Used for Evil Eye Effect (1)")]
    UsedForEvilEyeEffect1 = 108,

    [Description("Used for Evil Eye Effect (2)")]
    UsedForEvilEyeEffect2 = 109,
    [Description("Counter Damage")] CounterDamage = 110,
    [Description("1409F7282_HKS")] Hks1409F7282 = 112,
    Unknown113 = 113,
    Unknown114 = 114,

    [Description("Backstep Animation Change")]
    BackstepAnimationChange = 115,
    [Description("Death Blight")] DeathBlight = 116,

    [Description("Instant Death Animation")]
    InstantDeathAnimation = 117,
    [Description("Cure Death Blight")] CureDeathBlight = 118,
    Unknown119 = 119,

    [Description("Damage Level Change before Poise Break")]
    DamageLevelChangeBeforePoiseBreak = 120,
    [Description("Damage Level Change")] DamageLevelChange = 121,
    Unknown122 = 122,

    [Description("Trigger on Roll (Head)")]
    TriggerOnRollHead = 123,

    [Description("Trigger on Roll (Body)")]
    TriggerOnRollBody = 124,

    [Description("Trigger on Roll (Hands)")]
    TriggerOnRollHands = 125,

    [Description("Trigger on Roll (Legs)")]
    TriggerOnRollLegs = 126,
    [Description("Mimic Sleep")] MimicSleep127 = 127,
    [Description("Mimic Sleep")] MimicSleep128 = 128,
    [Description("Mimic Sleep")] MimicSleep130 = 130,
    [Description("Mimic Sleep")] MimicSleep131 = 131,
    [Description("Change Team Type")] ChangeTeamType = 132,

    [Description("Enable Developer Messages")]
    EnableDeveloperMessages = 133,
    [Description("Iron Flesh")] IronFlesh = 134,
    [Description("Mimic Sleep")] MimicSleep135 = 135,
    [Description("Death Blight")] DeathBlight136 = 136,
    [Description("Resonance Lvl 0")] ResonanceLvl0 = 137,
    [Description("Resonance Lvl 1")] ResonanceLvl1 = 138,
    [Description("Resonance Lvl 2")] ResonanceLvl2 = 139,
    [Description("Resonance Lvl 3")] ResonanceLvl3 = 140,
    [Description("Resonance Lvl 4")] ResonanceLvl4 = 141,

    [Description("NPC Behavior ID Change")]
    NpcBehaviorIdChange = 142,
    [Description("Character Respawn")] CharacterRespawn = 143,
    Unknown144 = 144,
    [Description("Restore Durability")] RestoreDurability = 146,
    [Description("Cast Light")] CastLight = 147,
    Unknown148 = 148,
    [Description("White Relief Magic")] WhiteReliefMagic = 149,
    [Description("Black Relief Magic")] BlackReliefMagic = 150,

    [Description("Lightning Weapon Buff VFX")]
    LightningWeaponBuffVfx = 151,

    [Description("Enable Attack Effect against Enemy")]
    EnableAttackEffectAgainstEnemy = 152,

    [Description("Enable Attack Effect against Player")]
    EnableAttackEffectAgainstPlayer = 153,
    [Description("Block Estus Usage")] BlockEstusUsage = 154,
    [Description("Modify Poise")] ModifyPoise = 155,
    [Description("Disable Durability")] DisableDurability = 156,
    [Description("Transient Curse")] TransientCurse = 157,
    [Description("Left-hand Buff VFX")] LeftHandBuffVfx = 158,

    [Description("Destroy Accessory but Save Runes")]
    DestroyAccessoryButSaveRunes = 159,

    [Description("Rare Ring of Sacrifice [DS1]")]
    RareRingOfSacrificeDs1 = 160,
    [Description("Warp to Grace B")] WarpToGraceB = 161,
    [Description("Warp to Grace C")] WarpToGraceC = 162,
    [Description("Warp to Grace D")] WarpToGraceD = 163,
    [Description("Warp to Grace E")] WarpToGraceE = 164,
    [Description("Warp to Grace A")] WarpToGraceA = 165,
    [Description("Pledge Discarded")] PledgeDiscarded = 166,

    [Description("Full Body Transform VFX")]
    FullBodyTransformVfx = 167,
    [Description("Bow Distance Change")] BowDistanceChange = 168,
    Unknown169 = 169,

    [Description("Karmic Justice Counter")]
    KarmicJusticeCounter = 170,

    [Description("Used for Behavior Change")]
    UsedForBehaviorChange = 171,
    Ritual = 173,
    [Description("Power of Evil Spirits")] PowerOfEvilSpirits = 174,
    [Description("Revival Production")] RevivalProduction = 175,
    [Description("Aural Decoy")] AuralDecoy = 176,
    [Description("Soul Coin (Beastly)")] SoulCoinBeastly = 177,
    [Description("Death Effect Lv1")] DeathEffectLv1 = 179,
    [Description("Death Effect Lv2")] DeathEffectLv2 = 180,
    [Description("Death Effect Lv3")] DeathEffectLv3 = 181,
    [Description("Death Effect Lv4")] DeathEffectLv4 = 182,
    BlueSignVisualization = 183,
    [Description("Hide Weapon")] HideWeapon = 184,
    Unknown185 = 185,
    Unknown186 = 186,
    Unknown188 = 188,
    Unknown189 = 189,
    Unknown190 = 190,
    Unknown191 = 191,
    Unknown192 = 192,

    [Description("Modify Effect Duration")]
    ModifyEffectDuration = 193,

    [Description("Increase Number of Magic Uses")]
    IncreaseNumberOfMagicUses = 194,
    Unknown195 = 195,
    IfWorldChrManNull = 196,

    [Description("Enhance Thrusting Counter Attacks")]
    EnhanceThrustingCounterAttacks = 197,
    [Description("Cure Death Blight")] CureDeathBlight198 = 198,
    [Description("Apply Kill Effect")] ApplyKillEffect = 199,
    [Description("Power Within VFX")] PowerWithinVfx = 200,
    VowOfSilenceVisual = 201,
    [Description("Dragon Roar")] DragonRoar = 202,

    [Description("Decrease Number of Times Magic Is Used")]
    DecreaseNumberOfTimesMagicIsUsed = 203,

    [Description("Strengthen Guard [Large]")]
    StrengthenGuardLarge = 204,
    [Description("Holy Weapon Buff VFX")] HolyWeaponBuffVfx = 205,
    Unknown206 = 206,
    [Description("Jailer HP Drain")] JailerHpDrain = 207,

    [Description("Trigger on Enemy Backstab")]
    TriggerOnEnemyBackstab = 213,
    Unknown221 = 221,
    ItemBan = 222,
    Unknown223 = 223,
    Unknown224 = 224,

    [Description("No Use of Prohibited Items in Arena")]
    NoUseOfProhibitedItemsInArena = 232,
    [Description("Calamity Ring")] CalamityRing = 237,
    Oil = 252,
    Fire = 253,
    Unknown258 = 258,
    Unknown259 = 259,
    Frostbite = 260,

    [Description("Remove Effect If Torch In Hand")]
    RemoveEffectIfTorchInHand = 261,
    [Description("Worm Recovery (Torch)")] WormRecoveryTorch = 262,
    SetCultBool = 264,
    FallDeathIsDisabled = 266,
    [Description("AI Eye Angle")] AiEyeAngle = 267,
    Unknown269 = 269,

    [Description("Follow and Warp to Player (Spirit Summon)")]
    FollowAndWarpToPlayerSpiritSummon = 270,
    Unknown271 = 271,
    Unknown272 = 272,

    [Description("Player Behavior ID Change")]
    PlayerBehaviorIdChange = 275,
    [Description("Cure Frostbite")] CureFrostbite = 276,
    [Description("Trigger Great Rune")] TriggerGreatRune = 277,
    DisableSpellEffect = 278,
    WatchdogTriggerForHost = 280,
    WetSystemTrigger = 281,

    [Description("NPC Correction for Coop")]
    NpcCorrectionForCoop = 282,

    [Description("Tears of Denial Trigger")]
    TearsOfDenialTrigger = 283,
    [Description("Heal Spell")] HealSpell = 284,
    Unknown285 = 285,
    [Description("Weak Movement Slow")] WeakMovementSlow = 286,
    [Description("Strong Movement Slow")] StrongMovementSlow = 287,

    [Description("Trigger on Critical Hit (HP)")]
    TriggerOnCriticalHitHp = 288,

    [Description("Trigger on Critical Hit (FP)")]
    TriggerOnCriticalHitFp = 289,

    [Description("Extend Roll Invincibility")]
    ExtendRollInvincibility = 290,

    [Description("Grants Roll Invisibility")]
    GrantsRollInvisibility = 291,
    Repair1 = 292,
    [Description("Change Durability")] ChangeDurability = 293,
    Unknown294 = 294,
    Unknown295 = 295,
    Unknown296 = 296,

    [Description("Not in Multiplayer Session")]
    NotInMultiplayerSession = 297,

    [Description("Trigger on Player Backstab")]
    TriggerOnPlayerBackstab = 298,

    [Description("Enemies Attack Invaders")]
    EnemiesAttackInvaders299 = 299,

    [Description("Enemies Attack Invaders")]
    EnemiesAttackInvaders300 = 300,
    [Description("Law of Regression")] LawOfRegression = 301,
    [Description("Warp to Grace")] WarpToGrace302 = 302,
    [Description("Accumulator 1")] Accumulator1 = 303,
    [Description("Accumulator 2")] Accumulator2 = 304,
    [Description("Accumulator 3")] Accumulator3 = 305,
    [Description("Accumulator 4")] Accumulator4 = 306,
    [Description("Accumulator 5")] Accumulator5 = 307,
    [Description("Accumulator 6")] Accumulator6 = 308,
    [Description("Accumulator 7")] Accumulator7 = 309,
    [Description("Accumulator 8")] Accumulator8 = 310,
    [Description("Accumulator 9")] Accumulator9 = 311,
    Unknown312 = 312,
    [Description("Seek Guidance")] SeekGuidance = 313,

    [Description("All Active Accumulators")]
    AllActiveAccumulators = 314,

    [Description("Scale Attack Power with Equip Load")]
    ScaleAttackPowerWithEquipLoad = 315,
    CalcCorrectGraph33 = 316,
    [Description("Revoke Online Penalty")] RevokeOnlinePenalty = 317,
    [Description("Dead Again")] DeadAgain = 318,
    Unknown319 = 319,
    Unknown320 = 320,
    [Description("Reverse Hollowing")] ReverseHollowing = 321,
    ChrAsmStyleHksLefthand = 322,
    ChrAsmStyleHksRighthand = 323,
    Unknown324 = 324,
    [Description("Wet Aspect Param")] WetAspectParam = 325,

    [Description("Switch Animation Gender")]
    SwitchAnimationGender = 326,
    Ember = 327,
    [Description("Change Durability")] ChangeDurability328 = 328,
    GetEstusCharge = 329,

    [Description("Online Check Reset Event Flag 2100")]
    OnlineCheckResetEventFlag2100 = 330,
    AiParam1 = 331,
    AiParamEffect16189 = 332,

    [Description("Spell Enhance (+6 to ID)")]
    SpellEnhancePlus6ToId = 333,

    [Description("Bullet Behavior ID Change")]
    BulletBehaviorIdChange = 334,

    [Description("Trigger during Critical Hit")]
    TriggerDuringCriticalHit = 335,
    [Description("Summon Torrent")] SummonTorrent = 336,
    Unknown337 = 337,
    Unknown338 = 338,
    Reload = 339,
    Unknown342 = 342,
    Unknown343 = 343,
    Unknown344 = 344,
    Unknown345 = 345,
    Unknown346 = 346,
    Unknown347 = 347,
    Unknown348 = 348,
    Unknown349 = 349,
    Unknown350 = 350,
    Unknown351 = 351,
    Unknown352 = 352,
    Unknown353 = 353,
    Unknown354 = 354,
    Unknown355 = 355,
    Unknown356 = 356,
    [Description("Trigger Spirit Summon")] TriggerSpiritSummon = 357,
    Unknown358 = 358,
    Unknown359 = 359,
    Unknown360 = 360,
    Unknown361 = 361,
    Unknown362 = 362,
    Unknown363 = 363,
    Unknown364 = 364,
    Unknown365 = 365,
    Unknown366 = 366,

    [Description("Enhance Critical Attacks")]
    EnhanceCriticalAttacks = 367,
    Unknown368 = 368,
    Unknown369 = 369,
    Unknown370 = 370,
    Unknown371 = 371,
    Unknown372 = 372,
    Unknown373 = 373,
    Unknown375 = 375,
    Unknown376 = 376,
    Unknown377 = 377,
    Unknown378 = 378,

    [Description("Trigger in Presence of Blood Loss")]
    TriggerInPresenceOfBloodLoss = 379,

    [Description("Trigger in Presence of Rot")]
    TriggerInPresenceOfRot = 380,
    Unknown381 = 381,
    Unknown382 = 382,
    [Description("Straggler - Stop Act")] StragglerStopAct = 383,
    [Description("Determination Buff")] DeterminationBuff384 = 384,
    [Description("Determination Buff")] DeterminationBuff385 = 385,
    Unknown387 = 387,
    Unknown388 = 388,
    Unknown389 = 389,
    [Description("Pacify Wild Animals")] PacifyWildAnimals = 390,
    Unknown391 = 391,
    Unknown392 = 392,
    Unknown393 = 393,

    [Description("Crystal Dart Confusion for Enemy")]
    CrystalDartConfusionForEnemy = 394,
    [Description("Unknown - Swarm Pot?")] UnknownSwarmPot = 395,
    Unknown396 = 396,
    Unknown397 = 397,
    [Description("Pull towards Caster")] PullTowardsCaster = 398,
    Unknown399 = 399,
    Unknown402 = 402,
    Unknown403 = 403,
    Unknown404 = 404,
    [Description("Reveal Phantom Signs")] RevealPhantomSigns = 405,

    [Description("Request Friendly Phantom")]
    RequestFriendlyPhantom = 406,

    [Description("Answer Phantom Request")]
    AnswerPhantomRequest = 407,
    [Description("Encourage Invasion")] EncourageInvasion = 408,

    [Description("Send Summon Sign to Pool")]
    SendSummonSignToPool = 409,

    [Description("Send Invasion Sign to Pool")]
    SendInvasionSignToPool = 410,

    [Description("Activate Spirit Summon")]
    ActivateSpiritSummon = 411,
    Unknown412 = 412,
    Unknown413 = 413,
    Unknown414 = 414,
    Unknown415 = 415,
    Unknown416 = 416,
    Unknown417 = 417,
    Unknown418 = 418,

    [Description("Unknown - Enemy Bullet?")]
    UnknownEnemyBullet = 419,

    [Description("Trigger Fall Immunity (Spiritspring)")]
    TriggerFallImmunitySpritspring420 = 420,
    Unknown421 = 421,
    Unknown422 = 422,
    Unknown423 = 423,
    Unknown424 = 424,
    Unknown425 = 425,
    Unknown426 = 426,
    Unknown427 = 427,
    Unknown428 = 428,
    Unknown429 = 429,
    Unknown430 = 430,
    Unknown431 = 431,

    [Description("Spirit Summon - Phantom Color Change?")]
    SpiritSummonPhantomColorChange = 432,

    [Description("Forced Torrent Dismount")]
    ForcedTorrentDismount = 433,
    Unknown435 = 435,
    Sleep = 436,
    Madness = 437,
    [Description("Cure Sleep")] CureSleep = 438,
    [Description("Cure Madness")] CureMadness = 439,

    [Description("Purifying Crystal Tear - Purify Mohg's Nihil")]
    PurifyingCrystalTearPurifyMohgsNihil = 440,
    [Description("Mohg's Great Rune")] MohgsGreatRune = 441,
    Unknown442 = 442,
    [Description("Triggers Shockwave")] TriggersShockwave = 443,
    Unknown445 = 445,
    Unknown446 = 446,
    [Description("Phantom Great Rune")] PhantomGreatRune = 447,

    [Description("Heal Invader when Blessed Enemy Kills Player")]
    HealInvaderWhenBlessedEnemyKillsPlayer = 448,
    [Description("Malenia's Great Rune")] MaleniasGreatRune = 449,

    [Description("Reduce Headshot Impact")]
    ReduceHeadshotImpact = 450,

    [Description("Is Damaged by Healing Incantations")]
    IsDamagedByHealingIncantations = 452,

    [Description("Fade after Death - Normal")]
    FadeAfterDeathNormal = 453,

    [Description("Fade after Death - Blue")]
    FadeAfterDeathBlue = 454,

    [Description("Fade after Death - Boss")]
    FadeAfterDeathBoss = 455,
    Unknown456 = 456,
    Unknown457 = 457,
    [Description("Attempt Invasion")] AttemptInvasion = 458,
    Unknown459 = 459,
    Unknown460 = 460,
    Unknown461 = 461,
    Unknown462 = 462,
    Unknown463 = 463,

    [Description("Trigger Fall Immunity (Spiritspring)")]
    TriggerFallImmunitySpiritspring464 = 464,
    Unknown465 = 465,
    [Description("Trigger on Crouch")] TriggerOnCrouch = 466,
    [Description("Apply Behavior Effect")] ApplyBehaviorEffect = 467,
    Unknown468 = 468,
    Unknown469 = 469,
    Unknown470 = 470,

    [Description("Unknown - Hefty Furnace Pot")]
    UnknownHeftyFurnacePot = 471,
    [Description("Scadutree Fragment")] ScadutreeFragment = 472,

    [Description("Unknown - Spirit Raisin/Fine Crucible Feather Talisman")]
    UnknownSpiritRaisinFineCrucibleFeatherTalisman = 473,

    [Description("Unknown - Man-Fly Effect")]
    UnknownManFlyEffect = 474,

    [Description("Apply Spirit Summon Death Effect")]
    ApplySpiritSummonDeathEffect = 475,

    [Description("Enhance Spirit Summon Attack")]
    EnhanceSpiritSummonAttack = 476,

    [Description("Transformation - Prevent Rock Heart")]
    TransformationPreventRockHeart = 477,

    [Description("Transformation - Prevent Priestess Heart")]
    TransformationPreventPriestessHeart = 478,

    [Description("Transformation - Prevent Lamenter's Mask")]
    TransformationPreventLamentersMask = 479,
    Unknown480 = 480,

    [Description("Trigger after Successful Critical Attack")]
    TriggerAfterSuccessfulCriticalAttack = 483,
    Unknown484 = 484,
    Unknown485 = 485,

    [Description("Unknown - Hefty Fire Pot")]
    UnknownHeftyFirePot = 486,
    Unknown487 = 487,
    Unknown488 = 488,
    Unknown489 = 489,
    Unknown490 = 490,
    Unknown491 = 491,
    Unknown492 = 492,
    Unknown495 = 495,

    [Description("Trigger after Flask Consumption")]
    TriggerAfterFlaskConsumption = 496,
    Unknown498 = 498,

    [Description("Remove Stamina Consumption")]
    RemoveStaminaConsumption = 499,

    [Description("Convert Elemental Damage into HP")]
    ConvertElementalDamageIntoHp = 500,

    [Description("Trigger after Holding Stance #1")]
    TriggerAfterHoldingStance1 = 502,

    [Description("Trigger after Holding Stance #2")]
    TriggerAfterHoldingStance2 = 503,

    [Description("Trigger after Holding Stance #3")]
    TriggerAfterHoldingStance3 = 504,

    [Description("Reduce FP Cost of Spirit Summons")]
    ReduceFpCostOfSpiritSummons = 505,
    [Description("Euporia Weapon Effect")] EuporiaWeaponEffect = 506,
    Unknown507 = 507,

    [Description("Ancient Dragon-form - Unknown")]
    AncientDragonFormUnknown = 508,
    Unknown509 = 509,
    Unknown510 = 510,
    Unknown511 = 511,
    Unknown512 = 512,
    SkipSpCategoryCheck1 = 1000,
    SkipSpCategoryCheck2 = 1001,
    Unknown59999 = 59999
}