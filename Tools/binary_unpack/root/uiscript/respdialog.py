import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/resp/"
HEADER_TEXT_COLOR = 0xffdcccb1
HEADER_FONT_NAME = "Tahoma:12"
BADGE_TEXT_COLOR = 0xffe6e6e6
BADGE_FONT_NAME = "Tahoma:12"
ERROR_TEXT_COLOR = 0xffcb6b67
ERROR_FONT_NAME = "Tahoma:12"

window = {
    "name": "RespDialog",
    "style": ("movable", "float",),

    "x": (SCREEN_WIDTH - 486) / 10,
    "y": (SCREEN_HEIGHT - 374) / 2,

    "width": 486,
    "height": 374,

    "children":
        (
            {
                "name": "board",
                "type": "board_with_titlebar",
                "style": ("attach",),

                "x": 0,
                "y": 0,

                "width": 486,
                "height": 374,

                "title": uiScriptLocale.RESP_TITLE,

                "children":
                    (
                        {
                            "name": "mob_board",
                            "type": "resp_board",

                            "x": 7,
                            "y": 30,

                            "width": 194,
                            "height": 336,

                            "children":
                                (
                                    {
                                        "name": "header_mob",
                                        "type": "image",

                                        "x": 4,
                                        "y": 1,

                                        "image": ROOT_PATH + "header_mob.sub",

                                        "children":
                                            (
                                                {
                                                    "name": "header_mob_wnd",
                                                    "type": "window",

                                                    "x": 3,
                                                    "y": 6,

                                                    "width": 36,
                                                    "height": 13,
                                                },
                                                {
                                                    "name": "header_mob_text",
                                                    "type": "text",

                                                    "x": 30,
                                                    "y": -1,

                                                    "color": HEADER_TEXT_COLOR,
                                                    "fontname": HEADER_FONT_NAME,

                                                    "all_align": "center",

                                                    "text": uiScriptLocale.RESP_MOB_HEADER,
                                                },
                                                #{
                                                #    "name": "header_mob_checkbox",
                                                #    "type": "resp_checkbox",
                                                #
                                                #    "x": 42,
                                                #    "y": 6,
                                                #
                                                #    "width": 15,
                                                #    "height": 13,
                                                #},
                                            ),
                                    },
                                ),
                        },
                        {
                            "name": "resp_board",
                            "type": "resp_board",

                            "x": 199,
                            "y": 30,

                            "width": 280,
                            "height": 336,

                            "children":
                                (
                                    {
                                        "name": "header_resp",
                                        "type": "image",

                                        "x": 4,
                                        "y": 1,

                                        "image": ROOT_PATH + "header_resp.sub",

                                        "children":
                                            (
                                                {
                                                    "name": "header_resp_wnd",
                                                    "type": "window",

                                                    "x": 3,
                                                    "y": 6,

                                                    "width": 36,
                                                    "height": 13,
                                                },
                                                {
                                                    "name": "header_resp_text",
                                                    "type": "text",

                                                    "x": 20,
                                                    "y": -1,

                                                    "color": HEADER_TEXT_COLOR,
                                                    "fontname": HEADER_FONT_NAME,

                                                    "all_align": "center",

                                                    "text": "Ork Reisi (Sev. 50)",
                                                },
                                                #{
                                                #    "name": "header_resp_checkbox",
                                                #    "type": "resp_checkbox",
                                                #
                                                #    "x": 42,
                                                #    "y": 6,
                                                #
                                                #    "width": 15,
                                                #    "height": 13,
                                                #},
                                                {
                                                    "name": "drop_button",
                                                    "type": "button",

                                                    "x": 246,
                                                    "y": 4,

                                                    "default_image": ROOT_PATH + "drop_button_01.sub",
                                                    "over_image": ROOT_PATH + "drop_button_02.sub",
                                                    "down_image": ROOT_PATH + "drop_button_03.sub",
                                                },
                                                {
                                                    "name": "badge_time",
                                                    "type": "image",

                                                    "x": 3,
                                                    "y": 23,

                                                    "image": ROOT_PATH + "badge_time.sub",

                                                    "children":
                                                        (
                                                            {
                                                                "name": "badge_time_value",
                                                                "type": "text",

                                                                "x": 5,
                                                                "y": -2,

                                                                "all_align": "center",

                                                                "color": BADGE_TEXT_COLOR,
                                                                "fontname": BADGE_FONT_NAME,

                                                                "text": "15 dakika",
                                                            },
                                                        ),
                                                },
                                                {
                                                    "name": "badge_level",
                                                    "type": "image",

                                                    "x": 89,
                                                    "y": 23,

                                                    "image": ROOT_PATH + "badge_level.sub",

                                                    "children":
                                                        (
                                                            {
                                                                "name": "badge_level_value",
                                                                "type": "text",

                                                                "x": 7,
                                                                "y": -2,

                                                                "all_align": "center",

                                                                "color": BADGE_TEXT_COLOR,
                                                                "fontname": BADGE_FONT_NAME,

                                                                "text": "140 - 150 Lv",
                                                            },
                                                        ),
                                                },
                                                {
                                                    "name": "badge_affect",
                                                    "type": "image",

                                                    "x": 182,
                                                    "y": 23,

                                                    "image": ROOT_PATH + "badge_affect.sub",

                                                    "children":
                                                        (
                                                            {
                                                                "name": "badge_affect_value",
                                                                "type": "text",

                                                                "x": 12,
                                                                "y": -2,

                                                                "all_align": "center",

                                                                "color": BADGE_TEXT_COLOR,
                                                                "fontname": BADGE_FONT_NAME,

                                                                "text": "Ölümsüzler",
                                                            },
                                                        ),
                                                },
                                            ),
                                    },
                                    {
                                        "name": "resp_window",
                                        "type": "window",

                                        "x": 3,
                                        "y": 41,

                                        "width": 274,
                                        "height": 266,

                                        "children":
                                            (
                                                {
                                                    "name": "list_resp",
                                                    "type": "image",

                                                    "x": 0,
                                                    "y": 0,

                                                    "image": ROOT_PATH + "list_resp.sub",

                                                    "children":
                                                        (
                                                            {
                                                                "name": "resp_error_text",
                                                                "type": "text",

                                                                "x": 0,
                                                                "y": 0,

                                                                "all_align": "center",

                                                                "color": ERROR_TEXT_COLOR,
                                                                "fontname": ERROR_FONT_NAME,

                                                                "text": uiScriptLocale.RESP_ERROR_TEXT,
                                                            },
                                                        ),
                                                },
                                            ),
                                    },
                                    {
                                        "name": "drop_window",
                                        "type": "window",

                                        "x": 12,
                                        "y": 60,

                                        "width": 256,
                                        "height": 224,

                                        "children":
                                            (
                                                {
                                                    "name": "drop_slot",
                                                    "type": "grid_table",

                                                    "x": 0,
                                                    "y": 0,

                                                    "start_index": 0,

                                                    "x_count": 8,
                                                    "y_count": 7,

                                                    "x_step": 32,
                                                    "y_step": 32,

                                                    "image": ROOT_PATH + "slot_base.sub"
                                                },
                                            ),
                                    },
                                    {
                                        "name": "page_window",
                                        "type": "window",

                                        "x": 3,
                                        "y": 308,

                                        "width": 274,
                                        "height": 25,

                                        "children":
                                            (
                                                {
                                                    "name": "left_button",
                                                    "type": "button",

                                                    "x": 0,
                                                    "y": 0,

                                                    "default_image": ROOT_PATH + "left_button_01.sub",
                                                    "over_image": ROOT_PATH + "left_button_02.sub",
                                                    "down_image": ROOT_PATH + "left_button_03.sub",
                                                    "disable_image": ROOT_PATH + "left_button_03.sub",
                                                },
                                                {
                                                    "name": "right_button",
                                                    "type": "button",

                                                    "x": 49,
                                                    "y": 0,

                                                    "horizontal_align": "right",

                                                    "default_image": ROOT_PATH + "right_button_01.sub",
                                                    "over_image": ROOT_PATH + "right_button_02.sub",
                                                    "down_image": ROOT_PATH + "right_button_03.sub",
                                                    "disable_image": ROOT_PATH + "right_button_03.sub",
                                                },
                                                {
                                                    "name": "page_count_slot",
                                                    "type": "image",

                                                    "x": 0,
                                                    "y": 1,

                                                    "horizontal_align": "center",
                                                    "vertical_align": "center",

                                                    "image": ROOT_PATH + "page_count_slot.sub",

                                                    "children":
                                                        (
                                                            {
                                                                "name": "page_count_value",
                                                                "type": "text",

                                                                "x": 0,
                                                                "y": -1,

                                                                "all_align": "center",

                                                                "color": BADGE_TEXT_COLOR,
                                                                "fontname": HEADER_FONT_NAME,

                                                                "text": "Sayfa 1 / 3",
                                                            },
                                                        ),
                                                },
                                            ),
                                    },
                                ),
                        },
                        {
                            "name": "loading_bar",
                            "type": "bar",

                            "x": 8,
                            "y": 30,

                            "width": 470,
                            "height": 336,

                            "color": 0xa0000000,

                            "children":
                                (
                                    {
                                        "name": "loading_animation",
                                        "type": "ani_image",

                                        "x": 185,
                                        "y": 118,

                                        "delay": 3,

                                        "images": (
                                            "{}animation/{:03d}.sub".format(ROOT_PATH, (i + 1)) for i in range(75)
                                        ),
                                    },
                                ),
                        },
                    ),
            },
            {
                "name" : "OpenWhenTeleportButton",
                "type" : "toggle_button",
                "x" : 18,
                "y" : 6,
                "tooltip_text_new" : "Otwieraj automatycznie (przy zmianie mapy/channela)",
                "tooltip_text_color" : 0xfff1e6c0,
                "tooltip_x" : 0,
                "tooltip_y" : -35,
                "default_image" : "d:/ymir work/ui/off_btn.png",
                "over_image" : "d:/ymir work/ui/off_btn.png",
                "down_image" : "d:/ymir work/ui/on_btn.png",
            },
        ),
}

