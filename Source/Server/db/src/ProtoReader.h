#pragma once

#include "CsvReader.h"

#include <iostream>
#include <map>

void putItemIntoTable();

int32_t get_Item_Type_Value(const std::string &inputString);
int32_t get_Item_SubType_Value(uint32_t type_value, const std::string &inputString);
int32_t get_Item_AntiFlag_Value(const std::string &inputString);
int32_t get_Item_Flag_Value(const std::string &inputString);
int32_t get_Item_WearFlag_Value(const std::string &inputString);
int32_t get_Item_Immune_Value(const std::string &inputString);
int32_t get_Item_LimitType_Value(const std::string &inputString);
int32_t get_Item_ApplyType_Value(const std::string &inputString);

int32_t get_Mob_Rank_Value(const std::string &inputString);
int32_t get_Mob_Type_Value(const std::string &inputString);
int32_t get_Mob_BattleType_Value(const std::string &inputString);

int32_t get_Mob_Size_Value(const std::string &inputString);
int32_t get_Mob_AIFlag_Value(const std::string &inputString);
int32_t get_Mob_RaceFlag_Value(const std::string &inputString);
int32_t get_Mob_ImmuneFlag_Value(const std::string &inputString);

bool Set_Proto_Mob_Table(TMobTable *mobTable, const cCsvTable &csvTable, std::map<int32_t, const char*> &nameMap);
bool Set_Proto_Item_Table(TItemTable *itemTable, const cCsvTable &csvTable,std::map<int32_t, const char*> &nameMap);
