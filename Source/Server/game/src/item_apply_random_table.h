#pragma once

#ifdef ENABLE_APPLY_RANDOM
// Official "APPLY_RANDOM" base bonuses: item_proto ApplyType = APPLY_RANDOM, ApplyValue = group id.
// On creation one apply of the group and one value path are rolled, the value follows the refine level.
// Table format after Owsap's apply_random_table.txt (locale/<country>/apply_random_table.txt).
enum EApplyRandom
{
	APPLY_RANDOM_PATH1 = 1,
	APPLY_RANDOM_PATH2,
	APPLY_RANDOM_PATH3,
	APPLY_RANDOM_PATH4,
	APPLY_RANDOM_PATH5,
	APPLY_RANDOM_MAX_PATHS = APPLY_RANDOM_PATH5, // official values come in 2-5 versions per bonus
	APPLY_RANDOM_MAX_REFINE_LEVEL = 15,
};

class CGroupNode;
class CGroupTextParseTreeLoader;

class CApplyRandomTable
{
	public:
		CApplyRandomTable();
		~CApplyRandomTable();

		bool ReadApplyRandomTableFile(const char* c_pszFileName);
		bool GetApplyRandom(int iGroup, int iLevel, WORD& rwApplyType, short& rsApplyValue, BYTE& rbPath) const;
		short GetApplyRandomValue(int iGroup, int iLevel, BYTE bPath, WORD wApplyType) const;

	private:
		struct SApplyRandom
		{
			WORD wApplyType;
			std::string strValueGroup;
		};

		bool ReadApplyRandomMapper();
		bool ReadApplyRandomTypes();
		bool GetPathValue(const std::string& strValueGroup, int iLevel, BYTE bPath, short& rsValue) const;
		BYTE GetPathCount(const std::string& strValueGroup) const;

	private:
		CGroupTextParseTreeLoader* m_pLoader;
		CGroupNode* m_pValueNode;
		std::vector<std::string> m_vecGroupNames;
		std::map<std::string, std::vector<SApplyRandom> > m_mapGroup;
};
#endif
