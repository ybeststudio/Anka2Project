# -*- coding: utf-8 -*-
"""
wikicontent.py -- Wiki content layer (data only, no UI). Python 2.7, ASCII source.

ARCHITECTURE NOTE
-----------------
Content lives in the "locale" pack under archive prefix  locale/common/ :

	locale/common/locale_wiki.json                  id -> {lang: text}   (826 ids, en/tr/de/es/fr/it/pl/pt/ro/...)
	locale/common/config_json/wiki_categories.json  {"articles": [...], "categories": [...]}
	locale/common/config_json/wiki_quest_rewards.json
	locale/common/config_json/{beginner,guild,mount_runes,skill_tree,guideline}.json
	locale/common/wiki/**.txt                       article markup (language neutral; text via {WK:id})

The client has NO json module, so a tiny pure-python JSON parser is embedded here.
Everything returned is a utf-8 encoded `str` (client codepage 65001). Legacy articles
stored as cp1252 are transcoded to utf-8 on load. CRLF is normalised to LF.
All loaders are lazy and cached; ReloadAll() drops the caches.

PUBLIC API
----------
GetLangCode()                -> 'tr' | 'en' | 'de' ...  (app.GetLocaleName(), fallback 'en')
SetLangCode(code)            -> override (tests / language switch), clears text caches
GetLangChain()               -> [lang, 'en']  (lookup order)

ReadFile(path)               -> str | None   (pack.Get, falls back to open() outside the client)
ReadCommonFile(rel)          -> str | None   ("locale/common/" + rel)
LoadJson(path)               -> object | None  (cached)
LoadCommonJson(rel)          -> object | None  (e.g. "config_json/mount_runes.json")

Tr(key, default=None)        -> localised text. key: int / "429" -> locale_wiki.json;
                                "WIKI_FOO" -> localeInfo.WIKI_FOO; else default or key.
                                Literal backslash-n sequences become real newlines.
ExpandWK(text)               -> replaces every {WK:id} with Tr(id)

GetCategories()              -> [ {id, name, title, type, btn, items:[{id, name, title, file}]} ]
GetCategory(catId)           -> one category dict | None
GetArticles()                -> [ {id, file} ]
GetArticleFile(id, part=0)   -> "wiki-relative" path e.g. "systems/pets.txt" | None.
                                Category items with "{}" get `part` substituted
                                (dungeon pages 0..3 and "enigma"). Searches articles, then category items.
GetArticleTitle(id)          -> title of the category item (Tr) or ""
LoadArticleText(file, part=0)-> article markup (LF, utf-8, {WK:id} NOT expanded) | None
                                `file` is wiki-relative or an article id. Language folder
                                wiki/<lang>/<file> wins over wiki/<file> (optional overrides).
LoadArticle(id_or_file, part=0) -> same as LoadArticleText but with {WK:id} expanded
GetQuestRewards()            -> parsed wiki_quest_rewards.json
IsKnownArticle(id)           -> bool
SearchArticles(query, limit) -> [(id, title)]  localised title substring match
                                (id -> "System#<id>" wiki argument). Used by the
                                search box so guide/system pages are searchable too.
"""

import os
import re
import sys

COMMON = "locale/common/"
WIKI_DIR = COMMON + "wiki/"
CATEGORY_JSON = "config_json/wiki_categories.json"
LOCALE_JSON = "locale_wiki.json"
STRINGS_JSON = "wiki_strings.json"
QUEST_REWARDS_JSON = "config_json/wiki_quest_rewards.json"

SUPPORTED_LANGS = ("en", "tr", "de", "es", "fr", "it", "pl", "pt", "ro", "hu", "cz", "ae")
LANG_ALIASES = {
	"turkey": "tr", "turkish": "tr", "english": "en", "german": "de", "deutsch": "de",
	"spanish": "es", "french": "fr", "italian": "it", "polish": "pl", "portuguese": "pt",
	"romanian": "ro", "hungarian": "hu", "czech": "cz", "cs": "cz", "arabic": "ae", "ar": "ae",
	"ymir": "en", "": "en",
}

