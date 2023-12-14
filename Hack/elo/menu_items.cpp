#include "menu_items.hpp"

#define ITEM_DATA_VERSION 2558
#define ITEM_DATA_MAGIC	  0x6589


#include "gui/CGuiBase.hpp"
#include "Shlobj.h"
Items g_items;

#include <pshpack1.h>
struct ItemDataHeader
{
	/*int magic;
	int version;*/
	int itemSize;
};
//struct ItemData
//{
//	hash64_t	hash;
//	int			value;
//};
#include <poppack.h>
//
//#define MY_DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//       extern "C" const GUID DECLSPEC_SELECTANY name \
//                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
//
//MY_DEFINE_KNOWN_FOLDER(XXFOLDERID_ProgramData, 0x62AB5D82, 0xFDC1, 0x4DC3, 0xA9, 0xDD, 0x07, 0x0D, 0x1D, 0x49, 0x5D, 0x97);
//int GetPath(char* totalPath, int profile)
//{
//	PWSTR path = NULL;
//	if (SUCCEEDED(SHGetFolderPathA(XXFOLDERID_ProgramData,
//		KF_FLAG_DEFAULT,
//		NULL,
//		&path)))
//	{
//		char pcname[200];
//		DWORD sz = 200;
//
//		auto cTempPath = riAnsiConvert(path);
//		char file_id[200];
//		if (runtime->getComputerNameA(pcname, &sz))
//		{
//			CORE_LOG_EX("%s", pcname);
//			pcname[sz] = 0;
//			strcpy(file_id, pcname);
//
//			auto hwidlen = riStrLength(g_userData.hdd_serial);
//
//			for (int i = 0; i < hwidlen; i++) {
//				file_id[i] ^= (((i + 1) ^ 0x99) % 0xFF);
//			}
//		}
//		else
//		{
//			strcpy(file_id, "tmpdata");
//		}
//		char _fmt[] = { '%','s','\\','%','x','%','i','\0' };
//
//		runtime->stringCbPrintfA(totalPath, 255, _fmt, cTempPath, hash32(file_id), profile);
//		return file_id[0];
//	}
//	return 0;
//}
//
//void SaveItems(int profile)
//{
//	char cPath[255];
//	int crypto = GetPath(cPath, profile);
//	CORE_LOG_EX("Save to %s", cPath);
//
//	HANDLE file = runtime->createFile((const char*)cPath, (DWORD)FILE_ALL_ACCESS, (DWORD)NULL, (LPSECURITY_ATTRIBUTES)NULL, (DWORD)CREATE_ALWAYS, (DWORD)NULL, (HANDLE)NULL);
//	if (file != INVALID_HANDLE_VALUE)
//	{
//		DWORD written = 0;
//
//		ItemDataHeader header;
//	/*	header.magic = ITEM_DATA_MAGIC;
//		header.version = ITEM_DATA_VERSION;*/
//		header.itemSize = sizeof(Items);
//
//		Items localItems = g_items;
//
//		BYTE gk = 0x44;
//		for (int i = 0; i < sizeof(Items); i++)
//		{
//			*(BYTE*)((uintptr_t)&localItems + i) ^= (BYTE)(crypto + (i ^ gk));
//			gk -= (BYTE)(i ^ 0x22);
//		}
//		runtime->writeFile( file, (LPCVOID)&header, (DWORD)sizeof(header), (LPDWORD)&written, (LPOVERLAPPED)NULL);
//		runtime->writeFile( file, (LPCVOID)&localItems, (DWORD)sizeof(Items), (LPDWORD)&written, (LPOVERLAPPED)NULL);
//		runtime->closeHandle( file);
//	}
//}
//void LoadItems(int profile)
//{
//	char cPath[255];
//	int crypto = GetPath(cPath, profile);
//	CORE_LOG_EX("Load from %s", cPath);
//
//	HANDLE file = runtime->createFile((const char*)cPath, (DWORD)FILE_ALL_ACCESS, (DWORD)NULL, (LPSECURITY_ATTRIBUTES)NULL, (DWORD)OPEN_EXISTING, (DWORD)NULL, (HANDLE)NULL);
//	if (file != INVALID_HANDLE_VALUE)
//	{
//		DWORD written = 0;
//
//		ItemDataHeader header;
//
//		runtime->readFile( file, (LPVOID)&header, (DWORD)sizeof(header), &written, (LPOVERLAPPED)NULL);
//		//if (header.magic == ITEM_DATA_MAGIC && header.version == ITEM_DATA_VERSION)
//		{
//			if (header.itemSize == sizeof(Items))
//			{
//				Items localItems;
//				runtime->readFile( file, (LPVOID)&localItems, (DWORD)sizeof(Items), &written, (LPOVERLAPPED)NULL);
//
//				BYTE gk = 0x44;
//				for (int i = 0; i < sizeof(Items); i++)
//				{
//					*(BYTE*)((uintptr_t)&localItems + i) ^= (BYTE)(crypto + (i ^ gk));
//					gk -= (BYTE)(i ^ 0x22);
//				}
//
//				g_items = localItems;
//			}
//		}
//		runtime->closeHandle( file);
//	}
//}