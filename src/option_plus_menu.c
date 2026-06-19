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
    MENU_MAIN,
    MENU_SYSTEM,
    MENU_COUNT,
};

//Menu 1 Gameplay
enum
{
    MENUITEM_MAIN_DIFFICULTY,
    MENUITEM_MAIN_BATTLESTYLE,
    MENUITEM_MAIN_MATCHCALL,
    MENUITEM_MAIN_CATCHMODE,
    MENUITEM_MAIN_BALLPROMPT,
    MENUITEM_MAIN_AUTORUN,
    MENUITEM_MAIN_BOXMODE,
    MENUITEM_MAIN_CANCEL,
    MENUITEM_MAIN_COUNT,
};

//Menu 2 System
enum
{
    MENUITEM_SYSTEM_TEXTSPEED,
    MENUITEM_SYSTEM_BATTLESCENE,
    MENUITEM_SYSTEM_SOUND,
    MENUITEM_SYSTEM_FONT,
    MENUITEM_SYSTEM_BUTTONMODE,
    MENUITEM_SYSTEM_FRAMETYPE,
    MENUITEM_SYSTEM_CANCEL,
    MENUITEM_SYSTEM_COUNT,
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
    u8 sel[MENUITEM_MAIN_COUNT];
    u8 sel_custom[MENUITEM_SYSTEM_COUNT];
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
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void ScrollMenu(int direction);
static void ScrollAll(int direction); // to bottom or top
static int GetMiddleX(const u8 *txt1, const u8 *txt2, const u8 *txt3);
static int XOptions_ProcessInput(int x, int selection);
static int ProcessInput_Options_Two(int selection);
static int ProcessInput_Options_Three(int selection);
static int ProcessInput_Options_Four(int selection);
static int ProcessInput_Sound(int selection);
static int ProcessInput_FrameType(int selection);
static const u8 *const OptionTextDescription(void);
static const u8 *const OptionTextRight(u8 menuItem);
static u8 MenuItemCount(void);
static u8 MenuItemCancel(void);
static void DrawDescriptionText(void);
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active);
static void DrawChoices_Options_Four(const u8 *const *const strings, int selection, int y, bool8 active);
static void ReDrawAll(void);
static void DrawChoices_Difficulty(int selection, int y);
static void DrawChoices_BattleStyle(int selection, int y);
static void DrawChoices_MatchCall(int selection, int y);
static void DrawChoices_CatchMode(int selection, int y);
static void DrawChoices_BallPrompt(int selection, int y);
static void DrawChoices_AutoRun(int selection, int y);
static void DrawChoices_BoxMode(int selection, int y);
static void DrawChoices_TextSpeed(int selection, int y);
static void DrawChoices_BattleScene(int selection, int y);
static void DrawChoices_Sound(int selection, int y);
static void DrawChoices_ButtonMode(int selection, int y);
static void DrawChoices_Font(int selection, int y);
static void DrawChoices_FrameType(int selection, int y);
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
    {0, 2, 4}, //0 - Normal Black Text
    {0, 1, 2}, //1 - Normal White Text
    {0, 5, 6}, //2 - Option Selected
    {0, 7, 8}, //3 - Option Disabled
};

// Menu draw and input functions
struct // MENU_MAIN
{
    void (*drawChoices)(int selection, int y);
    int (*processInput)(int selection);
} static const sItemFunctionsMain[MENUITEM_MAIN_COUNT] =
{
    [MENUITEM_MAIN_DIFFICULTY]      = {DrawChoices_Difficulty,      ProcessInput_Options_Three},
    [MENUITEM_MAIN_BATTLESTYLE]     = {DrawChoices_BattleStyle,     ProcessInput_Options_Two},
    [MENUITEM_MAIN_MATCHCALL]       = {DrawChoices_MatchCall,       ProcessInput_Options_Two},
    [MENUITEM_MAIN_CATCHMODE]       = {DrawChoices_CatchMode,       ProcessInput_Options_Two},
    [MENUITEM_MAIN_BALLPROMPT]      = {DrawChoices_BallPrompt,      ProcessInput_Options_Two},
    [MENUITEM_MAIN_AUTORUN]         = {DrawChoices_AutoRun,         ProcessInput_Options_Two},
    [MENUITEM_MAIN_BOXMODE]         = {DrawChoices_BoxMode,         ProcessInput_Options_Two},
    [MENUITEM_MAIN_CANCEL]          = {NULL,                        NULL},
};