_langOverride = None
_cacheFile = {}
_cacheJson = {}
_cacheText = {}
_cacheCats = [None]

########################################################################
# language
########################################################################

_langCache = [None]

def GetLangCode():
	if _langOverride:
		return _langOverride
	if _langCache[0] is not None:
		return _langCache[0]
	code = _DetectLang()
	_langCache[0] = code
	return code

def _DetectLang():
	code = ""
	try:
		import app
		code = app.GetLocaleName()
	except:
		code = ""
	code = (code or "").strip().lower()
	code = LANG_ALIASES.get(code, code)
	if code not in SUPPORTED_LANGS:
		code = "en"
	return code

def SetLangCode(code):
	global _langOverride
	if code:
		code = code.strip().lower()
		code = LANG_ALIASES.get(code, code)
		if code not in SUPPORTED_LANGS:
			code = "en"
	_langOverride = code or None
	_cacheText.clear()
	_cacheCats[0] = None

def GetLangChain():
	lang = GetLangCode()
	if lang == "en":
		return ["en"]
	return [lang, "en"]

########################################################################
# file access
########################################################################

_diag = []

def Note(msg):
	# kalici teshis: ilk 60 kayit, dbg.TraceError ile syserr/log'a da yazilir
	if len(_diag) < 60:
		_diag.append(msg)
		try:
			import dbg
			dbg.TraceError("WIKIDIAG " + msg)
		except:
			pass

def Diagnose():
	return "\n".join(_diag)

def ReadFile(path):
	if path in _cacheFile:
		return _cacheFile[path]
	data = None
	try:
		import pack
		exist = pack.Exist(path)
		if exist:
			data = pack.Get(path)
		# The pack bridge can report an entry as present but return empty data.
		if not data and (path.find("config_json") != -1 or path.find("landingpage") != -1 or path.endswith("locale_wiki.json")):
			Note("ReadFile %s exist=%s len=%s" % (path, str(exist), str(len(data or ""))))
	except:
		Note("ReadFile pack error %s: %s" % (path, str(sys.exc_info()[:2])))
		data = None
	if not data:
		for diskPath in (path, os.path.join(os.path.dirname(sys.executable), path)):
			try:
				f = open(diskPath, "rb")
				try:
					data = f.read()
				finally:
					f.close()
				if data:
					break
			except:
				data = None
	_cacheFile[path] = data
	return data

def ReadCommonFile(rel):
	return ReadFile(COMMON + rel)

def _Utf8Points(data):
	"""Decode UTF-8 bytes without the stripped client's codec registry."""
	i = 0
	while i < len(data):
		first = ord(data[i])
		if first < 0x80:
			value, width, minimum = first, 1, 0
		elif 0xC2 <= first <= 0xDF:
			value, width, minimum = first & 0x1F, 2, 0x80
		elif 0xE0 <= first <= 0xEF:
			value, width, minimum = first & 0x0F, 3, 0x800
		elif 0xF0 <= first <= 0xF4:
			value, width, minimum = first & 0x07, 4, 0x10000
		else:
			raise ValueError("invalid UTF-8 lead byte")
		if i + width > len(data):
			raise ValueError("truncated UTF-8 sequence")
		for j in range(1, width):
			continuation = ord(data[i + j])
			if continuation < 0x80 or continuation > 0xBF:
				raise ValueError("invalid UTF-8 continuation")
			value = (value << 6) | (continuation & 0x3F)
		if value < minimum or 0xD800 <= value <= 0xDFFF or value > 0x10FFFF:
			raise ValueError("invalid UTF-8 codepoint")
		yield value
		i += width

def _ToUtf8(data):
	if data is None:
		return None
	if data.startswith("\xef\xbb\xbf"):
		data = data[3:]
	try:
		for codepoint in _Utf8Points(data):
			pass
		return data
	except ValueError:
		# client lib'inde cp1252/latin-1 codec YOK (encodings: sadece cp949): elle latin-1 -> utf-8
		out = []
		for ch in data:
			o = ord(ch)
			if o < 128:
				out.append(ch)
			else:
				out.append(chr(0xC0 | (o >> 6)) + chr(0x80 | (o & 0x3F)))
		return "".join(out)

