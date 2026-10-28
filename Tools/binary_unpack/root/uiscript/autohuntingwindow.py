import uiScriptLocale
import app

# 오토 UI
AUTO_HUNTING_BOARD_WIDTH			= 254
AUTO_HUNTING_BOARD_HIGHT			= 490

# 액티브 스킬 셋팅
SKILL_SETTING_BAR_X					= 14
SKILL_SETTING_BAR_Y					= 34
SKILL_SETTING_BAR_WIDTH				= 223

# 초기화 버튼
CLEAR_BUTTON_X						= 207
CLEAR_BUTTON_Y						= 1

SKILL_SETTING_TEXT_X				= 9
SKILL_SETTING_TEXT_Y				= 2

# 스킬 슬롯 위치
SKILL_SLOT_X						= 15
SKILL_SLOT_Y						= 57

SLOT_WIDTH							= 221
SLOT_HEIGHT							= 90

## 포션 슬롯 위치
POTION_SLOT_X						= 15
POTION_SLOT_Y						= 192

# 슬롯 인덱스 간격
SLOT_INDEX_INTERVAL_X				= 38
SLOT_INDEX_INTERVAL_Y				= 54
SLOT_INTERVAL_Y						= 29

# 슬롯 크기
SLOT_INDEX_WIDTH					= 32
SLOT_INDEX_HEIGHT					= 32

# 물약 셋팅 바
POTION_SETTING_BAR_X				= 14
POTION_SETTING_BAR_Y				= 171
POTION_SETTING_BAR_WIDTH			= 223

# 물약 셋팅 텍스트
POTION_SETTING_TEXT_X				= 9
POTION_SETTING_TEXT_Y				= 2

# 설정 바
SETTING_BAR_X						= 14
SETTING_BAR_Y						= 308
SETTING_BAR_WIDTH					= 223

# 설정 텍스트
SETTING_AUTO_SETTING_TEXT_X			= 9
SETTING_AUTO_SETTING_TEXT_Y			= 2

SETTING_AUTO_BAR_X					= 3

SETTING_AUTO_ATTACK_BAR_Y			= 20
SETTING_AUTO_RANGE_BAR_Y			= 47
SETTING_AUTO_POTION_BAR_Y			= 74
SETTING_AUTO_SKILL_BAR_Y			= 101
SETTING_AUTO_RESTART_BAR_Y			= 128

# 버튼 (자동 공격, 자동 스킬, 자동 물약, 사냥 범위, 자동 부활)
AUTO_BUTTON_X						= 194
AUTO_BUTTON_BASE_Y					= 327
AUTO_BUTTON_ADD_Y					= 27

# 저장 버튼
AUTO_SAVE_BUTTON_X					= 10
AUTO_SAVE_BUTTON_Y					= 459

# 시작 버튼
AUTO_START_BUTTON_X					= 88
AUTO_START_BUTTON_Y					= 459

# 종료 버튼
AUTO_END_BUTTON_X					= 166
AUTO_END_BUTTON_Y					= 459

# 툴팁 버튼
AUTO_TOOLTIP_BUTTON_X				= 155
AUTO_TOOLTIP_BUTTON_Y				= 8

## 쿨타임 EditLine
SKILL_COOL_TIME_EDITLINE_BASE_X		= 15
SKILL_COOL_TIME_EDITLINE_BASE_Y		= 89
SKILL_COOL_TIME_INTERVAL_Y			= 54
SKILL_COOL_TIME_EDITLINE_WIDTH		= 221
SKILL_COOL_TIME_EDITLINE_HEIGHT		= 20

POTION_COOL_TIME_EDITLINE_BASE_X	= 15
POTION_COOL_TIME_EDITLINE_BASE_Y	= 226
POTION_COOL_TIME_INTERVAL_Y			= 54
POTION_COOL_TIME_EDITLINE_WIDTH		= 221
POTION_COOL_TIME_EDITLINE_HEIGHT	= 20

