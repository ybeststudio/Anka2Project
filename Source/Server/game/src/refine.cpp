#include "stdafx.h"
#include "refine.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "boost/format.hpp"
CRefineManager::CRefineManager()
{
}

CRefineManager::~CRefineManager()
{
}

bool CRefineManager::Initialize(TRefineTable * table, int size)
{
	for (int i=0; i<size; ++i, ++table)
	{
		m_map_RefineRecipe.insert(std::make_pair(table->id, *table));
	}

	return true;
}

const TRefineTable* CRefineManager::GetRefineRecipe(DWORD vnum)
{
	if (vnum == 0)
		return NULL;

	itertype(m_map_RefineRecipe) it = m_map_RefineRecipe.find(vnum);

	if (it == m_map_RefineRecipe.end())
	{
		return NULL;
	}

	return &it->second;
}
