if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

ROOT = "d:/ymir work/ui/game/"

Y_ADD_POSITION = 0
X_SPACE = 37

window = {
	"name" : "ExpandTaskBar",
	"style" : ("ltr", ),

	"x" : SCREEN_WIDTH/2 - 5,
	"y" : SCREEN_HEIGHT - 74,

	"width" : 37,
	"height" : 37,

	"children" :
	[
		{
			"name" : "ExpanedTaskBar_Board",
			"type" : "window",
			"style" : ("ltr", ),

			"x" : 0,
			"y" : 0,

			"width" : 37,
			"height" : 37,

			"children" :
			[
				{
					"name" : "DragonSoulButton",
					"type" : "button",
					"style" : ("ltr", ),

					"x" : 0,
					"y" : 0,

					"width" : 37,
					"height" : 37,

					"tooltip_text" : uiScriptLocale.TASKBAR_DRAGON_SOUL,

					"default_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_01.tga",
					"over_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_02.tga",
					"down_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_03.tga",
				},
			],
		},
	],
}
# Buton pozisyonlarýný dinamik hesapla
current_x = X_SPACE  # Ýlk buton (DragonSoul) 0'da, ikinci buton X_SPACE'den baþlar
button_count = 1  # DragonSoulButton zaten var

if app.ENABLE_GROWTH_PET_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "PetInfoButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : uiScriptLocale.TASKBAR_PET_INFO,

						"default_image" : "d:/ymir work/ui/pet/TaskBar_Pet_Button_01.tga",
						"over_image" : "d:/ymir work/ui/pet/TaskBar_Pet_Button_02.tga",
						"down_image" : "d:/ymir work/ui/pet/TaskBar_Pet_Button_03.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_HUNTING_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "HuntingButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Av Görevleri",

						"default_image" : "d:/ymir work/ui/public/huntsystem/taskbar_button_normal.sub", 
						"over_image" : "d:/ymir work/ui/public/huntsystem/taskbar_button_hover.sub", 
						"down_image" : "d:/ymir work/ui/public/huntsystem/taskbar_button_down.sub",
					},]
	current_x += X_SPACE
	button_count += 1

# Sidebar butonlarý
if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "SidebarTeleportButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Iþýnlanma",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/tpbutton1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/tpbutton2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/tpbutton3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_SPECIAL_INVENTORY:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "SidebarSpecialDepButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Özel Depo",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/k_envanter_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/k_envanter_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/k_envanter_3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_RENEWAL_OFFLINESHOP:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "SidebarOfflineShopButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Çevrimdýþý Pazar",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/pazar_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/pazar_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/pazar_3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_RENEWAL_SWITCHBOT:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "SidebarSwitchbotButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Efsun Botu",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/efsun_botu_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/efsun_botu_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/efsun_botu_3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "SidebarPotionsButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Ýksirler",
						"default_image" : "d:/ymir work/ui/game/taskbarnew/iksir_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/iksir_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/iksir_3.tga",
					},]
current_x += X_SPACE
button_count += 1

if app.ENABLE_WIKI_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "WikiButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Vikipedi",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/wiki_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/wiki_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/wiki_3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.__AUTO_HUNT__:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "AutoButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Otomatik Av",

						"default_image" : "d:/ymir work/ui/game/taskbar/btn_eq_1.png",
						"over_image" : "d:/ymir work/ui/game/taskbar/btn_eq_2.png",
						"down_image" : "d:/ymir work/ui/game/taskbar/btn_eq_3.png",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_AUTO_SELL_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "AutoSell",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Otomatik Satýþ",

						"default_image" : "d:/ymir work/ui/game/taskbarnew/sil_1.tga",
						"over_image" : "d:/ymir work/ui/game/taskbarnew/sil_2.tga",
						"down_image" : "d:/ymir work/ui/game/taskbarnew/sil_3.tga",
					},]
	current_x += X_SPACE
	button_count += 1

if app.ENABLE_BIOLOGIST_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "BiologistButton",
						"type" : "button",
						"style" : ("ltr", ),

						"x" : current_x,
						"y" : 0,

						"width" : 37,
						"height" : 37,

						"tooltip_text" : "Biyolog Görevleri",

                        "default_image" : "d:/ymir work/ui/game/taskbarnew/taskbar_button_normal.sub",
                        "over_image" : "d:/ymir work/ui/game/taskbarnew/taskbar_button_hover.sub",
                        "down_image" : "d:/ymir work/ui/game/taskbarnew/taskbar_button_down.sub",
					},]
	current_x += X_SPACE
	button_count += 1

# Window geniþliðini toplam buton sayýsýna göre ayarla
window["width"] = X_SPACE * button_count
window["children"][0]["width"] = window["width"]
# Window'u ortalamak için x pozisyonunu güncelle
window["x"] = SCREEN_WIDTH/2 - (window["width"] / 2)
