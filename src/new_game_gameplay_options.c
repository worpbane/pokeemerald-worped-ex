#include "global.h"
#include "option_plus_menu.h"
#include "main.h"
#include "menu.h"
#include "scanline_effect.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "malloc.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "sound.h"
#include "text.h"
#include "text_window.h"
#include "international_string_util.h"
#include "strings.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "menu_helpers.h"
#include "decompress.h"

enum
{
    MENU_MODE,
    MENU_GAMEPLAY,
    MENU_COUNT,
};

//Menu 1 Gamemode Settings (Cannot change later)
enum
{
    MENUITEM_MODE_SHINYCHANCE,
    MENUITEM_MODE_LEVELCAP,
    MENUITEM_MODE_EXPMULT,
    MENUITEM_MODE_CATCHRATE,
    MENUITEM_MODE_TRAINERSUSEITEMS,
    MENUITEM_MODE_PLAYERUSEITEMS,
    MENUITEM_MODE_UNLIMITEDWT,
    MENUITEM_MODE_NEXT,
    MENUITEM_MODE_COUNT,
};

//Menu 2 Gameplay Settings (Can change later)
enum
{
    MENUITEM_GAMEPLAY_DIFFICULTY,
    MENUITEM_GAMEPLAY_BATTLESTYLE,
    MENUITEM_GAMEPLAY_MATCHCALL,
    MENUITEM_GAMEPLAY_CATCHMODE,
    MENUITEM_GAMEPLAY_BALLPROMPT,
    MENUITEM_GAMEPLAY_BOXMODE,
    MENUITEM_GAMEPLAY_SAVE,
    MENUITEM_GAMEPLAY_COUNT,
};

