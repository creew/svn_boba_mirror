#include "..\include\Dir.h"
#include "..\include\Types.h"
#include "..\include\dll.h"
#include "..\Dlls\LibraryDLL\export\LibraryDLL.h"
#include "calls.h"
#include "vars.h"
#include "regfile.h"
#include "helper.h"
#include "dll.h"
#include "elfloader.h"


typedef void (*USERDATACONSTRUCTOR)(void*, void*);

typedef struct
{
	USERDATACONSTRUCTOR constr;
	void * data;
}UDATA;

void * malloc (int size)
{
#ifdef DB2020
	return(memalloc(0,size,1,5,"SwiLib",0));
#elif A2
	return(memalloc(0xFFFFFFFF,size,1,5,"SwiLib",0));
#else
	return(memalloc(size,1,5,"SwiLib",0));
#endif
}

void mfree (void * mem)
{
#ifdef DB2020
	if (mem) memfree(0, mem,"SwiLib",0);
#elif A2
	if (mem) memfree(0, mem,"SwiLib",0);
#else
	memfree(mem,"SwiLib",0);
#endif
}

int CmpProc(UDATA* listitem, USERDATACONSTRUCTOR itemtofind)
{
	return listitem->constr != itemtofind;
}

void* GetUserData (int size, USERDATACONSTRUCTOR constr, void* param)
{

	UDATA * ud;
	int index;

	_printf("GetUserData(0x%X,0x%X,0x%X)",size,constr,param);
	index = LIST_FIND(elfpackdata->UserDataList, constr, CmpProc);
	if (index!=0xFFFF)
	{
		ud=List_Get(elfpackdata->UserDataList,index);
		_printf("UserData  @0x%X",ud->data);
	}
	else
	{
		if (constr)
		{
			ud=malloc(sizeof(UDATA));
			ud->constr=constr;
			ud->data=malloc(size);
			List_InsertFirst(elfpackdata->UserDataList,ud);
			constr(ud->data,param);

			_printf("New UserData  @0x%X",ud->data);
			_printf("Total Used Bloks  0x%%X",epd->UserDataList->FirstFree);
		}
		else
		{
			_printf("Error. GetUserData() without Constructor!!");
			// ��� ������������
			return(0);
		}
	}
	return(ud->data);

};



int RemoveUserData(void (*constr)(void *, void *))
{
	UDATA * ud;
	int index;

	_printf("RemoveUserData(0x%X)",constr);

	index = LIST_FIND(elfpackdata->UserDataList, constr, CmpProc);
	if (index!=0xFFFF)
	{
		ud=List_RemoveAt(elfpackdata->UserDataList,index);
		{
			mfree(ud->data);
			mfree(ud);

			_printf("Removed OK...");

			return(0);
		}
	}
	else
	{
		_printf("0x%X Not in List!!",constr);

		// ��� �������� � ������
		return(1);
	}

}

//============================================================================

wchar_t* GetDir (int DirIndex)
{

	_printf("GetDir(0x%X)",DirIndex);

	switch (DirIndex>>1)
	{
	case  DIR_AUDIO>>1 : return (DirIndex&1)? PATH_AUDIO_INT: PATH_AUDIO_EXT;
	case  DIR_IMAGE>>1 : return (DirIndex&1)? PATH_IMAGE_INT: PATH_IMAGE_EXT;
	case  DIR_VIDEO>>1 : return (DirIndex&1)? PATH_VIDEO_INT: PATH_VIDEO_EXT;
	case  DIR_THEME>>1 : return (DirIndex&1)? PATH_THEME_INT: PATH_THEME_EXT;
	case  DIR_OTHER>>1 : return (DirIndex&1)? PATH_OTHER_INT: PATH_OTHER_EXT;

	case  DIR_ELFS>>1 : return (DirIndex&1)? PATH_ELF_INT: PATH_ELF_EXT;
	case  DIR_ELFS_DAEMONS>>1 : return (DirIndex&1)? PATH_ELF_DAEMONS_INT: PATH_ELF_DAEMONS_EXT;
	case  DIR_ELFS_CONFIG>>1 : return (DirIndex&1)? PATH_ELF_CONFIG_INT: PATH_ELF_CONFIG_EXT;
	case  DIR_INI>>1 : return PATH_INI;
	case  DIR_DLL>>1: return PATH_DLL;

	case  DIR_USER>>1 : return (DirIndex&1)? PATH_USER_INT: PATH_USER_EXT;

	default: return(PATH_DEFAULT);
	}
}

