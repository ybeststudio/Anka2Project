#include "StdAfx.h"

#include <windows.h>
#include <mmsystem.h>
#include <io.h>
#include <assert.h>

#include "EterPack.h"
#include "Inline.h"

#pragma warning(push, 3)
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/tiger.h>
#include <cryptopp/sha.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/panama.h>

#include <cryptopp/cryptoppLibLink.h>
#pragma warning(pop)

#include "../EterBase/utils.h"
#include "../EterBase/Debug.h"
#include "../EterBase/CRC32.h"

#include <iostream>
#include <fstream>

void CMakePackLog::SetFileName(const char *c_szFileName)
{
	m_stFileName=c_szFileName;
	m_stFileName +=".log";
	m_stErrorFileName = c_szFileName;
	m_stErrorFileName += ".err";
}

CMakePackLog& CMakePackLog::GetSingleton()
{
	static CMakePackLog s_kMakePackLog;
	return s_kMakePackLog;
}

CMakePackLog::CMakePackLog()
{
	m_fp=NULL;
	m_fp_err = nullptr;
}

CMakePackLog::~CMakePackLog()
{
	if (NULL!=m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}
	if (NULL != m_fp_err)
	{
		fclose(m_fp_err);
		m_fp_err = nullptr;
	}
}

bool CMakePackLog::__IsLogMode()
{
	if (m_stFileName.empty())
		return false;

	return true;
}

void CMakePackLog::Writef(const char *c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024];
	int nBufLen = _vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	szBuf[nBufLen++] = '\0';
	__Write(szBuf, nBufLen);
}

void CMakePackLog::Writenf(const char *c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024+1];
	int nBufLen = _vsnprintf(szBuf, sizeof(szBuf)-1, c_szFormat, args);
	if (nBufLen > 0)
	{
		szBuf[nBufLen++] = '\n';
		szBuf[nBufLen++] = '\0';
	}
	__Write(szBuf, nBufLen);
}

void CMakePackLog::Write(const char *c_szBuf)
{
	if (!__IsLogMode())
		return;

	__Write(c_szBuf, strlen(c_szBuf)+1);
}

void CMakePackLog::__Write(const char *c_szBuf, int nBufLen)
{
	if (!__IsLogMode())
		return;

	if (NULL==m_fp)
		m_fp=fopen(m_stFileName.c_str(), "w");

	fwrite(c_szBuf, nBufLen, 1, m_fp);

	printf("%s", c_szBuf);
}



void CMakePackLog::WriteErrorf(const char *c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024];
	int nBufLen = _vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	szBuf[nBufLen++] = '\0';
	__WriteError(szBuf, nBufLen);
}

void CMakePackLog::WriteErrornf(const char *c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024+1];
	int nBufLen = _vsnprintf(szBuf, sizeof(szBuf)-1, c_szFormat, args);
	if (nBufLen > 0)
	{
		szBuf[nBufLen++] = '\n';
		szBuf[nBufLen++] = '\0';
	}
	__WriteError(szBuf, nBufLen);
}

void CMakePackLog::WriteError(const char *c_szBuf)
{
	if (!__IsLogMode())
		return;

	__WriteError(c_szBuf, strlen(c_szBuf)+1);
}

void CMakePackLog::__WriteError(const char *c_szBuf, int nBufLen)
{
	if (!__IsLogMode())
		return;

	if (NULL==m_fp_err)
		m_fp_err=fopen(m_stErrorFileName.c_str(), "w");

	fwrite(c_szBuf, nBufLen, 1, m_fp_err);

	printf("Error: %s", c_szBuf);
}

void CMakePackLog::FlushError()
{
	std::wifstream iFile(m_stErrorFileName.c_str());
	std::istream_iterator <std::wstring, wchar_t> iit(iFile);
	std::istream_iterator <std::wstring, wchar_t> eos;

	std::vector <std::wstring> vText;

	std::copy (iit, eos, std::back_inserter(vText));

	std::ostream_iterator <std::wstring, wchar_t, std::char_traits <wchar_t> > oit(std::wcout);

	std::sort (vText.begin(), vText.end());
	std::copy (vText.begin(), vText.end(), oit);
}