// Window Ids
enum
{
    WIN_TOPBAR,
    WIN_OPTIONS,
    WIN_DESCRIPTION
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    {//WIN_TOPBAR
        .bg = 1,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    {//WIN_OPTIONS
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 10,
        .paletteNum = 1,
        .baseBlock = 62
    },
    {//WIN_DESCRIPTION
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 1,
        .baseBlock = 500
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
    {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
    },
    {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
    },
    {
       .bg = 2,
       .charBaseIndex = 0,
       .mapBaseIndex = 29,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
    },
    {
       .bg = 3,
       .charBaseIndex = 0,
       .mapBaseIndex = 27,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
    },
};

struct OptionMenu
{
    u8 submenu;
    u8 sel_mode[MENUITEM_MODE_COUNT];
    u8 sel_gameplay[MENUITEM_GAMEPLAY_COUNT];
    int menuCursor[MENU_COUNT];
    int visibleCursor[MENU_COUNT];
    u8 arrowTaskId;
    u8 gfxLoadState;
};

#define Y_DIFF 16 // Difference in pixels between items.
#define OPTIONS_ON_SCREEN 5
#define NUM_OPTIONS_FROM_BORDER 1

// local functions
static void MainCB2(void);
static void VBlankCB(void);
static void DrawTopBarText(void); //top Option text
static void DrawLeftSideOptionText(int selection, int y);
static void DrawRightSideChoiceText(const u8 *str, int x, int y, bool8 choosen, bool8 active);
static void DrawOptionMenuTexts(void); //left side text;
static void DrawChoices(u32 id, int y); //right side draw function
static void HighlightOptionMenuItem(void);
static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_NGOptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void ScrollMenu(int direction);
static void ScrollAll(int direction); // to bottom or top
static int GetMiddleX(const u8 *txt1, const u8 *txt2, const u8 *txt3);
static int XOptions_ProcessInput(int x, int selection);
static int ProcessInput_Options_Two(int selection);
static int ProcessInput_Options_Three(int selection);
static int ProcessInput_Options_Five(int selection);
static const u8 *const OptionTextDescription(void);
static const u8 *const OptionTextRight(u8 menuItem);
static u8 MenuItemCount(void);
static void DrawDescriptionText(void);
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active);
static void DrawChoices_Options_Five(const u8 *const *const strings, int selection, int y, bool8 active);
static void ReDrawAll(void);

static void DrawChoices_ShinyChance(int selection, int y);
static void DrawChoices_LevelCap(int selection, int y);
static void DrawChoices_ExpMult(int selection, int y);
static void DrawChoices_CatchRate(int selection, int y);
static void DrawChoices_TrainerUseItems(int selection, int y);
static void DrawChoices_PlayerUseItems(int selection, int y);
static void DrawChoices_UnlimitedWT(int selection, int y);
static void DrawChoices_Difficulty(int selection, int y);
static void DrawChoices_BattleStyle(int selection, int y);
static void DrawChoices_MatchCall(int selection, int y);
static void DrawChoices_CatchMode(int selection, int y);
static void DrawChoices_BallPrompt(int selection, int y);
static void DrawChoices_BoxMode(int selection, int y);

static void DrawBgWindowFrames(void);

// EWRAM vars
EWRAM_DATA static struct OptionMenu *sOptions = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg3TilemapBuffer = NULL;

// const data
static const u8 sEqualSignGfx[]         = INCGFX_U8("graphics/interface/option_menu_equals_sign.png", ".4bpp"); // note: this is only used in the Japanese release
static const u16 sOptionMenuBg_Pal[]    = {RGB(17, 18, 31)};

static const u32 sOptionsPlusTiles[]    = INCGFX_U32("graphics/ui_options_plus/options_plus_tiles.png", ".4bpp.smol");
static const u16 sOptionsPlusPalette[]  = INCGFX_U16("graphics/ui_options_plus/options_plus_tiles.png", ".gbapal");
static const u32 sOptionsPlusTilemap[]  = INCGFX_U32("graphics/ui_options_plus/options_plus_tiles.bin", ".smolTM");
static const u32 sOptionsPlusBGTilemap[]= INCGFX_U32("graphics/ui_options_plus/options_plus_bg.bin", ".smolTM");

static const u8 wTextColors[][3] = //(BG, Primary, Shadow)
{
    {0, 2, 4},      //0 - Normal Black Text
    {0, 1, 2},      //1 - Normal White Text
    {0, 6, 5},      //2 - Option Selected
    {0, 13, 14},    //3 - Option Disabled Left
    {0, 14, 13},    //4 - Option Disabled Right
};

// Menu draw and input functions
struct // MENU_MODE
{
    void (*drawChoices)(int selection, int y);
    int (*processInput)(int selection);
} static const sItemFunctionsMain[MENUITEM_MODE_COUNT] =
{
    [MENUITEM_MODE_SHINYCHANCE]         = {DrawChoices_ShinyChance,         ProcessInput_Options_Five}, 
    [MENUITEM_MODE_LEVELCAP]            = {DrawChoices_LevelCap,            ProcessInput_Options_Two},
    [MENUITEM_MODE_EXPMULT]             = {DrawChoices_ExpMult,             ProcessInput_Options_Three},
    [MENUITEM_MODE_CATCHRATE]           = {DrawChoices_CatchRate,           ProcessInput_Options_Three},
    [MENUITEM_MODE_TRAINERSUSEITEMS]    = {DrawChoices_TrainerUseItems,     ProcessInput_Options_Two},
    [MENUITEM_MODE_PLAYERUSEITEMS]      = {DrawChoices_PlayerUseItems,      ProcessInput_Options_Two},
    [MENUITEM_MODE_UNLIMITEDWT]         = {DrawChoices_UnlimitedWT,         ProcessInput_Options_Two},
    [MENUITEM_MODE_NEXT]                = {NULL,                            NULL},
};

struct // MENU_MAIN
{
    void (*drawChoices)(int selection, int y);
    int (*processInput)(int selection);
} static const sItemFunctionsCustom[MENUITEM_GAMEPLAY_COUNT] =
{
    [MENUITEM_GAMEPLAY_DIFFICULTY]      = {DrawChoices_Difficulty,      ProcessInput_Options_Three},
    [MENUITEM_GAMEPLAY_BATTLESTYLE]     = {DrawChoices_BattleStyle,     ProcessInput_Options_Two},
    [MENUITEM_GAMEPLAY_MATCHCALL]       = {DrawChoices_MatchCall,       ProcessInput_Options_Two},
    [MENUITEM_GAMEPLAY_CATCHMODE]       = {DrawChoices_CatchMode,       ProcessInput_Options_Two},
    [MENUITEM_GAMEPLAY_BALLPROMPT]      = {DrawChoices_BallPrompt,      ProcessInput_Options_Two},
    [MENUITEM_GAMEPLAY_BOXMODE]         = {DrawChoices_BoxMode,         ProcessInput_Options_Two},
    [MENUITEM_GAMEPLAY_SAVE]            = {NULL,                        NULL},
};

//Strings
//System Options
static const u8 sText_ShinyChance[]         = _("Shiny Chance");
static const u8 sText_LevelCap[]            = _("Level Cap");
static const u8 sText_ExpMult[]             = _("Exp Mult");
static const u8 sText_CatchRate[]           = _("Catch Rate");
static const u8 sText_TrainerItems[]        = _("Trainer Items");
static const u8 sText_PlayerItems[]         = _("Player Items");
static const u8 sText_UnlimitedWT[]         = _("WT Limit");
//Gameplay  Options
static const u8 sText_Difficulty[]          = _("Difficulty");
static const u8 sText_BattleStyle[]         = _("Battle Style");
static const u8 sText_MatchCall[]           = _("Match Calls");
static const u8 sText_CatchMode[]           = _("Catch Mode");
static const u8 sText_BallPrompt[]          = _("Ball Prompt");
static const u8 sText_BoxMode[]             = _("Box Mode");

static const u8 gText_OptionMenuNext[]      = _("Next");
static const u8 gText_OptionMenuSave[]      = _("Save");

// Menu left side option names text
static const u8 *const sOptionMenuItemsNamesMain[MENUITEM_MODE_COUNT] =
{
    [MENUITEM_MODE_SHINYCHANCE]         = sText_ShinyChance, 
    [MENUITEM_MODE_LEVELCAP]            = sText_LevelCap,
    [MENUITEM_MODE_EXPMULT]             = sText_ExpMult,
    [MENUITEM_MODE_CATCHRATE]           = sText_CatchRate,
    [MENUITEM_MODE_TRAINERSUSEITEMS]    = sText_TrainerItems,
    [MENUITEM_MODE_PLAYERUSEITEMS]      = sText_PlayerItems,
    [MENUITEM_MODE_UNLIMITEDWT]         = sText_UnlimitedWT,
    [MENUITEM_MODE_NEXT]                = gText_OptionMenuNext,
};

static const u8 *const sOptionMenuItemsNamesCustom[MENUITEM_GAMEPLAY_COUNT] =
{
    [MENUITEM_GAMEPLAY_DIFFICULTY]      = sText_Difficulty,
    [MENUITEM_GAMEPLAY_BATTLESTYLE]     = sText_BattleStyle,
    [MENUITEM_GAMEPLAY_MATCHCALL]       = sText_MatchCall,
    [MENUITEM_GAMEPLAY_CATCHMODE]       = sText_CatchMode,
    [MENUITEM_GAMEPLAY_BALLPROMPT]      = sText_BallPrompt,
    [MENUITEM_GAMEPLAY_BOXMODE]         = sText_BoxMode,
    [MENUITEM_GAMEPLAY_SAVE]            = gText_OptionMenuSave,
};

static const u8 *const OptionTextRight(u8 menuItem)
{
    switch (sOptions->submenu)
    {
        case MENU_GAMEPLAY:
            return sOptionMenuItemsNamesCustom[menuItem];
        default:
            return sOptionMenuItemsNamesMain[menuItem];
    }
}

// Menu left side text conditions
static bool8 CheckConditions(int selection)
{
    switch (sOptions->submenu)
    {
    case MENU_GAMEPLAY:
        switch(selection)
        {
            case MENUITEM_GAMEPLAY_DIFFICULTY:
                return TRUE;
            case MENUITEM_GAMEPLAY_BATTLESTYLE:
                return TRUE;
            case MENUITEM_GAMEPLAY_MATCHCALL:
                return TRUE;
            case MENUITEM_GAMEPLAY_CATCHMODE:
                return TRUE;
            case MENUITEM_GAMEPLAY_BALLPROMPT:
                return TRUE;
            case MENUITEM_GAMEPLAY_BOXMODE:
                return TRUE;
            case MENUITEM_GAMEPLAY_SAVE:
                return TRUE;
            case MENUITEM_GAMEPLAY_COUNT:
                return TRUE;
        }
    default:
        switch(selection)
        {
        case MENUITEM_MODE_SHINYCHANCE:
            return TRUE;
        case MENUITEM_MODE_LEVELCAP:
            return TRUE;
        case MENUITEM_MODE_EXPMULT:
            return TRUE;
        case MENUITEM_MODE_CATCHRATE:
            return TRUE;
        case MENUITEM_MODE_TRAINERSUSEITEMS:
            return FALSE;
        case MENUITEM_MODE_PLAYERUSEITEMS:
            return FALSE;
        case MENUITEM_MODE_UNLIMITEDWT:
            return TRUE;
        case MENUITEM_MODE_NEXT:
            return TRUE;
        case MENUITEM_MODE_COUNT:
            return TRUE;
        default:
            return TRUE;
        }
    }
}

//General Strings
static const u8 sText_Empty[]                   = _("");
static const u8 sText_Desc_Next[]               = _("Continue to Game Options.\n{COLOR 8}{SHADOW 7}(Game Rules CAN NOT be changed later)");
static const u8 sText_Desc_Save[]               = _("Save your settings and continue.\n{COLOR 12}{SHADOW 11}(Game Options CAN be changed later)");

//System Menu Descriptions
static const u8 sText_Desc_Shiny8192[]          = _("Classic rates ({COLOR 8}{SHADOW 7}1/8192{COLOR 2}{SHADOW 4}).\nThe original challenge.");
static const u8 sText_Desc_Shiny4096[]          = _("Modern rates ({COLOR 12}{SHADOW 11}1/4096{COLOR 2}{SHADOW 4}).\nThe standard since GenVI.");
static const u8 sText_Desc_Shiny2048[]          = _("Increased rates ({COLOR 12}{SHADOW 11}1/2048{COLOR 2}{SHADOW 4}).\nA casual hunt.");
static const u8 sText_Desc_Shiny1024[]          = _("Boosted rates ({COLOR 6}{SHADOW 5}1/1024{COLOR 2}{SHADOW 4}).\nEasier encounters.");
static const u8 sText_Desc_Shiny512[]           = _("High odds ({COLOR 6}{SHADOW 5}1/512{COLOR 2}{SHADOW 4}).\nSimilar to PokémonGO.");
static const u8 sText_Desc_LevelCap_None[]      = _("No level limits. Power level your\nteam without restriction.");
static const u8 sText_Desc_LevelCap_Normal[]    = _("Player team is capped to the next\n{COLOR 6}{SHADOW 5}Gym Leader's{COLOR 2}{SHADOW 4} entry level.");
static const u8 sText_Desc_ExpMult_1_0[]        = _("Standard growth.\nA more authentic experience.");
static const u8 sText_Desc_ExpMult_1_5[]        = _("Fast growth.\nReduce the need for grinding by 50%.");
static const u8 sText_Desc_ExpMult_2_0[]        = _("Accelerated growth.\nDouble xp for rapid team building.");
static const u8 sText_Desc_CatchRate_1x[]       = _("Classic catch rate.\nEvery catch remains a test of luck!");
static const u8 sText_Desc_CatchRate_2x[]       = _("Double catch rate. Higher chances to\ncapture your favorite Pokémon!");
static const u8 sText_Desc_CatchRate_3x[]       = _("Triple catch rate. Focus on your\njourney with less stress.");
static const u8 sText_Desc_TrainerUseItemsOn[]  = _("Enemy trainers {COLOR 12}{SHADOW 11}can{COLOR 2}{SHADOW 4} use items in battle.");
static const u8 sText_Desc_TrainerUseItemsOff[] = _("Enemy trainers can {COLOR 8}{SHADOW 7}NOT{COLOR 2}{SHADOW 4} use items\nin battle.");
static const u8 sText_Desc_PlayerUseItemsOn[]   = _("The player {COLOR 12}{SHADOW 11}can{COLOR 2}{SHADOW 4} use items in battle.");
static const u8 sText_Desc_PlayerUseItemsOff[]  = _("The player {COLOR 8}{SHADOW 7}can NOT{COLOR 2}{SHADOW 4} use items\nin battle.");
static const u8 sText_Desc_WonderTrade_Limit[]  = _("Enables a daily limit of 3 WonderTrades.\n({COLOR 12}{SHADOW 11}Recommended{COLOR 2}{SHADOW 4})");
static const u8 sText_Desc_WonderTrade_Unlimited[]  = _("WonderTrades have no daily limit.");

static const u8 *const sOptionMenuItemDescriptionsMain[MENUITEM_MODE_COUNT][5] =
{
    [MENUITEM_MODE_SHINYCHANCE]         = {sText_Desc_Shiny8192,            sText_Desc_Shiny4096,               sText_Desc_Shiny2048,       sText_Desc_Shiny1024,       sText_Desc_Shiny512},
    [MENUITEM_MODE_LEVELCAP]            = {sText_Desc_LevelCap_None,        sText_Desc_LevelCap_Normal,         sText_Empty,                sText_Empty,                sText_Empty},
    [MENUITEM_MODE_EXPMULT]             = {sText_Desc_ExpMult_1_0,          sText_Desc_ExpMult_1_5,             sText_Desc_ExpMult_2_0,     sText_Empty,                sText_Empty},
    [MENUITEM_MODE_CATCHRATE]           = {sText_Desc_CatchRate_1x,         sText_Desc_CatchRate_2x,            sText_Desc_CatchRate_3x,    sText_Empty,                sText_Empty},
    [MENUITEM_MODE_TRAINERSUSEITEMS]    = {sText_Desc_TrainerUseItemsOn,    sText_Desc_TrainerUseItemsOff,      sText_Empty,                sText_Empty,                sText_Empty},
    [MENUITEM_MODE_PLAYERUSEITEMS]      = {sText_Desc_PlayerUseItemsOn,     sText_Desc_PlayerUseItemsOff,       sText_Empty,                sText_Empty,                sText_Empty},
    [MENUITEM_MODE_UNLIMITEDWT]         = {sText_Desc_WonderTrade_Limit,    sText_Desc_WonderTrade_Unlimited,   sText_Empty,                sText_Empty,                sText_Empty},
    [MENUITEM_MODE_NEXT]                = {sText_Desc_Next,                 sText_Empty,                        sText_Empty,                sText_Empty,                sText_Empty},
};

//Gameplay Menu Descriptions
static const u8 sText_Desc_DiffEasy[]          = _("A balanced experience perfect\nfor {COLOR 6}casual playthroughs.");
static const u8 sText_Desc_DiffNormal[]        = _("Close to a classic\n{COLOR 12}Emerald experience.");
static const u8 sText_Desc_DiffHard[]          = _("Demanding battles for those\nwho crave a {COLOR 8}true challenge.");
static const u8 sText_Desc_BattleStyle_Shift[] = _("Switch your Pokémon freely\nafter defeating an enemy.");
static const u8 sText_Desc_BattleStyle_Set[]   = _("Strategic combat with no free\nswitches after a knockout.");
static const u8 sText_Desc_MatchCallOn[]       = _("Trainers will be able to call you,\noffering rematches and info.");
static const u8 sText_Desc_MatchCallOff[]      = _("You will not receive calls.\nSpecial events will still occur.");
static const u8 sText_Desc_CatchModeOn[]       = _("{COLOR 10}{SHADOW 11}Enable{COLOR 2}{SHADOW 4} the Catch Mode prompt.");
static const u8 sText_Desc_CatchModeOff[]      = _("{COLOR 8}{SHADOW 7}Disable{COLOR 2}{SHADOW 4} the Catch Mode prompt.");
static const u8 sText_Desc_BallPromptOn[]      = _("{COLOR 10}{SHADOW 11}Enable{COLOR 2}{SHADOW 4} the Last Used Ball prompt.");
static const u8 sText_Desc_BallPromptOff[]     = _("{COLOR 8}{SHADOW 7}Disable{COLOR 2}{SHADOW 4} the Last Used Ball prompt.");
static const u8 sText_Desc_BoxModeClassic[]    = _("Use PC terminals to manage your team.\nClassic storage style. (GenIII / {COLOR 8}{SHADOW 7}R{COLOR 2}{SHADOW 4}{COLOR 6}{SHADOW 5}S{COLOR 2}{SHADOW 4}{COLOR 12}{SHADOW 11}E{COLOR 2}{SHADOW 4})");
static const u8 sText_Desc_BoxModeModern[]     = _("Can access your boxes via Party menu.\nModern convenience. (GenIX / {COLOR 6}{SHADOW 5}Sw{COLOR 8}{SHADOW 7}Sh{COLOR 2}{SHADOW 4})");

static const u8 *const sOptionMenuItemDescriptionsCustom[MENUITEM_GAMEPLAY_COUNT][3] =
{
    // [ITEM] = {Option 1 Desc, Option 2 Desc, Option 3 Desc}
    [MENUITEM_GAMEPLAY_DIFFICULTY]      = {sText_Desc_DiffEasy,             sText_Desc_DiffNormal,      sText_Desc_DiffHard},
    [MENUITEM_GAMEPLAY_BATTLESTYLE]     = {sText_Desc_BattleStyle_Shift,    sText_Desc_BattleStyle_Set, sText_Empty},
    [MENUITEM_GAMEPLAY_MATCHCALL]       = {sText_Desc_MatchCallOff,         sText_Desc_MatchCallOn,     sText_Empty},
    [MENUITEM_GAMEPLAY_CATCHMODE]       = {sText_Desc_CatchModeOff,         sText_Desc_CatchModeOn,     sText_Empty},
    [MENUITEM_GAMEPLAY_BALLPROMPT]      = {sText_Desc_BallPromptOff,        sText_Desc_BallPromptOn,    sText_Empty},
    [MENUITEM_GAMEPLAY_BOXMODE]         = {sText_Desc_BoxModeClassic,       sText_Desc_BoxModeModern,   sText_Empty},
    [MENUITEM_GAMEPLAY_SAVE]            = {sText_Desc_Save,                 sText_Empty,                sText_Empty},
};

static const u8 sText_Desc_NotImplemented[]               = _("Not Implemented.");
// Disabled Custom
static const u8 *const sOptionMenuItemDescriptionsDisabledMain[MENUITEM_MODE_COUNT] =
{
    [MENUITEM_MODE_SHINYCHANCE]         = sText_Desc_NotImplemented,
    [MENUITEM_MODE_LEVELCAP]            = sText_Desc_NotImplemented,
    [MENUITEM_MODE_EXPMULT]             = sText_Desc_NotImplemented,
    [MENUITEM_MODE_CATCHRATE]           = sText_Desc_NotImplemented,
    [MENUITEM_MODE_TRAINERSUSEITEMS]    = sText_Desc_NotImplemented,
    [MENUITEM_MODE_PLAYERUSEITEMS]      = sText_Desc_NotImplemented,
    [MENUITEM_MODE_UNLIMITEDWT]         = sText_Desc_NotImplemented,
    [MENUITEM_MODE_NEXT]                = sText_Empty,
};
// Disabled Descriptions
static const u8 *const sOptionMenuItemDescriptionsDisabledCustom[MENUITEM_GAMEPLAY_COUNT] =
{
    [MENUITEM_GAMEPLAY_DIFFICULTY]  = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_BATTLESTYLE] = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_MATCHCALL]   = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_CATCHMODE]   = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_BALLPROMPT]  = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_BOXMODE]     = sText_Desc_NotImplemented,
    [MENUITEM_GAMEPLAY_SAVE]        = sText_Empty,
};

