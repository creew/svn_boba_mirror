#include "..\deleaker\mem2.h"
#include "..\include\Lib_Clara.h"

#include "cl.h"

////////////////////////////////////////////////////////////////////////

void CDispObjBase::InvalidateRect(RECT* rect)
{
	::DispObject_InvalidateRect(this, rect);
}

char* CDispObjBase::GetName()
{
	return DispObject_GetName(this);
}

char CDispObjBase::IsVisible()
{
	return DispObject_GetVisible(this);
}

DISP_DESC* CDispObjBase::GetDESC()
{
	return DispObject_GetDESC(this);
}

WINDOW* CDispObjBase::GetWindow()
{
	return DispObject_GetWindow(this);
}

int CDispObjBase::GetAbsoluteXPos()
{
	return DispObject_GetAbsoluteXPos(this);
}

int CDispObjBase::GetAbsoluteYPos()
{
	return DispObject_GetAbsoluteYPos(this);
}

int CDispObjBase::GetWindowHeight()
{
	return DispObject_GetWindowHeight(this);
}

int CDispObjBase::GetWindowWidth()
{
	return DispObject_GetWindowWidth(this);
}

void CDispObjBase::KillRefreshTimer()
{
	DispObject_KillRefreshTimer(this);
}

void CDispObjBase::SetRefreshTimer(int time)
{
	DispObject_SetRefreshTimer(this,time);
}

void CDispObjBase::SetLayerColor(int color)
{
	DispObject_SetLayerColor(this, color);
}

CGuiBase* CDispObjBase::GetGUI()
{
	return (CGuiBase*)DispObject_GetGUI(this);
}

void CDispObjBase::Show(int mode)
{
	DispObject_Show(this, mode);
}

void CDispObjBase::SetTitleText(STRID strid)
{
	DispObject_SetTitleText(this, strid);
}
////////////////////////////////////////////////////////////////////////

CBookBase* CGuiBase::GetBook()
{
	return (CBookBase*) ::GUIObject_GetBook(this);
}

CDispObjBase* CGuiBase::GetDISPObj()
{
	return (CDispObjBase*)::GUIObject_GetDispObject(this);
}

CGuiBase* CGuiBase::Free()
{
	return (CGuiBase*)::GUIObject_Destroy(this);
}

void CGuiBase::SoftKeys_Hide()
{
	::GUIObject_SoftKeys_Hide(this);
}

void CGuiBase::SetFocus(int i)
{
	::GUIObject_SetFocus(this, i);
}

void CGuiBase::SetTitleText(int StrID)
{
	::GUIObject_SetTitleText(this, StrID);
}

void CGuiBase::SetStyle(int style)
{
	::GUIObject_SetStyle(this, style);
}

void CGuiBase::SetTitleType(int type)
{
	::GUIObject_SetTitleType(this, type);
}

void CGuiBase::Show()
{
	::GUIObject_Show(this);
}

void CGuiBase::SoftKeys_Show()
{
	::GUIObject_SoftKeys_Show(this);
}

void CGuiBase::SetBackgroundImage(wchar_t imageID)
{
	::GUIObject_SetBackgroundImage(this, imageID);
}

void CGuiBase::SetCursorImage(wchar_t imageID)
{
	::GUIObject_SetCursorImage(this, imageID);
}

void CGuiBase::SetTitleIcon(wchar_t imageID)
{
	::GUIObject_SetTitleIcon(this, imageID);
}

void CGuiBase::SetTitleImage(wchar_t imageID)
{
	::GUIObject_SetTitleBackgroundImage(this, imageID);
}

void CGuiBase::SoftKeys_AddErrorStr( u16 actionID, STRID strid)
{
	::GUIObject_SoftKeys_AddErrorStr( this, actionID, strid);
}

void CGuiBase::SoftKeys_ExecuteAction( u16 actionID )
{
	::GUIObject_SoftKeys_ExecuteAction( this, actionID );
}

void CGuiBase::SoftKeys_RemoveItem(u16 actionID )
{
	::GUIObject_SoftKeys_RemoveItem( this, actionID );
}

void CGuiBase::SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ) )
{
	::GUIObject_SoftKeys_SetAction( this, actionID, (void(*)(BOOK*,GUI*))proc );
}

void CGuiBase::SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ), STRID strid )
{
	::GUIObject_SoftKeys_SetAction( this, actionID, (void(*)(BOOK*,GUI*))proc );
	::GUIObject_SoftKeys_SetText( this, actionID, strid );
}

void CGuiBase::SoftKeys_SetAction( u16 actionID, void (*proc)( CBookBase*, CGuiBase* ), STRID short_text, STRID full_text )
{
	::GUIObject_SoftKeys_SetAction( this, actionID, (void(*)(BOOK*,GUI*))proc );
	::GUIObject_SoftKeys_SetTexts( this, actionID, short_text, full_text );
}

