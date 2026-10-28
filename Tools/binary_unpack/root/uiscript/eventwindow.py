import uiScriptLocale
NEW_EVENT = "d:/ymir work/ui/image/"

RUMI_ROOT = 'd:/ymir work/ui/'
window = {
	'name': 'EventDialog',
	"x" : SCREEN_WIDTH - 156 - 80+89,
	"y" : 3+26,
	'width': 22,
	'height': 22,
	'children':
	(
		{
			'name': 'event_window',
			'type': 'window',
			'x': 0,
			'y': 0,
			'width': 22,
			'height': 22,
			'children':
			(
				{
					'name': 'game_event_button',
					'type': 'button',
					'x': 0,
					'y': 0,
					"default_image" : NEW_EVENT + "event_open_default.tga",
					"over_image" : NEW_EVENT + "event_open_over.tga",
					"down_image" : NEW_EVENT + "event_open_down.tga",
				},
			)
		},
	)
}