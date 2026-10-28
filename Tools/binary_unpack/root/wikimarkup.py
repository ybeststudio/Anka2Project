# -*- coding: ascii -*-
#
# wikimarkup.py
#
# ARCHITECTURE NOTE
# -----------------
# Wiki article markup engine (reference "Wikipedi" markup), split in 3 layers:
#
#   1) Parse(text, loader)   -> Document (pure python, NO ui dependency, py2/py3 safe)
#        text lines -> tag tokens -> node dicts. Component macros
#        ([TITLE; X=..; TEXT_1=..]) are expanded with the composants/*.txt
#        templates (ParentTxt(n) / ParentImg(n) / ParentVnum(n) /
#        ParentPrice(n) / ParentTooltip placeholders), tables
#        (TABLE_START/TR/TD/TABLE_END) become one 'table' node.
#        Values keep their raw text; numeric fields are folded to int when
#        they are constant expressions, otherwise kept as str and resolved
#        at build time (they may contain data functions).
#
#   2) Resolver (+ DataProvider)
#        {WK:id[:arg..]} locale keys, data functions (AppendBonus, GetBonus,
#        HaveSkill, AppendShopItem, AppendDropItem, ...) and Color()/Color2().
#        Data functions are served by a pluggable DataProvider (default: empty).
#
#   3) Build(parent, doc, width, ctx) -> BuildResult
#        Creates ui.* windows (ImageBox/TextLine/Button/Bar/...) and returns
#        the total height + anchors. Link clicks go to ctx.OnLink(kind,id,pos).
#        The caller MUST keep the returned BuildResult alive (it owns the
#        window references).
#
# Safe arithmetic: no eval(); EvalExpr() is a small recursive-descent parser.
# This file is Python 2.7 and Python 3 compatible; keep it ASCII only.

import re

# --------------------------------------------------------------------------
# constants
# --------------------------------------------------------------------------

COLOR_GOLD = 0xFFD7B25B
COLOR_TEXT_DEFAULT = 0xFFCEBB98
FUNC_COLOR1 = "|cFFD7B25B"	# Color(x)
FUNC_COLOR2 = "|cFF6CFF6C"	# Color2(x)

COMPONENT_ALIASES = {
	"TITLE": "title",
	"SUBTITLE": "subtitle",
	"DESCRIPTION_WITH_ITEM": "description_with_item",
	"DESCRIPTION_WITH_2_ITEMS": "description_with_2_items",
	"DESCRIPTION_WITH_3_ITEMS": "description_with_3_items",
	"DESCRIPTION_WITH_4_ITEMS": "description_with_4_items",
	"DESCRIPTION_BETWEEN_2_ITEMS": "description_between_2_items",
	"DESCRIPTION_WITH_BG": "description_with_bg",
	"DESCRIPTION_WITH_RENDER": "description_with_render",
	"ITEM_LIST_HORIZONTAL_1": "item_list_horizontal_1",
	"DUNGEON": "dungeon",
	"SKILL_TREE_ITEM": "skill_tree/skill_tree_item",
	"67_BONUS_ITEM": "67_bonus/bonus_item",
	"FRAG_TYPE": "67_bonus/frag_type",
	"LIST_BONUS": "67_bonus/list_bonus",
	"BONUS_ITEM": "bonus_list/bonus_item",
	"BP_QUEST_LINE": "battlepass/quest_item",
	"BIOLO_ITEM": "biolo/biolo_item",
	"BIOLO_REWARD": "biolo/biolo_reward",
	"PET_EVOLVE": "pet/pet_evolve",
	"PET_SKILL_ITEM": "pet/skill_item",
	"DRAGON_SOUL_BONUS_LIST": "dragon_soul/bonus_list",
	"DROP_ITEM": "dungeon_collector/drop_item",
	"ROW_TRIGGER": "event/row_trigger",
}

# bare tokens that are NOT component names
STRUCT_FLAGS = (
	"NO_ADD_Y", "CENTER_ALIGN", "RIGHT_ALIGN", "LEFT_ALIGN", "OUTLINE",
	"TR", "TD", "TABLE_START", "TABLE_END", "THINBOARD", "BOLD",
	"CHANGE Y", "CHANGE_Y",
)

# data functions known to the resolver (longest first for the regex)
DATA_FUNCS = (
	"AppendSkillTreeCount", "AppendSkillTreeItem", "GetBuffiSkillLevel",
	"HaveBuffiSkill", "AppendBioloData", "AppendDropCount", "AppendDropItem",
	"AppendShopData", "AppendShopItem", "GetSkillLevel", "AppendBonus2",
	"AppendBonus", "AppendSkill", "AppendItem", "AppendTree", "AppendPNJ",
	"HaveBonus", "HaveSkill", "GetBonus", "Color2", "Color", "Roll",
)

_FUNC_RE = re.compile(r"(?<![A-Za-z0-9_])(" + "|".join(DATA_FUNCS) + r")\(")
_PARENT_RE = re.compile(r"Parent(Txt|Img|Vnum|Price)\((\d+)\)|ParentTooltip")
_WK_RE = re.compile(r"\{WK:([^{}]*)\}")
_CODE_RE = re.compile(r"\|c[0-9a-fA-F]{8}|\|r|\|h|\|H[^|]*\|h")
_EXPR_CHARS = re.compile(r"^[0-9+\-*/%().\s]+$")
_INT_RE = re.compile(r"^-?\d+$")
_HEX_RE = re.compile(r"^[0-9a-fA-F]+$")

MAX_COMPONENT_DEPTH = 4


# --------------------------------------------------------------------------
# safe arithmetic
# --------------------------------------------------------------------------

class ExprError(ValueError):
	pass


