#include "global.h"
#include "assertf.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "bg.h"
#include "config/debug.h"
#include "constants/battle_pyramid.h"
#include "constants/characters.h"
#include "constants/field_weather.h"
#include "constants/unbound_start_menu.h"
#include "constants/unbound_start_menu.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "datetime.h"
#include "debug.h"
#include "decompress.h"
#include "even_sprite.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "frontier_pass.h"
#include "gba/defines.h"
#include "gba/io_reg.h"
#include "gba/isagbprint.h"
#include "gba/macro.h"
#include "gba/types.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item_menu.h"
#include "link.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokenav.h"
#include "rtc.h"
#include "safari_zone.h"
#include "save_dialog.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "start_menu.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trainer_card.h"
#include "unbound_start_menu.h"
#include "window.h"
#include <string.h>
#include <sys/cdefs.h>

typedef bool8 (*Usm_MenuCB)(void) ;

#define USM_MAX_ICON_COUNT 7
#define USM_ICON_WIDTH 30
#define USM_BANNER_WIDTH 224
#define USM_ICON_YPOS 128

enum Usm_IconTiletags {
    USM_TILETAG_POKEDEX = 0x1000,
    USM_TILETAG_PARTY,
    USM_TILETAG_BAGM,
    USM_TILETAG_BAGF,
    USM_TILETAG_POKENAV,
    USM_TILETAG_DEXNAV,
    USM_TILETAG_TRAINER,
    USM_TILETAG_SAVE,
    USM_TILETAG_OPTIONS,
    USM_TILETAG_RETIRE,
    USM_TILETAG_DEBUG,
    USM_TILETAG_HAND,
};

enum Usm_Paltags {
    USM_PALTAG_ICON = 0x1000 | BLEND_IMMUNE_FLAG,
};

enum Usm_Activation {
    USM_INACTIVE,
    USM_ACTIVE,
};

enum Usm_Windows {
    USM_WIN_NAME,
    USM_WIN_CLOCK,
    USM_WIN_HINTS,
    USM_WIN_COUNT,
};

struct Usm_VisibleIcons {
    u8 iconIndex[USM_MAX_ICON_COUNT];
    u8 count;
};

struct Usm_State {
    MainCallback savedCb;
    u8 loadState;
    u8 selectedIcon;
    u8 windowCount;
    u8 scrollOffset;
    u8 items[USM_ICO_COUNT];
    u8 itemCount;
    struct Usm_VisibleIcons visible;
};

struct Usm_Memory {
    struct Usm_State state;
    u8 spriteIds[USM_MAX_ICON_COUNT];
    u8 windowIds[USM_WIN_COUNT];
};

struct Usm_MenuItem {
    enum Usm_Icons iconId;
    const struct SpriteTemplate* template;
    const struct CompressedSpriteSheet* sheet;
    const u8* label;
    bool8 shouldFade;
    Usm_MenuCB callback;
};

// Graphics
static const u32 sPokedexIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconPokedex.png", ".4bpp.smol");
static const u16 sIconPal[] = INCGFX_U16("graphics/unbound_start_menu/usm_iconPokedex.png", ".gbapal");
static const u32 sPartyIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconParty.png", ".4bpp.smol");
static const u32 sBagMIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconBagM.png", ".4bpp.smol");
static const u32 sBagFIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconBagF.png", ".4bpp.smol");
static const u32 sPokenavIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconPokenav.png", ".4bpp.smol");
static const u32 sDexnavIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconDexnav.png", ".4bpp.smol");
static const u32 sTrainerIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconTrainer.png", ".4bpp.smol");
static const u32 sSaveIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconSave.png", ".4bpp.smol");
static const u32 sOptionsIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconOptions.png", ".4bpp.smol");
static const u32 sDebugIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconDebug.png", ".4bpp.smol");
static const u32 sRetireIconGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconRetire.png", ".4bpp.smol");

static const u32 sUsmHandGfx[] = INCGFX_U32("graphics/unbound_start_menu/usm_iconHand.png", ".4bpp.smol");

static const u32 sUsmBgTiles[] = INCGFX_U32("graphics/unbound_start_menu/usm_tiles.png", ".4bpp.smol");
static const u32 sUsmBgTilemap[] = INCGFX_U32("graphics/unbound_start_menu/usm_tilemap.bin", ".smolTM");
static const u16 sUsmBgPalette[] = INCGFX_U16("graphics/unbound_start_menu/usm_tiles.png", ".gbapal");

enum FontColor {
    FONT_WHITE,
    FONT_BLACK,
};

static const struct WindowTemplate sUsmWindowTemplates[] = {
    [USM_WIN_NAME] =
        {.bg = 0, .tilemapLeft = 0, .tilemapTop = 11, .width = 7, .height = 2, .paletteNum = 14, .baseBlock = 48},
    [USM_WIN_CLOCK] =
        {.bg = 0, .tilemapLeft = 20, .tilemapTop = 11, .width = 10, .height = 2, .paletteNum = 14, .baseBlock = 62},
    [USM_WIN_HINTS] =
        {.bg = 0, .tilemapLeft = 10, .tilemapTop = 18, .width = 20, .height = 2, .paletteNum = 14, .baseBlock = 82},
    DUMMY_WIN_TEMPLATE};


static const u8 sUsmWinFontColors[][3] = {
    [FONT_BLACK] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY},
    [FONT_WHITE] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY},
};

static const struct OamData sIconOam = {
    .x = 0,
    .y = 0,
    .objMode = ST_OAM_OBJ_NORMAL,
    .affineMode = ST_OAM_AFFINE_OFF,
    .affineParam = 0,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .matrixNum = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
};