struct // MENU_SYSTEM
{
    void (*drawChoices)(int selection, int y);
    int (*processInput)(int selection);
} static const sItemFunctionsCustom[MENUITEM_SYSTEM_COUNT] =
{
    [MENUITEM_SYSTEM_TEXTSPEED]       = {DrawChoices_TextSpeed,       ProcessInput_Options_Four}, 
    [MENUITEM_SYSTEM_BATTLESCENE]     = {DrawChoices_BattleScene,     ProcessInput_Options_Two},
    [MENUITEM_SYSTEM_SOUND]           = {DrawChoices_Sound,           ProcessInput_Sound},
    [MENUITEM_SYSTEM_BUTTONMODE]      = {DrawChoices_ButtonMode,      ProcessInput_Options_Three},
    [MENUITEM_SYSTEM_FONT]            = {DrawChoices_Font,            ProcessInput_Options_Two},
    [MENUITEM_SYSTEM_FRAMETYPE]       = {DrawChoices_FrameType,       ProcessInput_FrameType},
    [MENUITEM_SYSTEM_CANCEL]          = {NULL,                        NULL},
};

//Strings
//Gameplay  Options
static const u8 sText_Difficulty[]              = _("Difficulty");
static const u8 sText_BattleStyle[]             = _("Battle Style");
static const u8 sText_MatchCall[]               = _("Match Calls");
static const u8 sText_CatchMode[]               = _("Catch Mode");
static const u8 sText_BallPrompt[]              = _("Ball Prompt");
static const u8 sText_AutoRun[]                 = _("AutoRun");
static const u8 sText_BoxMode[]                 = _("Box Mode");
//System Options
static const u8 sText_TextSpeed[]               = _("Text Speed");
static const u8 sText_BattleScene[]             = _("Battle Scene");
static const u8 sText_Sound[]                   = _("Sound");
static const u8 sText_ButtonMode[]              = _("Button Mode");
static const u8 sText_Font[]                    = _("Font");
static const u8 sText_Frame[]                   = _("Frame");

static const u8 gText_OptionMenuSave[]          = _("Save");

// Menu left side option names text
static const u8 *const sOptionMenuItemsNamesMain[MENUITEM_MAIN_COUNT] =
{
    [MENUITEM_MAIN_DIFFICULTY]      = sText_Difficulty,
    [MENUITEM_MAIN_BATTLESTYLE]     = sText_BattleStyle,
    [MENUITEM_MAIN_MATCHCALL]       = sText_MatchCall,
    [MENUITEM_MAIN_CATCHMODE]       = sText_CatchMode,
    [MENUITEM_MAIN_BALLPROMPT]      = sText_BallPrompt,
    [MENUITEM_MAIN_AUTORUN]         = sText_AutoRun,
    [MENUITEM_MAIN_BOXMODE]         = sText_BoxMode,
    [MENUITEM_MAIN_CANCEL]          = gText_OptionMenuSave,
};

static const u8 *const sOptionMenuItemsNamesCustom[MENUITEM_SYSTEM_COUNT] =
{
    [MENUITEM_SYSTEM_TEXTSPEED]       = sText_TextSpeed, 
    [MENUITEM_SYSTEM_BATTLESCENE]     = sText_BattleScene,
    [MENUITEM_SYSTEM_SOUND]           = sText_Sound,
    [MENUITEM_SYSTEM_BUTTONMODE]      = sText_ButtonMode,
    [MENUITEM_SYSTEM_FONT]            = sText_Font,
    [MENUITEM_SYSTEM_FRAMETYPE]       = sText_Frame,
    [MENUITEM_SYSTEM_CANCEL]          = gText_OptionMenuSave,
};

