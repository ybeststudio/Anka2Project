BOARD_WIDTH = 598
BOARD_HEIGHT = 340
PATH = "d:/ymir work/ui/new_weekly_rank/"
ROOT_PATH = "d:/ymir work/ui/game/offlineshop/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

MARGIN_X = 0 #category
MARGIN_Y = 5 #whole_window

#category
CATEGORY_WINDOW_WIDTH = 162
CATEGORY_WINDOW_HEIGHT = 204
CATEGORY_BUTTON_POSX = 7
CATEGORY_BUTTON_POSY = 29
POS_ADD = 24
CATEGORY_BUTTON_NAME = ["Zabite Potwory", "Pokonane Bossy", "Zniszczone Metiny", "Ukoñczone Wyprawy", "Ulepszenia Alchemii", "Ulepszenia Przedmiotów", "Wydane SM"]
CATEGORY_TEXT_FONTNAME = "Tahoma:14"

#ranking
RANKING_MARGIN_X = CATEGORY_WINDOW_WIDTH+3
RANKING_MARGIN_Y = MARGIN_Y
RANKING_WINDOW_WIDTH = 411
RANKING_WINDOW_HEIGHT = 237
RANKING_Y_START = 28
RANKING_Y_ADD = 20

#myranking
MY_RANKING_MARGIN_Y = BOARD_HEIGHT-100
MY_RANKING_WINDOW_WIDTH = RANKING_WINDOW_WIDTH
MY_RANKING_WINDOW_HEIGHT = 57
MY_RANKING_Y_START = 28

#title
TITLE_WINDOW_HEIGHT = BOARD_HEIGHT-43
TITLE_X_ACCEPT_BUTTON = 74
TITLE_X_ACCEPT_BUTTON2 = 113
TITLE_FONTNAME = "Tahoma:14"

