#include "stdafx.h"

#ifdef ENABLE_BOT_PLAYER
#include "BotPlayer.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "config.h"
#include "p2p.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "vector.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "entity.h"
#include "questmanager.h"
#include "../../common/length.h"  // ENABLE_MULTI_LANGUAGE_SYSTEM için gerekli

// Forward declarations
extern void SendShout(const char* szText, BYTE bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
	, const char* c_szName
#endif
);
#include "PetSystem.h"
#include "horsename_manager.h"
#include "horse_rider.h"
#include "motion.h"
#include "skill.h"
#include "ani.h"
#include "battle.h"
#include "locale_service.h"
#include "buffer_manager.h"
#include "locale.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <map>

// Empire map artık kullanılmıyor - bot'lar spawn eden oyuncunun yanına spawn olur
// const std::unordered_map<int8_t, std::tuple<int32_t, int32_t, int32_t>> empire_map = {
// 	{1, {1, 459800, 953900}},
// 	{2, {21, 52070, 166600}},
// 	{3, {41, 957300, 255200}}
// };

// İmparatorluk ve sınıf isimleri (PvP logları için)
static const std::string jobArray[] = { "Savaşçı", "Ninja", "Sura", "Şaman" };
static const std::string empireArray[] = { "Shinsoo", "Chunjo", "Jinno" };

EVENTINFO(bot_character_event_info)
{
	DynamicCharacterPtr botCharacter;
	uint32_t savedMountVnum; // Saldırı öncesi kullanılan binek VNUM'u
	bool wasMounted; // Saldırı öncesi binekli miydi?
	DWORD m_dwSkillNextTime; // Skill kullanımı için timer (Buffi mantığı)
	size_t m_lastSkillIndex; // Son kullanilan skill indeksi (6 skill rotasyonu icin)
	std::map<DWORD, DWORD> m_buffSkillCooldownEnd; // Buff skill soguma: skillVnum -> get_dword_time() bitis
	DWORD m_dwLastVictimTime;  // Son hedef bulma zamani (idle icin)
	DWORD m_dwIdleUntil;       // Bu zamana kadar bekle (sag-sol yapmasin)
	
	bot_character_event_info() :
		botCharacter(),
		savedMountVnum(0),
		wasMounted(false),
		m_dwSkillNextTime(0),
		m_lastSkillIndex(0),
		m_dwLastVictimTime(0),
		m_dwIdleUntil(0)
	{
	}
};

class FuncFindVictim
{
public:
	FuncFindVictim(LPCHARACTER pkChr, int iMaxDistance) : m_pkChr(pkChr),
		m_iMinDistance(INT_MAX),
		m_iMaxDistance(iMaxDistance),
		m_lx(pkChr->GetX()),
		m_ly(pkChr->GetY()),
		m_pkChrVictim(NULL),
		m_bFoundBoss(false) {};

	void operator()(LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
			return;
		LPCHARACTER pkChr = (LPCHARACTER)ent;
		if (!pkChr)
			return;
		// Kendini hedefleme
		if (pkChr == m_pkChr)
			return;
		// Ölü hedefleri atla
		if (pkChr->IsDead())
			return;
		
		int iDistance = DISTANCE_APPROX(m_lx - pkChr->GetX(), m_ly - pkChr->GetY());

		// Hedef öncelik sistemi
		bool isValidTarget = false;
		int targetPriority = 0; // 0=düşük, 1=orta, 2=yüksek, 3=çok yüksek
		
		// 1. Metin taşları ve Boss'lar (en yüksek öncelik)
		if (pkChr->IsStone() || (pkChr->IsMonster() && pkChr->GetMobRank() >= MOB_RANK_BOSS))
		{
			isValidTarget = true;
			targetPriority = 3;
		}
		// 2. Dusman imparatorluk oyunculari (GM haric) (yuksek oncelik)
		else if (pkChr->IsPC() && !pkChr->IsBotCharacter() && pkChr->GetEmpire() != m_pkChr->GetEmpire())
		{
			if (pkChr->GetGMLevel() == GM_PLAYER)
			{
				isValidTarget = true;
				targetPriority = 2;
			}
		}
		// 3. Dusman imparatorluk botlari (orta oncelik)
		else if (pkChr->IsPC() && pkChr->IsBotCharacter() && pkChr->GetEmpire() != m_pkChr->GetEmpire())
		{
			isValidTarget = true;
			targetPriority = 1;
		}
		// 4. Normal moblar (düşük öncelik)
		else if (pkChr->IsMonster())
		{
			isValidTarget = true;
			targetPriority = 0;
		}
		
		if (!isValidTarget)
			return;
		
		// Öncelik bazlı hedef seçimi
		if (iDistance <= m_iMaxDistance)
		{
			// Daha yüksek öncelikli hedef bulundu
			if (targetPriority > m_iCurrentPriority)
			{
				m_pkChrVictim = pkChr;
				m_iMinDistance = iDistance;
				m_iCurrentPriority = targetPriority;
				m_bFoundBoss = (targetPriority >= 3);
			}
			// Aynı öncelikte ama daha yakın hedef
			else if (targetPriority == m_iCurrentPriority && iDistance < m_iMinDistance)
		{
			m_pkChrVictim = pkChr;
			m_iMinDistance = iDistance;
				if (targetPriority >= 3)
				m_bFoundBoss = true;
			}
		}
	}
	LPCHARACTER GetVictim() { return (m_pkChrVictim); }
private:
	LPCHARACTER m_pkChr;
	int m_iMinDistance;
	int m_iMaxDistance;
	long m_lx;
	long m_ly;
	LPCHARACTER m_pkChrVictim;
	bool m_bFoundBoss;
	int m_iCurrentPriority = -1; // Hedef öncelik seviyesi
};

// Sinif bazli skill tanimlamalari - Saldiri + buff skillleri (buff'lar soguma doldugunda kullanilir)
static std::vector<DWORD> GetClassSkills(BYTE job, BYTE skillGroup)
{
	if (skillGroup == 1)
	{
		if (job == JOB_WARRIOR)      return {SKILL_SAMYEON, SKILL_PALBANG, SKILL_JEONGWI, SKILL_GEOMKYUNG, SKILL_TANHWAN, 6};
		if (job == JOB_ASSASSIN)     return {SKILL_AMSEOP, SKILL_GUNGSIN, SKILL_CHARYUN, SKILL_EUNHYUNG, SKILL_SANGONG, 36};
		if (job == JOB_SURA)         return {SKILL_SWAERYUNG, SKILL_YONGKWON, SKILL_GWIGEOM, SKILL_TERROR, SKILL_JUMAGAP, SKILL_PABEOB};
		if (job == JOB_SHAMAN)       return {SKILL_BIPABU, SKILL_YONGBI, SKILL_PAERYONG, SKILL_HOSIN, SKILL_REFLECT, SKILL_GICHEON};
	}
	else if (skillGroup == 2)
	{
		if (job == JOB_WARRIOR)      return {SKILL_GIGONGCHAM, SKILL_GYOKSAN, SKILL_DAEJINGAK, SKILL_CHUNKEON, SKILL_GEOMPUNG, 21};
		if (job == JOB_ASSASSIN)     return {SKILL_YEONSA, SKILL_KWANKYEOK, SKILL_HWAJO, SKILL_GYEONGGONG, SKILL_GIGUNG, 51};
		if (job == JOB_SURA)         return {SKILL_MARYUNG, SKILL_HWAYEOMPOK, SKILL_MUYEONG, SKILL_MANASHILED, SKILL_TUSOK, SKILL_MAHWAN};
		if (job == JOB_SHAMAN)       return {SKILL_NOEJEON, SKILL_BYEURAK, SKILL_CHAIN, SKILL_JEONGEOP, SKILL_KWAESOK, SKILL_JEUNGRYEOK};
	}
	return {};
}

// Saldirida kullanilmayan buff/ozel skilller - hedef olmadan kullanilabilir (STANDING_SKILL)
// skilldesc.txt'e gore: Ofke, Hava Kilici, Guclu Beden, Kamuflaj, Hafif Adim, vs.
// Botlar bu skillleri hedef yokken de kullanir (soguma sureleri mevcut: ~92sn, ~100sn)
static std::vector<DWORD> GetClassBuffSkills(BYTE job, BYTE skillGroup)
{
	if (skillGroup == 1)
	{
		if (job == JOB_WARRIOR)      return {SKILL_JEONGWI, SKILL_GEOMKYUNG};           // Ofke, Hava Kilici
		if (job == JOB_ASSASSIN)     return {SKILL_EUNHYUNG};                           // Kamuflaj
		if (job == JOB_SURA)         return {SKILL_GWIGEOM, SKILL_TERROR, SKILL_JUMAGAP}; // Buyulu Keskinlik, Deset, Buyulu Zirh
		if (job == JOB_SHAMAN)       return {SKILL_HOSIN, SKILL_REFLECT, SKILL_GICHEON};  // Kutsama, Yansitma, Ejderha Yardimi
	}
	else if (skillGroup == 2)
	{
		if (job == JOB_WARRIOR)      return {SKILL_CHUNKEON};                            // Guclu Beden
		if (job == JOB_ASSASSIN)     return {SKILL_GYEONGGONG};                          // Hafif Adim
		if (job == JOB_SURA)         return {SKILL_MUYEONG, SKILL_MANASHILED};          // Ates Hayaleti, Karanlik Koruma
		if (job == JOB_SHAMAN)       return {SKILL_JEONGEOP, SKILL_KWAESOK, SKILL_JEUNGRYEOK}; // Iyilestirme, Cabukluk, Yuksek Saldiri
	}
	return {};
}