static const u8 *const OptionTextDescription(void)
{
    u8 menuItem = sOptions->menuCursor[sOptions->submenu];
    u8 selection;

    switch (sOptions->submenu)
    {
        case MENU_GAMEPLAY:
            if (!CheckConditions(menuItem))
                return sOptionMenuItemDescriptionsDisabledCustom[menuItem];
            selection = sOptions->sel_gameplay[menuItem];
            return sOptionMenuItemDescriptionsCustom[menuItem][selection];
        default:
            if (!CheckConditions(menuItem))
                return sOptionMenuItemDescriptionsDisabledMain[menuItem];
            selection = sOptions->sel_mode[menuItem];
            return sOptionMenuItemDescriptionsMain[menuItem][selection];
    }
}

static u8 MenuItemCount(void)
{
    switch (sOptions->submenu)
    {
        case MENU_GAMEPLAY:
            return MENUITEM_GAMEPLAY_COUNT;
        default:
            return MENUITEM_MODE_COUNT;
    }
}

// Main code
static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    //ChangeBgY(3, 96, BG_COORD_ADD);
}

static void DrawCenteredText(u8 windowId, const u8 *text, int y, const u8 *color, int windowWidth)
{
    int textWidth = GetStringWidth(FONT_SHORT, text, 0); 
    int x = (windowWidth - textWidth) / 2;
    AddTextPrinterParameterized3(windowId, FONT_SHORT, x, y, color, 0, text);
}