########################################################################
# utf-8 -> client codepage (Anka client = cp1254 for tr, cp1252 otherwise).
# Client lib has no cp1254/cp1252 codec, so the mapping is done by hand.
########################################################################

_EXTRA_1252 = {
	0x20AC: 0x80, 0x201A: 0x82, 0x0192: 0x83, 0x201E: 0x84, 0x2026: 0x85, 0x2020: 0x86,
	0x2021: 0x87, 0x02C6: 0x88, 0x2030: 0x89, 0x0160: 0x8A, 0x2039: 0x8B, 0x0152: 0x8C,
	0x2018: 0x91, 0x2019: 0x92, 0x201C: 0x93, 0x201D: 0x94, 0x2022: 0x95, 0x2013: 0x96,
	0x2014: 0x97, 0x02DC: 0x98, 0x2122: 0x99, 0x0161: 0x9A, 0x203A: 0x9B, 0x0153: 0x9C,
	0x0178: 0x9F,
}
_TR_BLOCK = (0xD0, 0xDD, 0xDE, 0xF0, 0xFD, 0xFE)
_TR_MAP = {0x011E: 0xD0, 0x0130: 0xDD, 0x015E: 0xDE, 0x011F: 0xF0, 0x0131: 0xFD, 0x015F: 0xFE}
_tables = {}

def _Table():
	lang = GetLangCode()
	t = _tables.get(lang)
	if t is not None:
		return t
	t = {}
	for c in range(0xA0, 0x100):
		t[c] = c
	t.update(_EXTRA_1252)
	if lang == "tr":
		for c in _TR_BLOCK:
			del t[c]
		t.update(_TR_MAP)
	_tables[lang] = t
	return t

def _CharToClient(u):
	c = ord(u)
	if c < 128:
		return chr(c)
	return chr(_Table().get(c, 63))

def _Utf8ToClient(text):
	if text is None:
		return None
	table = _Table()
	try:
		return "".join(chr(c if c < 128 else table.get(c, 63)) for c in _Utf8Points(text))
	except ValueError:
		return text

########################################################################
# minimal JSON parser (client has no json module)
########################################################################

_WS = re.compile(r'[ \t\r\n]*')
_STR = re.compile(r'"((?:[^"\\]|\\.)*)"', re.S)
_NUM = re.compile(r'-?(?:0|[1-9][0-9]*)(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?')
_ESC = re.compile(r'\\(u[0-9a-fA-F]{4}|.)', re.S)
_PAIR = re.compile(r'\\u([dD][89abAB][0-9a-fA-F]{2})\\u([dD][c-fC-F][0-9a-fA-F]{2})')
_SIMPLE = {'"': '"', '\\': '\\', '/': '/', 'b': '\b', 'f': '\f', 'n': '\n', 'r': '\r', 't': '\t'}

def _PairSub(m):
	hi = int(m.group(1), 16)
	lo = int(m.group(2), 16)
	cp = 0x10000 + ((hi - 0xD800) << 10) + (lo - 0xDC00)
	return "".join(["\\U%08x" % cp])

def _EscSub(m):
	g = m.group(1)
	if g in _SIMPLE:
		return _SIMPLE[g]
	if g[0] == 'u':
		return _CharToClient(unichr(int(g[1:], 16)))
	return g

def _DecodeString(raw):
	if "\\" not in raw:
		return raw
	if "\\u" in raw:
		raw = _PAIR.sub(_PairSub, raw)
		# \Uxxxxxxxx (astral) handled by splitting
		parts = re.split(r'(\\U[0-9a-f]{8})', raw)
		out = []
		for p in parts:
			if p[:2] == "\\U" and len(p) == 10:
				out.append("?")
			else:
				out.append(_ESC.sub(_EscSub, p))
		return "".join(out)
	return _ESC.sub(_EscSub, raw)