EVENTFUNC(bot_character_event)
{
	auto info = dynamic_cast<bot_character_event_info*>(event->info);
	if (!info)
		return 0;

	LPCHARACTER botCharacter = info->botCharacter;
	if (!botCharacter)
		return 0;

	// Standing mount üzerindeyken botun silah parçasını her tikte garantiye al
#ifdef ENABLE_STANDING_MOUNT
	if (botCharacter->GetMountVnum() && IS_STANDING_MOUNT_VNUM(botCharacter->GetMountVnum()))
	{
		LPITEM __w = botCharacter->GetWear(WEAR_WEAPON);
		if (__w)
		{
			WORD __partWeapon = botCharacter->GetPart(PART_WEAPON);
			if (__partWeapon != __w->GetVnum())
				botCharacter->SetPart(PART_WEAPON, __w->GetVnum());
		}
		else
		{
			int savedWeaponVnum = CBotCharacterManager::instance().GetBotWeaponVnum(botCharacter->GetName());
			if (savedWeaponVnum > 0 && botCharacter->GetPart(PART_WEAPON) != savedWeaponVnum)
				botCharacter->SetPart(PART_WEAPON, savedWeaponVnum);
		}
	}
#endif

	auto& botManager = CBotCharacterManager::instance();

	LPDESC d = DESC_MANAGER::instance().FindByCharacterName(botCharacter->GetName());
	LPCHARACTER tch = d ? d->GetCharacter() : nullptr;
	if (tch)
	{
		botManager.BotCharacterRemove(tch->GetName());
		sys_err("Real player entered bot %s has been deleted.", tch->GetName());
		return 0;
	}

	// Bot karakterlerin otomatik potion kullanımı - Her event'te kontrol et
	// HP %70'in altına düştüğünde HP potion kullan (daha agresif)
	if (botCharacter->GetMaxHP() > 0)
	{
		int currentHP = botCharacter->GetHP();
		int maxHP = botCharacter->GetMaxHP();
		int hpPercent = (maxHP > 0) ? (currentHP * 100 / maxHP) : 0;
		
		if (hpPercent < 70)
		{
			sys_log(0, "Bot %s: [POTION CHECK] HP düşük! HP: %d/%d (%d%%)", 
			        botCharacter->GetName(), currentHP, maxHP, hpPercent);
			
			// Kuşak envanterinden HP potion bul ve kullan
			bool potionUsed = false;
			for (int i = 0; i < 3; i++)
			{
				int slotIndex = BELT_INVENTORY_SLOT_START + i;
				LPITEM hpPotion = botCharacter->GetInventoryItem(slotIndex);
				
				if (hpPotion)
				{
					sys_log(0, "Bot %s: [POTION CHECK] Slot %d: VNUM=%d, Count=%d", 
					        botCharacter->GetName(), i, hpPotion->GetVnum(), hpPotion->GetCount());
					
					if (hpPotion->GetCount() > 0)
					{
						sys_log(0, "Bot %s: [POTION USE] HP potion kullanılıyor (Slot: %d, VNUM: %d)", 
						        botCharacter->GetName(), i, hpPotion->GetVnum());
						
						bool useResult = botCharacter->UseItemEx(hpPotion, TItemPos(BELT_INVENTORY, i));
						sys_log(0, "Bot %s: [POTION USE] UseItemEx sonucu: %s (HP: %d/%d)", 
						        botCharacter->GetName(), useResult ? "BAŞARILI" : "BAŞARISIZ", 
						        botCharacter->GetHP(), botCharacter->GetMaxHP());
						potionUsed = true;
						break;
					}
				}
			}
			
			if (!potionUsed)
			{
				sys_log(0, "Bot %s: [POTION WARNING] HP potion bulunamadı veya kullanılamadı!", 
				        botCharacter->GetName());
			}
		}
	}
	
	// SP %50'nin altına düştüğünde SP potion kullan
	if (botCharacter->GetMaxSP() > 0 && botCharacter->GetSP() < (botCharacter->GetMaxSP() * 50 / 100))
	{
		// Kuşak envanterinden SP potion bul ve kullan
		for (int i = 3; i < 6; i++)
		{
			int slotIndex = BELT_INVENTORY_SLOT_START + i;
			LPITEM spPotion = botCharacter->GetInventoryItem(slotIndex);
			if (spPotion && spPotion->GetCount() > 0)
			{
				botCharacter->UseItemEx(spPotion, TItemPos(BELT_INVENTORY, i));
				sys_log(0, "Bot %s: SP potion kullandı (SP: %d/%d, Slot: %d)", 
				        botCharacter->GetName(), botCharacter->GetSP(), botCharacter->GetMaxSP(), i);
				break;
			}
		}
	}

	// Buff/destek skillleri - SADECE soguma sresi doldugunda kullan (surekli basma)
	// Korumali alanda buff da kullanma (tutarlilik icin)
	if (botCharacter->GetSectree() && botCharacter->GetSectree()->IsAttr(botCharacter->GetX(), botCharacter->GetY(), ATTR_BANPK))
	{
		// Korumali alanda hicbir saldiri/buff aksiyonu yapma
	}
	else
	{
		// Ofke, Hava Kilici, Kamuflaj, Kutsama vb. - skill kapandiginda tekrar bas
		BYTE job = botCharacter->GetJob();
		BYTE skillGroup = botCharacter->GetSkillGroup();
		if (skillGroup == 0) skillGroup = 1;
		auto buffSkills = GetClassBuffSkills(job, skillGroup);
		const DWORD now = get_dword_time();
		
		for (DWORD skillVnum : buffSkills)
		{
			if (botCharacter->GetSkillLevel(skillVnum) == 0)
				continue;
			if (botCharacter->GetSP() < 30)
				break;
			
			auto it = info->m_buffSkillCooldownEnd.find(skillVnum);
			if (it != info->m_buffSkillCooldownEnd.end() && now < it->second)
				continue;
			
			CSkillProto* pkSk = CSkillManager::instance().Get(skillVnum);
			if (!pkSk)
				continue;
			
			float k = 1.0f * botCharacter->GetSkillPower(skillVnum) * pkSk->bMaxLevel / 100;
			pkSk->kCooldownPoly.SetVar("k", k);
			int iCooltimeSec = (int) pkSk->kCooldownPoly.Eval();
			if (iCooltimeSec <= 0)
				iCooltimeSec = 90;
			
			bool useOk = botCharacter->UseSkill(skillVnum, botCharacter, false);
			if (useOk)
			{
				info->m_buffSkillCooldownEnd[skillVnum] = now + (iCooltimeSec * 1000);
				sys_log(0, "Bot %s: Buff skill (soguma doldu) - Skill:%u, Soguma:%dsn", 
				        botCharacter->GetName(), skillVnum, iCooltimeSec);
				return PASSES_PER_SEC(0.5f);
			}
		}
	}

	FuncFindVictim f(botCharacter, 1000);		//metin alan hesaplama
	if (botCharacter->GetSectree())
	{
		botCharacter->GetSectree()->ForEachAround(f);
	}

	auto victim = f.GetVictim();
	// Sectree dusman bot bulamadiysa, BotCharacterManager uzerinden dene (bot vs bot icin)
	if (!victim)
	{
		victim = CBotCharacterManager::instance().FindNearestEnemyBot(botCharacter, 1000);
	}
	// Korumali alan (ATTR_BANPK) - bot burada ise saldiri/skill kullanma
	if (victim && botCharacter->GetSectree() && botCharacter->GetSectree()->IsAttr(botCharacter->GetX(), botCharacter->GetY(), ATTR_BANPK))
		victim = nullptr;
	if (victim)
	{
		info->m_dwLastVictimTime = get_dword_time();  // Hedef var, idle sayacini sifirla
		// Bot SP her zaman full - skill kullanimi icin (botlar icin ST kullanilmiyor)
		if (botCharacter->GetSP() < botCharacter->GetMaxSP())
			botCharacter->PointChange(POINT_SP, botCharacter->GetMaxSP() - botCharacter->GetSP());

		// Okcu ninjalar icin ok kontrolu - yoksa veya azsa Gumus Ok ekle (sinirsiz)
		if (botCharacter->GetJob() == JOB_ASSASSIN)
		{
			LPITEM weapon = botCharacter->GetWear(WEAR_WEAPON);
			if (weapon && weapon->GetSubType() == WEAPON_BOW)
			{
				LPITEM arrow = botCharacter->GetWear(WEAR_ARROW);
				if (!arrow || arrow->GetCount() < 100)
				{
					if (arrow)
						arrow->SetCount(99999);
					else
					{
						LPITEM newArrow = ITEM_MANAGER::instance().CreateItem(8005, 99999);
						if (newArrow)
						{
							int pos = botCharacter->GetEmptyInventory(newArrow->GetSize());
							if (pos >= 0 && newArrow->AddToCharacter(botCharacter, TItemPos(INVENTORY, pos)))
								botCharacter->EquipItem(newArrow);
							else
								M2_DESTROY_ITEM(newArrow);
						}
					}
				}
			}
		}
		
		// Hedef mesafesi hesapla
		int distance = DISTANCE_APPROX(botCharacter->GetX() - victim->GetX(), 
		                                botCharacter->GetY() - victim->GetY());
		
		// Saldırı menzili (normal: 170, okçu: 800)
		int attackRange = 170;
		BYTE job = botCharacter->GetJob();
		if (job == JOB_ASSASSIN && botCharacter->GetWear(WEAR_WEAPON))
		{
			LPITEM weapon = botCharacter->GetWear(WEAR_WEAPON);
			if (weapon && weapon->GetSubType() == WEAPON_BOW)
				attackRange = 800; // Okçu menzili
		}
		
		// Sadece boss, metin taşı ve patronlara saldırırken bindir
		bool shouldMount = victim->IsStone() || 
		                   (victim->IsMonster() && victim->GetMobRank() >= MOB_RANK_BOSS);
		
#ifdef ENABLE_STANDING_MOUNT
		// ENABLE_STANDING_MOUNT: Tüm botlar standing mount kullanabilir (Şampiyon şartı yok)
		if (shouldMount && !botCharacter->IsRiding())
		{
			// Saldırı öncesi binek durumunu kaydet
			if (!info->wasMounted)
			{
				info->wasMounted = false; // Zaten binekli değildi
				info->savedMountVnum = 0;
			}
			// Mevcut silah vnum'unu sakla (gerektiğinde geri set edebilmek için)
			if (LPITEM __w = botCharacter->GetWear(WEAR_WEAPON))
				CBotCharacterManager::instance().SetBotWeaponVnum(botCharacter->GetName(), __w->GetVnum());
			
			// Standing mount mob vnumları: 40003, 40004, 40005
			uint32_t standingMountVnums[] = {40003, 40004, 40005};
			uint32_t selectedMountVnum = standingMountVnums[number(0, 2)]; // 3 standing mount arasından rastgele
			
			// Standing mount'u Horse sistemi ile çağır (mob VNUM ile)
			botCharacter->HorseSummon(true, true, selectedMountVnum);
			botCharacter->StartRiding();
			// Standing mount'a biner binmez PART_WEAPON'ı garanti altına al
			{
				int savedWeaponVnum = CBotCharacterManager::instance().GetBotWeaponVnum(botCharacter->GetName());
				WORD partWeapon = botCharacter->GetPart(PART_WEAPON);
				LPITEM wearWeapon = botCharacter->GetWear(WEAR_WEAPON);
				DWORD ensureVnum = wearWeapon ? wearWeapon->GetVnum() : (savedWeaponVnum > 0 ? (DWORD)savedWeaponVnum : 0);
				if (ensureVnum > 0 && partWeapon != ensureVnum)
					botCharacter->SetPart(PART_WEAPON, (WORD)ensureVnum);
			}
			
			sys_log(0, "Bot %s: Boss/Metin için Standing Mount eklendi (VNUM: %d)", 
			        botCharacter->GetName(), selectedMountVnum);
		}
		else if (!shouldMount && botCharacter->IsRiding())
		{
			// Hedef öldü veya normal mob - attan in
			botCharacter->StopRiding();
			
			// Eski bineğe geri dön (eğer önceden binekliyse)
			if (info->wasMounted && info->savedMountVnum > 0)
			{
				botCharacter->HorseSummon(true, true, info->savedMountVnum);
				botCharacter->StartRiding();
				sys_log(0, "Bot %s: Eski bineğe geri döndü (VNUM: %d)", 
				        botCharacter->GetName(), info->savedMountVnum);
			}
			else
			{
				sys_log(0, "Bot %s: Binekten indi (yürüyerek)", 
				        botCharacter->GetName());
			}
			
			// Binek durumunu sıfırla
			info->wasMounted = false;
			info->savedMountVnum = 0;
		}
#else
		// Eski şampiyon sistemi kullanılmıyor; normal at mantığı devre dışı
#endif

		// Hedefe yaklaşmak için hareket et
		if (distance > attackRange)
		{
			long destX = victim->GetX(), destY = victim->GetY();
			// Bot VID ile benzersiz offset: hedef etrafinda 8 yon (gercek oyuncu gibi carpisma)
			static const int offsetX[] = {50, 35, 0, -35, -50, -35, 0, 35};
			static const int offsetY[] = {0, 35, 50, 35, 0, -35, -50, -35};
			int idx = (botCharacter->GetVID() * 7) % 8;
			destX += offsetX[idx];
			destY += offsetY[idx];
			
			if (SECTREE_MANAGER::instance().IsMovablePosition(victim->GetMapIndex(), destX, destY))
			{
				float dx = (float)(destX - botCharacter->GetX());
				float dy = (float)(destY - botCharacter->GetY());
				botCharacter->SetRotation(GetDegreeFromPosition(dx, dy));
				
				if (botCharacter->Goto(destX, destY))
				{
					botCharacter->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
				}
				
				return PASSES_PER_SEC(0.08f); // Hedefe yaklarken dengeli bekleme
			}
		}

		// Hedefe yeterince yaklaştık, saldır
		if (distance <= attackRange && SECTREE_MANAGER::instance().IsMovablePosition(victim->GetMapIndex(), victim->GetX(), victim->GetY()))
		{
			// Hedefe dön
			float dx = victim->GetX() - botCharacter->GetX();
			float dy = victim->GetY() - botCharacter->GetY();
			botCharacter->SetRotation(GetDegreeFromPosition(dx, dy));
			
			// DitoSystem mantığı ile skill kullanımı - Timer kontrolü
			const DWORD now = get_dword_time();
			bool canUseSkill = (info->m_dwSkillNextTime <= now);
			
			sys_log(0, "Bot %s: [SKILL CHECK] canUseSkill=%d, m_dwSkillNextTime=%u, now=%u, victim=%s", 
			        botCharacter->GetName(), canUseSkill ? 1 : 0, info->m_dwSkillNextTime, now, victim->GetName());
			
			if (canUseSkill)
			{
				// Skill arasi bekleme k?salt?ld? (mant?kl? s?ra için daha s?k deneme)
				info->m_dwSkillNextTime = now + 1500;
				
				std::vector<DWORD> skills = GetClassSkills(job, botCharacter->GetSkillGroup());
				auto buffSkills = GetClassBuffSkills(job, botCharacter->GetSkillGroup());
				std::set<DWORD> buffSkillSet(buffSkills.begin(), buffSkills.end());
				
				// Önce sald?r? skilleri, ard?ndan buff skilleri (mant?ken s?rayla)
				std::vector<DWORD> reordered;
				for (DWORD sv : skills)
				{
					if (buffSkillSet.find(sv) == buffSkillSet.end())
						reordered.push_back(sv);
				}
				for (DWORD sv : skills)
				{
					if (buffSkillSet.find(sv) != buffSkillSet.end())
						reordered.push_back(sv);
				}
				
				int currentSP = botCharacter->GetSP();
				int maxSP = botCharacter->GetMaxSP();
				
				if (currentSP < 30)
				{
					sys_log(0, "Bot %s: [SKILL SKIP] SP yetersiz (%d < 30), normal saldiriya gec", 
					        botCharacter->GetName(), currentSP);
				}
				else if (reordered.empty())
				{
					sys_log(0, "Bot %s: [SKILL SKIP] Skill listesi boş!", 
					        botCharacter->GetName());
				}
				else
				{
					const size_t skillCount = reordered.size();
					for (size_t i = 0; i < skillCount; ++i)
					{
						size_t idx = (info->m_lastSkillIndex + 1 + i) % skillCount;
						DWORD skillVnum = reordered[idx];
						
						if (botCharacter->GetSkillLevel(skillVnum) == 0)
							botCharacter->SetSkillLevel(skillVnum, SKILL_MAX_LEVEL);
						
						bool isBuffSkill = (buffSkillSet.find(skillVnum) != buffSkillSet.end());
						if (isBuffSkill)
						{
							auto it = info->m_buffSkillCooldownEnd.find(skillVnum);
							if (it != info->m_buffSkillCooldownEnd.end() && now < it->second)
								continue;
						}
						
						LPCHARACTER skillTarget = isBuffSkill ? botCharacter : victim;
						bool useOk = botCharacter->UseSkill(skillVnum, skillTarget, false);
						
						if (useOk)
						{
							info->m_lastSkillIndex = idx;
							if (isBuffSkill)
							{
								CSkillProto* pkSk = CSkillManager::instance().Get(skillVnum);
								int iCooltimeSec = 90;
								if (pkSk)
								{
									float k = 1.0f * botCharacter->GetSkillPower(skillVnum) * pkSk->bMaxLevel / 100;
									pkSk->kCooldownPoly.SetVar("k", k);
									iCooltimeSec = (int)pkSk->kCooldownPoly.Eval();
								}
								if (iCooltimeSec <= 0) iCooltimeSec = 90;
								info->m_buffSkillCooldownEnd[skillVnum] = now + (iCooltimeSec * 1000);
							}
							return PASSES_PER_SEC(0.5f);
						}
					}
				}
			}
			
			// Normal saldırı - Profesyonel kombo sistemi
			// Bot karakterlerin silahının korunması - Saldırı öncesi kontrol
			WORD currentWeapon = botCharacter->GetPart(PART_WEAPON);
			if (currentWeapon == 0)
			{
				// Silah kaybolmuş, quest flag'den silah vnum'ını al ve yeniden set et
				int savedWeaponVnum = CBotCharacterManager::instance().GetBotWeaponVnum(botCharacter->GetName());
				if (savedWeaponVnum > 0)
				{
					botCharacter->SetPart(PART_WEAPON, savedWeaponVnum);
					sys_log(0, "Bot %s: Silah kaybolmuş, quest flag'den yeniden set edildi (Weapon: %d)", 
						botCharacter->GetName(), savedWeaponVnum);
				}
				else
				{
					// Quest flag yoksa, job ve skill grubuna gore silah sec
					BYTE job = botCharacter->GetJob();
					BYTE skillGroup = botCharacter->GetSkillGroup();
					if (skillGroup == 0) skillGroup = 1;
					uint32_t weaponVnum = 0;
					
					if (job == JOB_WARRIOR)
						weaponVnum = number(0, 1) == 0 ? 19 : 3009;
					else if (job == JOB_ASSASSIN)
						weaponVnum = (skillGroup == 2) ? 2009 : 1009;
					else if (job == JOB_SURA)
						weaponVnum = 49;
					else if (job == JOB_SHAMAN)
						weaponVnum = number(0, 1) == 0 ? 5009 : 7009;
					
					if (weaponVnum > 0)
					{
						botCharacter->SetPart(PART_WEAPON, weaponVnum);
						CBotCharacterManager::instance().SetBotWeaponVnum(botCharacter->GetName(), weaponVnum);
						sys_log(0, "Bot %s: Silah kaybolmuş, job'a göre yeniden set edildi (Weapon: %d, Job: %d)", 
							botCharacter->GetName(), weaponVnum, job);
						// Yay restore edildiyse Gumus Ok ekle
						if (job == JOB_ASSASSIN && skillGroup == 2)
						{
							LPITEM arrow = botCharacter->GetWear(WEAR_ARROW);
							if (!arrow || arrow->GetCount() < 100)
							{
								if (arrow)
									arrow->SetCount(99999);
								else
								{
									LPITEM newArrow = ITEM_MANAGER::instance().CreateItem(8005, 99999);
									if (newArrow)
									{
										int apos = botCharacter->GetEmptyInventory(newArrow->GetSize());
										if (apos >= 0 && newArrow->AddToCharacter(botCharacter, TItemPos(INVENTORY, apos)))
											botCharacter->EquipItem(newArrow);
										else
											M2_DESTROY_ITEM(newArrow);
									}
								}
							}
						}
					}
				}
			}
			// Standing mount'tayken saldırıdan önce part'ı giyili silahla eşitle
#ifdef ENABLE_STANDING_MOUNT
			if (botCharacter->GetMountVnum() && IS_STANDING_MOUNT_VNUM(botCharacter->GetMountVnum()))
			{
				LPITEM __w = botCharacter->GetWear(WEAR_WEAPON);
				if (__w)
				{
					WORD __partWeapon = botCharacter->GetPart(PART_WEAPON);
					if (__partWeapon != __w->GetVnum())
					{
						botCharacter->SetPart(PART_WEAPON, __w->GetVnum());
						sys_log(0, "Bot %s: Standing mount'ta saldırı öncesi PART_WEAPON düzeltildi (%u)", botCharacter->GetName(), __w->GetVnum());
					}
				}
				else
				{
					int savedWeaponVnum = CBotCharacterManager::instance().GetBotWeaponVnum(botCharacter->GetName());
					if (savedWeaponVnum > 0 && botCharacter->GetPart(PART_WEAPON) != savedWeaponVnum)
					{
						botCharacter->SetPart(PART_WEAPON, savedWeaponVnum);
						sys_log(0, "Bot %s: Standing mount'ta saldırı öncesi PART_WEAPON quest flag'den geri yüklendi (%d)",
							botCharacter->GetName(), savedWeaponVnum);
					}
				}
			}
#endif
			
			// Attack() fonksiyonu zaten gerekli paketleri gönderir
			if (botCharacter->Attack(victim))
			{
				// Standing mount'ta saldırıdan sonra da PART_WEAPON'ı koru
#ifdef ENABLE_STANDING_MOUNT
				if (botCharacter->GetMountVnum() && IS_STANDING_MOUNT_VNUM(botCharacter->GetMountVnum()))
				{
					int savedWeaponVnum = CBotCharacterManager::instance().GetBotWeaponVnum(botCharacter->GetName());
					LPITEM wearWeapon = botCharacter->GetWear(WEAR_WEAPON);
					WORD want = wearWeapon ? (WORD)wearWeapon->GetVnum() : (savedWeaponVnum > 0 ? (WORD)savedWeaponVnum : 0);
					if (want > 0 && botCharacter->GetPart(PART_WEAPON) != want)
						botCharacter->SetPart(PART_WEAPON, want);
				}
#endif
				// PvP saldırı logu
				bool isPvPTarget = victim->IsPC();
				if (isPvPTarget && number(1, 10) == 1) // Her 10 saldırıda bir log
				{
					sys_log(0, "Bot %s (%s) düşman %s (%s) oyuncusuna saldırıyor! (HP: %d/%d)", 
						botCharacter->GetName(), 
						empireArray[botCharacter->GetEmpire() - 1].c_str(),
						victim->GetName(),
						empireArray[victim->GetEmpire() - 1].c_str(),
						victim->GetHP(), victim->GetMaxHP());
				}
				
				// Kombo dizisini sürekli artır (1-5 arası döngü)
				int currentCombo = botCharacter->GetComboSequence();
				int nextCombo = (currentCombo % 5) + 1;
				
				botCharacter->SetComboSequence(nextCombo);
				
				// Kombo index hesapla (0-4 arası)
				int comboIndex = (nextCombo - 1) % 5;
				
				DWORD dwTime = get_dword_time();
				int interval = dwTime - botCharacter->GetLastComboTime();
				botCharacter->SetValidComboInterval(interval);
				botCharacter->SetLastComboTime(dwTime);
				
				// Kombo motion gönder - SendMovePacket zaten client'a animasyon paketini gnderir
				botCharacter->SendMovePacket(FUNC_COMBO, MOTION_COMBO_ATTACK_1 + comboIndex, 
				                           botCharacter->GetX(), botCharacter->GetY(), 0, dwTime);
				
				// Saldırı animasyonu için dengeli bekleme süresi
				return PASSES_PER_SEC(0.25f); // Hizli kombo saldırı hızı
			}
		}
	}
	else
	{
		if (botCharacter->IsHorseRiding() && std::rand() % 5 == 0)
			botCharacter->StopRiding();

		const DWORD now = get_dword_time();
		// Idle: hedef yokken belirli sure dur (sag-sol yapmasin)
		if (info->m_dwLastVictimTime == 0)
			info->m_dwLastVictimTime = now;
		if (now < info->m_dwIdleUntil)
		{
			// Idle suresi dolana kadar bekle
			return PASSES_PER_SEC(2.0f);
		}
		if (now - info->m_dwLastVictimTime > 5000 && info->m_dwIdleUntil == 0)
		{
			// 5 sn hedef yok, 4 sn idle (bir yerde dur)
			info->m_dwIdleUntil = now + 4000;
			return PASSES_PER_SEC(2.0f);
		}
		if (info->m_dwIdleUntil > 0 && now > info->m_dwIdleUntil)
			info->m_dwIdleUntil = 0;  // Idle bitti, sifirla

		// Nadiren hareket (1/10 - aptal gibi sag sol yapmasin), guvenli bolge/NPC yonune
		if (number(0, 9) == 0)
		{
			int iDist[4] = { 200, 400, 600, 800 };
			for (int iDistIdx = 2; iDistIdx >= 0; --iDistIdx)
			{
				for (int iTryCount = 0; iTryCount < 6; ++iTryCount)
				{
					botCharacter->SetRotation(number(0, 359));
					float fx, fy;
					float fDist = number(iDist[iDistIdx], iDist[iDistIdx + 1]);
					GetDeltaByDegree(botCharacter->GetRotation(), fDist, &fx, &fy);

					bool isBlock = false;
					for (int j = 1; j <= 100; ++j)
					{
						if (!SECTREE_MANAGER::instance().IsMovablePosition(botCharacter->GetMapIndex(), botCharacter->GetX() + (int)fx * j / 100, botCharacter->GetY() + (int)fy * j / 100))
						{
							isBlock = true;
							break;
						}
					}
					if (isBlock)
						continue;

					int iDestX = botCharacter->GetX() + (int)fx;
					int iDestY = botCharacter->GetY() + (int)fy;
					if (botCharacter->Goto(iDestX, iDestY))
						botCharacter->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
					break;
				}
			}
		}
	}

	// Bot tepkileri - dengeli kontrol sıklığı
	return PASSES_PER_SEC(victim ? (float)0.15 : (int)5); // Dengeli tepki hızı (victim varken 0.15 saniye)
}