static const union AnimCmd sAnim_Icon_Frame1[] = {ANIMCMD_FRAME(0, 30), ANIMCMD_END};

static const union AnimCmd sAnim_Icon_Frame2[] = {ANIMCMD_FRAME(16, 30), ANIMCMD_END};

static const union AnimCmd* const sIconAnimTable[] = {
    sAnim_Icon_Frame1,
    sAnim_Icon_Frame2,
};

static const union AffineAnimCmd sAffineAnim_RotateAndScale[] = {
    AFFINEANIMCMD_FRAME(10, 10, 0, 5),
    AFFINEANIMCMD_FRAME(-5, -5, 0, 10),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),

    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_JUMP(3),
};

static const union AffineAnimCmd* const sIconAffineAnimTable[] = {
    sAffineAnim_RotateAndScale
};

#define ICON_TEMPLATE(_icon, name)                                    \
    static const struct SpriteTemplate sSpriteTemplate_##name = {     \
        .tileTag = USM_TILETAG_##_icon,                               \
        .paletteTag = USM_PALTAG_ICON,                                \
        .oam = &sIconOam,                                             \
        .anims = sIconAnimTable,                                      \
        .affineAnims = sIconAffineAnimTable,                          \
        .callback = SpriteCallbackDummy,                              \
    };                                                                \
    static const struct CompressedSpriteSheet sSpriteSheet_##name = { \
        .data = s##name##IconGfx, .size = 0x400, .tag = USM_TILETAG_##_icon};

ICON_TEMPLATE(POKEDEX, Pokedex)
ICON_TEMPLATE(PARTY, Party)
ICON_TEMPLATE(BAGM, BagM)
ICON_TEMPLATE(BAGF, BagF)
ICON_TEMPLATE(POKENAV, Pokenav)
ICON_TEMPLATE(DEXNAV, Dexnav)
ICON_TEMPLATE(TRAINER, Trainer)
ICON_TEMPLATE(SAVE, Save)
ICON_TEMPLATE(OPTIONS, Options)
ICON_TEMPLATE(DEBUG, Debug)
ICON_TEMPLATE(RETIRE, Retire)

static const struct SpritePalette sSpritePalette_Icons = {.data = sIconPal, .tag = USM_PALTAG_ICON};

// Static Variables
static EWRAM_DATA struct Usm_Memory* sUsmMemory;
static EWRAM_DATA struct Usm_State* sUsmState;
static EWRAM_DATA u8 sUsmSavedIcon = 0;
static EWRAM_DATA u8 sUsmSavedScrollOffset = 0;

// Tasks
static void Task_UsmHandleMainInput(u8 taskId);
static void Task_UsmHandleMoveItems(u8 taskId);
static void Task_UsmFadeAndRunCallback(u8 taskId);
static void Task_UsmRunCallbackNoFade(u8 taskId);

// Static Functions
static void Usm_LoadBgGfx(void);
static void Usm_CreateIcons(s16 x, s16 y);
static void Usm_LoadIconGfx(void);
static struct Sprite* Usm_GetSelectedSprite(void);
static void Usm_BuildVisibleList(void);
static void Usm_SetupWindows();
static u8 Usm_GetWindowBaseColor(u8 winId);
static void Usm_PrintText(u8 winId, u8 fontId, s16 x, s16 y, const u8* color, const u8* str);
static void Usm_PrintIconLabel(void);
static void Usm_PrintClockText();
static void Usm_PrintButtonHints();
static void Usm_AnimateSelectedIcon(void);
static struct Sprite* Usm_GetIconSprite(u8 iconId);
static void Usm_ExitStartMenu(void);
static u32 Usm_ReadKeys(void);
static void Usm_HandleDPadInput(u8 input);
static void GetCurrentDateTime(struct DateTime* dateTime);
static void BuildDateTimeString(void);
static void Usm_BuildMenuItems(void);
static void Usm_BuildDefaultMenuItems(void);
static void Usm_AddMenuItem(enum Usm_Icons icon);
static u32 Usm_CreateHandSprite(s16 x, s16 y);
static void Usm_MoveItem(s8 dir);
static void Usm_RedrawIcons(bool32 startAffine);
static void Usm_DestroyVisibleIcons(void);
static void Usm_StartIconAnim(u8 iconId);
static void Usm_SaveItems(void);
static bool32 Usm_IsItemAvailable(enum Usm_Icons item);
static bool32 IsPlayerInBattlePyramid(void);

// Menu Callbacks
static bool8 StartMenuPokedexCallback(void);
static bool8 StartMenuPokemonCallback(void);
static bool8 StartMenuBagCallback(void);
static bool8 StartMenuPokeNavCallback(void);
static bool8 StartMenuPlayerNameCallback(void);
static bool8 StartMenuSaveCallback(void);
static bool8 StartMenuOptionCallback(void);
static bool8 StartMenuExitCallback(void);
static bool8 StartMenuSafariZoneRetireCallback(void);
static bool8 StartMenuLinkModePlayerNameCallback(void);
static bool8 StartMenuBattlePyramidRetireCallback(void);
static bool8 StartMenuBattlePyramidBagCallback(void);
static bool8 StartMenuDebugCallback(void);
static bool8 StartMenuDexNavCallback(void);

