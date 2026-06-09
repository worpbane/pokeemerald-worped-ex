// ============================================================================
// PC Storage System - Data
// ============================================================================

// Wallpaper IDs
enum {
	//Wallpapers Page 1
	WALLPAPER_BASE,			//Default
    WALLPAPER_PLAINS, 		//Plains (Grass / Meadow)
    WALLPAPER_CITY,			//City
    WALLPAPER_DESERT, 		//Desert
    WALLPAPER_CENTER, 		//PokeCenter
	//Wallpapers Page 2
    WALLPAPER_SHORE,		//Surface Water
    WALLPAPER_OCEAN,		//Underwater
    WALLPAPER_MOUNTAIN, 	//Mountain
    WALLPAPER_VOLCANO,		//Volcano
    WALLPAPER_CAVE, 		//Cave
	//Wallpapers Page 3
    WALLPAPER_BEACH,		//Beach
    WALLPAPER_SNOW, 		//Snow
    WALLPAPER_SKY, 			//Sky
    WALLPAPER_COMPUTA, 		//PC
    WALLPAPER_CUTE, 		//Cute (Cross Stitch)
	//Wallpapers Page 4
    WALLPAPER_SPACE,		// Spaic
    WALLPAPER_DAYCARE,		// Daycare
    WALLPAPER_CONTEST,		// Contest Stage
    WALLPAPER_CLASSIC,		// Classic
    WALLPAPER_CLASSIC2,		// Classic 2
    WALLPAPER_COUNT
};
#define MAX_DEFAULT_WALLPAPER WALLPAPER_FLYING

// ============================================================================
// Structs
// ============================================================================

struct Wallpaper
{
    const u32 *tiles;
    const u32 *tilemap;
    const u16 *palettes;
};

struct StorageMessage
{
    const u8 *text;
    u8 format;
};

// ============================================================================
// Graphics - Storage System UI
// ============================================================================

static const u32 sSwShStorage_Gfx[]           = INCGFX_U32("graphics/pokemon_storage/swsh/tiles.png", ".4bpp.smol");
static const u16 sSwShStorage_Pal[]           = INCGFX_U16("graphics/pokemon_storage/swsh/tiles.png", ".gbapal");
static const u32 sSwShStorage_BG1_Tilemap[]   = INCGFX_U32("graphics/pokemon_storage/swsh/bg1.bin", ".smolTM");
static const u32 sSwShStorage_BG2_Tilemap[]   = INCGFX_U32("graphics/pokemon_storage/swsh/bg2.bin", ".smolTM");
static const u32 sMonInfo_Gfx[]               = INCGFX_U32("graphics/pokemon_storage/swsh/mon_info.png", ".4bpp.smol");
static const u32 sMonInfo_Tilemap[]           = INCGFX_U32("graphics/pokemon_storage/swsh/mon_info.bin", ".smolTM");
static const u16 sTextWindows_Pal[]           = INCGFX_U16("graphics/pokemon_storage/swsh/text_windows.pal", ".gbapal");

static const u32 sBoxTitleFrame_Gfx[]         = INCGFX_U32("graphics/pokemon_storage/swsh/box_title_frame.png", ".4bpp.smol");
static const u32 sBoxTitleArrow_Gfx[]         = INCGFX_U32("graphics/pokemon_storage/swsh/box_title_arrow.png", ".4bpp.smol");
static const u32 sChooseBoxMenu_Gfx[]         = INCGFX_U32("graphics/pokemon_storage/swsh/choose_box_menu.png", ".4bpp.smol");
static const u32 sCursor_Gfx[]                = INCGFX_U32("graphics/pokemon_storage/swsh/cursor.png", ".4bpp.smol");
static const u16 sCursor_Pal[]                = INCGFX_U16("graphics/pokemon_storage/swsh/cursor.png", ".gbapal");
static const u32 sGenderIcons_Gfx[]           = INCGFX_U32("graphics/pokemon_storage/swsh/gender_icons.png", ".4bpp.smol");
static const u16 sMarkings_Pal[]              = INCGFX_U16("graphics/pokemon_storage/swsh/markings.pal", ".gbapal");
static const u32 sMarkingsMenu_Gfx[]          = INCGFX_U32("graphics/pokemon_storage/swsh/markings_menu.png", ".4bpp.smol");
static const u32 sMessageWindow_Gfx[]         = INCGFX_U32("graphics/pokemon_storage/swsh/message_window.png", ".4bpp.smol");
static const u32 sShinyIcon_Gfx[]             = INCGFX_U32("graphics/pokemon_storage/swsh/shiny_icon.png", ".4bpp.smol");
static const u32 sPokerusIcon_Gfx[]           = INCGFX_U32("graphics/pokemon_storage/swsh/pokerus_icon.png", ".4bpp.smol");
static const u32 sStatLabels_Gfx[]            = INCGFX_U32("graphics/pokemon_storage/swsh/stat_labels.png", ".4bpp.smol");
static const ALIGNED(4) u8 sTypeIcons_Gfx[]   = INCGFX_U8("graphics/pokemon_storage/swsh/type_icons.png", ".4bpp");
static const u16 sTypeIcons_Pal[]             = INCGFX_U16("graphics/pokemon_storage/swsh/type_icons.png", ".gbapal");

