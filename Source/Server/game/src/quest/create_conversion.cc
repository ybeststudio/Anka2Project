extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <iostream>

#include "crc32.h"

using namespace std;

lua_State* L;

bool ReadStateFile()
{
    DIR* pdir = opendir("state");

    if (!pdir)
    {
	cerr << "cannot find state directory" << endl;
	return false;
    }

    dirent * pde;

    while ((pde = readdir(pdir)))
    {
	if (pde->d_name[0] == '.')
	    continue;

	string filename = string("state/") + pde->d_name;

	lua_dofile(L, filename.c_str());
    }

    closedir(pdir);

    return true;
}

int main()
{
    L = lua_open();

    if (!L)
    {
	cerr << "fail to create Lua state" << endl;
	return -1;
    }

    if (!ReadStateFile())
	return -1;

    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX))
    {
	// stack: questname state_table
	string quest_name = lua_tostring(L, -2);
	//printf("quest name : %s\n", quest_name.c_str());

	for(lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
	{
	    if (lua_isstring(L, -2) && lua_isnumber(L, -1))
	    {
		// skip `start' state
		if (lua_tonumber(L, -1) == 0)
		    continue;

		string state_name = lua_tostring(L, -2);
		int old_index = (int) lua_tonumber(L, -1);
		int new_index = (int) get_crc32(state_name.c_str(), state_name.size());
		//printf("\tstate name : %s\n", state_name.c_str());
		//printf("\tindex : %d -> %d\n", old_index, new_index);
		/*
		cout << "UPDATE quest SET lValue = " << new_index << " "
		     << "WHERE szName = '" << quest_name << "' "
		     << "AND szState = '__status' "
		     << "AND lValue = " << old_index << ";" << endl;
		     */

		//cout << quest_name << '\t' << old_index << '\t' << new_index << endl;
		cout << "perl -pi -e 's/" << quest_name << '\t' << old_index << "$/" << quest_name << '\t' << new_index << "/g'" << endl;
	    }
	}

	lua_pop(L, 1);
    }

    lua_close(L);

    return 0;
}
