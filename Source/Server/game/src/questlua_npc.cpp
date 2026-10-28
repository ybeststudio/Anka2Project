#include "../../common/service.h"
#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "questmanager.h"
#include "char.h"
#include "party.h"
#include "xmas_event.h"
#include "char_manager.h"
#include "shop_manager.h"
#include "guild.h"
#include "sectree_manager.h"

namespace quest
{
	//
	// "npc" lua functions
	//
	int npc_open_shop(lua_State * L)
	{
		int iShopVnum = 0;

		if (lua_gettop(L) == 1)
		{
			if (lua_isnumber(L, 1))
				iShopVnum = (int) lua_tonumber(L, 1);
		}

		if (CQuestManager::instance().GetCurrentNPCCharacterPtr())
			CShopManager::instance().StartShopping(CQuestManager::instance().GetCurrentCharacterPtr(), CQuestManager::instance().GetCurrentNPCCharacterPtr(), iShopVnum);
		return 0;
	}

	int npc_is_pc(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();
		if (npc && npc->IsPC())
			lua_pushboolean(L, 1);
		else
			lua_pushboolean(L, 0);
		return 1;
	}

	int npc_get_empire(lua_State * L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();
		if (npc)
			lua_pushnumber(L, npc->GetEmpire());
		else
			lua_pushnumber(L, 0);
		return 1;
	}

	int npc_get_race(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentNPCRace());
		return 1;
	}

	int npc_get_guild(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();
		CGuild* pGuild = NULL;
		if (npc)
			pGuild = npc->GetGuild();

		lua_pushnumber(L, pGuild ? pGuild->GetID() : 0);
		return 1;
	}

	int npc_get_remain_skill_book_count(lua_State* L)
	{
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		if (!npc || npc->IsPC() || npc->GetRaceNum() != xmas::MOB_SANTA_VNUM)
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		lua_pushnumber(L, MAX(0, npc->GetPoint(POINT_ATT_GRADE_BONUS)));
		return 1;
	}

	int npc_dec_remain_skill_book_count(lua_State* L)
	{
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		if (!npc || npc->IsPC() || npc->GetRaceNum() != xmas::MOB_SANTA_VNUM)
		{
			return 0;
		}
		npc->SetPoint(POINT_ATT_GRADE_BONUS, MAX(0, npc->GetPoint(POINT_ATT_GRADE_BONUS)-1));
		return 0;
	}

	int npc_get_remain_hairdye_count(lua_State* L)
	{
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		if (!npc || npc->IsPC() || npc->GetRaceNum() != xmas::MOB_SANTA_VNUM)
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		lua_pushnumber(L, MAX(0, npc->GetPoint(POINT_DEF_GRADE_BONUS)));
		return 1;
	}

	int npc_dec_remain_hairdye_count(lua_State* L)
	{
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		if (!npc || npc->IsPC() || npc->GetRaceNum() != xmas::MOB_SANTA_VNUM)
		{
			return 0;
		}
		npc->SetPoint(POINT_DEF_GRADE_BONUS, MAX(0, npc->GetPoint(POINT_DEF_GRADE_BONUS)-1));
		return 0;
	}

	int npc_is_quest(lua_State * L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		if (npc)
		{
			const std::string & r_st = q.GetCurrentQuestName();

			if (q.GetQuestIndexByName(r_st) == npc->GetQuestBy())
			{
				lua_pushboolean(L, 1);
				return 1;
			}
		}

		lua_pushboolean(L, 0);
		return 1;
	}

	int npc_kill(lua_State * L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		ch->SetQuestNPCID(0);
		if (npc)
		{
			npc->Dead();
		}
		return 0;
	}

	int npc_purge(lua_State * L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		ch->SetQuestNPCID(0);
		if (npc)
		{
			M2_DESTROY_CHARACTER(npc);
		}
		return 0;
	}

	int npc_is_near(lua_State * L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		lua_Number dist = 10;

		if (lua_isnumber(L, 1))
			dist = lua_tonumber(L, 1);

		if (ch == NULL || npc == NULL)
		{
			lua_pushboolean(L, false);
		}
		else
		{
			lua_pushboolean(L, DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY()) < dist*100);
		}

		return 1;
	}

	int npc_is_near_vid(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid vid");
			lua_pushboolean(L, 0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find((DWORD)lua_tonumber(L, 1));
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		lua_Number dist = 10;

		if (lua_isnumber(L, 2))
			dist = lua_tonumber(L, 2);

		if (ch == NULL || npc == NULL)
		{
			lua_pushboolean(L, false);
		}
		else
		{
			lua_pushboolean(L, DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY()) < dist*100);
		}

		return 1;
	}

	int npc_unlock(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		if ( npc != NULL )
		{
			if (npc->IsPC())
				return 0;

			if (npc->GetQuestNPCID() == ch->GetPlayerID())
			{
				npc->SetQuestNPCID(0);
			}
		}
		return 0;
	}

	int npc_lock(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		if (!npc || npc->IsPC())
		{
			lua_pushboolean(L, TRUE);
			return 1;
		}

		if (npc->GetQuestNPCID() == 0 || npc->GetQuestNPCID() == ch->GetPlayerID())
		{
			npc->SetQuestNPCID(ch->GetPlayerID());
			lua_pushboolean(L, TRUE);
		}
		else
		{
			lua_pushboolean(L, FALSE);
		}

		return 1;
	}

	int npc_get_leader_vid(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		LPPARTY party = npc->GetParty();
		LPCHARACTER leader = party->GetLeader();

		if (leader)
			lua_pushnumber(L, leader->GetVID());
		else
			lua_pushnumber(L, 0);


		return 1;
	}

	int npc_get_vid(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();

		lua_pushnumber(L, npc->GetVID());


		return 1;
	}

	int npc_get_vid_attack_mul(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		lua_Number vid = lua_tonumber(L, 1);
		LPCHARACTER targetChar = CHARACTER_MANAGER::instance().Find(vid);

		if (targetChar)
			lua_pushnumber(L, targetChar->GetAttMul());
		else
			lua_pushnumber(L, 0);


		return 1;
	}

	int npc_set_vid_attack_mul(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		lua_Number vid = lua_tonumber(L, 1);
		lua_Number attack_mul = lua_tonumber(L, 2);

		LPCHARACTER targetChar = CHARACTER_MANAGER::instance().Find(vid);

		if (targetChar)
			targetChar->SetAttMul(attack_mul);

		return 0;
	}

	int npc_get_vid_damage_mul(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		lua_Number vid = lua_tonumber(L, 1);
		LPCHARACTER targetChar = CHARACTER_MANAGER::instance().Find(vid);

		if (targetChar)
			lua_pushnumber(L, targetChar->GetDamMul());
		else
			lua_pushnumber(L, 0);


		return 1;
	}

	int npc_set_vid_damage_mul(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		lua_Number vid = lua_tonumber(L, 1);
		lua_Number damage_mul = lua_tonumber(L, 2);

		LPCHARACTER targetChar = CHARACTER_MANAGER::instance().Find(vid);

		if (targetChar)
			targetChar->SetDamMul(damage_mul);

		return 0;
	}

	int npc_get_level(lua_State * L){
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();
		if (npc)
			lua_pushnumber(L, npc->GetLevel());
		else
			lua_pushnumber(L, 0);
		return 1;
	}