static const u8 sText_TopBar_Main[]         = _("Game Rules");
static const u8 sText_TopBar_Main_Right[]   = _("{R_BUTTON} Options");
static const u8 sText_TopBar_Custom[]       = _("Game Options");
static const u8 sText_TopBar_Custom_Left[]  = _("{L_BUTTON} Rules");
static void DrawTopBarText(void)
{
    FillWindowPixelBuffer(WIN_TOPBAR, PIXEL_FILL(0));
    switch (sOptions->submenu)
    {
        case MENU_MODE:
            DrawCenteredText(WIN_TOPBAR, sText_TopBar_Main, 1, wTextColors[1], 240);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 185, 1, wTextColors[1], 0, sText_TopBar_Main_Right);
            break;
        case MENU_GAMEPLAY:
            DrawCenteredText(WIN_TOPBAR, sText_TopBar_Custom, 1, wTextColors[1], 240);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 3, 1, wTextColors[1], 0, sText_TopBar_Custom_Left);
            break;
    }
    PutWindowTilemap(WIN_TOPBAR);
    CopyWindowToVram(WIN_TOPBAR, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(void) //left side text
{
    u8 i;

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(0));
    for (i = 0; i < MenuItemCount(); i++)
        DrawLeftSideOptionText(i, (i * Y_DIFF) + 1);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

static void DrawDescriptionText(void)
{
    FillWindowPixelBuffer(WIN_DESCRIPTION, PIXEL_FILL(1));
    AddTextPrinterParameterized4(WIN_DESCRIPTION, FONT_NORMAL, 2, 1, 0, 0, wTextColors[0], TEXT_SKIP_DRAW, OptionTextDescription());
    CopyWindowToVram(WIN_DESCRIPTION, COPYWIN_FULL);
}

static void DrawLeftSideOptionText(int selection, int y)
{
    if (CheckConditions(selection))
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 0, y, 0, 0, wTextColors[1], TEXT_SKIP_DRAW, OptionTextRight(selection));
    else
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 0, y, 0, 0, wTextColors[3], TEXT_SKIP_DRAW, OptionTextRight(selection));
}

