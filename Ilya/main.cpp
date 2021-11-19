

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <string.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include "save.h"
#include <functional>

using namespace std;



/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK AboutDlgProc    (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */


    LoadLibrary(TEXT("COMCTL32.LIB"));


    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS| CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = "Menu_for";                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument       /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    UpdateWindow(hwnd);

    //InitCommonControls();

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}




/*  This function is called by the Windows function DispatchMessage()  */
long int filesize(FILE*fp){
    // длина файла в байтах
    long int save_pos, size_of_file;
    save_pos = ftell(fp);
    fseek(fp,0,SEEK_END);
    size_of_file = ftell(fp);
    fseek(fp,save_pos,SEEK_SET);
    return(size_of_file);
}
// модель файла
struct TextModel{
    char *namefile = NULL; // название файла
    char *valuefile = NULL; // содержимое файла
    long  sizefile; // длина файла в байтах
    int   line = 0; // количество строк
    int*  size_line_file = NULL; // длины всех строк
    int* number_file = NULL; // кол во строк
    int multiplier; // множитель
};

//модель рабочей области
struct WorkModel{
    int cxChar,cyChar; // размеры символов
    int cxClient, cyClient; // размеры рабочей области
    int iVscrollPos; // позици€ дл€ вывода
    int iVscrollMax; //максимальное значение
    int iHscrollPos; // позици€ на данный момент
    int iHscrollMax; // по горизонтали
    bool layout = true; //включена верстка или нет
    int diff_line = 0; // количество линий с учетом верстки
};

struct SystemModel{
    int x; // ширина экрана
    int y; // длина экрана
    HINSTANCE hInstance; // описатель экземпл€ра программы

};
int PrintString(HDC& hdc,int index,char*str ,int len,WorkModel& workmodel,TextModel& textmodel,bool layer = false){
    int len_new = len;
    tagSIZE size_;
    if(layer == false && len > 0){
            TextOut(hdc,workmodel.cxChar*(1 - workmodel.iHscrollPos),workmodel.cyChar*(index + 1 - textmodel.multiplier*workmodel.iVscrollPos),str,len);
            index++;
    }
    else{
        while(len_new > 0){
            GetTextExtentPoint32(hdc,str,len,&size_);
            while(size_.cx > workmodel.cxClient){
                len -= 1;
                GetTextExtentPoint32(hdc,str,len,&size_);
            }
            TextOut(hdc,workmodel.cxChar*(1 - workmodel.iHscrollPos),workmodel.cyChar*(index + 1 - textmodel.multiplier*workmodel.iVscrollPos),str,len);
            str = str + len;
            len_new -= len;
            len = len_new;
            index++;
        }
    }
    return index;

}
//void text_metrix(HDC &hdc,TextModel& textmodel, WorkModel&workmodel, SystemModel&sysmodel){
//    // запасна€ функци€
//    int sum_ = 0;
//    for(int i = 0;i < sysmodel.x;i++){
//        sum_ += ((i - 1)/workmodel.cxClient) * textmodel.diff[i];
//    }
//    workmodel.diff_line = sum_;
//}

