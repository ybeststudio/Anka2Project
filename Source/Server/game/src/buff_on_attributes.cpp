#include "stdafx.h"
#include "../../common/tables.h"
#include "item.h"
#include "char.h"
#include "buff_on_attributes.h"
#include <algorithm>

CBuffOnAttributes::CBuffOnAttributes(LPCHARACTER pOwner, BYTE point_type, std::vector <BYTE>* p_vec_buff_wear_targets)
:	m_pBuffOwner(pOwner), m_bPointType(point_type), m_p_vec_buff_wear_targets(p_vec_buff_wear_targets)
{
	Initialize();
}

CBuffOnAttributes::~CBuffOnAttributes()
{
	Off();
}

void CBuffOnAttributes::Initialize()
{
	m_bBuffValue = 0;
	m_map_additional_attrs.clear();
}

void CBuffOnAttributes::RemoveBuffFromItem(LPITEM pItem)
{
	if (0 == m_bBuffValue)
		return ;
	if (NULL != pItem)
	{
		if (pItem->GetCell() < INVENTORY_MAX_NUM)
			return;
		std::vector <BYTE>::iterator it = find (m_p_vec_buff_wear_targets->begin(), m_p_vec_buff_wear_targets->end(), pItem->GetCell() - INVENTORY_MAX_NUM);
		if (m_p_vec_buff_wear_targets->end() == it)
			return;

		int m = pItem->GetAttributeCount();
		for (int j = 0; j < m; j++)
		{
			TPlayerItemAttribute attr = pItem->GetAttribute(j);
			TMapAttr::iterator it = m_map_additional_attrs.find(attr.bType);

			if (it != m_map_additional_attrs.end())
			{
				int& sum_of_attr_value = it->second;
				int old_value = sum_of_attr_value * m_bBuffValue / 100;
				int new_value = (sum_of_attr_value - attr.sValue) * m_bBuffValue / 100;
				m_pBuffOwner->ApplyPoint(attr.bType, new_value - old_value);
				sum_of_attr_value -= attr.sValue;
			}
			else
			{
				sys_err ("Buff ERROR(type %d). This item(%d) attr_type(%d) was not in buff pool", m_bPointType, pItem->GetVnum(), attr.bType);
				return;
			}
		}
	}
}

void CBuffOnAttributes::AddBuffFromItem(LPITEM pItem)
{
	if (0 == m_bBuffValue)
		return ;
	if (NULL != pItem)
	{
		if (pItem->GetCell() < INVENTORY_MAX_NUM)
			return;
		std::vector <BYTE>::iterator it = find (m_p_vec_buff_wear_targets->begin(), m_p_vec_buff_wear_targets->end(), pItem->GetCell() - INVENTORY_MAX_NUM);
		if (m_p_vec_buff_wear_targets->end() == it)
			return;

		int m = pItem->GetAttributeCount();
		for (int j = 0; j < m; j++)
		{
			TPlayerItemAttribute attr = pItem->GetAttribute(j);
			TMapAttr::iterator it = m_map_additional_attrs.find(attr.bType);

			if (it == m_map_additional_attrs.end())
			{
				m_pBuffOwner->ApplyPoint(attr.bType, attr.sValue * m_bBuffValue / 100);
				m_map_additional_attrs.insert(TMapAttr::value_type(attr.bType, attr.sValue));
			}
			else
			{
				int& sum_of_attr_value = it->second;
				int old_value = sum_of_attr_value * m_bBuffValue / 100;
				int new_value = (sum_of_attr_value + attr.sValue) * m_bBuffValue / 100;
				m_pBuffOwner->ApplyPoint(attr.bType, new_value - old_value);
				sum_of_attr_value += attr.sValue;
			}
		}
	}
}

void CBuffOnAttributes::ChangeBuffValue(BYTE bNewValue)
{
	if (0 == m_bBuffValue)
		On(bNewValue);
	else if (0 == bNewValue)
		Off();
	else
	{
		for (TMapAttr::iterator it = m_map_additional_attrs.begin(); it != m_map_additional_attrs.end(); ++it)	//@fixme541
		{
			int& sum_of_attr_value = it->second;
			int old_value = sum_of_attr_value * m_bBuffValue / 100;
			int new_value = sum_of_attr_value * bNewValue / 100;
			(void)old_value;
			(void)new_value;
			m_pBuffOwner->ApplyPoint(it->first, -sum_of_attr_value * m_bBuffValue / 100);
		}
		m_bBuffValue = bNewValue;
	}
}

void CBuffOnAttributes::GiveAllAttributes()
{
	if (0 == m_bBuffValue)
		return;
	for (TMapAttr::iterator it = m_map_additional_attrs.begin(); it != m_map_additional_attrs.end(); it++)
	{
		BYTE apply_type = it->first;
		int apply_value = it->second * m_bBuffValue / 100;
		
		m_pBuffOwner->ApplyPoint(apply_type, apply_value);
	}
}

bool CBuffOnAttributes::On(BYTE bValue)
{
	if (0 != m_bBuffValue || 0 == bValue)
		return false;
	
	int n = m_p_vec_buff_wear_targets->size();
	m_map_additional_attrs.clear();
	for (int i = 0; i < n; i++)
	{
		LPITEM pItem = m_pBuffOwner->GetWear(m_p_vec_buff_wear_targets->at(i));
		if (NULL != pItem)
		{
			int m = pItem->GetAttributeCount();
			for (int j = 0; j < m; j++)
			{
				TPlayerItemAttribute attr = pItem->GetAttribute(j);
				TMapAttr::iterator it = m_map_additional_attrs.find(attr.bType);
				if (it != m_map_additional_attrs.end())
				{
					it->second += attr.sValue;
				}
				else
				{
					m_map_additional_attrs.insert(TMapAttr::value_type(attr.bType, attr.sValue));
				}
			}
		}
	}

	for (TMapAttr::iterator it = m_map_additional_attrs.begin(); it != m_map_additional_attrs.end(); ++it)	//@fixme541
	{
		m_pBuffOwner->ApplyPoint(it->first, it->second * bValue / 100);
	}

	m_bBuffValue = bValue;

	return true;
}

void CBuffOnAttributes::Off()
{
	if (0 == m_bBuffValue)
		return ;

	for (TMapAttr::iterator it = m_map_additional_attrs.begin(); it != m_map_additional_attrs.end(); it++)
	{
		m_pBuffOwner->ApplyPoint(it->first, -it->second * m_bBuffValue / 100);
	}
	Initialize();
}