void BotCharacter::SetStartEvent(LPCHARACTER botCharacter)
{
	if (!botCharacter)
		return;

	auto info = AllocEventInfo<bot_character_event_info>();
	info->botCharacter = botCharacter;
	// Bot event'ini başlat (dengeli tepki için)
	m_botCharacterEvent = event_create(bot_character_event, info, PASSES_PER_SEC(0.15f)); // Dengeli başlangıç süresi
}

EVENTFUNC(bot_character_chat_event)
{
	auto info = dynamic_cast<bot_character_event_info*>(event->info);
	if (!info)
		return 0;

	LPCHARACTER botCharacter = info->botCharacter;
	if (!botCharacter)
		return 0;

	auto& botManager = CBotCharacterManager::instance();

	LPDESC d = DESC_MANAGER::instance().FindByCharacterName(botCharacter->GetName());
	LPCHARACTER tch = d ? d->GetCharacter() : nullptr;
	if (tch)
	{
		botManager.BotCharacterRemove(tch->GetName());
		sys_err("Real player entered bot %s has been deleted.", tch->GetName());
		return 0;
	}

	// Chat mesajlarından rastgele birini seç
	const std::vector<std::string>& chatMessages = botManager.GetChatMessages();
	const char* message = "Merhaba!";  // Varsayılan
	
	if (!chatMessages.empty())
	{
		int randomIndex = number(0, chatMessages.size() - 1);
		message = chatMessages[randomIndex].c_str();
	}

	// Türkçe karakterleri temizle
	char cleanMessage[256];
	strlcpy(cleanMessage, message, sizeof(cleanMessage));
	
	// Türkçe karakter düzeltmeleri
	for (size_t i = 0; i < strlen(cleanMessage); i++)
	{
		if (i + 1 < strlen(cleanMessage))
		{
			unsigned char byte1 = (unsigned char)cleanMessage[i];
			unsigned char byte2 = (unsigned char)cleanMessage[i+1];
			
			// Çoklu byte UTF-8 karakterlerini kontrol et
			if (byte1 == 0xC3)
			{
				switch (byte2)
				{
					case 0xA7: cleanMessage[i] = 'c'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ç
					case 0x9F: cleanMessage[i] = 'g'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ğ
					case 0xB1: cleanMessage[i] = 'i'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ı
					case 0xB6: cleanMessage[i] = 'o'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ö
					case 0xBC: cleanMessage[i] = 'u'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ü
				}
			}
			else if (byte1 == 0xC4 && byte2 == 0x84)
			{
				cleanMessage[i] = 'A'; // Ä -> A
				memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
			}
			else if (byte1 == 0xC5)
			{
				if (byte2 == 0xA1)
				{
					cleanMessage[i] = 's'; // š -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
				else if (byte2 == 0x9F)
				{
					cleanMessage[i] = 's'; // ş -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
			}
		}
	}

	// Bağırma (Shout) ile mesaj gönder (input_main.cpp formatı)
	char shoutMessage[CHAT_MAX_LEN + 1];
	
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	// Dil bayrağı (input_main.cpp gibi)
	// Bot karakterlerin dil bilgisini CBotCharacterManager'dan al
	const char* botNameForLang = botCharacter->GetName();
	BYTE botLanguage = CBotCharacterManager::instance().GetBotLanguage(botNameForLang);
	
	// Debug: Eğer dil bulunamazsa log
	if (botLanguage == 0)
	{
		sys_log(0, "Bot %s: GetBotLanguage() 0 döndü, fallback kullanılıyor", botNameForLang);
		botLanguage = botCharacter->GetLanguage(); // Fallback: GetLanguage() kullan
	}
	
	sys_log(0, "Bot %s: Mesaj gönderiliyor - Dil: %d (%s)", botNameForLang, botLanguage, get_locale(botLanguage));
	
	char szLocaleName[64];
	snprintf(szLocaleName, sizeof(szLocaleName), "|Eflag/%s|e ", get_locale(botLanguage));
	
	// Empire bayrağı
	std::string strEmpireFlagToken;
	switch (botCharacter->GetEmpire())
	{
		case 1: strEmpireFlagToken = "|Eempire/shinsoo|e "; break;
		case 2: strEmpireFlagToken = "|Eempire/chunjo|e "; break;
		case 3: strEmpireFlagToken = "|Eempire/jinno|e "; break;
	}
	
	// Mesaj formatı: |Eflag/XX|e |Eempire/YY|e |Hpm:name|hname|h|r : message
	snprintf(shoutMessage, sizeof(shoutMessage), "%s%s|Hpm:%s|h%s|h|r : %s", 
	         szLocaleName,
	         strEmpireFlagToken.c_str(),
	         botCharacter->GetName(),
	         botCharacter->GetName(),
	         cleanMessage);
	
	sys_log(0, "Bot %s: Çok dilli chat - Dil: %d (%s), İmparatorluk: %d, Mesaj: %s", 
	        botCharacter->GetName(), 
	        botLanguage,
	        get_locale(botLanguage),
	        botCharacter->GetEmpire(), 
	        cleanMessage);
#else
	// Empire bayrağı
	std::string strEmpireFlagToken;
	switch (botCharacter->GetEmpire())
	{
		case 1: strEmpireFlagToken = "|Eempire/shinsoo|e "; break;
		case 2: strEmpireFlagToken = "|Eempire/chunjo|e "; break;
		case 3: strEmpireFlagToken = "|Eempire/jinno|e "; break;
	}
	
	snprintf(shoutMessage, sizeof(shoutMessage), "%s|Hpm:%s|h%s|h|r : %s", 
	         strEmpireFlagToken.c_str(),
	         botCharacter->GetName(),
	         botCharacter->GetName(),
	         cleanMessage);
#endif
	
	// Bağırma paketi gönder
	TPacketGGShout p;
	p.bHeader = HEADER_GG_SHOUT;
	p.bEmpire = botCharacter->GetEmpire();
	strlcpy(p.szText, shoutMessage, sizeof(p.szText));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));
	SendShout(shoutMessage, botCharacter->GetEmpire()
#ifdef ENABLE_MESSENGER_BLOCK
		, botCharacter->GetName()
#endif
	);
	
	sys_log(0, "Bot %s bağırdı: %s", botCharacter->GetName(), cleanMessage);

	// 1-5 dakika arası rastgele interval (60-300 saniye)
	int randomInterval = number(60, 300);
	return PASSES_PER_SEC(randomInterval);
}

