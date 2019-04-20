#pragma once

struct mdFile
{
	char szFileLocation[128];
	uint32_t iCorrectDigestArray[4];
	unsigned char szRawLocalDigest[16];
};

bool FileCheckSum();
bool CheckFile(mdFile* mdChkFile);