def EvalExpr(text):
	"""Evaluate +,-,*,/,%,(),unary -/+ on numbers. Returns int when integral."""
	s = text.strip()
	if not s or not _EXPR_CHARS.match(s):
		raise ExprError("bad expression: %r" % text)
	toks = re.findall(r"\d+\.\d+|\d+|[-+*/%()]", s)
	pos = [0]

	def peek():
		return toks[pos[0]] if pos[0] < len(toks) else None

	def take():
		t = peek()
		pos[0] += 1
		return t

	def primary():
		t = take()
		if t is None:
			raise ExprError("unexpected end: %r" % text)
		if t == "(":
			v = expr()
			if take() != ")":
				raise ExprError("missing ): %r" % text)
			return v
		if t in "+-*/%)":
			raise ExprError("unexpected %s: %r" % (t, text))
		return float(t) if "." in t else int(t)

	def unary():
		t = peek()
		if t == "-":
			take()
			return -unary()
		if t == "+":
			take()
			return unary()
		return primary()

	def term():
		v = unary()
		while peek() in ("*", "/", "%"):
			op = take()
			r = unary()
			if op == "*":
				v = v * r
			elif r == 0:
				raise ExprError("division by zero: %r" % text)
			elif op == "/":
				v = float(v) / r
			else:
				v = v % r
		return v

	def expr():
		v = term()
		while peek() in ("+", "-"):
			op = take()
			r = term()
			v = v + r if op == "+" else v - r
		return v

	v = expr()
	if pos[0] != len(toks):
		raise ExprError("trailing tokens: %r" % text)
	if isinstance(v, float):
		iv = int(v)
		return iv if iv == v else v
	return v


def ToNumber(value, default=None):
	"""int/float passthrough, str -> EvalExpr, failure -> default."""
	if isinstance(value, (int, float)):
		return value
	if value is None:
		return default
	try:
		return EvalExpr(value)
	except ExprError:
		return default


def _fold_num(value):
	"""Constant expression -> int/float, else the raw str (deferred)."""
	if value is None:
		return None
	v = value.strip()
	if not v:
		return None
	if _EXPR_CHARS.match(v):
		return EvalExpr(v)
	return v


# --------------------------------------------------------------------------
# low level text helpers
# --------------------------------------------------------------------------

def _match_close(s, start, open_ch, close_ch):
	"""index of the bracket closing s[start] (== open_ch), nesting aware; -1 if none."""
	depth = 0
	i = start
	n = len(s)
	while i < n:
		c = s[i]
		if c == open_ch:
			depth += 1
		elif c == close_ch:
			depth -= 1
			if depth == 0:
				return i
		i += 1
	return -1


def _split_depth0(s, sep):
	"""split on sep outside of (), {}, []"""
	out = []
	cur = []
	depth = 0
	for c in s:
		if c in "({[":
			depth += 1
		elif c in ")}]":
			if depth > 0:
				depth -= 1
		if c == sep and depth == 0:
			out.append("".join(cur))
			cur = []
		else:
			cur.append(c)
	out.append("".join(cur))
	return out


def StripCodes(text):
	"""remove |cAARRGGBB |r |h codes (for measuring)."""
	return _CODE_RE.sub("", text)


def ParseColor(value, default=None):
	"""'cebb98' / '0xffd700' / '0xRRGGBBAA' -> 0xAARRGGBB int."""
	if value is None:
		return default
	v = value.strip()
	if v[:2] in ("0x", "0X"):
		v = v[2:]
	if not _HEX_RE.match(v or "x"):
		return default
	if len(v) == 6:
		return 0xFF000000 | int(v, 16)
	if len(v) == 8:
		n = int(v, 16)
		return ((n & 0xFF) << 24) | (n >> 8)
	return default


def ParseFontSize(value, default=12):
	"""'18b' -> (18, True)"""
	if value is None:
		return default, False
	v = value.strip().lower()
	bold = v.endswith("b")
	if bold:
		v = v[:-1]
	try:
		return int(v), bold
	except ValueError:
		return default, bold


def ParseSockets(value):
	"""'0:0#1:100' -> [(0,0),(1,100)]"""
	out = []
	if not value:
		return out
	for part in value.split("#"):
		a = part.split(":")
		if len(a) == 2 and _INT_RE.match(a[0].strip()) and _INT_RE.match(a[1].strip()):
			out.append((int(a[0]), int(a[1])))
	return out


def ParseAttrs(value):
	"""'0:9?20#1:15?10' -> [(0,9,20),(1,15,10)]"""
	out = []
	if not value:
		return out
	for part in value.split("#"):
		m = re.match(r"^\s*(\d+):(\d+)\?(-?\d+)\s*$", part)
		if m:
			out.append((int(m.group(1)), int(m.group(2)), int(m.group(3))))
	return out


def ParseLinkIndex(value):
	"""'article#122#0.0' / 'website#https://x#NONE' / 'warp#x#y#z' -> (kind, id, pos)"""
	if not value:
		return ("", "", "")
	p = value.split("#", 2)
	while len(p) < 3:
		p.append("")
	return (p[0].strip().lower(), p[1].strip(), p[2].strip())


def WrapText(text, max_width, measure):
	"""greedy word wrap. measure(str)->width (codes are stripped before calling)."""
	if max_width <= 0:
		return [text]
	lines = []
	for para in text.split("\n"):
		words = para.split(" ")
		cur = ""
		for w in words:
			cand = w if not cur else cur + " " + w
			if cur and measure(StripCodes(cand)) > max_width:
				lines.append(cur)
				cur = w
			else:
				cur = cand
		lines.append(cur)
	return lines


# --------------------------------------------------------------------------
# tokenizer / parser
# --------------------------------------------------------------------------

def _logical_lines(text):
	"""-> list of (lineno, line). Joins '\\' continuation of '[' tags."""
	raw = text.replace("\r\n", "\n").replace("\r", "\n").split("\n")
	out = []
	i = 0
	while i < len(raw):
		start = i + 1
		ln = raw[i]
		i += 1
		if ln.lstrip().startswith("["):
			while ln.rstrip().endswith("\\") and i < len(raw):
				ln = ln.rstrip()[:-1] + raw[i].strip()
				i += 1
			if _match_close(ln.lstrip(), 0, "[", "]") < 0:
				# multi line tag without '\\' (opening '[' alone on its line)
				j = i
				acc = ln
				while j < len(raw) and j - i < 80:
					acc = acc + raw[j].strip()
					j += 1
					if _match_close(acc.lstrip(), 0, "[", "]") >= 0:
						ln = acc
						i = j
						break
		out.append((start, ln))
	return out