void BotCharacter::SetStartChatEvent(LPCHARACTER botCharacter)
{
	if (!botCharacter)
		return;

	auto info = AllocEventInfo<bot_character_event_info>();
	info->botCharacter = botCharacter;
	m_botCharacterChatEvent = event_create(bot_character_chat_event, info, number(1, 10));
}

void CBotCharacterManager::NoticePacket(LPCHARACTER ch, const char* szNotice)
{
	if (!ch)
		return;

	if (!szNotice || szNotice[0] == '\0')
	{
		sys_err("Invalid notice string.");
		return;
	}

	// Türkçe karakterleri temizle
	char cleanNotice[1024];
	strlcpy(cleanNotice, szNotice, sizeof(cleanNotice));
	
	// Türkçe karakter düzeltmeleri
	for (size_t i = 0; i < strlen(cleanNotice); i++)
	{
		if (i + 1 < strlen(cleanNotice))
		{
			unsigned char byte1 = (unsigned char)cleanNotice[i];
			unsigned char byte2 = (unsigned char)cleanNotice[i+1];
			
			// Çoklu byte UTF-8 karakterlerini kontrol et
			if (byte1 == 0xC3)
			{
				switch (byte2)
				{
					case 0xA7: cleanNotice[i] = 'c'; memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1); break; // ç
					case 0x9F: cleanNotice[i] = 'g'; memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1); break; // ğ
					case 0xB1: cleanNotice[i] = 'i'; memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1); break; // ı
					case 0xB6: cleanNotice[i] = 'o'; memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1); break; // ö
					case 0xBC: cleanNotice[i] = 'u'; memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1); break; // ü
				}
			}
			else if (byte1 == 0xC4 && byte2 == 0x84)
			{
				cleanNotice[i] = 'A'; // Ä -> A
				memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1);
			}
			else if (byte1 == 0xC5)
			{
				if (byte2 == 0xA1)
				{
					cleanNotice[i] = 's'; // š -> s
					memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1);
				}
				else if (byte2 == 0x9F)
				{
					cleanNotice[i] = 's'; // ş -> s
					memmove(&cleanNotice[i+1], &cleanNotice[i+2], strlen(cleanNotice) - i - 1);
				}
			}
		}
	}

	char noticeMessage[CHAT_MAX_LEN + 1];
	strlcpy(noticeMessage, cleanNotice, sizeof(noticeMessage));

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	// Dil bayrağı (input_main.cpp gibi)
	// Bot karakterlerin dil bilgisini CBotCharacterManager'dan al
	const char* botNameForLang = ch->GetName();
	BYTE botLanguage = CBotCharacterManager::instance().GetBotLanguage(botNameForLang);
	
	// Debug: Eğer dil bulunamazsa log
	if (botLanguage == 0)
	{
		sys_log(0, "Bot Notice %s: GetBotLanguage() 0 döndü, fallback kullanılıyor", botNameForLang);
		botLanguage = ch->GetLanguage(); // Fallback: GetLanguage() kullan
	}
	
	char szLocaleName[64];
	snprintf(szLocaleName, sizeof(szLocaleName), "|Eflag/%s|e ", get_locale(botLanguage));
	
	// Empire bayrağı
	std::string strEmpireFlagToken;
	switch (ch->GetEmpire())
	{
		case 1: strEmpireFlagToken = "|Eempire/shinsoo|e "; break;
		case 2: strEmpireFlagToken = "|Eempire/chunjo|e "; break;
		case 3: strEmpireFlagToken = "|Eempire/jinno|e "; break;
	}
	
	// Mesaj formatı: |Eflag/XX|e |Eempire/YY|e |Hpm:name|hname|h|r : message
	snprintf(noticeMessage, sizeof(noticeMessage), "%s%s|Hpm:%s|h%s|h|r : %s", 
	         szLocaleName,
	         strEmpireFlagToken.c_str(),
	         ch->GetName(),
	         ch->GetName(),
	         cleanNotice);
	
	sys_log(0, "Bot Notice: Çok dilli - Dil: %d (%s), İmparatorluk: %d, Mesaj: %s", 
	        botLanguage,
	        get_locale(botLanguage),
	        ch->GetEmpire(), 
	        cleanNotice);
#else
	// Empire bayrağı
	std::string strEmpireFlagToken;
	switch (ch->GetEmpire())
	{
		case 1: strEmpireFlagToken = "|Eempire/shinsoo|e "; break;
		case 2: strEmpireFlagToken = "|Eempire/chunjo|e "; break;
		case 3: strEmpireFlagToken = "|Eempire/jinno|e "; break;
	}
	
	snprintf(noticeMessage, sizeof(noticeMessage), "%s|Hpm:%s|h%s|h|r : %s", 
	         strEmpireFlagToken.c_str(),
	         ch->GetName(),
	         ch->GetName(),
	         cleanNotice);
#endif

	TPacketGGShout p;
	p.bHeader = HEADER_GG_SHOUT;
	p.bEmpire = ch->GetEmpire();
	strlcpy(p.szText, noticeMessage, sizeof(p.szText));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));
	SendShout(noticeMessage, ch->GetEmpire()
#ifdef ENABLE_MESSENGER_BLOCK
		, ch->GetName()
#endif
	);
}

void CBotCharacterManager::InitializeBotNames()
{
	const std::string filename = LocaleService_GetBasePath() + "/bot_player/bot_name.txt";
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		sys_err("bot_name.txt file could not be opened.");
		// Default bot isimleri ekle
		m_botNames = {"Bot1", "Bot2", "Bot3", "Bot4", "Bot5", "Bot6", "Bot7", "Bot8", "Bot9", "Bot10"};
		return;
	}

	std::string line;
	int lineNumber = 0;
	int skippedCount = 0;
	
	while (std::getline(ifs, line))
	{
		lineNumber++;
		
		// Boş satırları ve yorumları atla
		if (line.empty() || line[0] == '#' || line[0] == '/')
			continue;

		// Satır başındaki ve sonundaki boşlukları temizle
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);

		if (!line.empty())
		{
			// Bot isim uzunluğu kontrolü - CHARACTER_NAME_MAX_LEN (24 karakter) sınırı
			if (line.length() > CHARACTER_NAME_MAX_LEN)
			{
				std::string truncatedName = line.substr(0, CHARACTER_NAME_MAX_LEN);
				sys_log(0, "bot_name.txt satır %d: İsim çok uzun, kısaltıldı: '%s' -> '%s' (Max: %d karakter)",
				        lineNumber, line.c_str(), truncatedName.c_str(), CHARACTER_NAME_MAX_LEN);
				line = truncatedName;
				skippedCount++;
			}
			
			m_botNames.push_back(line);
		}
	}

	if (m_botNames.empty())
	{
		sys_err("No bot names found in bot_name.txt, using defaults.");
		m_botNames = {"Bot1", "Bot2", "Bot3", "Bot4", "Bot5", "Bot6", "Bot7", "Bot8", "Bot9", "Bot10"};
	}

	sys_log(0, "Loaded %zu bot names from bot_name.txt (%d names truncated to %d chars)", 
	        m_botNames.size(), skippedCount, CHARACTER_NAME_MAX_LEN);
}

void CBotCharacterManager::InitializeChatMessages()
{
	const std::string filename = LocaleService_GetBasePath() + "/bot_player/bot_player_chat.txt";
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		sys_err("bot_player_chat.txt file could not be opened.");
		// Default mesajlar ekle
		m_chatMessages = {"Merhaba!", "Nasılsınız?", "İyi günler!", "Bol şans!", "Görüşürüz!"};
		return;
	}

	std::string line;
	while (std::getline(ifs, line))
	{
		// Boş satırları ve yorumları atla
		if (line.empty() || line[0] == '#' || line[0] == '/')
			continue;

		// Satır başındaki ve sonundaki boşlukları temizle
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);

		if (!line.empty())
		{
			m_chatMessages.push_back(line);
		}
	}

	if (m_chatMessages.empty())
	{
		sys_err("No chat messages found in bot_player_chat.txt, using defaults.");
		m_chatMessages = {"Merhaba!", "Nasılsınız?", "İyi günler!", "Bol şans!", "Görüşürüz!"};
	}

	sys_log(0, "Loaded %zu chat messages from bot_player_chat.txt", m_chatMessages.size());
}

void CBotCharacterManager::Initialize()
{
	if (g_bAuthServer)
		return;

	// Bot isimlerini yükle
	InitializeBotNames();

	// Chat mesajlarını yükle
	InitializeChatMessages();

	// JSON'dan item template'lerini yükle
	const std::string filename = LocaleService_GetBasePath() + "/bot_player/bot_player.json";
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		sys_err("bot_player.json file could not be opened. Using default items.");
		sys_log(0, "Bot system initialized with %zu bot names (no JSON)", m_botNames.size());
		return;
	}

	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document doc;
	doc.ParseStream(isw);

	if (!doc.IsArray())
	{
		sys_err("JSON is not formatted as an array.");
		sys_log(0, "Bot system initialized with %zu bot names (invalid JSON)", m_botNames.size());
		return;
	}

	// JSON'dan item template'lerini oku
	sys_log(0, "JSON'dan %zu adet template okunuyor...", doc.GetArray().Size());
	
	for (const auto& player : doc.GetArray())
	{
		if (!(player.HasMember("job") && player.HasMember("level") &&
			player["job"].IsInt() && player["level"].IsInt()))
		{
			sys_err("JSON template eksik alan içeriyor, atlanıyor...");
			continue;
		}

		std::unique_ptr<TBotCharacterInfo> playerInfo(new TBotCharacterInfo());
		playerInfo->name = ""; // İsim bot_name.txt'den gelecek
		playerInfo->job = static_cast<BYTE>(player["job"].GetInt());
		playerInfo->level = static_cast<BYTE>(player["level"].GetInt());
		playerInfo->alignment = player.HasMember("alignment") && player["alignment"].IsInt() ? player["alignment"].GetInt() : 0;
		playerInfo->mountVnum = player.HasMember("mount_vnum") && player["mount_vnum"].IsInt() ? player["mount_vnum"].GetInt() : 0;
		
		// Item bilgilerini oku
		playerInfo->itemWeapon = player.HasMember("item_weapon") && player["item_weapon"].IsInt() ? static_cast<DWORD>(player["item_weapon"].GetInt()) : 0;
		playerInfo->itemArmor = player.HasMember("item_armor") && player["item_armor"].IsInt() ? static_cast<DWORD>(player["item_armor"].GetInt()) : 0;
		playerInfo->itemHair = player.HasMember("item_hair") && player["item_hair"].IsInt() ? static_cast<DWORD>(player["item_hair"].GetInt()) : 0;
		playerInfo->itemNeck = player.HasMember("item_neck") && player["item_neck"].IsInt() ? static_cast<DWORD>(player["item_neck"].GetInt()) : 0;
		playerInfo->itemEar = player.HasMember("item_ear") && player["item_ear"].IsInt() ? static_cast<DWORD>(player["item_ear"].GetInt()) : 0;
		playerInfo->itemFoots = player.HasMember("item_foots") && player["item_foots"].IsInt() ? static_cast<DWORD>(player["item_foots"].GetInt()) : 0;
		playerInfo->itemWrist = player.HasMember("item_wrist") && player["item_wrist"].IsInt() ? static_cast<DWORD>(player["item_wrist"].GetInt()) : 0;
		playerInfo->itemShield = player.HasMember("item_shield") && player["item_shield"].IsInt() ? static_cast<DWORD>(player["item_shield"].GetInt()) : 0;
		playerInfo->itemHead = player.HasMember("item_head") && player["item_head"].IsInt() ? static_cast<DWORD>(player["item_head"].GetInt()) : 0;
		playerInfo->itemAura = player.HasMember("item_aura") && player["item_aura"].IsInt() ? static_cast<DWORD>(player["item_aura"].GetInt()) : 0;
		playerInfo->itemAcce = player.HasMember("item_acce") && player["item_acce"].IsInt() ? static_cast<DWORD>(player["item_acce"].GetInt()) : 0;

		// Template olarak sakla
		std::string templateKey = "template_" + std::to_string(m_mapBotCharacterInfo.size());
		sys_log(0, "Template '%s' ekleniyor: job=%d, weapon=%d, armor=%d, shield=%d", 
		        templateKey.c_str(), playerInfo->job, playerInfo->itemWeapon, 
		        playerInfo->itemArmor, playerInfo->itemShield);
		m_mapBotCharacterInfo.emplace(templateKey, std::move(playerInfo));
	}
	
	sys_log(0, "Bot system initialized with %zu bot names and %zu item templates", m_botNames.size(), m_mapBotCharacterInfo.size());
}

