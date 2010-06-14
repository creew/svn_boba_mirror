#ifndef _CL_H_
#define _CL_H_

#include "..\\include\lib_clara.h"
//#include "..\\include\types.h"

//class CDispObjBase;
class CGuiBase;
class CBookBase;
//class CGCBase;
//class CBook;
//template class CDispObjT;
//template class CGuiT;
//class CGUIMessage;
//class CStdGui;
//class CGuiListMenu;
//class CGuiOneOfMany;
//class CGuiNOfMany;
//class CGuiTabMenuBar;
//class CPaintGC;
//class CMemoryGC;

//template class auto_ptr;




class CDispObjBase : public DISP_OBJ
{
public:
	CGuiBase* GetGUI();                   //DispObject_GetGUI
	char IsVisible();                     //DispObject_GetVisible
	char* GetName ();                     //DispObject_GetName
	DISP_DESC* GetDESC();                 //DispObject_GetDESC
	int GetAbsoluteXPos();                //DispObject_GetAbsoluteXPos
	int GetAbsoluteYPos();                //DispObject_GetAbsoluteYPos
	int GetWindowHeight();                //DispObject_GetWindowHeight
	int GetWindowWidth();                 //DispObject_GetWindowWidth
	void InvalidateRect(RECT* rect);      //DispObject_InvalidateRect
	void KillRefreshTimer();              //DispObject_KillRefreshTimer
	void SetLayerColor(int color);        //DispObject_SetLayerColor
	void SetRefreshTimer(int time);       //DispObject_SetRefreshTimer
	void SetTitleText(int StrID);         //DispObject_SetTitleText
	void Show(int mode);                  //DispObject_Show
	WINDOW* GetWindow();                  //DispObject_GetWindow
	void GetRect( RECT* retrect );	      //DispObject_GetRect

	/*
        LIST* DispObject_SoftKeys_GetList( DISP_OBJ*, BOOK* book, char __zero );
        u16 DISP_DESC_GetSize( DISP_OBJ* );
        void DispObject_SetBackgroundImage( DISP_OBJ*, wchar_t imageID );
        void DispObject_SetCursorImage( DISP_OBJ*, wchar_t imageID );
        void DispObject_SetListTextColor( DISP_OBJ*, int unk1, int list_color, int unk2, int unk3, int list_select_color, int unk4, int _zerro );
        void DispObject_SetScrollBarBackgroundColor(void *disp_obj,int color);
        void DispObject_SetScrollBarColor(void *disp_obj,int color);
        void DispObject_SetSoftkeysColor(void *disp_obj,int color);
        void DispObject_SetTitleImage( DISP_OBJ*, wchar_t imageID );
        void DispObject_SetTitleTextColor( DISP_OBJ*, int color );
        void DispObject_SoftKeys_RestoreDefaultAction( DISP_OBJ*, int action );
        void* DispObject_SoftKeys_GetParams( DISP_OBJ* );

	DISP_OBJ_METHOD DispObject_GetMethod04( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod05( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod06( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod07( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod0A( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod0B( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetMethod0C( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetOnConfig( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetOnLayout( DISP_OBJ* );
	DISP_OBJ_METHOD DispObject_GetonRefresh( DISP_OBJ* );
	DISP_OBJ_ONCLOSE_METHOD DispObject_GetOnClose( DISP_OBJ* );
	DISP_OBJ_ONCREATE_METHOD DispObject_GetOnCreate( DISP_OBJ* );
	DISP_OBJ_ONKEY_METHOD DispObject_GetOnKey( DISP_OBJ* );
	DISP_OBJ_ONREDRAW_METHOD DispObject_GetOnRedraw( DISP_OBJ* );

	void Feedback_SetManualScrollingText( DISP_OBJ* feedback_disp_obj, int );
	void MediaPlayer_ShowNowPlaying( DISP_OBJ* , int );
	void SoftKeys_GetLabel( DISP_OBJ* softkeys, SKLABEL* lbl, int id );
	void SoftKeys_Update( DISP_OBJ* softkeys );
	void StringInput_DispObject_SetText( DISP_OBJ*, STRID );
	*/
};