def _split_groups(line):
	"""'[a;b][c]rest' -> (['a;b','c'], 'rest'); unbalanced -> (None, line)"""
	s = line.lstrip()
	groups = []
	while s.startswith("["):
		j = _match_close(s, 0, "[", "]")
		if j < 0:
			return None, line
		groups.append(s[1:j])
		s = s[j + 1:]
		if not s.startswith("["):
			break
	return groups, s


def _tokens(body):
	"""'K=V; FLAG ; K2 = V2' -> [(KEY, value|None)] (key upper, spaces collapsed)."""
	out = []
	for tok in _split_depth0(body, ";"):
		t = tok.strip()
		if not t:
			continue
		if "=" in t:
			k, v = t.split("=", 1)
			out.append((re.sub(r"\s+", " ", k.strip()).upper(), v.strip()))
		else:
			out.append((re.sub(r"\s+", " ", t).upper(), None))
	return out


def _get(tokens, key):
	for k, v in tokens:
		if k == key:
			return v
	return None


def _has(tokens, key):
	for k, v in tokens:
		if k == key:
			return True
	return False


def _common(tokens):
	"""layout / format attributes shared by all visible nodes"""
	d = {}
	for key, name in (("X", "x"), ("Y", "y"), ("X_PADDING", "x_pad"), ("Y_PADDING", "y_pad"),
					("MAX_WIDTH", "max_width"), ("WIDTH", "w"), ("HEIGHT", "h"),
					("X_SHIFT", "x_shift"), ("Y_SHIFT", "y_shift")):
		v = _get(tokens, key)
		if v is not None:
			d[name] = _fold_num(v)
	if _has(tokens, "NO_ADD_Y"):
		d["no_add_y"] = True
	if _has(tokens, "CENTER_ALIGN"):
		d["align"] = "center"
	elif _has(tokens, "RIGHT_ALIGN"):
		d["align"] = "right"
	if _has(tokens, "OUTLINE"):
		d["outline"] = True
	fs = _get(tokens, "FONT_SIZE")
	if fs is not None:
		d["font_size"], d["bold"] = ParseFontSize(fs)
	c = _get(tokens, "COLOR")
	if c is not None:
		d["color"] = ParseColor(c, COLOR_TEXT_DEFAULT)
	t = _get(tokens, "TOOLTIP")
	if t is not None:
		d["tooltip"] = t
	return d


class Document(object):
	def __init__(self):
		self.nodes = []
		self.errors = []	# (lineno, message)
		self.summary = False
		self.banner = None


def _substitute(line, params):
	"""replace ParentXxx(n) placeholders, apply TEXT_n_X_PADDING etc. to the line."""
	used = []

	def repl(m):
		if m.group(0) == "ParentTooltip":
			return params.get("TOOLTIP", "")
		kind = m.group(1)
		n = m.group(2)
		prefix = {"Txt": "TEXT_", "Img": "IMG_", "Vnum": "VNUM_", "Price": "PRICE_"}[kind] + n
		used.append(prefix)
		return params.get(prefix, "")

	new = _PARENT_RE.sub(repl, line)
	if used and new.lstrip().startswith("["):
		extra = []
		for prefix in used:
			xp = params.get(prefix + "_X_PADDING")
			yp = params.get(prefix + "_Y_PADDING")
			al = params.get(prefix + "_ALIGN")
			if xp:
				extra.append("X_SHIFT=" + xp)
			if yp:
				extra.append("Y_SHIFT=" + yp)
			if al and al.lower() == "center":
				extra.append("CENTER_ALIGN")
			elif al and al.lower() == "right":
				extra.append("RIGHT_ALIGN")
		if extra:
			k = new.index("[")
			new = new[:k + 1] + ";".join(extra) + ";" + new[k + 1:]
	return new


