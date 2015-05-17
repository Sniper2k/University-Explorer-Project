// Explorer.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Explorer.h"
#include "SDirectory.h"
#include <locale>
#include <codecvt>
#include <fstream>

#define MAX_LOADSTRING 100

static const DWORD idCreate=10001;
static const DWORD idLeftList=10002;
static const DWORD idRightList=10003;
static const DWORD idDelete=10004;
static const DWORD idCopy=10005;
static const DWORD idCut=10006;
static const DWORD idSearch=10007;

const unsigned searchResultLength= 20;

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна
SDirectory root;
SDirectory* selected;
SDirectory* notSelected;
SDirectory* right;
SDirectory* left;

HWND hWnd;
HWND selectedList;
HWND notSelectedList;
HWND leftList;
HWND rightList;

HWND leftStatic;
HWND rightStatic;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	stringDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	errDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	resultsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void updateList(HWND listbox);
void addStringToList(HWND list, std::string* str, unsigned pos, bool file, SDirectory* dir);
void addStringToSelectedList(std::string* str, unsigned pos, bool file);
void deleteStringFromSelectedList(unsigned pos);

void createErrorDlg(LPCWSTR errmsg);
void createStringDialog(bool add);
void createResultsDialog(List* l);

void navigateTo();
void deleteSelected();
void copySelected();
void cutPasteSelected();

void saveToFile();
void loadFromFile();