static void DrawRightSideChoiceText(const u8 *text, int x, int y, bool8 choosen, bool8 active)
{
    const u8 *colorPtr;

   if (active && choosen)
    {
        colorPtr = wTextColors[2];
    }
    else if (active && !choosen)
    {
        colorPtr = wTextColors[0];
    }
    else
    {
        colorPtr = wTextColors[4];
    }
    AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, x, y, 0, 0, colorPtr, TEXT_SKIP_DRAW, text);
}

static void DrawChoices(u32 id, int y) //right side draw function
{
    switch (sOptions->submenu)
    {
        case MENU_MODE:
            if (sItemFunctionsMain[id].drawChoices != NULL)
                sItemFunctionsMain[id].drawChoices(sOptions->sel_mode[id], y);
            break;
        case MENU_GAMEPLAY:
            if (sItemFunctionsCustom[id].drawChoices != NULL)
                sItemFunctionsCustom[id].drawChoices(sOptions->sel_gameplay[id], y);
            break;
    }
}

static void HighlightOptionMenuItem(void)
{
    int cursor = sOptions->visibleCursor[sOptions->submenu];

    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(8, 232));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(cursor * Y_DIFF + 24, cursor * Y_DIFF + 40));
}

static bool8 OptionsMenu_LoadGraphics(void) // Load all the tilesets, tilemaps, spritesheets, and palettes
{
    switch (sOptions->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2, sOptionsPlusTiles, 0, 0, 0);
        sOptions->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sOptionsPlusTilemap, sBg2TilemapBuffer);
            sOptions->gfxLoadState++;
        }
        break;
    case 2:
        ResetTempTileDataBuffers();
        sOptions->gfxLoadState++;
        break;
    case 3:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sOptionsPlusBGTilemap, sBg3TilemapBuffer);
            sOptions->gfxLoadState++;
        }
        break;
    case 4:
        LoadPalette(sOptionsPlusPalette, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
        sOptions->gfxLoadState++;
        break;
    default:
        sOptions->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

void CB2_InitNGOptionPlusMenu(void)
{
    u32 i;
    switch (gMain.state)
    {
        default:
        case 0:
            SetVBlankHBlankCallbacksToNull();
            ClearScheduledBgCopiesToVram();
            ResetVramOamAndBgCntRegs();
            sOptions = AllocZeroed(sizeof(*sOptions));
            FreeAllSpritePalettes();
            ResetTasks();
            ResetSpriteData();
            gMain.state++;
            break;
        case 1:
            DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
            DmaClear32(3, OAM, OAM_SIZE);
            DmaClear16(3, PLTT, PLTT_SIZE);
            ResetBgsAndClearDma3BusyFlags(0);
            ResetBgPositions();
            
            DeactivateAllTextPrinters();
            SetGpuReg(REG_OFFSET_WIN0H, 0);
            SetGpuReg(REG_OFFSET_WIN0V, 0);
            SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ);
            SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_DARKEN | BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG2);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0);
            SetGpuReg(REG_OFFSET_BLDY, 4);
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON);
            
            ResetAllBgsCoordinates();
            ResetBgsAndClearDma3BusyFlags(0);
            InitBgsFromTemplates(0, sOptionMenuBgTemplates, NELEMS(sOptionMenuBgTemplates));
            InitWindows(sOptionMenuWinTemplates);

            sBg2TilemapBuffer = Alloc(0x800);
            memset(sBg2TilemapBuffer, 0, 0x800);
            SetBgTilemapBuffer(2, sBg2TilemapBuffer);
            ScheduleBgCopyTilemapToVram(2);

            sBg3TilemapBuffer = Alloc(0x800);
            memset(sBg3TilemapBuffer, 0, 0x800);
            SetBgTilemapBuffer(3, sBg3TilemapBuffer);
            ScheduleBgCopyTilemapToVram(3);
            gMain.state++;
            break;
        case 2:
            ResetPaletteFade();
            ScanlineEffect_Stop();
            gMain.state++;
            sOptions->gfxLoadState = 0;
            break;
        case 3:
            if (OptionsMenu_LoadGraphics() == TRUE)
            {
                gMain.state++;
                LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
            }
            break;
        case 4:
            LoadPalette(sOptionMenuBg_Pal, BG_PLTT_ID(0), sizeof(sOptionMenuBg_Pal));
            LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x70, 0x20);
            gMain.state++;
            break;
        case 5:
            //Don't need the palette that was here
            gMain.state++;
            break;
        case 6:
            gSaveBlock1Ptr->wx_ShinyChance              =       1;
            gSaveBlock1Ptr->wx_LevelCap                 =       0;
            gSaveBlock1Ptr->wx_ExpMulti                 =       0;
            gSaveBlock1Ptr->wx_CatchRate                =       0;
            gSaveBlock1Ptr->wx_TrainerItemUse           =       0;
            gSaveBlock1Ptr->wx_PlayerItemUse            =       0;
            gSaveBlock1Ptr->wx_UnlimitedWT              =       0;

            sOptions->sel_mode[MENUITEM_MODE_SHINYCHANCE]               = gSaveBlock1Ptr->wx_ShinyChance;
            sOptions->sel_mode[MENUITEM_MODE_LEVELCAP]                  = gSaveBlock1Ptr->wx_LevelCap;
            sOptions->sel_mode[MENUITEM_MODE_EXPMULT]                   = gSaveBlock1Ptr->wx_ExpMulti;
            sOptions->sel_mode[MENUITEM_MODE_CATCHRATE]                 = gSaveBlock1Ptr->wx_CatchRate;
            sOptions->sel_mode[MENUITEM_MODE_TRAINERSUSEITEMS]          = gSaveBlock1Ptr->wx_TrainerItemUse;
            sOptions->sel_mode[MENUITEM_MODE_PLAYERUSEITEMS]            = gSaveBlock1Ptr->wx_PlayerItemUse;
            sOptions->sel_mode[MENUITEM_MODE_UNLIMITEDWT]               = gSaveBlock1Ptr->wx_UnlimitedWT;


            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_DIFFICULTY]         = gSaveBlock2Ptr->w_opDifficulty;
            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BATTLESTYLE]        = gSaveBlock2Ptr->optionsBattleStyle;
            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_MATCHCALL]          = gSaveBlock2Ptr->w_opMatchCall;
            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_CATCHMODE]          = gSaveBlock2Ptr->w_opCatchMode;
            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BALLPROMPT]         = gSaveBlock2Ptr->w_opBallPrompt;
            sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BOXMODE]            = gSaveBlock2Ptr->w_opBoxMode;

            sOptions->submenu = MENU_MODE;

            gMain.state++;
            break;
        case 7:
            PutWindowTilemap(WIN_TOPBAR);
            DrawTopBarText();
            gMain.state++;
            break;
        case 8:
            PutWindowTilemap(WIN_DESCRIPTION);
            DrawDescriptionText();
            gMain.state++;
            break;
        case 9:
            PutWindowTilemap(WIN_OPTIONS);
            DrawOptionMenuTexts();
            gMain.state++;
            break;
        case 10:
            CreateTask(Task_OptionMenuFadeIn, 0);
            
            sOptions->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 240 / 2, 20, 110, MENUITEM_MODE_COUNT - 1, 110, 110, 0);
            

            for (i = 0; i < min(OPTIONS_ON_SCREEN, MenuItemCount()); i++)
                DrawChoices(i, i * Y_DIFF);

            HighlightOptionMenuItem();

            CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
            gMain.state++;
            break;
        case 11:
            DrawBgWindowFrames();
            gMain.state++;
            break;
        case 12:
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
            SetVBlankCallback(VBlankCB);
            SetMainCallback2(MainCB2);
            return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_OptionMenuProcessInput;
        SetGpuReg(REG_OFFSET_WIN0H, 0); // Idk man Im just trying to stop this stupid graphical bug from happening dont judge me
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_DARKEN | BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG2);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        HighlightOptionMenuItem();
        return;
    }
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN , MenuItemCount());
    if (JOY_NEW(A_BUTTON))
    {
        if (sOptions->submenu == MENU_MODE && sOptions->menuCursor[sOptions->submenu] == MENUITEM_MODE_NEXT)
        {
            PlaySE(SE_SELECT);
            sOptions->submenu = MENU_GAMEPLAY;
            sOptions->menuCursor[sOptions->submenu] = 0;
            sOptions->visibleCursor[sOptions->submenu] = 0;
            
            DrawTopBarText();
            ReDrawAll();
            HighlightOptionMenuItem();
            DrawDescriptionText();
        }
        else if (sOptions->submenu == MENU_GAMEPLAY && sOptions->menuCursor[sOptions->submenu] == MENUITEM_GAMEPLAY_SAVE)
        {
            PlaySE(SE_PC_ON);
            gTasks[taskId].func = Task_NGOptionMenuSave;
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_CLICK);
        if (sOptions->visibleCursor[sOptions->submenu] == NUM_OPTIONS_FROM_BORDER) // don't advance visible cursor until scrolled to the bottom
        {
            if (--sOptions->menuCursor[sOptions->submenu] == 0)
                sOptions->visibleCursor[sOptions->submenu]--;
            else
                ScrollMenu(1);
        }
        else
        {
            if (--sOptions->menuCursor[sOptions->submenu] < 0) // Scroll all the way to the bottom.
            {
                sOptions->visibleCursor[sOptions->submenu] = sOptions->menuCursor[sOptions->submenu] = optionsToDraw-2;
                ScrollAll(0);
                sOptions->visibleCursor[sOptions->submenu] = optionsToDraw-1;
                sOptions->menuCursor[sOptions->submenu] = MenuItemCount() - 1;
            }
            else
            {
                sOptions->visibleCursor[sOptions->submenu]--;
            }
        }
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_CLICK);
        if (sOptions->visibleCursor[sOptions->submenu] == optionsToDraw-2) // don't advance visible cursor until scrolled to the bottom
        {
            if (++sOptions->menuCursor[sOptions->submenu] == MenuItemCount() - 1)
                sOptions->visibleCursor[sOptions->submenu]++;
            else
                ScrollMenu(0);
        }
        else
        {
            if (++sOptions->menuCursor[sOptions->submenu] >= MenuItemCount()-1) // Scroll all the way to the top.
            {
                sOptions->visibleCursor[sOptions->submenu] = optionsToDraw-2;
                sOptions->menuCursor[sOptions->submenu] = MenuItemCount() - optionsToDraw-1;
                ScrollAll(1);
                sOptions->visibleCursor[sOptions->submenu] = sOptions->menuCursor[sOptions->submenu] = 0;
            }
            else
            {
                sOptions->visibleCursor[sOptions->submenu]++;
            }
        }
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        if (sOptions->submenu == MENU_MODE)
        {
            int cursor = sOptions->menuCursor[sOptions->submenu];
            u8 previousOption = sOptions->sel_mode[cursor];
            if (CheckConditions(cursor))
            {
                if (sItemFunctionsMain[cursor].processInput != NULL)
                {
                    PlaySE(SE_CLICK);
                    sOptions->sel_mode[cursor] = sItemFunctionsMain[cursor].processInput(previousOption);
                    ReDrawAll();
                    DrawDescriptionText();
                }

                if (previousOption != sOptions->sel_mode[cursor])
                    DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
            }
        }
        else if (sOptions->submenu == MENU_GAMEPLAY)
        {
            int cursor = sOptions->menuCursor[sOptions->submenu];
            u8 previousOption = sOptions->sel_gameplay[cursor];
            if (CheckConditions(cursor))
            {
                if (sItemFunctionsCustom[cursor].processInput != NULL)
                {
                    PlaySE(SE_CLICK);
                    sOptions->sel_gameplay[cursor] = sItemFunctionsCustom[cursor].processInput(previousOption);
                    ReDrawAll();
                    DrawDescriptionText();
                }

                if (previousOption != sOptions->sel_gameplay[cursor])
                    DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
            }
        }
    }
    else if (JOY_NEW(R_BUTTON))
    {
        if (sOptions->submenu != MENU_GAMEPLAY)
        {
            PlaySE(SE_SELECT);
            sOptions->submenu++;
        }

        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sOptions->submenu != 0)
        {
            PlaySE(SE_SELECT);
            sOptions->submenu--;
        }
        
        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
}