class _Parser(object):
	def __init__(self, loader, doc, depth=0):
		self.loader = loader
		self.doc = doc
		self.depth = depth
		self.nodes = []
		self.table = None
		self.row = None

	def err(self, lineno, msg):
		self.doc.errors.append((lineno, msg))

	# -- entry -----------------------------------------------------------
	def feed(self, text, params=None, skip_first_line=False, base_line=0):
		lines = _logical_lines(text)
		for idx, (lineno, line) in enumerate(lines):
			if skip_first_line and idx == 0:
				continue
			if params is not None:
				line = _substitute(line, params)
			self.line(base_line or lineno, line)
		if self.table is not None:
			self.err(base_line, "TABLE_START without TABLE_END")
			self.close_table()
		return self.nodes

	# -- one logical line ---------------------------------------------------
	def line(self, lineno, line):
		s = line.strip()
		if not s:
			return
		if not s.startswith("["):
			self.text_line(lineno, [], s)
			return
		groups, rest = _split_groups(line)
		if groups is None:
			self.err(lineno, "unbalanced [ ]: %s" % s[:60])
			return
		if len(groups) == 1 and groups[0].strip().startswith("/"):
			self.doc.summary = True
			return
		tokens = []
		for g in groups:
			tokens.extend(_tokens(g))
		self.dispatch(lineno, tokens, rest.strip())

	def dispatch(self, lineno, tokens, rest):
		keys = [k for k, v in tokens]
		if "TABLE_START" in keys:
			self.open_table(lineno, tokens)
			return
		if "TABLE_END" in keys:
			if self.table is None:
				self.err(lineno, "TABLE_END without TABLE_START")
			else:
				self.close_table()
			return
		if "TR" in keys:
			if self.table is None:
				self.err(lineno, "TR outside table")
				return
			h = _get(tokens, "H")
			self.row = {"h": _fold_num(h) if h else 0, "cells": []}
			self.table["rows"].append(self.row)
			return
		if "TD" in keys:
			if self.table is None or self.row is None:
				self.err(lineno, "TD outside table row")
				return
			self.row["cells"].append(self.make_cell(tokens, rest))
			return
		if "CHANGE Y" in keys or "CHANGE_Y" in keys:
			# "[CHANGE Y=713]" -> key "CHANGE Y" with value
			v = _get(tokens, "CHANGE Y")
			if v is None:
				v = _get(tokens, "CHANGE_Y")
			self.nodes.append({"type": "changey", "y": _fold_num(v) if v else 0})
			return
		if "BANNER_IMG" in keys:
			path = _get(tokens, "BANNER_IMG")
			self.doc.banner = path
			self.nodes.append({"type": "banner", "path": path})
			return
		# component macro: a bare token that is not a known flag
		comp = self.find_component(tokens)
		if comp is not None:
			self.component(lineno, comp, tokens)
			return
		self.simple(lineno, tokens, rest)

	# -- components -----------------------------------------------------------
	def find_component(self, tokens):
		bare = [k for k, v in tokens if v is None and k not in STRUCT_FLAGS
				and not k.startswith("/")]
		# reject plain flags that are valid on their own
		bare = [k for k in bare if k not in ("NO_ADD_Y", "CENTER_ALIGN", "RIGHT_ALIGN", "OUTLINE")]
		if not bare:
			return None
		# last resolvable bare token wins (BP_QUEST_LINE; BONUS_ITEM; ...)
		for k in reversed(bare):
			if self.resolve_component_name(k) is not None:
				return k
		return bare[0]

	def resolve_component_name(self, name):
		rel = COMPONENT_ALIASES.get(name)
		if rel is None:
			rel = name.lower()
		if self.loader is None:
			return None
		return rel if self.loader(rel) is not None else None

	def component(self, lineno, name, tokens):
		if self.depth >= MAX_COMPONENT_DEPTH:
			self.err(lineno, "component nesting too deep: %s" % name)
			return
		rel = self.resolve_component_name(name)
		if rel is None:
			self.err(lineno, "unknown component: %s" % name)
			return
		params = {}
		for k, v in tokens:
			if v is not None:
				params[k] = v
		sub = _Parser(self.loader, self.doc, self.depth + 1)
		children = sub.feed(self.loader(rel), params=params, base_line=lineno)
		node = {"type": "component", "name": name, "children": children}
		if "X" in params:
			node["x"] = _fold_num(params["X"])
		if "Y" in params:
			node["y"] = _fold_num(params["Y"])
		if "Y_PADDING" in params:
			node["y_pad"] = _fold_num(params["Y_PADDING"])
		if "ANCHOR" in params:
			node["anchor"] = params["ANCHOR"]
		self.nodes.append(node)

	# -- simple tags ------------------------------------------------------------
	def simple(self, lineno, tokens, rest):
		common = _common(tokens)
		made = False

		img = _get(tokens, "IMG")
		if img is not None:
			made = True
			if img.strip():
				n = {"type": "img", "path": img}
				n.update(common)
				self.nodes.append(n)

		item = _get(tokens, "ITEM")
		if item is not None:
			made = True
			if item.strip() and item.strip() != "0":
				n = {"type": "item", "vnum": item.strip(),
					"sockets": ParseSockets(_get(tokens, "SOCKET")),
					"attrs": ParseAttrs(_get(tokens, "ATTR"))}
				n.update(common)
				self.nodes.append(n)

		rt = _get(tokens, "RENDERTARGET")
		if rt is not None:
			made = True
			n = {"type": "rendertarget", "vnum": rt.strip()}
			n.update(common)
			self.nodes.append(n)

		btn = _get(tokens, "BUTTON")
		if btn is not None:
			made = True
			imgs = [p.strip() for p in btn.split("#")]
			while len(imgs) < 3:
				imgs.append(imgs[-1] if imgs else "")
			n = {"type": "button", "images": imgs[:3],
				"link": ParseLinkIndex(_get(tokens, "LINKINDEX")),
				"text": _get(tokens, "TEXT") or "",
				"color_text": ParseColor(_get(tokens, "COLOR_TEXT"), COLOR_TEXT_DEFAULT)}
			n.update(common)
			self.nodes.append(n)
			return

		if _has(tokens, "THINBOARD"):
			n = {"type": "board", "sx": _fold_num(_get(tokens, "SIZE_X") or "0"),
				"sy": _fold_num(_get(tokens, "SIZE_Y") or "0")}
			n.update(common)
			self.nodes.append(n)
			return

		link = _get(tokens, "LINK")
		if link is not None:
			n = {"type": "link", "link": ParseLinkIndex(link),
				"text": _get(tokens, "TEXT") or ""}
			n.update(common)
			self.nodes.append(n)
			return

		if made and not rest:
			return
		self.text_line(lineno, tokens, rest)

	def text_line(self, lineno, tokens, rest):
		text = rest
		t = _get(tokens, "TEXT")
		if t is not None and not text:
			text = t
		if not text:
			return	# format-only line, e.g. "[X=5;Y=1375;FONT_SIZE=13b;NO_ADD_Y]"
		n = {"type": "text", "text": text}
		n.update(_common(tokens))
		if text.startswith("*"):
			n["para"] = True
			n["text"] = text[1:].lstrip()
		self.nodes.append(n)

	# -- tables --------------------------------------------------------------
	def open_table(self, lineno, tokens):
		if self.table is not None:
			self.err(lineno, "nested TABLE_START")
			self.close_table()
		hdr = _get(tokens, "TABLE_HEADERS") or ""
		wid = _get(tokens, "TABLE_WIDTHS") or ""
		widths = []
		for w in wid.split("|"):
			w = w.strip()
			if w:
				fw = _fold_num(w)
				widths.append(fw if fw is not None else 0)
		t = {"type": "table",
			"max_width": _fold_num(_get(tokens, "TABLE_MAX_WIDTH") or "0"),
			"head_height": _fold_num(_get(tokens, "TABLE_HEAD_HEIGHT") or "0"),
			"hide_title": (_get(tokens, "TABLE_HIDE_TITLE") or "0").strip() not in ("", "0"),
			"headers": [h.strip() for h in hdr.split("|")] if hdr else [],
			"widths": widths, "rows": []}
		t.update(_common(tokens))
		self.table = t
		self.row = None

	def close_table(self):
		if self.table is not None:
			self.nodes.append(self.table)
		self.table = None
		self.row = None

	def make_cell(self, tokens, rest):
		c = {"align": (_get(tokens, "ALIGN") or "left").strip().lower()}
		it = _get(tokens, "ITEM")
		if it is not None and it.strip():
			c["item"] = it.strip()
			c["sockets"] = ParseSockets(_get(tokens, "SOCKET"))
			c["attrs"] = ParseAttrs(_get(tokens, "ATTR"))
			cnt = _get(tokens, "COUNT")
			if cnt:
				c["count"] = cnt
		im = _get(tokens, "IMG")
		if im is not None and im.strip():
			c["img"] = im.strip()
		tx = _get(tokens, "TEXT")
		if tx is None and rest:
			tx = rest
		if tx is not None:
			c["text"] = tx
		tip = _get(tokens, "TOOLTIP")
		if tip is not None:
			c["tooltip"] = tip
		col = _get(tokens, "COLOR")
		if col is not None:
			c["color"] = ParseColor(col, COLOR_TEXT_DEFAULT)
		return c