static const u8 *const OptionTextRight(u8 menuItem)
{
    switch (sOptions->submenu)
    {
        case MENU_SYSTEM:
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
    case MENU_SYSTEM:
        switch(selection)
        {
            case MENUITEM_SYSTEM_TEXTSPEED:
                return TRUE;
            case MENUITEM_SYSTEM_BATTLESCENE:
                return TRUE;
            case MENUITEM_SYSTEM_SOUND:
                return TRUE;
            case MENUITEM_SYSTEM_BUTTONMODE:
                return TRUE;
            case MENUITEM_SYSTEM_FONT:
                return TRUE;
            case MENUITEM_SYSTEM_FRAMETYPE:
                return TRUE;
            case MENUITEM_SYSTEM_CANCEL:
                return TRUE;
            case MENUITEM_SYSTEM_COUNT:
                return TRUE;
        }
    default:
        switch(selection)
        {
        case MENUITEM_MAIN_DIFFICULTY:
            return TRUE;
        case MENUITEM_MAIN_BATTLESTYLE:
            return TRUE;
        case MENUITEM_MAIN_MATCHCALL:
            return TRUE;
        case MENUITEM_MAIN_CATCHMODE:
            return TRUE;
        case MENUITEM_MAIN_BALLPROMPT:
            return TRUE;
        case MENUITEM_MAIN_AUTORUN:
            return TRUE;
        case MENUITEM_MAIN_BOXMODE:
            return TRUE;
        case MENUITEM_MAIN_CANCEL:
            return TRUE;
        case MENUITEM_MAIN_COUNT:
            return TRUE;
        default:
            return TRUE;
        }
    }
}

//General Strings
static const u8 sText_Empty[]                   = _("");
static const u8 sText_Desc_Save[]               = _("Save your settings.");
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
static const u8 sText_Desc_AutoRun[]           = _("Toggle automatic running.");
static const u8 sText_Desc_BoxModeClassic[]    = _("Only able to access your\nPokémon boxes at PC terminals.");
static const u8 sText_Desc_BoxModeModern[]     = _("Access Pokémon boxes from\nanywhere in the Party Menu.");

static const u8 *const sOptionMenuItemDescriptionsMain[MENUITEM_MAIN_COUNT][3] =
{
    // [ITEM] = {Option 1 Desc, Option 2 Desc, Option 3 Desc}
    [MENUITEM_MAIN_DIFFICULTY]  = {sText_Desc_DiffEasy,             sText_Desc_DiffNormal,      sText_Desc_DiffHard},
    [MENUITEM_MAIN_BATTLESTYLE] = {sText_Desc_BattleStyle_Shift,    sText_Desc_BattleStyle_Set, sText_Empty},
    [MENUITEM_MAIN_MATCHCALL]   = {sText_Desc_MatchCallOff,         sText_Desc_MatchCallOn,     sText_Empty},
    [MENUITEM_MAIN_CATCHMODE]   = {sText_Desc_CatchModeOff,         sText_Desc_CatchModeOn,     sText_Empty},
    [MENUITEM_MAIN_BALLPROMPT]  = {sText_Desc_BallPromptOff,        sText_Desc_BallPromptOn,    sText_Empty},
    [MENUITEM_MAIN_AUTORUN]     = {sText_Desc_AutoRun,              sText_Empty,                sText_Empty},
    [MENUITEM_MAIN_BOXMODE]     = {sText_Desc_BoxModeClassic,       sText_Desc_BoxModeModern,   sText_Empty},
    [MENUITEM_MAIN_CANCEL]      = {sText_Desc_Save,                 sText_Empty,                sText_Empty},
};