// ============================================================================
// Graphics - Wallpapers
// ============================================================================

static const u32 sWallpaperTiles_Base[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/base.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Base[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/base.bin", ".smolTM");
static const u16 sWallpaperPalette_Base[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/base.png", ".gbapal");

static const u32 sWallpaperTiles_Plains[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/plains.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Plains[]   = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/plains.bin", ".smolTM");
static const u16 sWallpaperPalette_Plains[]   = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/plains.png", ".gbapal");

static const u32 sWallpaperTiles_City[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/city.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_City[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/city.bin", ".smolTM");
static const u16 sWallpaperPalette_City[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/city.png", ".gbapal");

static const u32 sWallpaperTiles_Desert[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/desert.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Desert[]   = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/desert.bin", ".smolTM");
static const u16 sWallpaperPalette_Desert[]   = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/desert.png", ".gbapal");

static const u32 sWallpaperTiles_Center[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/center.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Center[]   = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/center.bin", ".smolTM");
static const u16 sWallpaperPalette_Center[]   = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/center.png", ".gbapal");

static const u32 sWallpaperTiles_Shore[]      = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/river.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Shore[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/river.bin", ".smolTM");
static const u16 sWallpaperPalette_Shore[]    = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/river.png", ".gbapal");

static const u32 sWallpaperTiles_Ocean[]      = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/ocean.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Ocean[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/ocean.bin", ".smolTM");
static const u16 sWallpaperPalette_Ocean[]    = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/ocean.png", ".gbapal");

static const u32 sWallpaperTiles_Mountain[]   = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/mountain.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Mountain[] = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/mountain.bin", ".smolTM");
static const u16 sWallpaperPalette_Mountain[] = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/mountain.png", ".gbapal");

static const u32 sWallpaperTiles_Volcano[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/volcano.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Volcano[]  = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/volcano.bin", ".smolTM");
static const u16 sWallpaperPalette_Volcano[]  = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/volcano.png", ".gbapal");

static const u32 sWallpaperTiles_Cave[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/cave.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Cave[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/cave.bin", ".smolTM");
static const u16 sWallpaperPalette_Cave[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/cave.png", ".gbapal");

static const u32 sWallpaperTiles_Beach[]      = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/beach.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Beach[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/beach.bin", ".smolTM");
static const u16 sWallpaperPalette_Beach[]    = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/beach.png", ".gbapal");

static const u32 sWallpaperTiles_Snow[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/snow.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Snow[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/snow.bin", ".smolTM");
static const u16 sWallpaperPalette_Snow[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/snow.png", ".gbapal");

static const u32 sWallpaperTiles_Sky[]        = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/sky.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Sky[]      = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/sky.bin", ".smolTM");
static const u16 sWallpaperPalette_Sky[]      = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/sky.png", ".gbapal");

static const u32 sWallpaperTiles_Computa[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/computa.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Computa[]  = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/computa.bin", ".smolTM");
static const u16 sWallpaperPalette_Computa[]  = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/computa.png", ".gbapal");

static const u32 sWallpaperTiles_Cute[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/cute.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Cute[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/cute.bin", ".smolTM");
static const u16 sWallpaperPalette_Cute[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/cute.png", ".gbapal");

static const u32 sWallpaperTiles_Space[]      = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/space.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Space[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/space.bin", ".smolTM");
static const u16 sWallpaperPalette_Space[]    = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/space.png", ".gbapal");

static const u32 sWallpaperTiles_Daycare[]       = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/daycare.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Daycare[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/daycare.bin", ".smolTM");
static const u16 sWallpaperPalette_Daycare[]     = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/daycare.png", ".gbapal");

static const u32 sWallpaperTiles_Contest[]     = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/contest.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Contest[]   = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/contest.bin", ".smolTM");
static const u16 sWallpaperPalette_Contest[]   = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/contest.png", ".gbapal");

static const u32 sWallpaperTiles_Classic[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/classic.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Classic[]  = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/classic.bin", ".smolTM");
static const u16 sWallpaperPalette_Classic[]  = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/classic.png", ".gbapal");

static const u32 sWallpaperTiles_Classic2[]    = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/classic2.png", ".4bpp.smol");
static const u32 sWallpaperTilemap_Classic2[]  = INCGFX_U32("graphics/pokemon_storage/swsh/wallpapers/classic2.bin", ".smolTM");
static const u16 sWallpaperPalette_Classic2[]  = INCGFX_U16("graphics/pokemon_storage/swsh/wallpapers/classic2.png", ".gbapal");

#define WALLPAPER_ENTRY(name) {sWallpaperTiles_##name, sWallpaperTilemap_##name, sWallpaperPalette_##name}

static const struct Wallpaper sSwShWallpapers[] =
{
    // --- PAGE 1 ---
    [WALLPAPER_BASE]     = WALLPAPER_ENTRY(Base),
    [WALLPAPER_PLAINS]   = WALLPAPER_ENTRY(Plains),
    [WALLPAPER_CITY]     = WALLPAPER_ENTRY(City),
    [WALLPAPER_DESERT]   = WALLPAPER_ENTRY(Desert),
    [WALLPAPER_CENTER]   = WALLPAPER_ENTRY(Center),

    // --- PAGE 2 ---
    [WALLPAPER_SHORE]    = WALLPAPER_ENTRY(Shore),
    [WALLPAPER_OCEAN]    = WALLPAPER_ENTRY(Ocean),
    [WALLPAPER_MOUNTAIN] = WALLPAPER_ENTRY(Mountain),
    [WALLPAPER_VOLCANO]  = WALLPAPER_ENTRY(Volcano),
    [WALLPAPER_CAVE]     = WALLPAPER_ENTRY(Cave),

    // --- PAGE 3 ---
    [WALLPAPER_BEACH]    = WALLPAPER_ENTRY(Beach),
    [WALLPAPER_SNOW]     = WALLPAPER_ENTRY(Snow),
    [WALLPAPER_SKY]      = WALLPAPER_ENTRY(Sky),
    [WALLPAPER_COMPUTA]  = WALLPAPER_ENTRY(Computa),
    [WALLPAPER_CUTE]     = WALLPAPER_ENTRY(Cute),

    // --- PAGE 4 ---
    [WALLPAPER_SPACE]    = WALLPAPER_ENTRY(Space),
    [WALLPAPER_DAYCARE]  = WALLPAPER_ENTRY(Daycare),
    [WALLPAPER_CONTEST]  = WALLPAPER_ENTRY(Contest),
    [WALLPAPER_CLASSIC]  = WALLPAPER_ENTRY(Classic),
    [WALLPAPER_CLASSIC2]  = WALLPAPER_ENTRY(Classic2),
};

// ============================================================================
// Text Strings
// ============================================================================

static const u8 gText_JustOnePkmn[]   = _("There is just one Pokémon with you.");
static const u8 gText_PartyFull[]     = _("Your party is full!");
static const u8 sText_Lv[]            = _("Lv");
static const u8 gPCText_Give[]        = _("Give");

struct {
    const u8 *text;
    const u8 *desc;
} static const sMainMenuTexts[OPTIONS_COUNT] =
{
    [OPTION_WITHDRAW]   = {COMPOUND_STRING("Withdraw Pokémon"), COMPOUND_STRING("Move Pokémon stored in boxes to\nyour party.")},
    [OPTION_DEPOSIT]    = {COMPOUND_STRING("Deposit Pokémon"),  COMPOUND_STRING("Store Pokémon in your party in boxes.")},
    [OPTION_MOVE_MONS]  = {COMPOUND_STRING("Move Pokémon"),     COMPOUND_STRING("Organize the Pokémon in boxes and\nin your party.")},
    [OPTION_MOVE_ITEMS] = {COMPOUND_STRING("Move Items"),       COMPOUND_STRING("Move items held by any Pokémon\nin a box or your party.")},
    [OPTION_EXIT]       = {COMPOUND_STRING("See ya!"),          COMPOUND_STRING("Return to the previous menu.")}
};

static const u8 *const sMenuTexts[] =
{
    [MENU_CANCEL]     = COMPOUND_STRING("Cancel"),
    [MENU_STORE]      = COMPOUND_STRING("Store"),
    [MENU_WITHDRAW]   = COMPOUND_STRING("Withdraw"),
    [MENU_MOVE]       = COMPOUND_STRING("Move"),
    [MENU_SHIFT]      = COMPOUND_STRING("Shift"),
    [MENU_PLACE]      = COMPOUND_STRING("Place"),
    [MENU_SUMMARY]    = COMPOUND_STRING("Summary"),
    [MENU_RELEASE]    = COMPOUND_STRING("Release"),
    [MENU_MARK]       = COMPOUND_STRING("Mark"),
    [MENU_JUMP]       = COMPOUND_STRING("Jump"),
    [MENU_WALLPAPER]  = COMPOUND_STRING("Wallpaper"),
    [MENU_NAME]       = COMPOUND_STRING("Name"),
    [MENU_TAKE]       = COMPOUND_STRING("Take"),
    [MENU_GIVE]       = gPCText_Give,
    [MENU_GIVE_2]     = gPCText_Give,
    [MENU_SWITCH]     = COMPOUND_STRING("Switch"),
    [MENU_BAG]        = COMPOUND_STRING("Bag"),
    [MENU_SELECT]     = COMPOUND_STRING("Select"),
    //Wallpapers Page 1
    [MENU_BASE]       = COMPOUND_STRING("Default"),
    [MENU_PLAINS]     = COMPOUND_STRING("Plains"),
    [MENU_CITY]       = COMPOUND_STRING("City"),
    [MENU_DESERT]     = COMPOUND_STRING("Desert"),
    [MENU_CENTER]     = COMPOUND_STRING("Center"),
    //Wallpapers Page 2
    [MENU_SHORE]      = COMPOUND_STRING("River"),
    [MENU_OCEAN]      = COMPOUND_STRING("Seabed"),
    [MENU_MOUNTAIN]   = COMPOUND_STRING("Mountain"),
    [MENU_VOLCANO]    = COMPOUND_STRING("Volcano"),
    [MENU_CAVE]       = COMPOUND_STRING("Cave"),
	//Wallpapers Page 3
    [MENU_BEACH]      = COMPOUND_STRING("Beach"),
    [MENU_SNOW]       = COMPOUND_STRING("Snow"),
    [MENU_SKY]        = COMPOUND_STRING("Sky"),
    [MENU_COMPUTA]    = COMPOUND_STRING("Machine"),
    [MENU_CUTE]       = COMPOUND_STRING("Checks"),
	//Wallpapers Page 4
    [MENU_SPACE]      = COMPOUND_STRING("Space"),
    [MENU_DAYCARE]    = COMPOUND_STRING("Daycare"),
    [MENU_CONTEST]   = COMPOUND_STRING("Contest"),
    [MENU_CLASSIC]    = COMPOUND_STRING("Classic"),
    [MENU_CLASSIC2]   = COMPOUND_STRING("Classic 2"),
    [MENU_COUNT]      = gText_EmptyString2,
};

// ============================================================================
// Messages
// ============================================================================

static const struct StorageMessage sMessages[] =
{
    [MSG_BOX_IS_FULL]          = {COMPOUND_STRING("The box is full."),           MSG_VAR_NONE},
    [MSG_RELEASE_POKE]         = {COMPOUND_STRING("Release this Pokémon?"),      MSG_VAR_NONE},
    [MSG_WAS_RELEASED]         = {COMPOUND_STRING("{DYNAMIC 0} was released."),  MSG_VAR_RELEASE_MON_1},
    [MSG_BYE_BYE]              = {COMPOUND_STRING("Bye-bye, {DYNAMIC 0}!"),      MSG_VAR_RELEASE_MON_3},
    [MSG_LAST_POKE]            = {COMPOUND_STRING("That's your last Pokémon!"),  MSG_VAR_NONE},
    [MSG_PARTY_FULL]           = {gText_YourPartysFull,                          MSG_VAR_NONE},
    [MSG_HOLDING_POKE]         = {COMPOUND_STRING("You're holding a Pokémon!"),  MSG_VAR_NONE},
    [MSG_WHICH_ONE_WILL_TAKE]  = {COMPOUND_STRING("Which one will you take?"),   MSG_VAR_NONE},
    [MSG_CANT_RELEASE_EGG]     = {COMPOUND_STRING("You can't release an egg."),  MSG_VAR_NONE},
    [MSG_CONTINUE_BOX]         = {COMPOUND_STRING("Continue P.C. operations?"),  MSG_VAR_NONE},
    [MSG_CAME_BACK]            = {COMPOUND_STRING("{DYNAMIC 0} came back!"),     MSG_VAR_MON_NAME_1},
    [MSG_WORRIED]              = {COMPOUND_STRING("Was it worried about you?"),  MSG_VAR_NONE},
    [MSG_SURPRISE]             = {COMPOUND_STRING("… … … … !"),                  MSG_VAR_NONE},
    [MSG_PLEASE_REMOVE_MAIL]   = {COMPOUND_STRING("Please remove the mail."),    MSG_VAR_NONE},
    [MSG_PLACED_IN_BAG]        = {COMPOUND_STRING("Placed item in the bag."),    MSG_VAR_ITEM_NAME},
    [MSG_BAG_FULL]             = {COMPOUND_STRING("The bag is full."),           MSG_VAR_NONE},
    [MSG_PUT_IN_BAG]           = {COMPOUND_STRING("Put this item in the bag?"),  MSG_VAR_NONE},
    [MSG_CANT_STORE_MAIL]      = {COMPOUND_STRING("Mail can't be stored!"),      MSG_VAR_NONE},
};

// ============================================================================
// Window Templates
// ============================================================================

static const struct WindowTemplate sWindowTemplate_MainMenu =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 17,
    .height = 10,
    .paletteNum = 15,
    .baseBlock = 0x1,
};

static const struct WindowTemplate sYesNoWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 24,
    .tilemapTop = 11,
    .width = 5,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x5C,
};

static const struct WindowTemplate sWindowTemplate_MultiMove =
{
    .bg = 0,
    .tilemapLeft = 10,
    .tilemapTop = 3,
    .width = 19,
    .height = 16,
    .paletteNum = 9,
    .baseBlock = 0x1,
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_MESSAGE] = {
        .bg = 0,
        .tilemapLeft = 9,
        .tilemapTop = 17,
        .width = 20,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 44,
    },
    [WIN_ITEM_DESC] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 13,
        .width = 21,
        .height = 7,
        .paletteNum = 15,
        .baseBlock = 44,
    },
    [WIN_MON_INFO_NICKNAME_LEFT] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 23,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 202
    },
    [WIN_MON_INFO_LEVEL_LEFT] = {
        .bg = 0,
        .tilemapLeft = 9,
        .tilemapTop = 23,
        .width = 4,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 218
    },
    [WIN_MON_INFO_STATS_COL1_LEFT] = {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 27,
        .width = 3,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 226
    },
    [WIN_MON_INFO_STATS_COL2_LEFT] = {
        .bg = 0,
        .tilemapLeft = 10,
        .tilemapTop = 27,
        .width = 3,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 244
    },
    [WIN_MON_INFO_ABILITY_LEFT] = {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 34,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 262
    },
    [WIN_MON_INFO_ITEM_LEFT] = {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 36,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 280
    },
    [WIN_MON_INFO_NICKNAME_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 43,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 202
    },
    [WIN_MON_INFO_LEVEL_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 26,
        .tilemapTop = 43,
        .width = 4,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 218
    },
    [WIN_MON_INFO_STATS_COL1_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 20,
        .tilemapTop = 47,
        .width = 3,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 226
    },
    [WIN_MON_INFO_STATS_COL2_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 27,
        .tilemapTop = 47,
        .width = 3,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 244
    },
    [WIN_MON_INFO_ABILITY_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 54,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 262
    },
    [WIN_MON_INFO_ITEM_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 56,
        .width = 9,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 280
    },
    DUMMY_WIN_TEMPLATE
};

static const u8 sTextColors[][3] =
{
    {1, 2, 3}, // Standard menus, mon info (stats, ability, item)
    {4, 2, 5}, // Mon info (nickname and level) (grey BG)
    {0, 4, 7}, // Choose box menu - actually uses PALTAG_MISC_3 and not bg pal 15
    {0, 1, 6}, // Main message window
};


// ============================================================================
// BG Templates
// ============================================================================

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 2,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 2,
        .mapBaseIndex = 26,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 27,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    },
};

// ============================================================================
// Choose Box Menu Sprites
// ============================================================================

static const struct OamData sOamData_ChooseBoxMenu =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(32x32),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_ChooseBoxMenu =
{
    .data = sChooseBoxMenu_Gfx,
    .size = (32 * 32) / 2,
    .tag = GFXTAG_BOX_SELECTION,
};
    
static const struct SpriteTemplate sSpriteTemplate_ChooseBoxMenu =
{
    .tileTag = GFXTAG_BOX_SELECTION,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_ChooseBoxMenu,
};

// ============================================================================
// Box Selection Mon Count Sprite (loaded dynamically)
// ============================================================================

static const struct OamData sOamData_ChooseBoxMenu_MonCount =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(16x16),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteTemplate sSpriteTemplate_ChooseBoxMenu_MonCount =
{
    .tileTag = GFXTAG_BOX_SELECTION_PER_30,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_ChooseBoxMenu_MonCount,
};

// ============================================================================
// Box Title Frame Sprites
// ============================================================================

static const struct OamData sOamData_BoxTitleFrame =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(32x16),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(32x16),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_BoxTitleFrame_0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_BoxTitleFrame_1[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_BoxTitleFrame_2[] = {
    ANIMCMD_FRAME(0, 0, TRUE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_BoxTitleFrame[] = {
    sSpriteAnim_BoxTitleFrame_0,
    sSpriteAnim_BoxTitleFrame_1,
    sSpriteAnim_BoxTitleFrame_2,
};

static const u8 sBoxTitleFrameAnims[4] = {0, 1, 1, 2};

static const struct CompressedSpriteSheet sSpriteSheet_BoxTitleFrame =
{
    .data = sBoxTitleFrame_Gfx,
    .size = (32 * 16 * 2) / 2,
    .tag = GFXTAG_BOX_TITLE_FRAME,
};

static const struct SpriteTemplate sSpriteTemplate_BoxTitleFrame =
{
    .tileTag = GFXTAG_BOX_TITLE_FRAME,
    .paletteTag = PALTAG_MISC_1,
    .oam = &sOamData_BoxTitleFrame,
    .anims = sSpriteAnimTable_BoxTitleFrame,
};

// ============================================================================
// Box Title Arrow Sprites
// ============================================================================

static const struct OamData sOamData_BoxTitleArrow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(8x8),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(8x8),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_BoxTitleArrow_Left[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_BoxTitleArrow_Right[] = {
    ANIMCMD_FRAME(0, 0, TRUE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_BoxTitleArrow[] = {
    sSpriteAnim_BoxTitleArrow_Left,
    sSpriteAnim_BoxTitleArrow_Right,
};

static const struct CompressedSpriteSheet sSpriteSheet_BoxTitleArrow =
{
    .data = sBoxTitleArrow_Gfx,
    .size = (8 * 8) / 2,
    .tag = GFXTAG_BOX_TITLE_ARROW,
};

static const struct SpriteTemplate sSpriteTemplate_BoxTitleArrow =
{
    .tileTag = GFXTAG_BOX_TITLE_ARROW,
    .paletteTag = PALTAG_MISC_1,
    .oam = &sOamData_BoxTitleArrow,
    .anims = sSpriteAnimTable_BoxTitleArrow,
    .callback = SpriteCB_Arrow,
};

// ============================================================================
// Box Title Sprites
// ============================================================================

static const u16 sUnusedColor = RGB(26, 29, 8);

#define BOX_TITLE_FRAME_MAIN    RGB_WHITE
#define BOX_TITLE_TEXT_MAIN     RGB(29, 29, 29)
#define BOX_TITLE_SHADOW_MAIN   RGB(5, 5, 5)

#define BOX_TITLE_FRAME_HOVER   RGB(5, 5, 5)
#define BOX_TITLE_TEXT_HOVER    RGB_WHITE
#define BOX_TITLE_SHADOW_HOVER  RGB(14, 14, 14)

static const struct OamData sOamData_BoxTitle =
{
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
    .priority = 2
};

static const union AnimCmd sAnim_BoxTitle_Left[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BoxTitle_Right[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_BoxTitle[] =
{
    sAnim_BoxTitle_Left,
    sAnim_BoxTitle_Right
};

static const struct SpriteTemplate sSpriteTemplate_BoxTitle =
{
    .tileTag = GFXTAG_BOX_TITLE,
    .paletteTag = PALTAG_MISC_1,
    .oam = &sOamData_BoxTitle,
    .anims = sAnims_BoxTitle,
};

// ============================================================================
// Cursor Sprites
// ============================================================================

static const struct CompressedSpriteSheet sSpriteSheet_Cursor[] =
{
    {
        .data = sCursor_Gfx,
        .size = (16 * 16 * 3) / 2,
        .tag = GFXTAG_CURSOR,
    },
    {},
};

static const struct SpritePalette sSpritePal_Cursor[] =
{
    {
        .data = sCursor_Pal,
        .tag = PALTAG_MISC_1,
    },
    {
        .data = sCursor_Pal + 16,
        .tag = PALTAG_MISC_2,
    },
    {
        .data = sCursor_Pal + 32,
        .tag = PALTAG_MISC_3,
    },
    {},
};

static const struct OamData sOamData_Cursor =
{
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const union AnimCmd sAnim_Cursor_Bouncing[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Cursor_Main[] =
{
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Cursor[] =
{
    [CURSOR_ANIM_BOUNCE] = sAnim_Cursor_Bouncing,
    [CURSOR_ANIM_MAIN]   = sAnim_Cursor_Main,
};

static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = GFXTAG_CURSOR,
    .paletteTag = PALTAG_MISC_1,
    .oam = &sOamData_Cursor,
    .anims = sAnims_Cursor,
};

// ============================================================================
// Mon Icon Sprites
// ============================================================================

static const struct OamData sOamData_MonIcon;
static const struct SpriteTemplate sSpriteTemplate_MonIcon =
{
    .tileTag = GFXTAG_MON_ICON,
    .paletteTag = PALTAG_MON_ICON_0,
    .oam = &sOamData_MonIcon,
};

static const struct OamData sOamData_MonIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AffineAnimCmd sAffineAnim_ReleaseMon_Release[] =
{
    AFFINEANIMCMD_FRAME(-2, -2, 0, 120),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ReleaseMon_CameBack[] =
{
    AFFINEANIMCMD_FRAME(16, 16, 0, 0),
    AFFINEANIMCMD_FRAME(16, 16, 0, 15),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ReleaseMon[] =
{
    [RELEASE_ANIM_RELEASE]   = sAffineAnim_ReleaseMon_Release,
    [RELEASE_ANIM_CAME_BACK] = sAffineAnim_ReleaseMon_CameBack
};

// ============================================================================
// Gender Icon Sprites
// ============================================================================

static const struct OamData sOamData_GenderIcons =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 7,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_GenderFemale[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_GenderMale[] = {
    ANIMCMD_FRAME(2, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_GenderIcons[] = {
    sSpriteAnim_GenderFemale,
    sSpriteAnim_GenderMale,
};

static const struct CompressedSpriteSheet sSpriteSheet_GenderIcons =
{
    .data = sGenderIcons_Gfx,
    .size = (8 * 16 * 2) / 2,
    .tag = GFXTAG_GENDER_ICON
};

static const struct SpriteTemplate sSpriteTemplate_GenderIcons =
{
    .tileTag = GFXTAG_GENDER_ICON,
    .paletteTag = PALTAG_MISC_2,
    .oam = &sOamData_GenderIcons,
    .anims = sSpriteAnimTable_GenderIcons,
    .images = NULL,
};

// ============================================================================
// Shiny Icon Sprites
// ============================================================================

static const struct OamData sOamData_ShinyIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_ShinyIcon =
{
    .data = sShinyIcon_Gfx,
    .size = (8 * 8) / 2,
    .tag = GFXTAG_SHINY_ICON
};

static const struct SpriteTemplate sSpriteTemplate_ShinyIcon =
{
    .tileTag = GFXTAG_SHINY_ICON,
    .paletteTag = PALTAG_MISC_2,
    .oam = &sOamData_ShinyIcon,
};

// ============================================================================
// Pokerus Icon Sprites
// ============================================================================

static const struct OamData sOamData_PokerusIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_PokerusIcon =
{
    .data = sPokerusIcon_Gfx,
    .size = (32 * 8) / 2,
    .tag = GFXTAG_PKRS_ICON
};

static const struct SpriteTemplate sSpriteTemplate_PokerusIcon =
{
    .tileTag = GFXTAG_PKRS_ICON,
    .paletteTag = PALTAG_MISC_1,
    .oam = &sOamData_PokerusIcon,
	.anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// ============================================================================
// Stat Label Sprites
// ============================================================================

static const struct OamData sOamData_StatLabels =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 0,
};

static const union AnimCmd sSpriteAnim_StatAtk[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatDef[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatSpAtk[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatSpDef[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatSpeed[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_StatLabels[] = {
    sSpriteAnim_StatAtk,
    sSpriteAnim_StatDef,
    sSpriteAnim_StatSpAtk,
    sSpriteAnim_StatSpDef,
    sSpriteAnim_StatSpeed,
};

static const struct CompressedSpriteSheet sSpriteSheet_StatLabels =
{
    .data = sStatLabels_Gfx,
    .size = (16 * 16 * 5) / 2,
    .tag = GFXTAG_STAT_LABELS
};

static const struct SpriteTemplate sSpriteTemplate_StatLabels =
{
    .tileTag = GFXTAG_STAT_LABELS,
    .paletteTag = PALTAG_MISC_2,
    .oam = &sOamData_StatLabels,
    .anims = sSpriteAnimTable_StatLabels,
};

// ============================================================================
// Type Icon Sprites
// ============================================================================

static const struct OamData sOamData_TypeIcons =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

// Type icons now use DMA copy instead of animations to save VRAM (only loads 2 slots instead of all 21 types)
// Uncompressed sprite sheet (only 2 slots loaded to save VRAM)
static const struct SpriteSheet sSpriteSheet_TypeIcons =
{
    .data = sTypeIcons_Gfx,
    .size = 2 * 0x100, // Only load 2 type icon slots (saves 4.75 KB VRAM)
    .tag = GFXTAG_TYPE_ICON,
};

static const struct SpriteTemplate sSpriteTemplate_TypeIcons =
{
    .tileTag = GFXTAG_TYPE_ICON,
    .paletteTag = PALTAG_TYPE_ICON,
    .oam = &sOamData_TypeIcons,
};

// ============================================================================
// Markings Menu Sprites
// ============================================================================

static const struct CompressedSpriteSheet sSpriteSheet_MarkingsMenu =
{
    .data = sMarkingsMenu_Gfx,
    .size = (
        32 * 32 * 2 + // marking menu window
        8 * 8 * 8 +   // marking menu marks
        16 * 16 * 3   // marking menu cursor
    ) / 2,
    .tag = GFXTAG_MARKING_MENU,
};

static const struct OamData sOamData_MarkingsMenu_Window =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_MarkingsMenu_Window_0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_Window_1[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_Window_2[] = {
    ANIMCMD_FRAME(0, 0, TRUE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_MarkingsMenu_Window[] = {
    sAnim_MarkingsMenu_Window_0,
    sAnim_MarkingsMenu_Window_1,
    sAnim_MarkingsMenu_Window_2,
};

static const u8 sMarkingsMenu_WindowAnims[3] = {0, 1, 2};

static const struct SpriteTemplate sSpriteTemplate_MarkingsMenu_Window =
{
    .tileTag = GFXTAG_MARKING_MENU,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_MarkingsMenu_Window,
    .anims = sAnims_MarkingsMenu_Window,
};

static const struct OamData sOamData_MarkingsMenu_Marks =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_MarkingsMenu_CircleOff[] = {
    ANIMCMD_FRAME(32, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_CircleOn[] = {
    ANIMCMD_FRAME(33, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_SquareOff[] = {
    ANIMCMD_FRAME(34, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_SquareOn[] = {
    ANIMCMD_FRAME(35, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_TriangleOff[] = {
    ANIMCMD_FRAME(36, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_TriangleOn[] = {
    ANIMCMD_FRAME(37, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_HeartOff[] = {
    ANIMCMD_FRAME(38, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MarkingsMenu_HeartOn[] = {
    ANIMCMD_FRAME(39, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_MarkingsMenu_Marks[] = {
    sAnim_MarkingsMenu_CircleOff,
    sAnim_MarkingsMenu_CircleOn,
    sAnim_MarkingsMenu_SquareOff,
    sAnim_MarkingsMenu_SquareOn,
    sAnim_MarkingsMenu_TriangleOff,
    sAnim_MarkingsMenu_TriangleOn,
    sAnim_MarkingsMenu_HeartOff,
    sAnim_MarkingsMenu_HeartOn,
};

static const struct SpriteTemplate sSpriteTemplate_MarkingsMenu_Marks =
{
    .tileTag = GFXTAG_MARKING_MENU,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_MarkingsMenu_Marks,
    .anims = sAnims_MarkingsMenu_Marks,
};

// Marking menu cursor (16x16, 3 frames)
static const struct OamData sOamData_MarkingsMenu_Cursor =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_MarkingsMenu_Cursor_Bouncing[] = {
    ANIMCMD_FRAME(40, 8, FALSE, FALSE),
    ANIMCMD_FRAME(44, 8, FALSE, FALSE),
    ANIMCMD_FRAME(48, 8, FALSE, FALSE),
    ANIMCMD_FRAME(44, 8, FALSE, FALSE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_MarkingsMenu_Cursor_Main[] = {
    ANIMCMD_FRAME(44, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_MarkingsMenu_Cursor[] = {
    sAnim_MarkingsMenu_Cursor_Bouncing,
    sAnim_MarkingsMenu_Cursor_Main,
};

static const struct SpriteTemplate sSpriteTemplate_MarkingsMenu_Cursor =
{
    .tileTag = GFXTAG_MARKING_MENU,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_MarkingsMenu_Cursor,
    .anims = sAnims_MarkingsMenu_Cursor,
};

// ============================================================================
// Message window sprites
// ============================================================================

static const struct OamData sOamData_MessageWindow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(32x32),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_MessageWindow_0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_1[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_2[] = {
    ANIMCMD_FRAME(0, 0, TRUE, TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MessageWindow[] = {
    sSpriteAnim_MessageWindow_0,
    sSpriteAnim_MessageWindow_1,
    sSpriteAnim_MessageWindow_2,
};

static const u8 sMessageWindowAnims[6] = {0, 1, 1, 1, 1, 2};

static const struct CompressedSpriteSheet sSpriteSheet_MessageWindow =
{
    .data = sMessageWindow_Gfx,
    .size = (32 * 32 * 2) / 2,
    .tag = GFXTAG_MESSAGE_WINDOW,
};

static const struct SpriteTemplate sSpriteTemplate_MessageWindow =
{
    .tileTag = GFXTAG_MESSAGE_WINDOW,
    .paletteTag = PALTAG_MISC_3,
    .oam = &sOamData_MessageWindow,
    .anims = sSpriteAnimTable_MessageWindow,
};

// ============================================================================
// Item Icon Sprites
// ============================================================================

static const struct OamData sOamData_ItemIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_Small[] =
{
    AFFINEANIMCMD_FRAME(192, 192, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_Appear[] =
{
    AFFINEANIMCMD_FRAME(152, 152, 0, 0),
    AFFINEANIMCMD_FRAME(5, 5, 0, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_Disappear[] =
{
    AFFINEANIMCMD_FRAME(192, 192, 0, 0),
    AFFINEANIMCMD_FRAME(-5, -5, 0, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_PickUp[] =
{
    AFFINEANIMCMD_FRAME(192, 192, 0, 0),
    AFFINEANIMCMD_FRAME(6, 6, 0, 12),
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_PutDown[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(-6, -6, 0, 12),
    AFFINEANIMCMD_FRAME(192, 192, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_PutAway[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(-5, -5, 0, 16),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIcon_Large[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ItemIcon[] =
{
    [ITEM_ANIM_NONE]      = sAffineAnim_ItemIcon_Small,
    [ITEM_ANIM_APPEAR]    = sAffineAnim_ItemIcon_Appear,
    [ITEM_ANIM_DISAPPEAR] = sAffineAnim_ItemIcon_Disappear,
    [ITEM_ANIM_PICK_UP]   = sAffineAnim_ItemIcon_PickUp,
    [ITEM_ANIM_PUT_DOWN]  = sAffineAnim_ItemIcon_PutDown,
    [ITEM_ANIM_PUT_AWAY]  = sAffineAnim_ItemIcon_PutAway,
    [ITEM_ANIM_LARGE]     = sAffineAnim_ItemIcon_Large
};

static const struct SpriteTemplate sSpriteTemplate_ItemIcon =
{
    .tileTag = GFXTAG_ITEM_ICON_0,
    .paletteTag = PALTAG_ITEM_ICON_0,
    .oam = &sOamData_ItemIcon,
    .affineAnims = sAffineAnims_ItemIcon,
};
