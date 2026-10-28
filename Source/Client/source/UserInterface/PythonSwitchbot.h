#pragma once

#ifdef ENABLE_RENEWAL_SWITCHBOT

class CPythonSwitchbot : public CSingleton <CPythonSwitchbot>
{
	public:
		#pragma pack(1)
		struct TSwitchbotAttributeAlternativeTable
		{
			TPlayerItemAttribute attributes[MAX_NORM_ATTR_NUM];

			bool IsConfigured() const
			{
				for (const auto& it : attributes)
				{
					if (it.bType && it.sValue)
					{
						return true;
					}
				}

				return false;
			}
		};

		struct TSwitchbotTable
		{
			DWORD player_id;
			bool active[SWITCHBOT_SLOT_COUNT];
			bool finished[SWITCHBOT_SLOT_COUNT];
			DWORD items[SWITCHBOT_SLOT_COUNT];
			TSwitchbotAttributeAlternativeTable alternatives[SWITCHBOT_SLOT_COUNT][SWITCHBOT_ALTERNATIVE_COUNT];

			TSwitchbotTable() : player_id(0)
			{
				memset(&items, 0, sizeof(items));
				memset(&alternatives, 0, sizeof(alternatives));
				memset(&active, false, sizeof(active));
				memset(&finished, false, sizeof(finished));
			}
		};

		struct TSwitchbottAttributeTable
		{
			BYTE attribute_set;
			int apply_num;
			long max_value;
		};

	#pragma pack()

		CPythonSwitchbot();
		virtual ~CPythonSwitchbot();

		void Initialize();
		void Update(const TSwitchbotTable& table);

		void SetAttribute(BYTE slot, BYTE alternative, BYTE attrIdx, WORD attrType, int attrValue);
		TPlayerItemAttribute GetAttribute(BYTE slot, BYTE alternative, BYTE attrIdx);
		void GetAlternatives(BYTE slot, std::vector<TSwitchbotAttributeAlternativeTable>& attributes);
		int GetAttibuteSet(BYTE slot);
		bool IsActive(BYTE slot);
		bool IsFinished(BYTE slot);

		void ClearSlot(BYTE slot);
		void ClearAttributeMap();

		void AddAttributeToMap(const TSwitchbottAttributeTable& table);
		void GetAttributesBySet(int iAttributeSet, std::vector<TPlayerItemAttribute>& vec_attributes);
		long GetAttributeMaxValue(int iAttributeSet, WORD attrType);

	protected:
		TSwitchbotTable m_SwitchbotTable;
		std::map<BYTE, std::map<BYTE, long>> m_map_AttributesBySet;
};
#endif