//System Menu Descriptions
static const u8 sText_Desc_TextSpeed[]      = _("Sets how fast text is displayed.");
static const u8 sText_Desc_BattleScene_On[] = _("Show Pokémon battle animations.");
static const u8 sText_Desc_BattleScene_Off[]= _("Skip Pokémon battle animations.");
static const u8 sText_Desc_SoundMono[]      = _("Sound is the same in all speakers.\n{COLOR 8}{SHADOW 7}Recommended for original hardware.");
static const u8 sText_Desc_SoundStereo[]    = _("Play the left and right audio channel\nseperatly. {COLOR 10}{SHADOW 11}Great with headphones.");
static const u8 sText_Desc_ButtonMode[]     = _("Standard button configuration.");
static const u8 sText_Desc_ButtonMode_LR[]  = _("On some screens the L and R buttons\nact as left and right.");
static const u8 sText_Desc_ButtonMode_LA[]  = _("The L button acts as another A\nbutton for one-handed play.");
static const u8 sText_Desc_FontRSE[]        = _("Use the {COLOR 8}{SHADOW 7}Ruby{COLOR 2}{SHADOW 4}/{COLOR 6}{SHADOW 5}Sapphire{COLOR 2}{SHADOW 4}/{COLOR 12}{SHADOW 11}Emerald\n{COLOR 2}{SHADOW 4}font.");
static const u8 sText_Desc_FontFRLG[]       = _("Use the {COLOR 14}{SHADOW 13}FireRed{COLOR 2}{SHADOW 4}/{COLOR 10}{SHADOW 9}LeafGreen{COLOR 2}{SHADOW 4} font.");
static const u8 sText_Desc_FrameType[]      = _("Select a window border style.");
static const u8 sText_Desc_SaveAndExit[]    = _("Save your settings.");

static const u8 *const sOptionMenuItemDescriptionsCustom[MENUITEM_SYSTEM_COUNT][3] =
{
    [MENUITEM_SYSTEM_TEXTSPEED]   = {sText_Desc_TextSpeed,            sText_Empty,                sText_Empty},
    [MENUITEM_SYSTEM_BATTLESCENE] = {sText_Desc_BattleScene_On,       sText_Desc_BattleScene_Off, sText_Empty},
    [MENUITEM_SYSTEM_SOUND]       = {sText_Desc_SoundMono,            sText_Desc_SoundStereo,     sText_Empty},
    [MENUITEM_SYSTEM_BUTTONMODE]  = {sText_Desc_ButtonMode,           sText_Desc_ButtonMode_LR,   sText_Desc_ButtonMode_LA},
    [MENUITEM_SYSTEM_FONT]        = {sText_Desc_FontRSE,              sText_Desc_FontFRLG,        sText_Empty},
    [MENUITEM_SYSTEM_FRAMETYPE]   = {sText_Desc_FrameType,            sText_Empty,                sText_Empty},
    [MENUITEM_SYSTEM_CANCEL]      = {sText_Desc_Save,                 sText_Empty,                sText_Empty},
};

// Disabled Descriptions
static const u8 *const sOptionMenuItemDescriptionsDisabledMain[MENUITEM_MAIN_COUNT] =
{
    [MENUITEM_MAIN_DIFFICULTY]  = sText_Empty,
    [MENUITEM_MAIN_BATTLESTYLE] = sText_Empty,
    [MENUITEM_MAIN_MATCHCALL]   = sText_Empty,
    [MENUITEM_MAIN_CATCHMODE]   = sText_Empty,
    [MENUITEM_MAIN_BALLPROMPT]  = sText_Empty,
    [MENUITEM_MAIN_AUTORUN]     = sText_Empty,
    [MENUITEM_MAIN_BOXMODE]     = sText_Empty,
    [MENUITEM_MAIN_CANCEL]      = sText_Empty,
};

// Disabled Custom
static const u8 *const sOptionMenuItemDescriptionsDisabledCustom[MENUITEM_SYSTEM_COUNT] =
{
    [MENUITEM_SYSTEM_TEXTSPEED]   = sText_Empty,
    [MENUITEM_SYSTEM_BATTLESCENE] = sText_Empty,
    [MENUITEM_SYSTEM_SOUND]       = sText_Empty,
    [MENUITEM_SYSTEM_BUTTONMODE]  = sText_Empty,
    [MENUITEM_SYSTEM_FONT]        = sText_Empty,
    [MENUITEM_SYSTEM_FRAMETYPE]   = sText_Empty,
    [MENUITEM_SYSTEM_CANCEL]      = sText_Empty,
};

