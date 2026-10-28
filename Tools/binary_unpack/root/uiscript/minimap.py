"""Anka minimap layout using the verified Baerim minimap asset coordinates."""

ROOT = "d:/ymir work/ui/minimap/"


def button(name, x, y, normal, hover, down):
    return {
        "name": name,
        "type": "button",
        "x": x,
        "y": y,
        "default_image": ROOT + normal,
        "over_image": ROOT + hover,
        "down_image": ROOT + down,
    }


def label(name, x, y):
    return {
        "name": name,
        "type": "text",
        "x": x,
        "y": y,
        "text_horizontal_align": "center",
        "outline": 1,
        "text": "",
    }


open_children = [
    {"name": "OpenWindowBGI", "type": "image", "x": 9, "y": 0, "image": ROOT + "background2.png"},
    {"name": "MiniMapWindow", "type": "window", "x": 4, "y": 5, "width": 128, "height": 128},
    button("ScaleUpButton", 130, 125, "+_normal.png", "+_hover.png", "+_down.png"),
    button("ScaleDownButton", 144, 112, "-_normal.png", "-_hover.png", "-_down.png"),
    button("AtlasShowButton", 150, 26, "m_normal.png", "m_hover.png", "m_down.png"),
    button("MiniMapHideButton", 136, 5, "x_normal.png", "x_hover.png", "x_down.png"),
    button("OpenRewardWindow", 21, 1, "rank_normal.png", "rank_hover.png", "rank_down.png"),
    button("DungeonSystemButton", 6, 27, "dung_normal.png", "dung_hover.png", "dung_down.png"),
    button("battlepass", 0, 56, "quest_normal.png", "quest_hover.png", "quest_down.png"),
    button("BiologistButton", 3, 86, "bio_normal.png", "bio_hover.png", "bio_down.png"),
    button("TeleportButton", 16, 114, "teleport_1_normal.png", "teleport_1_hover.png", "teleport_1_down.png"),
    button("PortableSearchButton", 39, 134, "search_normal.png", "search_hover.png", "search_down.png"),
    button("WarpLinkButton", 77, 150, "teleports_normal.png", "teleports_hover.png", "teleports_down.png"),
    {"name": "ServerInfoBg", "type": "image", "x": 25, "y": 183, "image": ROOT + "minimap_separator2.png"},
    label("ServerInfo", 95, 191),
    label("PositionInfo", 95, 198),
    label("DateInfo", 95, 207),
    label("TimeInfo", 95, 224),
    label("FPSInfo", -15, 9),
    label("ObserverCount", 70, 180),
]

window = {
    "name": "MiniMap",
    "x": SCREEN_WIDTH - 180,
    "y": 0,
    "width": 180,
    "height": 254,
    "children": [
        {
            "name": "OpenWindow",
            "type": "window",
            "x": 0,
            "y": 0,
            "width": 180,
            "height": 254,
            "children": open_children,
        },
        {
            "name": "CloseWindow",
            "type": "window",
            "x": 0,
            "y": 0,
            "width": 180,
            "height": 48,
            "children": [
                button("MiniMapShowButton", 145, 4,
                       "minimap_open_default.sub", "minimap_open_default.sub", "minimap_open_default.sub"),
            ],
        },
       #{
       #    "name": "bead", "type": "button", "x": 15, "y": 0,
       #    "default_image": "d:/ymir work/ui/game/12zi/bead/bead_default.sub",
       #    "over_image": "d:/ymir work/ui/game/12zi/bead/bead_down.sub",
       #    "down_image": "d:/ymir work/ui/game/12zi/bead/bead_over.sub",
       #},
       #label("beadInfo", 27, 7),
    ],
}