class CGuiBase: public GUI
{
public:
        CBookBase* GetBook();                         //GUIObject_GetBook
        CDispObjBase* GetDISPObj();                   //GUIObject_GetDispObject
        CGuiBase* Free();                             //GUIObject_Destroy
        void SetBackgroundImage(wchar_t imageID);     //GUIObject_SetBackgroundImage
        void SetBacklightTimeout( int time );	      //GUIObject_SetBacklightTimeout
        void SetCursorImage(wchar_t imageID);         //GUIObject_SetCursorImage
        void SetFocus (int i);                        //GUIObject_SetFocus
        void SetStyle(int style);                     //GUIObject_SetStyle
        void SetTitleIcon(wchar_t imageID);           //GUIObject_SetTitleIcon
        void SetTitleImage(wchar_t imageID);          //GUIObject_SetTitleBackgroundImage
        void SetTitleText(STRID strid);                 //GUIObject_SetTitleText
        void SetTitleType(int type);                  //GUIObject_SetTitleType
        void Show();                                  //GUIObject_Show
        void SoftKeys_AddErrorStr( u16 actionID, STRID );//GUIObject_SoftKeys_AddErrorStr
        void SoftKeys_ExecuteAction( u16 actionID );   //GUIObject_SoftKeys_ExecuteAction
        void SoftKeys_Hide();                         //GUIObject_SoftKeys_Hide
        void SoftKeys_RemoveBackground();             //GUIObject_SoftKeys_RemoveBackground
        void SoftKeys_RemoveItem( u16 actionID );//GUIObject_SoftKeys_RemoveItem
        void SoftKeys_RestoreBackground();            //GUIObject_SoftKeys_RestoreBackground
        void SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ) ); //GUIObject_SoftKeys_SetAction
        void SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ), STRID short_text, STRID full_text );
        void SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ), STRID strid );
        void SoftKeys_SetEnable( u16 actionID, BOOL enabled );//GUIObject_SoftKeys_SetEnable
        void SoftKeys_SetInfoText( u16 actionID, STRID );//GUIObject_SoftKeys_SetInfoText
        void SoftKeys_SetItemAsSubItem( u16 actionID, int subitem );//GUIObject_SoftKeys_SetItemAsSubItem
        void SoftKeys_SetText( u16 actionID, STRID );  //GUIObject_SoftKeys_SetText
        void SoftKeys_SetTexts( u16 actionID, STRID short_text, STRID full_text );//GUIObject_SoftKeys_SetTexts
        void SoftKeys_SetVisible( u16 actionID, BOOL visible );//GUIObject_SoftKeys_SetVisible
        void SoftKeys_Show();                         //GUIObject_SoftKeys_Show
        void SoftKeys_SuppressDefaultAction( u16 actionID );//GUIObject_SoftKeys_SuppressDefaultAction
	/*
	void GUIObject_SetListTextColor( GUI*, int unk1, int list_color, int unk2, int unk3, int list_select_color, int unk4, int _zerro );
	void GUIObject_SetTitleBackgroundImage( GUI*, wchar_t imageID );
	void GUIObject_SetTitleTextColor( GUI*, int color );

	void GUIInput_SetIcon( GUI* , wchar_t icon );

	int PercentInput_GetPercent( GUI* PercentInput );

	int StringInput_GetStringAndLen( GUI*, wchar_t**, u16* );
	void StringInput_MenuItem_SetPriority( GUI* strinp, int prio, int actionID );
	void StringInput_SetCursorPosition( GUI*, u16 curs_pos, char unk );

	void ProgressBar_SetBarDisabled( GUI*, BOOL disabled );
	void ProgressBar_SetIcon( GUI*, u16 icon_id );
	void ProgressBar_SetPercentValue( GUI*, int value );
	void ProgressBar_SetText( GUI*, STRID text );

	void StatusIndication_SetItemText( GUI* , int item, STRID );

	void TabMenuBar_SetTabGui( GUI_TABMENUBAR*, int tab, GUI* );

	void Video_SetSkin( GUI* VideoPlayerGUI, int skinID );

	char* MainInput_getPNUM( GUI* );
	int MainInput_getCurPos( GUI* );
	int MainInput_getVisible( GUI* );
	int MainInput_isPlus( GUI* );
	int MainInput_strlen( GUI* );
	void MainInput_Hide( GUI* );

	void YesNoQuestion_SetIcon(GUI*, wchar_t iconId);
	*/
};