def Parse(text, loader=None, skip_first_line=False):
	"""Parse article/template text -> Document.

	loader(rel) -> template text or None, rel like 'title' or 'skill_tree/skill_tree_item'.
	skip_first_line: reference desc.txt style files ignore their first line.
	"""
	doc = Document()
	p = _Parser(loader, doc)
	doc.nodes = p.feed(text, skip_first_line=skip_first_line)
	return doc


class DictLoader(object):
	"""loader backed by {rel: text}"""
	def __init__(self, mapping):
		self.mapping = mapping

	def __call__(self, rel):
		return self.mapping.get(rel)


class DirLoader(object):
	"""loader reading <root>/<rel>.txt (root = .../wiki/composants). Cached."""
	def __init__(self, root, encoding="cp1254"):
		self.root = root.replace("\\", "/").rstrip("/")
		self.encoding = encoding
		self.cache = {}

	def __call__(self, rel):
		if rel in self.cache:
			return self.cache[rel]
		data = None
		try:
			f = open(self.root + "/" + rel + ".txt", "rb")
			try:
				data = f.read()
			finally:
				f.close()
		except (IOError, OSError):
			data = None
		if data is not None and not isinstance(data, str):
			data = data.decode(self.encoding, "replace")
		self.cache[rel] = data
		return data


# --------------------------------------------------------------------------
# data provider / resolver
# --------------------------------------------------------------------------

class DataProvider(object):
	"""Pluggable server/client data source. Every method returns '' by default.

	Numeric args arrive as int, everything else as str. Return value is
	converted with str() by the resolver (None -> '').
	"""
	def Locale(self, key):
		"""{WK:key} -> text (None = missing)"""
		return None

	def AppendBonus(self, bonus_id):
		return ""

	def AppendBonus2(self, bonus_id, value):
		return ""

	def GetBonus(self, bonus_id):
		return ""

	def HaveBonus(self, bonus_id, value):
		return ""

	def AppendSkill(self, skill_id):
		return ""

	def HaveSkill(self, skill_id, level):
		return ""

	def GetSkillLevel(self, skill_id):
		return ""

	def HaveBuffiSkill(self, index, level):
		return ""

	def GetBuffiSkillLevel(self, index):
		return ""

	def AppendShopItem(self, npc, shop, index):
		return ""

	def AppendShopData(self, npc, shop, index):
		return ""

	def AppendSkillTreeItem(self, tree, index):
		return ""

	def AppendSkillTreeCount(self, tree, index):
		return ""

	def AppendItem(self, vnum):
		return ""

	def AppendBioloData(self, a, b, c):
		return ""

	def AppendTree(self, index):
		return ""

	def AppendPNJ(self, vnum):
		return ""

	def AppendDropItem(self, mob, index):
		return ""

	def AppendDropCount(self, mob, index):
		return ""

	def Roll(self, *args):
		return ""


def _split_args(argstr):
	if not argstr.strip():
		return []
	return [a.strip() for a in _split_depth0(argstr, ",")]


def _to_arg(a):
	return int(a) if _INT_RE.match(a) else a


class Resolver(object):
	"""{WK:..} + data function + escape resolution on strings."""
	def __init__(self, provider=None, locale=None, missing_text=""):
		self.provider = provider or DataProvider()
		self.locale = locale	# optional callable(key)->str|None, tried before provider.Locale
		self.missing_text = missing_text

	def Locale(self, key):
		s = None
		if self.locale is not None:
			s = self.locale(key)
		if s is None:
			s = self.provider.Locale(key)
		return s

	def Text(self, s):
		if s is None:
			return ""
		s = s.replace("\\n", "\n")
		if "{WK:" in s:
			s = self._wk(s)
		if "(" in s:
			s = self._funcs(s)
		return s

	def Number(self, v, default=0):
		"""numeric attribute (int/float or deferred str) -> number"""
		if isinstance(v, (int, float)):
			return v
		if v is None:
			return default
		r = ToNumber(self.Text(v).strip(), None)
		return default if r is None else r

	# -- internals ---------------------------------------------------------
	def _wk(self, s):
		def repl(m):
			parts = _split_depth0(m.group(1), ":")
			key = parts[0].strip()
			args = [self.Text(a.strip()) for a in parts[1:]]
			txt = self.Locale(key)
			if txt is None:
				return self.missing_text
			if args:
				txt = self._format(txt, args)
			return txt
		return _WK_RE.sub(repl, s)

	def _format(self, txt, args):
		for i, a in enumerate(args):
			txt = txt.replace("{%d}" % i, a).replace("$%d" % (i + 1), a)
		if "%s" in txt:
			try:
				txt = txt % tuple(args[:txt.count("%s")])
			except (TypeError, ValueError):
				pass
		return txt

	def _funcs(self, s):
		out = []
		i = 0
		while True:
			m = _FUNC_RE.search(s, i)
			if not m:
				out.append(s[i:])
				break
			out.append(s[i:m.start()])
			j = _match_close(s, m.end() - 1, "(", ")")
			if j < 0:
				out.append(s[m.start():])
				break
			argstr = s[m.end():j]
			args = [self.Text(a) for a in _split_args(argstr)]
			out.append(self._call(m.group(1), args, s[m.start():j + 1]))
			i = j + 1
		return "".join(out)

	def _call(self, name, args, literal):
		if name == "Color":
			return FUNC_COLOR1 + ",".join(args) + "|r"
		if name == "Color2":
			return FUNC_COLOR2 + ",".join(args) + "|r"
		conv = [_to_arg(a) for a in args]
		if name == "Roll" and [a for a in conv if not isinstance(a, int)]:
			return literal	# prose like "Boss Roll(heavy)"
		fn = getattr(self.provider, name, None)
		if fn is None:
			return literal
		try:
			r = fn(*conv)
		except TypeError:
			return literal
		if r is None:
			return ""
		return r if isinstance(r, str) else str(r)


