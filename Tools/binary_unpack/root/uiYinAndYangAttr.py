if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import chat
import item
import snd
import wndMgr
import localeInfo
import ui
import uiCommon
import uiToolTip
import mouseModule
import constInfo
import yinAndYangAttr

class Util(object):
	ATTACH_ATTR_ITEM_GAUGE_IMG_PATH_EMPTY = {
		1: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_empty_1.sub",
		2: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_empty_2.sub",
		3: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_empty_3.sub"
	}
	ATTACH_ATTR_ITEM_GAUGE_IMG_PATH_YANG = {
		1: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yang_1.sub",
		2: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yang_2.sub",
		3: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yang_3.sub"
	}
	ATTACH_ATTR_ITEM_GAUGE_IMG_PATH_YIN_GLOVE = {
		1: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_glove_1.sub",
		2: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_glove_2.sub",
		3: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_glove_3.sub"
	}
	ATTACH_ATTR_ITEM_GAUGE_IMG_PATH_YIN_WEAPON = {
		1: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_weapon_1.sub",
		2: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_weapon_2.sub",
		3: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/attach_attr_gauge_yin_weapon_3.sub"
	}

	EQUIPMENT_INDEX_TO_WEARABLE_FLAG = {
		0: 1,
		1: 2,
		2: 4,
		4: 16,
		11: 4096,
	}

	YIN_AND_YANG_ATTR_APPLICABLE_PART_TEXT = {}
	YIN_AND_YANG_ATTR_KEYWORD_TEXT = {}
	YIN_AND_YANG_ATTR_KEYWORD_DESC_TEXT = {
		0: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_supernatural.txt",
		1: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_cast.txt",
		2: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_provoke.txt",
		3: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_backattack.txt",
		4: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_talisman.txt",
		5: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_revive.txt",
		6: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_smite.txt",
		7: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_occupy.txt",
		8: "locale/tr/yin_and_yang_attr/guide_book_desc_keyword_berserk.txt",
	}

	@classmethod
	def InitializeLoca(cls):
		cls.YIN_AND_YANG_ATTR_KEYWORD_TEXT = {
			0: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_SUPERNATURAL,
			1: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_CAST,
			2: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_PROVOKE,
			3: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_BACK_ATTACK,
			4: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_TALISMAN,
			5: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_REVIVE,
			6: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_SMITE,
			7: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_OCCUPY,
			8: localeInfo.YIN_AND_YANG_ATTR_KEYWORD_BERSERK,
		}
		cls.YIN_AND_YANG_ATTR_APPLICABLE_PART_TEXT = {
			1: localeInfo.YIN_AND_YANG_ATTR_APPLICABLE_PART_BODY,
			2: localeInfo.YIN_AND_YANG_ATTR_APPLICABLE_PART_HEAD,
			4: localeInfo.YIN_AND_YANG_ATTR_APPLICABLE_PART_FOOTS,
			16: localeInfo.YIN_AND_YANG_ATTR_APPLICABLE_PART_WEAPON,
			4096: localeInfo.YIN_AND_YANG_ATTR_APPLICABLE_PART_GLOVE,
		}

	@classmethod
	def GetYinAndYangAttrIconImagePath(cls, attr_type, wear_flag):
		if attr_type == 1:
			return "d:/ymir work/ui/game/yin_and_yang_attr/icon_yang_attr.sub"
		elif attr_type == 0:
			if wear_flag == 4096:
				return "d:/ymir work/ui/game/yin_and_yang_attr/icon_yin_attr_glove.sub"
			else:
				return "d:/ymir work/ui/game/yin_and_yang_attr/icon_yin_attr_weapon.sub"
		return ""

	@classmethod
	def GetApplicableWearFlag(cls, attr_vnum):
		return yinAndYangAttr.GetYinAndYangAttrApplicableWearFlag(attr_vnum)

	@classmethod
	def GetFormattedDescText(cls, attr_vnum, attr_level):
		lines = yinAndYangAttr.GetYinAndYangAttrDescText(attr_vnum, attr_level)
		if lines:
			return "\n".join(lines)
		return ""

	@classmethod
	def GetYinAndYangAttrAttachedItemToolTipInfo(cls, item_vnum, attr_vnum, attr_level):
		if not attr_vnum:
			return None
		attr_type = yinAndYangAttr.GetYinAndYangAttrType(attr_vnum)
		wear_flag = cls.GetApplicableWearFlag(attr_vnum)
		icon_path = cls.GetYinAndYangAttrIconImagePath(attr_type, wear_flag)
		formatted_text = cls.GetFormattedDescText(attr_vnum, attr_level)
		return (attr_type, icon_path, formatted_text)

	@classmethod
	def MakeYinAndYangAttrTooltip(cls, tooltip, attr_vnum, attr_level, remain_time = 0):
		if not tooltip or not attr_vnum:
			return
		attr_type = yinAndYangAttr.GetYinAndYangAttrType(attr_vnum)
		wear_flag = cls.GetApplicableWearFlag(attr_vnum)
		keyword = yinAndYangAttr.GetYinAndYangAttrKeyword(attr_vnum)
		icon_path = cls.GetYinAndYangAttrIconImagePath(attr_type, wear_flag)

		tooltip.AppendSpace(5)
		if attr_type == 0:
			tooltip.AppendTextLine(localeInfo.YIN_AND_YANG_ATTR_TYPE_YIN, 0xFF4AE2E2)
		else:
			tooltip.AppendTextLine(localeInfo.YIN_AND_YANG_ATTR_TYPE_YANG, 0xFFFFA020)

		if keyword in cls.YIN_AND_YANG_ATTR_KEYWORD_TEXT:
			tooltip.AppendTextLine("%s: %s" % (localeInfo.YIN_AND_YANG_ATTR_KEYWORD, cls.YIN_AND_YANG_ATTR_KEYWORD_TEXT[keyword]), 0xFFE0E0E0)

		if wear_flag in cls.YIN_AND_YANG_ATTR_APPLICABLE_PART_TEXT:
			tooltip.AppendTextLine("%s: %s" % (localeInfo.YIN_AND_YANG_ATTR_TOOLTIP_APPLICABLE_PART, cls.YIN_AND_YANG_ATTR_APPLICABLE_PART_TEXT[wear_flag]), 0xFFB0B0B0)

		desc_text = cls.GetFormattedDescText(attr_vnum, attr_level)
		for line in desc_text.split("\n"):
			tooltip.AppendTextLine(line, 0xFFFFFFFF)

		if remain_time > 0:
			tooltip.AppendTextLine("%s %s" % (localeInfo.YIN_AND_YANG_ATTR_TOOLTIP_REMAIN_TIME, localeInfo.FormatTime(remain_time)), 0xFFFFFF00)

	@classmethod
	def MakeYinAndYangAttrTooltipForGuideBook(cls, tooltip, attr_vnum, attr_level):
		cls.MakeYinAndYangAttrTooltip(tooltip, attr_vnum, attr_level, 0)