window = {
    "name" : "WeeklyRank_Window",
	"style" : ("movable", "float",),
    "x" : SCREEN_WIDTH/2, "y" : SCREEN_HEIGHT/5,
    "width": BOARD_WIDTH,"height": BOARD_HEIGHT,
    "children" :
    (
        {
            "name" : "board",
            "type" : "board",
            "x": 0, "y": 0,
            "width": BOARD_WIDTH,"height": BOARD_HEIGHT,
            "children" :
            (
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,"y" : 7,
					"width" : BOARD_WIDTH-15,
					"color" : "yellow",
					"children" :
					(
						{ 
							"name": "TitleName",
							"type":"text",
							"x":BOARD_WIDTH/2,"y":0,
							"text": "Ranking Tygodniowy (Sezon 0)",
                            "outline":"1",
							"text_horizontal_align":"center",
						},
					),
				},
				{
					"name" : "clear_button",
					"type" : "image",
					"x" : 17,
					"y" : 8,
					"image" : "icon/emoji/image-example-007.png",

					"tooltip_text_new" : "Ranking odœwie¿any jest co 10 minut.",
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
				},
				{
					"name" : "Window_Board",
					"type" : "window",
					"x": 10, "y": 25,
					"width":BOARD_WIDTH,"height":BOARD_HEIGHT,
					"children" :
					(
						{
							"name": "category_window",
							"type": "border_a",
							"x": MARGIN_X,
							"y": MARGIN_Y,
							"width": CATEGORY_WINDOW_WIDTH,
							"height": CATEGORY_WINDOW_HEIGHT,

							"children":
								(
									{
										"name":"Header",
										"type":"image",
										"x":3,"y":3,
										"image":PATH+"header_small_new.png",
										"children":
										(
											{
												"name":"header_text_main",
												"type":"text",
												"x":0,"y":0,
												"all_align" : "center",
												"text":"Kategoria",
												"outline":"1",
											},  
										),
									},
									{
										"name" : "CategoryButton_0",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[0],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_1",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[1],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_2",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD*2,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[2],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_3",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD*3,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[3],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_4",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD*4,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[4],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_5",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD*5,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[5],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
									{
										"name" : "CategoryButton_6",
										"type" : "radio_button",
										"x": CATEGORY_BUTTON_POSX, "y":CATEGORY_BUTTON_POSY+POS_ADD*6,
										"default_image" : PATH+"button_new1.tga",
										"over_image" : PATH+"button_new3.tga",
										"down_image" : PATH+"button_new2.tga",
										"children":
										(
											{
												"name":"categorytext","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[6],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
											},  
										),
									},  
								),
						},
						{
							"name" : "Title_Button_window",
							"type" : "window",
							"x": 0, "y": 207,
							"width":BOARD_WIDTH,"height":BOARD_HEIGHT,
							"children" :
							(
								{
									"name": "category_window",
									"type": "border_a",
									"x": MARGIN_X,
									"y": MARGIN_Y,
									"width": CATEGORY_WINDOW_WIDTH,
									"height": 90,

									"children":
										(
											{
												"name":"Header",
												"type":"image",
												"x":3,"y":3,
												"image":PATH+"header_small_new.png",
												"children":
												(
													{
														"name":"header_text_main",
														"type":"text",
														"x":0,"y":0,
														"all_align" : "center",
														"text":"Tytu³y",
														"outline":"1",
													},  
												),
											},
											{
												"name":"separator",
												"type":"image",
												"x":0,"y":30,
												"horizontal_align":"center",
												"image":"d:/ymir work/ui/bar.png",
											},
											{
												"name":"title_button",
												"type":"button",
												"x": 0, "y":17,
												"horizontal_align" : "center",
												"vertical_align" : "center",
												"default_image" : PATH+"button-example-000.png",
												"over_image" : PATH+"button-example-001.png",
												"down_image" : PATH+"button-example-002.png",
											},
										),
								},
							),
						},
						{
							"name": "rank_window",
							"type": "window",
							"x": RANKING_MARGIN_X,
							"y": RANKING_MARGIN_Y,
							"width": RANKING_WINDOW_WIDTH,
							"height": TITLE_WINDOW_HEIGHT,
							"children":
							(
								{
									"name": "ranking_window",
									"type": "border_a",
									"x": 0,
									"y": 0,
									"width": RANKING_WINDOW_WIDTH,
									"height": RANKING_WINDOW_HEIGHT,
									"children":
										(
											{
												"name":"Header",
												"type":"image",
												"x":3,"y":RANKING_Y_START-RANKING_Y_START+3,
												"image":PATH+"header_big_new.png",
												"children":
												(
													{
														"name":"header_text_main",
														"type":"text",
														"x":0,"y":0,
														"all_align" : "center",
														"text":"Ranking Globalny",
														"outline":"1",
													},  
												),
											},
											{
												"name":"box0",
												"type":"image",
												"x":0,"y":RANKING_Y_START,
												"horizontal_align":"center",
												"image":PATH+"box-1.png",
												"children":
												(
													{
														"name":"crown0",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-005.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"1",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box1",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD,
												"horizontal_align":"center",
												"image":PATH+"box-2.png",
												"children":
												(
													{
														"name":"crown2",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-006.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"2",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box2",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*2,
												"horizontal_align":"center",
												"image":PATH+"box-3.png",
												"children":
												(
													{
														"name":"crown3",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-007.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"3",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box3",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*3,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"4",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box4",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*4,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"5",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box5",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*5,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"6",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box6",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*6,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"7",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box7",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*7,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"8",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box8",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*8,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"9",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name":"box9",
												"type":"image",
												"x":0,"y":RANKING_Y_START+RANKING_Y_ADD*9,
												"horizontal_align":"center",
												"image":PATH+"box-4.png",
												"children":
												(
													{
														"name":"crown4",
														"type":"image",
														"x":7,"y":-1,
														"image":PATH+"image-example-008.png",
														"children":
														(
															{
																"name":"crown_text",
																"type":"text",
																"x":8,"y":-1,
																"all_align" : "center",
																"text":"10",
																"fontname":"Tahoma:12"
															},
														),
													},
												),
											},
											{
												"name": "my_ranking_window",
												"type": "border_a",
												"x": 0,
												"y": MY_RANKING_MARGIN_Y,
												"width": MY_RANKING_WINDOW_WIDTH,
												"height": MY_RANKING_WINDOW_HEIGHT,
												"children":
													(
														{
															"name":"Header",
															"type":"image",
															"x":3,"y":MY_RANKING_Y_START-MY_RANKING_Y_START+3,
															"image":PATH+"header_big_new.png",
															"children":
															(
																{
																	"name":"header_text_main",
																	"type":"text",
																	"x":0,"y":0,
																	"all_align" : "center",
																	"text":"Twoje Statystyki",
																	"outline":"1",
																},  
															),
														},
														{
															"name":"box10",
															"type":"image",
															"x":0,"y":MY_RANKING_Y_START,
															"horizontal_align":"center",
															"image":PATH+"box-4.png",
															"children":
															(
																{
																	"name":"crown4",
																	"type":"image",
																	"x":7,"y":-1,
																	"image":PATH+"image-example-008.png",
																	"children":
																	(
																		{
																			"name":"crown_text",
																			"type":"text",
																			"x":8,"y":-1,
																			"all_align" : "center",
																			"text":"-",
																			"fontname":"Tahoma:12"
																		},
																	),
																},
															),
														},
													),
											},
										),
								},
							),
						},
						{
							"name": "title_window",
							"type": "window",
							"x": RANKING_MARGIN_X,
							"y": RANKING_MARGIN_Y,
							"width": RANKING_WINDOW_WIDTH,
							"height": TITLE_WINDOW_HEIGHT,
							"children":
							(
								{
									"name": "sub_title_window",
									"type": "border_a",
									"x": 0,
									"y": 0,
									"width": RANKING_WINDOW_WIDTH,
									"height": TITLE_WINDOW_HEIGHT,
									"children":
									(
										{
											"name":"Header",
											"type":"image",
											"x":3,"y":RANKING_Y_START-RANKING_Y_START+3,
											"image":PATH+"header_big_new.png",
											"children":
											(
												{
													"name":"header_text_main",
													"type":"text",
													"x":0,"y":0,
													"all_align" : "center",
													"text":"Informacje o Twoich Tytu³ach",
													"outline":"1",
												},  
											),
										},
										{
											"name":"rectangle",
											"type":"image",
											"x":0,"y":94,
											"horizontal_align":"center",
											"image":"d:/ymir work/ui/settings/rectangle.png",
										},
										{
											"name":"rectangle",
											"type":"image",
											"x":0,"y":240,
											"horizontal_align":"center",
											"image":"d:/ymir work/ui/settings/rectangle.png",
										},
										{
											"name":"rectangle",
											"type":"image",
											"x":30,"y":35,
											# "horizontal_align":"center",
											"image":PATH+"sase.png",
										},
										{
											"name":"rectangle",
											"type":"image",
											"x":367,"y":35,
											# "horizontal_align":"center",
											"image":PATH+"sase.png",
										},
										{
											"name":"info_slot",
											"type":"image",
											"x":270,"y":42,
											"horizontal_align":"right",
											"image":PATH+"info_slot.tga",
											"children":
											(
												{
													"name":"info_slot_text1","type":"text","x":-1,"y":-10,"all_align" : "center","text":"",
												},
												{
													"name":"info_slot_text2","type":"text","x":-1,"y":8,"all_align" : "center","text":"",
												},
											),
										},
										{
											"name":"character_slot_real",
											"type":"image",
											"x":100,"y":36,
											"horizontal_align":"left",
											"image":PATH+"chracter_slot.tga",
										},
										{
											"name": "sub_title_window",
											"type": "border_a",
											"x": 0,
											"y": 100,
											"horizontal_align":"center",
											# "vertical_align":"center",
											"width": 279,
											"height": 69,
											"children":
											(
												{
													"name":"title_box0",
													"type":"image",
													"x":0,"y":4,
													"horizontal_align":"center",
													"image":PATH+"title_box-1.png",
													"children":
													(
														{
															"name":"crown1",
															"type":"image",
															"x":7,"y":-1,
															"image":PATH+"image-example-005.png",
															"children":
															(
																{
																	"name":"crown_text",
																	"type":"text",
																	"x":8,"y":-1,
																	"all_align" : "center",
																	"text":"1",
																	"fontname":"Tahoma:12"
																},
															),
														},
														{
															"name":"title0_accept_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"get_title0.png",
															"over_image":PATH+"get_title1.png",
															"down_image":PATH+"get_title2.png",
														},
														{
															"name":"title0_bonus_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON2,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"select_title0.png",
															"over_image":PATH+"select_title1.png",
															"down_image":PATH+"select_title2.png",
														},
														{
															"name":"title0_text","type":"text","x":-23,"y":-2,"all_align" : "center","text":"TitleName I", "outline":1, "fontname":TITLE_FONTNAME,
														}
													),
												},
												{
													"name":"title_box1",
													"type":"image",
													"x":0,"y":24,
													"horizontal_align":"center",
													"image":PATH+"title_box-2.png",
													"children":
													(
														{
															"name":"crown1",
															"type":"image",
															"x":7,"y":-1,
															"image":PATH+"image-example-006.png",
															"children":
															(
																{
																	"name":"crown_text",
																	"type":"text",
																	"x":8,"y":-1,
																	"all_align" : "center",
																	"text":"2",
																	"fontname":"Tahoma:12"
																},
															),
														},
														{
															"name":"title1_accept_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"get_title0.png",
															"over_image":PATH+"get_title1.png",
															"down_image":PATH+"get_title2.png",
														},
														{
															"name":"title1_bonus_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON2,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"select_title0.png",
															"over_image":PATH+"select_title1.png",
															"down_image":PATH+"select_title2.png",
														},
														{
															"name":"title1_text","type":"text","x":-23,"y":-2,"all_align" : "center","text":"TitleName II", "outline":1, "fontname":TITLE_FONTNAME,
														}
													),
												},
												{
													"name":"title_box2",
													"type":"image",
													"x":0,"y":44,
													"horizontal_align":"center",
													"image":PATH+"title_box-3.png",
													"children":
													(
														{
															"name":"crown1",
															"type":"image",
															"x":7,"y":-1,
															"image":PATH+"image-example-008.png",
															"children":
															(
																{
																	"name":"crown_text",
																	"type":"text",
																	"x":8,"y":-1,
																	"all_align" : "center",
																	"text":"3",
																	"fontname":"Tahoma:12"
																},
															),
														},
														{
															"name":"title2_accept_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"get_title0.png",
															"over_image":PATH+"get_title1.png",
															"down_image":PATH+"get_title2.png",
														},
														{
															"name":"title2_bonus_button",
															"type":"radio_button",
															"x":TITLE_X_ACCEPT_BUTTON2,"y":0,
															"horizontal_align" : "center",
															"vertical_align" : "center",
															"default_image":PATH+"select_title0.png",
															"over_image":PATH+"select_title1.png",
															"down_image":PATH+"select_title2.png",
														},
														{
															"name":"title2_text","type":"text","x":-23,"y":-2,"all_align" : "center","text":"TitleName III", "outline":1, "fontname":TITLE_FONTNAME,
														}
													),
												},
											),
										},
										{
											"name": "sub_title_window",
											"type": "border_a",
											"x": 0,
											"y": 200,
											"horizontal_align":"center",
											# "vertical_align":"center",
											"width": 230,
											"height": 25,
											"children":
											(
												# {
												# 	"name":"character_slot",
												# 	"type":"image",
												# 	"x":8,"y":2,
												# 	"horizontal_align":"center",
												# 	"image":PATH+"char_slot.png",
												# },
												{
													"name":"bonus_slotbar",
													"type":"image",
													"x":0,"y":7,
													"horizontal_align":"center",
													"image":PATH+"slotbar_new.png",
													"children":
													(
														{
															"name":"bonus_text","type":"text","x":0,"y":-2,"all_align" : "center","text":"-", "color":0xffd3bf7e, "outline":1, "fontname":TITLE_FONTNAME,
														},
													),
												},
												{
													"name":"rectangle",
													"type":"image",
													"x":0,"y":-28,
													"horizontal_align":"center",
													"image":"d:/ymir work/ui/settings/rectangle.png",
												},
											),
										},
										{
											"name":"titleaccept_button",
											"type":"button",
											"x":0,"y":177,
											"horizontal_align":"center",
											"default_image":PATH+"accept_title0.png",
											"over_image":PATH+"accept_title1.png",
											"down_image":PATH+"accept_title2.png",

											"tooltip_text_new" : "Wy³acz tytu³",
											"tooltip_text_color" : 0xfff1e6c0,
											"tooltip_x" : 0,
											"tooltip_y" : -35,
										},
										
										{
											"name":"progress_text","type":"text","x":10,"y":106,"all_align" : "center",
											"text":"-", 
											"color":0xffc4c4c4, 
											"outline":1, 
											"fontname":"Tahoma:16",
										},
										{
											"name":"progressbar_img",
											"type":"image",
											"x":0,"y":260,
											"horizontal_align":"center",
											"image":PATH+"time_slot.png",
											"children":
											(
												{
													"name":"progressbar",
													"type":"expanded_image",
													"x":11,"y":0,
													"horizontal_align":"center",
													"vertical_align":"center",
													"image":PATH+"time_bar.png",
												},
											),
										},
									),
								},
								
							),
						},
					),
				},
            ),
        },
    ),
}