ROOT_PATH = "d:/ymir work/ui/game/resp/"
HEADER_TEXT_COLOR = 0xffdcccb1
HEADER_FONT_NAME = "Tahoma:12"

window = {
    "name": "RespRenderTargetDialog",
    "style": ("movable", "float",),

    "x": (SCREEN_WIDTH - 201) / 2,
    "y": (SCREEN_HEIGHT - 220) / 2,

    "width": 201,
    "height": 220,

    "children":
        (
            {
                "name": "render_board",
                "type": "resp_board",

                "x": 0,
                "y": 0,

                "width": 201,
                "height": 220,

                "children":
                    (
                        {
                            "name": "render_target",
                            "type": "render_target",

                            "x": 4,
                            "y": 8,

                            "width": 194,
                            "height": 210,

                            "index": 50,
                        },
                        {
                            "name": "header_render",
                            "type": "image",

                            "x": 4,
                            "y": 3,

                            "image": ROOT_PATH + "header_render.sub",

                            "children":
                                (
                                    {
                                        "name": "header_render_text",
                                        "type": "text",

                                        "x": 0,
                                        "y": -1,

                                        "color": HEADER_TEXT_COLOR,
                                        "fontname": HEADER_FONT_NAME,

                                        "all_align": "center",

                                        "text": "Ork Reisi (Sev. 50)",
                                    },
                                ),
                        },
                    ),
            },
        ),
}
