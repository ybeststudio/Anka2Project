# -*- coding: utf-8 -*-
path = "uiadvancedgameoptions.py"
with open(path, "r", encoding="utf-8", errors="replace") as f:
    s = f.read()

# 1) view_chat_off_button -> getOpt
s = s.replace(
    'view_chat_off_button = GetObject("view_chat_off_button")',
    'view_chat_off_button = getOpt("view_chat_off_button")'
)
# 2) show_damage_on_button -> getOpt
s = s.replace(
    'show_damage_on_button = GetObject("show_damage_on_button")',
    'show_damage_on_button = getOpt("show_damage_on_button")'
)
# 3) Market Ismi: salestext_window and buttons via getOpt
old_market = """					# Market Ýsmi
					try:
						salestext_window = GetObject("salestext_window")
						if salestext_window:
							salestext_on_button = salestext_window.GetChild("salestext_on_button")
							salestext_off_button = salestext_window.GetChild("salestext_off_button")
						else:
							# Fallback: direkt GetObject dene
							salestext_on_button = GetObject("salestext_on_button")
							salestext_off_button = GetObject("salestext_off_button")
					except:
						salestext_on_button = None
						salestext_off_button = None"""

new_market = """					# Market Ýsmi
					try:
						salestext_window = getOpt("salestext_window")
						if salestext_window:
							salestext_on_button = salestext_window.GetChild("salestext_on_button") or getOpt("salestext_on_button")
							salestext_off_button = salestext_window.GetChild("salestext_off_button") or getOpt("salestext_off_button")
						else:
							salestext_on_button = getOpt("salestext_on_button")
							salestext_off_button = getOpt("salestext_off_button")
					except:
						salestext_on_button = None
						salestext_off_button = None"""

if old_market in s:
    s = s.replace(old_market, new_market)
    print("Market block replaced.")
else:
    print("Market block not found (check encoding).")

with open(path, "w", encoding="utf-8", newline="") as f:
    f.write(s)
print("Done.")