def ParseJson(text):
	"""Parse JSON text (utf-8 str). Strings come back as utf-8 str, objects as dict."""
	text = _Utf8ToClient(_ToUtf8(text))
	pos = [0]
	n = len(text)

	def ws():
		pos[0] = _WS.match(text, pos[0]).end()

	def value():
		ws()
		c = text[pos[0]]
		if c == '{':
			pos[0] += 1
			obj = {}
			ws()
			if text[pos[0]] == '}':
				pos[0] += 1
				return obj
			while True:
				ws()
				m = _STR.match(text, pos[0])
				if not m:
					raise ValueError("json key expected at %d" % pos[0])
				key = _DecodeString(m.group(1))
				pos[0] = m.end()
				ws()
				if text[pos[0]] != ':':
					raise ValueError("json ':' expected at %d" % pos[0])
				pos[0] += 1
				obj[key] = value()
				ws()
				c2 = text[pos[0]]
				pos[0] += 1
				if c2 == '}':
					return obj
				if c2 != ',':
					raise ValueError("json ',' expected at %d" % pos[0])
		if c == '[':
			pos[0] += 1
			arr = []
			ws()
			if text[pos[0]] == ']':
				pos[0] += 1
				return arr
			while True:
				arr.append(value())
				ws()
				c2 = text[pos[0]]
				pos[0] += 1
				if c2 == ']':
					return arr
				if c2 != ',':
					raise ValueError("json ',' expected at %d" % pos[0])
		if c == '"':
			m = _STR.match(text, pos[0])
			if not m:
				raise ValueError("json string error at %d" % pos[0])
			pos[0] = m.end()
			return _DecodeString(m.group(1))
		if text.startswith("true", pos[0]):
			pos[0] += 4
			return True
		if text.startswith("false", pos[0]):
			pos[0] += 5
			return False
		if text.startswith("null", pos[0]):
			pos[0] += 4
			return None
		m = _NUM.match(text, pos[0])
		if not m:
			raise ValueError("json value error at %d" % pos[0])
		pos[0] = m.end()
		s = m.group(0)
		if "." in s or "e" in s or "E" in s:
			return float(s)
		return int(s)

	return value()

def LoadJson(path):
	if path in _cacheJson:
		return _cacheJson[path]
	obj = None
	data = ReadFile(path)
	if data is not None:
		try:
			obj = ParseJson(data)
		except Exception, e:
			try:
				import dbg
				dbg.TraceError("wikicontent.LoadJson(%s) failed: %s" % (path, e))
			except:
				pass
			obj = None
	_cacheJson[path] = obj
	return obj

def LoadCommonJson(rel):
	return LoadJson(COMMON + rel)

########################################################################
# translation
########################################################################

def Tr(key, default=None):
	if isinstance(key, (int, long)):
		key = str(key)
	if key is None:
		return default or ""
	text = None
	if key.isdigit():
		table = LoadCommonJson(LOCALE_JSON)
		entry = table.get(key) if table else None
		if entry:
			for lang in GetLangChain():
				if lang in entry and entry[lang]:
					text = entry[lang]
					break
			if text is None:
				for v in entry.values():
					if v:
						text = v
						break
	else:
		# referans locale (wiki_strings.json: key -> {lang: text}) once, sonra localeInfo.<KEY>
		strings = LoadCommonJson(STRINGS_JSON)
		entry = strings.get(key) if strings else None
		if entry:
			for lang in GetLangChain():
				if lang in entry and entry[lang]:
					text = entry[lang]
					break
		if text is None:
			try:
				import localeInfo
				text = getattr(localeInfo, key, None)
			except:
				text = None
		if text is not None and not isinstance(text, str):
			text = None
	if text is None:
		if default is not None:
			return default
		return key
	return text.replace("\\n", "\n")

_WK = re.compile(r'\{WK:(\d+)\}')

def ExpandWK(text):
	if not text:
		return text
	return _WK.sub(lambda m: Tr(m.group(1)), text)