void CBotCharacterManager::Reload()
{
	sys_log(0, "Reloading bot system...");
	
	// Mevcut bot template'lerini temizle
	m_mapBotCharacterInfo.clear();
	m_botNames.clear();
	m_chatMessages.clear();
	m_botLanguages.clear();
	
	// Bot dosyalarını yeniden yükle
	Initialize();
	
	sys_log(0, "Bot system reloaded successfully!");
	sys_log(0, "Loaded: %zu names, %zu chat messages, %zu templates", 
	        m_botNames.size(), m_chatMessages.size(), m_mapBotCharacterInfo.size());
}

// Gerçek Aura itemleri (49001-49006)
const uint32_t auraItems[] = {
	49001, // Genel Aura Giysisi
	49002, // Sade Aura Giysisi
	49003, // Soylu Aura Giysisi
	49004, // Işıltılı Aura Giysisi
	49005, // Gösterişli Aura Giysisi
	49006  // Parlak Aura Giysisi
};
const int auraItemCount = sizeof(auraItems) / sizeof(auraItems[0]);

// Rastgele Kuşak (Acce) itemleri
const uint32_t acceItems[] = {
	85001, 85002, 85003, 85004, 85005, 85006, 85007, 85008,
	85011, 85012, 85013, 85014, 85015,
	86001, 86002, 86003, 86004, 86005, 86006, 86007, 86008,
	86011, 86012, 86013, 86014, 86015, 86016, 86017, 86018,
	86021, 86022, 86023, 86024, 86025, 86026
};
const int acceItemCount = sizeof(acceItems) / sizeof(acceItems[0]);

void CBotCharacterManager::BotSpawn(LPCHARACTER ch, int32_t spawn_count, int8_t empire_id)
{
	if (!ch)
		return;

	if (spawn_count >= 200)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "En fazla 200 bot oluşturabilirsiniz.");
		return;
	}
	
	// Empire ID kontrolü (0=Rastgele, 1=Shinsoo, 2=Chunjo, 3=Jinno)
	if (empire_id < 0 || empire_id > 3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz empire ID. 0=Rastgele (tüm krallıklar), 1=Shinsoo, 2=Chunjo, 3=Jinno");
		return;
	}
	
	// Empire ID belirleme mantığı
	bool useRandomEmpire = (empire_id == 0); // 0 = Her bot için rastgele empire
	int8_t baseEmpireId = empire_id; // Sabit empire için (1, 2, 3)

	if (m_botNames.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bot isimleri yüklenemedi.");
		return;
	}

	// Maksimum bot sayısı kontrolü - bot_name.txt'deki isim sayısına göre
	if (m_botCharacters.size() >= m_botNames.size())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Maksimum bot sayısına ulaşıldı (%zu bot). Daha fazla bot spawnlayamazsınız.", m_botNames.size());
		return;
	}

	// Bot isimlerini karıştır
	std::random_device rd;
	std::mt19937 g(rd());
	std::vector<std::string> shuffledNames = m_botNames;
	std::shuffle(shuffledNames.begin(), shuffledNames.end(), g);

	int spawnedCount = 0;
	auto nameIt = shuffledNames.begin();

	while (spawnedCount < spawn_count && nameIt != shuffledNames.end())
	{
		const std::string& botNameOriginal = *nameIt;
		++nameIt;

		// Bot isim uzunluğu kontrolü - CHARACTER_NAME_MAX_LEN (24 karakter) sınırı
		std::string botName = botNameOriginal;
		if (botName.length() > CHARACTER_NAME_MAX_LEN)
		{
			botName = botName.substr(0, CHARACTER_NAME_MAX_LEN);
			sys_log(0, "Bot ismi çok uzun, kısaltıldı: '%s' -> '%s' (Max: %d karakter)", 
			        botNameOriginal.c_str(), botName.c_str(), CHARACTER_NAME_MAX_LEN);
		}

		// Eğer bu isimde bot zaten varsa atla
		if (m_botCharacters.find(botName) != m_botCharacters.end())
			continue;
			
		// Gerçek oyuncu ismi kontrolü - varsa bot çıkış yapsın
		LPDESC existingDesc = DESC_MANAGER::instance().FindByCharacterName(botName.c_str());
		if (existingDesc && existingDesc->GetCharacter())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Bot ismi '%s' gerçek oyuncu tarafından kullanılıyor. Bot çıkış yapıyor.", botName.c_str());
			continue;
		}

		auto botCharacter = CHARACTER_MANAGER::instance().CreateCharacter("");
		if (!botCharacter)
			continue;

		std::unique_ptr<BotCharacter> pBotPlayer(new BotCharacter());
		pBotPlayer->SetBotCharacter(botCharacter);

		botCharacter->SetBotCharacter(true);
		botCharacter->SetName(botName);  // Bot ismini bot_name.txt'den al (CHARACTER_NAME_MAX_LEN sınırlı)
		botCharacter->SetCharType(CHAR_TYPE_PC);
		
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		// Rastgele dil/bayrak atama - Bot için çok dilli destek
		// LOCALE_EN, LOCALE_DE, LOCALE_TR, LOCALE_PT, LOCALE_ES, LOCALE_FR, LOCALE_RO, LOCALE_PL, LOCALE_IT, LOCALE_CZ, LOCALE_HU
		BYTE randomLanguages[] = {
			LOCALE_EN,  // İngilizce (UK)
			LOCALE_DE,  // Almanca
			LOCALE_TR,  // Türkçe
			LOCALE_PT,  // Portekizce
			LOCALE_ES,  // İspanyolca
			LOCALE_FR,  // Fransızca
			LOCALE_RO,  // Romence
			LOCALE_PL,  // Lehçe
			LOCALE_IT,  // İtalyanca
			LOCALE_CZ,  // Çekçe
			LOCALE_HU   // Macarca
		};
		
		BYTE selectedLanguage = randomLanguages[number(0, 10)]; // 11 dil arasından rastgele seç
		
		// Bot karakterlerin dil bilgisini CBotCharacterManager'da sakla
		// ÖNEMLİ: botName (std::string) kullan, botCharacter->GetName() henüz set edilmemiş olabilir
		m_botLanguages[botName] = selectedLanguage;
		
		// Debug: Dil bilgisinin kaydedildiğini doğrula
		sys_log(0, "Bot %s: Dil bilgisi kaydediliyor - Key: '%s', Dil: %d", 
		        botName.c_str(), botName.c_str(), selectedLanguage);
		
		// Bot karakterlerin DESC'ini kontrol et ve dil ayarla (eğer varsa)
		if (botCharacter->GetDesc())
		{
			botCharacter->GetDesc()->SetLanguage(selectedLanguage);
		}
		
		const char* languageName = get_locale(selectedLanguage);
		sys_log(0, "Bot %s: Çok dilli sistem - Dil: %s (Kod: %d) - Map'e kaydedildi", 
		        botName.c_str(), languageName, selectedLanguage);
		
		// Dil bilgisi kaydedildikten sonra, bot ismi set edildikten sonra UpdatePacket çağrılacak
		// Bu sayede GetLanguage() doğru dil bilgisini döndürecek
#else
		sys_log(0, "Bot %s: Çok dilli sistem kapalı", botName.c_str());
#endif
		
		// Otomatik rastgele job ve level oluşturma sistemi
		// 0=Warrior, 1=Assassin, 2=Sura, 3=Shaman
		uint8_t randomJob = number(JOB_WARRIOR, JOB_SHAMAN);
		
		// Yohara/Şampiyon sistemi devre dışı: tüm botlar normal
		uint8_t bot_level = 120; // Normal seviye sabit 120
		// Conqueror/Yohara sistemi yok
		
		int alignment = number(1000, 20000); // Rastgele alignment
		
		// Debug log - Job ismini göster
		const char* jobName = "Bilinmeyen";
		switch (randomJob)
		{
			case JOB_WARRIOR: jobName = "Savaşçı"; break;
			case JOB_ASSASSIN: jobName = "Ninja"; break;
			case JOB_SURA: jobName = "Sura"; break;
			case JOB_SHAMAN: jobName = "Şaman"; break;
		}
		
		// Otomatik skill grubu: 1. bot=Grup1, 2. bot=Grup2, 3. bot=Grup1, ...
		uint32_t sameJobCount = CBotCharacterManager::instance().GetBotCountByJob(randomJob);
		BYTE skillGroup = (sameJobCount % 2) + 1;
		
		sys_log(0, "Bot %s: Otomatik oluşturuldu - Job: %d (%s), SkillGroup: %d, Seviye: %d", 
			botName.c_str(), randomJob, jobName, skillGroup, bot_level);
		
		// Job'u cinsiyet bazlı race'e çevir (FakePlayerManager gibi)
		BYTE byRace = randomJob;
		BYTE bySex = (number(0, 1) == 1 ? SEX_MALE : SEX_FEMALE);
		
		switch (randomJob)
		{
			case JOB_WARRIOR:
				byRace = (bySex == SEX_MALE) ? MAIN_RACE_WARRIOR_M : MAIN_RACE_WARRIOR_W;
				break;
			case JOB_ASSASSIN:
				byRace = (bySex == SEX_MALE) ? MAIN_RACE_ASSASSIN_M : MAIN_RACE_ASSASSIN_W;
				break;
			case JOB_SURA:
				byRace = (bySex == SEX_MALE) ? MAIN_RACE_SURA_M : MAIN_RACE_SURA_W;
				break;
			case JOB_SHAMAN:
				byRace = (bySex == SEX_MALE) ? MAIN_RACE_SHAMAN_M : MAIN_RACE_SHAMAN_W;
				break;
		}
		
		botCharacter->SetRace(byRace);
		botCharacter->SetLevel(bot_level);
		
		sys_log(0, "Bot %s: Race ayarlandı - Job: %d, Race: %d, Sex: %s", 
			botName.c_str(), randomJob, byRace, bySex == SEX_MALE ? "Erkek" : "Kadın");
		
		// Empire ataması - Rastgele modda 1-2-3 rotasyonu (bot vs bot icin karisik imparatorluk garantisi)
		int8_t botEmpire;
		if (useRandomEmpire)
		{
			botEmpire = (spawnedCount % 3) + 1;  // 1, 2, 3, 1, 2, 3...
		}
		else
		{
			botEmpire = baseEmpireId;
		}
		botCharacter->SetEmpire(botEmpire);
		
		sys_log(0, "Bot %s: Empire atandı - %s (Mode: %s)", 
			botName.c_str(), 
			empireArray[botEmpire - 1].c_str(),
			useRandomEmpire ? "Rastgele" : "Sabit");
		botCharacter->UpdateAlignment(alignment * 10);
		botCharacter->SetPoint(POINT_MOV_SPEED, 200);   // Hareket hizi 250
		botCharacter->SetPoint(POINT_ATT_SPEED, 180);   // Saldiri hizi 250
		botCharacter->SetPoint(POINT_CASTING_SPEED, 150); // Buyu hizi cok yuksek

		// Base stats
		botCharacter->SetRealPoint(POINT_ST, 200);
		botCharacter->SetRealPoint(POINT_HT, 200);
		botCharacter->SetRealPoint(POINT_DX, 200);
		botCharacter->SetRealPoint(POINT_IQ, 200);

		botCharacter->SetPoint(POINT_ST, 200);
		botCharacter->SetPoint(POINT_HT, 200);
		botCharacter->SetPoint(POINT_DX, 200);
		botCharacter->SetPoint(POINT_IQ, 200);

		// Bot oyuncuların HP'si sabit 30000, SP rastgele
		int botHP = 30000; // Bot oyuncularn sabit HP deeri
		int randomSP = number(35000, 40000);
		
		botCharacter->SetPoint(POINT_BOW_DISTANCE, 200);
		botCharacter->SetMaxHP(botHP);
		botCharacter->SetMaxSP(randomSP);
		botCharacter->SetHP(botHP);
		botCharacter->SetSP(randomSP);
		botCharacter->PointChange(POINT_HP, botHP);
		botCharacter->PointChange(POINT_SP, randomSP);
		botCharacter->PointChange(POINT_STAMINA, 1525);
		
		// Genel savunma değeri - Bot oyuncuların her karaktere karşı %100 savunması
		botCharacter->SetPoint(POINT_DEF_GRADE, 800);       // Çok yüksek temel savunma (neredeyse tüm hasarı engeller)
		botCharacter->SetPoint(POINT_DEF_BONUS, 0);          // Savunma bonusu
		
		// Hasar azaltma değerleri - %100 koruma için
		botCharacter->SetPoint(POINT_RESIST_NORMAL_DAMAGE, 100);    // %100 normal hasar azaltma
		botCharacter->SetPoint(POINT_NORMAL_HIT_DEFEND_BONUS, 99); // %99 normal vuruş azaltma (maksimum)
		botCharacter->SetPoint(POINT_SKILL_DEFEND_BONUS, 99);      // %99 skill hasarı azaltma (maksimum)
		
		// Tüm savunma değerleri 100 (Kılıç, Çift El, Bıçak, Yelpaze, Çan, Yay, Pençe)
		botCharacter->SetPoint(POINT_RESIST_SWORD, 100);      // Kılıç savunması
		botCharacter->SetPoint(POINT_RESIST_TWOHAND, 100);    // Çift El savunması
		botCharacter->SetPoint(POINT_RESIST_DAGGER, 100);     // Bıçak savunması
		botCharacter->SetPoint(POINT_RESIST_BELL, 100);       // Çan savunması
		botCharacter->SetPoint(POINT_RESIST_FAN, 100);        // Yelpaze savunması
		botCharacter->SetPoint(POINT_RESIST_BOW, 100);        // Yay savunması
		
		// Elemental savunmalar 100
		botCharacter->SetPoint(POINT_RESIST_FIRE, 100);       // Ateş savunması
		botCharacter->SetPoint(POINT_RESIST_ELEC, 100);       // Elektrik savunması
		botCharacter->SetPoint(POINT_RESIST_MAGIC, 100);      // Büyü savunması
		botCharacter->SetPoint(POINT_RESIST_WIND, 100);       // Rüzgar savunması
		botCharacter->SetPoint(POINT_RESIST_ICE, 100);        // Buz savunması
		botCharacter->SetPoint(POINT_RESIST_EARTH, 100);      // Toprak savunması
		
		// Karakter sınıflarına karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_WARRIOR, 25);    // Savaşçıya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_ASSASSIN, 25);   // Ninja'ya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_SURA, 25);       // Sura'ya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_SHAMAN, 25);    // Şaman'a karşı %100 savunma
