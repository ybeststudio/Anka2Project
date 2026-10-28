	#include "stdafx.h"
#include "config.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char.h"
#include "affect.h"
#include "db.h"

namespace quest
{
#ifdef ENABLE_SUNG_MAHI_TOWER
	int affect_add_type(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager & q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		
		DWORD dwType = (DWORD) lua_tonumber(L, 1);
		int applyOn = (int) lua_tonumber(L, 2);
		
		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}
		
		long value = (long) lua_tonumber(L, 3);
		long duration = (long) lua_tonumber(L, 4);
		
		ch->AddAffect(dwType, aApplyInfo[applyOn].bPointType, value, 0, duration, 0, true);
		return 0;
	}
	
	int affect_remove_type(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}
		
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch)
		{
			DWORD dwType = (DWORD) lua_tonumber(L, 1);
			ch->RemoveAffect(dwType);
		}
		
		return 0;
	}
#endif

	int affect_add(lua_State * L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager & q = CQuestManager::instance();

		int applyOn = (int) lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}

		if (ch->FindAffect(AFFECT_QUEST_START_IDX, applyOn))
			return 0;

		long value = (long) lua_tonumber(L, 2);
		long duration = (long) lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_QUEST_START_IDX, aApplyInfo[applyOn].bPointType, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove(lua_State * L)
	{
		CQuestManager & q = CQuestManager::instance();
		int iType;

		if (lua_isnumber(L, 1))
		{
			iType = (int) lua_tonumber(L, 1);

			if (iType == 0)
				iType = q.GetCurrentPC()->GetCurrentQuestIndex() + AFFECT_QUEST_START_IDX;
		}
		else
			iType = q.GetCurrentPC()->GetCurrentQuestIndex() + AFFECT_QUEST_START_IDX;

		q.GetCurrentCharacterPtr()->RemoveAffect(iType);

		return 0;
	}

	int affect_remove_bad(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveBadAffect();
		return 0;
	}

	int affect_remove_good(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveGoodAffect();
		return 0;
	}

	int affect_add_hair(lua_State * L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager & q = CQuestManager::instance();

		int applyOn = (int) lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
		{
			sys_err("apply is out of range : %d", applyOn);
			return 0;
		}

		long value = (long) lua_tonumber(L, 2);
		long duration = (long) lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_HAIR, aApplyInfo[applyOn].bPointType, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove_hair(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CAffect* pkAff = ch->FindAffect( AFFECT_HAIR );

		if ( pkAff != NULL )
		{
			lua_pushnumber(L, pkAff->lDuration);
			ch->RemoveAffect( pkAff );
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int affect_get_apply_on(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		DWORD affectType = lua_tonumber(L, 1);

		CAffect* pkAff = ch->FindAffect(affectType);

		if ( pkAff != NULL )
			lua_pushnumber(L, pkAff->bApplyOn);
		else
			lua_pushnil(L);

		return 1;

	}

	int affect_add_collect(lua_State * L)
	{
		// affect.add_collect(affect_type, apply_on, value, duration)
		// ENABLE_COLLECT_WINDOW sistemi için 4 parametre: affect_type (310-315), apply_on, value, duration
		// Eski sistem için 3 parametre: apply_on, value, duration (affect_type = AFFECT_COLLECT)
		
		if (lua_gettop(L) >= 4 && lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4))
		{
			// Yeni format: affect_type, apply_on, value, duration
			CQuestManager & q = CQuestManager::instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
			
			DWORD affectType = (DWORD) lua_tonumber(L, 1);
			int applyOn = (int) lua_tonumber(L, 2);
			
			if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
			{
				sys_err("apply is out of range : %d", applyOn);
				return 0;
			}
			
			long value = (long) lua_tonumber(L, 3);
			long duration = (long) lua_tonumber(L, 4);
			
			ch->AddAffect(affectType, aApplyInfo[applyOn].bPointType, value, 0, duration, 0, false);
			
			if (test_server)
			{
				sys_log(0, "affect_add_collect: %s affect_type %d apply %d value %d duration %d", ch->GetName(), affectType, applyOn, value, duration);
			}
		}
		else if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3))
		{
			// Eski format: apply_on, value, duration (affect_type = AFFECT_COLLECT)
			CQuestManager & q = CQuestManager::instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
			
			int applyOn = (int) lua_tonumber(L, 1);
			
			if (applyOn >= MAX_APPLY_NUM || applyOn < 1)
			{
				sys_err("apply is out of range : %d", applyOn);
				return 0;
			}
			
			long value = (long) lua_tonumber(L, 2);
			long duration = (long) lua_tonumber(L, 3);
			
			ch->AddAffect(AFFECT_COLLECT, aApplyInfo[applyOn].bPointType, value, 0, duration, 0, false);
		}
		else
		{
			sys_err("affect_add_collect: invalid argument count");
			return 0;
		}

		return 0;
	}

	int affect_add_collect_point(lua_State * L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager & q = CQuestManager::instance();

		BYTE point_type = (BYTE) lua_tonumber(L, 1);

		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (point_type >= POINT_MAX_NUM || point_type < 1)
		{
			sys_err("point is out of range : %d", point_type);
			return 0;
		}

		long value = (long) lua_tonumber(L, 2);
		long duration = (long) lua_tonumber(L, 3);

		ch->AddAffect(AFFECT_COLLECT, point_type, value, 0, duration, 0, false);

		return 0;
	}

	int affect_remove_collect(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			BYTE bApply = (BYTE)lua_tonumber(L, 1);

			if ( bApply >= MAX_APPLY_NUM ) return 0;

			bApply = aApplyInfo[bApply].bPointType;
			long value = (long)lua_tonumber(L, 2);

			const std::list<CAffect*>& rList = ch->GetAffectContainer();
			const CAffect* pAffect = NULL;

			for ( std::list<CAffect*>::const_iterator iter = rList.begin(); iter != rList.end(); ++iter )
			{
				pAffect = *iter;

				if ( pAffect->dwType == AFFECT_COLLECT )
				{
					if ( pAffect->bApplyOn == bApply && pAffect->lApplyValue == value )
					{
						break;
					}
				}

				pAffect = NULL;
			}

			if ( pAffect != NULL )
			{
				ch->RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}

		return 0;
	}

	int affect_remove_all_collect( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_COLLECT);
		}

		return 0;
	}