static const u8 *const OptionTextDescription(void)
{
    u8 menuItem = sOptions->menuCursor[sOptions->submenu];
    u8 selection;

    switch (sOptions->submenu)
    {
        case MENU_SYSTEM:
            if (!CheckConditions(menuItem))
                return sOptionMenuItemDescriptionsDisabledCustom[menuItem];
            selection = sOptions->sel_custom[menuItem];
            if (menuItem == MENUITEM_SYSTEM_TEXTSPEED || menuItem == MENUITEM_SYSTEM_FRAMETYPE)
                selection = 0;
            return sOptionMenuItemDescriptionsCustom[menuItem][selection];
        default:
            if (!CheckConditions(menuItem))
                return sOptionMenuItemDescriptionsDisabledMain[menuItem];
            selection = sOptions->sel[menuItem];
            if (menuItem == MENUITEM_MAIN_AUTORUN)
                selection = 0;
            return sOptionMenuItemDescriptionsMain[menuItem][selection];
    }
}

static u8 MenuItemCount(void)
{
    switch (sOptions->submenu)
    {
        case MENU_SYSTEM:
            return MENUITEM_SYSTEM_COUNT;
        default:
            return MENUITEM_MAIN_COUNT;
    }
}

static u8 MenuItemCancel(void)
{
    switch (sOptions->submenu)
    {
        case MENU_SYSTEM:
            return MENUITEM_SYSTEM_CANCEL;
        default:
            return MENUITEM_MAIN_CANCEL;
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

void DrawCenteredText(u8 windowId, const u8 *text, int y, const u8 *color, int windowWidth)
{
    int textWidth = GetStringWidth(FONT_SHORT, text, 0); 
    int x = (windowWidth - textWidth) / 2;
    AddTextPrinterParameterized3(windowId, FONT_SHORT, x, y, color, 0, text);
}

static const u8 sText_TopBar_Main[]         = _("Gameplay Options");
static const u8 sText_TopBar_Main_Right[]   = _("{R_BUTTON} System");
static const u8 sText_TopBar_Custom[]       = _("System Options");
static const u8 sText_TopBar_Custom_Left[]  = _("{L_BUTTON} Gameplay");
static void DrawTopBarText(void)
{
    FillWindowPixelBuffer(WIN_TOPBAR, PIXEL_FILL(0));
    switch (sOptions->submenu)
    {
        case MENU_MAIN:
            DrawCenteredText(WIN_TOPBAR, sText_TopBar_Main, 1, wTextColors[1], 240);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 190, 1, wTextColors[1], 0, sText_TopBar_Main_Right);
            break;
        case MENU_SYSTEM:
            DrawCenteredText(WIN_TOPBAR, sText_TopBar_Custom, 1, wTextColors[1], 240);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 2, 1, wTextColors[1], 0, sText_TopBar_Custom_Left);
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
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 4, y, 0, 0, wTextColors[1], TEXT_SKIP_DRAW, OptionTextRight(selection));
    else
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 4, y, 0, 0, wTextColors[3], TEXT_SKIP_DRAW, OptionTextRight(selection));
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
        colorPtr = wTextColors[1];
    }
    AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, x, y, 0, 0, colorPtr, TEXT_SKIP_DRAW, text);
}

