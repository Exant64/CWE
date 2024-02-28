/**
 * SADX Mod Loader
 * Code file parser.
 */

#include "stdafx.h"
#include "CodeParser.hpp"

#include <cerrno>
#include <intrin.h>
#include <fstream>
using std::istream;
using std::ifstream;
using std::list;
using std::string;
#ifdef _MSC_VER
using std::wstring;
#endif /* _MSC_VER */
using std::vector;

CodeParser::CodeParser(const string& filename)
{
	readCodes(filename);
}

CodeParser::CodeParser(istream& f)
{
	readCodes(f);
}

CodeParser::CodeParser()
{
}

CodeParser::~CodeParser()
{
	clear();
}

void* CodeParser::GetAddress(const Code& code, valuetype* regs)
{
	void* addr = code.address;
	if (addr < (void*)16)
		addr = &regs[(int)addr];
	if (!code.pointer)
		return addr;
	addr = *(void**)addr;
	if (code.offsetcount == 0 || addr == nullptr)
		return addr;
	for (int i = 0; i < code.offsetcount - 1; i++)
	{
		addr = (void*)((uint32_t)addr + code.offsets[i]);
		addr = *(void**)addr;
		if (addr == nullptr)
			return nullptr;
	}
	addr = (void*)((uint32_t)addr + code.offsets[code.offsetcount - 1]);
	return addr;
}

#define addr_add(s) addr=(valuetype*)(s+(uint8_t*)addr)

#define ifcode(size, op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->u##size op it->value.u##size; \
		addr_add(size/8); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

#define ifcodes(size, op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->s##size op it->value.s##size; \
		addr_add(size/8); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

#define ifcodef(op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->f op it->value.f; \
		addr_add(4); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

#define ifcodereg(size, op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->u##size op regs[it->value.u8].u##size; \
		addr_add(size/8); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

#define ifcoderegs(size, op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->s##size op regs[it->value.u8].s##size; \
		addr_add(size/8); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

#define ifcoderegf(op) \
do { \
	for (uint32_t i = 0; i < it->repeatcount; i++) \
	{ \
		cond &= addr->f op regs[it->value.u8].f; \
		addr_add(4); \
	} \
	if (cond) \
		regnum = processCodeList_int(it->trueCodes, regnum); \
	else \
		regnum = processCodeList_int(it->falseCodes, regnum); \
} while (0)

/*
inline BOOL WriteData(void* writeaddress, const void* data, SIZE_T datasize, SIZE_T* byteswritten)
{
	return WriteProcessMemory(GetCurrentProcess(), writeaddress, data, datasize, byteswritten);
}

inline BOOL WriteData(void* writeaddress, const void* data, SIZE_T datasize)
{
	return WriteData(writeaddress, data, datasize, nullptr);
}

template <typename T>
inline BOOL WriteData(T const* writeaddress, const T data, SIZE_T* byteswritten)
{
	return WriteData((void*)writeaddress, (void*)&data, (SIZE_T)sizeof(data), byteswritten);
}

template <typename T>
inline BOOL WriteData(T const* writeaddress, const T data)
{
	if ((uint32_t)writeaddress < 0x87342C)
		return WriteData(writeaddress, data, nullptr);
	else
		*writeaddress = data;
	return true;
}

template <typename T>
inline BOOL WriteData(T* writeaddress, const T& data, SIZE_T* byteswritten)
{
	return WriteData(writeaddress, &data, sizeof(data), byteswritten);
}

template <typename T>
inline BOOL WriteData(T* writeaddress, const T& data)
{
	if ((uint32_t)writeaddress < 0x87342C)
		return WriteData(writeaddress, data, nullptr);
	else
		*writeaddress = data;
	return true;
}

template <typename T, size_t N>
inline BOOL WriteData(void* writeaddress, const T(&data)[N], SIZE_T* byteswritten)
{
	return WriteData(writeaddress, data, SizeOfArray(data), byteswritten);
}

template <typename T, size_t N>
inline BOOL WriteData(void* writeaddress, const T(&data)[N])
{
	return WriteData(writeaddress, data, nullptr);
}
*/

template <typename T>
inline void writecode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, data);
		++address;
	}
}

