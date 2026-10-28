#pragma once

#include <memory>
#include <ctime>
#include <unordered_map>
#include <string>
#include <vector>

struct TBotCharacterInfo
{
	std::string name;
	uint8_t job;
	uint8_t level;
	int32_t alignment;
	int32_t mountVnum;

	uint32_t itemWeapon;
	uint32_t itemArmor;
	uint32_t itemHair;
	uint32_t itemNeck;
	uint32_t itemEar;
	uint32_t itemFoots;
	uint32_t itemWrist;
	uint32_t itemShield;
	uint32_t itemHead;
	uint32_t itemAura;
	uint32_t itemAcce;

	TBotCharacterInfo()
		: name(""), job(0), level(0), alignment(0), mountVnum(0),
		itemWeapon(0), itemArmor(0), itemHair(0),
		itemNeck(0), itemEar(0), itemFoots(0),
		itemWrist(0), itemShield(0), itemHead(0),
		itemAura(0), itemAcce(0)
	{}
};

class BotCharacter
{
public:
	BotCharacter() : m_botCharacter(nullptr), m_botCharacterEvent(nullptr) {}
	~BotCharacter() = default;

	void SetBotCharacter(LPCHARACTER botCharacter) { m_botCharacter = botCharacter; }
	LPCHARACTER GetBotCharacter() const { return m_botCharacter; }

	void SetStartEvent(LPCHARACTER botCharacter);
	void SetStartChatEvent(LPCHARACTER botCharacter);

public:
	LPCHARACTER m_botCharacter;
	LPEVENT m_botCharacterEvent;
	LPEVENT m_botCharacterChatEvent;
};

class CBotCharacterManager : public singleton<CBotCharacterManager>
{
public:
	CBotCharacterManager() = default;
	~CBotCharacterManager() = default;

	void Initialize();
	void InitializeBotNames();  // Bot isimlerini bot_name.txt'den yükle
	void InitializeChatMessages();  // Chat mesajlarýný bot_player_chat.txt'den yükle
	void Reload();  // Bot dosyalarýný yeniden yükle
	void BotSpawn(LPCHARACTER ch, int32_t spawn_count, int8_t empire_id = 0);
	
	// Empire bazlý bot spawn fonksiyonlarý
	void BotSpawnShinsoo(LPCHARACTER ch, int32_t spawn_count);  // Kýrmýzý Krallýk
	void BotSpawnChunjo(LPCHARACTER ch, int32_t spawn_count);   // Sarý Krallýk  
	void BotSpawnJinno(LPCHARACTER ch, int32_t spawn_count);    // Mavi Krallýk

	void EquipItem(LPCHARACTER ch);
	void EquipItemAttributes(LPCHARACTER ch);

	void BotFullRemove();
	void BotCharacterRemove(const char* c_szName);

	uint32_t BotCharacterCount() const { return static_cast<uint32_t>(m_botCharacters.size()); }
	uint32_t GetBotCountByJob(BYTE job) const;

	void TalkingMessage(LPCHARACTER ch, const char* c_szMessage);

	void NoticePacket(LPCHARACTER ch, const char* szNotice);
	
	// PM sistemi: Bot'a gelen mesajlarý GM'lere yönlendir
	void ForwardPMToGMs(LPCHARACTER sender, const char* botName, const char* message);
	
	// GM'in bot PM'ine cevap verme sistemi
	void SetGMBotReplySession(LPCHARACTER gm, const std::string& playerName, const std::string& botName);
	bool GetGMBotReplySession(LPCHARACTER gm, std::string& outPlayerName, std::string& outBotName);
	void ClearGMBotReplySession(LPCHARACTER gm);

	bool IsBotCharacter(const char* c_szName) const;
	LPCHARACTER GetBotCharacter(const char* c_szName) const;

	// Dusman imparatorluk botlarini bulmak icin (sectree fallback)
	LPCHARACTER FindNearestEnemyBot(LPCHARACTER botChr, int maxDistance) const;
	
	const std::vector<std::string>& GetChatMessages() const { return m_chatMessages; }

	// Bot karakterlerin dil bilgisi (bot ismi -> dil kodu)
	BYTE GetBotLanguage(const char* c_szName) const;
	
private:
	std::unordered_map<std::string, std::unique_ptr<TBotCharacterInfo>> m_mapBotCharacterInfo;
	std::unordered_map<std::string, std::unique_ptr<BotCharacter>> m_botCharacters;
	std::vector<std::string> m_botNames;  // Bot isimleri listesi
	std::vector<std::string> m_chatMessages;  // Chat mesajlarý listesi
	
	// Bot karakterlerin dil bilgisi (bot ismi -> dil kodu)
	std::unordered_map<std::string, BYTE> m_botLanguages;
	
	// GM'lerin bot PM session'larý (GM adý -> {oyuncu, bot})
	struct BotPMSession {
		std::string playerName;
		std::string botName;
		time_t timestamp;
	};
	std::unordered_map<std::string, BotPMSession> m_gmBotSessions;

public:
	// Hafif depolama: bot silah vnum cache (quest flag yerine)
	void SetBotWeaponVnum(const char* name, int vnum) { if (name && *name) m_botWeaponVnum[name] = vnum; }
	int GetBotWeaponVnum(const char* name) const {
		if (!name || !*name) return 0;
		auto it = m_botWeaponVnum.find(name);
		return it != m_botWeaponVnum.end() ? it->second : 0;
	}
	void EraseBotWeaponVnum(const char* name) { if (name && *name) m_botWeaponVnum.erase(name); }

private:
	std::unordered_map<std::string, int> m_botWeaponVnum;
};