static void DrawChoices(u32 id, int y) //right side draw function
{
    switch (sOptions->submenu)
    {
        case MENU_MAIN:
            if (sItemFunctionsMain[id].drawChoices != NULL)
                sItemFunctionsMain[id].drawChoices(sOptions->sel[id], y);
            break;
        case MENU_SYSTEM:
            if (sItemFunctionsCustom[id].drawChoices != NULL)
                sItemFunctionsCustom[id].drawChoices(sOptions->sel_custom[id], y);
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

void CB2_InitOptionPlusMenu(void)
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
            //LoadPalette(sOptionMenuText_Pal, 16, sizeof(sOptionMenuText_Pal)); //Added palette to the tiles.png
            gMain.state++;
            break;
        case 6:
            
            sOptions->sel[MENUITEM_MAIN_DIFFICULTY]         = gSaveBlock2Ptr->w_opDifficulty;
            sOptions->sel[MENUITEM_MAIN_BATTLESTYLE]        = gSaveBlock2Ptr->optionsBattleStyle;
            sOptions->sel[MENUITEM_MAIN_MATCHCALL]          = gSaveBlock2Ptr->w_opMatchCall;
            sOptions->sel[MENUITEM_MAIN_CATCHMODE]          = gSaveBlock2Ptr->w_opCatchMode;
            sOptions->sel[MENUITEM_MAIN_BALLPROMPT]         = gSaveBlock2Ptr->w_opBallPrompt;
            sOptions->sel[MENUITEM_MAIN_AUTORUN]            = gSaveBlock2Ptr->w_opAutoRun;
            sOptions->sel[MENUITEM_MAIN_BOXMODE]            = gSaveBlock2Ptr->w_opBoxMode;
            
            sOptions->sel_custom[MENUITEM_SYSTEM_TEXTSPEED]     = gSaveBlock2Ptr->optionsTextSpeed;
            sOptions->sel_custom[MENUITEM_SYSTEM_BATTLESCENE]   = gSaveBlock2Ptr->optionsBattleSceneOff;
            sOptions->sel_custom[MENUITEM_SYSTEM_SOUND]         = gSaveBlock2Ptr->optionsSound;
            sOptions->sel_custom[MENUITEM_SYSTEM_BUTTONMODE]    = gSaveBlock2Ptr->optionsButtonMode;
            sOptions->sel_custom[MENUITEM_SYSTEM_FONT]          = gSaveBlock2Ptr->w_opFontType;
            sOptions->sel_custom[MENUITEM_SYSTEM_FRAMETYPE]     = gSaveBlock2Ptr->optionsWindowFrameType;

            sOptions->submenu = MENU_MAIN;

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
            
            sOptions->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 240 / 2, 20, 110, MENUITEM_MAIN_COUNT - 1, 110, 110, 0);
            

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
        if (sOptions->menuCursor[sOptions->submenu] == MenuItemCancel())
        {
            PlaySE(SE_PC_ON);
            gTasks[taskId].func = Task_OptionMenuSave;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_ON);
        gTasks[taskId].func = Task_OptionMenuSave;
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
        PlaySE(SE_CLICK);
        if (sOptions->submenu == MENU_MAIN)
        {
            int cursor = sOptions->menuCursor[sOptions->submenu];
            u8 previousOption = sOptions->sel[cursor];
            if (CheckConditions(cursor))
            {
                if (sItemFunctionsMain[cursor].processInput != NULL)
                {
                    sOptions->sel[cursor] = sItemFunctionsMain[cursor].processInput(previousOption);
                    ReDrawAll();
                    DrawDescriptionText();
                }

                if (previousOption != sOptions->sel[cursor])
                    DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
            }
        }
        else if (sOptions->submenu == MENU_SYSTEM)
        {
            int cursor = sOptions->menuCursor[sOptions->submenu];
            u8 previousOption = sOptions->sel_custom[cursor];
            if (CheckConditions(cursor))
            {
                if (sItemFunctionsCustom[cursor].processInput != NULL)
                {
                    sOptions->sel_custom[cursor] = sItemFunctionsCustom[cursor].processInput(previousOption);
                    ReDrawAll();
                    DrawDescriptionText();
                }

                if (previousOption != sOptions->sel_custom[cursor])
                    DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
            }
        }
    }
    else if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sOptions->submenu != MENU_SYSTEM)
            sOptions->submenu++;

        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sOptions->submenu != 0)
            sOptions->submenu--;
        
        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    gSaveBlock2Ptr->w_opDifficulty      = sOptions->sel[MENUITEM_MAIN_DIFFICULTY];
    gSaveBlock2Ptr->optionsBattleStyle  = sOptions->sel[MENUITEM_MAIN_BATTLESTYLE];
    gSaveBlock2Ptr->w_opMatchCall       = sOptions->sel[MENUITEM_MAIN_MATCHCALL];
    gSaveBlock2Ptr->w_opCatchMode       = sOptions->sel[MENUITEM_MAIN_CATCHMODE];
    gSaveBlock2Ptr->w_opBallPrompt      = sOptions->sel[MENUITEM_MAIN_BALLPROMPT];
    gSaveBlock2Ptr->w_opAutoRun         = sOptions->sel[MENUITEM_MAIN_AUTORUN];
    gSaveBlock2Ptr->w_opBoxMode         = sOptions->sel[MENUITEM_MAIN_BOXMODE];

    gSaveBlock2Ptr->optionsTextSpeed            = sOptions->sel_custom[MENUITEM_SYSTEM_TEXTSPEED];
    gSaveBlock2Ptr->optionsBattleSceneOff       = sOptions->sel_custom[MENUITEM_SYSTEM_BATTLESCENE];
    gSaveBlock2Ptr->optionsSound                = sOptions->sel_custom[MENUITEM_SYSTEM_SOUND];
    gSaveBlock2Ptr->optionsButtonMode           = sOptions->sel_custom[MENUITEM_SYSTEM_BUTTONMODE];
    gSaveBlock2Ptr->w_opFontType                = sOptions->sel_custom[MENUITEM_SYSTEM_FONT];
    gSaveBlock2Ptr->optionsWindowFrameType      = sOptions->sel_custom[MENUITEM_SYSTEM_FRAMETYPE];

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