#ifdef ENABLE_WOLFMAN_CHARACTER
		botCharacter->SetPoint(POINT_RESIST_WOLFMAN, 25);    // Kurt Adam'a karşı %100 savunma
#endif
		botCharacter->SetPoint(POINT_RESIST_DARK, 100);       // Karanlık savunması
		
		// Özel savunmalar 100
		botCharacter->SetPoint(POINT_RESIST_CRITICAL, 100);   // Kritik savunması
		botCharacter->SetPoint(POINT_RESIST_PENETRATE, 100);  // Delici vuruş savunması
		
		// Yohara/Şampiyon sistemi kullanlmyor
		
		botCharacter->ComputePoints();
		botCharacter->SetPoint(POINT_MOV_SPEED, 200);
		botCharacter->SetPoint(POINT_ATT_SPEED, 180);
		// ComputePoints() sonrasında HP ve savunma değerlerini tekrar ayarla (bot oyuncular için sabit değerler)
		const int botFixedHP = 30000;
		botCharacter->SetMaxHP(botFixedHP);
		// HP'yi MaxHP'ye eşitle (MaxHP değiştiyse HP'yi de güncelle)
		if (botCharacter->GetHP() != botFixedHP)
		{
			botCharacter->SetHP(botFixedHP);
			botCharacter->PointChange(POINT_HP, botFixedHP - botCharacter->GetHP());
		}
		
		botCharacter->SetPoint(POINT_DEF_GRADE, 500);       // Çok yüksek temel savunma
		botCharacter->SetPoint(POINT_DEF_BONUS, 0);
		
		// Hasar azaltma değerleri - %100 koruma için
		botCharacter->SetPoint(POINT_RESIST_NORMAL_DAMAGE, 1);
		botCharacter->SetPoint(POINT_NORMAL_HIT_DEFEND_BONUS, 99);
		botCharacter->SetPoint(POINT_SKILL_DEFEND_BONUS, 99);
		
		// Karakter sınıflarına karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_WARRIOR, 25);    // Savaşçıya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_ASSASSIN, 25);   // Ninja'ya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_SURA, 25);       // Sura'ya karşı %100 savunma
		botCharacter->SetPoint(POINT_RESIST_SHAMAN, 25);     // Şaman'a karşı %100 savunma
#ifdef ENABLE_WOLFMAN_CHARACTER
		botCharacter->SetPoint(POINT_RESIST_WOLFMAN, 25);    // Kurt Adam'a karşı %100 savunma
#endif

		// Bot'u spawn eden oyuncunun yanına spawn et
		long showX = ch->GetX() + number(-500, 500);
		long showY = ch->GetY() + number(-500, 500);
		int32_t playerMapIndex = ch->GetMapIndex();

		ch->ChatPacket(CHAT_TYPE_INFO, "Bot oluşturuluyor: X:%ld Y:%ld (Harita: %d)", showX, showY, playerMapIndex);

		if (!botCharacter->Show(playerMapIndex, showX, showY, 0))
		{
			M2_DESTROY_CHARACTER(botCharacter);
			sys_err("Cannot show bot character.");
			continue;
		}

		botCharacter->Stop();
		botCharacter->ReviveInvisible(5);

		// Otomatik seviyeye ve sınıfa göre item sistemi
		{
			// Sınıfa göre silah seçimi
			uint32_t weaponVnum = 0;
			uint32_t armorVnum = 0;
			uint32_t hairVnum = 0;
			
			// Cinsiyet belirleme (yukarıda zaten belirlendi)
			bool isMale = (bySex == SEX_MALE);
			
			// Seviyeye göre item tier belirleme
			int itemTier = 0; // 0=düşük, 1=orta, 2=yüksek
			if (bot_level >= 75)
				itemTier = 2;
			else if (bot_level >= 40)
				itemTier = 1;
			
			switch (randomJob)
			{
			case JOB_WARRIOR: // Savaşçı
				{
					if (itemTier == 2)
					{
						// Yüksek seviye - Kılıç veya Çiftel
						uint32_t highWeapons[] = {189, 279, 319, 335, 375, 469, 3159, 3169, 3199, 3229, 3245, 3265, 3509};
						weaponVnum = highWeapons[number(0, 12)];
					}
					else if (itemTier == 1)
					{
						// Orta seviye
						weaponVnum = number(0, 1) == 0 ? 19 : 3009;
		}
		else
		{
						// Düşük seviye
						weaponVnum = number(0, 1) == 0 ? 9 : 3000;
					}
					
					// Savaşçı zırhları
					uint32_t warriorArmors[] = {11299, 12019, 12059, 19299, 20009, 20769, 20819, 20869, 20919, 21225, 21325};
					armorVnum = warriorArmors[number(0, 10)];
					
					// Savaşçı saçları - Erkek/Kadın ayrımı
					if (isMale)
					{
						// Erkek saçları: Temel (901-906) + Özel saçlar
						uint32_t maleHairs[] = {901, 902, 903, 904, 905, 906, 45053, 45055, 45057, 45061, 45108, 45139, 45147, 45151};
						hairVnum = maleHairs[number(0, 13)];
					}
					else
					{
						// Kadın saçları: Temel (901-906) + Özel saçlar
						uint32_t femaleHairs[] = {901, 902, 903, 904, 905, 906, 45054, 45056, 45058, 45062, 45110, 45148, 45152};
						hairVnum = femaleHairs[number(0, 12)];
					}
				}
				break;
				
			case JOB_ASSASSIN: // Ninja - Skill grubuna gore silah (Grup1=Hancer, Grup2=Yay)
				{
					bool useBow = (skillGroup == 2);
					if (itemTier == 2)
					{
						// Yüksek seviye - Skill grubuna gore Bıçak veya Yay
						if (useBow)
						{
							uint32_t highBows[] = {2169, 2179, 2209, 2225, 2245, 2379, 2509};
							weaponVnum = highBows[number(0, 6)];
						}
						else
						{
							uint32_t highDaggers[] = {355, 1139, 1189, 1205, 1225, 1349, 1509, 4049};
							weaponVnum = highDaggers[number(0, 7)];
						}
					}
					else if (itemTier == 1)
					{
						weaponVnum = useBow ? 2009 : 1009;
					}
					else
					{
						weaponVnum = useBow ? 2000 : 1000;
					}
					
					// Ninja zırhları
					uint32_t ninjaArmors[] = {11499, 12029, 12069, 19499, 20259, 20779, 20829, 20879, 20929, 21245, 21345};
					armorVnum = ninjaArmors[number(0, 10)];
					
					// Ninja saçları - Erkek/Kadın ayrımı
					if (isMale)
					{
						// Erkek saçları: Temel (2001-2006) + Özel saçlar
						uint32_t maleHairs[] = {2001, 2002, 2003, 2004, 2005, 2006, 45053, 45055, 45057, 45061, 45108, 45135, 45139, 45147, 45151};
						hairVnum = maleHairs[number(0, 14)];
					}
					else
					{
						// Kadın saçları: Temel (2001-2006) + Özel saçlar
						uint32_t femaleHairs[] = {2001, 2002, 2003, 2004, 2005, 2006, 45054, 45056, 45058, 45062, 45110, 45136, 45148, 45152};
						hairVnum = femaleHairs[number(0, 13)];
					}
				}
				break;
				
			case JOB_SURA: // Sura
				{
					if (itemTier == 2)
					{
						// Yüksek seviye sura silahları
						uint32_t highSwords[] = {189, 199, 279, 289, 309, 335, 375, 395, 469, 479};
						weaponVnum = highSwords[number(0, 9)];
					}
					else if (itemTier == 1)
					{
						weaponVnum = 49;
					}
					else
					{
						weaponVnum = 50;
					}
					
					// Sura zırhları
					uint32_t suraArmors[] = {11699, 12039, 12079, 19699, 20509, 20789, 20839, 20889, 20939, 21265, 21365};
					armorVnum = suraArmors[number(0, 10)];
					
					// Sura saçları - Erkek/Kadın ayrımı
					if (isMale)
					{
						// Erkek saçları: Temel (3001-3006) + Özel saçlar
						uint32_t maleHairs[] = {3001, 3002, 3003, 3004, 3005, 3006, 45053, 45055, 45057, 45061, 45108, 45139, 45147, 45151, 45155};
						hairVnum = maleHairs[number(0, 14)];
					}
					else
					{
						// Kadın saçları: Temel (3001-3006) + Özel saçlar
						uint32_t femaleHairs[] = {3001, 3002, 3003, 3004, 3005, 3006, 45054, 45056, 45058, 45062, 45110, 45136, 45148, 45152};
						hairVnum = femaleHairs[number(0, 13)];
					}
				}
				break;
				
			case JOB_SHAMAN: // Şaman
				{
					if (itemTier == 2)
					{
						// Yüksek seviye - Çan veya Yelpaze
						bool useFan = (number(0, 1) == 1);
						if (useFan)
						{
							uint32_t highFans[] = {7199, 7309, 7325, 7345, 7379, 7509};
							weaponVnum = highFans[number(0, 5)];
						}
						else
						{
							uint32_t highBells[] = {5129, 5169, 5185, 5215, 5339, 5349, 5509};
							weaponVnum = highBells[number(0, 6)];
						}
					}
					else if (itemTier == 1)
					{
						weaponVnum = number(0, 1) == 0 ? 5009 : 7009;
					}
					else
					{
						weaponVnum = number(0, 1) == 0 ? 5000 : 7000;
					}
					
					// Şaman zırhları
					uint32_t shamanArmors[] = {11899, 12049, 12089, 19899, 20759, 20799, 20849, 20899, 20949, 21285, 21385};
					armorVnum = shamanArmors[number(0, 10)];
					
					// Şaman saçları - Erkek/Kadın ayrımı
					if (isMale)
					{
						// Erkek saçları: Temel (4001-4006) + Özel saçlar
						uint32_t maleHairs[] = {4001, 4002, 4003, 4004, 4005, 4006, 45053, 45055, 45057, 45061, 45079, 45108, 45139, 45147, 45151};
						hairVnum = maleHairs[number(0, 14)];
					}
					else
					{
						// Kadın saçları: Temel (4001-4006) + Özel saçlar
						uint32_t femaleHairs[] = {4001, 4002, 4003, 4004, 4005, 4006, 45054, 45056, 45058, 45062, 45080, 45110, 45148, 45152};
						hairVnum = femaleHairs[number(0, 13)];
					}
				}
				break;
			}
			
			// Item'ları gerçek eşya olarak giydir (sadece parça set etmek yerine)
			if (weaponVnum > 0)
			{
				if (LPITEM w = ITEM_MANAGER::instance().CreateItem(weaponVnum, 1))
				{
					// Uygun giyme hücresini bul ve giydir
					if (!w->EquipTo(botCharacter, w->FindEquipCell(botCharacter)))
					{
						M2_DESTROY_ITEM(w);
					}
					else
					{
						// Silah VNUM'unu hafif cache'e yaz (garanti amaçlı)
						CBotCharacterManager::instance().SetBotWeaponVnum(botCharacter->GetName(), weaponVnum);
						// Yay kullanan Assassin botlara Gumus Ok ekle (skill 46-50 icin zorunlu, sinirsiz)
						if (randomJob == JOB_ASSASSIN && w->GetSubType() == WEAPON_BOW)
						{
							const uint32_t arrowVnum = 8005; // Gumus Ok (en iyi ok)
							if (LPITEM arrow = ITEM_MANAGER::instance().CreateItem(arrowVnum, 99999))
							{
								int apos = botCharacter->GetEmptyInventory(arrow->GetSize());
								if (apos >= 0 && arrow->AddToCharacter(botCharacter, TItemPos(INVENTORY, apos)))
								{
									if (botCharacter->EquipItem(arrow))
										sys_log(0, "Bot %s: Yay icin Gumus Ok eklendi (VNUM: %u, sinirsiz)", botCharacter->GetName(), arrowVnum);
								}
								else
									M2_DESTROY_ITEM(arrow);
							}
						}
					}
				}
			}
			if (armorVnum > 0)
			{
				if (LPITEM a = ITEM_MANAGER::instance().CreateItem(armorVnum, 1))
				{
					if (!a->EquipTo(botCharacter, a->FindEquipCell(botCharacter)))
						M2_DESTROY_ITEM(a);
				}
			}
			// Saç sistemi devre dışı; istenirse aşağıdaki blok açılabilir
			// if (hairVnum > 0) { ... EquipTo ... }
			
			sys_log(0, "Bot %s: Otomatik itemler eklendi - Cinsiyet: %s, Weapon: %d, Armor: %d, Hair: %d", 
				botCharacter->GetName(), isMale ? "Erkek" : "Kadın", weaponVnum, armorVnum, hairVnum);
		}
		
	// Aura & Kuşak sistemleri: Conqueror/Yohara olmadan da botlara varsayılan parça ekle
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	{
		// Rastgele bir aura seç ve uygula
		uint32_t auraVnum = auraItems[number(0, auraItemCount - 1)];
		botCharacter->SetPart(PART_AURA, auraVnum);
	}