########################################################################
# categories / articles
########################################################################

def _Cats():
	if _cacheCats[0] is not None:
		return _cacheCats[0]
	raw = LoadCommonJson(CATEGORY_JSON) or {}
	cats = []
	for c in raw.get("categories", []):
		items = []
		for it in c.get("items", []):
			items.append({"id": it.get("id"), "name": it.get("name", ""),
				"title": Tr(it.get("name", "")), "file": it.get("file", "")})
		cats.append({"id": c.get("id"), "name": c.get("name", ""), "title": Tr(c.get("name", "")),
			"type": c.get("type", ""), "btn": c.get("btn", ""), "items": items})
	_cacheCats[0] = (cats, raw.get("articles", []))
	return _cacheCats[0]

def GetCategories():
	value = _Cats()
	if isinstance(value, tuple):
		return value[0]
	return value

def GetArticles():
	value = _Cats()
	if isinstance(value, tuple):
		return value[1]
	return []

def GetCategory(catId):
	for c in GetCategories():
		if c["id"] == catId:
			return c
	return None

def _FindFile(articleId):
	for a in GetArticles():
		if a.get("id") == articleId:
			return a.get("file"), ""
	for c in GetCategories():
		for it in c["items"]:
			if it["id"] == articleId:
				return it["file"], it["title"]
	return None, ""

def GetArticleFile(articleId, part=0):
	f, title = _FindFile(articleId)
	if f is None:
		return None
	if "{}" in f:
		f = f.replace("{}", str(part))
	return f

def GetArticleTitle(articleId):
	return _FindFile(articleId)[1]

def IsKnownArticle(articleId):
	return _FindFile(articleId)[0] is not None

def LoadArticleText(fileOrId, part=0):
	if isinstance(fileOrId, (int, long)):
		fileOrId = GetArticleFile(fileOrId, part)
		if fileOrId is None:
			return None
	rel = fileOrId.replace("\\", "/")
	key = (GetLangCode(), rel)
	if key in _cacheText:
		return _cacheText[key]
	data = None
	for lang in GetLangChain():
		data = ReadFile(WIKI_DIR + lang + "/" + rel)
		if data is not None:
			break
	if data is None:
		data = ReadFile(WIKI_DIR + rel)
	data = _Utf8ToClient(_ToUtf8(data))
	if data is not None:
		data = data.replace("\r\n", "\n").replace("\r", "\n")
	_cacheText[key] = data
	return data

def LoadArticle(fileOrId, part=0):
	return ExpandWK(LoadArticleText(fileOrId, part))

def GetQuestRewards():
	return LoadCommonJson(QUEST_REWARDS_JSON)

def _FoldText(s):
	# case-insensitive fold for matching; .lower() only folds ASCII under the
	# client codepage (tr bytes stay as-is), which is fine for substring search.
	if s is None:
		return ""
	try:
		return s.lower()
	except Exception:
		return s

def SearchArticles(query, limit=8):
	"""Substring match of `query` against localised article titles.

	Returns [(id, title)] where id maps to the "System#<id>" wiki argument.
	Case-insensitive, de-duplicated, shortest title first (best prefix match on top).
	Scans wiki_categories.json category items (the pages reachable from the tree);
	the bare `articles` list carries no titles so it is skipped.
	"""
	if query is None:
		return []
	q = _FoldText(query).strip()
	if not q:
		return []
	out = []
	seen = {}
	for cat in GetCategories():
		for it in cat.get("items", []):
			aid = it.get("id")
			if aid is None or aid in seen:
				continue
			title = it.get("title") or it.get("name") or ""
			# match the localised title only: matching the raw locale KEY would make
			# generic words ("system", "wiki", "names") hit every WIKI_* entry.
			if title and q in _FoldText(title):
				seen[aid] = True
				out.append((aid, title))
	out.sort(key=lambda e: (len(e[1]), e[1]))
	return out[:limit]

def ReloadAll():
	_cacheFile.clear()
	_cacheJson.clear()
	_cacheText.clear()
	_cacheCats[0] = None