int fopen (const wchar_t* fname, int mode, int rights)
{
	wchar_t* path;
	wchar_t* name;
	int len;
	name=wstrrchr(fname,0x2F);
	if (!name) return -1;
	path=malloc(((len=name-fname)+1)*sizeof(wchar_t));
	memcpy(path,fname,len*sizeof(wchar_t));
	path[len]=0;
	name++;
	len=_fopen(path,name,mode,rights,NULL);
	mfree(path);
	return len;
}


//===============  KBD_HOOK  ================

typedef struct
{
	KEYHOOKPROC proc;
	void *data;
}KEY_HOOK_ELEM;

int KeyHookCmpProc(KEY_HOOK_ELEM* listitem, KEYHOOKPROC itemtofind)
{
	return listitem->proc != itemtofind;
}

int ModifyKeyHook( KEYHOOKPROC proc, int mode , void *data )
{
	int i=0;

	_printf("ModifyKeyHook PROC@0x%x, mode=0x%x",proc,mode)  ;

	i = LIST_FIND(elfpackdata->gKbdHookList, proc, KeyHookCmpProc);
	switch (mode)
	{
	case 0: //remove
		if (i!=0xFFFF)
		{
			mfree(List_Get(elfpackdata->gKbdHookList,i));
			List_RemoveAt(elfpackdata->gKbdHookList,i);

			_printf("PROC(0x%x) Removed..",proc)  ;

			return(0);
		}
		else
		{
			_printf("PROC(0x%x) Not in List !!!",proc)  ;

			return(-2);
		}
	case 1: //add
		if (i==0xFFFF)
		{
			KEY_HOOK_ELEM *elem = malloc(sizeof(KEY_HOOK_ELEM));
			elem->proc = proc;
			elem->data = data;
			List_InsertFirst(elfpackdata->gKbdHookList,(void*)elem);

			_printf("PROC(0x%x) Added in List",proc)  ;

			return(0);
		}
		else
		{
			_printf("Warning! PROC(0x%x) was in List",proc)  ;

			return(-3);
		}
	default:
		{
			_printf("Error. Wrong mode(0x%x). Use 0/1 for remove/add",proc)  ;


			return(-4);
		}
	}
}

int KbdHook(int r0,int r1,int r2)
{
	int i=0;
	int result;
	while(i < elfpackdata->gKbdHookList->FirstFree)
	{
		KEY_HOOK_ELEM *elem = (KEY_HOOK_ELEM *)List_Get(elfpackdata->gKbdHookList,i++);
		result = elem->proc(r0,r1,r2,elem->data);
		if (result==-1) return(0);
	}
	return(0);
}
//===============  OSE_HOOK  ================
int OSEHookCmpProc(OSE_HOOK_ITEM* ud, void (*e2)(void*))
{
	return ud->HOOK != e2;
}

int ModifyOSEHook(int event , void (*proc)(void*),int mode)
{
	int n = LIST_FIND(elfpackdata->OseHookList, proc, OSEHookCmpProc);

	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(elfpackdata->OseHookList, n));
			return(0);
		}
		else
		{
			return(-2);
		}

	case 1:
		if (n==0xFFFF)
		{
			OSE_HOOK_ITEM *item=malloc(sizeof(OSE_HOOK_ITEM));
			item->signo=event;
			item->HOOK = proc;
			List_InsertFirst(elfpackdata->OseHookList,(void*)item);
			return(0);
		}
		else
		{
			return(-3);
		}
	default: return(-4);
	}
}

