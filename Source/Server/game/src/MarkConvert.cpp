#include "stdafx.h"
#include "MarkManager.h"

#define OLD_MARK_INDEX_FILENAME "guild_mark.idx"
#define OLD_MARK_DATA_FILENAME "guild_mark.tga"

static Pixel * LoadOldGuildMarkImageFile()
{
	FILE * fp = fopen(OLD_MARK_DATA_FILENAME, "rb");

	if (!fp)
	{
		sys_err("cannot open %s", OLD_MARK_INDEX_FILENAME);
		return NULL;
	}

	int dataSize = 512 * 512 * sizeof(Pixel);
	Pixel * dataPtr = (Pixel *) malloc(dataSize);

	fread(dataPtr, dataSize, 1, fp);

	fclose(fp);

	return dataPtr;
}

bool GuildMarkConvert(const std::vector<DWORD> & vecGuildID)
{
	mkdir("mark", S_IRWXU);

	if (0 != access(OLD_MARK_INDEX_FILENAME, F_OK))
		return true;

	FILE* fp = fopen(OLD_MARK_INDEX_FILENAME, "r");

	if (NULL == fp)
		return false;

	Pixel * oldImagePtr = LoadOldGuildMarkImageFile();

	if (NULL == oldImagePtr)
	{
		fclose(fp);
		return false;
	}

	sys_log(0, "Guild Mark Converting Start.");

	char line[256];
	DWORD guild_id;
	DWORD mark_id;
	Pixel mark[SGuildMark::SIZE];

	while (fgets(line, sizeof(line)-1, fp))
	{
		sscanf(line, "%u %u", &guild_id, &mark_id);

		if (find(vecGuildID.begin(), vecGuildID.end(), guild_id) == vecGuildID.end())
		{
			sys_log(0, "  skipping guild ID %u", guild_id);
			continue;
		}

		uint row = mark_id / 32;
		uint col = mark_id % 32;

		if (row >= 42)
		{
			sys_err("invalid mark_id %u", mark_id);
			continue;
		}

		uint sx = col * 16;
		uint sy = row * 12;

		Pixel * src = oldImagePtr + sy * 512 + sx;
		Pixel * dst = mark;

		for (int y = 0; y != SGuildMark::HEIGHT; ++y)
		{
			for (int x = 0; x != SGuildMark::WIDTH; ++x)
				*(dst++) = *(src+x);

			src += 512;
		}

		CGuildMarkManager::instance().SaveMark(guild_id, (BYTE *) mark);
		line[0] = '\0';
	}

	free(oldImagePtr);
	fclose(fp);

	system("mv -f guild_mark.idx guild_mark.idx.removable");
	system("mv -f guild_mark.tga guild_mark.tga.removable");

	sys_log(0, "Guild Mark Converting Complete.");

	return true;
}