template <typename T>
inline void addcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address + data));
		++address;
	}
}

template <typename T>
inline void subcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address - data));
		++address;
	}
}

template <typename T>
inline void mulcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address * data));
		++address;
	}
}

template <typename T>
inline void divcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address / data));
		++address;
	}
}

template <typename T>
inline void modcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address % data));
		++address;
	}
}

template <typename T>
inline void shlcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address << data));
		++address;
	}
}

template <typename T>
inline void shrcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address >> data));
		++address;
	}
}

template <typename T>
inline void andcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address & data));
		++address;
	}
}

template <typename T>
inline void orcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address | data));
		++address;
	}
}

template <typename T>
inline void xorcode(T* address, uint32_t repeatcount, T data)
{
	for (uint32_t i = 0; i < repeatcount; i++)
	{
		WriteData(address, (T)(*address ^ data));
		++address;
	}
}

inline uint32_t _round(float num)
{
	return (uint32_t)(num > 0 ? num + 0.5 : ceil(num - 0.5));
}

/**
 * Process a code list. (Internal recursive function)
 * @param codes Code list.
 * @param regnum Next register number.
 * @return Last register number used.
 */
int CodeParser::processCodeList_int(const list<Code>& codes, int regnum)
{
	for (auto it = codes.begin(); it != codes.end(); ++it)
	{
		if (it->newregs)
			regnum++;
		valuetype* regs = nullptr;
		if (regnum >= 0)
			regs = m_registers[regnum];

		void* address = GetAddress(*it, regs);
		auto* addr = (valuetype*)address;

		if (it->type != ifkbkey && address == nullptr)
		{
			if (distance(it->falseCodes.begin(), it->falseCodes.end()) > 0)
				regnum = processCodeList_int(it->falseCodes, regnum);
			continue;
		}

		bool cond = true;
		switch (it->type)
		{
		case write8:
			writecode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case write16:
			writecode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case write32:
		case writefloat:
			writecode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case add8:
			addcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case add16:
			addcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case add32:
			addcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case addfloat:
			addcode(&addr->f, it->repeatcount, it->value.f);
			break;
		case sub8:
			subcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case sub16:
			subcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case sub32:
			subcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case subfloat:
			subcode(&addr->f, it->repeatcount, it->value.f);
			break;
		case mulu8:
			mulcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case mulu16:
			mulcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case mulu32:
			mulcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case mulfloat:
			mulcode(&addr->f, it->repeatcount, it->value.f);
			break;
		case muls8:
			mulcode(&addr->s8, it->repeatcount, it->value.s8);
			break;
		case muls16:
			mulcode(&addr->s16, it->repeatcount, it->value.s16);
			break;
		case muls32:
			mulcode(&addr->s32, it->repeatcount, it->value.s32);
			break;
		case divu8:
			divcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case divu16:
			divcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case divu32:
			divcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case divfloat:
			divcode(&addr->f, it->repeatcount, it->value.f);
			break;
		case divs8:
			divcode(&addr->s8, it->repeatcount, it->value.s8);
			break;
		case divs16:
			divcode(&addr->s16, it->repeatcount, it->value.s16);
			break;
		case divs32:
			divcode(&addr->s32, it->repeatcount, it->value.s32);
			break;
		case modu8:
			modcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case modu16:
			modcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case modu32:
			modcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case mods8:
			modcode(&addr->s8, it->repeatcount, it->value.s8);
			break;
		case mods16:
			modcode(&addr->s16, it->repeatcount, it->value.s16);
			break;
		case mods32:
			modcode(&addr->s32, it->repeatcount, it->value.s32);
			break;
		case shl8:
			shlcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case shl16:
			shlcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case shl32:
			shlcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case shru8:
			shrcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case shru16:
			shrcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case shru32:
			shrcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case shrs8:
			shrcode(&addr->s8, it->repeatcount, it->value.s8);
			break;
		case shrs16:
			shrcode(&addr->s16, it->repeatcount, it->value.s16);
			break;
		case shrs32:
			shrcode(&addr->s32, it->repeatcount, it->value.s32);
			break;
		case rol8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u8, (uint8_t)(_rotl8(addr->u8, it->value.u8)));
				addr_add(1);
			}
			break;
		case rol16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u16, (uint16_t)(_rotl16(addr->u16, it->value.u8)));
				addr_add(2);
			}
			break;
		case rol32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u32, (uint32_t)(_rotl(addr->u32, it->value.s32)));
				addr_add(4);
			}
			break;
		case ror8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u8, (uint8_t)(_rotr8(addr->u8, it->value.u8)));
				addr_add(1);
			}
			break;
		case ror16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u16, (uint16_t)(_rotr16(addr->u16, it->value.u8)));
				addr_add(2);
			}
			break;
		case ror32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u32, (uint32_t)(_rotr(addr->u32, it->value.s32)));
				addr_add(4);
			}
			break;
		case and8:
			andcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case and16:
			andcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case and32:
			andcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case or8:
			orcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case or16:
			orcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case or32:
			orcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case xor8:
			xorcode(&addr->u8, it->repeatcount, it->value.u8);
			break;
		case xor16:
			xorcode(&addr->u16, it->repeatcount, it->value.u16);
			break;
		case xor32:
			xorcode(&addr->u32, it->repeatcount, it->value.u32);
			break;
		case writenop:
			while (1);
			//WriteData(&addr->u8, 0x90u, it->value.u32);
			break;
		case writeoff:
			while (1);
			//WriteData(&addr->u32, it->repeatcount, it->value.u32 + offset);
			break;
		case ifeq8:
			ifcode(8, == );
			break;
		case ifeq16:
			ifcode(16, == );
			break;
		case ifeq32:
			ifcode(32, == );
			break;
		case ifeqfloat:
			ifcodef(== );
			break;
		case ifne8:
			ifcode(8, != );
			break;
		case ifne16:
			ifcode(16, != );
			break;
		case ifne32:
			ifcode(32, != );
			break;
		case ifnefloat:
			ifcodef(!= );
			break;
		case ifltu8:
			ifcode(8, < );
			break;
		case ifltu16:
			ifcode(16, < );
			break;
		case ifltu32:
			ifcode(32, < );
			break;
		case ifltfloat:
			ifcodef(< );
			break;
		case iflts8:
			ifcodes(8, < );
			break;
		case iflts16:
			ifcodes(16, < );
			break;
		case iflts32:
			ifcodes(32, < );
			break;
		case ifltequ8:
			ifcode(8, <= );
			break;
		case ifltequ16:
			ifcode(16, <= );
			break;
		case ifltequ32:
			ifcode(32, <= );
			break;
		case iflteqfloat:
			ifcodef(<= );
			break;
		case iflteqs8:
			ifcodes(8, <= );
			break;
		case iflteqs16:
			ifcodes(16, <= );
			break;
		case iflteqs32:
			ifcodes(32, <= );
			break;
		case ifgtu8:
			ifcode(8, > );
			break;
		case ifgtu16:
			ifcode(16, > );
			break;
		case ifgtu32:
			ifcode(32, > );
			break;
		case ifgtfloat:
			ifcodef(> );
			break;
		case ifgts8:
			ifcodes(8, > );
			break;
		case ifgts16:
			ifcodes(16, > );
			break;
		case ifgts32:
			ifcodes(32, > );
			break;
		case ifgtequ8:
			ifcode(8, >= );
			break;
		case ifgtequ16:
			ifcode(16, >= );
			break;
		case ifgtequ32:
			ifcode(32, >= );
			break;
		case ifgteqfloat:
			ifcodef(>= );
			break;
		case ifgteqs8:
			ifcodes(8, >= );
			break;
		case ifgteqs16:
			ifcodes(16, >= );
			break;
		case ifgteqs32:
			ifcodes(32, >= );
			break;
		case ifmask8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u8 & it->value.u8) == it->value.u8;
				addr_add(1);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case ifmask16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u16 & it->value.u16) == it->value.u16;
				addr_add(2);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case ifmask32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u32 & it->value.u32) == it->value.u32;
				addr_add(4);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case ifkbkey:
			if (GetAsyncKeyState(it->value.s32))
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case readreg8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				regs[it->value.u8 + i].u8 = addr->u8;
				addr_add(1);
			}
			break;
		case readreg16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				regs[it->value.u8 + i].u16 = addr->u16;
				addr_add(2);
			}
			break;
		case readreg32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				regs[it->value.u8 + i].u32 = addr->u32;
				addr_add(4);
			}
			break;
		case writereg8:
			writecode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case writereg16:
			writecode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case writereg32:
			writecode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case addreg8:
			addcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case addreg16:
			addcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case addreg32:
			addcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case addregfloat:
			addcode(&addr->f, it->repeatcount, regs[it->value.u8].f);
			break;
		case subreg8:
			subcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case subreg16:
			subcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case subreg32:
			subcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case subregfloat:
			subcode(&addr->f, it->repeatcount, regs[it->value.u8].f);
			break;
		case mulregu8:
			mulcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case mulregu16:
			mulcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case mulregu32:
			mulcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case mulregfloat:
			mulcode(&addr->f, it->repeatcount, regs[it->value.u8].f);
			break;
		case mulregs8:
			mulcode(&addr->s8, it->repeatcount, regs[it->value.u8].s8);
			break;
		case mulregs16:
			mulcode(&addr->s16, it->repeatcount, regs[it->value.u8].s16);
			break;
		case mulregs32:
			mulcode(&addr->s32, it->repeatcount, regs[it->value.u8].s32);
			break;
		case divregu8:
			divcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case divregu16:
			divcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case divregu32:
			divcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case divregfloat:
			divcode(&addr->f, it->repeatcount, regs[it->value.u8].f);
			break;
		case divregs8:
			divcode(&addr->s8, it->repeatcount, regs[it->value.u8].s8);
			break;
		case divregs16:
			divcode(&addr->s16, it->repeatcount, regs[it->value.u8].s16);
			break;
		case divregs32:
			divcode(&addr->s32, it->repeatcount, regs[it->value.u8].s32);
			break;
		case modregu8:
			modcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case modregu16:
			modcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case modregu32:
			modcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case modregs8:
			modcode(&addr->s8, it->repeatcount, regs[it->value.u8].s8);
			break;
		case modregs16:
			modcode(&addr->s16, it->repeatcount, regs[it->value.u8].s16);
			break;
		case modregs32:
			modcode(&addr->s32, it->repeatcount, regs[it->value.u8].s32);
			break;
		case shlreg8:
			shlcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case shlreg16:
			shlcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case shlreg32:
			shlcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case shrregu8:
			shrcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case shrregu16:
			shrcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case shrregu32:
			shrcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case shrregs8:
			shrcode(&addr->s8, it->repeatcount, regs[it->value.u8].s8);
			break;
		case shrregs16:
			shrcode(&addr->s16, it->repeatcount, regs[it->value.u8].s16);
			break;
		case shrregs32:
			shrcode(&addr->s32, it->repeatcount, regs[it->value.u8].s32);
			break;
		case rolreg8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u8, (uint8_t)(_rotl8(addr->u8, regs[it->value.u8].u8)));
				addr_add(1);
			}
			break;
		case rolreg16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u16, (uint16_t)(_rotl16(addr->u16, regs[it->value.u8].u8)));
				addr_add(2);
			}
			break;
		case rolreg32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u32, (uint32_t)(_rotl(addr->u32, regs[it->value.u8].s32)));
				addr_add(4);
			}
			break;
		case rorreg8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u8, (uint8_t)(_rotr8(addr->u8, regs[it->value.u8].u8)));
				addr_add(1);
			}
			break;
		case rorreg16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u16, (uint16_t)(_rotr16(addr->u16, regs[it->value.u8].u8)));
				addr_add(2);
			}
			break;
		case rorreg32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				WriteData(&addr->u32, (uint32_t)(_rotr(addr->u32, regs[it->value.u8].s32)));
				addr_add(4);
			}
			break;
		case andreg8:
			andcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case andreg16:
			andcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case andreg32:
			andcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case orreg8:
			orcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case orreg16:
			orcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case orreg32:
			orcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case xorreg8:
			xorcode(&addr->u8, it->repeatcount, regs[it->value.u8].u8);
			break;
		case xorreg16:
			xorcode(&addr->u16, it->repeatcount, regs[it->value.u8].u16);
			break;
		case xorreg32:
			xorcode(&addr->u32, it->repeatcount, regs[it->value.u8].u32);
			break;
		case writenopreg:
			while (1);
			//WriteData(&addr->u8, 0x90u, regs[it->value.u8].u32);
			break;
		case ifeqreg8:
			ifcodereg(8, == );
			break;
		case ifeqreg16:
			ifcodereg(16, == );
			break;
		case ifeqreg32:
			ifcodereg(32, == );
			break;
		case ifeqregfloat:
			ifcoderegf(== );
			break;
		case ifnereg8:
			ifcodereg(8, != );
			break;
		case ifnereg16:
			ifcodereg(16, != );
			break;
		case ifnereg32:
			ifcodereg(32, != );
			break;
		case ifneregfloat:
			ifcoderegf(!= );
			break;
		case ifltregu8:
			ifcodereg(8, < );
			break;
		case ifltregu16:
			ifcodereg(16, < );
			break;
		case ifltregu32:
			ifcodereg(32, < );
			break;
		case ifltregfloat:
			ifcoderegf(< );
			break;
		case ifltregs8:
			ifcoderegs(8, < );
			break;
		case ifltregs16:
			ifcoderegs(16, < );
			break;
		case ifltregs32:
			ifcoderegs(32, < );
			break;
		case iflteqregu8:
			ifcodereg(8, <= );
			break;
		case iflteqregu16:
			ifcodereg(16, <= );
			break;
		case iflteqregu32:
			ifcodereg(32, <= );
			break;
		case iflteqregfloat:
			ifcoderegf(<= );
			break;
		case iflteqregs8:
			ifcoderegs(8, <= );
			break;
		case iflteqregs16:
			ifcoderegs(16, <= );
			break;
		case iflteqregs32:
			ifcoderegs(32, <= );
			break;
		case ifgtregu8:
			ifcodereg(8, > );
			break;
		case ifgtregu16:
			ifcodereg(16, > );
			break;
		case ifgtregu32:
			ifcodereg(32, > );
			break;
		case ifgtregfloat:
			ifcoderegf(> );
			break;
		case ifgtregs8:
			ifcoderegs(8, > );
			break;
		case ifgtregs16:
			ifcoderegs(16, > );
			break;
		case ifgtregs32:
			ifcoderegs(32, > );
			break;
		case ifgteqregu8:
			ifcodereg(8, >= );
			break;
		case ifgteqregu16:
			ifcodereg(16, >= );
			break;
		case ifgteqregu32:
			ifcodereg(32, >= );
			break;
		case ifgteqregfloat:
			ifcoderegf(>= );
			break;
		case ifgteqregs8:
			ifcoderegs(8, >= );
			break;
		case ifgteqregs16:
			ifcoderegs(16, >= );
			break;
		case ifgteqregs32:
			ifcoderegs(32, >= );
			break;
		case ifmaskreg8:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u8 & it->value.u8) == regs[it->value.u8].u8;
				addr_add(1);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case ifmaskreg16:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u16 & it->value.u16) == regs[it->value.u8].u16;
				addr_add(2);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case ifmaskreg32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				cond &= (addr->u32 & it->value.u32) == regs[it->value.u8].u32;
				addr_add(4);
			}
			if (cond)
			{
				regnum = processCodeList_int(it->trueCodes, regnum);
			}
			else
			{
				regnum = processCodeList_int(it->falseCodes, regnum);
			}
			break;
		case s8tos32:
			addr->s32 = addr->s8;
			break;
		case s16tos32:
			addr->s32 = addr->s16;
			break;
		case s32tofloat:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				addr->f = (float)addr->s32++;
				addr_add(4);
			}
			break;
		case u32tofloat:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				addr->f = (float)addr->u32++;
				addr_add(4);
			}
			break;
		case floattos32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				addr->s32 = _round(addr->f++);
				addr_add(4);
			}
			break;
		case floattou32:
			for (uint32_t i = 0; i < it->repeatcount; i++)
			{
				addr->u32 = _round(addr->f++);
				addr_add(4);
			}
			break;
		default:
			// Invalid opcode.
			// TODO: Show an error message.
			break;
		}
	}

	return regnum;
}

