#if app.GuildWhiteDragonlairWindow
import uiScriptLocale
import localeInfo

ICON_PATH						= "icon/item/"
PUBLIC_PATH						= "d:/ymir work/ui/public/"
ROOT_PATH						= "d:/ymir work/ui/game/guild_white_dragon_lair/"

WINDOW_WIDTH					= 148
WINDOW_HEIGHT					= 304
OUTLINE_WIDTH					= 228
OUTLINE_HEIGHT					= 260
SLOT_WIDTH						= 45
SLOT_HEIGHT						= 45


window = {
	"name"		: "guild_white_dragon_window",
	"style"		: ("float", "not_pick",),

	"x"			: SCREEN_WIDTH - WINDOW_WIDTH,
	"y"			: 177,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		# 보스 이미지
		{
			"name"		: "boss_img_window",
			"type"		: "window",
			
			"x"			: 0,
			"y"			: 2,
			
			"width"		: 32,
			"height"	: 32,

			"children" :
			(
				#보스 빈 이미지
				{
					"name" : "empty_boss_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: PUBLIC_PATH + "Slot_Base.sub",	
				},

				# 보스 이미지
				{
					"name" : "boss_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: ICON_PATH + "guild_whitedragon_boss.tga",	
				},
			),
		},

		# 보스 hp
		{
			"name"		: "boss_thin_board",
			"type"		: "thinboard",
			
			"x"			: 32,
			"y"			: 0,
			
			"width"		: 110,
			"height"	: 36,

			"children" :
			(
				{
					"name" : "boss_name",
					"type" : "text",

					"x" : 0,
					"y" : -6,

					"horizontal_align"		: "center",
					"text_horizontal_align" : "center",

					"vertical_align"		: "center",
					"text_vertical_align"	: "center",
				},

				{
					"name"					: "boss_hp_gague",
					"type"					: "gauge",

					"x"						: 10,
					"y"						: 21,

					"width"					: 90,
					"color"					: "red",
				},
			),
		},

		# 응시 게이지 UI
		{
			"name"		: "flying_flame_breath_gague_window",
			"type"		: "window",
			
			"x"			: 0,
			"y"			: 38,
			
			"width"		: WINDOW_WIDTH,
			"height"	: 25,

			"children" :
			(
				# 빈 게이지
				{
					"name" : "empty_gauge_0",
					"type" : "image",

					"x" : 10,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_empty.sub",
				},
				{
					"name" : "empty_gauge_1",
					"type" : "image",

					"x" : 35,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_empty.sub",
				},
				{
					"name" : "empty_gauge_2",
					"type" : "image",

					"x" : 60,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_empty.sub",
				},
				{
					"name" : "empty_gauge_3",
					"type" : "image",

					"x" : 85,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_empty.sub",
				},
				{
					"name" : "empty_gauge_4",
					"type" : "image",

					"x" : 110,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_empty.sub",
				},

				# 게이지
				{
					"name" : "gauge_0",
					"type" : "expanded_image",

					"x" : 10,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_full.sub",
				},
				{
					"name" : "gauge_1",
					"type" : "expanded_image",

					"x" : 35,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_full.sub",
				},
				{
					"name" : "gauge_2",
					"type" : "expanded_image",

					"x" : 60,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_full.sub",
				},
				{
					"name" : "gauge_3",
					"type" : "expanded_image",

					"x" : 85,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_full.sub",
				},
				{
					"name" : "gauge_4",
					"type" : "expanded_image",

					"x" : 110,
					"y" : 0,

					"image" : ROOT_PATH + "gauge_full.sub",
				},

				# 반짝거리는 게이지
				{
					"name" : "shine_gauge_0",
					"type" : "ani_image",
					
					"x" : 13,
					"y" : 2,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "gauge_shine_01.sub",
						ROOT_PATH + "gauge_shine_02.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_04.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_02.sub",
					),
				},
				{
					"name" : "shine_gauge_1",
					"type" : "ani_image",
					
					"x" : 38,
					"y" : 2,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "gauge_shine_01.sub",
						ROOT_PATH + "gauge_shine_02.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_04.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_02.sub",
					),
				},
				{
					"name" : "shine_gauge_2",
					"type" : "ani_image",
					
					"x" : 63,
					"y" : 2,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "gauge_shine_01.sub",
						ROOT_PATH + "gauge_shine_02.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_04.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_02.sub",
					),
				},
				{
					"name" : "shine_gauge_3",
					"type" : "ani_image",
					
					"x" : 88,
					"y" : 2,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "gauge_shine_01.sub",
						ROOT_PATH + "gauge_shine_02.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_04.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_02.sub",
					),
				},
				{
					"name" : "shine_gauge_4",
					"type" : "ani_image",
					
					"x" : 113,
					"y" : 2,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "gauge_shine_01.sub",
						ROOT_PATH + "gauge_shine_02.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_04.sub",
						ROOT_PATH + "gauge_shine_03.sub",
						ROOT_PATH + "gauge_shine_02.sub",
					),
				},
			),
		},

		# 금이간 진백룡알 빈 이미지
		{
			"name"		: "broken_egg_img_window",
			"type"		: "window",
			
			"x"			: 0,
			"y"			: 66,
			
			"width"		: 32,
			"height"	: 32,

			"children" :
			(
				#보스 빈 이미지
				{
					"name" : "empty_broken_egg_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: PUBLIC_PATH + "Slot_Base.sub",	
				},

				# 보스 이미지
				{
					"name" : "broken_egg_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: ICON_PATH + "guild_whitedragon_egg.tga",	
				},
			),
		},

		# 금이간 진백룡알 board
		{
			"name"		: "broken_egg_thin_board",
			"type"		: "thinboard",
			
			"x"			: 32,
			"y"			: 64,
			
			"width"		: 110,
			"height"	: 36,

			"children" :
			(
				{
					"name" : "remain_broken_egg_text_01",
					"type" : "text",

					"x" : 0,
					"y" : -7,

					"horizontal_align"		: "center",
					"text_horizontal_align" : "center",

					"vertical_align"		: "center",
					"text_vertical_align"	: "center",
				},
				{
					"name" : "remain_broken_egg_text_02",
					"type" : "text",

					"x" : 0,
					"y" : 7,

					"horizontal_align"		: "center",
					"text_horizontal_align" : "center",

					"vertical_align"		: "center",
					"text_vertical_align"	: "center",
				},
			),
		},

		# 신기전 이미지
		{
			"name" : "singijeon_slot_img",
			"type" : "image",
		
			"x" : 0,
			"y" : 106,
					
			"image"	: PUBLIC_PATH + "Slot_Base.sub",	

			"children" :
			(
				{
					"name"		: "singijeon_slot",
					"type"		: "slot",
					
					"x"			: 0,
					"y"			: 0,
					
					"width"		: 32,
					"height"	: 32,

					"slot" : 
					(
						{"index":0, "x":0, "y":0, "width":32, "height":32},
					),
				},
			),
		},

		# 신기전 hp
		{
			"name"		: "singijeon_thin_board",
			"type"		: "thinboard",
			
			"x"			: 32,
			"y"			: 104,
			
			"width"		: 110,
			"height"	: 36,

			"children" :
			(
				{
					"name" : "singijeon_name",
					"type" : "text",

					"x" : 0,
					"y" : -6,

					"horizontal_align"		: "center",
					"text_horizontal_align" : "center",

					"vertical_align"		: "center",
					"text_vertical_align"	: "center",
				},

				{
					"name"					: "singijeon_hp_gague",
					"type"					: "gauge",

					"x"						: 10,
					"y"						: 21,

					"width"					: 90,
					"color"					: "red",
				},
			),
		},
	),
}