static int ProcessInput_Options_Four(int selection)
{
    return XOptions_ProcessInput(4, selection);
}

// Process Input functions ****SPECIFIC****
static int ProcessInput_Sound(int selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
    }

    return selection;
}

static int ProcessInput_FrameType(int selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < WINDOW_FRAMES_COUNT - 1)
            selection++;
        else
            selection = 0;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0x70, 0x20);
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = WINDOW_FRAMES_COUNT - 1;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0x70, 0x20);
    }
    return selection;
}

// Draw Choices functions ****GENERIC****
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active)
{
    bool8 choosen = FALSE;
    if (style != 0)
        choosen = TRUE;

    DrawRightSideChoiceText(text, x, y+1, choosen, active);
}

static void DrawChoices_Options_Four(const u8 *const *const strings, int selection, int y, bool8 active)
{
    static const u8 choiceOrders[][3] =
    {
        {0, 1, 2},
        {0, 1, 2},
        {1, 2, 3},
        {1, 2, 3},
    };
    u8 styles[4] = {0};
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
//Gameplay Draw Choices
static const u8 sText_DifficultyEasy[] = _("Easy");
static const u8 sText_DifficultyNorm[] = _("Normal");
static const u8 sText_DifficultyHard[] = _("Hard");
static void DrawChoices_Difficulty(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_DIFFICULTY);
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
    bool8 active = CheckConditions(MENUITEM_MAIN_BATTLESTYLE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BattleStyleShift, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BattleStyleSet, GetStringRightAlignXOffset(FONT_NORMAL, sText_BattleStyleSet, 198), y, styles[1], active);
}

static const u8 sText_MatchCallOn[] = _("On");
static const u8 sText_MatchCallOff[] = _("Off");
static void DrawChoices_MatchCall(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_MATCHCALL);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_MatchCallOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_MatchCallOn, GetStringRightAlignXOffset(1, sText_MatchCallOn, 198), y, styles[1], active);
}

static const u8 sText_CatchModeOn[] = _("On");
static const u8 sText_CatchModeOff[] = _("Off");
static void DrawChoices_CatchMode(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_CATCHMODE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_CatchModeOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_CatchModeOn, GetStringRightAlignXOffset(1, sText_CatchModeOn, 198), y, styles[1], active);
}

