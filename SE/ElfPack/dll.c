#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\dll.h"
#include "vars.h"

extern EP_DATA * getepd(void);
extern long elfload(const unsigned short * filename, void *param1, void *param2, void *param3);


int isInDllList(void * p0,void * p1)
{
	DLL_LIST_ELEMENT *p = (DLL_LIST_ELEMENT*)p0;
	if (!wstrcmp(p->name,p1)) return(0);
	return(1);
}

// ���������� ����� this ��� errorcode
void * LoadDLL(wchar_t * DllName)
{
	__get_epd;
	
	DLL_LIST_ELEMENT * dll;
	DLL_DATA * dll_public_data;
	
	int index = List_Find(epd->DLLList,DllName,isInDllList);
	if (index==LIST_ERROR)
	{
		//���� �� ��������� DLL��
		wchar_t * dllfullname = malloc( (wstrlen( GetDir(DIR_DLL | MEM_INTERNAL))*2)+ sizeof(L"/")+ (wstrlen(DllName)*2)+2);
		DLL_LIST_ELEMENT * newdll = malloc(sizeof(DLL_LIST_ELEMENT));
		wchar_t * name = malloc((wstrlen(DllName)*2)+2);
		// ������� ����
		wstrcpy(dllfullname,GetDir(DIR_DLL | MEM_INTERNAL));
		if (fstat(dllfullname,DllName,0))  return(DLL_ERROR_FILE_NOT_FOUND);
		wstrcat(dllfullname,L"/");
		wstrcat(dllfullname,DllName);
		//������� ����� ���
		wstrcpy(name,DllName);
		newdll->name=name;
		
		newdll->EntryPoint = (int (*)(int,DLL_DATA*)) elfload(dllfullname,0,0,0);
		mfree(dllfullname);
		
		// ���� �� �������� EntryPoint
		if ((int)newdll->EntryPoint<=0)
		{
			mfree(newdll->name);
			mfree(newdll);
			return(DLL_ERROR_FILE_NOT_LOADED);
		}
		IMB();
		// ���� �� ������������ - �����
		if (newdll->EntryPoint(DLL_INIT,0))
		{
			mfree(newdll->name);
			mfree(newdll);
			return(DLL_ERROR_INIT);
		}
		
		// ���� DLL �� ������ ����� this
		if (!(dll_public_data=(void*)newdll->EntryPoint(DLL_LOAD,0)))
		{
			mfree(newdll->name);
			mfree(newdll);
			return(DLL_ERROR_LOAD);
		}
		
		// �������� � ������ ����������� DLL
		List_InsertLast(epd->DLLList,newdll);
		// ��� DLL ������ � this
		dll_public_data->name = DllName;
		// ������� ����� this
		return(dll_public_data);
	}
	else
	{
		// ���� ����� DLL ����������, �� ����� �� ������
		dll = List_Get(epd->DLLList,index);
	}
	
	// ���� DLL �� ������ ����� this
	if (!(dll_public_data=(void*)dll->EntryPoint(DLL_LOAD,0)))
	{
		return(DLL_ERROR_LOAD);
	}
	
	// ��� DLL ������ � this
	dll_public_data->name = DllName;
	
	// ������� ����� this
	return(dll_public_data);
}


int UnLoadDLL(DLL_DATA * DllData)
{
	__get_epd;
	DLL_LIST_ELEMENT * dll;
	int usage_count;
	
	// ���� ���� this
	if (!DllData)    return(DLL_ERROR_NO_DATA_P);
	
	// ���� � ������ DLL
	int index = List_Find(epd->DLLList,DllData->name,isInDllList);
	if (index==LIST_ERROR) return(DLL_ERROR_DLL_NOT_LOADED);
	
	dll = List_Get(epd->DLLList,index);
	
	// �������� � UnLoader
	usage_count=dll->EntryPoint(DLL_UNLOAD,DllData);
	
	// ���� ������������ ��� ��� ��
	if (usage_count) return(usage_count);
	
	// ����� ������� ���,������� �� ������ � ������� ��� DLL_LIST_ELEMENT
	mfree(dll->name);
	mfree(List_RemoveAt(epd->DLLList,index));
	return(0);
}