class CBookBase : public BOOK
{
public:
	int GetBookID();                              //BookObj_GetBookID
	int GetDisplayOrientation();                  //BookObj_GetDisplayOrientation
	int GetSessionID();                           //BookObj_GetSessionID
	UI_APP_SESSION* GetSession();                 //BookObj_GetSession
	void AddGUIObject(CGuiBase* gui);             //BookObj_AddGUIObject
	void CallPage(const PAGE_DESC* page);         //BookObj_CallPage
	void Free();                                  //FreeBook
	void GotoPage(const PAGE_DESC* page);         //BookObj_GotoPage
	void Hide( int display );                     //BookObj_Hide
	void ReturnPage(int event);                   //BookObj_ReturnPage
	void SetDisplayOrientation(int orientation);  //BookObj_SetDisplayOrientation
	void Show( int display );                     //BookObj_Show
	void UI_Event(int event);                     //UI_Event_toBookID
	void UI_Event_wData(int event,void *message,void (*mfree)(void*));    //UI_Event_toBookIDwData
	/*
	void BookObj_CallSubroutine( BOOK* book, BOOK_SUBROUTINE* subr );
	void BookObj_SetFocus( BOOK* book, int display );
	void BookObj_SoftKeys_SetAction( BOOK* book, int actionID, void (*proc)( BOOK*, CGuiBase* ) );
	void BookObj_SoftKeys_SetText( BOOK* book, int actionID, STRID );

	void* IncommingCall_Accept( BOOK* book );
	void* IncommingCall_Mute( BOOK* book );
	void* IncommingCall_Reject( BOOK* book );

	wchar_t* MenuBook_Desktop_GetSelectedItemID( BOOK* MenuBook_Desktop );

	int SoundRecorder_RecordCall( BOOK* OngoingCallBook );

	void Audio_Pause( BOOK* );
	void Audio_Play( BOOK* );

	int Video_GetCurrentSkinID( BOOK* VideoPlayerBook );
	int Video_Refresh( BOOK* VideoPlayerBook, GUI* VideoPlayerGUI );
	int Video_SetPermit( BOOK* VideoPlayerBook, u16 NewWidth, u16 NewHeight );
	void VideoPlayerControl( BOOK* VideoPlayerBook, int );
	void VideoResize_AllScreenMode( BOOK* VideoPlayerBook, u16 VideoWidth, u16 VideoHeight, u16* NewWidth, u16* NewHeight );
	void VideoResize_AutomateMode( BOOK* VideoPlayerBook, u16 VideoWidth, u16 VideoHeight, u16* NewWidth, u16* NewHeight );
	void Video_ActionBack( BOOK* VideoPlayerBook, void* );
	void Video_ExtractFrame( BOOK* VideoPlayerBook, void* );
	void Video_Pause( BOOK* VideoPlayerBook, void* );
	void Video_Play( BOOK* VideoPlayerBook, void* );
	void Video_Stop( BOOK* VideoPlayerBook, void* );
	void Video_ZoomOn( BOOK* VideoPlayerBook, void* );

	void PlayerControl( BOOK* AudioPlayerBook, int );

	void SwitchRadioStationFromList( BOOK* FmRadioBook, int );

	SUB_EXECUTE* BrowserItem_Get_SUB_EXECUTE( BOOK* BrowserItemBook );

	GUI* SBY_GetMainInput( BOOK* StandBy ); // !!!!! �� ��������� !!!!!!
	GUI* SBY_GetStatusIndication( BOOK* StandBy ); // !!!!! �� ��������� !!!!!!
	*/
};

/////////////////////////////////////////////////////////////////////////

class CGCBase
{
protected:
	GC* gc;
	int gc_xx_old;
	CGCBase();
	~CGCBase();
public:
	int DrawRawBitmap(int x1, int y1, int x2, int y2, int * bmp); //GC_WritePixels
	int GetBrushColor();                  //GC_GetBrushColor
	int GetPenColor();                    //GC_GetPenColor
	int GetXX();                          //GC_GetXX
	void DrawFRect (int color,int x1,int y1,int x2 ,int y2);      //GC_DrawFRect
	void DrawLine(int x1, int y1, int x2, int y2);                //GC_DrawLine
	void DrawRoundRect(RECT *,int arcWidth,int arcHeight,int border_flag,int fill_flag);  //GC_DrawRoundRect
	void GetRect(RECT*);                  //GC_GetRect
	void SetBrushColor(int brush_color);  //GC_SetBrushColor
	void SetPenColor(int pen_color);      //GC_SetPenColor
	void SetPixel (int x1,int y1,int color);      //GC_SetPixel
	void SetXX(int);                      //GC_SetXX
	void ValidateRect(RECT*);             //GC_ValidateRect
	//void GC_PutChar( void *xx , int x, int y, int _zero, int zero1 , short wchar);
};


/////////////////////////////////////////////////////////////////////////