CEterPack::CEterPack() : m_indexCount(0), m_indexData(nullptr), m_FragmentSize(0), m_bEncrypted(false), m_bReadOnly(false), m_bDecrypedIV(false) {}

CEterPack::~CEterPack()
{
	Destroy();
}

void CEterPack::Destroy()
{
	m_bReadOnly = false;
	m_bEncrypted = false;
	m_indexCount = 0;
	m_DataPositionMap.clear();

	for (int i = 0; i < FREE_INDEX_MAX_SIZE + 1; ++i)
		m_FreeIndexList[i].clear();

	SAFE_DELETE_ARRAY(m_indexData);

	m_FragmentSize = 0;

	memset(m_dbName, 0, sizeof(m_dbName));
	memset(m_indexFileName, 0, sizeof(m_indexFileName));	
}

const std::string& CEterPack::GetPathName()
{
	return m_stPathName;
}

bool CEterPack::Create(CEterFileDict& rkFileDict, const char *dbname, const char *pathName, bool bReadOnly, const BYTE* iv)
{
	if (iv)
	{
		m_bDecrypedIV = false;
	}

	m_stPathName = pathName;

	strncpy(m_dbName, dbname, DBNAME_MAX_LEN);

	strncpy(m_indexFileName, dbname, MAX_PATH);
	strcat(m_indexFileName, ".eix");

	m_stDataFileName = dbname;
	m_stDataFileName += ".epk";

	m_bReadOnly = bReadOnly;

	if (!CreateIndexFile())
		return false;

	if (!CreateDataFile())
		return false;

	bool bOverwrite = (iv != nullptr);
	__BuildIndex(rkFileDict, bOverwrite);

	if (m_bReadOnly)
	{}
	else
	{
		DecryptIndexFile();
	}

	return true;
}

