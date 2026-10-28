import uiScriptLocale

WINDOW_WIDTH = 543
WINDOW_HEIGHT = 450

PATH = 'd:/ymir work/ui/game/teleport/renewal/'

window = {
	'name': 'TeleportWindow', 'style': ('float', 'movable', 'window_without_alpha'), 'x': SCREEN_WIDTH/2 - WINDOW_WIDTH/2, 'y': SCREEN_HEIGHT/2 - WINDOW_HEIGHT/2, 'width': WINDOW_WIDTH, 'height': WINDOW_HEIGHT,
	'children':
	(
		{'name': 'board', 'type': 'board_with_titlebar', 'style': ('attach',), 'x': 0, 'y': 0, 'width': WINDOW_WIDTH, 'height': WINDOW_HEIGHT - 73 + 24, 'title': uiScriptLocale.TELEPORT_WINDOW_TITLE,
			'children':
			[
				{'name': 'nav_board', 'type': 'thinboard_special_1', 'x': 10, 'y': 32, 'width': WINDOW_WIDTH - 20, 'height': WINDOW_HEIGHT - 93,
					'children':
					[
						{'name': 'result_title_bar', 'type': 'specialbar1', 'x': 3, 'y': 3, 'width': WINDOW_WIDTH - 20 - 6,
							'children':
							[
								{'name': 'result_title_bar_value', 'type': 'text', 'x': 0, 'y': 1, 'horizontal_align': 'center', 'text_horizontal_align': 'center', 'text': uiScriptLocale.TELEPORT_WINDOW_NAVIGATION_TITLE, 'color': 0xFF5eb43c},
							],
						},
					],
				},
				{'name': 'sub_nav_board', 'type': 'thinboard_special_1', 'x': 10, 'y': 32, 'width': WINDOW_WIDTH - 20, 'height': WINDOW_HEIGHT - 93,
					'children':
					[
						{'name': 'result_title_bar', 'type': 'specialbar1', 'x': 3, 'y': 3, 'width': WINDOW_WIDTH - 20 - 6,
							'children':
							[
								{'name': 'sub_nav_board_title', 'type': 'text', 'x': 0, 'y': 1, 'horizontal_align': 'center', 'text_horizontal_align': 'center', 'text': uiScriptLocale.TELEPORT_WINDOW_NAVIGATION_TITLE, 'color': 0xFF5eb43c},
							],
						},
					],
				},
			],
		},
		{'name': 'backButton', 'type': 'button', 'x': 28, 'y': 7, 'horizontal_align': 'right', 'tooltip_text' : uiScriptLocale.LOGIN_BACK, 'default_image': PATH + 'back_btn_01.png', 'over_image': PATH + 'back_btn_02.png', 'down_image': PATH + 'back_btn_03.png'},
		{'name': 'info_board', 'type': 'thinboard_special_1', 'x': 150, 'y': 135, 'width': WINDOW_WIDTH - 307, 'height': 132, 'vertical_align': 'bottom',
			'children':
			[
				{'name': 'info_board_bar', 'type': 'specialbar1', 'x': 3, 'y': 3, 'width': WINDOW_WIDTH - 307 - 6,
					'children':
					[
						{'name': 'mapName', 'type': 'text', 'x': 0, 'y': 1, 'horizontal_align': 'center', 'text_horizontal_align': 'center', 'text': '--', 'color': 0xFF5eb43c},
					],
				},
				{'name': 'InformationBase', 'type': 'image', 'x': 3, 'y': 0, 'image': PATH + 'info_base.png',
					'children':
					[
						{'name': 'lvReqIcon', 'type': 'image', 'x': 3, 'y': 28, 'image': PATH + 'lv_icon.png'},
						{'name': 'moneyReqIcon', 'type': 'image', 'x': 5, 'y': 54, 'image': PATH + 'money_icon.png'},

						{'name': 'lvReqValue', 'type': 'text', 'x': 50, 'y': 29, 'text': '--'},
						{'name': 'moneyReqValue', 'type': 'text', 'x': 50, 'y': 53, 'text': '--'},

						{'name': 'itemImage', 'type': 'image', 'x': 2, 'y': 75, 'image': PATH + 'label_item.png',
							'children':
							[
								{'name': 'itemReqIcon', 'type': 'image', 'x': 1, 'y': -2, 'image': PATH + 'item_icon.png'},
								{'name': 'itemReqValue', 'type': 'text', 'x': 49, 'y': 2, 'text': '--'},
							],
						},
					],
				},
				{'name': 'warpButton', 'type': 'button', 'x': 7+17, 'y': 77 + 24, 'text': uiScriptLocale.TELEPORT_WINDOW_BUTTON_TELEPORT, 'default_image': PATH + 'warp_btn_01.png', 'over_image': PATH + 'warp_btn_02.png', 'down_image': PATH + 'warp_btn_03.png'},
			],
		},
	),
}