static const u8 sText_BallPromptOn[] = _("On");
static const u8 sText_BallPromptOff[] = _("Off");
static void DrawChoices_BallPrompt(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_BALLPROMPT);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BallPromptOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BallPromptOn, GetStringRightAlignXOffset(1, sText_BallPromptOn, 198), y, styles[1], active);
}

static const u8 sText_AutoRunOn[] = _("On");
static const u8 sText_AutoRunOff[] = _("Off");
static void DrawChoices_AutoRun(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_AUTORUN);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_AutoRunOff, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_AutoRunOn, GetStringRightAlignXOffset(1, sText_AutoRunOn, 198), y, styles[1], active);
}

static const u8 sText_BoxModeClassic[] = _("Classic");
static const u8 sText_BoxModeModern[] = _("Modern");
static void DrawChoices_BoxMode(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_MAIN_BOXMODE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BoxModeClassic, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BoxModeModern, GetStringRightAlignXOffset(1, sText_BoxModeModern, 198), y, styles[1], active);
}

// System Draw Choices
static const u8 sText_TextSpeedSlow[] = _("Slow");
static const u8 sText_TextSpeedMid[] = _("Mid");
static const u8 sText_TextSpeedFast[] = _("Fast");
static const u8 sText_TextSpeedInstant[] = _("Instant");
static const u8 *const sTextSpeedStrings[] = {sText_TextSpeedSlow, sText_TextSpeedMid, sText_TextSpeedFast, sText_TextSpeedInstant};
static void DrawChoices_TextSpeed(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_TEXTSPEED);
    DrawChoices_Options_Four(sTextSpeedStrings, selection, y, active);
}

static const u8 sText_BattleSceneOn[] = _("On");
static const u8 sText_BattleSceneOff[] = _("Off");
static void DrawChoices_BattleScene(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_BATTLESCENE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, sText_BattleSceneOff, 198), y, styles[1], active);
}

static const u8 sText_SoundMono[] = _("Mono");
static const u8 sText_SoundStereo[] = _("Stereo");
static void DrawChoices_Sound(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_SOUND);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_SoundMono, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_SoundStereo, GetStringRightAlignXOffset(FONT_NORMAL, sText_SoundStereo, 198), y, styles[1], active);
}

static const u8 sText_ButtonTypeNormal[] = _("Normal");
static const u8 sText_ButtonTypeLR[] = _("LR");
static const u8 sText_ButtonTypeLEqualsA[] = _("L=A");
static void DrawChoices_ButtonMode(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_BUTTONMODE);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(sText_ButtonTypeNormal, sText_ButtonTypeLR, sText_ButtonTypeLEqualsA);
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_ButtonTypeNormal, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_ButtonTypeLR, xMid, y, styles[1], active);
    DrawOptionMenuChoice(sText_ButtonTypeLEqualsA, GetStringRightAlignXOffset(1, sText_ButtonTypeLEqualsA, 198), y, styles[2], active);
}

static const u8 sText_OptionFontRSE[] = _("RSE");
static const u8 sText_OptionFontFRLG[] = _("FRLG");
static void DrawChoices_Font(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_FONT);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(sText_OptionFontRSE, 104, y, styles[0], active);
    DrawOptionMenuChoice(sText_OptionFontFRLG, GetStringRightAlignXOffset(1, sText_OptionFontFRLG, 198), y, styles[1], active);
}

static const u8 sText_FrameTypeNumber[] = _(" ");
static const u8 sText_FrameType[] = _("Type");
static void DrawChoices_FrameType(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SYSTEM_FRAMETYPE);
    u8 text[16];
    u8 n = selection + 1;
    u16 i;

    for (i = 0; sText_FrameTypeNumber[i] != EOS && i <= 5; i++)
        text[i] = sText_FrameTypeNumber[i];

    // Convert a number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = 0x77;
        i++;
    }

    text[i] = EOS;

    DrawOptionMenuChoice(sText_FrameType, 104, y, 0, active);
    DrawOptionMenuChoice(text, 128, y, 1, active);
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