# --------------------------------------------------------------------------
# build layer (ui dependent, imported lazily)
# --------------------------------------------------------------------------

class WikiContext(object):
	"""Everything Build() needs from its host. Override what you need."""
	def __init__(self, provider=None, locale=None, on_link=None, loader=None):
		self.resolver = Resolver(provider, locale)
		self._on_link = on_link
		self.line_extra = 4		# line height = font size + line_extra
		self.default_font_size = 12
		self.text_color = COLOR_TEXT_DEFAULT
		self.table_head_color = 0xFF2A2218
		self.table_row_colors = (0xFF1C1511, 0xFF201813)
		self.table_line_color = 0xFF3B2B22

	# links -------------------------------------------------------------
	def OnLink(self, kind, ident, pos):
		if self._on_link:
			self._on_link(kind, ident, pos)

	# text ----------------------------------------------------------------
	def FontName(self, size, bold):
		"""return a ui font name for the size/bold, or None to keep the default."""
		return None

	def MeasureText(self, text, size, bold):
		"""-> (w, h). Default: rough estimate; hosts should measure with a TextLine."""
		return (int(len(text) * (size * 0.55 + (0.5 if bold else 0))), size + self.line_extra)

	# items / render targets -------------------------------------------------------
	def CreateItem(self, parent, vnum, sockets, attrs, tooltip):
		"""-> (window, w, h) or None. Default: icon ImageBox from the item module."""
		return _default_item(self, parent, vnum, sockets, attrs, tooltip)

	def CreateRenderTarget(self, parent, vnum, w, h):
		"""-> window or None (monster/NPC model render). Host must implement."""
		return None

	def ShowItemTooltip(self, vnum, sockets, attrs):
		pass

	def HideItemTooltip(self):
		pass


def _default_item(ctx, parent, vnum, sockets, attrs, tooltip):
	import ui
	try:
		import item
		v = int(vnum)
		item.SelectItem(v)
		icon = item.GetIconImageFileName()
		try:
			_w, h = item.GetItemSize()
		except Exception:
			h = 1
	except Exception:
		return None
	img = (getattr(ui, "ExpandedImageBox", None) or ui.ImageBox)()
	img.SetParent(parent)
	img.LoadImage(icon)
	img.Show()
	return (img, img.GetWidth(), img.GetHeight())


class BuildResult(object):
	def __init__(self):
		self.height = 0
		self.anchors = {}
		self.windows = []	# keep references!
		self.links = []

	def Destroy(self):
		for w in self.windows:
			try:
				w.Hide()
			except Exception:
				pass
		self.windows = []
		self.links = []


_UI_CLASSES = {}


def _ui_classes():
	"""lazy ui subclasses (needs the game ui module)"""
	if _UI_CLASSES:
		return _UI_CLASSES
	import ui

	class LinkArea(ui.Window):
		def __init__(self, callback):
			ui.Window.__init__(self)
			self.callback = callback

		def OnMouseLeftButtonUp(self):
			if self.callback:
				self.callback()
			return True

	_UI_CLASSES["LinkArea"] = LinkArea
	return _UI_CLASSES


class _Callback(object):
	def __init__(self, fn, *args):
		self.fn = fn
		self.args = args

	def Call(self):
		self.fn(*self.args)