static const struct Usm_MenuItem sUsmMenuItems[USM_ICO_COUNT] = {
    [USM_ICO_POKEDEX] =
        {
            .iconId = USM_ICO_POKEDEX,
            .template = &sSpriteTemplate_Pokedex,
            .sheet = &sSpriteSheet_Pokedex,
            .label = COMPOUND_STRING("Pokédex"),
            .shouldFade = TRUE,
            .callback = StartMenuPokedexCallback,
        },
    [USM_ICO_PARTY] =
        {
            .iconId = USM_ICO_PARTY,
            .template = &sSpriteTemplate_Party,
            .sheet = &sSpriteSheet_Party,
            .label = COMPOUND_STRING("Pokémon"),
            .shouldFade = TRUE,
            .callback = StartMenuPokemonCallback,
        },
    [USM_ICO_BAGM] =
        {
            .iconId = USM_ICO_BAGM,
            .template = &sSpriteTemplate_BagM,
            .sheet = &sSpriteSheet_BagM,
            .label = COMPOUND_STRING("Bag"),
            .shouldFade = TRUE,
            .callback = StartMenuBagCallback,
        },
    [USM_ICO_BAGF] =
        {
            .iconId = USM_ICO_BAGF,
            .template = &sSpriteTemplate_BagF,
            .sheet = &sSpriteSheet_BagF,
            .label = COMPOUND_STRING("Bag"),
            .shouldFade = TRUE,
            .callback = StartMenuBagCallback,
        },
    [USM_ICO_POKENAV] =
        {
            .iconId = USM_ICO_POKENAV,
            .template = &sSpriteTemplate_Pokenav,
            .sheet = &sSpriteSheet_Pokenav,
            .label = COMPOUND_STRING("PokéNav"),
            .shouldFade = TRUE,
            .callback = StartMenuPokeNavCallback,
        },
    [USM_ICO_DEXNAV] =
        {
            .iconId = USM_ICO_DEXNAV,
            .template = &sSpriteTemplate_Dexnav,
            .sheet = &sSpriteSheet_Dexnav,
            .label = COMPOUND_STRING("DexNav"),
            .shouldFade = TRUE,
            .callback = StartMenuDexNavCallback,
        },
    [USM_ICO_TRAINER] =
        {
            .iconId = USM_ICO_TRAINER,
            .template = &sSpriteTemplate_Trainer,
            .sheet = &sSpriteSheet_Trainer,
            .label = COMPOUND_STRING("Trainer"),
            .shouldFade = TRUE,
            .callback = StartMenuPlayerNameCallback,
        },
    [USM_ICO_SAVE] =
        {
            .iconId = USM_ICO_SAVE,
            .template = &sSpriteTemplate_Save,
            .sheet = &sSpriteSheet_Save,
            .label = COMPOUND_STRING("Save"),
            .shouldFade = FALSE,
            .callback = StartMenuSaveCallback,
        },
    [USM_ICO_OPTIONS] =
        {
            .iconId = USM_ICO_OPTIONS,
            .template = &sSpriteTemplate_Options,
            .sheet = &sSpriteSheet_Options,
            .label = COMPOUND_STRING("Options"),
            .shouldFade = TRUE,
            .callback = StartMenuOptionCallback,
        },
    [USM_ICO_DEBUG] =
        {
            .iconId = USM_ICO_DEBUG,
            .template = &sSpriteTemplate_Debug,
            .sheet = &sSpriteSheet_Debug,
            .label = COMPOUND_STRING("Debug"),
            .shouldFade = FALSE,
            .callback = StartMenuDebugCallback,
        },
    [USM_ICO_SAFARI_RETIRE] =
        {
            .iconId = USM_ICO_SAFARI_RETIRE,
            .template = &sSpriteTemplate_Retire,
            .sheet = &sSpriteSheet_Retire,
            .label = COMPOUND_STRING("Retire"),
            .shouldFade = TRUE,
            .callback = StartMenuSafariZoneRetireCallback,
        },
    [USM_ICO_FRONTIER_RETIRE] =
        {
            .iconId = USM_ICO_SAVE,
            .template = &sSpriteTemplate_Save,
            .sheet = &sSpriteSheet_Save,
            .label = COMPOUND_STRING("Retire"),
            .shouldFade = FALSE,
            .callback = StartMenuBattlePyramidRetireCallback,
        },
};

bool8 StartMenuPokedexCallback(void)
{
    if (!gPaletteFade.active)
    {
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_OpenPokedex);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokemonCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PartyMenuFromStartMenu); // Display party menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBagCallback(void)
{
    if (IsPlayerInBattlePyramid())
    {
       return StartMenuBattlePyramidBagCallback();
    }
    else if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_BagMenuFromStartMenu); // Display bag menu
        return TRUE;
    }
    return FALSE;
}

static bool8 StartMenuPokeNavCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitPokeNav); 

        return TRUE;
    }
    return FALSE;
}

static bool8 StartMenuPlayerNameCallback(void)
{
    if (IsOverworldLinkActive()) {
        return StartMenuLinkModePlayerNameCallback();
    }
    else if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();

        if (IsOverworldLinkActive() || InUnionRoom())
            ShowPlayerTrainerCard (CB2_ReturnToFieldWithOpenMenu); // Display trainer card
        else if (FlagGet(FLAG_SYS_FRONTIER_PASS))
            ShowFrontierPass(CB2_ReturnToFieldWithOpenMenu); // Display frontier pass
        else
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card

        return TRUE;
    }
    return FALSE;
}

static bool8 StartMenuSaveCallback(void)
{
    sUsmSavedIcon = 0;
    sUsmSavedScrollOffset = 0;
    SaveDialog_InitSave();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CreateTask(Task_SaveDialogHandleSave, 0);
    return FALSE;
}

