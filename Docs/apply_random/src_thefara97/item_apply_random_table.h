#ifndef __INC_ITEM_APPLY_RANDOM_H__
#define __INC_ITEM_APPLY_RANDOM_H__

/**
* Title: Apply Random (Base Bonus)
* Author: Owsap
* Date: 2022.03.25
**/

#include "../../common/length.h"
#if defined(__ITEM_APPLY_RANDOM__)
enum EApplyRandom
{
	APPLY_RANDOM_PATH1 = 1,
	APPLY_RANDOM_PATH2,
	APPLY_RANDOM_MAX_PATHS,
	ITEM_REFINE_MAX_LEVEL = 15,
};

struct SApplyRandom
{
	SApplyRandom(EApplyTypes _ApplyType, std::string _ApplyValueGroupName) : ApplyType(_ApplyType), ApplyValueGroupName(_ApplyValueGroupName) {}
	EApplyTypes ApplyType;
	std::string ApplyValueGroupName;
};

class CGroupNode;
class CGroupTextParseTreeLoader;

class CApplyRandomTable
{
public:
	CApplyRandomTable();
	virtual ~CApplyRandomTable();

	bool ReadApplyRandomTableFile(const char* c_pszFileName);
	bool GetApplyRandom(BYTE bIndex, int iLevel, BYTE& bApplyType, short& sApplyValue, BYTE& bPath);
	short GetNextApplyRandomValue(BYTE bIndex, int iLevel, BYTE bPath, BYTE bApplyType);

public:
	typedef std::vector<SApplyRandom> ApplyRandomVector;
	typedef std::map<std::string, ApplyRandomVector> ApplyRandomGroupMap;

private:
	bool ReadApplyRandomMapper();
	bool ReadApplyRandomTypes();
	bool GetApplyRandomValue(std::string stApplyValueGroupName, int iLevel, short& sPath1Value, short& sPath2Value);

private:
	CGroupTextParseTreeLoader* m_pLoader;
	std::string stFileName;

	CGroupNode* m_pApplyRandomValueTableNode;

	std::vector<std::string> m_vecApplyRandomGroups;
	ApplyRandomGroupMap m_mapApplyRandomGroup;
};

#endif // __INC_ITEM_APPLY_RANDOM_H__
#endif // __ITEM_APPLY_RANDOM__
