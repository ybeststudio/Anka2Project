import localeInfo

MAINBOARD_WIDTH = 529
MAINBOARD_HEIGHT = 310

LEFTBOARD_X = 13
LEFTBOARD_Y = 36

IMG_DIR = "d:/ymir work/ui/game/dungeon_info/"

window = {
    "name" : "DungeonInfoWindow",
    "style" : ("movable", "float",),
    "x" : 0,
    "y" : 0,
    "width" : MAINBOARD_WIDTH,
    "height" : MAINBOARD_HEIGHT,
    "children" :
    (
        {
            "name" : "board",
            "type" : "board",
            "style" : ("attach", "ltr"),
            "x" : 0,
            "y" : 0,
            "width" : MAINBOARD_WIDTH,
            "height" : MAINBOARD_HEIGHT,
            "children" :
            (
                {
                    "name" : "title_bar",
                    "type" : "titlebar",
                    "style" : ("attach",),
                    "x" : 6,
                    "y" : 7,
                    "width" : MAINBOARD_WIDTH - 13,
                    "children" :
                    (
                        {
                            "name" : "TitleName",
                            "type" : "text",
                            "x" : 0,
                            "y" : -2,
                            "text": localeInfo.DUNGEONINFO_TITLE,
                            "all_align":"center"
                        },
                    ),
                },

                {
                    "name" : "listbox_image",
                    "type" : "image",
                    "x" : LEFTBOARD_X,
                    "y" : LEFTBOARD_Y,
                    "image":IMG_DIR+"bg_dungeons.tga",
                    "children":
                    (
                        {
                            "name" : "ListBox",
                            "type" : "listboxex",
                            "x" : 4,
                            "y" : 4,
                            "width" : 311-8,
                            "height" : 258-8,
                            "viewcount" : 5,
                        },
                    ),
                },

                {
                    "name" : "scrollbar",
                    "type" : "scrollbar_ex",
                    "x" : LEFTBOARD_X + 311 + 1,
                    "y" : LEFTBOARD_Y,
                    "size" : 258,
                },

                {
                    "name" : "info_board",
                    "type" : "image",
                    "x" : LEFTBOARD_X+311+1+8+1,
                    "y" : LEFTBOARD_Y,
                    "image":IMG_DIR+"bg_info.tga",
                    "children" : 
                    (
                        {"name" : "dungeon_name","type" : "text","x" : 90,"y" : 9,"text_horizontal_align" : "center",},

                        {"name" : "level_range_text","type" : "text","x" : 9,"y" : 35,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_LEVELRANGE,},
                        {"name" : "level_range_value","type" : "text","x" : 165,"y" : 35,"text_horizontal_align" : "right",},

                        {
                            "name" : "group_info_img",
                            "type" : "image",
                            "x" : 3,
                            "y" : 49,
                            "image":IMG_DIR+"line.tga",
                            "children" : 
                            (
                                {"name" : "group_text","type" : "text","x" : 7,"y" : 3,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_GROUP,},
                                {"name" : "group_value","type" : "text","x" : 162,"y" : 3,"text_horizontal_align" : "right",},
                            ),
                        },

                        {"name" : "cooldown_text","type" : "text","x" : 9,"y" : 70,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_COOLDOWN,},
                        {"name" : "cooldown_value","type" : "text","x" : 165,"y" : 70,"text_horizontal_align" : "right",},

                        {"name" : "ticket_text","type" : "text","x" : 9,"y" : 100,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_TICKET,},
                        {"name" : "ticket_img","type" : "image","x" : 165-32,"y" : 100-10,"image":"icon/item/71095.tga"},


                        {"name" : "statistics_title","type" : "text","x" : 90,"y" : 140,"text_horizontal_align" : "center","text":localeInfo.DUNGEONINFO_STATISTICS,},
    
                        {"name" : "complete_text","type" : "text","x" : 9,"y" : 130+35,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_COMPLETE,},
                        {"name" : "complete_value","type" : "text","x" : 165,"y" : 130+35,"text_horizontal_align" : "right",},

                        {
                            "name" : "min_time_img",
                            "type" : "image",
                            "x" : 3,
                            "y" : 130+49,
                            "image":IMG_DIR+"line.tga",
                            "children" : 
                            (
                                {"name" : "min_time_text","type" : "text","x" : 7,"y" : 3,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_MIN_TIME,},
                                {"name" : "min_time_value","type" : "text","x" : 162,"y" : 3,"text_horizontal_align" : "right",},
                            ),
                        },

                        {"name" : "max_dmg_text","type" : "text","x" : 9,"y" : 130+70,"text_horizontal_align" : "left","text":localeInfo.DUNGEONINFO_MAX_DMG,},
                        {"name" : "max_dmg_value","type" : "text","x" : 165,"y" : 130+70,"text_horizontal_align" : "right",},

                        {
                            "name" : "teleport",
                            "type" : "button",
                            "x" : 45,
                            "y" : 225,
                            "text" : localeInfo.DUNGEON_INFO_TELEPORT,
                            "default_image" : IMG_DIR+"button_0.tga",
                            "over_image" : IMG_DIR+"button_1.tga",
                            "down_image" : IMG_DIR+"button_2.tga",
                        },
                    ),
                },
            ),
        },
    ),
}