static void Task_NGOptionMenuSave(u8 taskId)
{
    gSaveBlock2Ptr->w_opDifficulty      = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_DIFFICULTY];
    gSaveBlock2Ptr->optionsBattleStyle  = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BATTLESTYLE];
    gSaveBlock2Ptr->w_opMatchCall       = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_MATCHCALL];
    gSaveBlock2Ptr->w_opCatchMode       = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_CATCHMODE];
    gSaveBlock2Ptr->w_opBallPrompt      = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BALLPROMPT];
    gSaveBlock2Ptr->w_opBoxMode         = sOptions->sel_gameplay[MENUITEM_GAMEPLAY_BOXMODE];

    gSaveBlock1Ptr->wx_ShinyChance      = sOptions->sel_mode[MENUITEM_MODE_SHINYCHANCE];
    gSaveBlock1Ptr->wx_LevelCap         = sOptions->sel_mode[MENUITEM_MODE_LEVELCAP];
    gSaveBlock1Ptr->wx_ExpMulti         = sOptions->sel_mode[MENUITEM_MODE_EXPMULT];
    gSaveBlock1Ptr->wx_CatchRate        = sOptions->sel_mode[MENUITEM_MODE_CATCHRATE];
    gSaveBlock1Ptr->wx_TrainerItemUse   = sOptions->sel_mode[MENUITEM_MODE_TRAINERSUSEITEMS];
    gSaveBlock1Ptr->wx_PlayerItemUse    = sOptions->sel_mode[MENUITEM_MODE_PLAYERUSEITEMS];
    gSaveBlock1Ptr->wx_UnlimitedWT      = sOptions->sel_mode[MENUITEM_MODE_UNLIMITEDWT];

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sOptions);
        try_free(sBg2TilemapBuffer);
        try_free(sBg3TilemapBuffer);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        HideBg(2);
        HideBg(3);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void ScrollMenu(int direction)
{
    int menuItem, pos;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    if (direction == 0) // scroll down
        menuItem = sOptions->menuCursor[sOptions->submenu] + NUM_OPTIONS_FROM_BORDER, pos = optionsToDraw - 1;
    else
        menuItem = sOptions->menuCursor[sOptions->submenu] - NUM_OPTIONS_FROM_BORDER, pos = 0;

    // Hide one
    ScrollWindow(WIN_OPTIONS, direction, Y_DIFF, PIXEL_FILL(0));
    // Show one
    FillWindowPixelRect(WIN_OPTIONS, PIXEL_FILL(0), 0, Y_DIFF * pos, 26 * 8, Y_DIFF);
    // Print
    DrawChoices(menuItem, pos * Y_DIFF);
    DrawLeftSideOptionText(menuItem, (pos * Y_DIFF) + 1);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}
