import uiScriptLocale
import item
if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))


window = {
	"name" : "RumiWaitingPage",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH - 250 - 200 - 80 + 110,
	"y" : 60,
	"width" : 331,
	"height" : 120,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			"width" : 331,
			"height" : 120,
			
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : 374-56,
					"color" : "yellow",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":188-33+8, "y":3, "text":"Etkinlik genel görünümü", "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "Taskbar",
					"type" : "image",

					"x" : 10,
					"y" : 35,
					
					"image" : "d:/ymir work/ui/image/eventtaskbar.tga",
				},
				##CalendarButton
				{
					"name" : "CalendarInfo",
					"type" : "button",

					"x" : -7+32-35+291,
					"y" : 31+87+5-92-22,

					"default_image" : "d:/ymir work/calendar_1.tga",
					"over_image" : "d:/ymir work/calendar_2.tga",
					"down_image" : "d:/ymir work/calendar_3.tga",
				},
				{
					"name" : "ArkaPlan",
					"type" : "image",

					"x" : 19,
					"y" : 35+10+18,
					
					"image" : "d:/ymir work/ui/image/evenbuttonarka.tga",
				},
				{
					"name" : "Text1",
					"type" : "text",
					
					"x" : 22,
					"y" : 35+10+18+23,
					
					"text" : "Etkinlik için son saat:START_TIME",
				},
				{
					"name":"bg_charm_1",
					"type":"image",
					"x":142+64-2,
					"y":30+39-2,
					"image" : "icon/item/50930.tga"
				},
				{
					"name":"bg_charm_2",
					"type":"image",
					"x":142+64+35-2,
					"y":30+39-2,
					"image" : "icon/item/50929.tga"
				},
				{
					"name":"bg_charm_3",
					"type":"image",
					"x":142+64+35+35-2,
					"y":30+39-2,
					"image" : "icon/item/50928.tga"
				},
			),
		},		
	),	
}