for i in range(10):
    window["children"][0]["children"][1]["children"][1]["children"] += (
                                                                          {
                                                                              "name": "resp_slot_%02d" % (i + 1),
                                                                              "type": "image",

                                                                              "x": 11,
                                                                              "y": 8 + 25 * i,

                                                                              "image": ROOT_PATH + "resp_slot.sub",

                                                                              "children":
                                                                                  (
                                                                                      {
                                                                                          "name": "count_icon_%02d" % (
                                                                                                  i + 1),
                                                                                          "type": "image",

                                                                                          "x": 7,
                                                                                          "y": 5,

                                                                                          "image": ROOT_PATH + "count_icon.sub",

                                                                                          "children":
                                                                                              (
                                                                                                  {
                                                                                                      "name": "count_value_%02d" % (
                                                                                                              i + 1),
                                                                                                      "type": "text",

                                                                                                      "x": 20,
                                                                                                      "y": -2,

                                                                                                      "vertical_align": "center",
                                                                                                      "text_vertical_align": "center",

                                                                                                      "color": BADGE_TEXT_COLOR,
                                                                                                      "fontname": BADGE_FONT_NAME,

                                                                                                      "text": "%d." % (
                                                                                                              i + 1),
                                                                                                  },
                                                                                              ),
                                                                                      },
                                                                                      {
                                                                                          "name": "time_icon_%02d" % (
                                                                                                  i + 1),
                                                                                          "type": "image",

                                                                                          "x": 46,
                                                                                          "y": 5,

                                                                                          "image": ROOT_PATH + "time_icon.sub",

                                                                                          "children":
                                                                                              (
                                                                                                  {
                                                                                                      "name": "time_value_%02d" % (
                                                                                                              i + 1),
                                                                                                      "type": "text",

                                                                                                      "x": 20,
                                                                                                      "y": -2,

                                                                                                      "vertical_align": "center",
                                                                                                      "text_vertical_align": "center",

                                                                                                      "color": BADGE_TEXT_COLOR,
                                                                                                      "fontname": BADGE_FONT_NAME,

                                                                                                      "text": "Mevcut",
                                                                                                  },
                                                                                              ),
                                                                                      },
                                                                                      {
                                                                                          "name": "cord_icon_%02d" % (
                                                                                                  i + 1),
                                                                                          "type": "image",

                                                                                          "x": 121,
                                                                                          "y": 5,

                                                                                          "image": ROOT_PATH + "cord_icon.sub",

                                                                                          "children":
                                                                                              (
                                                                                                  {
                                                                                                      "name": "cord_value_%02d" % (
                                                                                                              i + 1),
                                                                                                      "type": "text",

                                                                                                      "x": 20,
                                                                                                      "y": -2,

                                                                                                      "vertical_align": "center",
                                                                                                      "text_vertical_align": "center",

                                                                                                      "color": BADGE_TEXT_COLOR,
                                                                                                      "fontname": BADGE_FONT_NAME,

                                                                                                      "text": "(213, 769)",
                                                                                                  },
                                                                                              ),
                                                                                      },
                                                                                      {
                                                                                          "name": "teleport_button_%02d" % (
                                                                                                  i + 1),
                                                                                          "type": "button",

                                                                                          "x": 208,
                                                                                          "y": 0,

                                                                                          "default_image": ROOT_PATH + "teleport_button_01.sub",
                                                                                          "over_image": ROOT_PATH + "teleport_button_02.sub",
                                                                                          "down_image": ROOT_PATH + "teleport_button_03.sub",
                                                                                      },
                                                                                  ),
                                                                          },
                                                                      )
