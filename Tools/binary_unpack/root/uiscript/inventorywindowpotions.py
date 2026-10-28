import item
import uiScriptLocale

window = {
	"name" : "InventoryWindowPotions", "style" : ("movable", "float",),
	"x" : SCREEN_WIDTH - 410, "y" : SCREEN_HEIGHT - 367,
	"width" : 180, "height" : 230,

	"children" :
	(
		{
			"name" : "Board", "type" : "board",
			"x" : 0, "y" : 0,
			"width" : 180, "height" : 230,

			"children" :
			(
				{
					"name" : "TitleBar", "type" : "titlebar", "style" : ("attach",),
					"x" : 8, "y" : 7,
					"width" : 165, "color" : "yellow",

					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 80, "y" : 3, "text" : uiScriptLocale.POTIONS_INVENTORY_TITLE, "text_horizontal_align" : "center" },
					),
				},
				{
					"name" : "GridTable", "type" : "grid_table",
					"x" : 10, "y" : 35,

					"start_index" : item.BELT_INVENTORY_SLOT_START,
					"x_count" : 5, "y_count" : 5,
					"x_step" : 32, "y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
				{
					"name" : "UseInventoryPotions", "type" : "button",
					"x" : 0, "y" : 200,
					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
			)
		},
	),
}