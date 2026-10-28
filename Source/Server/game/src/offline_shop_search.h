#pragma once

class COfflineShopSearch
{
	public:
		static bool IsHerbs(DWORD p)
		{
			if(p == 0)
				return false;

			if(p >= 50701 && p <= 50729)
				return true;
			return false;
		}

		static bool IsOtherBook(DWORD p)
		{
			if(p == 0)
				return false;

			if(p >= 50301 && p <= 50511)
				return true;
			if(p >= 50530 && p <= 50535)
				return true;
			return false;
		}

		static bool IsBoostBook(DWORD p)
		{
			if(p == 0)
				return false;
			if(p >= 50560 && p <= 50568)
				return true;
			return false;
		}

		static bool IsWardBook(DWORD p)
		{
			if(p == 0)
				return false;

			if(p >= 50516 && p <= 50524)
				return true;
			return false;
		}
		static bool IsPetBook(DWORD p)
		{
			if(p == 0)
				return false;

			if(p >= 55003 && p <= 55005)
				return true;

			if(p >= 55010 && p <= 55027)
				return true;
			return false;
		}

		static bool IsUpgrade(DWORD p)
		{
			if(p == 0)
				return false;

			switch (p)
			{
				case 30003:
				case 30004:
				case 30005:
				case 30006:
				case 30007:
				case 30008:
				case 30009:
				case 30010:
				case 30011:
				case 30014:
				case 30015:
				case 30016:
				case 30017:
				case 30018:
				case 30019:
				case 30021:
				case 30022:
				case 30023:
				case 30025:
				case 30027:
				case 30028:
				case 30030:
				case 30031:
				case 30032:
				case 30033:
				case 30034:
				case 30035:
				case 30037:
				case 30038:
				case 30039:
				case 30040:
				case 30041:
				case 30042:
				case 30045:
				case 30046:
				case 30047:
				case 30048:
				case 30049:
				case 30050:
				case 30051:
				case 30052:
				case 30053:
				case 30055:
				case 30056:
				case 30057:
				case 30058:
				case 30059:
				case 30060:
				case 30061:
				case 30067:
				case 30069:
				case 30070:
				case 30071:
				case 30072:
				case 30073:
				case 30074:
				case 30075:
				case 30076:
				case 30077:
				case 30078:
				case 30079:
				case 30080:
				case 30081:
				case 30082:
				case 30083:
				case 30084:
				case 30085:
				case 30086:
				case 30087:
				case 30088:
				case 30089:
				case 30090:
				case 30091:
				case 30092:
				case 30192:
				case 30193:
				case 30194:
				case 30195:
				case 30196:
				case 30197:
				case 30198:
				case 30199:
				case 30500:
				case 30501:
				case 30502:
				case 30503:
				case 30504:
				case 30505:
				case 30506:
				case 30507:
				case 30508:
				case 30509:
				case 30510:
				case 30511:
				case 30512:
				case 30513:
				case 30514:
				case 30515:
				case 30516:
				case 30517:
				case 30518:
				case 30519:
				case 30520:
				case 30521:
				case 30522:
				case 30523:
				case 30524:
				case 30525:
				case 30600:
				case 30601:
				case 30602:
				case 30603:
				case 30604:
				case 30605:
				case 30606:
				case 30607:
				case 30608:
				case 30609:
				case 30610:
				case 30611:
				case 30612:
				case 30613:
				case 30614:
				case 30615:
				case 30616:
				case 30617:
				case 30618:
				case 30619:
				case 30620:
				case 30621:
				case 30622:
				case 30623:
				case 30624:
				case 30625:
				case 30626:
				case 30627:
				case 30628:
				case 30629:
				case 30421:
				case 30422:
				case 30423:
				case 30424:
				case 30425:
				case 27992:
				case 27993:
				case 27994:
				case 33001:
				case 33002:
				case 33003:
				case 33004:
				case 33005:
				case 33006:
				case 33007:
				case 33008:
				case 33009:
				case 33010:
				case 33011:
				case 33012:
				case 33013:
				case 33014:
				case 33015:
				case 33016:
				case 33017:
				case 33018:
				case 33019:
				case 33020:
				case 33021:
				case 33022:
				case 27799:
				case 71520:
				case 71521:
				case 71522:
				case 30165:
				case 30166:
				case 30167:
				case 30168:
				case 30251:
				case 30252:
				case 30550:
				case 70031:
				case 27991:
				case 30401:
				case 30402:
				case 30403:
				case 30404:
				case 30405:
				case 30406:
				case 30407:
				case 30408:
				case 30409:
				case 30410:
				case 30411:
				case 30412:
				case 30413:
				case 30414:
				case 30415:
					return true;
			}
			return false;
		}

		static bool IsBiologist(DWORD p)
		{
			if(p == 0)
				return false;

			if(p >= 30220 && p <= 30228)
				return true;

			if(p == 30251 || p == 30252 || p == 70022)
				return true;
			return false;
		}

		static bool IsAttr(DWORD p)
		{
			if(p == 0)
				return false;

			switch(p)
			{
				case 71084:
				case 71085:
				case 76015:
				case 76023:
				case 76024:
				case 71152:
				case 71151:
				//case 70063:
				//case 70064:
				//case 77025:
				case 76013:
				case 76014:
				//case 80039:
				case 72304:
				case 70024:
				case 39004:
				case 39028:
				case 39029:
				case 80067:
					return true;
			}
			return false;
		}

		static bool IsAttr2(DWORD p)
		{
			if(p == 0)
				return false;

			switch(p)
			{
				case 70063:
				case 70064:
				case 77025:
				case 80039:
					return true;
			}
			return false;
		}

		static bool IsOre(DWORD p)
		{
			if(p == 0)
				return false;

			if(p == 50621)
				return true;

			if(p >= 50623 && p <= 50640)
				return true;
			return false;
		}

		static bool IsPetName(DWORD p)
		{
			if(p == 0)
				return false;

			if(p == 55030)
				return true;
			return false;
		}

		static bool IsNewPet(DWORD p)
		{
			if(p == 0)
				return false;

			if(p == 55002)
				return true;
			return false;
		}
};