static bool8 StartMenuOptionCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitOptionMenu); // Display option menu
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;

        return TRUE;
    }

    return FALSE;
}
static bool8 UNUSED StartMenuExitCallback(void)
{
    Usm_ExitStartMenu();
    UnlockPlayerFieldControls();
    UnfreezeObjectEvents();
    return TRUE;
}

static bool8 StartMenuSafariZoneRetireCallback(void)
{
    SafariZoneRetirePrompt();
    return TRUE;
}

static bool8 StartMenuLinkModePlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        ShowTrainerCardInLink(gLocalLinkPlayerId, CB2_ReturnToFieldWithOpenMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBattlePyramidRetireCallback(void)
{
    sUsmSavedIcon = 0;
    SaveDialog_InitBattlePyramidRetire();
    LockPlayerFieldControls();
    FreezeObjectEvents();
    CreateTask(Task_SaveDialogHandleBattlePyramidRetire, 0);
    return FALSE;
}

static bool8 StartMenuBattlePyramidBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PyramidBagMenuFromStartMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuDebugCallback(void)
{
    sUsmSavedIcon = 0;
    Debug_ShowMainMenu();

return TRUE;
}

static bool8 UNUSED StartMenuDexNavCallback(void)
{
    return FALSE;
}

void Usm_InitStartMenu(void)
{
    if (!IsOverworldLinkActive()) {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }
    LockPlayerFieldControls();

    sUsmMemory = AllocZeroed(sizeof(struct Usm_Memory));

    if (sUsmMemory == NULL) {
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        return;
    }

    if (!!GetFlashLevel())
    {
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);
        SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_BG0);
    }

    sUsmState = &sUsmMemory->state;

    sUsmState->selectedIcon = sUsmSavedIcon;
    sUsmState->scrollOffset = sUsmSavedScrollOffset;
    Usm_BuildMenuItems();

    Usm_LoadBgGfx();
    Usm_SetupWindows();
    Usm_BuildVisibleList();
    Usm_PrintClockText();
    Usm_PrintButtonHints();
    Usm_PrintIconLabel();
    Usm_LoadIconGfx();
    Usm_LoadIconPalette();
    Usm_CreateIcons(0, USM_ICON_YPOS);
    Usm_StartIconAnim(sUsmState->selectedIcon);
    CreateTask(Task_UsmHandleMainInput, 0);
}

static void Usm_PrintText(u8 winId, u8 fontId, s16 x, s16 y, const u8* color, const u8* str)
{
    AddTextPrinterParameterized4(winId, fontId, x, y, 0, 0, color, TEXT_SKIP_DRAW, str);
}

static void Usm_PrintIconLabel(void)
{
    u8 iconId = sUsmState->visible.iconIndex[sUsmState->selectedIcon];
    const u8* text = sUsmMenuItems[iconId].label;
    u8 winId = sUsmMemory->windowIds[USM_WIN_NAME];
    s16 x = GetStringCenterAlignXOffset(FONT_SMALL, text, GetWindowAttribute(winId, WINDOW_WIDTH) * 8);
    FillWindowPixelBuffer(winId, PIXEL_FILL(Usm_GetWindowBaseColor(USM_WIN_NAME)));
    Usm_PrintText(winId, FONT_SMALL, x, 0, sUsmWinFontColors[FONT_WHITE], text);
    CopyWindowToVram(winId, COPYWIN_GFX);
}

static void Usm_PrintClockText()
{
    u8 winId = sUsmMemory->windowIds[USM_WIN_CLOCK];
    BuildDateTimeString();
    s16 x = GetStringCenterAlignXOffset(FONT_SHORT, gStringVar4, GetWindowAttribute(winId, WINDOW_WIDTH) * 8);
    FillWindowPixelBuffer(winId, PIXEL_FILL(Usm_GetWindowBaseColor(USM_WIN_CLOCK)));
    Usm_PrintText(sUsmMemory->windowIds[USM_WIN_CLOCK], FONT_SMALL, x, 0, sUsmWinFontColors[FONT_BLACK], gStringVar4);
    CopyWindowToVram(winId, COPYWIN_GFX);
}

static void Usm_PrintButtonHints() //worpTODO: Maybe make this dynamic like my changes to other UI? ([Select] Move and then [A] Confirm or something??)
{
    u8 winId = sUsmMemory->windowIds[USM_WIN_HINTS];
    const u8* text = COMPOUND_STRING("{SELECT_BUTTON} Move    ");
    s16 x = GetStringRightAlignXOffset(FONT_SMALL_NARROWER, text, GetWindowAttribute(winId, WINDOW_WIDTH) * 8);
    FillWindowPixelBuffer(winId, PIXEL_FILL(Usm_GetWindowBaseColor(USM_WIN_HINTS)));
    Usm_PrintText(winId, FONT_SMALL_NARROWER, x, 0, sUsmWinFontColors[FONT_WHITE], text);
    CopyWindowToVram(winId, COPYWIN_GFX);
}

static void Usm_SetupWindows()
{
    DeactivateAllTextPrinters();
    for (u32 i = 0; i < USM_WIN_COUNT; i++) {
        u8 winId = AddWindow(&sUsmWindowTemplates[i]);
        FillWindowPixelBuffer(winId, PIXEL_FILL(Usm_GetWindowBaseColor(i)));
        PutWindowTilemap(winId);
        CopyWindowToVram(winId, COPYWIN_GFX);
        sUsmMemory->windowIds[i] = winId;
        sUsmState->windowCount++;
    }
}

static u8 Usm_GetWindowBaseColor(u8 winId)
{
    switch (winId) {
        case USM_WIN_CLOCK:
            return 15;
        case USM_WIN_NAME:
            return 11;
        case USM_WIN_HINTS:
            return 10;
        default:
            return 1;
    }
}

