#if app.ENABLE_MESSENGER_RENEWAL:

import uiScriptLocale

ROOT_PATH						= "d:/ymir work/ui/game/windows/"
PATTERN_PATH					= "d:/ymir work/ui/pattern/"

WINDOW_WIDTH					= 302
WINDOW_HEIGHT					= 424

UPPER_OUTLINE_WIDTH				= 280
UPPER_OUTLINE_HEIGHT			= 76

MEMBER_INFO_WINDOW_WIDTH		= 266
MEMBER_INFO_WINDOW_HEIGHT		= 255

MAIN_WINDOW_POS_X				= 0
MAIN_WINDOW_POS_Y				= 30

LOWER_MAIN_TAB_IMAGE_HEIGHT		= 68

window = {
	"name"		: "community_window",
	"style"		: ("movable", "float",),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		## 메인 bg 1
		{
			"name"		: "main_bg",
			"type"		: "board_with_titlebar",

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.COMMUNITY_MAIN_TITLE,				# 커뮤니티

			"children"	:
			(
				## my info bg
				{
					"name"				: "my_info_bg",
					"type"				: "image",

					"x"					: 11,
					"y"					: UPPER_OUTLINE_HEIGHT / 2 - 3,

					"image"				: ROOT_PATH + "community_info_bar_player.sub",
					"arabic_reverse"	: True, 

					"children"	:
					(
						## my info 드롭 버튼
						{
							"name"					: "my_info_state_drop_list_button",
							"type"					: "button",

							"x"						: 26,
							"y"						: 9,

							"default_image"			: ROOT_PATH + "community_drop_button_default.sub",
							"over_image"			: ROOT_PATH + "community_drop_button_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_drop_button_click.sub",
						},

						## my info 내 길드 text
						{
							"name"					: "my_info_main_player_guild_name",
							"type"					: "text",

							"x"						: 95,
							"y"						: 0,

							"vertical_align"		: "center",
							"horizontal_align"		: "left",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.COMMUNITY_NO_GUILD,			# 길드 없음
						},

						## my info 내 ID text
						{
							"name"					: "my_info_main_player_name",
							"type"					: "text",
							"style"					: ("not_pick",),

							"x"						: 205,
							"y"						: 0,

							"vertical_align"		: "center",
							"horizontal_align"		: "left",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.COMMUNITY_ERROR,				# 알 수 없음
						},

						## my info 내 ID Over 시 선악치 표기용 윈도우 
						{
							"name"					: "my_info_main_player_name_collision",
							"type"					: "window",

							"x"						: 150,
							"y"						: 8,

							"width"					: 110, 
							"height"				: 17, 
						}, 

						## my info 상태 메시지 버튼
						{
							"name"					: "my_info_my_message_button",
							"type"					: "button",

							"x"						: 259,
							"y"						: 8,

							"default_image"			: ROOT_PATH + "community_icon_my_message_default.sub",
							"over_image"			: ROOT_PATH + "community_icon_my_message_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_icon_my_message_click.sub",

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MOUSE_OVER_IN_STATUS_MESSAGE_BUTTON,			# 클릭 시, 상태메시지 입력 가능 
							"outline_tooltip_width"	: 100, 
						},
					),
				},

				## 1. 메신저 탭 누르면 보여줄 윈도우
				{
					"name"		: "messenger_view_window",
					"type"		: "window",

					"x"			: MAIN_WINDOW_POS_X,
					"y"			: MAIN_WINDOW_POS_Y,
					
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,

					"children"	:
					(
						### upper outline window
						{
							"name"		: "messenger_upper_outline_window",
							"type"		: "outline_window",

							"x"			: 11,
							"y"			: 4,

							"width"		: UPPER_OUTLINE_WIDTH,
							"height"	: UPPER_OUTLINE_HEIGHT,
						},

						## [메신저] 탭 메뉴 버튼 (이미지) - 친구 목록
						{
							"name"				: "messenger_friend_member_tab_image",
							"type"				: "image",

							"x"					: 10,
							"y"					: 82,

							"image"				: ROOT_PATH + "community_friend_member.sub", 
							"arabic_reverse"	: True, 
						},

						## [메신저] 탭 메뉴 버튼 (이미지) - 차단 목록
						{
							"name"				: "messenger_block_member_tab_image",
							"type"				: "image",

							"x"					: 10,
							"y"					: 82,

							"image"				: ROOT_PATH + "community_friend_block.sub",
							"arabic_reverse"	: True, 
						},

						## [메신저] 탭 메뉴 버튼 (이미지) - 요청 목록
						{
							"name"				: "messenger_request_member_tab_image",
							"type"				: "image",

							"x"					: 10,
							"y"					: 82,

							"image"				: ROOT_PATH + "community_friend_request.sub",
							"arabic_reverse"	: True, 
						},

						## [메신저] 탭 메뉴 버튼 (라디오 버튼) - 친구 목록
						{
							"name"					: "messenger_friend_member_tab_button",
							"type"					: "radio_button",

							"x"						: 13,
							"y"						: 88,

							"width"					: 46,
							"height"				: 20, 

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_SUB_TAB_FRIEND,	# 친구 목록 
							"outline_tooltip_width"	: 65, 
						},

						## [메신저] 탭 메뉴 버튼 (라디오 버튼) - 차단 목록
						{
							"name"					: "messenger_block_member_tab_button",
							"type"					: "radio_button",

							"x"						: 58,
							"y"						: 88,

							"width"					: 46,
							"height"				: 20, 

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_SUB_TAB_BLOCK,		# 차단 목록 
							"outline_tooltip_width"	: 65, 
						},

						## [메신저] 탭 메뉴 버튼 (라디오 버튼) - 요청 목록
						{
							"name"					: "messenger_request_member_tab_button",
							"type"					: "radio_button",

							"x"						: 103,
							"y"						: 88,

							"width"					: 46,
							"height"				: 20, 

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_SUB_TAB_REQUEST,	# 친구 요청 목록 
							"outline_tooltip_width"	: 65, 
						},

						## [메신저] 탭 메뉴 버튼 요청 목록 깜빡임 효과 
						{
							"name"					: "community_request_button_twinkle", 
							"type"					: "image",
							"style"					: ("not_pick",),

							"x"						: 104,
							"y"						: 89,

							"image"					: ROOT_PATH + "community_request_button_twinkle.sub", 
						},

						## [메신저] 친구 초대 버튼
						{
							"name"					: "friend_invite",
							"type"					: "button",

							"x"						: 160,
							"y"						: 112 - MAIN_WINDOW_POS_Y,
							
							"default_image"			: ROOT_PATH + "community_icon_friend_invite_default.sub",
							"over_image"			: ROOT_PATH + "community_icon_friend_invite_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_icon_friend_invite_click.sub",

							"outline_tooltip_text"	: uiScriptLocale.MESSENGER_ADD_FRIEND,					# 친구 추가
							"outline_tooltip_width"	: 100, 
						},

						## [메신저] 친구 차단 버튼
						{
							"name"					: "friend_block",
							"type"					: "button",

							"x"						: 160 + 30,
							"y"						: 112 - MAIN_WINDOW_POS_Y,
							
							"default_image"			: ROOT_PATH + "community_icon_block_default.sub",
							"over_image"			: ROOT_PATH + "community_icon_block_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_icon_block_click.sub",

							"outline_tooltip_text"	: uiScriptLocale.MESSENGER_BLOCK,						# 차단
							"outline_tooltip_width"	: 100, 
						},

						## [메신저] 귓속말 버튼
						{
							"name"					: "friend_whisper",
							"type"					: "button",

							"x"						: 160 + 60,
							"y"						: 112 - MAIN_WINDOW_POS_Y,
							
							"default_image"			: ROOT_PATH + "community_icon_whisper_default.sub",
							"over_image"			: ROOT_PATH + "community_icon_whisper_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_icon_whisper_click.sub",

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_WHISPER,			# 귓속말
							"outline_tooltip_width"	: 100, 
						},

						## [메신저] 삭제 버튼
						{
							"name"					: "friend_delete",
							"type"					: "button",

							"x"						: 160 + 90,
							"y"						: 112 - MAIN_WINDOW_POS_Y,
							
							"default_image"			: ROOT_PATH + "community_icon_delete_default.sub",
							"over_image"			: ROOT_PATH + "community_icon_delete_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_icon_delete_click.sub",

							"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_FRIEND_DELETE,		# 삭제
							"outline_tooltip_width"	: 100, 
						},
					),
				},

				## 2. 길드 탭 누르면 보여줄 윈도우
				{
					"name"		: "guild_view_window",
					"type"		: "window",

					"x"			: MAIN_WINDOW_POS_X,
					"y"			: MAIN_WINDOW_POS_Y,
					
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,

					"children"	:
					(
						### [길드] 캐릭터가 길드에 속해있을 경우의 윈도우
						## [길드] 중단부터 하단까지의 member info bg
						{
							"name"		: "guild_player_has_guild_window",
							"type"		: "window",

							"x"			: 10,
							"y"			: 45,

							"width"		: UPPER_OUTLINE_WIDTH,
							"height"	: MEMBER_INFO_WINDOW_HEIGHT + LOWER_MAIN_TAB_IMAGE_HEIGHT,

							"children"	:
							(
								## [길드] 탭 메뉴 버튼 (이미지) - 길드원 목록
								{
									"name"					: "guild_member_tab_image",
									"type"					: "image",

									"x"						: 0,
									"y"						: 0,

									"image"					: ROOT_PATH + "community_guild_member.sub",
									"arabic_reverse"		: True, 
								}, 

								## [길드] 탭 메뉴 버튼 (라디오 버튼) - 길드원 목록
								{
									"name"					: "guild_member_tab_button",
									"type"					: "radio_button",

									"x"						: 3,
									"y"						: 5,

									"width"					: 46,
									"height"				: 20, 

									"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_SUB_TAB_GUILD_MEMBER,			# 길드원 
									"outline_tooltip_width"	: 65, 
								},

								## [길드] 친구 초대 버튼
								{
									"name"					: "guild_invite",
									"type"					: "button",

									"x"						: 150,
									"y"						: 0,
									
									"default_image"			: ROOT_PATH + "community_icon_friend_invite_default.sub",
									"over_image"			: ROOT_PATH + "community_icon_friend_invite_mouse_over.sub",
									"down_image"			: ROOT_PATH + "community_icon_friend_invite_click.sub",

									"outline_tooltip_text"	: uiScriptLocale.MESSENGER_ADD_FRIEND,								# 친구 추가
									"outline_tooltip_width"	: 100, 
								},

								## [길드] 친구 차단 버튼
								{
									"name"					: "guild_block",
									"type"					: "button",

									"x"						: 150 + 30,
									"y"						: 0,
									
									"default_image"			: ROOT_PATH + "community_icon_block_default.sub",
									"over_image"			: ROOT_PATH + "community_icon_block_mouse_over.sub",
									"down_image"			: ROOT_PATH + "community_icon_block_click.sub", 

									"outline_tooltip_text"	: uiScriptLocale.MESSENGER_BLOCK,									# 차단
									"outline_tooltip_width"	: 100, 
								},

								## [길드] 귓속말 버튼
								{
									"name"					: "guild_whisper",
									"type"					: "button",

									"x"						: 150 + 60,
									"y"						: 0,
									
									"default_image"			: ROOT_PATH + "community_icon_whisper_default.sub",
									"over_image"			: ROOT_PATH + "community_icon_whisper_mouse_over.sub",
									"down_image"			: ROOT_PATH + "community_icon_whisper_click.sub",

									"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_WHISPER,						# 귓속말
									"outline_tooltip_width"	: 100, 
								},

								## [길드] 삭제 버튼
								{
									"name"					: "guild_delete",
									"type"					: "button",

									"x"						: 150 + 90,
									"y"						: 0,
									
									"default_image"			: ROOT_PATH + "community_icon_delete_default.sub",
									"over_image"			: ROOT_PATH + "community_icon_delete_mouse_over.sub",
									"down_image"			: ROOT_PATH + "community_icon_delete_click.sub",

									"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MESSENGER_FRIEND_DELETE,					# 삭제
									"outline_tooltip_width"	: 100, 
								},

								## [길드] 길드 정보 버튼
								{
									"name"					: "guild_info_button",
									"type"					: "button",

									"x"						: 165,
									"y"						: 295,

									"default_image"			: ROOT_PATH + "community_button_default.sub",
									"over_image"			: ROOT_PATH + "community_button_mouse_over.sub",
									"down_image"			: ROOT_PATH + "community_button_click.sub",

									"text"					: uiScriptLocale.COMMUNITY_GUILD_INFO,			# 길드 정보
								},
							),
						},

						### [길드] 캐릭터가 길드에 속해있지 않을 경우의 윈도우
						{
							"name"		: "guild_player_no_guild_window",
							"type"		: "outline_window",

							"x"			: 10,
							"y"			: 72,

							"width"		: MEMBER_INFO_WINDOW_WIDTH + 15,
							"height"	: MEMBER_INFO_WINDOW_HEIGHT,

							"children"	:
							(
								## [길드] 가입된 길드가 없습니다.
								{
									"name"					: "guild_has_no_guild_text",
									"type"					: "text",

									"x"						: 140,
									"y"						: -35,

									"horizontal_align"		: "left",
									"vertical_align"		: "center",
									"text_vertical_align"	: "center",
									"text_horizontal_align"	: "center",

									"text"					: uiScriptLocale.COMMUNITY_HAS_NO_GUILD,		# 가입된 길드가 없습니다.
								},

								## [길드] 가입 가능한 길드 보러가기
								{
									"name"					: "guild_has_no_guild_text_2",
									"type"					: "text",

									"x"						: 140,
									"y"						: 0,

									"horizontal_align"		: "left",
									"vertical_align"		: "center",
									"text_vertical_align"	: "center",
									"text_horizontal_align"	: "center",

									"text"					: uiScriptLocale.COMMUNITY_HAS_NO_GUILD_2,		# 가입 가능한 길드 보러가기
								},

								## [길드] 가입 가능한 길드 보는 버튼
								{
									"name"					: "guild_view_guild_list_button",
									"type"					: "button",

									"x"						: 95,
									"y"						: 165,

									"default_image"			: "d:/ymir work/ui/game/guild/guildbuttons/infopage/GuildListButton00.sub",
									"over_image"			: "d:/ymir work/ui/game/guild/guildbuttons/infopage/GuildListButton01.sub",
									"down_image"			: "d:/ymir work/ui/game/guild/guildbuttons/infopage/GuildListButton02.sub",
								},
							),
						},
					),
				},

				## 3. 설정 탭을 눌렀을 때, 보이는 윈도우
				{
					"name"		: "config_view_window",
					"type"		: "window",

					"x"			: MAIN_WINDOW_POS_X,
					"y"			: MAIN_WINDOW_POS_Y,
					
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,

					"children"	:
					(
						## [설정] config_info_bg
						{
							"name"		: "config_info_bg",
							"type"		: "outline_window",

							"x"			: 11,
							"y"			: 1,

							"width"		: 266,
							"height"	: 325,
						},

						## [설정] 우측 스크롤 바
						{
							"name"					: "config_scroll_bar",
							"type"					: "scrollbar",

							"x"						: 278,
							"y"						: 10,

							"size"					: 335,
						},

						## [설정] 저장 버튼
						{
							"name"					: "config_setting_save_button",
							"type"					: "button",

							"x"						: 175,
							"y"						: 335,

							"default_image"			: ROOT_PATH + "community_button_default.sub",
							"over_image"			: ROOT_PATH + "community_button_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_button_click.sub",

							"text"					: uiScriptLocale.COMMUNITY_CONFIG_SAVE,			# 저장
						},

						## [설정] 초기화 버튼
						{
							"name"					: "config_setting_init_button",
							"type"					: "button",

							"x"						: 12,
							"y"						: 335,

							"default_image"			: ROOT_PATH + "community_button_default.sub",
							"over_image"			: ROOT_PATH + "community_button_mouse_over.sub",
							"down_image"			: ROOT_PATH + "community_button_click.sub",

							"text"					: uiScriptLocale.COMMUNITY_CONFIG_INIT,			# 초기화
						},
					),
				},

				## 하단 친구 탭 메뉴 (이미지)
				{
					"name"				: "community_messenger_tab_image",
					"type"				: "image",

					"x"					: 0.5,
					"y"					: 396,

					"image"				: ROOT_PATH + "community_lower_tab_friend.sub",
					"arabic_reverse"	: True, 
				},

				## 하단 길드 탭 메뉴 (이미지)
				{
					"name"				: "community_guild_tab_image",
					"type"				: "image",

					"x"					: 0.5,
					"y"					: 396,

					"image"				: ROOT_PATH + "community_lower_tab_guild.sub",
					"arabic_reverse"	: True, 
				},

				## 하단 설정 탭 메뉴 (이미지)
				{
					"name"				: "community_config_tab_image",
					"type"				: "image",

					"x"					: 0.5,
					"y"					: 396,

					"image"				: ROOT_PATH + "community_lower_tab_config.sub",
					"arabic_reverse"	: True, 
				},

				## 하단 친구 탭 메뉴 (라디오 버튼)
				{
					"name"					: "community_messenger_tab_button",
					"type"					: "radio_button",

					"x"						: 10,
					"y"						: 400,

					"width"					: 95,
					"height"				: 90,

					"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MAIN_TAB_FRIEND,				# 친구
					"outline_tooltip_width"	: 65, 
				},

				## 하단 길드 탭 메뉴 (라디오 버튼)
				{
					"name"					: "community_guild_tab_button",
					"type"					: "radio_button",

					"x"						: 105,
					"y"						: 400,

					"width"					: 95,
					"height"				: 90,

					"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MAIN_TAB_GUILD,				# 길드
					"outline_tooltip_width"	: 65, 
				},

				## 하단 설정 탭 메뉴 (라디오 버튼)
				{
					"name"					: "community_config_tab_button",
					"type"					: "radio_button",

					"x"						: 200,
					"y"						: 400,

					"width"					: 95,
					"height"				: 90,

					"outline_tooltip_text"	: uiScriptLocale.COMMUNITY_MAIN_TAB_CONFIG,				# 설정
					"outline_tooltip_width"	: 65, 
				},
			),
		},
	),
}