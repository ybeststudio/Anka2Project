import uiScriptLocale
import app
ROOT = "d:/ymir work/ui/game/mailbox/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"
PUBLIC_PATH = "d:/ymir work/ui/public/"

# MailBox.py
#MAILBOX_WINDOW_WIDTH		= 316
#MAILBOX_WINDOW_HEIGHT		= 455

WINDOW_WIDTH	= 274
WINDOW_HEIGHT	= 316

BOARD_WINDOW_WIDTH	= 253
BOARD_WINDOW_HEIGHT	= 248

window = {
	"name" : "PostWrite",
	"style" : ("movable", "float",),
	
	#"x" : SCREEN_WIDTH / 2 - MAILBOX_WINDOW_WIDTH / 2 + MAILBOX_WINDOW_WIDTH -4,
	#"y" : SCREEN_HEIGHT / 2 - MAILBOX_WINDOW_HEIGHT / 2,
	"x" : SCREEN_WIDTH - 176 - WINDOW_WIDTH,
	"y" : SCREEN_HEIGHT - 37 - 565,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.MAILBOX_POST_SEND_TITLE,
		},
		
		## main bg
		{
			"name" : "bg_window",
			"type" : "window",

			"x" : 10,
			"y" : 32,
			
			"width" : BOARD_WINDOW_WIDTH,
			"height" : BOARD_WINDOW_HEIGHT,
			
			"children" :
			(
				## bg
				{
					"name" : "BGImg",
					"type" : "image",
					
					"x" : 0,
					"y" : 0,
					"image" : ROOT + "post_bg.sub",
				},
				## to text
				{ 
					"name" : "to_text_window", "type" : "window", "x" : 4, "y" : 4, "width" : 74, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"to_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MAILBOX_POST_WRITE_TO, "all_align" : "center"},
					),	
				},
				## to editline
				{ 
					"name" : "to_editline_window", "type" : "window", "x" : 86, "y" : 4, "width" : 115, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{
							"name":"to_editline",
							"type":"editline",
							"x":0,
							"y":2,
							"width" : 115,
							"height" : 14,
							"input_limit" : 12,
							
							"text":"",
						},
					),
				},
				## title text
				{ 
					"name" : "title_text_window", "type" : "window", "x" : 4, "y" : 25, "width" : 74, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"title_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MAILBOX_POST_WRITE_TITLE, "all_align" : "center"},
					),	
				},
				## title editline text
				{ 
					"name" : "title_editline_window", "type" : "window", "x" : 86, "y" : 25, "width" : 166, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{
							"name":"title_editline",
							"type":"editline",
							"x":0,
							"y":2,
							"width" : 166,
							"height" : 14,
							"input_limit" : 25,
							
							"text":"",
						},
					),
				},
				# post_friend_select_button
				{
					"name" : "post_friend_select_button",
					"type" : "button",

					"x" : 198,
					"y" : 4,

					"default_image" : ROOT + "friend_select_arrow_button_default.sub",
					"over_image" : ROOT + "friend_select_arrow_button_over.sub",
					"down_image" : ROOT + "friend_select_arrow_button_down.sub",
				},
				# post write confirm button
				{
					"name" : "post_write_confirm_button",
					"type" : "button",

					"x" : 216,
					"y" : 2,

					"default_image" : ROOT + "post_write_confirm_default.sub",
					"over_image" : ROOT + "post_write_confirm_over.sub",
					"down_image" : ROOT + "post_write_confirm_down.sub",
				},
				
				{
					"name" : "message_line",
					"type" : "editline",

					"x" : 8,#3,
					"y" : 50,#45,

					"width" : 235,#247,
					"height" : 99,

					"with_codepage" : 1,
					"input_limit" : 100,
					"limit_width" : 235,#247,
					"multi_line" : 1,
				},
				## msg count text
				{ 
					"name" : "msg_count_text_window", "type" : "window", "x" : 30, "y" : 128, "width" : BOARD_WINDOW_WIDTH-70, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"msg_count_text", "type":"text", "x":0, "y":2, "text":"000 / 100", "horizontal_align" : "right", "text_horizontal_align" : "center" },
					),
				},
			),
		},
		# send button
		{
			"name" : "post_send_button",
			"type" : "button",

			"x" : 48,
			"y" : 282,

			"default_image" : "d:/ymir work/ui/public/Large_Button_01.sub",
			"over_image" : "d:/ymir work/ui/public/Large_Button_02.sub",
			"down_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
			"disable_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
			
			"text" : uiScriptLocale.MAILBOX_POST_WRITE_SEND_TEXT,
		},
		# close button
		{
			"name" : "post_close_button",
			"type" : "button",

			"x" : 136,
			"y" : 282,

			"default_image" : "d:/ymir work/ui/public/Large_Button_01.sub",
			"over_image" : "d:/ymir work/ui/public/Large_Button_02.sub",
			"down_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
			"disable_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
			
			"text" : uiScriptLocale.MAILBOX_POST_WRITE_CLOSE_TEXT,
		},
		
		## won text
		{ 
			"name" : "won_text_window", "type" : "window", "x" : 170, "y" : 181, "width" : 85, "height" : 16, "style" : ("attach",),
			"children" :
			(
				{"name":"won_text", "type":"text", "x":0, "y":2, "text":"0", "horizontal_align" : "right", "text_horizontal_align" : "right" },
			),
		},
		## yang text
		{ 
			"name" : "yang_text_window", "type" : "window", "x" : 170, "y" : 199, "width" : 85, "height" : 16, "style" : ("attach",),
			"children" :
			(
				{"name":"yang_text", "type":"text", "x":0, "y":2, "text":"0", "horizontal_align" : "right", "text_horizontal_align" : "right" },
			),	
		},
		# won & yang window
		{
			"name" : "post_won_yang_window",
			"type" : "window",
			
			"x" : 138,
			"y" : 180,
			"width" : 117,
			"height" : 36,
		},
		## ITEM Slot
		{
			"name" : "item_slot",
			"type" : "grid_table",
			"x" : 61,
			"y" : 180,
			"start_index" : 0,
			"x_count" : 1,
			"y_count" : 3,
			"x_step" : 32,
			"y_step" : 32,
		},
		
		## render box
		{
			"name" : "renderbox1",
			"type" : "renderbox",
			"x" : 61,
			"y" : 180,
			"width" : 32,
			"height" : 32,
			"color" : 0x4CFFFFFF,
		},
		{
			"name" : "renderbox2",
			"type" : "renderbox",
			"x" : 61,
			"y" : 212,
			"width" : 32,
			"height" : 32,
			"color" : 0x4CFFFFFF,
		},
		{
			"name" : "renderbox3",
			"type" : "renderbox",
			"x" : 61,
			"y" : 244,
			"width" : 32,
			"height" : 32,
			"color" : 0x4CFFFFFF,
		},
		
		## 친구 이름 버튼들
		{
			"name" : "friend_select_img_window",
			"type" : "window",

			"x" : 92,
			"y" : 52,
			"width" : 131,
			"height" : 0,
		},
		## mouse over img
		{
			"name" : "mouse_over_image",
			"type" : "expanded_image",
			"style" : ("not_pick",),
			
			"x" : 0 + 10,
			"y" : 0 + 32,
			"image" : ROOT + "friend_list_mouse_over_img.sub",
		},
		## 친구 이름 text
		{
			"name" : "friend_select_text_window",
			"type" : "window",

			"x" : 92,
			"y" : 52,
			"width" : 115,
			"height" : 0,
		},
		{
			"name" : "friend_select_window",
			"type" : "window",

			"x" : 92,
			"y" : 52,
			"width" : 131,
			"height" : 0,
		},
		{
			"name" : "post_friend_scrollbar",
			"type" : "scrollbar",

			"x" : 93+131-15,
			"y" : 53,
			"size" : 100,
		},
	),	
}

#"name":"to_editline"
window["children"][1]["children"][2]["children"][0]["width"]		= 112
window["children"][1]["children"][2]["children"][0]["movable_text"]	= 1
#title_editline
window["children"][1]["children"][4]["children"][0]["movable_text"]	= 1