class _Builder(object):
	def __init__(self, parent, ctx, result):
		import ui
		self.ui = ui
		self.parent = parent
		self.ctx = ctx
		self.res = result
		self.r = ctx.resolver

	def _Image(self):
		cls = getattr(self.ui, "ExpandedImageBox", None) or self.ui.ImageBox
		return cls()

	def keep(self, w):
		self.res.windows.append(w)
		return w

	def num(self, v, default=0):
		return int(self.r.Number(v, default))

	def bump(self, bottom):
		if bottom > self.res.height:
			self.res.height = bottom

	# -- driver ------------------------------------------------------------------
	def run(self, nodes, ox, oy, width, cursor=0):
		"""returns the cursor after the last node"""
		for n in nodes:
			t = n["type"]
			fn = getattr(self, "n_" + t, None)
			if fn is None:
				continue
			cursor = fn(n, ox, oy, width, cursor)
		return cursor

	def xoff(self, n):
		return self.num(n.get("x")) + self.num(n.get("x_pad")) + self.num(n.get("x_shift"))

	def pos(self, n, cursor, default_advance_cursor):
		# Y_PADDING = gap above a cursor-placed line; ignored when Y is absolute.
		# Y_SHIFT (from TEXT_n_Y_PADDING of component params) always applies.
		y = n.get("y")
		if y is None:
			base = cursor + self.num(n.get("y_pad"), 0)
		else:
			base = self.num(y)
		return base + self.num(n.get("y_shift"), 0)

	# -- simple nodes ----------------------------------------------------------------
	def n_changey(self, n, ox, oy, width, cursor):
		return self.num(n.get("y"))

	def n_summary(self, n, ox, oy, width, cursor):
		return cursor

	def n_banner(self, n, ox, oy, width, cursor):
		img = self.keep(self._Image())
		img.SetParent(self.parent)
		img.LoadImage(n["path"])
		img.SetPosition(ox, oy)
		img.Show()
		self.bump(oy + img.GetHeight())
		return cursor + img.GetHeight()

	def n_img(self, n, ox, oy, width, cursor):
		img = self.keep(self._Image())
		img.SetParent(self.parent)
		path = self.r.Text(n["path"]).strip()
		if not path:
			return cursor
		img.LoadImage(path)
		w, h = img.GetWidth(), img.GetHeight()
		x = self.xoff(n)
		y = self.pos(n, cursor, False)
		if n.get("align") == "center":
			x = (width - w) // 2 + x
		elif n.get("align") == "right":
			x = width - w - x
		img.SetPosition(ox + x, oy + y)
		img.Show()
		self.bump(oy + y + h)
		self.res.windows.append(img)
		return cursor

	def n_item(self, n, ox, oy, width, cursor):
		vnum = self.r.Text(n["vnum"]).strip()
		if not _INT_RE.match(vnum) or vnum == "0":
			return cursor
		made = self.ctx.CreateItem(self.parent, int(vnum), n.get("sockets"), n.get("attrs"),
			n.get("tooltip"))
		if not made:
			return cursor
		win, w, h = made
		x = self.xoff(n)
		y = self.pos(n, cursor, False)
		if n.get("align") == "center":
			x = (width - w) // 2 + x
		win.SetPosition(ox + x, oy + y)
		win.Show()
		self.keep(win)
		self.bump(oy + y + h)
		return cursor

	def n_rendertarget(self, n, ox, oy, width, cursor):
		vnum = self.r.Text(n["vnum"]).strip()
		if not _INT_RE.match(vnum):
			return cursor
		w, h = self.num(n.get("w")), self.num(n.get("h"))
		win = self.ctx.CreateRenderTarget(self.parent, int(vnum), w, h)
		x = self.num(n.get("x"))
		y = self.pos(n, cursor, False)
		if win is not None:
			win.SetPosition(ox + x, oy + y)
			win.Show()
			self.keep(win)
		self.bump(oy + y + h)
		return cursor

	def n_board(self, n, ox, oy, width, cursor):
		b = self.keep(self.ui.ThinBoard())
		b.SetParent(self.parent)
		sx, sy = self.num(n.get("sx")), self.num(n.get("sy"))
		x = self.num(n.get("x"))
		y = self.pos(n, cursor, False)
		b.SetSize(sx, sy)
		b.SetPosition(ox + x, oy + y)
		b.Show()
		self.bump(oy + y + sy)
		return cursor

	def n_button(self, n, ox, oy, width, cursor):
		b = self.keep(self.ui.Button())
		b.SetParent(self.parent)
		up, over, down = [self.r.Text(p) for p in n["images"]]
		b.SetUpVisual(up)
		b.SetOverVisual(over)
		b.SetDownVisual(down)
		text = self.r.Text(n.get("text"))
		if text:
			b.SetText(text)
		w, h = b.GetWidth(), b.GetHeight()
		x = self.xoff(n)
		y = self.pos(n, cursor, False)
		if n.get("align") == "center":
			x = (width - w) // 2 + x
		elif n.get("align") == "right":
			x = width - w - x
		b.SetPosition(ox + x, oy + y)
		kind, ident, pos = n["link"]
		if kind:
			cb = _Callback(self.ctx.OnLink, kind, self.r.Text(ident), pos)
			self.res.links.append(cb)
			b.SetEvent(self.ui.__mem_func__(cb.Call))
		b.Show()
		self.bump(oy + y + h)
		return cursor

	def n_link(self, n, ox, oy, width, cursor):
		kind, ident, pos = n["link"]
		text = self.r.Text(n.get("text"))
		size = n.get("font_size", self.ctx.default_font_size)
		tl = self.keep(self.ui.TextLine())
		tl.SetParent(self.parent)
		tl.SetText(text)
		self._style(tl, n, size, n.get("bold"))
		x = self.xoff(n)
		y = self.pos(n, cursor, False)
		tl.SetPosition(ox + x, oy + y)
		tl.Show()
		w, h = self.ctx.MeasureText(StripCodes(text), size, n.get("bold"))
		area = self.keep(_ui_classes()["LinkArea"](None))
		cb = _Callback(self.ctx.OnLink, kind, self.r.Text(ident), pos)
		self.res.links.append(cb)
		area.callback = cb.Call
		area.SetParent(self.parent)
		area.SetPosition(ox + x, oy + y)
		area.SetSize(w, h)
		area.Show()
		self.bump(oy + y + h)
		return cursor

	# -- text --------------------------------------------------------------------------
	def _style(self, tl, n, size, bold):
		fn = self.ctx.FontName(size, bold)
		if fn and hasattr(tl, "SetFontName"):
			tl.SetFontName(fn)
		color = n.get("color")
		if color is None:
			color = self.ctx.text_color
		tl.SetPackedFontColor(color)
		if n.get("outline") and hasattr(tl, "SetOutline"):
			tl.SetOutline()

	def make_lines(self, text, maxw, size, bold):
		meas = lambda s: self.ctx.MeasureText(s, size, bold)[0]
		return WrapText(text, maxw, meas)

	def n_text(self, n, ox, oy, width, cursor):
		text = self.r.Text(n["text"])
		size = n.get("font_size", self.ctx.default_font_size)
		bold = n.get("bold", False)
		lh = size + self.ctx.line_extra
		x = self.xoff(n)
		y = self.pos(n, cursor, True)
		align = n.get("align")
		maxw = self.num(n.get("max_width"), 0)
		if not maxw:
			maxw = width - x - 4 if align is None else width - 8
		indent = 0
		if n.get("para"):
			text = "- " + text
			indent = 0
		lines = self.make_lines(text, maxw, size, bold)
		cy = y
		for i, ln in enumerate(lines):
			tl = self.keep(self.ui.TextLine())
			tl.SetParent(self.parent)
			tl.SetText(ln)
			self._style(tl, n, size, bold)
			if align == "center":
				tl.SetPosition(ox + width // 2 + x, oy + cy)
				tl.SetHorizontalAlignCenter()
			elif align == "right":
				tl.SetPosition(ox + width - x, oy + cy)
				tl.SetHorizontalAlignRight()
			else:
				tl.SetPosition(ox + x + (indent if i else 0), oy + cy)
			tl.Show()
			cy += lh
		self.bump(oy + cy)
		if n.get("no_add_y"):
			return cursor
		return cy

	# -- components -------------------------------------------------------------------------
	def n_component(self, n, ox, oy, width, cursor):
		x = self.num(n.get("x"))
		y = self.num(n["y"]) if n.get("y") is not None else cursor
		if n.get("anchor"):
			self.res.anchors[self.r.Text(n["anchor"])] = oy + y
		before = self.res.height
		self.res.height = 0
		self.run(n["children"], ox + x, oy + y, width - x, 0)
		sub_h = self.res.height - (oy + y) if self.res.height else 0
		self.res.height = max(before, self.res.height)
		if sub_h < 0:
			sub_h = 0
		return y + sub_h + self.num(n.get("y_pad"))

	# -- tables ---------------------------------------------------------------------------------
	def n_table(self, n, ox, oy, width, cursor):
		ui = self.ui
		ctx = self.ctx
		x = ox + self.num(n.get("x"))
		y = oy + (self.num(n["y"]) if n.get("y") is not None else cursor)
		widths = [self.num(w) for w in n["widths"]]
		maxw = self.num(n.get("max_width"))
		if not widths:
			cols = max([len(r["cells"]) for r in n["rows"]] + [len(n["headers"]), 1])
			widths = [(maxw or width) // cols] * cols
		elif len(widths) == 1 and maxw > widths[0]:
			# Reference articles use TABLE_MAX_WIDTH for one-column notice rows.
			widths[0] = maxw
		total = sum(widths)
		cur = y
		head_h = self.num(n.get("head_height"))
		if head_h and not n["hide_title"] and n["headers"]:
			self._bar(x, cur, total, head_h, ctx.table_head_color)
			cx = x
			for i, h in enumerate(n["headers"]):
				if i >= len(widths):
					break
				self._cell_text(self.r.Text(h), cx, cur, widths[i], head_h, "center", 13, True,
					COLOR_GOLD)
				cx += widths[i]
			cur += head_h
		for ri, row in enumerate(n["rows"]):
			rh = self.num(row["h"])
			self._bar(x, cur, total, rh, ctx.table_row_colors[ri % 2])
			if ri > 0 and len(widths) > 1:
				self._bar(x, cur, total, 1, ctx.table_line_color)
			cx = x
			for ci, cell in enumerate(row["cells"]):
				if ci >= len(widths):
					break
				self._cell(cell, cx, cur, widths[ci], rh)
				cx += widths[ci]
			cur += rh
		if len(widths) > 1:
			cx = x
			for col_width in widths[:-1]:
				cx += col_width
				self._bar(cx, y, 1, cur - y, ctx.table_line_color)
		# 1px frame
		for (bx, by, bw, bh) in ((x, y, total, 1), (x, cur - 1, total, 1),
								(x, y, 1, cur - y), (x + total - 1, y, 1, cur - y)):
			self._bar(bx, by, bw, bh, ctx.table_line_color)
		self.bump(cur)
		return cur - oy

	def _bar(self, x, y, w, h, color):
		b = self.keep(self.ui.Bar())
		b.SetParent(self.parent)
		b.SetPosition(x, y)
		b.SetSize(w, h)
		b.SetColor(color)
		b.Show()

	def _cell_text(self, text, cx, cy, cw, ch, align, size, bold, color):
		lines = self.make_lines(text, cw - 8, size, bold)
		lh = size + self.ctx.line_extra
		ty = cy + (ch - lh * len(lines)) // 2
		for ln in lines:
			tl = self.keep(self.ui.TextLine())
			tl.SetParent(self.parent)
			tl.SetText(ln)
			self._style(tl, {"color": color}, size, bold)
			if align == "center":
				tl.SetPosition(cx + cw // 2, ty)
				tl.SetHorizontalAlignCenter()
			elif align == "right":
				tl.SetPosition(cx + cw - 4, ty)
				tl.SetHorizontalAlignRight()
			else:
				tl.SetPosition(cx + 4, ty)
			tl.Show()
			ty += lh

	def _cell(self, cell, cx, cy, cw, ch):
		if cell.get("item"):
			vnum = self.r.Text(cell["item"]).strip()
			if _INT_RE.match(vnum) and vnum != "0":
				made = self.ctx.CreateItem(self.parent, int(vnum), cell.get("sockets"),
					cell.get("attrs"), cell.get("tooltip"))
				if made:
					win, w, h = made
					win.SetPosition(cx + (cw - w) // 2, cy + (ch - h) // 2)
					win.Show()
					self.keep(win)
			return
		if cell.get("img"):
			img = self.keep(self._Image())
			img.SetParent(self.parent)
			img.LoadImage(self.r.Text(cell["img"]))
			img.SetPosition(cx + (cw - img.GetWidth()) // 2, cy + (ch - img.GetHeight()) // 2)
			img.Show()
			return
		txt = self.r.Text(cell.get("text", ""))
		if txt:
			col = cell.get("color")
			self._cell_text(txt, cx, cy, cw, ch, cell.get("align", "left"), 12, False,
				self.ctx.text_color if col is None else col)


def Build(parent, doc, width, ctx=None, x=0, y=0):
	"""Create the ui windows for `doc` inside `parent`. Returns BuildResult.

	result.height  : total content height (for the scroll range)
	result.anchors : {anchor_name: y}
	Keep `result` referenced as long as the windows should live.
	"""
	if ctx is None:
		ctx = WikiContext()
	res = BuildResult()
	b = _Builder(parent, ctx, res)
	b.run(doc.nodes if isinstance(doc, Document) else doc, x, y, width, 0)
	res.height = max(0, res.height - y)
	return res
