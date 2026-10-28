import localeInfo

window = {
	"name" : "DevilFloorInfo",
	"x" : SCREEN_WIDTH - 136, "y" : 0,
	"width" : 136, "height" : 161,

	"children" :
	(
		{
			"name" : "FloorInfoBG", "type" : "expanded_image",
			"x" : 0, "y" : 0,
		},
		{
			"name" : "CoolTime", "type" : "expanded_image",
			"x" : 8, "y" : 18,
		},
		{ "name":"LeftTime", "type":"text", "x":72, "y":75, "text" : "11:11", "text_horizontal_align":"center" },
		{ "name":"CurrentFloor", "type":"text", "x":72, "y":50, "text" : "1", "text_horizontal_align":"center", "r" : 1.0, "g" : 0.831, "b" : 0.043, "a" : 1.0, "fontsize":"LARGE"},
	),
}