void CGuiBase::SoftKeys_SetEnable(u16 actionID, int ena )
{
	::GUIObject_SoftKeys_SetEnable( this, actionID, ena );
}

void CGuiBase::SoftKeys_SetInfoText( u16 actionID, STRID strid)
{
	::GUIObject_SoftKeys_SetInfoText( this, actionID, strid );
}

void CGuiBase::SoftKeys_SetItemAsSubItem( u16 actionID, int subitem )
{
	::GUIObject_SoftKeys_SetItemAsSubItem( this, actionID, subitem );
}

void CGuiBase::SoftKeys_SetText(u16 actionID, STRID strid )
{
	::GUIObject_SoftKeys_SetText( this, actionID, strid );
}

void CGuiBase::SoftKeys_SetTexts( u16 actionID, STRID short_text, STRID full_text )
{
	::GUIObject_SoftKeys_SetTexts( this, actionID, short_text, full_text );
}

void CGuiBase::SoftKeys_SetVisible(u16 actionID, int visible )
{
	::GUIObject_SoftKeys_SetVisible( this, actionID, visible );
}

void CGuiBase::SoftKeys_SuppressDefaultAction( u16 actionID )
{
	::GUIObject_SoftKeys_SuppressDefaultAction( this, actionID );
}

void CGuiBase::SoftKeys_RemoveBackground()
{
	::GUIObject_SoftKeys_RemoveBackground( this );
}

void CGuiBase::SoftKeys_RestoreBackground()
{
	::GUIObject_SoftKeys_RestoreBackground( this );
}

void CGuiBase::SetBacklightTimeout( int time )
{
	::GUIObject_SetBacklightTimeout( this, time );
}

////////////////////////////////////////////////////////////////////////

void CBookBase::AddGUIObject(CGuiBase* gui)
{
	BookObj_AddGUIObject( this, gui );
}

void CBookBase::Free()
{
	FreeBook(this);
}

void CBookBase::Hide(int display_type)
{
	BookObj_Hide(this, display_type);
}

void CBookBase::Show(int display_type)
{
	BookObj_Show(this, display_type);
}

int CBookBase::GetBookID()
{
	return BookObj_GetBookID(this);
}

UI_APP_SESSION* CBookBase::GetSession()
{
	return BookObj_GetSession(this);
}

int CBookBase::GetSessionID()
{
	return BookObj_GetSessionID(this);
}

int CBookBase::GetDisplayOrientation()
{
	return BookObj_GetDisplayOrientation(this);
}

void CBookBase::SetDisplayOrientation(int orientation)
{
	BookObj_SetDisplayOrientation(this, orientation);
}

void CBookBase::UI_Event(int event)
{
	UI_Event_toBookID(event,GetBookID());
}
void CBookBase::UI_Event_wData(int event,void *message,void (*mfree)(void*))
{
	UI_Event_toBookIDwData(event,GetBookID(),message,mfree);
}

void CBookBase::GotoPage(const PAGE_DESC  * page)
{
	BookObj_GotoPage(this, page);
}

void CBookBase::ReturnPage(int event)
{
	BookObj_ReturnPage(this, event);
}

void CBookBase::CallPage(const PAGE_DESC  * page)
{
	BookObj_CallPage(this, page);
}

////////////////////////////////////////////////////////////////////////

CBook::CBook(char* pBookName, const PAGE_DESC * bp)
{
	CreateBook( this, (void(*)(BOOK*)) & __onClose, bp, pBookName, -1, 0);
}

CBook::~CBook()
{
}

void CBook::__onClose(CBook* bk)
{
	bk->~CBook();
}

/////////////////////////////////////////////////////////////////////////

CGuiBase* CGUIMessage::GetGui()
{
	return (CGuiBase*)GUIonMessage_GetGui( this );
}

CBook* CGUIMessage::GetBook()
{
	return (CBook*)GUIonMessage_GetBook( this );
}

int CGUIMessage::GetMsg()
{
	return GUIonMessage_GetMsg( this );
}

int CGUIMessage::GetCreatedItem()
{
	return GUIonMessage_GetCreatedItemIndex( this );
}

int CGUIMessage::GetSelectedItem()
{
	return GUIonMessage_GetSelectedItem( this );
}

int CGUIMessage::GetPrevSelectedItem()
{
	return GUIonMessage_GetPrevSelectedItem( this );
}

char CGUIMessage::SetItemText( STRID strid )
{
	return GUIonMessage_SetMenuItemText( this, strid );
}

char CGUIMessage::SetItemSecondLineText( STRID strid )
{
	return GUIonMessage_SetMenuItemSecondLineText( this, strid );
}