static void Usm_LoadBgGfx(void)
{
    u8* buffer = GetBgTilemapBuffer(0);
    CpuFastFill(0, buffer, BG_SCREEN_SIZE)
    LoadBgTilemap(0, 0, 0, 0);
    DecompressAndLoadBgGfxUsingHeap(0, sUsmBgTiles, 0, 0, 0);
    DecompressDataWithHeaderWram(sUsmBgTilemap, buffer);
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    LoadPalette(sUsmBgPalette, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
    SetBgTilemapBuffer(0, buffer);
    ScheduleBgCopyTilemapToVram(0);
}

static void GetCurrentDateTime(struct DateTime* dateTime)
{
    RtcCalcLocalTime();
    ConvertTimeToDateTime(dateTime, &gLocalTime);
}

static const u8* const sWeekdayNames[WEEKDAY_COUNT] = {
    [WEEKDAY_SUN] = COMPOUND_STRING("Sun"), [WEEKDAY_MON] = COMPOUND_STRING("Mon"),
    [WEEKDAY_TUE] = COMPOUND_STRING("Tue"), [WEEKDAY_WED] = COMPOUND_STRING("Wed"),
    [WEEKDAY_THU] = COMPOUND_STRING("Thu"), [WEEKDAY_FRI] = COMPOUND_STRING("Fri"),
    [WEEKDAY_SAT] = COMPOUND_STRING("Sat"),
};

static void BuildDateTimeString(void)
{
    const u8* text = COMPOUND_STRING("{STR_VAR_1}. {STR_VAR_2}");
    struct DateTime dt;
    GetCurrentDateTime(&dt);

    //Weekday
    StringCopy(gStringVar1, sWeekdayNames[dt.dayOfWeek]);
    //Hour (Made this a 12 Hour clock)
    u8 hour = dt.hour;
    const u8* ampm = COMPOUND_STRING("AM");

    if (hour == 0) hour = 12; // Midnight
    else if (hour == 12) ampm = COMPOUND_STRING("PM"); //Noon
    else if (hour > 12) //Afternoon
    {
        hour -= 12;
        ampm = COMPOUND_STRING("PM");
    }
    ConvertIntToDecimalStringN(gStringVar2, hour, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringAppend(gStringVar2, COMPOUND_STRING(":"));
    
    u8 minStr[8];
    ConvertIntToDecimalStringN(minStr, dt.minute, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar2, minStr);
    StringAppend(gStringVar2, COMPOUND_STRING(" "));
    StringAppend(gStringVar2, ampm);

    StringExpandPlaceholders(gStringVar4, text);
}

static void Usm_AddMenuItem(enum Usm_Icons icon)
{
    if (sUsmState->itemCount >= USM_ICO_COUNT)
        return;

    sUsmState->items[sUsmState->itemCount++] = icon;
}

static bool32 Usm_ListContains(enum Usm_Icons item, u8 *list, u8 count)
{
    for (u32 i = 0; i < count; i++)
        if (list[i] == item)
            return TRUE;
    return FALSE;
}

static void Usm_InjectFirst(enum Usm_Icons newIcon) //Injects an icon before all others by moving the list up by 1 and then injecting the icon at 0
{
    for (u8 i = sUsmState->itemCount; i > 0; i--)
    {
        sUsmState->items[i] = sUsmState->items[i - 1];
    }
    sUsmState->items[0] = newIcon;
    sUsmState->itemCount++;
}

static void Usm_InjectAfter(enum Usm_Icons newIcon, enum Usm_Icons anchorIcon) //Injects an icon after the anchor icon by moving the list after the anchor icon up by 1 and then inject icon at the old anchor icon position (DexNav AFTER PokeNav)
{
    u8 anchorIdx = 0;
    
    for (u8 i = 0; i < sUsmState->itemCount; i++)
    {
        if (sUsmState->items[i] == anchorIcon)
        {
            anchorIdx = i;
            break;
        }
    }

    if (anchorIdx != 0xFF)
    {
        for (u8 i = sUsmState->itemCount; i > anchorIdx + 1; i--)
        {
            sUsmState->items[i] = sUsmState->items[i - 1];
        }
        sUsmState->items[anchorIdx + 1] = newIcon;
        sUsmState->itemCount++;
    }
    else
    {
        Usm_AddMenuItem(newIcon);
    }
}

static void Usm_BuildMenuItems(void)
{
    struct Usm_SavedItems* saved = &gSaveBlock3Ptr->usmSaved;

    sUsmState->itemCount = 0;

    if (saved->count > 0) //This loops checks for items that hare in the saved list.
    {
        for (u32 i = 0; i < saved->count; i++)
        {
            if (Usm_IsItemAvailable(saved->items[i])) Usm_AddMenuItem(saved->items[i]);
        }
    }
    else //If no items in saved list, this makes a new list to save.
    {
        Usm_BuildDefaultMenuItems();
        return;
    }

    for (u32 item = 0; item < USM_ICO_COUNT; item++) //This loop checks for items that aren't in the saved list.
    {
        if (Usm_IsItemAvailable(item) && !Usm_ListContains(item, sUsmState->items, sUsmState->itemCount))
        {
            switch (item)
            {
                case USM_ICO_POKEDEX:
                    Usm_InjectFirst(USM_ICO_POKEDEX);
                    break;
                case USM_ICO_PARTY:
                    Usm_InjectFirst(USM_ICO_PARTY);
                    break;
                case USM_ICO_FRONTIER_RETIRE:
                    Usm_InjectFirst(USM_ICO_FRONTIER_RETIRE);
                    break;
                case USM_ICO_SAFARI_RETIRE:
                    Usm_InjectFirst(USM_ICO_SAFARI_RETIRE);
                    break;
                case USM_ICO_POKENAV:
                    Usm_InjectAfter(USM_ICO_POKENAV, USM_ICO_PARTY);
                    break;
                //case USM_ICO_DEXNAV:
                //    Usm_InjectAfter(USM_ICO_DEXNAV, USM_ICO_POKENAV);
                //    break;
                default:
                    Usm_AddMenuItem(item); // Append others to the end
                    break;
            }
        }
    }
}

static bool32 Usm_IsItemAvailable(enum Usm_Icons item)
{
    switch (item) {
        case USM_ICO_POKEDEX: return FlagGet(FLAG_SYS_POKEDEX_GET);
        case USM_ICO_PARTY: return FlagGet(FLAG_SYS_POKEMON_GET);
        case USM_ICO_POKENAV: return FlagGet(FLAG_SYS_POKENAV_GET);
        case USM_ICO_DEXNAV: return FALSE;
        case USM_ICO_BAGM: return (gSaveBlock2Ptr->playerGender == MALE);
        case USM_ICO_BAGF: return (gSaveBlock2Ptr->playerGender == FEMALE);
        case USM_ICO_FRONTIER_RETIRE: return IsPlayerInBattlePyramid();
        case USM_ICO_SAFARI_RETIRE: return FALSE;
        case USM_ICO_DEBUG: return (DEBUG_OVERWORLD_MENU && DEBUG_OVERWORLD_IN_MENU);
        default: return TRUE;
    }

}

static void Usm_BuildDefaultMenuItems(void)
{
    sUsmState->itemCount = 0;

    if (FlagGet(FLAG_SYS_POKEDEX_GET))
        Usm_AddMenuItem(USM_ICO_POKEDEX);

    if (FlagGet(FLAG_SYS_POKEMON_GET))
        Usm_AddMenuItem(USM_ICO_PARTY);

    if (gSaveBlock2Ptr->playerGender == MALE)
        Usm_AddMenuItem(USM_ICO_BAGM);
    else
        Usm_AddMenuItem(USM_ICO_BAGF);

    if (FlagGet(FLAG_SYS_POKENAV_GET))
        Usm_AddMenuItem(USM_ICO_POKENAV);

    Usm_AddMenuItem(USM_ICO_TRAINER);
    Usm_AddMenuItem(USM_ICO_SAVE);
    Usm_AddMenuItem(USM_ICO_OPTIONS);

    if (DEBUG_OVERWORLD_MENU && DEBUG_OVERWORLD_IN_MENU)
        Usm_AddMenuItem(USM_ICO_DEBUG);
}

static void Usm_BuildVisibleList(void)
{
    u8 start = sUsmState->scrollOffset;
    u8 end = start + USM_MAX_ICON_COUNT;

    if (end > sUsmState->itemCount)
        end = sUsmState->itemCount;

    sUsmState->visible.count = 0;

    for (u8 i = start; i < end; i++) {
        u8 itemId = sUsmState->items[i];
        sUsmState->visible.iconIndex[sUsmState->visible.count++] = itemId;
    }
}

void Usm_LoadIconPalette(void)
{
    LoadSpritePalette(&sSpritePalette_Icons);
    PreservePaletteInWeather(IndexOfSpritePaletteTag(USM_PALTAG_ICON) + 16);
}

static void Usm_CreateIcons(s16 x, s16 y)
{
    u8 count = sUsmState->visible.count;

    s16 startX = 24 + (USM_BANNER_WIDTH - (count * USM_ICON_WIDTH)) / 2;

    for (u32 i = 0; i < count; i++) {
        s16 posX = startX + i * USM_ICON_WIDTH;

        u8 iconId = sUsmState->visible.iconIndex[i];

        u8 id = CreateSprite(sUsmMenuItems[iconId].template, posX, y, 1);

        if (!!GetFlashLevel())
            gSprites[id].objWinMask = TRUE;

        sUsmMemory->spriteIds[i] = id;
    }
}

static void Usm_LoadIconGfx(void)
{
    for (u32 i = 0; i < USM_ICO_COUNT; i++) {
        LoadCompressedSpriteSheet(sUsmMenuItems[i].sheet);
    }
}

static struct Sprite* Usm_GetSelectedSprite(void)
{
    u8 selectedId = sUsmMemory->spriteIds[sUsmState->selectedIcon];
    struct Sprite* sprite = &gSprites[selectedId];
    return sprite;
}

static void Usm_AnimateSelectedIcon(void)
{
    for (u32 i = 0; i < sUsmState->visible.count; i++) {
        struct Sprite* sprite = &gSprites[sUsmMemory->spriteIds[i]];
        if (i != sUsmState->selectedIcon) {
            sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
            StartSpriteAnim(sprite, 0);
        }
        else {
            StartSpriteAnim(sprite, 1);
            sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
            u8 matrixNum = AllocOamMatrix();
            sprite->oam.matrixNum = matrixNum;
            StartSpriteAffineAnim(sprite, 0);
        }
    }
}

static void Usm_StartIconAnim(u8 iconId)
{
    struct Sprite* sprite = Usm_GetIconSprite(iconId);
    StartSpriteAnim(sprite, 1);
    sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
    u8 matrixNum = AllocOamMatrix();
    sprite->oam.matrixNum = matrixNum;
    StartSpriteAffineAnim(sprite, 0);
}

static struct Sprite* Usm_GetIconSprite(u8 iconId)
{
    struct Sprite* sprite = &gSprites[sUsmMemory->spriteIds[iconId]];
    return sprite;
}

static void Usm_ExitStartMenu(void)
{
    Usm_SaveItems();
    u8* buf = GetBgTilemapBuffer(0);

    sUsmSavedIcon = sUsmState->selectedIcon;
    sUsmSavedScrollOffset = sUsmState->scrollOffset;

    Usm_DestroyVisibleIcons();

    for (u32 i = 0; i < USM_ICO_COUNT; i++) {
        FreeSpriteTilesByTag(sUsmMenuItems[i].template->tileTag);
    }

    for (u32 i = 0; i < sUsmState->windowCount; i++) {
        u8 winId = sUsmMemory->windowIds[i];
        FillWindowPixelBuffer(winId, PIXEL_FILL(0));
        ClearWindowTilemap(winId);
        CopyWindowToVram(winId, COPYWIN_FULL);
        RemoveWindow(winId);
    }
    FreeSpritePaletteByTag(USM_PALTAG_ICON);
    ResetPreservedPalettesInWeather();

    CpuFastFill(0, buf, BG_SCREEN_SIZE);
    CpuFastFill(0, (void*)BG_CHAR_ADDR(2), BG_CHAR_SIZE);
    ScheduleBgCopyTilemapToVram(0);

    TRY_FREE_AND_SET_NULL(sUsmMemory);
}

static void Usm_SaveItems(void)
{
    struct Usm_SavedItems* saved = &gSaveBlock3Ptr->usmSaved;

    u8 count = sUsmState->itemCount;

    if (count > USM_ICO_COUNT)
        count = USM_ICO_COUNT;

    saved->count = count;

    for (u8 i = 0; i < count; i++)
        saved->items[i] = sUsmState->items[i];
}

static u32 Usm_ReadKeys(void)
{
    if (JOY_NEW(A_BUTTON)) {
        return A_BUTTON;
    }
    if (JOY_NEW(B_BUTTON)) {
        return B_BUTTON;
    }
    if (JOY_NEW(L_BUTTON)) {
        return L_BUTTON;
    }
    if (JOY_NEW(R_BUTTON)) {
        return R_BUTTON;
    }
    if (JOY_NEW(DPAD_UP)) {
        return DPAD_UP;
    }
    if (JOY_NEW(DPAD_DOWN)) {
        return DPAD_DOWN;
    }
    if (JOY_NEW(DPAD_RIGHT)) {
        return DPAD_RIGHT;
    }
    if (JOY_NEW(DPAD_LEFT)) {
        return DPAD_LEFT;
    }
    if (JOY_NEW(SELECT_BUTTON)) {
        return SELECT_BUTTON;
    }
    else {
        return 0;
    }
}

static void Task_UsmHandleMainInput(u8 taskId)
{
    u16 input = Usm_ReadKeys();
    TaskFunc func;
    switch (input) {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            u8 iconId = sUsmState->visible.iconIndex[sUsmState->selectedIcon];
            gMenuCallback = sUsmMenuItems[iconId].callback;
            sUsmSavedIcon = sUsmState->selectedIcon;
            if (sUsmMenuItems[iconId].shouldFade)
                func = Task_UsmFadeAndRunCallback;
            else
                func = Task_UsmRunCallbackNoFade;
            gTasks[taskId].func = func;
            break;
        case B_BUTTON:
            PlaySE(SE_PC_OFF);
            Usm_ExitStartMenu();
            UnfreezeObjectEvents();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
            break;
        case SELECT_BUTTON:
            PlaySE(SE_SELECT);
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].func = Task_UsmHandleMoveItems;
            break;
        case DPAD_UP:
        case DPAD_DOWN:
        case DPAD_RIGHT:
        case DPAD_LEFT:
            Usm_HandleDPadInput(input);
            break;
    }
}

static void Usm_RefreshMenu(void)
{
    Usm_DestroyVisibleIcons();
    Usm_BuildVisibleList();

    s16 startX = 24 + (USM_BANNER_WIDTH - (sUsmState->visible.count * USM_ICON_WIDTH)) / 2;
    Usm_CreateIcons(startX, USM_ICON_YPOS);

    StartSpriteAnim(Usm_GetSelectedSprite(), 1);
    Usm_StartIconAnim(sUsmState->selectedIcon);

    Usm_PrintIconLabel();
}

static void Usm_HandleDPadInput(u8 input)
{
    if (input == DPAD_RIGHT)
    {
        PlaySE(SE_SELECT);
        if (sUsmState->selectedIcon + sUsmState->scrollOffset == sUsmState->itemCount - 1) //If this is at the very last icon on the list, it will wrap around to item 0.
        {
            sUsmState->scrollOffset = 0;
            sUsmState->selectedIcon = 0;
        }
        else if (sUsmState->selectedIcon == USM_MAX_ICON_COUNT - 1)  sUsmState->scrollOffset++; //If it hits the max items on screen number, but not the last item on the list, everything gets shifted to the left by 1.
        else sUsmState->selectedIcon++; //Business as usual
        Usm_RefreshMenu();
    }
    else if (input == DPAD_LEFT)
    {
        PlaySE(SE_SELECT);
        if (sUsmState->selectedIcon + sUsmState->scrollOffset == 0) //If you move LEFT from the first icon, it wraps around to the last item on the list. It needs to check 
        {
            sUsmState->scrollOffset = (sUsmState->itemCount > USM_MAX_ICON_COUNT) ? sUsmState->itemCount - USM_MAX_ICON_COUNT : 0;
            sUsmState->selectedIcon = (sUsmState->itemCount > USM_MAX_ICON_COUNT) ? USM_MAX_ICON_COUNT - 1 : sUsmState->itemCount - 1;
        }
        else if (sUsmState->selectedIcon == 0 && sUsmState->scrollOffset > 0) sUsmState->scrollOffset--; //If it hits icon slot 0 and that isn't the first icon on the list, moves the whole list right by 1.
        else sUsmState->selectedIcon--;
        Usm_RefreshMenu();
    }
}

static void Task_UsmFadeAndRunCallback(u8 taskId)
{
    s16* tState = &gTasks[taskId].data[0];

    switch (*tState) {
        case 0:
            FadeScreen(FADE_TO_BLACK, 0);
            (*tState)++;
            break;
        case 1:
            if (!gPaletteFade.active) {
                Usm_ExitStartMenu();
                gMenuCallback();
                DestroyTask(taskId);
            }
            break;
    }
}

static void Task_UsmRunCallbackNoFade(u8 taskId)
{
    if (!gPaletteFade.active) {
        Usm_ExitStartMenu();
        gMenuCallback();
        DestroyTask(taskId);
    }
}

static void Task_UsmHandleMoveItems(u8 taskId)
{
    struct Task* task = &gTasks[taskId];

    s16* state      = &task->data[0];
    s16* grabIndex  = &task->data[1];
    s16* handSprite = &task->data[2];

    switch (*state)
    {
        case 0:
        {
            *grabIndex = sUsmState->scrollOffset + sUsmState->selectedIcon;

            struct Sprite* sprite = Usm_GetSelectedSprite();
            Usm_StartIconAnim(sUsmState->selectedIcon);
            sprite->oam.affineMode = ST_OAM_AFFINE_OFF;

            *handSprite = Usm_CreateHandSprite(sprite->x, sprite->y - 8);

            (*state)++;
            break;
        }

        case 1:
        {
            u16 input = Usm_ReadKeys();

            if (input == SELECT_BUTTON || input == A_BUTTON || input == B_BUTTON)
            {
                PlaySE(SE_SUCCESS);
                DestroySprite(&gSprites[*handSprite]);
                FreeSpriteTilesByTag(USM_TILETAG_HAND);
                Usm_RefreshMenu();
                task->func = Task_UsmHandleMainInput;
                return;
            }

            s8 dir = 0;
            if (input == DPAD_LEFT || input == DPAD_UP)
            {
                PlaySE(SE_CLICK);
                dir = -1;
            }
            else if (input == DPAD_RIGHT || input == DPAD_DOWN)
            {
                PlaySE(SE_CLICK);
                dir = 1;
            }
            if (dir != 0) Usm_MoveItem(dir);

            struct Sprite* hand = &gSprites[*handSprite];
            struct Sprite* target = Usm_GetSelectedSprite();

            hand->x = target->x;
            hand->y = target->y - 8;

            break;
        }
    }
}

static void Usm_MoveItem(s8 dir)
{
    struct Task* task = &gTasks[FindTaskIdByFunc(Task_UsmHandleMoveItems)];
    s16* grabIndex = &task->data[1];

    s16 newIndex = *grabIndex + dir;

    if (newIndex < 0 || newIndex >= sUsmState->itemCount)
        return;

    u8 tmp = sUsmState->items[*grabIndex];
    sUsmState->items[*grabIndex] = sUsmState->items[newIndex];
    sUsmState->items[newIndex] = tmp;

    *grabIndex = newIndex;

    if (newIndex < sUsmState->scrollOffset) sUsmState->scrollOffset = newIndex;
    else if (newIndex >= sUsmState->scrollOffset + USM_MAX_ICON_COUNT) sUsmState->scrollOffset = newIndex - USM_MAX_ICON_COUNT + 1;

    sUsmState->selectedIcon = newIndex - sUsmState->scrollOffset;

    Usm_RefreshMenu();
}

static void UNUSED Usm_RedrawIcons(bool32 startAffine)
{
    Usm_DestroyVisibleIcons();

    Usm_CreateIcons(0, USM_ICON_YPOS);
    if (startAffine)
        Usm_AnimateSelectedIcon();
    else
        StartSpriteAnim(Usm_GetSelectedSprite(), 1);
}

static void Usm_DestroyVisibleIcons(void)
{
    for (u32 i = 0; i < sUsmState->visible.count; i++) {
        struct Sprite* sprite = &gSprites[sUsmMemory->spriteIds[i]];
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
    }
}

static u32 Usm_CreateHandSprite(s16 x, s16 y)
{
    u8 spriteId = Even_CreateSpriteParametrized(
        sUsmHandGfx, USM_TILETAG_HAND, sIconPal, USM_PALTAG_ICON,
        SPRITE_SIZE(32x32), SPRITE_SHAPE(32x32), x, y, 0, SpriteCallbackDummy,
        TRUE);
    struct Sprite* sprite = &gSprites[spriteId];
    sprite->objWinMask = TRUE;
    sprite->oam.priority = 0;
    return spriteId;
}

static void Task_WaitForFadeShowUsm(u8 taskId)
{
    if (IsWeatherNotFadingIn()) {
        DestroyTask(taskId);
        Usm_InitStartMenu();
        FadeInFromBlack();
    }
}

void ReturnToFieldOpenUsm(void)
{
    CreateTask(Task_WaitForFadeShowUsm, 0x50);
    LockPlayerFieldControls();
}

bool8 FieldCB_ReturnToFieldUsm(void)
{
    ReturnToFieldOpenUsm();
    return TRUE;
}

static bool32 IsPlayerInBattlePyramid(void)
{
    return CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE;
}