bool CEterPack::DecryptIndexFile()
{
	if (!m_bEncrypted)
		return true;

	CFileBase file;
	
	if (!file.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
		return false;

	file.Write(&eterpack::c_IndexCC, sizeof(DWORD));
	file.Write(&eterpack::c_Version, sizeof(DWORD));
	file.Write(&m_indexCount, sizeof(long));
	file.Write(m_indexData, sizeof(TEterPackIndex) * m_indexCount);

	file.Close();

	m_bEncrypted = false;
	return true;
}

#ifdef ENABLE_ETERPACK_OBSCURING
static std::string s_strEterPackKey[][10] = {
	{"2", "0", "2", "0", "1", "7", "7", "7", "8", "0"},
	{"1", "9", "8", "7", "2", "0", "9", "5", "8", "2"},
	{"1", "8", "6", "8", "6", "5", "6", "2", "2", "5"},
	{"1", "8", "5", "2", "4", "0", "2", "7", "9", "8"}
};

static std::string s_strEterPackSecurityKey[][10] = {
	{"2", "0", "2", "0", "1", "7", "7", "7", "8", "0"},
	{"1", "8", "6", "8", "7", "8", "8", "5", "8", "5"},
	{"1", "7", "3", "5", "2", "8", "9", "1", "9", "7"},
	{"1", "8", "0", "1", "6", "7", "5", "1", "0", "6"}
};

static DWORD s_EncryptEterPackKey(int key)
{
	std::string sEterPackKey = "";
	int iEterPackKey = 0;

	for (int iter = 0; iter < (sizeof(s_strEterPackKey[key]) / sizeof(std::string)); ++iter)
	{
		sEterPackKey.append(s_strEterPackKey[key][iter]);
	}

	iEterPackKey = atoi(sEterPackKey.c_str());

	return iEterPackKey;
}

static DWORD s_EncryptEterPackSecurityKey(int key)
{
	std::string sEterPackSecurityKey = "";
	int iEterPackSecurityKey = 0;

	for (int iter = 0; iter < (sizeof(s_strEterPackSecurityKey[key]) / sizeof(std::string)); ++iter)
	{
		sEterPackSecurityKey.append(s_strEterPackSecurityKey[key][iter]);
	}

	iEterPackSecurityKey = atoi(sEterPackSecurityKey.c_str());

	return iEterPackSecurityKey;
}
#endif

static DWORD /*XTEA*/ s_adwEterPackKey[] = {
#ifdef ENABLE_ETERPACK_OBSCURING
	s_EncryptEterPackKey(0), // 2020177780
	s_EncryptEterPackKey(1), // 1987209582
	s_EncryptEterPackKey(2), // 1868656225
	s_EncryptEterPackKey(3) // 1852402798
#else
	45129401,
	92367215,
	681285731,
	1710201
#endif
};

static DWORD /*LZO*/ s_adwEterPackSecurityKey[] = {
#ifdef ENABLE_ETERPACK_OBSCURING
	s_EncryptEterPackSecurityKey(0), // 2020177780
	s_EncryptEterPackSecurityKey(1), // 1868788585
	s_EncryptEterPackSecurityKey(2), // 1735289197
	s_EncryptEterPackSecurityKey(3) // 1801675106
#else
	78952482,
	527348324,
	1632942,
	486274726
#endif
};

bool CEterPack::EncryptIndexFile()
{
	CMappedFile file;
	LPCVOID pvData;

	if (NULL == file.Create(m_indexFileName, &pvData, 0, 0))
	{
		TraceError("EncryptIndex: Cannot open pack index file! %s", m_dbName);
		return false;
	}

	BYTE * pbData = new BYTE[file.Size()];
	memcpy(pbData, pvData, file.Size());

	CLZObject zObj;
	
	if (!CLZO::Instance().CompressEncryptedMemory(zObj, pbData, file.Size(), s_adwEterPackKey))
	{
		TraceError("EncryptIndex: Cannot encrypt! %s", m_dbName);
		SAFE_DELETE_ARRAY(pbData);
		return false;
	}

	file.Destroy();

	while (!DeleteFile(m_indexFileName));

	FILE * fp;

	fp = fopen(m_indexFileName, "wb");

	if (!fp)
	{
		TraceError("EncryptIndex: Cannot open file for writing! %s", m_dbName);
		SAFE_DELETE_ARRAY(pbData);
		return false;
	}

	if (1 != fwrite(zObj.GetBuffer(), zObj.GetSize(), 1, fp))
	{
		TraceError("Encryptindex: Cannot write to file! %s", m_indexFileName);
		SAFE_DELETE_ARRAY(pbData);
		fclose(fp);
		return false;
	}
	
	fclose(fp);

	m_bEncrypted = true;
	delete [] pbData;
	return true;
}

bool CEterPack::__BuildIndex(CEterFileDict& rkFileDict, bool bOverwrite)
{
	CMappedFile file;
	LPCVOID pvData;
	CLZObject zObj;

	if (NULL == file.Create(m_indexFileName, &pvData, 0, 0))
	{
		TraceError("Cannot open pack index file! %s", m_dbName);
		return false;
	}

	if (file.Size() < eterpack::c_HeaderSize)
	{
		TraceError("Pack index file header error! %s", m_dbName);
		return false;
	}

	DWORD fourcc = *(DWORD *) pvData;

	BYTE * pbData;
	UINT uiFileSize;

	if (fourcc == eterpack::c_IndexCC)
	{
		pbData = (BYTE *) pvData;
		uiFileSize = file.Size();
	}
	else if (fourcc == CLZObject::ms_dwFourCC)
	{
		m_bEncrypted = true;

		if (!CLZO::Instance().Decompress(zObj, (const BYTE *) pvData, s_adwEterPackKey))
			return false;

		if (zObj.GetSize() < eterpack::c_HeaderSize)
			return false;

		pbData = zObj.GetBuffer();
		uiFileSize = zObj.GetSize();
	}
	else
	{
		TraceError("Pack index file fourcc error! %s", m_dbName);
		return false;
	}

	pbData += sizeof(DWORD);

	DWORD ver = *(DWORD *) pbData;
	pbData += sizeof(DWORD);

	if (ver != eterpack::c_Version)
	{
		TraceError("Pack index file version error! %s", m_dbName);
		return false;
	}

	m_indexCount = *(long *) pbData;
	pbData += sizeof(long);

	if (uiFileSize < eterpack::c_HeaderSize + sizeof(TEterPackIndex) * m_indexCount)
	{
		TraceError("Pack index file size error! %s, indexCount %d", m_dbName, m_indexCount);
		return false;
	}

	m_indexData = new TEterPackIndex[m_indexCount];
	memcpy(m_indexData, pbData, sizeof(TEterPackIndex) * m_indexCount);

	TEterPackIndex * index = m_indexData;

	for (int i = 0; i < m_indexCount; ++i, ++index)
	{
		if (!index->filename_crc)
		{
			PushFreeIndex(index);
		}
		else
		{
			if (index->real_data_size > index->data_size)
				m_FragmentSize += index->real_data_size - index->data_size;

			m_DataPositionMap.insert(TDataPositionMap::value_type(index->filename_crc, index));

			if (bOverwrite)
				rkFileDict.UpdateItem(this, index);
			else
				rkFileDict.InsertItem(this, index);
		}
	}

	return true;
}

bool CEterPack::Get(CMappedFile& out_file, const char *filename, LPCVOID * data)
{
	TEterPackIndex * index = FindIndex(filename);
	
	if (!index)
	{
		return false;
	}

	out_file.Create(m_stDataFileName.c_str(), data, index->data_position, index->data_size);

	bool bIsSecurityCheckRequired = ( index->compressed_type == COMPRESSED_TYPE_SECURITY);

	if( bIsSecurityCheckRequired )
	{
		DWORD dwCrc32 = GetCRC32((const char *)(*data), index->data_size);

		if( index->data_crc != dwCrc32 )
		{
			return false;
		}
	}

	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data)))
		{
			TraceError("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data), s_adwEterPackSecurityKey))
		{
			TraceError("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	return true;
}

bool CEterPack::Get2(CMappedFile& out_file, const char *filename, TEterPackIndex * index, LPCVOID * data)
{
	if (!index)
	{
		return false;
	}

	out_file.Create(m_stDataFileName.c_str(), data, index->data_position, index->data_size);

	bool bIsSecurityCheckRequired = ( index->compressed_type == COMPRESSED_TYPE_SECURITY);

	if( bIsSecurityCheckRequired )
	{
		DWORD dwCrc32 = GetCRC32((const char *)(*data), index->data_size);

		if( index->data_crc != dwCrc32 )
		{
			return false;
		}
	}

	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data)))
		{
			TraceError("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data), s_adwEterPackSecurityKey))
		{
			TraceError("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	return true;
}

bool CEterPack::Delete(TEterPackIndex * pIndex)
{
	CFileBase fileIndex;

	if (!fileIndex.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
		return false;

	PushFreeIndex(pIndex);
	WriteIndex(fileIndex, pIndex);
	return true;
}

bool CEterPack::Delete(const char *filename)
{
	TEterPackIndex * pIndex = FindIndex(filename);

	if (!pIndex)
		return false;

	return Delete(pIndex);
}

bool CEterPack::Extract()
{
	CMappedFile dataMapFile;
	LPCVOID		data;

	if (!dataMapFile.Create(m_stDataFileName.c_str(), &data, 0, 0))
		return false;

	CLZObject zObj;
	
	for (TDataPositionMap::iterator i = m_DataPositionMap.begin();
		i != m_DataPositionMap.end();
		++i)
	{
		TEterPackIndex* index = i->second;
		CFileBase writeFile;
		
		inlinePathCreate(index->filename);
		printf("%s\n", index->filename);
		
		writeFile.Create(index->filename, CFileBase::FILEMODE_WRITE);

		if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
		{
			if (!CLZO::Instance().Decompress(zObj, (const BYTE *) data + index->data_position))
			{
				printf("cannot decompress");
				return false;
			}

			writeFile.Write(zObj.GetBuffer(), zObj.GetSize());
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
		{
			if (!CLZO::Instance().Decompress(zObj, (const BYTE *) data + index->data_position, s_adwEterPackSecurityKey))
			{
				printf("cannot decompress");
				return false;
			}

			writeFile.Write(zObj.GetBuffer(), zObj.GetSize());
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_NONE == index->compressed_type)
			writeFile.Write((const char *) data + index->data_position, index->data_size);
		
		writeFile.Destroy();
	}
	return true;
}

bool CEterPack::GetNames(std::vector<std::string>* retNames)
{
	CMappedFile dataMapFile;
	LPCVOID		data;

	if (!dataMapFile.Create(m_stDataFileName.c_str(), &data, 0, 0))
		return false;

	CLZObject zObj;
	
	for (TDataPositionMap::iterator i = m_DataPositionMap.begin();
		i != m_DataPositionMap.end();
		++i)
	{
		TEterPackIndex* index = i->second;
		
		inlinePathCreate(index->filename);

		retNames->push_back(index->filename);
	}
	return true;
}

bool CEterPack::Put(const char *filename, const char *sourceFilename, BYTE packType, const std::string& strRelateMapName )
{
	CMappedFile mapFile;
	LPCVOID		data;

	if (sourceFilename)
	{
		if (!mapFile.Create(sourceFilename, &data, 0, 0))
		{
			return false;
		}
	}
	else if (!mapFile.Create(filename, &data, 0, 0))
	{
		return false;
	}

	BYTE*  pMappedData    = (BYTE*)data;
	int	   iMappedDataSize = mapFile.Size();

	return Put(filename, pMappedData, iMappedDataSize, packType);
}

bool CEterPack::Put(const char *filename, LPCVOID data, long len, BYTE packType)
{
	if (m_bEncrypted)
	{
		TraceError("EterPack::Put : Cannot put to encrypted pack (filename: %s, DB: %s)", filename, m_dbName);
		return false;
	}

	CFileBase fileIndex;

	if (!fileIndex.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
	{
		return false;
	}

	CFileBase fileData;

	if (!fileData.Create(m_stDataFileName.c_str(), CFileBase::FILEMODE_WRITE))
	{
		return false;
	}

	TEterPackIndex * pIndex;
	pIndex = FindIndex(filename);

	CLZObject zObj;
	std::string encryptStr;

	if (packType == COMPRESSED_TYPE_SECURITY || 
		packType == COMPRESSED_TYPE_COMPRESS)
	{
		if (packType == COMPRESSED_TYPE_SECURITY)
		{
			if (!CLZO::Instance().CompressEncryptedMemory(zObj, data, len, s_adwEterPackSecurityKey))
			{
				return false;
			}
		}
		else
		{
			if (!CLZO::Instance().CompressMemory(zObj, data, len))
			{
				return false;
			}
		}

		data = zObj.GetBuffer();
		len = zObj.GetSize();
	}

	DWORD data_crc;
	data_crc = GetCRC32((const char *) data, len);

	if (pIndex)
	{
		if (pIndex->real_data_size >= len)
		{
			++m_map_indexRefCount[pIndex->id];

			if ((pIndex->data_size != len) || (pIndex->data_crc != data_crc))
			{
				pIndex->data_size = len;
				pIndex->data_crc = data_crc;
				pIndex->compressed_type = packType;

				CMakePackLog::GetSingleton().Writef("Overwrite[type:%u] %s\n", pIndex->compressed_type, pIndex->filename);

				WriteIndex(fileIndex, pIndex);
				WriteData(fileData, pIndex, data);
			}

			return true;
		}

		PushFreeIndex(pIndex);
		WriteIndex(fileIndex, pIndex);
	}

	pIndex = NewIndex(fileIndex, filename, len);
	pIndex->data_size = len;
	pIndex->data_crc = data_crc;

	pIndex->data_position = GetNewDataPosition(fileData);
	pIndex->compressed_type = packType;

	WriteIndex(fileIndex, pIndex);
	WriteNewData(fileData, pIndex, data);

	++m_map_indexRefCount[pIndex->id];

	CMakePackLog::GetSingleton().Writef("Write[type:%u] %s\n", pIndex->compressed_type, pIndex->filename);

	return true;
}

long CEterPack::GetFragmentSize()
{
	return m_FragmentSize;
}

bool CEterPack::CreateIndexFile()
{
	FILE * fp;

	if (NULL != (fp = fopen(m_indexFileName, "rb")))
	{
		fclose(fp);
		return true;
	}
	else if (m_bReadOnly)
		return false;

	fp = fopen(m_indexFileName, "wb");

	if (!fp)
		return false;

	fwrite(&eterpack::c_IndexCC, sizeof(DWORD), 1, fp);
	fwrite(&eterpack::c_Version, sizeof(DWORD), 1, fp);
	fwrite(&m_indexCount, sizeof(long), 1, fp);

	fclose(fp);
	return true;
}

void CEterPack::WriteIndex(CFileBase & file, TEterPackIndex * index)
{
	file.Seek(sizeof(DWORD) + sizeof(DWORD));
	file.Write(&m_indexCount, sizeof(long));
	file.Seek(eterpack::c_HeaderSize + (index->id * sizeof(TEterPackIndex)));

	if (!file.Write(index, sizeof(TEterPackIndex)))
	{
		assert(!"WriteIndex: fwrite failed");
		return;
	}
}

/*
 *	Free Block 이란 데이터에서 지워진 부�?을 말한다.
 *	Free Block 들은 각각 FREE_INDEX_BLOCK_SIZE (32768) 단위로 나누어져
 *	리스트로 관리된다.
 *
 *	예를 들어 128k 의 데이터는
 *	128 * 1024 / FREE_INDEX_BLOCK_SIZE = 4 이므로
 *	최종 적으로는 m_FreeIndexList[4] 에 들어간다.
 *
 *	FREE_INDEX_BLOCK_SIZE 의 최대 값은 FREE_INDEX_MAX_SIZE(512) 이다.
 *	따라서 16MB 이상의 데이터는 무조건 배열의 512 위치에 들어간다.
 */
int CEterPack::GetFreeBlockIndex(long size)
{
	return min(FREE_INDEX_MAX_SIZE, size / FREE_INDEX_BLOCK_SIZE);
}

void CEterPack::PushFreeIndex(TEterPackIndex* index)
{
	if (index->filename_crc != 0)
	{
		TDataPositionMap::iterator i = m_DataPositionMap.find(index->filename_crc);

		if (i != m_DataPositionMap.end())
			m_DataPositionMap.erase(i);

		index->filename_crc = 0;
		memset(index->filename, 0, sizeof(index->filename));
	}

	int blockidx = GetFreeBlockIndex(index->real_data_size);
	m_FreeIndexList[blockidx].push_back(index);
	m_FragmentSize += index->real_data_size;
}

long CEterPack::GetNewIndexPosition(CFileBase & file)
{
	long pos = (file.Size() - eterpack::c_HeaderSize) / sizeof(TEterPackIndex);
	++m_indexCount;
	return (pos);
}

TEterPackIndex* CEterPack::NewIndex(CFileBase& file, const char *filename, long size)
{
	TEterPackIndex* index = nullptr;
	int block_size = size + (DATA_BLOCK_SIZE - (size % DATA_BLOCK_SIZE));

	int blockidx = GetFreeBlockIndex(block_size);

	for (TFreeIndexList::iterator i = m_FreeIndexList[blockidx].begin(); i != m_FreeIndexList[blockidx].end(); ++i)
	{
		if ((*i)->real_data_size >= size)
		{
			index = *i;
			m_FreeIndexList[blockidx].erase(i);

			assert(index->filename_crc == 0);
			break;
		}
	}

	if (!index)
	{
		index = new TEterPackIndex;
		index->real_data_size = block_size;
		index->id = GetNewIndexPosition(file);
	}

	strncpy(index->filename, filename, FILENAME_MAX_LEN);
	index->filename[FILENAME_MAX_LEN] = '\0';
	inlineConvertPackFilename(index->filename);

	index->filename_crc	= GetCRC32(index->filename, strlen(index->filename));

	m_DataPositionMap.insert(TDataPositionMap::value_type(index->filename_crc, index));
	return index;
}

TEterPackIndex* CEterPack::FindIndex(const char *filename)
{
	static char tmpFilename[MAX_PATH + 1];
	strncpy(tmpFilename, filename, MAX_PATH);
	inlineConvertPackFilename(tmpFilename);

	DWORD filename_crc = GetCRC32(tmpFilename, strlen(tmpFilename));
	TDataPositionMap::iterator i = m_DataPositionMap.find(filename_crc);

	if (i == m_DataPositionMap.end())
		return NULL;

	return (i->second);
}

bool CEterPack::IsExist(const char *filename)
{
	return FindIndex(filename) ? true : false;
}

bool CEterPack::CreateDataFile()
{
	FILE * fp;
	
	if (NULL != (fp = fopen(m_stDataFileName.c_str(), "rb")))
	{
		fclose(fp);
		return true;
	}
	else if (m_bReadOnly)
		return false;

	fp = fopen(m_stDataFileName.c_str(), "wb");

	if (!fp)
		return false;

	fclose(fp);
	return true;
}

long CEterPack::GetNewDataPosition(CFileBase& file)
{
	return file.Size();
}

bool CEterPack::ReadData(CFileBase & file, TEterPackIndex* index, LPVOID data, long maxsize)
{
	if (index->data_size > maxsize)
		return false;

	file.Seek(index->data_position);
	file.Read(data, index->data_size);
	return true;
}

bool CEterPack::WriteData(CFileBase & file, TEterPackIndex* index, LPCVOID data)
{
	file.Seek(index->data_position);
	
	if (!file.Write(data, index->data_size))
	{
		assert(!"WriteData: fwrite data failed");
		return false;
	}

	return true;
}

bool CEterPack::WriteNewData(CFileBase& file, TEterPackIndex* index, LPCVOID data)
{
	file.Seek(index->data_position);
	
	if (!file.Write(data, index->data_size))
	{
		assert(!"WriteData: fwrite data failed");
		return false;
	}

	int empty_size = index->real_data_size - index->data_size;

	if (empty_size < 0)
	{
		printf("SYSERR: WriteNewData(): CRITICAL ERROR: empty_size lower than 0!\n");
		exit(1);
	}

	if (empty_size == 0)
		return true;

	char *empty_buf = (char *) calloc(empty_size, sizeof(char));

	if (!file.Write(empty_buf, empty_size))
	{
		assert(!"WriteData: fwrite empty data failed");
		return false;
	}

	free(empty_buf);
	return true;
}

TDataPositionMap & CEterPack::GetIndexMap()
{
	return m_DataPositionMap;
}

DWORD CEterPack::DeleteUnreferencedData()
{
	TDataPositionMap::iterator i = m_DataPositionMap.begin();
	DWORD dwCount = 0;

	while (i != m_DataPositionMap.end())
	{
		TEterPackIndex * pIndex = (i++)->second;

		if (0 == m_map_indexRefCount[pIndex->id])
		{
			printf("Unref File %s\n", pIndex->filename);
			Delete(pIndex);
			++dwCount;
		}
	}

	return dwCount;
}

const char *CEterPack::GetDBName()
{
	return m_dbName;
}

void CEterFileDict::InsertItem(CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;

	item.pkPack = pkPack;
	item.pkInfo = pkInfo;

	m_dict.insert(TDict::value_type(pkInfo->filename_crc, item));
}

void CEterFileDict::UpdateItem(CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;

	item.pkPack = pkPack;
	item.pkInfo = pkInfo;

	TDict::iterator f = m_dict.find(pkInfo->filename_crc);
	if (f == m_dict.end())	
		m_dict.insert(TDict::value_type(pkInfo->filename_crc, item));
	else
	{
		if (strcmp(f->second.pkInfo->filename, item.pkInfo->filename) == 0)
		{
			f->second = item;
		}
		else
		{
			TraceError("NAME_COLLISION: OLD: %s NEW: %s", f->second.pkInfo->filename, item.pkInfo->filename);			
		}
		
	}
}

CEterFileDict::Item* CEterFileDict::GetItem(DWORD dwFileNameHash, const char *c_pszFileName)
{
	std::pair<TDict::iterator, TDict::iterator> iter_pair = m_dict.equal_range(dwFileNameHash);

	TDict::iterator iter = iter_pair.first;

	while (iter != iter_pair.second)
	{
		Item& item = iter->second;

		if (0 == strcmp(c_pszFileName, item.pkInfo->filename))
			return &item;

		++iter;
	}

	return NULL;
}