#ifdef ENABLE_BIOLOGIST_SYSTEM
	/**30 Lv Biyolog**/
	int affect_add_30( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[8].bPointType, 10, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**30 Lv Biyolog**/

	/**40 Lv Biyolog**/
	int affect_add_40( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[7].bPointType, 5, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**40 Lv Biyolog**/

	/**50 Lv Biyolog**/
	int affect_add_50( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[54].bPointType, 60, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**50 Lv Biyolog**/

	/**60 Lv Biyolog**/
	int affect_add_60( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[53].bPointType, 50, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**60 Lv Biyolog**/

	/**70 Lv Biyolog**/
	int affect_add_70( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[8].bPointType, 11, 0, 60*60*24*365*60, 0, false);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[7].bPointType, 10, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**70 Lv Biyolog**/

	/**80 Lv Biyolog**/
	int affect_add_80( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[7].bPointType, 6, 0, 60*60*24*365*60, 0, false);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[64].bPointType, 10, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**80 Lv Biyolog**/

	/**85 Lv Biyolog**/
	int affect_add_85( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[78].bPointType, 10, 0, 60*60*24*365*60, 0, false);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[79].bPointType, 10, 0, 60*60*24*365*60, 0, false);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[80].bPointType, 10, 0, 60*60*24*365*60, 0, false);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[81].bPointType, 10, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**85 Lv Biyolog**/

	/**90 Lv Biyolog**/
	int affect_add_90( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO);
			ch->AddAffect(AFFECT_BIO, aApplyInfo[17].bPointType, 10, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**90 Lv Biyolog**/

	/**92 Lv Biyolog**/
	int affect_add_92_1( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_92);
			ch->AddAffect(AFFECT_BIO_92, aApplyInfo[1].bPointType, 1000, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}

	int affect_add_92_2( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_92);
			ch->AddAffect(AFFECT_BIO_92, aApplyInfo[54].bPointType, 120, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}

	int affect_add_92_3( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_92);
			ch->AddAffect(AFFECT_BIO_92, aApplyInfo[53].bPointType, 50, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**92 Lv Biyolog**/

	/**94 Lv Biyolog**/
	int affect_add_94_1( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_94);
			ch->AddAffect(AFFECT_BIO_94, aApplyInfo[1].bPointType, 1100, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}

	int affect_add_94_2( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_94);
			ch->AddAffect(AFFECT_BIO_94, aApplyInfo[54].bPointType, 140, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}

	int affect_add_94_3( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			ch->RemoveAffect(AFFECT_BIO_94);
			ch->AddAffect(AFFECT_BIO_94, aApplyInfo[53].bPointType, 60, 0, 60*60*24*365*60, 0, false);
		}

		return 0;
	}
	/**94 Lv Biyolog**/
#endif

	void RegisterAffectFunctionTable()
	{
		luaL_reg affect_functions[] =
		{
			{ "add",		affect_add		},
			{ "remove",		affect_remove		},
			{ "remove_bad",	affect_remove_bad	},
			{ "remove_good",	affect_remove_good	},
			{ "add_hair",		affect_add_hair		},
			{ "remove_hair",	affect_remove_hair		},
			{ "add_collect",		affect_add_collect		},
			{ "add_collect_point",		affect_add_collect_point		},
			{ "remove_collect",		affect_remove_collect	},
			{ "remove_all_collect",	affect_remove_all_collect	},
			{ "get_apply_on",	affect_get_apply_on },

#ifdef ENABLE_SUNG_MAHI_TOWER
			{ "add_type",	affect_add_type },
			{ "remove_type",	affect_remove_type },
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
			/*****Biyolog******/
			{ "add_30",	affect_add_30	},
			{ "add_40",	affect_add_40	},
			{ "add_50",	affect_add_50	},
			{ "add_60",	affect_add_60	},
			{ "add_70",	affect_add_70	},
			{ "add_80",	affect_add_80	},
			{ "add_85",	affect_add_85	},
			{ "add_90",	affect_add_90	},
			{ "add_92_1",	affect_add_92_1	},
			{ "add_92_2",	affect_add_92_2	},
			{ "add_92_3",	affect_add_92_3	},
			{ "add_94_1",	affect_add_94_1	},
			{ "add_94_2",	affect_add_94_2	},
			{ "add_94_3",	affect_add_94_3	},
			/*****Biyolog******/
#endif

			{ NULL,		NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("affect", affect_functions);
	}
};