char CGUIMessage::SetItemInfoText( STRID strid )
{
	return GUIonMessage_SetMenuItemInfoText( this, strid );
}

char CGUIMessage::SetItemUnavailableText( STRID strid )
{
	return GUIonMessage_SetMenuItemUnavailableText( this, strid );
}

void CGUIMessage::SetSubitemText( STRID strid )
{
	GUIonMessage_SetSubitemText( this, strid );
}

int CGUIMessage::GetCreatedSubitemParent()
{
	return GUIonMessage_GetCreatedSubItemParentIndex( this );
}

int CGUIMessage::GetCurrentSubitem()
{
	return GUIonMessage_GetCurrentSubItem( this );
}

int CGUIMessage::GetCurrentItem()
{
	return GUIonMessage_GetCurrentItemIndex( this );
}

void CGUIMessage::SetItemIcon( int align, wchar_t iconID )
{
	GUIonMessage_SetMenuItemIcon( this, align, iconID );
}

void CGUIMessage::SetItemAsSubitem( int unk, int n_sub_items )
{
	GUIonMessage_SetItemAsSubitem( this, unk, n_sub_items );
}

void CGUIMessage::SetItemDisabled( int Disabled )
{
	GUIonMessage_SetItemDisabled( this, Disabled );
}

void CGUIMessage::SetLineSeparator( int _unk1 )
{
	GUIonMessage_SetLineSeparator( this, _unk1 );
}

////////////////////////////////////////////////////////////////////////

CStdGui::CStdGui(CBook* book, int display, void(*onguidestroy)(GUI*), void(*ondesccreate)(DISP_DESC*),
				 void (*ondispmessage)( DISP_OBJ*, void*, GUI* ) )
{
	GUIObject_Create(this, (void(*)(GUI*))__onclose, ondesccreate, book,
					 ondispmessage, display, sizeof(CGuiBase) );
	oldonclose = onguidestroy;
	if(book)
		book->AddGUIObject(this);
}

void CStdGui::__onclose( CStdGui* gui )
{
	gui->~CStdGui();
	gui->oldonclose( gui );
}

CStdGui::~CStdGui()
{
}


////////////////////////////////////////////////////////////////////////

int CGuiListMenu::OnMessage( CGUIMessage* msg )
{
	return 0;
}

int CGuiListMenu::__onmessage( CGUIMessage* msg )
{
	return ( static_cast<CGuiListMenu*>( msg->GetGui() ) ) -> OnMessage( msg );
}

CGuiListMenu::CGuiListMenu( CBook* book, int display )
: CStdGui( book, display, getListMenuOnGuiDestroy(), getListMenuOnDescCreate(), getListMenuOnDispMessage() )
{
	ListMenu_SetOnMessage( this, (int(*)(GUI_MESSAGE*))__onmessage );
}

void CGuiListMenu::SetItemCount( int count )
{
	ListMenu_SetItemCount( this, count );
}

int CGuiListMenu::GetSelectedItem()
{
	return ListMenu_GetSelectedItem( this );
}

void CGuiListMenu::SetFocused( int item )
{
	ListMenu_SetCursorToItem( this, item );
}

void CGuiListMenu::SetNoItemText( STRID strid )
{
	ListMenu_SetNoItemText( this, strid );
}

void CGuiListMenu::DestroyItems()
{
	ListMenu_DestroyItems( this );
}

////////////////////////////////////////////////////////////////////////

int CGuiOneOfMany::OnMessage( CGUIMessage* msg )
{
	return 0;
}

int CGuiOneOfMany::__onmessage( CGUIMessage* msg )
{
	return ( static_cast<CGuiOneOfMany*>( msg->GetGui() ) ) -> OnMessage( msg );
}

CGuiOneOfMany::CGuiOneOfMany( CBook* book, int display )
: CStdGui( book, display, getOneOfManyOnGuiDestroy(), getOneOfManyOnDescCreate(), getOneOfManyOnDispMessage() )
{
	OneOfMany_SetOnMessage( this, (int(*)(GUI_MESSAGE*))__onmessage );
}

void CGuiOneOfMany::SetItemCount( int count )
{
	OneOfMany_SetItemCount( this, count );
}

int CGuiOneOfMany::GetSelectedItem()
{
	return OneOfMany_GetSelected( this );
}

void CGuiOneOfMany::SetFocused( int item )
{
	OneOfMany_SetFocused( this, item );
}

void CGuiOneOfMany::SetChecked( int checked )
{
	OneOfMany_SetChecked( this, checked );
}

void CGuiOneOfMany::SetTexts( STRID* strids, int count )
{
	OneOfMany_SetTexts( this, strids, count );
}