class CBook : public CBookBase
{
	static void __onClose(CBook* bk);
protected:
	virtual ~CBook();//�� ������� ������ ��������
public:
	CBook(char* pBookName, const PAGE_DESC * bp);
};

/////////////////////////////////////////////////////////////////////////

template <class T>
class CDispObjT : public CDispObjBase
{
private:
	CDispObjT();
	~CDispObjT();
	static void __onKey(T* db, int key,int a3,int repeat,int type)
	{
		db->onKey(key,a3,repeat,type );
	}
	static void __onDraw(T* db, int a,int b,int c)
	{
		db->onDraw( a, b, c );
	}
	static int __onCreate(T* db)
	{
		return db->onCreate();
	}
	static void __onDestroy(T* db)
	{
		db->onDestroy();
	}
	static void __onRefresh(T* db)
	{
		db->onRefresh();
	}
	static void __onLayout(T* db)
	{
		db->onLayout();
	}
public:
	static void DispDescCreate(DISP_DESC *desc)
	{
		DISP_DESC_SetSize(desc,sizeof(T));
		DISP_DESC_SetName(desc, T::getName());
		DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)__onCreate);
		DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)__onDestroy);
		DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)__onDraw);
		DISP_DESC_SetOnKey(desc, (DISP_OBJ_ONKEY_METHOD)__onKey );
		DISP_DESC_SetOnRefresh(desc,(DISP_OBJ_METHOD)__onRefresh);
		DISP_DESC_SetOnLayout(desc,(DISP_OBJ_METHOD)__onLayout);
	}

	static char* getName();
	int onCreate()
	{
		return 1;
	}
	void onKey(int key,int,int repeat,int type){}
	void onDraw(int a,int b,int c){}
	void onDestroy(){}
	void onRefresh(){}
	void onLayout(){}
};

/////////////////////////////////////////////////////////////////////////

template<class T>
class CGuiT: public CGuiBase
{
	explicit CGuiT();
	static void __onclose( CGuiT<T>* gui )
	{
		gui-> ~CGuiT();
	}
protected:
	virtual ~CGuiT()
	{
	}
public:
	CGuiT(CBook* book, int display)
	{
		GUIObject_Create(this, (void(*)(GUI*))__onclose, (void(*)(DISP_DESC*))T::DispDescCreate, book,
						 0, display, sizeof(CGuiBase) );

		if(book)
			book->AddGUIObject(this);
	}
};

/////////////////////////////////////////////////////////////////////////

class CGUIMessage : public GUI_MESSAGE
{
public:
	CBook* GetBook();//
	CGuiBase* GetGui();//GUIonMessage_GetGui
	int GetMsg();//GUIonMessage_GetMsg
	int GetCreatedItem();//GUIonMessage_GetCreatedItemIndex
	int GetSelectedItem();//GUIonMessage_GetSelectedItem
	int GetPrevSelectedItem();//GUIonMessage_GetPrevSelectedItem
	char SetItemText( STRID strid );
	char SetItemSecondLineText( STRID strid );
	char SetItemInfoText( STRID strid );
	char SetItemUnavailableText( STRID strid );
	void SubItem_SetText( STRID strid );//GUIonMessage_SubItem_SetText
	int SubItem_GetCreatedParentIndex();//GUIonMessage_SubItem_GetCreatedParentIndex
	int SubItem_GetCreatedIndex();//GUIonMessage_SubItem_GetCreatedIndex
	int SubItem_GetSelectedParentIndex();//GUIonMessage_SubItem_GetSelectedParentIndex
	void SetItemIcon( int align, wchar_t iconID );
	void SetNumberOfSubItems( int unk, int n_sub_items );//GUIonMessage_SetNumberOfSubItems
	void SetItemDisabled( BOOL disabled );
	void SetLineSeparator( int _unk1 );
	wchar_t* GetSearchString();
	int GetSearchStringLength();
};

/////////////////////////////////////////////////////////////////////////

class CStdGui : public CGuiBase
{
	void (*oldonclose)(GUI*);
	explicit CStdGui();
	static void __onclose( CStdGui* disp );
protected:
	virtual ~CStdGui();
public:
	CStdGui(CBook* book, int display, void(*onguidestroy)(GUI*), void(*ondesccreate)(DISP_DESC*),
			void (*ondispmessage)( DISP_OBJ*, void*, GUI* ) );
};

/////////////////////////////////////////////////////////////////////////

class CGuiListMenu : public CStdGui
{
protected:
	static int __onmessage( CGUIMessage* msg );
public:
	CGuiListMenu( CBook* book, int display=0 );
	virtual int OnMessage( CGUIMessage* msg );