#endif
		
	// Geli?mi? Ku?ak (Acce) sistemi - Şampiyon botlara ku?ak ver
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	{
		// Rastgele bir kuşak seç ve gerçek eşya olarak giydir
		uint32_t acceVnum = acceItems[number(0, acceItemCount - 1)];
		if (LPITEM acce = ITEM_MANAGER::instance().CreateItem(acceVnum, 1))
		{
			// Parlama için emiş oranı eşiğini garanti et
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			long curAbs = acce->GetSocket(ACCE_ABSORPTION_SOCKET);
			if (curAbs < ACCE_EFFECT_FROM_ABS)
				acce->SetSocket(ACCE_ABSORPTION_SOCKET, ACCE_EFFECT_FROM_ABS);
#endif
			if (!acce->EquipTo(botCharacter, acce->FindEquipCell(botCharacter)))
			{
				M2_DESTROY_ITEM(acce);
			}
		}
	}
#endif
		
		// Bot karakterlerin ku?ak envanteri sistemi - Potion ve e?ya kullan?m?
		{
			// Bot seviyesine göre potion ve eşya ekleme
			int botLevel = botCharacter->GetLevel();
			
			// Gerçek HP Potion kodlar? - Bot seviyesine göre
			uint32_t hpPotionVnum = 0;
			if (botLevel >= 80)
				hpPotionVnum = 27001; // Büyük HP Potion
			else if (botLevel >= 50)
				hpPotionVnum = 27000; // Orta HP Potion
			else
				hpPotionVnum = 27001; // Küçük HP Potion
			
			// Gerçek SP Potion kodlar? - Bot seviyesine göre
			uint32_t spPotionVnum = 0;
			if (botLevel >= 80)
				spPotionVnum = 27011; // Büyük SP Potion
			else if (botLevel >= 50)
				spPotionVnum = 27010; // Orta SP Potion
			else
				spPotionVnum = 27011; // Küçük SP Potion
			
			// Ku?ak envanterine potion ekleme
			for (int i = 0; i < 3; i++) // 3 adet HP potion
			{
				LPITEM hpPotion = ITEM_MANAGER::instance().CreateItem(hpPotionVnum, 1);
				if (hpPotion)
				{
					hpPotion->AddToCharacter(botCharacter, TItemPos(BELT_INVENTORY, i));
					hpPotion->SetCount(99); // Maksimum stack
				}
			}
			
			for (int i = 3; i < 6; i++) // 3 adet SP potion
			{
				LPITEM spPotion = ITEM_MANAGER::instance().CreateItem(spPotionVnum, 1);
				if (spPotion)
				{
					spPotion->AddToCharacter(botCharacter, TItemPos(BELT_INVENTORY, i));
					spPotion->SetCount(99); // Maksimum stack
				}
			}
			
			// Ek eşyalar - Bot seviyesine göre
			if (botLevel >= 60)
			{
				// Yüksek seviye botlar için ek e?yalar
				uint32_t extraItems[] = {27020, 27021, 27022}; // Büyü, Antidot, vs.
				for (int i = 6; i < 9 && i < BELT_INVENTORY_SLOT_COUNT; i++)
				{
					uint32_t extraItemVnum = extraItems[number(0, 2)];
					LPITEM extraItem = ITEM_MANAGER::instance().CreateItem(extraItemVnum, 1);
					if (extraItem)
					{
						extraItem->AddToCharacter(botCharacter, TItemPos(BELT_INVENTORY, i));
						extraItem->SetCount(10); // S?n?rl? miktar
					}
				}
			}
			
			sys_log(0, "Bot %s: Seviye %d için Gerçek Kuşak envanteri dolduruldu (HP: %d, SP: %d)", 
				botCharacter->GetName(), botLevel, hpPotionVnum, spPotionVnum);
		}
		
		// Skill ve item attribute atamalari (bot skill kullanimi icin zorunlu)
		// Bot karakterler DB'den yuklenmedigi icin m_pSkillLevels NULL kalir - once allocate et
		botCharacter->EnsureSkillLevels();
		botCharacter->SetSkillGroup(skillGroup);
		botCharacter->DisableCooltime();
		EquipItemAttributes(botCharacter);
		
		// Görünümü güncelle
		sys_log(0, "Bot %s: UpdatePacket() çağrılıyor...", botCharacter->GetName());
		botCharacter->UpdatePacket();
		sys_log(0, "Bot %s: UpdatePacket() tamamlandı", botCharacter->GetName());

		pBotPlayer->SetStartEvent(botCharacter);

		if (botCharacter->GetLevel() >= 15)
			pBotPlayer->SetStartChatEvent(botCharacter);

		m_botCharacters.emplace(botName, std::move(pBotPlayer));

		// Bot spawn duyurusu - Şimdilik devre dışı (gereksiz spam)
		/*
		char notice[256];
		snprintf(notice, sizeof(notice), "%s az önce %s karakteri ile %s imparatorluğuna katıldı!",
			botCharacter->GetName(),
			jobArray[randomJob].c_str(),
			empireArray[botCharacter->GetEmpire() - 1].c_str()
		);
		BroadcastNotice(notice);
		*/

		botCharacter->SetHorseLevel(21);

		// Botlar bineksiz spawn olur - Performans için
		// Saldırı anında binek kullanacaklar
		sys_log(0, "Bot %s: Bineksiz spawn edildi (performans optimizasyonu)", 
		        botCharacter->GetName());

		++spawnedCount;
	}
	
	// GM'ye özet bilgi
	if (useRandomEmpire)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Toplam %d bot oluşturuldu (Rastgele Krallık Modu - Her bot farklı krallıktan)", spawnedCount);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Toplam %d bot oluşturuldu (%s Krallığı)", 
		               spawnedCount, empireArray[baseEmpireId - 1].c_str());
	}
}

// Empire bazlı bot spawn fonksiyonları
void CBotCharacterManager::BotSpawnShinsoo(LPCHARACTER ch, int32_t spawn_count)
{
	BotSpawn(ch, spawn_count, 1); // Shinsoo (Kırmızı)
}

void CBotCharacterManager::BotSpawnChunjo(LPCHARACTER ch, int32_t spawn_count)
{
	BotSpawn(ch, spawn_count, 2); // Chunjo (Sarı)
}

void CBotCharacterManager::BotSpawnJinno(LPCHARACTER ch, int32_t spawn_count)
{
	BotSpawn(ch, spawn_count, 3); // Jinno (Mavi)
}

LPCHARACTER CBotCharacterManager::FindNearestEnemyBot(LPCHARACTER botChr, int maxDistance) const
{
	if (!botChr || !botChr->IsBotCharacter())
		return nullptr;
	const int8_t myEmpire = botChr->GetEmpire();
	const int32_t myMap = botChr->GetMapIndex();
	const long myX = botChr->GetX();
	const long myY = botChr->GetY();
	LPCHARACTER nearest = nullptr;
	int minDist = maxDistance + 1;
	for (const auto& [_, pBot] : m_botCharacters)
	{
		if (!pBot)
			continue;
		LPCHARACTER other = pBot->GetBotCharacter();
		if (!other || other == botChr || other->IsDead())
			continue;
		if (other->GetMapIndex() != myMap)
			continue;
		if (other->GetEmpire() == myEmpire)
			continue;
		int d = DISTANCE_APPROX(myX - other->GetX(), myY - other->GetY());
		if (d <= maxDistance && d < minDist)
		{
			minDist = d;
			nearest = other;
		}
	}
	return nearest;
}

void CBotCharacterManager::BotFullRemove()
{
	for (auto& [_, botCharacter] : m_botCharacters)
	{
		if (botCharacter && botCharacter->GetBotCharacter())
			M2_DESTROY_CHARACTER(botCharacter->GetBotCharacter());
	}
	m_botCharacters.clear();
}

void CBotCharacterManager::BotCharacterRemove(const char* c_szName)
{
	if (auto it = m_botCharacters.find(c_szName); it != m_botCharacters.end())
	{
		if (it->second && it->second->GetBotCharacter())
			M2_DESTROY_CHARACTER(it->second->GetBotCharacter());

		m_botCharacters.erase(it);
	}
	
	// Bot dil bilgisini de temizle
	m_botLanguages.erase(c_szName);
}

BYTE CBotCharacterManager::GetBotLanguage(const char* c_szName) const
{
	if (!c_szName || c_szName[0] == '\0')
	{
		return 0;
	}
	
	std::string nameStr(c_szName);
	auto it = m_botLanguages.find(nameStr);
	if (it != m_botLanguages.end())
	{
		return it->second;
	}
	
	// Debug: Sadece hata durumunda log (ilk 5 kez)
	static int errorCount = 0;
	if (errorCount < 5)
	{
		sys_log(0, "GetBotLanguage: Bot '%s' için dil bulunamadı. Map'te %zu bot var:", c_szName, m_botLanguages.size());
		for (const auto& pair : m_botLanguages)
		{
			sys_log(0, "  - Map key: '%s', Dil: %d", pair.first.c_str(), pair.second);
		}
		errorCount++;
	}
	
	return 0; // Default language
}

bool CBotCharacterManager::IsBotCharacter(const char* c_szName) const
{
	return m_botCharacters.find(c_szName) != m_botCharacters.end();
}

uint32_t CBotCharacterManager::GetBotCountByJob(BYTE job) const
{
	uint32_t count = 0;
	for (const auto& kv : m_botCharacters)
	{
		LPCHARACTER pkChr = kv.second ? kv.second->GetBotCharacter() : nullptr;
		if (pkChr && pkChr->GetJob() == job)
			++count;
	}
	return count;
}

void CBotCharacterManager::ForwardPMToGMs(LPCHARACTER sender, const char* botName, const char* message)
{
	if (!sender || !botName || !message)
		return;
	
	// Türkçe karakterleri düzelt (gelen mesajda)
	char cleanMessage[CHAT_MAX_LEN + 1];
	strlcpy(cleanMessage, message, sizeof(cleanMessage));
	
	// Türkçe karakter düzeltmeleri
	for (size_t i = 0; i < strlen(cleanMessage); i++)
	{
		if (i + 1 < strlen(cleanMessage))
		{
			unsigned char byte1 = (unsigned char)cleanMessage[i];
			unsigned char byte2 = (unsigned char)cleanMessage[i+1];
			
			// Çoklu byte UTF-8 karakterlerini kontrol et
			if (byte1 == 0xC3)
			{
				switch (byte2)
				{
					case 0xA7: cleanMessage[i] = 'c'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ç
					case 0x9F: cleanMessage[i] = 'g'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ğ
					case 0xB1: cleanMessage[i] = 'i'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ı
					case 0xB6: cleanMessage[i] = 'o'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ö
					case 0xBC: cleanMessage[i] = 'u'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ü
				}
			}
			else if (byte1 == 0xC4 && byte2 == 0x84)
			{
				cleanMessage[i] = 'A'; // Ä -> A
				memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
			}
			else if (byte1 == 0xC5)
			{
				if (byte2 == 0xA1)
				{
					cleanMessage[i] = 's'; // š -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
				else if (byte2 == 0x9F)
				{
					cleanMessage[i] = 's'; // ş -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
			}
		}
	}
	
	// Tüm oyuncuları tara ve GM'leri bul
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	
	int gmCount = 0;
	for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPDESC d = *it;
		if (!d || !d->GetCharacter())
			continue;
		
		LPCHARACTER gm = d->GetCharacter();
		
		// Sadece GM'lere gönder
		if (!gm->GetGMLevel())
			continue;
		
		// GM için session oluştur (cevap verebilsin diye)
		SetGMBotReplySession(gm, sender->GetName(), botName);
		
		// GM'e özel formatlı mesaj + talimat (Türkçe karakterler düzeltildi)
		char gmNotification[CHAT_MAX_LEN + 256];
		snprintf(gmNotification, sizeof(gmNotification), 
		         "[BOT PM] %s -> %s: %s (Cevap icin: /w %s mesajiniz)", 
		         sender->GetName(), botName, cleanMessage, sender->GetName());
		
		// GM'e whisper olarak gönder
		TPacketGCWhisper pack;
		pack.bHeader = HEADER_GC_WHISPER;
		pack.bType = WHISPER_TYPE_SYSTEM;
		pack.wSize = sizeof(TPacketGCWhisper) + strlen(gmNotification) + 1;
		strlcpy(pack.szNameFrom, botName, sizeof(pack.szNameFrom)); // Bot adını göster
		
		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(pack));
		buf.write(gmNotification, strlen(gmNotification) + 1);
		d->Packet(buf.read_peek(), buf.size());
		
		gmCount++;
	}
	
	// Log kaydet
	sys_log(0, "BOT PM forwarded to %d GM(s): %s -> %s: %s", 
	        gmCount, sender->GetName(), botName, cleanMessage);
}

