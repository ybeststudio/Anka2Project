if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))

JOB_WARRIOR		= 0
JOB_ASSASSIN	= 1
JOB_SURA		= 2
JOB_SHAMAN		= 3

RACE_WARRIOR_M	= 0
RACE_ASSASSIN_W	= 1
RACE_SURA_M		= 2
RACE_SHAMAN_W	= 3
RACE_WARRIOR_W	= 4
RACE_ASSASSIN_M	= 5
RACE_SURA_W		= 6
RACE_SHAMAN_M	= 7

PASSIVE_GUILD_SKILL_INDEX_LIST = ( 151, )
ACTIVE_GUILD_SKILL_INDEX_LIST = ( 152, 153, 154, 155, 156, 157, )

NEW_678TH_SKILL_ENABLE = 0
SKILL_INDEX_DICT = []

def DefineSkillIndexDict():
	global SKILL_INDEX_DICT

	if app.ENABLE_CONQUEROR_LEVEL:
		SKILL_INDEX_DICT = {
			JOB_WARRIOR : { 
				1 : (1, 2, 3, 4, 5, 6, 0, 0, 176, 137, 0, 138, 0, 139, 0,), 
				2 : (16, 17, 18, 19, 20, 21, 0, 0, 176, 137, 0, 138, 0, 139, 0,), 
				"SUPPORT" : (0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 246)
			},
			JOB_ASSASSIN : { 
				1 : (31, 32, 33, 34, 35, 36, 0, 0, 177, 137, 0, 138, 0, 139, 0, 140,), 
				2 : (46, 47, 48, 49, 50, 51, 0, 0, 178, 137, 0, 138, 0, 139, 0, 140,), 
				"SUPPORT" : (0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 246)
			},
			JOB_SURA : { 
				1 : (61, 62, 63, 64, 65, 66, 0, 0, 179, 137, 0, 138, 0, 139, 0,),
				2 : (76, 77, 78, 79, 80, 81, 0, 0, 180, 137, 0, 138, 0, 139, 0,),
				"SUPPORT" : (0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 246)
			},
			JOB_SHAMAN : { 
				1 : (91, 92, 93, 94, 95, 96, 0, 0, 181, 137, 0, 138, 0, 139, 0,),
				2 : (106, 107, 108, 109, 110, 111, 0, 0, 182, 137, 0, 138, 0, 139, 0,),
				"SUPPORT" : (0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 246)
			},
		}
	else:
		SKILL_INDEX_DICT = {
			JOB_WARRIOR : { 
				1 : (1, 2, 3, 4, 5, 6, 0, 0, 137, 0, 138, 0, 139, 0,), 
				2 : (16, 17, 18, 19, 20, 21, 0, 0, 137, 0, 138, 0, 139, 0,), 
				"SUPPORT" : (122, 123, 121, 124, 125, 129, 0, 0, 130, 131,),
			},
			JOB_ASSASSIN : { 
				1 : (31, 32, 33, 34, 35, 36, 0, 0, 137, 0, 138, 0, 139, 0, 140,), 
				2 : (46, 47, 48, 49, 50, 51, 0, 0, 137, 0, 138, 0, 139, 0, 140,), 
				"SUPPORT" : (122, 123, 121, 124, 125, 129, 0, 0, 130, 131,),
			},
			JOB_SURA : { 
				1 : (61, 62, 63, 64, 65, 66, 0, 0, 137, 0, 138, 0, 139, 0,),
				2 : (76, 77, 78, 79, 80, 81, 0, 0, 137, 0, 138, 0, 139, 0,),
				"SUPPORT" : (122, 123, 121, 124, 125, 129, 0, 0, 130, 131,),
			},
			JOB_SHAMAN : { 
				1 : (91, 92, 93, 94, 95, 96, 0, 0, 137, 0, 138, 0, 139, 0,),
				2 : (106, 107, 108, 109, 110, 111, 0, 0, 137, 0, 138, 0, 139, 0,),
				"SUPPORT" : (122, 123, 121, 124, 125, 129, 0, 0, 130, 131,),
			},
		}

def __LoadRaceHeight():
	directory = "locale/common/race_height.txt"
	try:
		lines = pack_open(directory, "r").readlines()
	except IOError:
		return

	for line in lines:
		tokens = line[:-1].split("\t")
		if len(tokens) == 0 or not tokens[0]:
			continue

		chrmgr.SetRaceHeight(int(tokens[0]), float(tokens[1]))

loadGameDataDict = {
	"RACE_HEIGHT" : __LoadRaceHeight,
}

def LoadGameData(name):
	global loadGameDataDict

	load = loadGameDataDict.get(name, 0)
	if load:
		loadGameDataDict[name]=0
		try:
			load()
		except:
			print name
			import exception
			exception.Abort("LoadGameData")
			raise