class YinAndYangGuideBook(ui.ScriptWindow):
	ATTR_SLOT_PER_PAGE_COUNT = 24
	ATTR_SLOT_X_COUNT = 8
	ATTR_SLOT_Y_COUNT = 3
	IMAGE_PATH_YIN_AND_YANG_LETTER_ICON_VOID = "d:/ymir work/ui/game/yin_and_yang_attr/harmony/void_letter.sub"
	SEARCH_TYPE_KEYWORD = 0
	SEARCH_TYPE_WEAR = 1
	keywords_order = [0, 1, 2, 3, 4, 5, 6, 7, 8]

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.attr_slot_grid_start_line = 0
		self.__accordion_menu = None
		self.__prev_desc_keyword = -1
		self.__attr_slot_grid = None
		self.__attr_slot_grid_scroll_bar = None
		self.__desc_board = None
		self.__tooltip = None
		self.__current_vnums = []

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/YinAndYangAttr_GuideBook.py")
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_GuideBook.LoadWindow.LoadScript - %s" % str(e))
			return

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.__attr_slot_grid = self.GetChild("attr_slot")
			self.__attr_slot_grid_scroll_bar = self.GetChild("attr_slot_scroll_bar")
			self.__desc_board = self.GetChild("description_window")

			if self.__attr_slot_grid:
				self.__attr_slot_grid.SetOverInItemEvent(ui.__mem_func__(self.__OverInAttrSlot))
				self.__attr_slot_grid.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttrSlot))

			if self.__attr_slot_grid_scroll_bar:
				self.__attr_slot_grid_scroll_bar.SetScrollEvent(ui.__mem_func__(self.__AttrSlotGridScrollEvent))

			self.__tooltip = uiToolTip.ItemToolTip()
			self.__tooltip.Hide()
			self.__BuildMenu()
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_GuideBook.LoadWindow.LoadWindow - %s" % str(e))

	def __BuildMenu(self):
		# Default to first keyword
		self.OnSelectMenu((self.SEARCH_TYPE_KEYWORD, 0))

	def __GetKeywordsInWearAndType(self, wear_flag, attr_type):
		return yinAndYangAttr.GetYinAndYangAttrVnumsByWearAndType(wear_flag, attr_type)

	def OnSelectMenu(self, data):
		if not data:
			return
		search_type, param = data
		if search_type == self.SEARCH_TYPE_KEYWORD:
			self.__current_vnums = list(yinAndYangAttr.GetYinAndYangAttrVnumsByKeyword(param))
			self.RefreshDesc(param)
		else:
			wear_flag, attr_type = param
			self.__current_vnums = list(yinAndYangAttr.GetYinAndYangAttrVnumsByWearAndType(wear_flag, attr_type))
		self.attr_slot_grid_start_line = 0
		self.RefreshAttrSlot()

	def RefreshAttrSlot(self):
		if not self.__attr_slot_grid:
			return
		self.__attr_slot_grid.ClearSlot(0)
		start_idx = self.attr_slot_grid_start_line * self.ATTR_SLOT_X_COUNT
		for i in range(self.ATTR_SLOT_PER_PAGE_COUNT):
			idx = start_idx + i
			if idx < len(self.__current_vnums):
				vnum = self.__current_vnums[idx]
				attr_type = yinAndYangAttr.GetYinAndYangAttrType(vnum)
				wear_flag = Util.GetApplicableWearFlag(vnum)
				icon_path = Util.GetYinAndYangAttrIconImagePath(attr_type, wear_flag)
				self.__attr_slot_grid.SetItemSlot(i, vnum, 1)
			else:
				self.__attr_slot_grid.ClearSlot(i)
		self.__attr_slot_grid.RefreshSlot()

	def RefreshDesc(self, keyword):
		self.__prev_desc_keyword = keyword

	def __AttrSlotGridScrollEvent(self):
		pos = self.__attr_slot_grid_scroll_bar.GetPos()
		max_lines = max(0, (len(self.__current_vnums) + self.ATTR_SLOT_X_COUNT - 1) / self.ATTR_SLOT_X_COUNT - self.ATTR_SLOT_Y_COUNT)
		self.attr_slot_grid_start_line = int(pos * max_lines)
		self.RefreshAttrSlot()

	def __OverInAttrSlot(self, slot_pos):
		idx = self.attr_slot_grid_start_line * self.ATTR_SLOT_X_COUNT + slot_pos
		if idx < len(self.__current_vnums):
			vnum = self.__current_vnums[idx]
			if self.__tooltip:
				self.__tooltip.ClearToolTip()
				Util.MakeYinAndYangAttrTooltipForGuideBook(self.__tooltip, vnum, 1)
				self.__tooltip.ShowToolTip()

	def __OverOutAttrSlot(self):
		if self.__tooltip:
			self.__tooltip.HideToolTip()

	def OnMouseWheelButtonUp(self):
		if self.__attr_slot_grid_scroll_bar:
			self.__attr_slot_grid_scroll_bar.OnUp()

	def OnMouseWheelButtonDown(self):
		if self.__attr_slot_grid_scroll_bar:
			self.__attr_slot_grid_scroll_bar.OnDown()

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		if self.__tooltip:
			self.__tooltip.HideToolTip()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True