// GM session yönetimi
void CBotCharacterManager::SetGMBotReplySession(LPCHARACTER gm, const std::string& playerName, const std::string& botName)
{
	if (!gm)
		return;
	
	BotPMSession session;
	session.playerName = playerName;
	session.botName = botName;
	session.timestamp = time(0);
	
	m_gmBotSessions[gm->GetName()] = session;
	
	sys_log(0, "GM %s bot reply session set: Player=%s Bot=%s", 
	        gm->GetName(), playerName.c_str(), botName.c_str());
}

bool CBotCharacterManager::GetGMBotReplySession(LPCHARACTER gm, std::string& outPlayerName, std::string& outBotName)
{
	if (!gm)
		return false;
	
	auto it = m_gmBotSessions.find(gm->GetName());
	if (it == m_gmBotSessions.end())
		return false;
	
	// Session 5 dakikadan eskiyse sil
	if (time(0) - it->second.timestamp > 300)
	{
		m_gmBotSessions.erase(it);
		return false;
	}
	
	outPlayerName = it->second.playerName;
	outBotName = it->second.botName;
	return true;
}

void CBotCharacterManager::ClearGMBotReplySession(LPCHARACTER gm)
{
	if (!gm)
		return;
	
	m_gmBotSessions.erase(gm->GetName());
}

void CBotCharacterManager::TalkingMessage(LPCHARACTER ch, const char* c_szMessage)
{
	if (!ch)
		return;

	// Türkçe karakterleri temizle
	char cleanMessage[CHAT_MAX_LEN + 1];
	strlcpy(cleanMessage, c_szMessage, sizeof(cleanMessage));
	
	// Türkçe karakter düzeltmeleri
	for (size_t i = 0; i < strlen(cleanMessage); i++)
	{
		if (i + 1 < strlen(cleanMessage))
		{
			unsigned char byte1 = (unsigned char)cleanMessage[i];
			unsigned char byte2 = (unsigned char)cleanMessage[i+1];
			
			// Çoklu byte UTF-8 karakterlerini kontrol et
			if (byte1 == 0xC3)
			{
				switch (byte2)
				{
					case 0xA7: cleanMessage[i] = 'c'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ç
					case 0x9F: cleanMessage[i] = 'g'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ğ
					case 0xB1: cleanMessage[i] = 'i'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ı
					case 0xB6: cleanMessage[i] = 'o'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ö
					case 0xBC: cleanMessage[i] = 'u'; memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1); break; // ü
				}
			}
			else if (byte1 == 0xC4 && byte2 == 0x84)
			{
				cleanMessage[i] = 'A'; // Ä -> A
				memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
			}
			else if (byte1 == 0xC5)
			{
				if (byte2 == 0xA1)
				{
					cleanMessage[i] = 's'; // š -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
				else if (byte2 == 0x9F)
				{
					cleanMessage[i] = 's'; // ş -> s
					memmove(&cleanMessage[i+1], &cleanMessage[i+2], strlen(cleanMessage) - i - 1);
				}
			}
		}
	}

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), cleanMessage);
	std::string text = chatbuf;
	if (text.empty())
		return;

	struct packet_chat pack_chat;
	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type = CHAT_TYPE_TALKING;
	pack_chat.id = ch->GetVID();
	pack_chat.bEmpire = 0;
	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);
	ch->PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CBotCharacterManager::GetBotCharacter(const char* c_szName) const
{
	if (const auto it = m_botCharacters.find(c_szName); it != m_botCharacters.end())
		return it->second->GetBotCharacter();
	return nullptr;
}


void CBotCharacterManager::EquipItem(LPCHARACTER ch)
{
	if (!ch)
		return;

	auto it = m_mapBotCharacterInfo.find(ch->GetName());
	if (it == m_mapBotCharacterInfo.end())
		return;

	const TBotCharacterInfo& playerInfo = *it->second;

	auto EquipItem = [ch](DWORD vnum)
	{
		if (vnum > 0)
		{
			if (LPITEM item = ITEM_MANAGER::instance().CreateItem(vnum);
				item && !item->EquipTo(ch, item->FindEquipCell(ch)))
			{
				M2_DESTROY_ITEM(item);
			}
		}
	};

	EquipItem(playerInfo.itemWeapon);
	EquipItem(playerInfo.itemArmor);
	EquipItem(playerInfo.itemHair);
	EquipItem(playerInfo.itemNeck);
	EquipItem(playerInfo.itemEar);
	EquipItem(playerInfo.itemFoots);
	EquipItem(playerInfo.itemWrist);
	EquipItem(playerInfo.itemShield);
	EquipItem(playerInfo.itemHead);
}

void CBotCharacterManager::EquipItemAttributes(LPCHARACTER ch)
{
	if (!ch)
		return;

	BYTE job = ch->GetJob();
	LPITEM item = nullptr;

	switch (job)
	{
	case JOB_WARRIOR:
	case JOB_ASSASSIN:
	case JOB_SURA:
	case JOB_SHAMAN:
	{
		//  -- Kask Efsun
		item = ch->GetWear(WEAR_HEAD);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_ATT_SPEED, 8); // Saldiri hizi
			item->SetForceAttribute(1, APPLY_HP_REGEN, 30); // Hp uretimi
			item->SetForceAttribute(2, APPLY_SP_REGEN, 30); // Sp uretimi
			item->SetForceAttribute(3, APPLY_DODGE, 15);	// Orklardan korunma sansi
			item->SetForceAttribute(4, APPLY_STEAL_SP, 10); //%Hasar sp tarafindan emilecek
		}

		// -- Silah Efsun
		item = ch->GetWear(WEAR_WEAPON);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_CAST_SPEED, 20);	  // Buyu hizi
			item->SetForceAttribute(1, APPLY_CRITICAL_PCT, 10);	  // Kritik vurus sansi
			item->SetForceAttribute(2, APPLY_PENETRATE_PCT, 10);  // Delici vurus sansi
			item->SetForceAttribute(3, APPLY_ATTBONUS_DEVIL, 20); // Seytanlara karsi guc
			item->SetForceAttribute(4, APPLY_STR, 12);			  // Guc
		}

		// -- Kalkan Efsun
		item = ch->GetWear(WEAR_SHIELD);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_IMMUNE_STUN, 1);	 // Sersemlik karsisinda bagisiklilik
			item->SetForceAttribute(0, APPLY_CON, 12);			 // Canlilik
			item->SetForceAttribute(1, APPLY_BLOCK, 15);		 // Yakin dovus saldirilarini bloklama sansi
			item->SetForceAttribute(2, APPLY_REFLECT_MELEE, 10); // Yakin dovus vuruslarini yansitma
			item->SetForceAttribute(3, APPLY_IMMUNE_STUN, 1);	 // Sersemlik karsisinda bagisiklilik
			item->SetForceAttribute(4, APPLY_IMMUNE_SLOW, 1);	 // Yavaslatma karsisinda bagisiklilik
		}

		//  -- Zirh Efsun
		item = ch->GetWear(WEAR_BODY);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_MAX_HP, 2000);		   // Max.HP
			item->SetForceAttribute(1, APPLY_ATT_GRADE_BONUS, 30); // Saldiri degeri+30
			item->SetForceAttribute(1, APPLY_CAST_SPEED, 20);	   // Buyu hizi
			item->SetForceAttribute(2, APPLY_STEAL_HP, 10);		   //%Hasar hp tarafindan emilecek
			item->SetForceAttribute(3, APPLY_REFLECT_MELEE, 10);   // Yakin dovus vuruslarini yansitma
			item->SetForceAttribute(4, APPLY_ATT_GRADE_BONUS, 50); // Saldiri degeri+50
		}

		//  -- Ayakkabi Efsun
		item = ch->GetWear(WEAR_FOOTS);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_MAX_HP, 2000);		// Max.HP
			item->SetForceAttribute(1, APPLY_MAX_SP, 2000);		// Max.SP
			item->SetForceAttribute(2, APPLY_MOV_SPEED, 20);		// Hareket hizi
			item->SetForceAttribute(3, APPLY_ATT_SPEED, 8);		// Saldiri hizi
			item->SetForceAttribute(4, APPLY_CRITICAL_PCT, 10); // Kritik vurus sansi
		}

		//  -- Bilezik Efsun
		item = ch->GetWear(WEAR_WRIST);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_MAX_HP, 2000);		 // Max.HP
			item->SetForceAttribute(1, APPLY_PENETRATE_PCT, 10); // Delici vurus sansi
			item->SetForceAttribute(1, APPLY_MAX_SP, 2000);		 // Max.SP
			item->SetForceAttribute(2, APPLY_PENETRATE_PCT, 10); // Delici vurus sansi
			item->SetForceAttribute(3, APPLY_STEAL_HP, 10);		 //%Hasar hp tarafindan emilecek
			item->SetForceAttribute(4, APPLY_MANA_BURN_PCT, 10); // Sp calma sansi
		}

		//  -- Kolye Efsun
		item = ch->GetWear(WEAR_NECK);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_MAX_HP, 2000);		 // Max.HP
			item->SetForceAttribute(1, APPLY_MAX_SP, 2000);		 // Max.SP
			item->SetForceAttribute(2, APPLY_CRITICAL_PCT, 10);	 // Kritik vurus sansi
			item->SetForceAttribute(3, APPLY_PENETRATE_PCT, 10); // Delici vurus sansi
			item->SetForceAttribute(4, APPLY_STEAL_SP, 10);		 //%Hasar sp tarafindan emilecek
		}

		//  -- Kupe Efsun
		item = ch->GetWear(WEAR_EAR);
		if (item)
		{
			item->ClearAttribute();
			item->SetForceAttribute(0, APPLY_MOV_SPEED, 20);	   // Hareket hizi
			item->SetForceAttribute(1, APPLY_MANA_BURN_PCT, 10);   // Sp calma sansi
			item->SetForceAttribute(2, APPLY_POISON_REDUCE, 5);	   // Zehre karsi koyma
			item->SetForceAttribute(3, APPLY_ATTBONUS_DEVIL, 20);  // Seytanlara karsi guc
			item->SetForceAttribute(4, APPLY_ATTBONUS_UNDEAD, 20); // Olumsuzlere karsi guc
		}
	}
	break;
	}

	// Temel skill'ler
	ch->SetSkillLevel(SKILL_HORSE_SUMMON, 10);			  // At Cagirma
	ch->SetSkillLevel(SKILL_LEADERSHIP, SKILL_MAX_LEVEL); // Liderlik
	ch->SetSkillLevel(SKILL_COMBO, 2);					  // Kombo
	ch->SetSkillLevel(SKILL_CREATE, SKILL_MAX_LEVEL);     // Balikcilik
	ch->SetSkillLevel(SKILL_MINING, SKILL_MAX_LEVEL);	  // Madencilik
	ch->SetSkillLevel(SKILL_POLYMORPH, SKILL_MAX_LEVEL);  // Donusum
	ch->SetSkillLevel(SKILL_HORSE, 11);                   // Binicilik

	ch->SetSkillLevel(SKILL_LANGUAGE1, 20);
	ch->SetSkillLevel(SKILL_LANGUAGE2, 20);
	ch->SetSkillLevel(SKILL_LANGUAGE3, 20);
	
	// Sinif bazli skill grubuna gore 6 skill ver - Perfect Master seviyesi (SKILL_MAX_LEVEL)
	BYTE skillGroup = ch->GetSkillGroup();
	if (skillGroup == 0) skillGroup = 1;
	auto skills = GetClassSkills(job, skillGroup);
	for (DWORD skillVnum : skills)
	{
		ch->SetSkillLevel(skillVnum, SKILL_MAX_LEVEL);
	}
	
	sys_log(0, "Bot %s: Job=%d, SkillGroup=%d, %zu skill atandi", 
	        ch->GetName(), job, skillGroup, skills.size());

	// DitoSystem mantığı: SkillLevelPacket çağır (skill'lerin client'a gönderilmesi için)
	ch->SkillLevelPacket();
	
	ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
	ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

	ch->ComputePoints();
	ch->SetPoint(POINT_MOV_SPEED, 200);
	ch->SetPoint(POINT_ATT_SPEED, 180);
	// ComputePoints() sonrasında HP ve savunma değerlerini tekrar ayarla (bot oyuncular için sabit değerler)
	const int botFixedHP = 30000;
	ch->SetMaxHP(botFixedHP);
	// HP'yi MaxHP'ye eşitle (MaxHP değiştiyse HP'yi de güncelle)
	if (ch->GetHP() != botFixedHP)
	{
		ch->SetHP(botFixedHP);
		ch->PointChange(POINT_HP, botFixedHP - ch->GetHP());
	}
	
	// %100 savunma değerleri
	ch->SetPoint(POINT_DEF_GRADE, 800);       // Çok yüksek temel savunma
	ch->SetPoint(POINT_DEF_BONUS, 0);
	ch->SetPoint(POINT_RESIST_NORMAL_DAMAGE, 100);    // %100 normal hasar azaltma
	ch->SetPoint(POINT_NORMAL_HIT_DEFEND_BONUS, 99); // %99 normal vuruş azaltma (maksimum)
	ch->SetPoint(POINT_SKILL_DEFEND_BONUS, 99);      // %99 skill hasarı azaltma (maksimum)
	
	// Karakter sınıflarına karşı %100 savunma
	ch->SetPoint(POINT_RESIST_WARRIOR, 25);    // Savaşçıya karşı %100 savunma
	ch->SetPoint(POINT_RESIST_ASSASSIN, 25);   // Ninja'ya karşı %100 savunma
	ch->SetPoint(POINT_RESIST_SURA, 25);       // Sura'ya karşı %100 savunma
	ch->SetPoint(POINT_RESIST_SHAMAN, 25);     // Şaman'a karşı %100 savunma
#ifdef ENABLE_WOLFMAN_CHARACTER
	ch->SetPoint(POINT_RESIST_WOLFMAN, 25);    // Kurt Adam'a karşı %100 savunma
#endif
}
#endif