static void ScrollAll(int direction) // to bottom or top
{
    int i, y, menuItem, pos;
    int scrollCount;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    scrollCount = MenuItemCount() - optionsToDraw;

    // Move items up/down
    ScrollWindow(WIN_OPTIONS, direction, Y_DIFF * scrollCount, PIXEL_FILL(1));

    // Clear moved items
    if (direction == 0)
    {
        y = optionsToDraw - scrollCount;
        if (y < 0)
            y = optionsToDraw;
        y *= Y_DIFF;
    }
    else
    {
        y = 0;
    }

    FillWindowPixelRect(WIN_OPTIONS, PIXEL_FILL(0), 0, y, 26 * 8, Y_DIFF * scrollCount);
    // Print new texts
    for (i = 0; i < scrollCount; i++)
    {
        if (direction == 0) // From top to bottom
            menuItem = MenuItemCount() - 1 - i, pos = optionsToDraw - 1 - i;
        else // From bottom to top
            menuItem = i, pos = i;
        DrawChoices(menuItem, pos * Y_DIFF);
        DrawLeftSideOptionText(menuItem, (pos * Y_DIFF) + 1);
    }
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}

// Process Input functions ****GENERIC****
static int GetMiddleX(const u8 *txt1, const u8 *txt2, const u8 *txt3)
{
    int xMid;
    int widthLeft = GetStringWidth(1, txt1, 0);
    int widthMid = GetStringWidth(1, txt2, 0);
    int widthRight = GetStringWidth(1, txt3, 0);

    widthMid -= (198 - 104);
    xMid = (widthLeft - widthMid - widthRight) / 2 + 104;
    return xMid;
}

static int XOptions_ProcessInput(int x, int selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (++selection > (x - 1))
            selection = 0;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (--selection < 0)
            selection = (x - 1);
    }
    return selection;
}

static int ProcessInput_Options_Two(int selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;

    return selection;
}

static int ProcessInput_Options_Three(int selection)
{
    return XOptions_ProcessInput(3, selection);
}

static int ProcessInput_Options_Five(int selection)
{
    return XOptions_ProcessInput(5, selection);
}

// Draw Choices functions ****GENERIC****
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active)
{
    bool8 choosen = FALSE;
    if (style != 0)
        choosen = TRUE;

    DrawRightSideChoiceText(text, x, y+1, choosen, active);
}

static void DrawChoices_Options_Five(const u8 *const *const strings, int selection, int y, bool8 active)
{
    static const u8 choiceOrders[][3] =
    {
        {0, 1, 2},
        {0, 1, 2},
        {1, 2, 3},
        {2, 3, 4},
        {2, 3, 4},
    };
    u8 styles[5] = {0};
    int xMid;
    const u8 *order = choiceOrders[selection];
    styles[selection] = 1;
    xMid = GetMiddleX(strings[order[0]], strings[order[1]], strings[order[2]]);

    DrawOptionMenuChoice(strings[order[0]], 104, y, styles[order[0]], active);
    DrawOptionMenuChoice(strings[order[1]], xMid, y, styles[order[1]], active);
    DrawOptionMenuChoice(strings[order[2]], GetStringRightAlignXOffset(1, strings[order[2]], 198), y, styles[order[2]], active);
}

static void ReDrawAll(void)
{
    u8 menuItem = sOptions->menuCursor[sOptions->submenu] - sOptions->visibleCursor[sOptions->submenu];
    u8 i;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    if (MenuItemCount() <= OPTIONS_ON_SCREEN) // Draw or delete the scrolling arrows based on options in the menu
    {
        if (sOptions->arrowTaskId != TASK_NONE)
        {
            RemoveScrollIndicatorArrowPair(sOptions->arrowTaskId);
            sOptions->arrowTaskId = TASK_NONE;
        }
    }
    else
    {
        if (sOptions->arrowTaskId == TASK_NONE)
            sOptions->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 240 / 2, 20, 110, MenuItemCount() - 1, 110, 110, 0);

    }

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(0));
    for (i = 0; i < optionsToDraw; i++)
    {
        DrawChoices(menuItem+i, i * Y_DIFF);
        DrawLeftSideOptionText(menuItem+i, (i * Y_DIFF) + 1);
    }
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}