window = {
	"name" : "AutoHuntingWindow",
	
	"x" : 0,
	"y" : 0,
	
	"style" : ("movable", "float",),
	
	"width"		: AUTO_HUNTING_BOARD_WIDTH,
	"height"	: AUTO_HUNTING_BOARD_HIGHT,
	
	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			
			"x" : 0,
			"y" : 0,
			
			"width"		: AUTO_HUNTING_BOARD_WIDTH,
			"height"	: AUTO_HUNTING_BOARD_HIGHT,
			
			"title" : uiScriptLocale.AUTO_TITLE,
			
			"children" :
			[
				## 액티브 스킬셋팅.Bar
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					
					"x" : SKILL_SETTING_BAR_X,
					"y" : SKILL_SETTING_BAR_Y,
					
					"width" : SKILL_SETTING_BAR_WIDTH,
					
					"children":
					(
						## 액티브 스킬셋팅 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : SKILL_SETTING_TEXT_X, "y" : SKILL_SETTING_TEXT_Y, "text" : uiScriptLocale.AUTO_SKILL_SETTING,
						},
						
						## __skill_setting_clear_button
						{
							"name" : "SkillSettingClearButton",
							"type" : "button",
							
							"x" : CLEAR_BUTTON_X,
							"y" : CLEAR_BUTTON_Y,
							
							"default_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
							"over_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
							"down_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
						},
					),
				},
				
				## 액티브 스킬 셋팅 Slot
				{
					"name" : "AutoHuntingSkillSlot",
					"type" : "slot",
					
					"x" : SKILL_SLOT_X,
					"y" : SKILL_SLOT_Y,
					
					"width"		: SLOT_WIDTH,
					"height"	: SLOT_HEIGHT,
					"image"		: "d:/ymir work/ui/public/Slot_Base.sub",
					
					"slot" :	(
									## Active SKill
									{ "index" : 0,	"x" : SLOT_INDEX_INTERVAL_X * 0,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 1,	"x" : SLOT_INDEX_INTERVAL_X * 1,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 2,	"x" : SLOT_INDEX_INTERVAL_X * 2,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 3,	"x" : SLOT_INDEX_INTERVAL_X * 3,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 4,	"x" : SLOT_INDEX_INTERVAL_X * 4,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 5,	"x" : SLOT_INDEX_INTERVAL_X * 5,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 6,	"x" : SLOT_INDEX_INTERVAL_X * 0,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 7,	"x" : SLOT_INDEX_INTERVAL_X * 1,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 8,	"x" : SLOT_INDEX_INTERVAL_X * 2,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 9,	"x" : SLOT_INDEX_INTERVAL_X * 3,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 10,	"x" : SLOT_INDEX_INTERVAL_X * 4,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 11,	"x" : SLOT_INDEX_INTERVAL_X * 5,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },	
								),
				
				},				
				
				## 스킬 쿨타임 editline 1번째줄
				{
					"name"		: "SkillCoolTimeEditLineWindow_1",
					"type"		: "window",
					"x"			: SKILL_COOL_TIME_EDITLINE_BASE_X,
					"y"			: SKILL_COOL_TIME_EDITLINE_BASE_Y,

					"width"		: SKILL_COOL_TIME_EDITLINE_WIDTH,
					"height"	: SKILL_COOL_TIME_EDITLINE_HEIGHT,
				},
				
				
				## 스킬 쿨타임 editline 2번째줄
				{
					"name"		: "SkillCoolTimeEditLineWindow_2",
					"type"		: "window",
					"x"			: SKILL_COOL_TIME_EDITLINE_BASE_X,
					"y"			: SKILL_COOL_TIME_EDITLINE_BASE_Y + SKILL_COOL_TIME_INTERVAL_Y,

					"width"		: SKILL_COOL_TIME_EDITLINE_WIDTH,
					"height"	: SKILL_COOL_TIME_EDITLINE_HEIGHT,
				},

				
				## 물약 셋팅.Bar
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					
					"x" : POTION_SETTING_BAR_X,
					"y" : POTION_SETTING_BAR_Y,
					
					"width" : POTION_SETTING_BAR_WIDTH,
					
					"children":
					(
						## 물약 셋팅 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : POTION_SETTING_TEXT_X, "y" : POTION_SETTING_TEXT_Y, "text" : uiScriptLocale.AUTO_POTION_SETTING,
						},
									
						## __auto_potion_clear_button
						{
							"name" : "PotionSettingClearButton",
							"type" : "button",
							
							"x" : CLEAR_BUTTON_X,
							"y" : CLEAR_BUTTON_Y,
							
							"default_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
							"over_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
							"down_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
						},
					),
				},				
								
				## 물약 셋팅 Slot
				{
					"name" : "AutoHuntingPotionSlot",
					"type" : "slot",
					
					"x" : POTION_SLOT_X,
					"y" : POTION_SLOT_Y,
					
					"width"		: SLOT_WIDTH,
					"height"	: SLOT_HEIGHT,
					"image"		: "d:/ymir work/ui/public/Slot_Base.sub",
					
					"slot" :	(
								
									## Potion
									{ "index" : 0,	"x" : SLOT_INDEX_INTERVAL_X * 0,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 1,	"x" : SLOT_INDEX_INTERVAL_X * 1,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 2,	"x" : SLOT_INDEX_INTERVAL_X * 2,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 3,	"x" : SLOT_INDEX_INTERVAL_X * 3,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 4,	"x" : SLOT_INDEX_INTERVAL_X * 4,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 5,	"x" : SLOT_INDEX_INTERVAL_X * 5,	"y": 0,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 6,	"x" : SLOT_INDEX_INTERVAL_X * 0,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 7,	"x" : SLOT_INDEX_INTERVAL_X * 1,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 8,	"x" : SLOT_INDEX_INTERVAL_X * 2,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 9,	"x" : SLOT_INDEX_INTERVAL_X * 3,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 10,	"x" : SLOT_INDEX_INTERVAL_X * 4,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
									{ "index" : 11,	"x" : SLOT_INDEX_INTERVAL_X * 5,	"y": SLOT_INDEX_INTERVAL_Y,	"width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },	
								),
				
				},			
				
				## 물약 쿨타임 editline 1번째줄
				{
					"name"		: "PotionCoolTimeEditLineWindow_1",
					"type"		: "window",
					"x"			: POTION_COOL_TIME_EDITLINE_BASE_X,
					"y"			: POTION_COOL_TIME_EDITLINE_BASE_Y,

					"width"		: POTION_COOL_TIME_EDITLINE_WIDTH,
					"height"	: POTION_COOL_TIME_EDITLINE_HEIGHT,
				},
				
				
				## 물약 쿨타임 editline 2번째줄
				{
					"name"		: "PotionCoolTimeEditLineWindow_2",
					"type"		: "window",
					"x"			: POTION_COOL_TIME_EDITLINE_BASE_X,
					"y"			: POTION_COOL_TIME_EDITLINE_BASE_Y + POTION_COOL_TIME_INTERVAL_Y,

					"width"		: POTION_COOL_TIME_EDITLINE_WIDTH,
					"height"	: POTION_COOL_TIME_EDITLINE_HEIGHT,
				},
				
				## 설정 Bar
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					
					"x" : SETTING_BAR_X,
					"y" : SETTING_BAR_Y,
					
					"width" : SETTING_BAR_WIDTH,
				
					"children":
					(
						## 셋팅 Text
						{ "name" : "BaseInfo", "type" : "text", "x" : SETTING_AUTO_SETTING_TEXT_X,	"y" : SETTING_AUTO_SETTING_TEXT_Y,	"text" : uiScriptLocale.AUTO_SETTING, },
						
						# 자동 공격
						{
							"name"	: "attack_text_bar", "type" : "image", "x" : SETTING_AUTO_BAR_X, "y" : SETTING_AUTO_ATTACK_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_01.sub",
							"children":
							(
								{ "name" : "BaseInfo", "type" : "text", "x" : 2, "y" : 0, "all_align" : "center", "text" : uiScriptLocale.AUTO_AUTO_ATTACK, },
							),
						},
						# 사냥 범위
						{
							"name"	: "range_text_bar", "type" : "image", "x" : SETTING_AUTO_BAR_X, "y" : SETTING_AUTO_RANGE_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_01.sub",
							"children":
							(
								{ "name" : "BaseInfo", "type" : "text", "x" : 2, "y" : 0, "all_align" : "center", "text" : uiScriptLocale.AUTO_AUTO_RANGE, },
							),
						},
						# 자동 물약
						{
							"name"	: "potion_text_bar", "type" : "image", "x" : SETTING_AUTO_BAR_X, "y" : SETTING_AUTO_POTION_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{ "name" : "BaseInfo", "type" : "text", "x" : 2, "y" : 0, "all_align" : "center", "text" : uiScriptLocale.AUTO_AUTO_POTION, },
							), 
						},
						# 자동 스킬
						{
							"name"	: "skill_text_bar", "type" : "image", "x" : SETTING_AUTO_BAR_X, "y" : SETTING_AUTO_SKILL_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{ "name" : "BaseInfo", "type" : "text", "x" : 2, "y" : 0, "all_align" : "center", "text" : uiScriptLocale.AUTO_AUTO_SKILL, },
							),
						},
						# 자동 부활
						{
							"name"	: "restart_text_bar", "type" : "image", "x" : SETTING_AUTO_BAR_X, "y" : SETTING_AUTO_RESTART_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{ "name" : "BaseInfo", "type" : "text", "x" : 2, "y" : 0, "all_align" : "center", "text" : uiScriptLocale.AUTO_AUTO_RESTART, },
							),
						},
					),
				},
				
				## AutoAttack Button
				{
					"name" : "AutoAttackButton",
					"type" : "toggle_button",
					
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y,
					
					"text" : uiScriptLocale.AUTO_OFF,
					
					"default_image"	: "d:/ymir work/ui/public/Small_Button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/Small_Button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/Small_Button_03.sub",
				},

				## 자동 공격 옵션 버튼
				{
					"name" : "AutoAttackOptionButton",
					"type" : "button",
					
					"x" : AUTO_BUTTON_X + 42,
					"y" : AUTO_BUTTON_BASE_Y,
					
					"default_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_03.sub",
				},
				
				## AutoRangeButton
				{
					"name" : "AutoRangeButton",
					"type" : "toggle_button",
					
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + AUTO_BUTTON_ADD_Y,
					
					"text" : uiScriptLocale.AUTO_OFF,
					
					"default_image"	: "d:/ymir work/ui/public/Small_Button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/Small_Button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/Small_Button_03.sub",
				},

				## 사냥 범위 선택 화살표 버튼
				{
					"name" : "AutoRangeSelectButton",
					"type" : "button",
					
					"x" : AUTO_BUTTON_X + 42,
					"y" : AUTO_BUTTON_BASE_Y + AUTO_BUTTON_ADD_Y,
					
					"default_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/auto_hunting/option_window_open_button_03.sub",
				},
				
				## AutoPotion Button
				{
					"name" : "AutoPotionButton",
					"type" : "toggle_button",
					
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 2),
					
					"text" : uiScriptLocale.AUTO_OFF,
					
					"default_image"	: "d:/ymir work/ui/public/Small_Button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/Small_Button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/Small_Button_03.sub",
				},
				
				## AutoSkill Button
				{
					"name" : "AutoSkillButton",
					"type" : "toggle_button",
					
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 3),
					
					"text" : uiScriptLocale.AUTO_OFF,
					
					"default_image"	: "d:/ymir work/ui/public/Small_Button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/Small_Button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/Small_Button_03.sub",
				},
				
				## AutoRestartHere Button
				{
					"name" : "AutoRestartHereButton",
					"type" : "toggle_button",
					
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 4),
					
					"text" : uiScriptLocale.AUTO_OFF,
					
					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/Small_Button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/Small_Button_03.sub",
				},
				
				## Save Button
				{
					"name" : "SaveButton",
					"type" : "button",
					
					"x" : AUTO_SAVE_BUTTON_X,
					"y" : AUTO_SAVE_BUTTON_Y,
					
					"text" : uiScriptLocale.AUTO_SAVE,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				
				## Start Button
				{
					"name" : "StartButton",
					"type" : "button",
					
					"x" : AUTO_START_BUTTON_X,
					"y" : AUTO_START_BUTTON_Y,
					
					"text" : uiScriptLocale.AUTO_START,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				
				## End Button
				{
					"name" : "EndButton",
					"type" : "button",
					
					"x" : AUTO_END_BUTTON_X,
					"y" : AUTO_END_BUTTON_Y,
					
					"text" : uiScriptLocale.AUTO_END,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				
				## __help_button
				{
					"name" : "HelpButton",
					"type" : "button",
					
					"x" : AUTO_TOOLTIP_BUTTON_X,
					"y" : AUTO_TOOLTIP_BUTTON_Y,
					
					"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
				
				## __setting_all_clear_button
				{
					"name" : "SettingAllClearButton",
					"type" : "button",
					
					"x" : SETTING_BAR_X + CLEAR_BUTTON_X,
					"y" : SETTING_BAR_Y + CLEAR_BUTTON_Y,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
				},
			],
		},
	],
}
