import os
import sys

sys.path.append("conv")

import conv

# Process special files
conv.ProcessNpcList("source/root/NpcList.csv", "source/root/NpcList.py")
conv.ProcessGuildBuildingList("source/root/GuildBuildingList.csv", "source/root/GuildBuildingList.py")