/**
 * Process the code list.
 */
void CodeParser::processCodeList()
{
	processCodeList_int(m_codes, -1);
}

/**
* Sets a base offset for all codes.
* Call this before loading the code list.
*/
void CodeParser::setOffset(ptrdiff_t offset)
{
	this->offset = offset;
}

/**
 * Read codes from a code file.
 * This will clear all loaded codes before loading new codes.
 * @param filename Code file.
 * @return Number of codes read on success; -ENOENT if stream is closed; -EINVAL if file is invalid.
 */
int CodeParser::readCodes(const string& filename)
{
	//clear();
	ifstream stream(filename, ifstream::binary);
	if (!stream.is_open())
		return -ENOENT;

	return readCodes(stream);
}


/**
 * Read codes from a code file.
 * @param stream Code file.
 * @return Number of codes read on success; -ENOENT if stream is closed; -EINVAL if file is invalid.
 */
int CodeParser::readCodes(istream& stream)
{
	// Clear the codes first.
	//clear();

	// Check for the magic number.
	static const char codemagic[6] = { 'c', 'o', 'd', 'e', 'v', '5' };
	char buf[sizeof(codemagic)];
	stream.read(buf, sizeof(buf));
	if (memcmp(buf, codemagic, sizeof(codemagic)) != 0)
		return -EINVAL;

	// Get the number of codes defined in the file header.
	// This might not be equal to the number of codes in the file.
	int codecount_header;
	stream.read((char*)&codecount_header, sizeof(codecount_header));

	// Read the codes.
	// TODO: Check for codeeof?
	readCodes_int(stream, m_codes);

	// Return the number of codes read.
	return (int)m_codes.size();
}