	void SetItemCount( int count );
	int GetSelectedItem();
	void SetFocused( int item );
	void SetNoItemText( STRID );
	void DestroyItems();
//	void ListMenu_SetHotkeyMode(GUI*, LISTMENU_HOTKEY_MODE mode);
//	void ListMenu_SetSearchLanguageMode( GUI_LIST*, int mode );
//	void ListMenu_EnableSearchHeader( GUI_LIST*, BOOL );
};

/////////////////////////////////////////////////////////////////////////

class CGuiOneOfMany : public CStdGui
{
protected:
	static int __onmessage( CGUIMessage* msg );
public:
	CGuiOneOfMany( CBook* book, int display=0 );
	virtual int OnMessage( CGUIMessage* msg );

	void SetItemCount( int count );
	int GetSelectedItem();
	void SetFocused( int item );
	void SetChecked( int checked );
	void SetTexts( STRID* strids, int count );
};

/////////////////////////////////////////////////////////////////////////

class CGuiNOfMany : public CStdGui
{
protected:
	static int __onmessage( CGUIMessage* msg );
public:
	CGuiNOfMany( CBook* book, int display=0 );
	virtual int OnMessage( CGUIMessage* msg );

	void SetItemCount( int count );
	int GetChecked( u16* buffer );
	int GetCheckedCount();
	void SetFocused( int item );
	void SetChecked( u16* checked_table, int items_count );
	void SetTexts( STRID* strids, int count );
};

/////////////////////////////////////////////////////////////////////////

class CGuiTabMenuBar : public CStdGui
{
public:
	CGuiTabMenuBar( CBook* book, int display=0 );
	int GetFocusedTab();
	void SetTabCount( int count );
	void SetFocusedTab( int tab );
	void SetTabGui( int tab, CGuiBase* gui );
	void SetTabIcon( int tab, int icon, int state );
	void SetTabTitle( int tab, STRID strid );
};

/////////////////////////////////////////////////////////////////////////

class CPaintGC : public CGCBase
{
public:
	CPaintGC();
	~CPaintGC();
};

class CMemoryGC : public CGCBase
{
public:
	CMemoryGC( int xsize, int ysize, int bpp );
	~CMemoryGC();
};

/////////////////////////////////////////////////////////////////////////

#define __UNIQUENAME2(x,y) x##y
#define __UNIQUENAME1(x,y) __UNIQUENAME2(x,y)
#define __UNIQUENAME __UNIQUENAME1(__uniquename,__LINE__)

#define DECLARE_PAGE_DESC_BEGIN(objname, name)         \
	extern const PAGE_MSG __UNIQUENAME[];                \
		const PAGE_DESC objname = {name, 0,  __UNIQUENAME};  \
			static const PAGE_MSG __UNIQUENAME[]@ "DYN_PAGE" ={

#define DECLARE_PAGE_DESC_END NIL_EVENT_TAG,0 };

#define DECLARE_PAGE_DESC_MSG(evt,func) evt, (int(*)(void*,BOOK*))func,

/////////////////////////////////////////////////////////////////////////

// simplified STL realisation
template <class X>
class auto_ptr {
	X* m_ptr;
public:

	explicit auto_ptr(X* p =0) : m_ptr(p)
	{
	}

	auto_ptr(auto_ptr& copy) : m_ptr( copy.release() )
	{
	}

	auto_ptr& operator=(auto_ptr& copy)
	{
		reset(copy.release());
		return *this;
	}

	~auto_ptr()
	{
		delete m_ptr;
	}

	X* get() const
	{
		return m_ptr;
	}

	X* release()
	{
		X* retvalue = m_ptr;
		m_ptr = NULL;
		return retvalue;
	}

	void reset(X* p =0)
	{
		if(m_ptr !=p)
			delete m_ptr;
		m_ptr = p;
	}

	X& operator*() const
	{
		return *get();
	}

	X* operator->() const
	{
		return get();
	}

	/*
	template <class Y> auto_ptr(auto_ptr<Y>&) throw();
	template <class Y> auto_ptr& operator=(auto_ptr<Y>&) throw();
	template <class Y> operator auto_ptr_ref<Y>() throw();
	template <class Y> operator auto_ptr<Y>() throw();
	auto_ptr& operator=(auto_ptr_ref<X> r) throw();
	auto_ptr(auto_ptr_ref<X>) throw();
	*/
};

/////////////////////////////////////////////////////////////////////////

#endif