class YinAndYangHarmony(ui.ScriptWindow):
	IMAGE_PATH_HARMONY_BUTTON_DEFAULT = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_default.sub"
	IMAGE_PATH_HARMONY_BUTTON_DOWN = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_down.sub"
	IMAGE_PATH_HARMONY_BUTTON_OVER = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_over.sub"
	IMAGE_PATH_HARMONY_BUTTON_ENABLED_DEFAULT = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_enabled_default.sub"
	IMAGE_PATH_HARMONY_BUTTON_ENABLED_DOWN = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_enabled_down.sub"
	IMAGE_PATH_HARMONY_BUTTON_ENABLED_OVER = "d:/ymir work/ui/game/yin_and_yang_attr/button_yin_and_yang_harmony_enabled_over.sub"

	IMAGE_PATH_YIN_AND_YANG_ICON_YANG = "d:/ymir work/ui/game/yin_and_yang_attr/harmony/ver2/icon_yang_small.sub"
	IMAGE_PATH_YIN_AND_YANG_ICON_YIN_GLOVE = "d:/ymir work/ui/game/yin_and_yang_attr/harmony/ver2/icon_yin_glove_small.sub"
	IMAGE_PATH_YIN_AND_YANG_ICON_YIN_WEAPON = "d:/ymir work/ui/game/yin_and_yang_attr/harmony/ver2/icon_yin_weapon_small.sub"

	def __init__(self, parent = None):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()
		yinAndYangAttr.SetHarmonyUIHandler(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.__icon_map = {}
		self.__button_map = {}
		self.__effect_map = {}
		self.__line_map = {}
		self.__tooltip = None
		self.__inventory = None
		self.__guide_book = None
		self.__is_system_enabled = False
		self.__is_harmony_enabled = False
		self.__remain_time_sec = 0

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/YinAndYangAttr_Harmony.py")
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_Harmony.LoadWindow.LoadScript - %s" % str(e))
			return

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.__remain_time_text = self.GetChild("remain_time_text")
			self.__enable_toggle_button = self.GetChild("enable_toggle_button")
			if self.__enable_toggle_button:
				self.__enable_toggle_button.SetEvent(ui.__mem_func__(self.__OnToggleEnableButton))

			question_btn = self.GetChild("question_button")
			if question_btn:
				question_btn.SetEvent(ui.__mem_func__(self.__OpenGuideBook))
				question_btn.SetOverInEvent(ui.__mem_func__(self.__OverInQuestionButton))
				question_btn.SetOverOutEvent(ui.__mem_func__(self.__OverOutQuestionButton))

			# Equipment mapping (0: Body, 1: Head, 2: Shoes, 4: Weapon, 11: Glove)
			equip_names = {
				4: ("weapon_icon", "weapon_button", "weapon_activated_effect"),
				11: ("glove_icon", "glove_button", "glove_activated_effect"),
				1: ("head_icon", "head_button", "head_activated_effect"),
				2: ("shoes_icon", "shoes_button", "shoes_activated_effect"),
				0: ("body_icon", "body_button", "body_activated_effect"),
			}
			for wear_idx, (icon_name, btn_name, eff_name) in equip_names.items():
				try:
					btn = self.GetChild(btn_name)
					btn.SetOverInEvent(lambda arg=wear_idx: self.__OnOverEquipmentButton(arg))
					btn.SetOverOutEvent(lambda arg=wear_idx: self.__OverOutEquipmentButton(arg))
					self.__button_map[wear_idx] = btn
				except:
					pass

			self.__tooltip = uiToolTip.ItemToolTip()
			self.__tooltip.Hide()
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_Harmony.LoadWindow.LoadWindow - %s" % str(e))

	def BindInventory(self, inventory):
		self.__inventory = inventory

	def __OpenGuideBook(self):
		if not self.__guide_book:
			self.__guide_book = YinAndYangGuideBook()
		self.__guide_book.Open()

	def __OverInQuestionButton(self):
		pass

	def __OverOutQuestionButton(self):
		pass

	def __OnOverEquipmentButton(self, wear_index):
		pass

	def __OverOutEquipmentButton(self, wear_index):
		pass

	def __OnToggleEnableButton(self):
		bEnable = not self.__is_harmony_enabled
		yinAndYangAttr.SendRequestEnableYinAndYangAttr(bEnable)

	def __ClearConnectionEffects(self):
		pass

	def ClearToggledDownButton(self):
		pass

	def OnAttrActivated(self, keyword):
		pass

	def SetEnableOrDisable(self, is_enabled, remain_time_sec):
		self.__is_harmony_enabled = is_enabled
		self.__remain_time_sec = remain_time_sec
		if self.__remain_time_text:
			self.__remain_time_text.SetText(localeInfo.FormatTime(remain_time_sec) if is_enabled else "")

	def SetSystemEnable(self, is_enable):
		self.__is_system_enabled = is_enable

	def RefreshEquipmentInfo(self):
		pass

	def AppendYinAndYangSummaryToolTip(self, tooltip):
		pass

	def OnUpdate(self):
		pass

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()


class YinAndYangInventory(ui.ScriptWindow):
	GAUGE_ANIMATION_PART_1_TIME_MSEC = 250
	GAUGE_ANIMATION_PART_2_TIME_MSEC = 100
	SLOT_IMG_YANG = 418170776
	SLOT_IMG_YIN_GLOVE = 418170384
	SLOT_IMG_YIN_WEAPON = 418170216

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()
		yinAndYangAttr.SetInventoryUIHandler(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.__attach_attr_item_slot_before = None
		self.__attach_attr_item_slot_attr = None
		self.__attach_attr_item_slot_after = None
		self.__attach_attr_item_gauge_1 = None
		self.__attach_attr_item_gauge_2 = None
		self.__attach_attr_item_gauge_3 = None
		self.__attach_attr_button = None
		self.__attach_attr_need_gold_text = None
		self.__attr_slot_grid = None
		self.__recently_obtained_attr_slot = None
		self.__recently_obtained_attr_slot_remain_time_text = None
		self.__delete_attr_button = None
		self.__register_attr_button = None
		self.__attr_info_by_slot = {}
		self.__expansion_count = 0
		self.__tooltip = None
		self.__item_tooltip = None
		self.__inventory = None
		self.__attach_before_item_vnum = 0
		self.__attach_before_item_inventory_slot_pos = -1
		self.__attach_attr_vnum = 0
		self.__attach_attr_level = 0
		self.__attach_attr_inventory_slot_pos = -1
		self.__is_gauge_animation_processing = False
		self.__gauge_animation_start_time = 0
		self.__question_dialog = None
		self.__highlighted_slots = set()
		self.__guide_book = None

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/YinAndYangAttr_Inventory.py")
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_Inventory.LoadWindow.LoadScript - %s" % str(e))
			return

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.__attach_attr_item_slot_before = self.GetChild("attach_attr_item_slot_before")
			self.__attach_attr_item_slot_attr = self.GetChild("attach_attr_item_slot_attr")
			self.__attach_attr_item_slot_after = self.GetChild("attach_attr_item_slot_after")
			self.__attach_attr_item_gauge_1 = self.GetChild("attach_attr_item_gauge_1")
			self.__attach_attr_item_gauge_2 = self.GetChild("attach_attr_item_gauge_2")
			self.__attach_attr_item_gauge_3 = self.GetChild("attach_attr_item_gauge_3")
			self.__attach_attr_button = self.GetChild("attach_attr_button")
			self.__attach_attr_need_gold_text = self.GetChild("need_gold_text")
			self.__attr_slot_grid = self.GetChild("attr_inventory_slot_grid")
			self.__recently_obtained_attr_slot = self.GetChild("recently_obtained_attr_slot")
			self.__recently_obtained_attr_slot_remain_time_text = self.GetChild("remain_time_text")
			self.__delete_attr_button = self.GetChild("delete_attr_button")
			self.__register_attr_button = self.GetChild("register_attr_button")

			question_btn = self.GetChild("question_button")
			if question_btn:
				question_btn.SetEvent(ui.__mem_func__(self.__OpenGuideBook))

			shop_btn = self.GetChild("yin_and_yang_shop_button")
			if shop_btn:
				shop_btn.SetEvent(ui.__mem_func__(self.__OpenShop))

			if self.__attach_attr_button:
				self.__attach_attr_button.SetEvent(ui.__mem_func__(self.__AttachAttr))
			if self.__delete_attr_button:
				self.__delete_attr_button.SetEvent(ui.__mem_func__(self.__DeleteAttr))
			if self.__register_attr_button:
				self.__register_attr_button.SetEvent(ui.__mem_func__(self.__RegisterRecentlyObtainedAttr))

			# Slot events
			if self.__attach_attr_item_slot_before:
				self.__attach_attr_item_slot_before.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectSlotAttachBefore))
				self.__attach_attr_item_slot_before.SetSelectItemSlotEvent(ui.__mem_func__(self.__UseItemSlotAttachBefore))
				self.__attach_attr_item_slot_before.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlotAttachBefore))
				self.__attach_attr_item_slot_before.SetOverInItemEvent(ui.__mem_func__(self.__OverInAttachBefore))
				self.__attach_attr_item_slot_before.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttachBefore))

			if self.__attach_attr_item_slot_attr:
				self.__attach_attr_item_slot_attr.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectSlotAttachAttr))
				self.__attach_attr_item_slot_attr.SetSelectItemSlotEvent(ui.__mem_func__(self.__UseItemSlotAttachAttr))
				self.__attach_attr_item_slot_attr.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlotAttachAttr))
				self.__attach_attr_item_slot_attr.SetOverInItemEvent(ui.__mem_func__(self.__OverInAttachAttr))
				self.__attach_attr_item_slot_attr.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttachAttr))

			if self.__attach_attr_item_slot_after:
				self.__attach_attr_item_slot_after.SetOverInItemEvent(ui.__mem_func__(self.__OverInAttachAfter))
				self.__attach_attr_item_slot_after.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttach))

			if self.__attr_slot_grid:
				self.__attr_slot_grid.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectEmptySlotInventory))
				self.__attr_slot_grid.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectItemSlotInventory))
				self.__attr_slot_grid.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlotInventory))
				self.__attr_slot_grid.SetOverInItemEvent(ui.__mem_func__(self.__OverInAttrInventory))
				self.__attr_slot_grid.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttrInventory))

			if self.__recently_obtained_attr_slot:
				self.__recently_obtained_attr_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInRecentlyObtainedAttr))
				self.__recently_obtained_attr_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutAttach))

			self.__tooltip = uiToolTip.ItemToolTip()
			self.__tooltip.Hide()
			self.__ClearGauge()
		except Exception as e:
			import dbg
			dbg.TraceError("YinAndYangAttr_Inventory.LoadWindow.LoadWindow - %s" % str(e))

	def BindInventory(self, inventory):
		self.__inventory = inventory

	def SetItemToolTip(self, tooltip):
		self.__item_tooltip = tooltip

	def __OpenGuideBook(self):
		if not self.__guide_book:
			self.__guide_book = YinAndYangGuideBook()
		self.__guide_book.Open()

	def __OpenShop(self):
		net.SendChatPacket("/yin_and_yang_shop")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.UnregisterYinAndYangInventoryItemToAttachSlot()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def SetData(self, expansion_count, inventory_item_dict):
		self.__expansion_count = expansion_count
		self.__attr_info_by_slot = inventory_item_dict if inventory_item_dict else {}
		self.Refresh()

	def UpdateIventoryExpansionCount(self, expansion_count):
		self.__expansion_count = expansion_count
		self.Refresh()

	def OnDeleteItem(self, pos):
		if pos in self.__attr_info_by_slot:
			del self.__attr_info_by_slot[pos]
		self.Refresh()

	def OnUpdateItem(self, pos, vnum, level, obtain_time):
		self.__attr_info_by_slot[pos] = (vnum, level, obtain_time)
		self.Refresh()

	def OnMoveItem(self, src_pos, dst_pos):
		if src_pos in self.__attr_info_by_slot:
			self.__attr_info_by_slot[dst_pos] = self.__attr_info_by_slot.pop(src_pos)
		self.Refresh()

	def Refresh(self):
		if not self.__attr_slot_grid:
			return
		self.__attr_slot_grid.ClearSlot(0)
		for slot_pos, (vnum, level, obtain_time) in self.__attr_info_by_slot.items():
			attr_type = yinAndYangAttr.GetYinAndYangAttrType(vnum)
			wear_flag = Util.GetApplicableWearFlag(vnum)
			self.__attr_slot_grid.SetItemSlot(slot_pos, vnum, level)
		self.__attr_slot_grid.RefreshSlot()

	def __SelectSlotAttachBefore(self, slot_pos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if attachedSlotType == player.SLOT_TYPE_INVENTORY:
				self.RegisterInventoryItemToAttachSlotBefore(attachedSlotPos)
				mouseModule.mouseController.DeattachObject()

	def __UseItemSlotAttachBefore(self, slot_pos):
		self.UnregisterYinAndYangInventoryItemToAttachSlot()

	def __SelectSlotAttachAttr(self, slot_pos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if attachedSlotType == player.SLOT_TYPE_YIN_AND_YANG_INVENTORY:
				self.__RegisterYinAndYangInventoryItemToAttachSlot(attachedSlotPos)
				mouseModule.mouseController.DeattachObject()

	def __UseItemSlotAttachAttr(self, slot_pos):
		self.__attach_attr_vnum = 0
		self.__attach_attr_level = 0
		self.__attach_attr_inventory_slot_pos = -1
		if self.__attach_attr_item_slot_attr:
			self.__attach_attr_item_slot_attr.ClearSlot(0)
			self.__attach_attr_item_slot_attr.RefreshSlot()
		self.__ClearGauge()
		self.__ClearResultItem()

	def RegisterInventoryItemToAttachSlotBefore(self, inventory_slot_pos):
		itemVnum = player.GetItemIndex(inventory_slot_pos)
		if not yinAndYangAttr.IsYinAndYangAttrAttachablePart(itemVnum):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YIN_AND_YANG_ATTR_INVENTORY_ITEM_ONLY_TO_BEFORE_SLOT)
			return
		self.__attach_before_item_vnum = itemVnum
		self.__attach_before_item_inventory_slot_pos = inventory_slot_pos
		if self.__attach_attr_item_slot_before:
			self.__attach_attr_item_slot_before.SetItemSlot(0, itemVnum, 1)
			self.__attach_attr_item_slot_before.RefreshSlot()
		self.__ShowConnectedAttrItems()

	def UnregisterYinAndYangInventoryItemToAttachSlot(self):
		self.__attach_before_item_vnum = 0
		self.__attach_before_item_inventory_slot_pos = -1
		self.__attach_attr_vnum = 0
		self.__attach_attr_level = 0
		self.__attach_attr_inventory_slot_pos = -1
		if self.__attach_attr_item_slot_before:
			self.__attach_attr_item_slot_before.ClearSlot(0)
			self.__attach_attr_item_slot_before.RefreshSlot()
		if self.__attach_attr_item_slot_attr:
			self.__attach_attr_item_slot_attr.ClearSlot(0)
			self.__attach_attr_item_slot_attr.RefreshSlot()
		self.__ClearGauge()
		self.__ClearResultItem()

	def __RegisterYinAndYangInventoryItemToAttachSlot(self, slot_pos):
		if slot_pos not in self.__attr_info_by_slot:
			return
		vnum, level, obtain_time = self.__attr_info_by_slot[slot_pos]
		self.__attach_attr_vnum = vnum
		self.__attach_attr_level = level
		self.__attach_attr_inventory_slot_pos = slot_pos
		if self.__attach_attr_item_slot_attr:
			self.__attach_attr_item_slot_attr.SetItemSlot(0, vnum, level)
			self.__attach_attr_item_slot_attr.RefreshSlot()
		self.__ShowResultItem()

	def __ShowConnectedAttrItems(self):
		pass

	def __ShowResultItem(self):
		if self.__attach_before_item_vnum and self.__attach_attr_vnum:
			if self.__attach_attr_item_slot_after:
				self.__attach_attr_item_slot_after.SetItemSlot(0, self.__attach_before_item_vnum, 1)
				self.__attach_attr_item_slot_after.RefreshSlot()
			cost = yinAndYangAttr.GetNeedGoldToAttachAttr(self.__attach_before_item_vnum, 1)
			if self.__attach_attr_need_gold_text:
				self.__attach_attr_need_gold_text.SetText(localeInfo.NumberToMoneyString(cost))

	def __ClearResultItem(self):
		if self.__attach_attr_item_slot_after:
			self.__attach_attr_item_slot_after.ClearSlot(0)
			self.__attach_attr_item_slot_after.RefreshSlot()
		if self.__attach_attr_need_gold_text:
			self.__attach_attr_need_gold_text.SetText("0")

	def __ClearGauge(self):
		pass

	def __AttachAttr(self):
		if not self.__attach_before_item_inventory_slot_pos >= 0 or not self.__attach_attr_vnum:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YIN_AND_YANG_ATTR_ATTACH_REGISTER_BEFORE_ITEM)
			return
		cost = yinAndYangAttr.GetNeedGoldToAttachAttr(self.__attach_before_item_vnum, 1)
		if player.GetElk() < cost:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YIN_AND_YANG_ATTR_ATTACH_NOT_ENOUGH_GOLD % localeInfo.NumberToMoneyString(cost))
			return
		yinAndYangAttr.SendRequestAttachAttrToItem(self.__attach_before_item_inventory_slot_pos, self.__attach_attr_inventory_slot_pos)

	def __DeleteAttr(self):
		pass

	def __RegisterRecentlyObtainedAttr(self):
		yinAndYangAttr.SendRequestRegisterRecentlyGainedItem()

	def __SelectEmptySlotInventory(self, slot_pos):
		pass

	def __SelectItemSlotInventory(self, slot_pos):
		if slot_pos in self.__attr_info_by_slot:
			vnum, level, obtain_time = self.__attr_info_by_slot[slot_pos]
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_YIN_AND_YANG_INVENTORY, slot_pos, vnum, level)

	def __UseItemSlotInventory(self, slot_pos):
		self.__RegisterYinAndYangInventoryItemToAttachSlot(slot_pos)

	def __OverInAttachBefore(self):
		if self.__attach_before_item_vnum and self.__item_tooltip:
			self.__item_tooltip.SetInventoryItem(self.__attach_before_item_inventory_slot_pos)

	def __OverOutAttachBefore(self):
		if self.__item_tooltip:
			self.__item_tooltip.HideToolTip()

	def __OverInAttachAttr(self):
		if self.__attach_attr_vnum and self.__tooltip:
			self.__tooltip.ClearToolTip()
			Util.MakeYinAndYangAttrTooltip(self.__tooltip, self.__attach_attr_vnum, self.__attach_attr_level)
			self.__tooltip.ShowToolTip()

	def __OverOutAttachAttr(self):
		if self.__tooltip:
			self.__tooltip.HideToolTip()

	def __OverInAttachAfter(self):
		if self.__attach_before_item_vnum and self.__item_tooltip:
			self.__item_tooltip.SetInventoryItem(self.__attach_before_item_inventory_slot_pos)

	def __OverOutAttach(self):
		if self.__item_tooltip:
			self.__item_tooltip.HideToolTip()
		if self.__tooltip:
			self.__tooltip.HideToolTip()

	def __OverInAttrInventory(self, slot_pos):
		if slot_pos in self.__attr_info_by_slot and self.__tooltip:
			vnum, level, obtain_time = self.__attr_info_by_slot[slot_pos]
			self.__tooltip.ClearToolTip()
			Util.MakeYinAndYangAttrTooltip(self.__tooltip, vnum, level)
			self.__tooltip.ShowToolTip()

	def __OverOutAttrInventory(self):
		if self.__tooltip:
			self.__tooltip.HideToolTip()

	def __OverInRecentlyObtainedAttr(self):
		pass

	def HighlightSlot(self, inven_pos):
		self.__highlighted_slots.add(inven_pos)

	def DelHighlightSlot(self, inven_pos):
		self.__highlighted_slots.discard(inven_pos)

	def HasEmptySlotCount(self):
		return len(self.__attr_info_by_slot) < 45

	def IsAttrAttachableInventoryItem(self, inven_pos):
		itemVnum = player.GetItemIndex(inven_pos)
		return yinAndYangAttr.IsYinAndYangAttrAttachablePart(itemVnum)

	def CantAttachAttrInventorySlot(self, inven_pos):
		return not self.IsAttrAttachableInventoryItem(inven_pos)

	def OnUpdate(self):
		pass