//===============  UI_HOOK  ================
__thumb UI_MESSAGE * PageAction_Hook(UI_MESSAGE * ui)
{
	int f=0;
	int n;

	if (elfpackdata)
	{
		LIST * UI_Hook_List = elfpackdata->UIHookList;
		_printf("PageAction_Hook msg:0x%X",ui->event)  ;
		n=UI_Hook_List->FirstFree;

		if (n)
		{
			n--;
			UI_HOOK_ITEM ** uihook = (UI_HOOK_ITEM**)(UI_Hook_List->listdata);
			do
			{
				{
					UI_HOOK_ITEM * item=uihook[n];
					//          _printf("PageAction 0x%x 0x%x",item->event,uiev)  ;
					{
						if (item->event==ui->event) f|=item->HOOK(ui);
					}
				}
			}
			while(n--);
		}
	}
	if (f) ui->mode=0xFF;
	return(ui);
}


int UIHookCmpProc(UI_HOOK_ITEM* listitem, int (*itemtofind)(UI_MESSAGE*))
{
	return listitem->HOOK != itemtofind;
}

int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode)
{
	LIST * UI_Hook_List = elfpackdata->UIHookList;

	int n = LIST_FIND(UI_Hook_List, PROC, UIHookCmpProc);

	_printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;

	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(UI_Hook_List,n));

			_printf("PROC@0x%X, Removed OK..",PROC)  ;

			return(0);
		}
		else
		{
			return(-2);
		}

	case 1:
		if (n==0xFFFF)
		{
			UI_HOOK_ITEM *item=malloc(sizeof(UI_HOOK_ITEM));
			item->event=event;
			item->HOOK=PROC;
			List_InsertFirst(UI_Hook_List,(void*)item);

			_printf("PROC@0x%X, Added OK..",PROC)  ;
			_printf("Total HookItems : 0x%X",UI_Hook_List->FirstFree)  ;

			return(0);
		}
		else
		{
			return(-3);
		}
	default: return(-4);
	}
}
// ===============================================================

typedef struct {
	int (*PROC)(void *msg, BOOK *bk);
	int event;
	int book_id;
} PAGE_HOOK_ELEM;

__thumb void PageAction_Hook1(LIST *lst)
{
	if (elfpackdata)
	{
		LIST *UIPageHook = elfpackdata->UIPageHook;
		int n_pages=UIPageHook->FirstFree;
		while(n_pages--)
		{
			PAGE_HOOK_ELEM *my_act=(PAGE_HOOK_ELEM *)List_Get(UIPageHook,n_pages);
			int f=0;
			int n=lst->FirstFree;
			while(n--)
			{
				ACTION *act=(ACTION *)List_Get(lst,n);
				if (act->event==my_act->event)
				{
					f=1;
					break;
				}
			}
			if (f)
			{
				ACTION *new_act = ActionCreate(my_act->PROC, my_act->book_id, my_act->event, NULL, NULL);
				List_InsertFirst(lst, new_act);
			}
		}
	}
}


int UIHook1CmpProc(void * e1, void * e2)
{
	ACTION * ud = (ACTION*)e1;
	PAGE_HOOK_ELEM *page_elem=(PAGE_HOOK_ELEM *)e2;
	if (ud->PROC==page_elem->PROC && ud->event==page_elem->event) return (0); else  return(1);
}

int ModifyUIHook1(int event , int (*PROC)(void *msg, BOOK *bk), int book_id ,int mode)
{
	LIST * UIPageHook = elfpackdata->UIPageHook;
	PAGE_HOOK_ELEM page_elem;
	page_elem.PROC=PROC;
	page_elem.event=event;
	page_elem.book_id=book_id;
	int n = List_Find(UIPageHook,&page_elem,UIHook1CmpProc);
	_printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;

	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(UIPageHook,n));
			return(0);
		}
		else
		{
			return(-2);
		}

	case 1:
		if (n==0xFFFF)
		{
			ACTION *item=malloc(sizeof(PAGE_HOOK_ELEM));
			memcpy(item,&page_elem,sizeof(PAGE_HOOK_ELEM));
			List_InsertLast(UIPageHook,item);
			return(0);
		}
		else
		{
			return(-3);
		}
	default: return(-4);
	}
}


extern void* Library;