/**
 * Read codes from a code file.
 * Internal recursive function; used for internal code lists.
 * @param stream	[in] Code file.
 * @param clist		[out] Code list.
 * @return 0 when completed; codeeof on EOF; _else or endif while processing.
 */
unsigned char CodeParser::readCodes_int(istream& stream, list<Code>& clist)
{
	while (true)
	{
		uint8_t t = stream.get();
		if (t == codeeof || t == _else || t == endif)
			return t;

		Code code = {};
		if (t == newregs)
		{
			code.newregs = true;
			auto regs = new valuetype[16];
			memset(regs, 0, sizeof(valuetype) * 16);
			m_registers.push_back(regs);
			// FIXME: continue; instead?
			t = stream.get();
		}

		code.type = (CodeType)t;
		uintptr_t addr;
		stream.read((char*)&addr, sizeof(uintptr_t));
		code.pointer = (addr & 0x80000000u) == 0x80000000u;
		addr &= 0x7FFFFFFF;
		if (addr >= 16)
			addr += offset;
		code.address = (void*)addr;
		if (code.pointer)
		{
			code.offsetcount = stream.get();
			code.offsets = new int[code.offsetcount];
			for (int i = 0; i < code.offsetcount; i++)
			{
				stream.read((char*)&code.offsets[i], sizeof(int32_t));
			}
		}

		stream.read((char*)&code.value, sizeof(code.value));
		stream.read((char*)&code.repeatcount, sizeof(uint32_t));
		if ((code.type >= ifeq8 && code.type <= ifkbkey) ||
			(code.type >= ifeqreg8 && code.type <= ifmaskreg32))
		{
			switch (readCodes_int(stream, code.trueCodes))
			{
			case _else:
				if (readCodes_int(stream, code.falseCodes) == codeeof)
				{
					return codeeof;
				}
				break;
			case codeeof:
				return codeeof;
			}
		}

		clist.push_back(code);
	}

	// Finished parsing codes.
	return 0;
}

/**
 * Clear the loaded codes.
 */
void CodeParser::clear()
{
	// m_code doesn't have any allocated memory.
	m_codes.clear();

	// m_registers does have allocated memory.
	for (auto& m_register : m_registers)
	{
		// Allocated using new valuetype[16];
		delete[] m_register;
	}

	m_registers.clear();
}