//Gamemode Draw Choices
static const u8 sText_ShinyChance_8192[]   = _("8192");
static const u8 sText_ShinyChance_4096[]   = _("4096");
static const u8 sText_ShinyChance_2048[]   = _("2048");
static const u8 sText_ShinyChance_1024[]   = _("1024");
static const u8 sTexts_ShinyChance_512[]    = _("512");
static const u8 *const sText_ShinyChance_Strings[] = {sText_ShinyChance_8192,  sText_ShinyChance_4096,  sText_ShinyChance_2048,  sText_ShinyChance_1024,  sTexts_ShinyChance_512};
static void DrawChoices_ShinyChance(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_SHINYCHANCE);
    DrawChoices_Options_Five(sText_ShinyChance_Strings, selection, y, active);
}

static const u8 sText_LevelCapOff[] = _("Off");
static const u8 sText_LevelCapSoft[] = _("Soft");
static const u8 sText_LevelCapHard[] = _("Hard"); //WORPTodo: Implement
static void DrawChoices_LevelCap(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_LEVELCAP);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_LevelCapOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_LevelCapSoft, GetStringRightAlignXOffset(FONT_NORMAL, sText_LevelCapSoft, 198), y, styles[1], active);
}
static const u8 sText_ExpMult10[] = _("x1.0");
static const u8 sText_ExpMult15[] = _("x1.5");
static const u8 sText_ExpMult20[] = _("x2.0");
static void DrawChoices_ExpMult(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_EXPMULT);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(sText_ExpMult10, sText_ExpMult15, sText_ExpMult20);
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_ExpMult10, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_ExpMult15, xMid, y, styles[1], active);
    DrawOptionMenuChoice(sText_ExpMult20, GetStringRightAlignXOffset(1, sText_ExpMult20, 198), y, styles[2], active);
}
static const u8 sText_CatchRateNormal[] = _("x1.0");
static const u8 sText_CatchRateEasy[] = _("x2.0");
static const u8 sText_CatchRateEasiest[] = _("x3.0");
static void DrawChoices_CatchRate(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_EXPMULT);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(sText_CatchRateNormal, sText_CatchRateEasy, sText_CatchRateEasiest);
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_CatchRateNormal, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_CatchRateEasy, xMid, y, styles[1], active);
    DrawOptionMenuChoice(sText_CatchRateEasiest, GetStringRightAlignXOffset(1, sText_CatchRateEasiest, 198), y, styles[2], active);
}
static const u8 sText_TrainerUseItems[] = _("Yes");
static const u8 sText_TrainerNoUseItemsOn[] = _("No");
static void DrawChoices_TrainerUseItems(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_TRAINERSUSEITEMS);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_TrainerUseItems, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_TrainerNoUseItemsOn, GetStringRightAlignXOffset(FONT_NORMAL, sText_TrainerNoUseItemsOn, 198), y, styles[1], active);
}
static const u8 sText_PlayerUseItems[] = _("Yes");
static const u8 sText_PlayerNoUseItemsOn[] = _("No");
static void DrawChoices_PlayerUseItems(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_PLAYERUSEITEMS);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_PlayerUseItems, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_PlayerNoUseItemsOn, GetStringRightAlignXOffset(FONT_NORMAL, sText_PlayerNoUseItemsOn, 198), y, styles[1], active);
}
static const u8 sText_WT_Limit[] = _("Yes");
static const u8 sText_WT_NoLimit[] = _("No");
static void DrawChoices_UnlimitedWT(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MODE_UNLIMITEDWT);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_WT_Limit, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_WT_NoLimit, GetStringRightAlignXOffset(FONT_NORMAL, sText_WT_NoLimit, 198), y, styles[1], active);
}
//Gameplay Draw Choices
static const u8 sText_DifficultyEasy[] = _("Easy");
static const u8 sText_DifficultyNorm[] = _("Normal");
static const u8 sText_DifficultyHard[] = _("Hard");
static void DrawChoices_Difficulty(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_DIFFICULTY);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(sText_DifficultyEasy, sText_DifficultyNorm, sText_DifficultyHard);
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_DifficultyEasy, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_DifficultyNorm, xMid, y, styles[1], active);
    DrawOptionMenuChoice(sText_DifficultyHard, GetStringRightAlignXOffset(1, sText_DifficultyHard, 198), y, styles[2], active);
}

static const u8 sText_BattleStyleShift[] = _("Shift");
static const u8 sText_BattleStyleSet[] = _("Set");
static void DrawChoices_BattleStyle(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_BATTLESTYLE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BattleStyleShift, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BattleStyleSet, GetStringRightAlignXOffset(FONT_NORMAL, sText_BattleStyleSet, 198), y, styles[1], active);
}

static const u8 sText_MatchCallOn[] = _("On");
static const u8 sText_MatchCallOff[] = _("Off");
static void DrawChoices_MatchCall(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_MATCHCALL);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_MatchCallOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_MatchCallOn, GetStringRightAlignXOffset(1, sText_MatchCallOn, 198), y, styles[1], active);
}

static const u8 sText_CatchModeOn[] = _("On");
static const u8 sText_CatchModeOff[] = _("Off");
static void DrawChoices_CatchMode(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_CATCHMODE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_CatchModeOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_CatchModeOn, GetStringRightAlignXOffset(1, sText_CatchModeOn, 198), y, styles[1], active);
}

static const u8 sText_BallPromptOn[] = _("On");
static const u8 sText_BallPromptOff[] = _("Off");
static void DrawChoices_BallPrompt(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_BALLPROMPT);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BallPromptOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BallPromptOn, GetStringRightAlignXOffset(1, sText_BallPromptOn, 198), y, styles[1], active);
}

static const u8 sText_BoxModeClassic[] = _("Classic");
static const u8 sText_BoxModeModern[] = _("Modern");
static void DrawChoices_BoxMode(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GAMEPLAY_BOXMODE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BoxModeClassic, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BoxModeModern, GetStringRightAlignXOffset(1, sText_BoxModeModern, 198), y, styles[1], active);
}

// Background tilemap
#define TILE_TOP_CORNER_L 0x1A2 // 418
#define TILE_TOP_EDGE     0x1A3 // 419
#define TILE_TOP_CORNER_R 0x1A4 // 420
#define TILE_LEFT_EDGE    0x1A5 // 421
#define TILE_RIGHT_EDGE   0x1A7 // 423
#define TILE_BOT_CORNER_L 0x1A8 // 424
#define TILE_BOT_EDGE     0x1A9 // 425
#define TILE_BOT_CORNER_R 0x1AA // 426

static void DrawBgWindowFrames(void)
{
    //                     bg, tile,              x, y, width, height, palNum
    // Option Texts window
    //FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  2,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  2, 26,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  2,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  3,  1, 16,  7);
    //FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  3,  1, 16,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 13,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 13, 26,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 13,  1,  1,  7);

    // Description window
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1, 14,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2, 14, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28, 14,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1, 15,  1,  4,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28, 15,  1,  4,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 19,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 19, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 19,  1,  1,  7);

    CopyBgTilemapBufferToVram(1);
}
