import uiScriptLocale

BG_WIDTH						= 498
BG_HEIGHT						= 380
PATTERN_PATH					= "d:/ymir work/ui/pattern/"
ACHIEV_ROOT_PATH				= "d:/ymir work/ui/game/monster_card/achiev/"
ACHIEV_BUTTON_PATH				= "d:/ymir work/ui/game/monster_card/achiev/button/"
ACHIEV_LEFT_PATTERN_X_COUNT		= 27
ACHIEV_LEFT_PATTERN_Y_COUNT		= 18
ACHIEV_PATTERN_X_COUNT			= 27
ACHIEV_PATTERN_Y_COUNT			= 18

window = {
	"name"	: "MonsterCardAchievDetailWindow", "style" : ("float",), "x" : 0, "y" : 0, "width" : BG_WIDTH, "height"	: BG_HEIGHT, 
	"children" :
	(
		## BOARD
		{ "name" : "board", "type" : "board_with_titlebar", "x" : 0, "y" : 0, "width" : BG_WIDTH, "height" : BG_HEIGHT,  "title" : "", },
		## 백그라운드
		{ "name" : "achiev_detail_LeftTop", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 32, "image" : PATTERN_PATH + "border_A_left_top.tga", },
		{ "name" : "achiev_detail_RightTop", "type" : "image", "style" : ("ltr",), "x" : 470, "y" : 32, "image" : PATTERN_PATH + "border_A_right_top.tga", },
		{ "name" : "achiev_detail_LeftBottom", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 350, "image" : PATTERN_PATH + "border_A_left_bottom.tga", },
		{ "name" : "achiev_detail_RightBottom", "type" : "image", "style" : ("ltr",), "x" : 470, "y" : 350, "image" : PATTERN_PATH + "border_A_right_bottom.tga", },
		{ "name" : "achiev_detail_TopCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 32, "image" : PATTERN_PATH + "border_A_top.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
		{ "name" : "achiev_detail_LeftCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 10, "y" : 48, "image" : PATTERN_PATH + "border_A_left.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
		{ "name" : "achiev_detail_RightCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 470, "y" : 48, "image" : PATTERN_PATH + "border_A_right.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
		{ "name" : "achiev_detail_BottomCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 350, "image" : PATTERN_PATH + "border_A_bottom.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
		{ "name" : "achiev_detail_CenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 48, "image" : PATTERN_PATH + "border_A_center.tga", "rect" : (0.0, 0.0, ACHIEV_PATTERN_X_COUNT, ACHIEV_PATTERN_Y_COUNT), },
		## 업적 바
		{ 
			"name" : "achiev_detail_bar", "type" : "image", "x" : 16, "y" : 39, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_list_bar_img.sub",
			"children" :
			(
				{ "name" : "achiev_detail_minus",	"type" : "button",	"x" : 7,	"y" : 7, "default_image" : ACHIEV_BUTTON_PATH + "achiev_minus_down_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_minus_down_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_minus_down_button.sub", },
				{ "name" : "achiev_detail_text",	"type" : "text",	"x" : 39,	"y" : 10, "text" : "", },
				{ "name" : "achiev_detail_reward",	"type" : "image",	"x" : 367,	"y" : 4, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_list_bar_reward_img.sub", },
				{ "name" : "achiev_detail_applying","type" : "image",	"x" : 408,	"y" : 4, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_list_bar_applying_img.sub", },
				{ "name" : "achiev_detail_disable",	"type" : "image",	"x" : 408,	"y" : 4, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_list_bar_disable_img.sub", },
				{ "name" : "achiev_detail_apply",	"type" : "button",	"x" : 408,	"y" : 4, "default_image" : ACHIEV_BUTTON_PATH + "achiev_apply_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_apply_over_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_apply_down_button.sub", },
				{ "name" : "achiev_detail_regist",	"type" : "button",	"x" : 326,	"y" : 4, "default_image" : ACHIEV_BUTTON_PATH + "achiev_regist_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_regist_over_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_regist_down_button.sub", },
			),
		},
		## 업적 등록 별 버튼
		{ "name" : "achiev_star_button_00", "type" : "radio_button", "x" : 29, "y" : 85, "default_image" : ACHIEV_BUTTON_PATH + "achiev_star1_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_star1_over_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_star1_down_button.sub", },
		{ "name" : "achiev_star_button_01", "type" : "radio_button", "x" : 29 + 87, "y" : 85, "default_image" : ACHIEV_BUTTON_PATH + "achiev_star2_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_star2_over_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_star2_down_button.sub", },
		{ "name" : "achiev_star_button_02", "type" : "radio_button", "x" : 29 + 87 + 87, "y" : 85, "default_image" : ACHIEV_BUTTON_PATH + "achiev_star3_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_star3_over_button.sub", "down_image" : ACHIEV_BUTTON_PATH + "achiev_star3_down_button.sub", },
		{ "name" : "achiev_star_button_03", "type" : "radio_button", "x" : 29 + 87 + 87 + 87, "y" : 85, "default_image" : ACHIEV_BUTTON_PATH + "achiev_star4_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_star4_over_button.sub", "down_image" 	: ACHIEV_BUTTON_PATH + "achiev_star4_down_button.sub", },
		{ "name" : "achiev_star_button_04", "type" : "radio_button", "x" : 29 + 87 + 87 + 87 + 87, "y" : 85, "default_image" : ACHIEV_BUTTON_PATH + "achiev_star5_default_button.sub", "over_image" : ACHIEV_BUTTON_PATH + "achiev_star5_over_button.sub", "down_image"	: ACHIEV_BUTTON_PATH + "achiev_star5_down_button.sub", },

		## 업적 적용중인 별 이미지
		{ "name" : "achiev_applying_star_img_00", "type" : "image", "x" : 29,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star1_img.sub", },
		{ "name" : "achiev_applying_star_img_01", "type" : "image", "x" : 116,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star2_img.sub", },
		{ "name" : "achiev_applying_star_img_02", "type" : "image", "x" : 203,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star3_img.sub", },
		{ "name" : "achiev_applying_star_img_03", "type" : "image", "x" : 290,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star4_img.sub", },
		{ "name" : "achiev_applying_star_img_04", "type" : "image", "x" : 377,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star5_img.sub", },

		## 업적 등록된 별 이미지.
		{ "name" : "achiev_regist_star_img_00", "type" : "image", "x" : 29,		"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_regist_star1_img.sub", },
		{ "name" : "achiev_regist_star_img_01", "type" : "image", "x" : 116,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_regist_star2_img.sub", },
		{ "name" : "achiev_regist_star_img_02", "type" : "image", "x" : 203,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_regist_star3_img.sub", },
		{ "name" : "achiev_regist_star_img_03", "type" : "image", "x" : 290,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_regist_star4_img.sub", },
		{ "name" : "achiev_regist_star_img_04", "type" : "image", "x" : 377,	"y" : 85, "image" : "d:/ymir work/ui/game/monster_card/achiev/achiev_regist_star5_img.sub", },

		## 업적 달성에 필요한 카드 목록
		{ "name" : "achiev_detail_monster_card_00",			"type" : "image", "x" : 31,		"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_00",	"type" : "image", "x" : 31,		"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_00",	"type" : "image", "x" : 31,		"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_01",			"type" : "image", "x" : 140,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_01",	"type" : "image", "x" : 140,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_01",	"type" : "image", "x" : 140,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_02",			"type" : "image", "x" : 249,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_02",	"type" : "image", "x" : 249,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_02",	"type" : "image", "x" : 249,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_03",			"type" : "image", "x" : 358,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_03",	"type" : "image", "x" : 358,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_03",	"type" : "image", "x" : 358,	"y" : 120, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_04",			"type" : "image", "x" : 31,		"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_04",	"type" : "image", "x" : 31,		"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_04",	"type" : "image", "x" : 31,		"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_05",			"type" : "image", "x" : 140,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_05",	"type" : "image", "x" : 140,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_05",	"type" : "image", "x" : 140,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_06",			"type" : "image", "x" : 249,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_06",	"type" : "image", "x" : 249,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_06",	"type" : "image", "x" : 249,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

		{ "name" : "achiev_detail_monster_card_07",			"type" : "image", "x" : 358,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/empty_card.sub", },
		{ "name" : "achiev_detail_monster_card_clear_07",	"type" : "image", "x" : 358,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_alpha.sub", },
		{ "name" : "achiev_detail_monster_card_frame_07",	"type" : "image", "x" : 358,	"y" : 240, "image" : "d:/ymir work/ui/game/monster_card/card_view_line.sub", },

	),
}