bool selectedPosition(unsigned& lpos, LPCWSTR msg);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXPLORER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXPLORER));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXPLORER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   std::string s= "root";
   s.insert(s.end(),1,'\0');
   root = SDirectory(&s);
   selected = &root;
   notSelected = &root;
   right = &root;
   left = &root;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменно

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   leftList = CreateWindow(_T("LISTBOX"),_T(""),WS_CHILD|WS_BORDER|WS_VISIBLE|LBS_NOTIFY|WS_VSCROLL, 50, 50, 300, 400, hWnd,(HMENU)idLeftList,hInstance,NULL);
   rightList = CreateWindow(_T("LISTBOX"),_T(""),WS_CHILD|WS_BORDER|WS_VISIBLE|LBS_NOTIFY|WS_VSCROLL, 350, 50, 300, 400, hWnd,(HMENU)idRightList,hInstance,NULL);

   selectedList = leftList;
   notSelectedList = rightList;

   SetFocus(selectedList);

   leftStatic = CreateWindow(_T("STATIC"),_T("root"),WS_CHILD|WS_BORDER|WS_VISIBLE,50,30,300,20,hWnd,NULL,hInstance,NULL);
   rightStatic = CreateWindow(_T("STATIC"),_T("root"),WS_CHILD|WS_BORDER|WS_VISIBLE,350,30,300,20,hWnd,NULL,hInstance,NULL);

   CreateWindow(_T("BUTTON"),_T("Add"),WS_CHILD|WS_VISIBLE, 800, 50, 80, 30, hWnd,(HMENU)idCreate,hInstance,NULL);
   CreateWindow(_T("BUTTON"),_T("Delete"),WS_CHILD|WS_VISIBLE, 800, 100, 80, 30, hWnd,(HMENU)idDelete,hInstance,NULL);
   CreateWindow(_T("BUTTON"),_T("Copy"),WS_CHILD|WS_VISIBLE, 800, 150, 80, 30, hWnd,(HMENU)idCopy,hInstance,NULL);
   CreateWindow(_T("BUTTON"),_T("Cut/Paste"),WS_CHILD|WS_VISIBLE, 800, 200, 80, 30, hWnd,(HMENU)idCut,hInstance,NULL);
   CreateWindow(_T("BUTTON"),_T("Search"),WS_CHILD|WS_VISIBLE, 800, 250, 80, 30, hWnd,(HMENU)idSearch,hInstance,NULL);

   loadFromFile();

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case  WM_CREATE:
		{
			
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:

		if (wmId == idLeftList)
		{
			selectedList = leftList;
			notSelectedList = rightList;
			selected = left;
			notSelected = right;
		}
		else if (wmId == idRightList)
		{
			selectedList = rightList;
			notSelectedList = leftList;
			selected = right;
			notSelected = left;
		}

		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case idCreate:
			createStringDialog(true);
			break;
		case idLeftList:
		case idRightList:
			SendMessage(notSelectedList,LB_SETCURSEL,(WPARAM)-1,NULL);
			switch(wmEvent)
			{
			case LBN_DBLCLK:
					navigateTo();
				break;
			default:
					break;
			}
			break;
		case idDelete:
			deleteSelected();
			break;
		case  idCopy:
			copySelected();
			break;
		case idCut:
			cutPasteSelected();
			break;
		case idSearch:
			createStringDialog(false);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		saveToFile();
		root.clear();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK stringDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		{
			int ctrl_id=LOWORD(wParam);
			int cmd=HIWORD(wParam);
			switch(ctrl_id)
			{
			case IDOK:
				{
					bool add = GetWindowLongPtr(hDlg,GWLP_USERDATA);
					HWND editBox= GetDlgItem(hDlg,IDC_EDIT1);

					std::wstring wstr;
					wstr.resize(GetWindowTextLength(editBox)+1);
					GetWindowText(editBox,&wstr[0],wstr.size());
					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					std::string str= converter.to_bytes(wstr);

					HWND checkBox = GetDlgItem(hDlg,IDC_CHECK1);
					bool file = false;
					if (SendMessage(checkBox,BM_GETCHECK,NULL,NULL) == BST_CHECKED)
						file = true;

					if (add)
					{
						unsigned pos = selected->createChild(&str,file);
						if (pos != -1)
						{
							addStringToSelectedList(&str,pos,file);
						}
						else
						{
							createErrorDlg(_T("Already Exists!"));
							return (INT_PTR)FALSE;
						}
					}
					else
					{
						List* l = new List;
						selected->search(&str,file,l);
						if (l->isEmpty())
							createErrorDlg(_T("Nothing found"));
						else
							createResultsDialog(l);
					}
					
				}
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
				break;
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK errDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		{
			int ctrl_id=LOWORD(wParam);
			int cmd=HIWORD(wParam);
			switch(ctrl_id)
			{
			case IDOK:
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
				break;
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK resultsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		{
			int ctrl_id=LOWORD(wParam);
			int cmd=HIWORD(wParam);
			switch(ctrl_id)
			{
			case IDC_LIST1:
				if (cmd == LBN_DBLCLK)
				{
					List* l =(List*) GetWindowLongPtr(hDlg,GWLP_USERDATA);
					HWND list = GetDlgItem(hDlg,IDC_LIST1);
					unsigned i = SendMessage(list,LB_GETCURSEL,NULL,NULL);
					SFile* f = l->valueAtPosition(i);

					SDirectory** dir;
					if (selectedList == rightList)
						dir = &right;
					else
						dir = &left;

					if (f->isFile())
						*dir =(SDirectory*) f->fatherValue();
					else
						*dir = (SDirectory*)f;

					updateList(selectedList);
					delete l;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					
				}
				break;
			case IDCANCEL:
				if (cmd == BN_CLICKED)
				{
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}
				break;
			default:
				break;
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void updateList(HWND listbox)
{
	SDirectory* dir;
	HWND stat;
	if (listbox == rightList)
	{
		dir = right;
		stat = rightStatic;
	}
	else
	{
		stat = leftStatic;
		dir = left;
	}

	SendMessage(listbox, LB_RESETCONTENT, NULL,NULL);

	std::wstring s;
	if  (dir != &root)
	{
		s= _T("<...>");
		SendMessage(listbox, LB_ADDSTRING, NULL, (LPARAM)((LPCTSTR)s.data()));
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	List* l = dir->listOfChilds();

	l->toStart();
	while (l->currentIsValid())
	{
		SFile* p = l->currentValue();
		s = converter.from_bytes(p->stringValue());

		if (!p->isFile())
		{
			s.insert(s.begin(),1,L'<');
			s.insert(s.end()-1,1,L'>');
		}
		SendMessage(listbox, LB_ADDSTRING, NULL, (LPARAM)((LPCTSTR)s.data()));

		l->next();
	}

	s = converter.from_bytes(selected->way(searchResultLength));
	SetWindowText(stat,s.data());
}

void addStringToList(HWND list, std::string* str, unsigned pos, bool file, SDirectory* dir)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstr = converter.from_bytes(*str);

	if (!file)
	{
		wstr.insert(wstr.begin(),1,L'<');
		wstr.insert(wstr.end()-1,1,L'>');
	}

	if (dir != &root)
		pos++;

	SendMessage(list, LB_INSERTSTRING,(WPARAM)pos,(LPARAM)((LPCTSTR)wstr.data()));
}

void addStringToSelectedList(std::string* str, unsigned pos, bool file)
{
	addStringToList(selectedList, str,pos, file,selected);

	if (right == left)
	{
		addStringToList(notSelectedList, str,pos, file,notSelected);
	}
}

void deleteStringFromSelectedList(unsigned pos)
{
	SendMessage(selectedList, LB_DELETESTRING, pos,NULL);

	if (right == left)
		SendMessage(notSelectedList, LB_DELETESTRING, pos,NULL);
}

void createErrorDlg(LPCWSTR errmsg)
{
	HWND hdlg = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG2),hWnd,errDlg);
	HWND stat = GetDlgItem(hdlg,IDC_STATIC);
	SetWindowText(stat,errmsg);
}

void createStringDialog(bool add)
{
	HWND hdlg = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG1),hWnd,stringDlg);
	if (!add)
	{
		SetWindowText(hdlg,_T("Search"));
		HWND button = GetDlgItem(hdlg,IDOK);
		SetWindowText(button,_T("Search"));
	}
	
	SetWindowLongPtr(hdlg,GWLP_USERDATA,(LONG) add);
}

void createResultsDialog(List* l)
{
	HWND hdlg = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG3),hWnd,resultsDlg);
	SetWindowLongPtr(hdlg,GWLP_USERDATA,(LONG)l);
	
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	HWND list = GetDlgItem(hdlg,IDC_LIST1);
	l->toStart();
	while(l->currentIsValid())
	{
		SFile* f = l->currentValue();
		std::wstring s = converter.from_bytes(f->way(searchResultLength).data());
		SendMessage(list,LB_ADDSTRING,NULL,(LPARAM)((LPCTSTR)s.data()));
		l->next();
	}
}

void navigateTo()
{
	unsigned pos = SendMessage(selectedList, LB_GETCURSEL,NULL,NULL);
	SDirectory* newSelected = NULL;

	if (pos==-1)
		return;

	if (selected != &root)
	{
		if (pos == 0)
			newSelected =(SDirectory*) selected->fatherValue();
		
		pos--;
	}
	
	if (pos<selected->amountOfDirectories())
	{
		newSelected = (SDirectory*) selected->childAtPosition(pos);
	}

	if (newSelected == NULL)
		return;

	if (selectedList == rightList)
		right = newSelected;
	else
		left = newSelected;

	selected = newSelected;
	updateList(selectedList);
}

void deleteSelected()
{
	unsigned pos;
	bool file;

	bool done = selectedPosition(pos,_T("Can't delete this"));
	if (!done)
		return;

	SFile* p = selected->childAtPosition(pos); 
	file = p->isFile();

	bool update = false;
	if (!file)
		update = notSelected->childOf(p);

	selected->deleteChild(pos);
	if (selected != &root)
		pos++;

	deleteStringFromSelectedList(pos);

	if (update)
	{
		if (notSelectedList == rightList)
			right = &root;
		else
			left = &root;

		updateList(notSelectedList);
	}
}

void copySelected()
{
	if (left == right)
		return;

	unsigned pos;
	bool done = selectedPosition(pos,_T("Can't copy this"));
	if (!done)
		return;

	SFile* p = selected->childAtPosition(pos);
	if (notSelected->alreadyExistsInChilds(&(p->stringValue()),p->isFile()))
	{
		createErrorDlg(_T("Already exists!"));
	}
	else
	{
		pos = notSelected->addChild(p);
		addStringToList(notSelectedList,&(p->stringValue()),pos,p->isFile(),notSelected);
	}

}

void cutPasteSelected()
{
	if (left == right)
		return;

	unsigned pos;
	bool done = selectedPosition(pos,_T("Can't copy this"));
	if (!done)
		return;

	SFile* p = selected->childAtPosition(pos);
	if (notSelected->childOf(p))
	{
		createErrorDlg(_T("Can't copy this"));
		return;
	}

	if (notSelected->alreadyExistsInChilds(&(p->stringValue()),p->isFile()))
	{
		createErrorDlg(_T("Already exists!"));
		return;
	}
	
	unsigned position = notSelected->addChild(p);
	addStringToList(notSelectedList,&(p->stringValue()),position,p->isFile(),notSelected);
	
	selected->deleteChild(pos);
	if (selected != &root)
		pos++;

	deleteStringFromSelectedList(pos);
}

bool selectedPosition(unsigned& lpos, LPCWSTR msg)
{
	lpos = SendMessage(selectedList, LB_GETCURSEL,NULL,NULL);

	if (lpos==-1)
	{
		createErrorDlg(msg);
		return false;
	}

	if (selected != &root)
	{
		if (lpos == 0)
		{
			createErrorDlg(msg);
			return false;
		}
		lpos--;
	}

	return true;
}

void saveToFile()
{
	std::fstream file; 
 	file.open("tree.tb",std::ios::out| std::ios::binary);

 	if (!file.is_open())
		return;
 
 	root.save(&file);
 	file.close();
}

void loadFromFile()
{
 	std::fstream file; 
 	file.open("tree.tb",std::ios::in|std::ios::binary);
 
 	if (!file.is_open())
 		return;
 
 	root.load(&file);
 	file.close();
 
 	updateList(rightList);
 	updateList(leftList);
}