void CreateLists(void)
{
	elfpackdata = malloc(sizeof(EP_DATA));
	memset(elfpackdata,0,sizeof(EP_DATA));

	_printf("EP_DATA @%x",epd)  ;

	elfpackdata->UserDataList=List_Create();
	elfpackdata->gKbdHookList=List_Create();
	elfpackdata->UIHookList=List_Create();
	elfpackdata->OseHookList=List_Create();
	elfpackdata->DLLList=List_Create();
	elfpackdata->UIPageHook=List_Create();
	elfpackdata->LibraryCache = NULL;

	_printf("   epd->UserDataList @%x",elfpackdata->UserDataList)  ;
	_printf("   epd->gKbdHookList @%x",elfpackdata->gKbdHookList)  ;
	_printf("   epd->UIHookList @%x",elfpackdata->UIHookList)  ;
	_printf("   epd->OseHookList @%x",elfpackdata->OseHookList)  ;
	//  _printf("   epd->elflist @%x",elfpackdata->elflist)  ;

	elfpackdata->DBExtList=CreateDBExtList();
	elfpackdata->CreateDbExt = CreateDbExt;
	elfpackdata->IconSmall = NOIMAGE;
	elfpackdata->IconBig = NOIMAGE;
	elfpackdata->LibraryDLL = NULL;

	set_envp(get_bid(current_process()),"elfpackdata",(OSADDRESS)elfpackdata);

	ELFExtrRegister(elfpackdata);
}


void Init()
{
	//CreateLists();

	FILELISTITEM* mem = malloc(sizeof(FILELISTITEM));

	FILELISTITEM* fli;

	_printf("     Entered Init...")  ;

	// ������ ���������� �����
	ModifyKeyHook((KEYHOOKPROC)pKBD, 1, NULL);
	pKBD=KbdHook;

	// ��������� �������

	_printf("     StartHelper....")  ;

	StartHelper();

	_printf("     StartHelper OK. PID=%x",epd->HPID)  ;

	// ������ DLL ����������

	_printf("     Load LibraryDLL....")  ;

	elfpackdata->LibraryDLL = LoadDLL(L"LibraryDLL.dll");
	if ((INVALID(elfpackdata->LibraryDLL)))
	{
		_printf("     Load LibraryDLL Error")  ;
		elfpackdata->LibraryDLL = 0;
	}
	else _printf("     Load LibraryDLL OK")  ;

	// ������ ��� ����

	if (elfpackdata->LibraryDLL)
	{
		_printf("     Patching LibraryCache....")  ;

		void** lib = malloc(0x4000);
		memcpy(lib, &Library, 0x4000);

		const LIBRARY_DLL_FUNCTIONINFO* fns = ( (const LIBRARY_DLL_DATA*)elfpackdata->LibraryDLL )->functions;
		while (fns->functionnum)
		{
			lib[ fns->functionnum - 0x100 ] = fns->functionptr;
			fns++;
		}

		elfpackdata->LibraryCache = lib;

		_printf("     Patching LibraryCache OK")  ;
	}

	// ��������� �������

	_printf("     StartDaemons....")  ;
	_printf("     ------Begin List-------")  ;

#ifdef DAEMONS_INTERNAL
	DIR_HANDLE* handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_INTERNAL));
#else
	DIR_HANDLE* handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_EXTERNAL));
#endif

	do
	{
		if(fli=GetFname(handle,mem))
		{
			wchar_t* filename = malloc((wstrlen(fli->path)+wstrlen(fli->fname)+2)*2);
			wstrcpy(filename,fli->path);
			wstrcat(filename,L"/");
			wstrcat(filename,fli->fname);

			_printf("     ->...")  ;
#ifdef DBG
			_printf("     elfload Result=0x%X",elfload(filename,0,0,0));
#else
			elfload(filename,0,0,0);
#endif
			mfree(filename);
		}
	}
	while(fli);

	_printf("     ------End List-------")  ;


	if (handle) DestroyDirHandle(handle);
	mfree(mem);

	_printf("     Exit Init")  ;
}

__root  const int PATCH_AUTO_RUN @ "PATCH_AUTO_RUN" =(int)PAGE_ENTER_EVENT;
__root  const int PATCH_AUTO_RUN1 @ "PATCH_AUTO_RUN1" =(int)Init;

int GetExtTable()
{
	return((int)elfpackdata->DBExtList->listdata);
}
