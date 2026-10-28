ROOT_PATH = "d:/ymir work/ui/game/resp/"
HEADER_TEXT_COLOR = 0xffdcccb1
HEADER_FONT_NAME = "Tahoma:14"

window = {
    "name": "RespMinimapDialog",
    "style": ("movable", "float",),

    "x": (SCREEN_WIDTH - 258) / 2,
    "y": (SCREEN_HEIGHT - 278) / 2,

    "width": 258,
    "height": 278,

    "children":
        (
            {
                "name": "map_board",
                "type": "resp_board",

                "x": 0,
                "y": 0,

                "width": 258,
                "height": 278,

                "children":
                    (
                        {
                            "name": "header_map",
                            "type": "expanded_image",

                            "x": 4,
                            "y": 3,

                            "image": ROOT_PATH + "header_map.sub",

                            "children":
                                (
                                    {
                                        "name": "header_map_text",
                                        "type": "text",

                                        "x": 0,
                                        "y": -1,

                                        "color": HEADER_TEXT_COLOR,
                                        "fontname": HEADER_FONT_NAME,

                                        "all_align": "center",

                                        "text": "Seungryong Vadisi",
                                    },
                                ),
                        },
                        {
                            "name": "map_image",
                            "type": "expanded_image",

                            "x": 4,
                            "y": 25,

                            "image": "d:/ymir work/ui/atlas/metin2_map_a1/atlas.sub",

                            "children": (
                                {
                                    "name": "map_point",
                                    "type": "image",

                                    "x": 20,
                                    "y": 20,

                                    "image": "d:/ymir work/ui/minimap/whitemark_new.tga",
                                },
                            ),
                        },
                    ),
            },
        ),
}
