#include "stdafx.h"
#include <sa2modloader.h>
#include <vector>
#include <memory.h>
#include <cwe_api.h>

std::vector<const char*> MsgAlItem;

static std::vector<const char*> MsgFoName;

size_t RegisterAlItemString(const char* str) {
	MsgAlItem.push_back(str);
	return MsgAlItem.size() - 1;
}

void OverwriteAlItemString(size_t id, const char* str) {
	if (id >= MsgAlItem.size()) {
		PrintDebug("OverwriteAlItemString: invalid id %d, size of MsgAlItem is %d", id, MsgAlItem.size());
		return;
	}

	MsgAlItem[id] = str;
}

size_t RegisterFoName(char* str) {
	int len = strlen(str);
	char* buffer = new char[len + 1];
	memset(buffer, 0, len + 1);
	for (int i = 0; i < len; i++)
	{
		char finalChar = 0x5F;
		if (str[i] >= 'a' && str[i] <= 'z')
			finalChar = (str[i] - 'a') + 0x41;
		else if (str[i] >= 'A' && str[i] <= 'Z')
			finalChar = (str[i] - 'A') + 0x21;
		else if (str[i] >= '0' && str[i] <= '9')
			finalChar = (str[i] - '0') + 0x10;
		else if (str[i] == ' ')
			finalChar = 0x5F;

		buffer[i] = finalChar;
	}
	buffer[len] = 0;
	MsgFoName.push_back(buffer);
	return MsgFoName.size() - 1;
}

CWE_API_REGISTER_MSG AL_ModAPI_Msg = {
	.Version = CWE_API_REGISTER_MSG_VER,

	.AddFortuneTellerName = RegisterFoName,
	.AddAlItemString = RegisterAlItemString,
	.SetAlItemString = OverwriteAlItemString
};

const int sub_57A5D0Ptr = 0x57A5D0;
int* sub_57A5D0(int a1, const char* a2)
{
	int* result;
	__asm
	{
		mov ecx, a1
		push[a2]
		call sub_57A5D0Ptr
		add esp, 4
		mov result, eax
	}
	return result;
}

void LoadMsgMemIntoVec(int* pFile, std::vector<const char*>& vec) {
	if (!pFile)
		return;
	
	int i = 0;
	while (pFile[i] != -1)
	{
		unsigned int* v13 = (unsigned int*)&pFile[i++];
		if (!IsByteswapped(v13))
		{
			*v13 = ((*v13 & 0xFF00 | (*v13 << 16)) << 8) | (((*v13 >> 16) | *v13 & 0xFF0000) >> 8);
		}
		const char* nameTest = (const char*)(*v13 + (char*)pFile);
		vec.push_back(nameTest);
	}
}

void LoadMsgFileIntoVec(const char* filename, std::vector<const char*>& vec)
{
	int* loadedFile = sub_57A5D0(TextLanguage, filename);
	LoadMsgMemIntoVec(loadedFile, vec);
	//FREE(loadedFile);
}

void* GenerateMsgBuffer(std::vector<Uint8>& buffer, std::vector<const char*>& msgvec) {
	buffer.clear();

	int pointer = 4 * (msgvec.size() + 1); //make pointers start from 4 * (msg entries + FFFFFFF entry)
	for (size_t i = 0; i < msgvec.size(); i++) {
		unsigned char* ptrBuffer = (unsigned char*)&pointer;

		static_assert(sizeof(pointer) == 4);
		buffer.push_back(ptrBuffer[3]);
		buffer.push_back(ptrBuffer[2]);
		buffer.push_back(ptrBuffer[1]);
		buffer.push_back(ptrBuffer[0]);

		pointer += strlen(msgvec[i]) + 1; //string + terminator
	}

	buffer.push_back(0xFF);
	buffer.push_back(0xFF);
	buffer.push_back(0xFF);
	buffer.push_back(0xFF);

	for (size_t i = 0; i < msgvec.size(); i++) {
		for (size_t j = 0; j < strlen(msgvec[i]); j++) {
			buffer.push_back(msgvec[i][j]);
		}

		//string terminator
		buffer.push_back(0);
	}

	return buffer.data();
}

void* GenerateMsgFoNameBuffer() {
	static std::vector<unsigned char> MsgFoNameBuffer;
	return GenerateMsgBuffer(MsgFoNameBuffer, MsgFoName);
}

void* GenerateMsgItemBuffer() {
	static std::vector<unsigned char> MsgBuffer;
	return GenerateMsgBuffer(MsgBuffer, MsgAlItem);
}

void AL_ModAPI_Msg_Init() {
	LoadMsgFileIntoVec("MsgAlItem", MsgAlItem);
	LoadMsgFileIntoVec("MsgAlKinderFoName", MsgFoName);

	//load msgalitem custom
	WriteCall((void*)0x0058B71C, GenerateMsgItemBuffer);
	WriteData<2>((char*)0x0058B534, (char)0x90); //kill dealloc
	WriteData<6>((char*)0x0058B525, (char)0x90);

	WriteCall((void*)0x005835B6, GenerateMsgFoNameBuffer);
	WriteData<2>((char*)0x00583463, (char)0x90);
	WriteData<6>((char*)0x00583459, (char)0x90);
}