#ifdef ENABLE_ZODIAC_MISSION
	int npc_is_king(lua_State* L)
    {
		if (CQuestManager::instance().GetCurrentNPCCharacterPtr() != NULL)
			lua_pushboolean(L, CQuestManager::instance().GetCurrentNPCCharacterPtr()->GetMobRank() == MOB_RANK_KING);
		return 1;
	}

	int npc_is_boss(lua_State* L)
	{
		if (CQuestManager::instance().GetCurrentNPCCharacterPtr() != NULL)
			lua_pushboolean(L, CQuestManager::instance().GetCurrentNPCCharacterPtr()->GetMobRank() == MOB_RANK_BOSS);
		return 1;
	}
#endif

#ifdef ENABLE_MELEY_LAIR_DUNGEON
	int npc_get_protect_time(lua_State* L)
	{
		CQuestManager& q = CQuestManager::Instance();
		if (!lua_isnumber(L,1) || !lua_isstring(L,2))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			LPCHARACTER ch = 0;
			int a = lua_isnumber(L,1);
			const char* sz = lua_tostring(L, 2);
			if(a==1)
			{
				ch = q.GetCurrentNPCAttackCharacterPtr();
			}
			else if(a == 2)
			{
				ch = q.GetCurrentNPCCharacterPtr();
			}
			if(ch)
				lua_pushnumber(L,ch->GetProtectTime(string(sz)));
			else
				lua_pushnumber(L,0);
			return 1;
		}
		return 0;
	}

	int npc_set_protect_time(lua_State* L)
	{
		CQuestManager& q = CQuestManager::Instance();
		if (!lua_isnumber(L,1) || !lua_isstring(L,2) || !lua_isnumber(L,3))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			LPCHARACTER ch = 0;
			int a = lua_isnumber(L,1);
			const char* sz = lua_tostring(L,2);
			if(a==1)
			{
				ch = q.GetCurrentNPCAttackCharacterPtr();
			}
			else if(a==2)
			{
				ch = q.GetCurrentNPCCharacterPtr();
			}
			if(ch!=0)
				ch->SetProtectTime(sz, int(rint(lua_tonumber(L,3))));
		}
		return 0;
	}

	int npc_set_statu_affect(lua_State* L)
	{
		CQuestManager& q = CQuestManager::Instance();
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			LPCHARACTER ch = 0;
			int a = lua_isnumber(L,1);
			int sz = lua_tonumber(L,2);

			if(a==1)
			{
				ch = q.GetCurrentNPCAttackCharacterPtr();
			}
			else if(a==2)
			{
				ch = q.GetCurrentNPCCharacterPtr();
			}
			if(ch==0)
				return 0;

			if(sz==1)
			{
				if(!ch->FindAffect(AFFECT_STATUE))
					ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE1, 3600, 0, true);
			}
			else if(sz==2)
			{
				if(!ch->FindAffect(AFFECT_STATUE))
					ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE2, 3600, 0, true);
			}
			else if(sz==3)
			{
				if(!ch->FindAffect(AFFECT_STATUE))
					ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE3, 3600, 0, true);
			}
			else if(sz==4)
			{
				if(!ch->FindAffect(AFFECT_STATUE))
					ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE4, 3600, 0, true);
			}
		}
		return 0;
	}

	struct RemoveStatuEffect
	{
		RemoveStatuEffect() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsStone() || ch->IsMonster())
				{
					if(ch->FindAffect(AFFECT_STATUE))
					{
						ch->RemoveAffect(AFFECT_STATUE);
					}
				}
			}
		}
	};

	int npc_set_statu_affect_all(lua_State* L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		if (NULL != pSecMap)
		{
			RemoveStatuEffect f;
			pSecMap->for_each(f);
		}
		return 0;
	}

	struct MobDead
	{
		DWORD race;
		MobDead(DWORD a)
		{
			race = a;
		};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->GetRaceNum() == race)
				{
					ch->Dead();
				}
			}
		}
	};

	int npc_dead_by_vnum(lua_State* L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		if (NULL != pSecMap)
		{
			DWORD vnum = lua_tonumber(L,2);
			MobDead f(vnum);
			pSecMap->for_each(f);
		}
		return 0;
	}

	struct ShowStatuEffect
	{
		ShowStatuEffect() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsStone() || ch->IsMonster())
				{
					ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ(), true);
				}
			}
		}
	};

	int npc_show_statu(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		if (NULL != pSecMap)
		{
			ShowStatuEffect f;
			pSecMap->for_each(f);
		}
		return 0;
	}

	struct SetMeleyHP
	{
		DWORD race;
		long v;
		SetMeleyHP(DWORD vnum, long value)
		{
			race = vnum;
			v = value;
		};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->GetRaceNum() == race)
				{
					ch->SetHP(v);
				}
			}
		}
	};

	int npc_set_meley_hp(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		DWORD vnum = lua_tonumber(L,2);
		long value = lua_tonumber(L,3);
		if (NULL != pSecMap)
		{
			SetMeleyHP f(vnum,value);
			pSecMap->for_each(f);
		}
		return 0;
	}
	
	struct ShowEndEffect
	{
		ShowEndEffect() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsStone() || ch->IsMonster())
				{
					if(ch->FindAffect(AFFECT_STATUE))
					{
						ch->RemoveAffect(AFFECT_STATUE);
						ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE3, 3600, 0, true);
					}
					else
					{
						ch->AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE3, 3600, 0, true);
					}
				}
			}
		}
	};

	int npc_show_end_statu(lua_State*L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		if (NULL != pSecMap)
		{
			ShowEndEffect f;
			pSecMap->for_each(f);
		}
		return 0;
	}

	struct SetProtectFlag
	{
		const char* flag;
		int value;
		SetProtectFlag(const char* a, int v)
		{
			flag = a;
			value = v;
		};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsStone() || ch->IsMonster())
				{
					ch->SetProtectTime(flag,value);
				}
			}
		}
	};

	int npc_set_protect_flag2(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("Invalid Argument");
			return 0;
		}
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L,1));
		const char* flag = lua_tostring(L, 2);
		int value = lua_tonumber(L, 3);
		if (pSecMap == NULL || !pSecMap)
			return 0;
		if (pSecMap)
		{
			SetProtectFlag f(flag,value);
			pSecMap->for_each(f);
		}

		return 0;
	}
	
	int npc_get_hp(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		DWORD vid=0;
		LPCHARACTER targetChar=0;
		int p = lua_tonumber(L, 1);
		if(p==1)
		{
			targetChar = q.GetCurrentNPCAttackCharacterPtr();
		}
		else if(p==2)
		{
			vid = (DWORD) lua_tonumber(L, 2);
			targetChar = CHARACTER_MANAGER::instance().Find(vid);
		}

		if(!targetChar || targetChar == NULL)
			return 0;
		if (targetChar)
		{
			lua_pushnumber(L, targetChar->GetHP());
		}
		else
		{
			lua_pushnumber(L, 0);
		}
		return 1;
	}
	
	int npc_set_hp(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		DWORD vid=0;
		LPCHARACTER targetChar=0;

		int p = lua_tonumber(L, 1);
		int hp = lua_tonumber(L, 3);
		if(p==1)
		{
			targetChar = q.GetCurrentNPCAttackCharacterPtr();
		}
		else if(p==2)
		{
			vid = (DWORD) lua_tonumber(L, 2);
			targetChar = CHARACTER_MANAGER::instance().Find(vid);
		}

		if(!targetChar || targetChar == NULL)
			return 0;

		if (targetChar)
		{
			targetChar->SetHP(hp);
		}
		return 0;
	}
