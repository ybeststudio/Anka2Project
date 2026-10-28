import localeInfo

WINDOW_WIDTH = 495
WINDOW_HEIGHT = 558

IMG_DIR = "d:/ymir work/ui/game/dungeon_track/"

window = {
	"name" : "DungeonTrackWindow",
	"x" : 0, "y" : 0,
	"style" : ("movable", "float",),
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"style" : ("movable","attach", "float",),
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : localeInfo.TRACK_WINDOW_TITLE,

			"children" :
			(
				{
					"name" : "bg", "type" : "image",
					"style" : ("movable","attach", "float",),
					"x" : 10, "y" : 61,
					"image" : IMG_DIR + "bg.tga",
				},
				{
					"name" : "dungeon_info", "type" : "radio_button",
					"x" : 10, "y" : 35,

					"default_image" : IMG_DIR + "button_main.tga",
					"over_image" : IMG_DIR + "button_main.tga",
					"down_image" : IMG_DIR + "button_secondary.tga",

					"children":
					(
						{
							"name" : "dungeon_info_text", "type" : "text",
							"x" : 43, "y" : 7,
							"text" : localeInfo.TRACK_DUNGEON_INFORMATION,
							"text_horizontal_align":"center", "text_range" : 11,
						},
					),
				},
				{
					"name" : "worldboss_info", "type" : "radio_button",
					"x" : 100, "y" : 35,

					"default_image" : IMG_DIR + "button_main.tga",
					"over_image" : IMG_DIR + "button_main.tga",
					"down_image" : IMG_DIR + "button_secondary.tga",

					"children":
					(
						{
							"name" : "worldboss_info_text", "type" : "text",
							"x" : 43, "y" : 7,
							"text" : localeInfo.TRACK_BOSS_INFORMATION,
							"text_horizontal_align":"center", "text_range" : 11,
						},
					),
				},
				{
					"name" : "dungeon_info_notice", "type" : "toggle_button",
					"x" : 225, "y" : 40,

					"default_image" : IMG_DIR + "checkbox.tga",
					"over_image" : IMG_DIR + "checkbox.tga",
					"down_image" : IMG_DIR + "checked.tga",

					"children":
					(
						{
							"name" : "dungeon_info_notice_text", "type" : "text",
							"x" : 22, "y" : -1,
							"text" : localeInfo.TRACK_DUNGEON_INFORMATION_NOTICE,
							"text_horizontal_align":"left", "text_range" : 11,
						},
					),
				},
				{
					"name" : "worldboss_info_notice", "type" : "toggle_button",
					"x" : 355, "y" : 40,

					"default_image" : IMG_DIR + "checkbox.tga",
					"over_image" : IMG_DIR + "checkbox.tga",
					"down_image" : IMG_DIR + "checked.tga",

					"children":
					(
						{
							"name" : "dungeon_info_notice_text", "type" : "text",
							"x" : 22, "y" : -1,
							"text" : localeInfo.TRACK_BOSS_INFORMATION_NOTICE,
							"text_horizontal_align":"left", "text_range" : 11,
						},
					),
				},
				{"name" : "listbox", "type" : "listbox3", "x" : 20, "y" : 75, "width" : 405, "height" : 452,},
			),
		},
	),
}