////////////////////////////////////////////////////////////////////////

int CGuiNOfMany::OnMessage( CGUIMessage* msg )
{
	return 0;
}

int CGuiNOfMany::__onmessage( CGUIMessage* msg )
{
	return ( static_cast<CGuiNOfMany*>( msg->GetGui() ) ) -> OnMessage( msg );
}

CGuiNOfMany::CGuiNOfMany( CBook* book, int display )
: CStdGui( book, display, getNOfManyOnGuiDestroy(), getNOfManyOnDescCreate(), getNOfManyOnDispMessage() )
{
	NOfMany_SetOnMessage( this, (int(*)(GUI_MESSAGE*))__onmessage );
}

void CGuiNOfMany::SetItemCount( int count )
{
	ListMenu_SetItemCount( this, count );
}

int CGuiNOfMany::GetChecked( u16* buffer )
{
	return NOfMany_GetChecked( this, buffer );
}

int CGuiNOfMany::GetCheckedCount()
{
	return NOfMany_GetCheckedCount( this );
}

void CGuiNOfMany::SetFocused( int item )
{
	NOfMany_SetCursor( this, item );
}

void CGuiNOfMany::SetChecked( u16* checked_table, int items_count )
{
	NOfMany_SetChecked( this, checked_table, items_count );
}

void CGuiNOfMany::SetTexts( STRID* strids, int count )
{
	NOfMany_SetTexts( this, strids, count );
}

////////////////////////////////////////////////////////////////////////

CGuiTabMenuBar::CGuiTabMenuBar( CBook* book, int display )
: CStdGui( book, display, getTabMenuBarOnGuiDestroy(), getTabMenuBarOnDescCreate(), getTabMenuBarOnDispMessage() )
{
}

int CGuiTabMenuBar::GetFocusedTab()
{
	return TabMenuBar_GetFocusedTab( this );
}

void CGuiTabMenuBar::SetTabCount( int count )
{
	TabMenuBar_SetTabCount( this, count );
}

void CGuiTabMenuBar::SetFocusedTab( int tab )
{
	TabMenuBar_SetFocusedTab( this, tab );
}

void CGuiTabMenuBar::SetTabGui( int tab, CGuiBase* gui )
{
	TabMenuBar_SetTabGui( this, tab, gui );
}

void CGuiTabMenuBar::SetTabIcon( int tab, int icon, int state )
{
	TabMenuBar_SetTabIcon( this, tab, icon, state );
}

void CGuiTabMenuBar::SetTabTitle( int tab, STRID strid )
{
	TabMenuBar_SetTabTitle( this, tab, strid );
}

////////////////////////////////////////////////////////////////////////

CPaintGC::CPaintGC()
{
	gc= get_DisplayGC();
	gc_xx_old = GC_GetXX(gc);
	GC_SetXX(gc, 1);
}

CPaintGC::~CPaintGC()
{
	GC_SetXX(gc, gc_xx_old);
}

int CPaintGC::DrawRawBitmap(int x1, int y1, int x2, int y2, int * bmp)
{
	return ::GC_WritePixels(gc, x1, y1, x2, y2, bmp);
}

int CPaintGC::GetBrushColor()
{
	return GC_GetBrushColor(gc);
}

int CPaintGC::GetPenColor()
{
	return GC_GetPenColor(gc);
}

int CPaintGC::GetXX()
{
	return GC_GetXX(gc);
}

void CPaintGC::SetXX(int arg)
{
	GC_SetXX(gc, arg);
}

void CPaintGC::DrawFRect(int color,int x1,int y1,int x2 ,int y2)
{
	GC_DrawFRect(gc, color, x1, y1, x2, y2);
}

void CPaintGC::DrawLine(int x1, int y1, int x2, int y2)
{
	GC_DrawLine(gc, x1,  y1, x2, y2);
}

void CPaintGC::DrawRoundRect(RECT* rect,int arcWidth,int arcHeight,int border_flag,int fill_flag)
{
	GC_DrawRoundRect(gc, rect, arcWidth, arcHeight, border_flag, fill_flag);
}

void CPaintGC::SetBrushColor(int brush_color)
{
	GC_SetBrushColor(gc, brush_color);
}

void CPaintGC::SetPenColor(int pen_color)
{
	GC_SetPenColor(gc, pen_color);
}

void CPaintGC::SetPixel(int x1,int y1,int color)
{
	GC_SetPixel(gc, x1, y1, color);
}

void CPaintGC::ValidateRect(RECT* rect)
{
	GC_ValidateRect(gc, rect);
}

void CPaintGC::GetRect(RECT* rect)
{
	GC_GetRect(gc,rect);
}

/////////////////////////////////////////////////////////////////////////