if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import os
import localeInfo
import debugInfo

(sv1_ip, sv1_name) = net.GetServer_IPNAME()
(ch1_port, ch2_port, ch3_port, ch4_port, auth_port) = net.GetServer_PORT()

SRV1 = {
	"host" : sv1_ip,
	"name" : sv1_name,
	"auth1" : auth_port,
	"ch1" : ch1_port,
	"ch2" : ch2_port,
	"ch3" : ch3_port,
	"ch4" : ch4_port,
}

CHINA_PORT = 50000

def BuildServerList(orderList):
	retMarkAddrDict = {}
	retAuthAddrDict = {}
	retRegion0 = {}

	ridx = 1
	for region, auth, mark, channels in orderList:
		cidx = 1
		channelDict = {}
		for channel in channels:
			key = ridx * 10 + cidx
			channel["key"] = key
			channelDict[cidx] = channel
			cidx += 1

		region["channel"] = channelDict

		retRegion0[ridx] = region
		retAuthAddrDict[ridx] = auth
		retMarkAddrDict[ridx*10] = mark
		ridx += 1

	return retRegion0, retAuthAddrDict, retMarkAddrDict
	
app.ServerName = None

if app.ENABLE_RENEWAL_SERVER_LIST:
	STATE_NONE = localeInfo.CHANNEL_STATUS_OFFLINE
	STATE_DICT = {
		0 : localeInfo.CHANNEL_STATUS_OFFLINE,
		1 : localeInfo.CHANNEL_STATUS_RECOMMENDED,
		2 : localeInfo.CHANNEL_STATUS_RECOMMENDED,
		3 : localeInfo.CHANNEL_STATUS_BUSY,
		4 : localeInfo.CHANNEL_STATUS_FULL,
	}

	STATE_DICT_NAME = {
		localeInfo.CHANNEL_STATUS_OFFLINE : localeInfo.CHANNEL_STATUS_OFFLINE,
		localeInfo.CHANNEL_STATUS_VACANT : localeInfo.CHANNEL_STATUS_VACANT,
		localeInfo.CHANNEL_STATUS_RECOMMENDED : localeInfo.CHANNEL_STATUS_RECOMMENDED,
		localeInfo.CHANNEL_STATUS_BUSY : localeInfo.CHANNEL_STATUS_BUSY,
		localeInfo.CHANNEL_STATUS_FULL : localeInfo.CHANNEL_STATUS_FULL,
	}

	STATE_DICT_IMAGES = {
		localeInfo.CHANNEL_STATUS_OFFLINE : "offline.png",
		localeInfo.CHANNEL_STATUS_VACANT : "online.png",
		localeInfo.CHANNEL_STATUS_RECOMMENDED : "online.png",
		localeInfo.CHANNEL_STATUS_BUSY : "online.png",
		localeInfo.CHANNEL_STATUS_FULL : "online.png",
	}

	STATE_REVERSE_DICT = {}
	STATE_COLOR_DICT = { "..." : 0xffdadada}
	STATE_COLOR_LIST = [ 0xffffffff, 0xff00ff00, 0xff00ff00, 0xffffc000, 0xffff0000 ]

	idx = 0
	for key, value in STATE_DICT.items():
		STATE_REVERSE_DICT[value] = key
		STATE_COLOR_DICT[value] = STATE_COLOR_LIST[idx%len(STATE_COLOR_LIST)]
		idx += 1

	SERVER_STATE_DICT = {
		"NONE" : 0,
		"NEW" : 1,
		"SPECIAL" : 2,
		"CLOSE" : 3
	}
else:
	STATE_NONE = "OFF"
	STATE_DICT = {
		0 : "OFF",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	STATE_DICT_NAME = {
		"OFF" : localeInfo.CHANNEL_STATUS_OFFLINE,
		"NORM" : localeInfo.CHANNEL_STATUS_RECOMMENDED,
		"BUSY" : localeInfo.CHANNEL_STATUS_BUSY,
		"FULL" : localeInfo.CHANNEL_STATUS_FULL,
	}

	STATE_DICT_IMAGES = {
		localeInfo.CHANNEL_STATUS_OFFLINE : "offline.png",
		localeInfo.CHANNEL_STATUS_RECOMMENDED : "online.png",
		localeInfo.CHANNEL_STATUS_BUSY : "online.png",
		localeInfo.CHANNEL_STATUS_FULL : "online.png",
	}

if not app.ENABLE_RENEWAL_SERVER_LIST:
	if app.ENABLE_CHANNEL_LIST:
		STATE_NONE = "Offline"
		STATE_DICT = { 0 : "Offline", 1 : "Available", 2 : "Busy", 3 : "Full" }
		STATE_COLOR_DICT = { "Offline" : 0xffdadada, "Available" : 0xff00ff00, "Busy" : 0xffffff00, "Full" : 0xffff0000}
	else:
		STATE_NONE = "..."

		STATE_DICT = {
			0 : "....",
			1 : "NORM",
			2 : "BUSY",
			3 : "FULL"
		}

SERVER1_CHANNEL_DICT = {
	1 : {
		"key" : 11,
		"name" : "CH1   ",
		"ip" : SRV1["host"],
		"tcp_port" : SRV1["ch1"],
		"udp_port" : SRV1["ch1"],
		"state" : STATE_NONE,
	},
	2 : {
		"key" : 12,
		"name":"CH2   ",
		"ip" : SRV1["host"],
		"tcp_port" : SRV1["ch2"],
		"udp_port" : SRV1["ch2"],
		"state" : STATE_NONE,
	},
	3 : {
		"key" : 13,
		"name" : "CH3   ",
		"ip" : SRV1["host"],
		"tcp_port" : SRV1["ch3"],
		"udp_port" : SRV1["ch3"],
		"state" : STATE_NONE,
	},
	4 : {
		"key" : 14,
		"name" : "CH4   ",
		"ip" : SRV1["host"],
		"tcp_port" : SRV1["ch4"],
		"udp_port" : SRV1["ch4"],
		"state" : STATE_NONE,
	},
}

REGION_NAME_DICT = {
	0 : SRV1["name"],
}

REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : {
			"ip" : SRV1["host"],
			"port" : SRV1["auth1"],
		},
	}
}

REGION_DICT = {
	0 : {
		1 : {
			"state" : "NEW",
			"name" : SRV1["name"],
			"channel" : SERVER1_CHANNEL_DICT,
		},
	},
}

MARKADDR_DICT = {
	10 : {
		"ip" : SRV1["host"],
		"tcp_port" : SRV1["ch1"],
		"mark" : "10.tga",
		"symbol_path" : "10",
	},
}

TESTADDR = {
	"ip" : SRV1["host"],
	"tcp_port" : SRV1["ch1"],
	"udp_port" : SRV1["ch1"],
}
