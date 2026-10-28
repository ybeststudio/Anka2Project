#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "wedding.h"

#include "../../common/service.h"

#ifdef ENABLE_MESSENGER_BLOCK
	#include "messenger_manager.h"
#endif

#define NEED_TARGET	(1 << 0)
#define NEED_PC		(1 << 1)
#define WOMAN_ONLY	(1 << 2)
#define OTHER_SEX_ONLY	(1 << 3)
#define SELF_DISARM	(1 << 4)
#define TARGET_DISARM	(1 << 5)
#define BOTH_DISARM	(SELF_DISARM | TARGET_DISARM)

struct emotion_type_s
{
	const char *	command;
	const char *	command_to_client;
	long	flag;
	float	extra_delay;
} emotion_types[] = {
	{	"emo_1",		"french_kiss",			NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,			4.0f		},
	{	"emo_4",		"kiss",					NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,			1.66667f	},
	{	"emo_5",		"slap",					NEED_PC | SELF_DISARM,							2.0f		},
	{	"emo_6",		"clap",					0,												2.66667f	},
	{	"emo_7",		"cheer1",				0,												2.33333f	},
	{	"emo_8",		"cheer2",				0,												2.33333f	},
	{	"emo_9",		"dance1",				0,												28.3333f	},
	{	"emo_10",		"dance2",				0,												4.76667f	},
	{	"emo_11",		"dance3",				0,												28.3333f	},
	{	"emo_12",		"dance4",				0,												30.3333f	},
	{	"emo_13",		"dance5",				0,												21.1f		},
	{	"emo_14",		"dance6",				0,												30.3333f	},
	{	"emo_15",		"congratulation",		0,												6.33333f	},
	{	"emo_16",		"forgive",				0,												8.33333f	},
	{	"emo_17",		"angry",				0,												4.33333f	},
	{	"emo_18",		"attractive",			0,												4.83333f	},
	{	"emo_19",		"sad",					0,												7.33333f	},
	{	"emo_20",		"shy",					0,												4.66667f	},
	{	"emo_21",		"cheerup",				0,												5.0f		},
	{	"emo_22",		"banter",				0,												7.0f		},
	{	"emo_23",		"joy",					0,												5.33333f	},
	{	"emo_24",		"selfie",				0,												3.33333f	},
	{	"emo_25",		"dance7",				0,												10.3f		},
	{	"emo_26",		"doze",					0,												4.8f		},
	{	"emo_27",		"exercise",				0,												6.45666f	},
	{	"emo_28",		"pushup",				0,												7.33333f	},
	{	"emo_29",		"worship",				0,												10.0f		},
	{	"emo_30",		"backflip",				0,												10.0f		},
	{	"emo_31",		"breakdance",			0,												10.0f		},
	{	"emo_32",		"dance8",				0,												10.0f		},
	{	"emo_33",		"dab",					0,												10.0f		},
	{	"emo_34",		"facepalm",				0,												10.0f		},
	{	"emo_35",		"fuckoff",				0,												10.0f		},
	{	"emo_36",		"wank",					0,												10.0f		},
	{	"emo_37",		"shake_it",				0,												10.0f		},
	{	"emo_38",		"throw_money",			0,												10.0f		},
	{	"emo_39",		"dance9",				0,												10.0f		},
	{	"emo_40",		"twerk",				0,												10.0f		},
	{	"emo_44",		"samba",				0,												10.0f		},
	{	"emo_45",		"shimmy",				0,												10.0f		},
	{	"emo_46",		"slow2",				0,												10.0f		},
	{	"emo_47",		"bestmates",			0,												10.0f		},
	{	"emo_48",		"flair",				0,												10.0f		},
	{	"emo_49",		"horon",				0,												10.0f		},
	{	"emo_50",		"robotdance",			0,												10.0f		},
	{	"emo_51",		"snowball",				0,												10.0f		},
	{	"emo_52",		"surprised2",			0,												10.0f		},
	{	"\n",			"\n",					0,												0.0f		},
};


std::set<std::pair<DWORD, DWORD> > s_emotion_set;

ACMD(do_emotion_allow)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD val = 0; str_to_number(val, arg1);

#ifdef ENABLE_MESSENGER_BLOCK
	LPCHARACTER tch = CHARACTER_MANAGER::Instance().Find(val);

	if (!tch)
		return;

	if (MessengerManager::Instance().IsBlocked(ch->GetName(), tch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unblock %s to continue."), tch->GetName());
		return;
	}
	else if (MessengerManager::Instance().IsBlocked(tch->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s has blocked you."), tch->GetName());
		return;
	}
#endif

	s_emotion_set.insert(std::make_pair(ch->GetVID(), val));
}

bool CHARACTER_CanEmotion(CHARACTER& rch)
{
	return true;
}

ACMD(do_emotion)
{
	int i;
	{
		if (ch->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot express emotions whilst riding a horse."));
			return;
		}
	}

	for (i = 0; *emotion_types[i].command != '\n'; ++i)
	{
		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command))
			break;

		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command_to_client))
			break;
	}

	if (*emotion_types[i].command == '\n')
	{
		sys_err("cannot find emotion");
		return;
	}

	if (!CHARACTER_CanEmotion(*ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can do this when you wear an Emotion Mask."));
		return;
	}

	if (IS_SET(emotion_types[i].flag, WOMAN_ONLY) && SEX_MALE==GET_SEX(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Only women can do this."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	LPCHARACTER victim = NULL;

	if (*arg1)
		victim = ch->FindCharacterInView(arg1, IS_SET(emotion_types[i].flag, NEED_PC));

	if (IS_SET(emotion_types[i].flag, NEED_TARGET | NEED_PC))
	{
		if (!victim)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This person does not exist."));
			return;
		}
	}

	if (victim)
	{
		if (!victim->IsPC() || victim == ch)
			return;

		if (victim->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use emotions with a player who is riding on a Horse."));
			return;
		}

		long distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

		if (distance < 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too near."));
			return;
		}

		if (distance > 500)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too far away."));
			return;
		}

		if (IS_SET(emotion_types[i].flag, OTHER_SEX_ONLY))
		{
			if (GET_SEX(ch)==GET_SEX(victim))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This action can only be done with another gender."));
				return;
			}
		}

		if (IS_SET(emotion_types[i].flag, NEED_PC))
		{
			if (s_emotion_set.find(std::make_pair(victim->GetVID(), ch->GetVID())) == s_emotion_set.end())
			{
				if (true == marriage::CManager::instance().IsMarried( ch->GetPlayerID() ))
				{
					const marriage::TMarriage* marriageInfo = marriage::CManager::instance().Get( ch->GetPlayerID() );

					const DWORD other = marriageInfo->GetOther( ch->GetPlayerID() );

					if (0 == other || other != victim->GetPlayerID())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need your fellow player's approval for this."));
						return;
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need your fellow player's approval for this."));
					return;
				}
			}

			s_emotion_set.insert(std::make_pair(ch->GetVID(), victim->GetVID()));
		}
	}

	char chatbuf[256+1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %u %u", emotion_types[i].command_to_client, (DWORD) ch->GetVID(), victim ? (DWORD) victim->GetVID() : 0);

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	++len;

	TPacketGCChat pack_chat;
	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = CHAT_TYPE_COMMAND;
	pack_chat.id = 0;
	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	ch->PacketAround(buf.read_peek(), buf.size());

	if (victim)
		sys_log(1, "ACTION: %s TO %s", emotion_types[i].command, victim->GetName());
	else
		sys_log(1, "ACTION: %s", emotion_types[i].command);
}

