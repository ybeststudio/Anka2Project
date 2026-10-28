import uiScriptLocale
# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# import app 추가
import app

if app.ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# UIScript 경로 변경
	window = {
	
		"x" : 0,
		"y" : 0,
	
		"width" : SCREEN_WIDTH,
		"height" : SCREEN_HEIGHT,
	
		"children" :
		[
			## Board
			{
				"name" : "BackGround",
				"type" : "expanded_image",
	
				"x" : 0,
				"y" : 0,
	
				"image" : "d:/ymir work/ui/intro/pattern/Line_Pattern.tga",
	
				"x_scale" : float(SCREEN_WIDTH) / 800.0,
				"y_scale" : float(SCREEN_HEIGHT) / 600.0,
			},
			{ 
				"name":"ErrorMessage", 
				"type":"text", "x":10, "y":10, 
				"text": uiScriptLocale.LOAD_ERROR, 
			},
			
			{
				"name" : "GageBoard",
				"type" : "window",
				"style" : ("ltr",),
				"x" : float(SCREEN_WIDTH) * 400 / 800.0 - 200,
				"y" : float(SCREEN_HEIGHT) * 500 / 600.0 ,
				"width" : 400, 
				"height": 80,
	
				"children" :
				(
				
					{
						"name" : "BackGage",
						"type" : "expanded_image",
	
						"x" : 40,
						"y" : 25,
	
						"image" : "shared/common/ui/loading/gauge_empty.dds",
					},
					{
						"name" : "FullGage",
						"type" : "expanded_image",
	
						"x" : 40,
						"y" : 25,
	
						"image" : "shared/common/ui/loading/gauge_full.dds",
					},
				),
			},
		],
	}
	
	window["children"] = window["children"] + [
	{
		"name" : "TipBackground",
		"type" : "middleboard",
		
		"x"	: 0,
		"y" : float(SCREEN_HEIGHT) * 500 / 600.0 - 100,
		
		"width" : SCREEN_WIDTH,
		"height" : 100,
		
		"children" :
		(
			{
				"name" : "LoadingTip",
				"type" : "text",
				
				"x"	: float(SCREEN_WIDTH) / 2,
				"y" : 40,				
				
				"text" : " ",
				"text_horizontal_align" : "center",
				"fontsize" : "LARGE",
			},
		),		
	},]
else:
	window = {
		"name" : "LoginWindow",
		"sytle" : ("movable",),
	
		"x" : 0,
		"y" : 0,
	
		"width" : SCREEN_WIDTH,
		"height" : SCREEN_HEIGHT,
	
		"children" :
		(
			## Board
			{
				"name" : "BackGround",
				"type" : "expanded_image",
	
				"x" : 0,
				"y" : 0,
	
				"image" : "d:/ymir work/ui/intro/pattern/Line_Pattern.tga",
	
				"x_scale" : float(SCREEN_WIDTH) / 800.0,
				"y_scale" : float(SCREEN_HEIGHT) / 600.0,
			},
			{ 
				"name":"ErrorMessage", 
				"type":"text", "x":10, "y":10, 
				"text": uiScriptLocale.LOAD_ERROR, 
			},
			{
				"name" : "GageBoard",
				"type" : "window",
				"x" : float(SCREEN_WIDTH) * 400 / 800.0 - 200,
				"y" : float(SCREEN_HEIGHT) * 500 / 600.0 ,
				"width" : 400, 
				"height": 80,
	
				"children" :
				(
					{
						"name" : "BackGage",
						"type" : "ani_image",
	
	
						"x" : 0,
						"y" : 0,
	
						"delay" : 1,
	
						"images" :
						(
							"d:/ymir work/ui/intro/loading/00.sub",
							"d:/ymir work/ui/intro/loading/01.sub",
							"d:/ymir work/ui/intro/loading/02.sub",
							"d:/ymir work/ui/intro/loading/03.sub",
							"d:/ymir work/ui/intro/loading/04.sub",
							"d:/ymir work/ui/intro/loading/05.sub",
							"d:/ymir work/ui/intro/loading/06.sub",
							"d:/ymir work/ui/intro/loading/07.sub",
							"d:/ymir work/ui/intro/loading/08.sub",
							"d:/ymir work/ui/intro/loading/09.sub",
							"d:/ymir work/ui/intro/loading/10.sub",
							"d:/ymir work/ui/intro/loading/11.sub",						
							"d:/ymir work/ui/intro/loading/12.sub",
							"d:/ymir work/ui/intro/loading/13.sub",
							"d:/ymir work/ui/intro/loading/14.sub",
							"d:/ymir work/ui/intro/loading/15.sub",
							"d:/ymir work/ui/intro/loading/16.sub",
							"d:/ymir work/ui/intro/loading/17.sub",
							"d:/ymir work/ui/intro/loading/18.sub",
							"d:/ymir work/ui/intro/loading/19.sub",
							"d:/ymir work/ui/intro/loading/20.sub",
							"d:/ymir work/ui/intro/loading/21.sub",
							"d:/ymir work/ui/intro/loading/22.sub",
							"d:/ymir work/ui/intro/loading/23.sub",
						)
					},
					{
						"name" : "BackGage",
						"type" : "expanded_image",
	
						"x" : 70,
						"y" : 25,
	
						"image" : "d:/ymir work/ui/intro/loading/gauge_empty.sub",			
					},
					{
						"name" : "FullGage",
						"type" : "expanded_image",
	
						"x" : 70,
						"y" : 25,
	
						"image" : "d:/ymir work/ui/intro/loading/gauge_full.sub",			
					},
				),
			},
		),
	}