void ReadFile(char *namefile,TextModel* textmodel, WorkModel* workmodel){

    size_t sizechar = strlen(namefile) + 1;
    textmodel->namefile = new char[sizechar];

    // считывание название текстового файла
    for(int i = 0;i < sizechar;i++){
        textmodel->namefile[i] = namefile[i];
    }

    FILE* file = fopen(textmodel->namefile,"r");
    textmodel->sizefile = filesize(file);
    free(textmodel->valuefile);
    textmodel->valuefile = new char[textmodel->sizefile];
    // считывание содержимого файла
    int i = 0;
    textmodel->line = 0;
    while((textmodel->valuefile[i] = fgetc(file)) != EOF){
        if(textmodel->valuefile[i] == '\n'){
            textmodel->line++;
        }
        i++;
    }
    free(textmodel->size_line_file);
    textmodel->size_line_file =  new int[textmodel->line + 1];
    textmodel->size_line_file[0] = 0;
    int index = 0; // подсчет количества элементов в строке
    int index_line = 1; // номер строки
    for(int i = 0;i < textmodel->sizefile;i++){
        index++;
        if(textmodel->valuefile[i] == '\n'){
            textmodel->size_line_file[index_line] = index;
            index_line++;
        }

    }

    workmodel->iHscrollMax = index_line;
    textmodel->multiplier = max(1,textmodel->line/40000);

}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC             hdc;
    PAINTSTRUCT     ps;
    RECT            rect;
    TEXTMETRIC      tm;
    static TextModel       textmodel;
    static WorkModel       workmodel;
    static SystemModel     sysmodel;
    HMENU hMenu;

    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            free(textmodel.namefile);
            free(textmodel.valuefile);
            free(textmodel.size_line_file);
            break;
        case WM_MOUSEMOVE:{
            // cout << LOWORD(lParam) << '-' <<HIWORD(lParam)  <<endl;
            return 0;
        }
        case WM_SIZE:{


            workmodel.cxClient = LOWORD(lParam);
            workmodel.cyClient = HIWORD(lParam);
            hdc = GetDC(hwnd);


            textmodel.multiplier = max(1,(textmodel.line + workmodel.diff_line)/40000);
            workmodel.iVscrollMax = max(0,1 + textmodel.line - workmodel.cyClient/workmodel.cyChar)/textmodel.multiplier;
            workmodel.iVscrollPos = min(workmodel.iVscrollPos,workmodel.iVscrollMax);

            SetScrollRange(hwnd, SB_VERT, 0, workmodel.iVscrollMax, FALSE);
            SetScrollPos(hwnd,SB_VERT, workmodel.iVscrollPos,TRUE);


            workmodel.iHscrollMax = max(0,workmodel.cxClient/workmodel.cxChar);
            workmodel.iHscrollPos = min(workmodel.iHscrollPos,workmodel.iHscrollMax);

            SetScrollRange(hwnd, SB_HORZ, 0, workmodel.iHscrollMax, FALSE);
            SetScrollPos(hwnd,SB_HORZ, workmodel.iHscrollPos,TRUE);


            ReleaseDC(hwnd, hdc);
            return 0;
        }
        case WM_COMMAND:
            switch(LOWORD(wParam)){
            case ID_SAVE:
                workmodel.layout = !workmodel.layout;
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            case ID_FILE:


                //DialogBox(sysmodel.hInstance,"AboutBox",hwnd,(DLGPROC)AboutDlgProc);
                OPENFILENAME ofn;
                char szFile[260]; // name file;
                HANDLE hf;
                ZeroMemory(&ofn, sizeof(ofn)); //fills a block of memory with zero
                ofn.lStructSize = sizeof(ofn); // size
                ofn.hwndOwner = hwnd; // родитель
                ofn.lpstrFile = szFile;
                // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
                // use the contents of szFile to initialize itself.
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "Text\0*.TXT\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if(GetOpenFileName(&ofn) != NULL){
                    ReadFile(ofn.lpstrFile,&textmodel,&workmodel);
                    InvalidateRect(hwnd, NULL, TRUE);
                };
                cout << textmodel.multiplier << endl;
                //free(ofn.lpstrFilter);

                return 0;
            }
            break;
        case WM_CREATE:{

            sysmodel.hInstance = ((LPCREATESTRUCT) lParam) -> hInstance;

            sysmodel.x =GetSystemMetrics(SM_CXSCREEN);
            sysmodel.y =GetSystemMetrics(SM_CYSCREEN);

            hdc = GetDC(hwnd);
            GetTextMetrics(hdc, &tm);
            workmodel.cxChar = tm.tmAveCharWidth;
            workmodel.cyChar = tm.tmHeight +  tm.tmExternalLeading;

            CREATESTRUCT* p = (CREATESTRUCT*)lParam;
            char* params_p = (char*)p->lpCreateParams; // название файла

            ReadFile(params_p,&textmodel,&workmodel); // чтение файла и инициализаци€ textmodel и workmodel
            cout << textmodel.multiplier << endl;
            ReleaseDC(hwnd, hdc);
            return 0;
        }
        case WM_PAINT:{
            InvalidateRect(hwnd, NULL, TRUE);
            hdc = BeginPaint(hwnd, &ps);
            int iPaintBeg = max(0, textmodel.multiplier*(workmodel.iVscrollPos) - 1);
            int iPaintEnd = min(textmodel.line, (int)(iPaintBeg + ps.rcPaint.bottom/workmodel.cyChar + 1));

            if(workmodel.layout == true){
                    int index = iPaintBeg;
                    for(int i = iPaintBeg;i < iPaintEnd;i++){
                        index = PrintString(hdc,index,textmodel.valuefile + textmodel.size_line_file[i - 1],textmodel.size_line_file[i] - textmodel.size_line_file[i - 1],workmodel,textmodel,true);
                    }
            }else{
                    int index = iPaintBeg;
                    for(int i = iPaintBeg;i < iPaintEnd;i++){
                        index = PrintString(hdc,index,textmodel.valuefile + textmodel.size_line_file[i - 1],textmodel.size_line_file[i] - textmodel.size_line_file[i - 1],workmodel,textmodel,false);
                    }
            }
            EndPaint(hwnd, &ps);

            return 0;
        }
        case WM_HSCROLL:{
            int iHscrollInc;
            switch(LOWORD(wParam)){
            case SB_TOP:
                iHscrollInc = -workmodel.iHscrollPos;
                break;
            case SB_BOTTOM:
                iHscrollInc = workmodel.iHscrollMax - workmodel.iHscrollPos;
                break;
            case SB_LINEUP:
                iHscrollInc = -1;
                break;
            case SB_LINEDOWN:
                iHscrollInc = 1;
                break;
            case SB_PAGEUP:
                iHscrollInc = -8;
                break;
            case SB_PAGEDOWN:
                iHscrollInc = 8;
                break;
            case SB_THUMBTRACK:
                iHscrollInc = HIWORD(wParam) - workmodel.iHscrollPos;
                break;
            default:
                iHscrollInc = 0;
            }
            iHscrollInc = max(-workmodel.iHscrollPos,min(iHscrollInc,workmodel.iHscrollMax - iHscrollInc));
            if(iHscrollInc != 0){
                workmodel.iHscrollPos += iHscrollInc;
                ScrollWindow(hwnd, -workmodel.cxChar*iHscrollInc,0,NULL,NULL);
                SetScrollPos(hwnd,SB_HORZ,workmodel.iHscrollPos,TRUE);
            }

            return 0;
        }
        case WM_VSCROLL:{
            int iVscrollInc;
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                iVscrollInc = -workmodel.iVscrollPos;
                break;
            case SB_BOTTOM:
                iVscrollInc = workmodel.iVscrollMax - workmodel.iVscrollPos;
                break;
            case SB_LINEUP:
                iVscrollInc = -1;
                break;
            case SB_LINEDOWN:
                iVscrollInc = 1;
                break;
            case SB_PAGEUP:
                iVscrollInc = min(-1,-workmodel.cyClient/workmodel.cyChar);
                break;
            case SB_PAGEDOWN:
                iVscrollInc = max(1, workmodel.cyClient/workmodel.cyChar);
                break;
            case SB_THUMBTRACK:
                iVscrollInc = abs(HIWORD(wParam)) - workmodel.iVscrollPos;
                break;
            default:
                iVscrollInc = 0;
            }
            //гаранти€, чтобы значени€ были между
            iVscrollInc = max(-workmodel.iVscrollPos,min(iVscrollInc, workmodel.iVscrollMax - workmodel.iVscrollPos));
            if(iVscrollInc != 0){
                workmodel.iVscrollPos += iVscrollInc;
                ScrollWindow(hwnd,0,-workmodel.cyChar * iVscrollInc, NULL,NULL);
                SetScrollPos(hwnd, SB_VERT, workmodel.iVscrollPos, TRUE);
                UpdateWindow(hwnd);
            }
            return 0;
        }

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{

        switch(iMsg)
        {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)){
            case IDOK:
        EndDialog(hDlg,0);
        return TRUE;
        }
        break;

        }
        return FALSE;

}