#endif

#ifdef ENABLE_NPC_IS_STONE
	int npc_is_stone(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER npc = q.GetCurrentNPCCharacterPtr();
		if (npc && npc->IsStone())
			lua_pushboolean(L, 1);
		else
			lua_pushboolean(L, 0);
		return 1;
	}
#endif

	void RegisterNPCFunctionTable()
	{
		luaL_reg npc_functions[] =
		{
			{ "get_level",			npc_get_level			},
			{ "getrace",			npc_get_race			},
			{ "get_race",			npc_get_race			},
			{ "open_shop",			npc_open_shop			},
			{ "get_empire",			npc_get_empire			},
			{ "is_pc",				npc_is_pc			},
			{ "is_quest",			npc_is_quest			},
			{ "kill",				npc_kill			},
			{ "purge",				npc_purge			},
			{ "is_near",			npc_is_near			},
			{ "is_near_vid",			npc_is_near_vid			},
			{ "lock",				npc_lock			},
			{ "unlock",				npc_unlock			},
			{ "get_guild",			npc_get_guild			},
			{ "get_leader_vid",		npc_get_leader_vid	},
			{ "get_vid",			npc_get_vid	},
			{ "get_vid_attack_mul",		npc_get_vid_attack_mul	},
			{ "set_vid_attack_mul",		npc_set_vid_attack_mul	},
			{ "get_vid_damage_mul",		npc_get_vid_damage_mul	},
			{ "set_vid_damage_mul",		npc_set_vid_damage_mul	},

			// X-mas santa special
			{ "get_remain_skill_book_count",	npc_get_remain_skill_book_count },
			{ "dec_remain_skill_book_count",	npc_dec_remain_skill_book_count },
			{ "get_remain_hairdye_count",	npc_get_remain_hairdye_count	},
			{ "dec_remain_hairdye_count",	npc_dec_remain_hairdye_count	},
#ifdef ENABLE_ZODIAC_MISSION
			{ "is_boss",	npc_is_boss	},
			{ "is_king",	npc_is_king	},
#endif
#ifdef ENABLE_MELEY_LAIR_DUNGEON
			{ "get_protect_time", npc_get_protect_time},
			{ "set_protect_time", npc_set_protect_time},
			{ "set_statu_affect", npc_set_statu_affect},
			{ "set_statu_clear_aff", npc_set_statu_affect_all},
			{ "show_statu", npc_show_statu},
			{ "set_meley_hp", npc_set_meley_hp},
			{ "show_end_statu", npc_show_end_statu},
			{ "set_protect_flag2", npc_set_protect_flag2},
			{ "dead_by_vnum", npc_dead_by_vnum},
			{ "set_hp", npc_set_hp},
			{ "get_hp", npc_get_hp},
#endif
#ifdef ENABLE_NPC_IS_STONE
			{ "is_stone", npc_is_stone },
#endif
			{ NULL,				NULL			    	}
		};

		CQuestManager::instance().AddLuaFunctionTable("npc", npc_functions);
	}
};
