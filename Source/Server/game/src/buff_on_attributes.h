#ifndef __METIN2_BUFF_ON_ATTRIBUTES_H
#define __METIN2_BUFF_ON_ATTRIBUTES_H

class CHARACTER;

class CBuffOnAttributes
{
	public:
		CBuffOnAttributes(LPCHARACTER pOwner, BYTE m_point_type, std::vector <BYTE>* vec_buff_targets);
		~CBuffOnAttributes();

		void RemoveBuffFromItem(LPITEM pItem);
		void AddBuffFromItem(LPITEM pItem);
		void ChangeBuffValue(BYTE bNewValue);
		void GiveAllAttributes();

		bool On(BYTE bValue);
		void Off();

		void Initialize();

	private:
		LPCHARACTER m_pBuffOwner;
		BYTE m_bPointType;
		BYTE m_bBuffValue;
		std::vector <BYTE>* m_p_vec_buff_wear_targets;

		typedef std::map <BYTE, int> TMapAttr;
		TMapAttr m_map_additional_attrs;
};

#endif
