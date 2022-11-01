#include <Windows.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <objidl.h>
#include <gdiplus.h>
#include <Commctrl.h>
#include <wchar.h>
#include <tchar.h>
#include "resource.h"
#pragma comment (lib, "Gdiplus.lib")
using namespace std;
using namespace Gdiplus;

struct USUARIO {
    wchar_t PFPpath[MAX_PATH + 1];//Dirección de la imagen de perfil
    wchar_t user[16] = L"";       //Usuario
    wchar_t password[11] = L"";   //Contraseña
    wchar_t comercio[16] = L"";   //Comercio
    wchar_t nombre[51] = L"";     //Nombre
    wchar_t alias[11] = L"";      //Alias
    bool isAdmin = false;         //Es admin o no
    bool isOwner = false;         //Es el lider de el comercio o no
    bool PrimVez = true;          //Primera vez que se inicia sesión o no
    USUARIO* sig = nullptr;
};
USUARIO* USog = nullptr;
USUARIO* USaux = nullptr;
USUARIO* SesIn = nullptr;

struct CLIENTE {
    wchar_t nombre[51] = L"";        //Nombre del cliente
    wchar_t alias[16] = L"";         //Alias del cliente
    wchar_t telefono[11] = L"";      //Telefono del cliente
    wchar_t Email[26] = L"";         //Correo del cliente
    wchar_t comercio[16] = L"";      //Comercio al que pertenece el cliente
    int dia;                         //Dia de registro
    int mes;                         //Mes de registro
    int año;                         //Año de registro
    CLIENTE* sig = nullptr;
};
CLIENTE* CLIog = nullptr;
CLIENTE* CLIaux = nullptr;

struct PROMOCION {
    wchar_t comercio[16] = L"";      //Comercio al que pertenece la promocion
    wchar_t nombre[51] = L"";        //Nombre de la promocion
    int monto = 0;                   //Monto necesario para que la promocion aplique
    float porcentaje = 0;            //Porcentaje de descuento
    bool activa = false;             //Esta activa o no
    int dia;                         //Dia de registro
    int mes;                         //Mes de registro
    int año;                         //Año de registro
    PROMOCION* sig = nullptr;
};
PROMOCION* PROog = nullptr;
PROMOCION* PROaux = nullptr;

struct CONSUMO {
    wchar_t comercio[16] = L"";      //Comercio al que pertenece el consumo
    wchar_t cliente[51] = L"";       //Cliente que compro el consumo
    float monto = 0;                 //Monto comprado
    float promocion = 0;             //Promoción aplicada
    float subtotal = 0;              //Precio subtotal del consumo (si aplica, con promoción)
    float total = 0;                 //Precio total del consumo (con IVA)
    int dia;                         //Dia de registro
    int mes;                         //Mes de registro
    int año;                         //Año de registro
    CONSUMO* sig = nullptr;
};
CONSUMO* CONog = nullptr;
CONSUMO* CONaux = nullptr;

int xImg = 0; //Valor x de la posicion de la imagen
int yImg = 0; //Valor y de la posicion de la imagen
int IndiceUniversal = 0; //Variable para guardar el indice de un listbox

bool backtomain = false; //Regresar a la pantalla principal y despues cerrarla y abrir la pantalla de inicio de sesion
bool AlreadyPfp = false; //El usuario ya ha sido registrado, ya tiene una profile picture
bool borrarog = false; //Se borra un elemento que es el primero registrado en el sistema
bool borrarSesIn = false; //Se borra el usuario que actualmente ha iniciado sesión
bool borrarComercio = false; //Se borra toda la información relacionada al comercio

int NumClientes = 0, NumPromocionesAct = 0, NumPromocionesInact = 0, NumConsumos = 0;
float NumConsumosMonto = 0;
float a = 0, b = 0, c = 0, d = 0;

wstring fulldate;
wstring SelCon;
SYSTEMTIME Registro;

HWND ISWnd = 0;
HWND ListarUsWnd = 0;
HWND MainWnd = 0;

HMENU AdminMenu;
HMENU NotAdminMenu;

//Handlers pantalla inicio sesion
HWND ed_user;
HWND ed_pass;
HWND ListaComercios;

//Handlers pantalla registro de usuarios
HWND ed_user2;
HWND ed_pass2;
HWND ed_UScom;
HWND ed_confirm;

//Handlers y variables de pantalla actualización de usuario
wchar_t DfPfp[15] = L"DefaultPfp.jpg";
TCHAR ImagePath[MAX_PATH + 1];
HWND ed_USname;
HWND ed_USalias;

//Handlers de listado de pantalla listado de usuarios
HWND ListaUs;
HWND ListaInfoUs;

//Handlers de pantalla registro de clientes
HWND ed_CLIname;
HWND ed_CLIalias;
HWND ed_CLItelef;
HWND ed_CLImail;
HWND FechaCli;

//Handlers de pantalla listado de clientes
HWND ListaCli;
HWND ListaInfoCli;
HWND FechaRegCli;

//Handlers de pantalla registro de promociones
HWND ed_PROname;
HWND ed_PROmonto;
HWND ed_PROporcentaje;
HWND ed_PROstatus;
HWND FechaPRO;

//Handlers de pantalla de listado de promociones
HWND ListaPro;
HWND ListaInfoPro;
HWND FechaRegPro;

//Handler de pantalla de registro de consumos
HWND ListaConCli;
HWND ed_CONmonto;

//Handler de pantalla de listado de consumos
HWND ListaCon;
HWND ListaInfoCon;
HWND FechaRegCon;

//Funciones de archivos
void LoadInfo(HWND Father);
void LoadInfoFile(LPWSTR Path);
void SaveInfo(HWND Father);
void WriteFile(LPWSTR Path);

//Funciones de imagenes
void OnPaint(HDC hdc, int x, int y, TCHAR path[MAX_PATH+1]);
void UploadImg(HWND Father);

//Funciones de borrado completo
void DeleteEverything();
void DeleteCom(const wchar_t Comercio[16]);

//Funciones extra
bool RegistrarUs(bool CheckCom);
void CountAll();

//Validaciones
bool UserExists(const wchar_t CompareUs[16], bool RewriteAux);
bool MismoComUs(USUARIO* info);
bool ValidatePass(const wchar_t Password[11]);
bool ValidateEmail(const wchar_t Mail[26]);

//Main Procs
BOOL CALLBACK InicioSesion(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistrarAdm(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Procs de Usuarios
BOOL CALLBACK RegistrarNewUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ListarUsuarios(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ActUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RestablecerCont(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrarUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Procs de Clientes
BOOL CALLBACK RegistrarCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ListarClientes(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ActCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrarCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Procs de Promociones
BOOL CALLBACK RegistrarPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ListarPromociones(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ActPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrarPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Procs de Consumos
BOOL CALLBACK RegistrarCon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ListarConsumos(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//Pantalla de inicio de sesión
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    ISWnd = (HWND)CreateDialog(hInstance, MAKEINTRESOURCE(ID_WINInSes), 0, (DLGPROC)InicioSesion);
    ShowWindow(ISWnd, nCmdShow);
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    //Ciclo de mensajes
    while (GetMessage(&msg, 0, 0, 0)) {
        if (ISWnd == 0 || !IsDialogMessage(ISWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

//Proc de pantalla de inicio de sesión
BOOL CALLBACK InicioSesion(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        //Elementos de la ventana
        backtomain = false;
        ed_user = GetDlgItem(hwnd, ID_EDUser);
        ed_pass = GetDlgItem(hwnd, ID_EDPassword);
        ListaComercios = GetDlgItem(hwnd, ID_ListComercios);
        SendMessage(ed_user, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SendMessage(ed_pass, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        if (USog != nullptr) {
            USaux = USog;
            while (USaux->sig != nullptr) {
                if (USaux->isOwner) {
                    SendMessage(ListaComercios, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->comercio);
                }
                USaux = USaux->sig;
            }
            if (USaux->isOwner) {
                SendMessage(ListaComercios, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->comercio);
            }
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            //Compara el usuario y contraseña ingresados con los registrados
            //y si encuentra una igualdad se permite la entrada
        case ID_ContSes:
            //Si hay usuarios registrados
            if (USog != nullptr) {
                wchar_t InputUs[16];
                wchar_t InputPass[11];
                GetWindowText(ed_user, InputUs, sizeof(InputUs) / 2);
                GetWindowText(ed_pass, InputPass, sizeof(InputPass) / 2);
                //Si el usuario existe
                if (UserExists(InputUs, true)) {
                    //Si la contraseña ingresada y la contraseña 
                    //del usuario analizado son la misma
                    if (wcscmp(InputPass, USaux->password) == 0) {
                        //Se asigna un puntero al usuario que inicio sesion
                        SesIn = USaux;
                        EndDialog(hwnd, ID_WINInSes);
                        //Si es la primera vez que inicia sesión, se pide nombre, alias y pfp
                        if (SesIn->PrimVez == true) {
                            DialogBox(NULL, MAKEINTRESOURCE(ID_InfoUs), 0, (DLGPROC)ActUs);
                            SesIn->PrimVez = false;
                        }
                        //Pantalla principal
                        MainWnd = (HWND)DialogBox(NULL, MAKEINTRESOURCE(ID_MainWnd), 0, (DLGPROC)MainWindow);
                    }
                    else {
                        MessageBox(NULL, (LPCWSTR)L"Contraseña Incorrecta", (LPCWSTR)L"Error", MB_ICONWARNING);
                    }
                }
                else {
                    MessageBox(NULL, (LPCWSTR)L"Este usuario no existe", (LPCWSTR)L"Error", MB_ICONERROR);
                }
            }
            else {
                MessageBox(NULL, (LPCWSTR)L"No hay usuarios registrados", (LPCWSTR)L"Error", MB_ICONERROR);
            }
            break;

        case ID_ARCHIVO_ABRIRARCHIVO:
            if (USog != nullptr) {
                if (IDNO == MessageBox(NULL, L"Cualquier cambio que no haya sido guardado en un archivo se perdera, ¿Estas seguro?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                    break;
                }
            }
            DeleteEverything();
            SendMessage(ListaComercios, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
            LoadInfo(hwnd);
            if (USog != nullptr) {
                USaux = USog;
                while (USaux->sig != nullptr) {
                    if (USaux->isOwner) {
                        SendMessage(ListaComercios, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->comercio);
                    }
                    USaux = USaux->sig;
                }
                if (USaux->isOwner) {
                    SendMessage(ListaComercios, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->comercio);
                }
            }
            break;

        case ID_ARCHIVO_GUARDARCOMO40031:
            SaveInfo(hwnd);
            break;

        case ID_Registrar:
            //Ventana de registro de administradores
            DialogBox(NULL, MAKEINTRESOURCE(ID_RegAdWnd), hwnd, (DLGPROC)RegistrarAdm);
            break;

        case ID_Salir:
            //Se pregunta por confirmación para salir del programa
            if (IDOK == MessageBox(NULL, (LPCWSTR)L"¿Estas seguro que deseas salir?", (LPCWSTR)L"Salir", MB_ICONWARNING | MB_OKCANCEL)) {
                if (USog != nullptr) {
                    if (IDYES == MessageBox(NULL, L"¿Quieres guardar antes de salir?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                        SaveInfo(hwnd);
                    }
                }
                PostQuitMessage(0);
            }
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, ID_WINInSes);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    }
    return FALSE;
}

//Proc de pantalla de registro de administrador
BOOL CALLBACK RegistrarAdm(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        //Elementos de la ventana
        ed_user2 = GetDlgItem(hwnd, ID_EDUser);
        ed_pass2 = GetDlgItem(hwnd, ID_EDPassword);
        ed_UScom = GetDlgItem(hwnd, ID_EDCom);
        ed_confirm = GetDlgItem(hwnd, ID_EDPassword2);
        SendMessage(ed_user2, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SendMessage(ed_pass2, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SendMessage(ed_confirm, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SendMessage(ed_UScom, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_Regresar:
            EndDialog(hwnd, ID_RegAdWnd);
            break;

        case ID_RegAdmin: 
            //Registra un nuevo administrador y revisa si el comercio registrado ya existe
            if (RegistrarUs(true) == true) {
                USaux->isOwner = true;
                SendMessage(ListaComercios, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->comercio);
                EndDialog(hwnd, ID_RegAdWnd);
            }
            break;

        case IDCANCEL:
            EndDialog(hwnd, ID_RegAdWnd);
            return TRUE;
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de la pantalla principal
BOOL CALLBACK MainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

    case WM_ENABLE:
        SetDlgItemInt(hwnd, ID_NumCli, NumClientes, TRUE);
        SetDlgItemInt(hwnd, ID_NumActProm, NumPromocionesAct, TRUE);
        SetDlgItemInt(hwnd, ID_NumNoActProm, NumPromocionesInact, TRUE);
        SetDlgItemInt(hwnd, ID_NumTotCon, NumConsumos, TRUE);
        SetDlgItemText(hwnd, ID_NumMonCon, to_wstring(NumConsumosMonto).c_str());
        //Si el usuario con la sesion iniciada fue borrado
        if (backtomain) {
            ISWnd = CreateDialog(NULL, MAKEINTRESOURCE(ID_WINInSes), 0, (DLGPROC)InicioSesion);
            EndDialog(hwnd, ID_MainWnd);
            ShowWindow(ISWnd, SWP_SHOWWINDOW);
        }
        break;

    case WM_INITDIALOG: 
        //Elementos de la ventana
        AdminMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU2));
        NotAdminMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        xImg = 11;
        yImg = 8;
        SetWindowText(hwnd, SesIn->comercio);
        SetDlgItemText(hwnd, ID_UsName, SesIn->nombre);
        if (SesIn->isAdmin) {
            DestroyMenu(NotAdminMenu);
            SetMenu(hwnd, AdminMenu);
        }
        else {
            DestroyMenu(AdminMenu);
            SetMenu(hwnd, NotAdminMenu);
        }
        CountAll();
        SetDlgItemInt(hwnd, ID_NumCli, NumClientes, TRUE);
        SetDlgItemInt(hwnd, ID_NumActProm, NumPromocionesAct, TRUE);
        SetDlgItemInt(hwnd, ID_NumNoActProm, NumPromocionesInact, TRUE);
        SetDlgItemInt(hwnd, ID_NumTotCon, NumConsumos, TRUE);
        SetDlgItemText(hwnd, ID_NumMonCon, to_wstring(NumConsumosMonto).c_str());
        CreateWindow(L"STATIC", NULL, SS_OWNERDRAW | WS_CHILD | WS_VISIBLE | WS_BORDER, xImg, yImg, 100, 100,
            hwnd, (HMENU)10330, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

            //Actualiza el usuario con la sesión iniciada
        case ID_UpUs:
            USaux = SesIn;
            AlreadyPfp = true;
            DialogBox(NULL, MAKEINTRESOURCE(ID_InfoUs), hwnd, (DLGPROC)ActUs);
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            SetDlgItemText(hwnd, ID_UsName, SesIn->nombre);
            break;

            //Registra un nuevo usuario en el mismo comercio
        case ID_USUARIOS_NUEVOUSUARIO:
            DialogBox(NULL, MAKEINTRESOURCE(ID_RegUsWnd), hwnd, (DLGPROC)RegistrarNewUs);
            break;

            //Listar usuarios registrados del comercio
        case ID_USUARIOS_LISTARUSUARIOS:
            DialogBox(NULL, MAKEINTRESOURCE(ID_ListUsersWnd), hwnd, (DLGPROC)ListarUsuarios);
            break;

            //Restablece la contraseña del usuario seleccionado
        case ID_USUARIOS_RESTABLECERCONTRASE40015:
            DialogBox(NULL, MAKEINTRESOURCE(ID_RestPassWnd), hwnd, (DLGPROC)RestablecerCont);
            break;

            //Registra un nuevo cliente
        case ID_CLIENTES_NUEVOCLIENTE:
            DialogBox(NULL, MAKEINTRESOURCE(ID_RegCliWnd), hwnd, (DLGPROC)RegistrarCli);
            break;

            //Lista los clientes registrados del comercio
        case ID_CLIENTES_LISTARCLIENTES:
            //Si no hay clientes
            if (CLIog == nullptr) {
                MessageBox(NULL, L"No hay clientes registrados", L"Error", MB_ICONERROR);
                break;
            }
            //Si hay clientes, pero no en este comercio
            else {
                CLIaux = CLIog;
                while (CLIaux->sig != nullptr) {
                    if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
                        break;
                    }
                    CLIaux = CLIaux->sig;
                }
                if (wcscmp(CLIaux->comercio, SesIn->comercio) != 0) {
                    MessageBox(NULL, L"No hay clientes registrados en este comercio", L"Advertencia", MB_ICONWARNING);
                    break;
                }
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_ListClientsWnd), hwnd, (DLGPROC)ListarClientes);
            break;

            //Registra una nueva promoción
        case ID_PROMOCIONES_NUEVAPROMOCION:
            DialogBox(NULL, MAKEINTRESOURCE(ID_RegProWnd), hwnd, (DLGPROC)RegistrarPro);
            break;

            //Lista las promociones registradas del comercio
        case ID_PROMOCIONES_LISTARPROMOCIONES:
            //Si no hay promociones en el comercio
            if (PROog == nullptr) {
                MessageBox(NULL, L"No hay promociones registradas", L"Error", MB_ICONERROR);
                break;
            }
            //Si hay promociones, pero no en este comercio
            else {
                PROaux = PROog;
                while (PROaux->sig != nullptr) {
                    if (wcscmp(PROaux->comercio, SesIn->comercio) == 0) {
                        break;
                    }
                    PROaux = PROaux->sig;
                }
                if (wcscmp(PROaux->comercio, SesIn->comercio) != 0) {
                    MessageBox(NULL, L"No hay promociones registradas en este comercio", L"Advertencia", MB_ICONWARNING);
                    break;
                }
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_ListPromotionsWnd), hwnd, (DLGPROC)ListarPromociones);
            break;

            //Registra un nuevo consumo
        case ID_CONSUMOS_NUEVOCONSUMO:
            //Si no hay clientes
            if (CLIog == nullptr) {
                MessageBox(NULL, L"No hay clientes registrados", L"Error", MB_ICONERROR);
                break;
            }
            //Si hay clientes, pero no en este comercio
            else {
                CLIaux = CLIog;
                while (CLIaux->sig != nullptr) {
                    if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
                        break;
                    }
                    CLIaux = CLIaux->sig;
                }
                if (wcscmp(CLIaux->comercio, SesIn->comercio) != 0) {
                    MessageBox(NULL, L"No hay clientes registrados en este comercio", L"Advertencia", MB_ICONWARNING);
                    break;
                }
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_RegConWnd), hwnd, (DLGPROC)RegistrarCon);
            break;

            //Lista los consumos registrados del comercio
        case ID_CONSUMOS_LISTARCONSUMOS:
            //Si no hay consumos en el comercio
            if (CONog == nullptr) {
                MessageBox(NULL, L"No hay consumos registrados", L"Error", MB_ICONERROR);
                break;
            }
            //Si hay consumos, pero no en este comercio
            else {
                CONaux = CONog;
                while (CONaux->sig != nullptr) {
                    if (wcscmp(CONaux->comercio, SesIn->comercio) == 0) {
                        break;
                    }
                    CONaux = CONaux->sig;
                }
                if (wcscmp(CONaux->comercio, SesIn->comercio) != 0) {
                    MessageBox(NULL, L"No hay consumos registrados en este comercio", L"Advertencia", MB_ICONWARNING);
                    break;
                }
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_ListTicketsWnd), hwnd, (DLGPROC)ListarConsumos);
            break;

            //Guarda los elementos registrados en un archivo
        case ID_GUARDAR_GUARDARCOMO:
            SaveInfo(hwnd);
            break;

            //Cierra y guarda un archivo con la información
        case ID_CERRAR_CERRARYGUARDAR:
            if (IDYES == MessageBox(NULL, L"¿Estas seguro que deseas salir? ", L"Advertencia", MB_ICONQUESTION | MB_YESNO)) {
                SaveInfo(hwnd);
                PostQuitMessage(0);
            }
            break;

            //Cierra sin guardar un archivo
        case ID_CERRAR_CERRARSINGUARDAR:
            if (IDYES == MessageBox(NULL, L"Los cambios hechos no seran guardados, ¿Estas seguro?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                PostQuitMessage(0);
            }
            break;

            //Cierra la sesión y regresa a la pantalla de inicio de sesión
        case ID_CERRAR_CERRARSESI40025:
            if (IDYES == MessageBox(NULL, L"¿Estas seguro que deseas cerrar sesión?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                ISWnd = CreateDialog(NULL, MAKEINTRESOURCE(ID_WINInSes), 0, (DLGPROC)InicioSesion);
                EndDialog(hwnd, ID_MainWnd);
                ShowWindow(ISWnd, SWP_SHOWWINDOW);
            }
            break;
        }
        break;

    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        xImg = 11;
        yImg = 8;
        OnPaint(hdc, xImg, yImg, SesIn->PFPpath);
        
        EndPaint(hwnd, &ps);
        ReleaseDC(hwnd, hdc);
        break;
    }
    return FALSE;
}

//Proc de pantalla para actualizar usuario
BOOL CALLBACK ActUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        //Elementos de la ventana
        ed_USname = GetDlgItem(hwnd, ID_EDUsNombre);
        ed_USalias = GetDlgItem(hwnd, ID_EDUsAlias);
        SendMessage(ed_USname, EM_SETLIMITTEXT, (WPARAM)50, (LPARAM)0);
        SendMessage(ed_USalias, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SetDlgItemText(hwnd, ID_EDUsNombre, USaux->nombre);
        SetDlgItemText(hwnd, ID_EDUsAlias, USaux->alias);
        
        //Borde para la foto de perfil
        xImg = 123;
        yImg = 5;
        CreateWindow(L"STATIC", NULL, SS_OWNERDRAW | WS_CHILD | WS_VISIBLE | WS_BORDER, xImg, yImg, 100, 100,
            hwnd, (HMENU)10330, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
    }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_Regresar:
            ZeroMemory(ImagePath, sizeof(ImagePath));
            EndDialog(hwnd, ID_InfoUs);
            break;

        case ID_Guardar: {
            wstring CheckName;
            wchar_t nom[51] = L"";
            GetWindowText(ed_USname, nom, sizeof(nom) / 2);
            if (wcslen(nom) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El nombre debe tener 5 a 50 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            bool hasonlyletesp = true;
            CheckName.clear();
            CheckName.append(nom);
            //Busca si tiene caracteres especiales (excluyendo espacios) o numeros
            for (int i = 33; i < 127; i++) {
                switch (i) {
                case 65: //Ignorar mayusculas
                    i = 91;
                    break;

                case 97: //Ignorar minusculas
                    i = 123;
                    break;
                }
                if (CheckName.find(i) != wstring::npos) {
                    hasonlyletesp = false;
                }
            }
            if (!hasonlyletesp) {
                MessageBox(NULL, (LPCWSTR)L"El nombre debe usar solo letras y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
            }
            else {
                wstring CheckAlias;
                wchar_t al[11] = L"";
                GetWindowText(ed_USalias, al, sizeof(al) / 2);
                if (wcslen(al) < 3) {
                    MessageBox(NULL, (LPCWSTR)L"El alias debe tener 3 a 10 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                    break;
                }
                bool hasonlyalf = true;
                CheckAlias.clear();
                CheckAlias.append(al);
                //Busca si tiene caracteres especiales
                for (int i = 32; i < 127; i++) {
                    switch (i) {
                    case 48: //Ignorar numeros
                        i = 58;
                        break;

                    case 65: //Ignorar mayusculas
                        i = 91;
                        break;

                    case 97: //Ignorar minusculas
                        i = 123;
                        break;
                    }
                    if (CheckAlias.find(i) != wstring::npos) {
                        hasonlyalf = false;
                    }
                }
                if (!hasonlyalf) {
                    MessageBox(NULL, (LPCWSTR)L"El alias debe usar solo letras y/o numeros", (LPCWSTR)L"Error", MB_ICONERROR);
                }
                else {
                    GetWindowText(ed_USname, USaux->nombre, sizeof(USaux->nombre) / 2);
                    GetWindowText(ed_USalias, USaux->alias, sizeof(USaux->alias) / 2);
                    wcscpy_s(USaux->PFPpath, ImagePath);
                    ZeroMemory(ImagePath, sizeof(ImagePath));
                    EndDialog(hwnd, ID_InfoUs);
                }
            }
        }
            break;

        case ID_LoadPFP:
            UploadImg(hwnd);
            break;
        }
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        //Al iniciar la ventana se pinta una default pfp hasta que sea cambiada por otra
        //Si el usuario ya tenia una, se muestra esa primero
        if (AlreadyPfp) {
            wcscpy_s(ImagePath,USaux->PFPpath);
            AlreadyPfp = false;
        }
        OnPaint(hdc, xImg, yImg, ImagePath);

        EndPaint(hwnd, &ps);
        ReleaseDC(hwnd, hdc);
        break;
    }
    return FALSE;
}

//Proc de pantalla de registro de usuario normal
BOOL CALLBACK RegistrarNewUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        //Elementos de la ventana
        ed_user2 = GetDlgItem(hwnd, ID_EDUser);
        ed_pass2 = GetDlgItem(hwnd, ID_EDPassword);
        ed_UScom = GetDlgItem(hwnd, ID_EDCom);
        ed_confirm = GetDlgItem(hwnd, ID_EDPassword2);
        SendMessage(ed_user2, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SendMessage(ed_pass2, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SendMessage(ed_confirm, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SendMessage(ed_UScom, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SetWindowText(ed_UScom,SesIn->comercio);
        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_Regresar:
            EndDialog(hwnd, ID_RegUsWnd);
            break;

        case ID_RegUser: 
            //Registra un nuevo usuario
            if (RegistrarUs(false) == true) {
                if (IsDlgButtonChecked(hwnd, ID_AdCheck) != 0) {
                    USaux->isAdmin = true;
                }
                EndDialog(hwnd, ID_RegUsWnd);
            }
            break;

        case IDCANCEL:
            EndDialog(hwnd, ID_RegUsWnd);
            return TRUE;
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de listado de usuarios
BOOL CALLBACK ListarUsuarios(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

    case WM_ENABLE:
        break;

    case WM_INITDIALOG:
        //Elementos de la ventana
        wchar_t seleccion[16];
        ListaUs = GetDlgItem(hwnd, ID_ListaUsuarios);
        ListaInfoUs = GetDlgItem(hwnd, ID_ListaInfoUs);
        USaux = USog;
        //Muestra todos los usuarios del comercio
        while (USaux->sig != nullptr) {
            if (wcscmp(USaux->comercio, SesIn->comercio) == 0) {
                SendMessage(ListaUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
            }
            USaux = USaux->sig;
        }
        if (wcscmp(USaux->comercio, SesIn->comercio) == 0) {
            SendMessage(ListaUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, ID_ListUsersWnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_ListUsersWnd);
            break;

        case ID_ActUs: {
            int indice = SendMessage((HWND)ListaUs, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaUs, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            USaux = USog;
            //Si hay mas de un usuario y no es el ultimo
            while (USaux->sig != nullptr) {
                if (wcscmp(seleccion, USaux->user) == 0) {
                    break;
                }
                else {
                    USaux = USaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, USaux->user) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el usuario", L"Error", MB_ICONWARNING);
                break;
            }
            if (!SesIn->isAdmin) {
                if (USaux != SesIn) {
                    MessageBox(NULL, L"No cuentas con los permisos para modificar este usuario", L"Error", MB_ICONWARNING);
                    break;
                }
            }
            AlreadyPfp = true;
            DialogBox(NULL, MAKEINTRESOURCE(ID_InfoUs), hwnd, (DLGPROC)ActUs);
        }
            break;

        case ID_Eliminar: {
            if (!SesIn->isAdmin) {
                MessageBox(NULL, L"No cuentas con los permisos suficientes para realizar esta acción", L"Error", MB_ICONWARNING);
                break;
            }
            int indice = SendMessage((HWND)ListaUs, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaUs, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            USaux = USog;
            borrarog = false;
            borrarSesIn = false;
            borrarComercio = false;
            //Si hay mas de un usuario y no es el ultimo
            while (USaux->sig != nullptr) {
                if (wcscmp(seleccion, USaux->user) == 0) {
                    break;
                }
                else {
                    USaux = USaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, USaux->user) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el usuario", L"Error", MB_ICONWARNING);
                break;
            }
            //Si es el primero
            if (USaux == USog) {
                borrarog = true;
            }
            //Si es el que inicio sesion
            if (USaux == SesIn) {
                if (IDYES == MessageBox(NULL, L"Estas a punto de borrar tu propio usuario, se te regresara a la pantalla de inicio de sesion, ¿Deseas continuar?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                    borrarSesIn = true;
                }
                else {
                    break;
                }
            }
            if (USaux->isOwner) {
                if (SesIn->isOwner) {
                    if (IDYES == MessageBox(NULL, L"Este usuario es el lider del comercio, al borrarlo, se borrara todo el comercio, ¿Deseas continuar?", L"Advertencia", MB_ICONWARNING | MB_YESNO)) {
                        borrarComercio = true;
                    }
                    else {
                        break;
                    }
                }
                else {
                    MessageBox(NULL, L"No tienes suficientes permisos para realizar esta acción", L"Error", MB_ICONERROR);
                    break;
                }
            }

            IndiceUniversal = indice;

            DialogBox(NULL, MAKEINTRESOURCE(ID_DeleteUserWnd), hwnd, (DLGPROC)BorrarUs);
        
            if (borrarSesIn) {
                SesIn = nullptr;
                backtomain = true;
                EndDialog(hwnd, ID_ListUsersWnd);
            }
        }
            break;

        case ID_ListaUsuarios:
            switch (HIWORD(wParam)) {
            case LBN_SELCHANGE:
                SendMessage(ListaInfoUs, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                int indice = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                ZeroMemory(seleccion, sizeof(seleccion));
                SendMessage((HWND)lParam, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
                USaux = USog;
                while (USaux->sig != nullptr) {
                    if (wcscmp(seleccion, USaux->user) == 0) {
                        break;
                    }
                    else {
                        USaux = USaux->sig;
                    }
                }
                if (wcscmp(seleccion, USaux->user) != 0) {
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"No se pudo encontrar la información de este usuario");
                    break;
                }
                //Información del usuario
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Usuario: ");
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                //Solo el admin puede ver las contraseñas de los demás
                if (SesIn->isAdmin) {
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Contraseña: ");
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->password);
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                }
                //El cajero puede ver su propia contraseña
                else {
                    if (USaux == SesIn) {
                        SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Contraseña: ");
                        SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->password);
                        SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                    }
                }
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->nombre);
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Alias: ");
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->alias);
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Tipo de usuario:");
                if (USaux->isAdmin) {
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Administrador");
                }
                else {
                    SendMessage(ListaInfoUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Cajero");
                }
                break;
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla para restablecer contraseña
BOOL CALLBACK RestablecerCont(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        //Elementos de la ventana
        wchar_t seleccion[16];
        ed_pass = GetDlgItem(hwnd, ID_EDPassRest);
        ed_confirm = GetDlgItem(hwnd, ID_EDConfRest);
        ListaUs = GetDlgItem(hwnd, ID_ListUs);
        SendMessage(ed_pass, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SendMessage(ed_confirm, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        USaux = USog;
        //Muestra todos los usuarios del comercio
        while (USaux->sig != nullptr) {
            if (wcscmp(USaux->comercio, SesIn->comercio) == 0) {
                SendMessage(ListaUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
            }
            USaux = USaux->sig;
        }
        if (wcscmp(USaux->comercio, SesIn->comercio) == 0) {
            SendMessage(ListaUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_NewPass: {
            int indice = SendMessage((HWND)ListaUs, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaUs, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            USaux = USog;
            //Si hay mas de un usuario y no es el ultimo
            while (USaux->sig != nullptr) {
                if (wcscmp(seleccion, USaux->user) == 0) {
                    break;
                }
                else {
                    USaux = USaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, USaux->user) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el usuario", L"Error", MB_ICONWARNING);
                break;
            }
            wchar_t inputpassword[11];
            wchar_t confirm[11];
            ZeroMemory(confirm, sizeof(confirm));
            ZeroMemory(confirm, sizeof(confirm));
            GetWindowText(ed_pass, inputpassword, sizeof(inputpassword) / 2);
            GetWindowText(ed_confirm, confirm, sizeof(confirm) / 2);
            //Validación de contraseñas (Minimo 1 letra, 1 caracter especial y 1 numero, 3 a 10 caracteres)
            if (wcslen(inputpassword) < 3) {
                MessageBox(NULL, (LPCWSTR)L"La contraseña debe de tener 3 a 10 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            if (ValidatePass(inputpassword) == false) {
                MessageBox(NULL, (LPCWSTR)L"La contraseña es invalida", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            if (wcscmp(confirm, inputpassword) == 0) {
                wcscpy_s(USaux->password, inputpassword);
            }
            else {
                MessageBox(NULL, (LPCWSTR)L"La contraseña no se confirmo correctamente", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            wcscpy_s(USaux->password, inputpassword);
            EndDialog(hwnd, ID_RestPassWnd);
        }
            break;
            
        case ID_Regresar:
            EndDialog(hwnd, ID_RestPassWnd);
            break;
        }
    }
    return FALSE;
}

//Proc de pantalla de registro de cliente
BOOL CALLBACK RegistrarCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        //Elementos de la ventana
        FechaCli = GetDlgItem(hwnd, ID_FechaCli);
        ed_CLIalias = GetDlgItem(hwnd, ID_EDCliAlias);
        ed_CLImail = GetDlgItem(hwnd, ID_EDCliMail);
        ed_CLIname = GetDlgItem(hwnd, ID_EDCliNombre);
        ed_CLItelef = GetDlgItem(hwnd, ID_EDCliTelef);
        SendMessage(ed_CLIname, EM_SETLIMITTEXT, (WPARAM)50, (LPARAM)0);
        SendMessage(ed_CLIalias, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SendMessage(ed_CLImail, EM_SETLIMITTEXT, (WPARAM)25, (LPARAM)0);
        SendMessage(ed_CLItelef, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SetDlgItemText(hwnd, ID_EDCom, SesIn->comercio);
        break;
    
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_RegCliWnd);
            break;

        case ID_Guardar:
            if (CLIog == nullptr) {
                CLIog = new CLIENTE;
                CLIog->sig = nullptr;
                CLIaux = CLIog;
                ZeroMemory(CLIaux->nombre, sizeof(CLIaux->nombre));
                ZeroMemory(CLIaux->alias, sizeof(CLIaux->alias));
                ZeroMemory(CLIaux->telefono, sizeof(CLIaux->telefono));
                ZeroMemory(CLIaux->comercio, sizeof(CLIaux->comercio));
                ZeroMemory(CLIaux->Email, sizeof(CLIaux->Email));
            }
            else {
                CLIaux = CLIog;
                while (CLIaux->sig != nullptr) {
                    CLIaux = CLIaux->sig;
                }
                CLIaux->sig = new CLIENTE;
                CLIaux = CLIaux->sig;
                CLIaux->sig = nullptr;
                ZeroMemory(CLIaux->nombre, sizeof(CLIaux->nombre));
                ZeroMemory(CLIaux->alias, sizeof(CLIaux->alias));
                ZeroMemory(CLIaux->telefono, sizeof(CLIaux->telefono));
                ZeroMemory(CLIaux->comercio, sizeof(CLIaux->comercio));
                ZeroMemory(CLIaux->Email, sizeof(CLIaux->Email));
            }
            bool cerrar = true;
            bool* borrar = new bool;
            *borrar = false;
            GetWindowText(ed_CLIname, CLIaux->nombre, sizeof(CLIaux->nombre) / 2);
            if (wcslen(CLIaux->nombre) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El nombre del cliente debe de tener 5 a 50 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                *borrar = true;
            }
            if (!*borrar) {
                wstring CheckName;
                bool hasonlyletesp = true;
                CheckName.clear();
                CheckName.append(CLIaux->nombre);
                //Busca si tiene caracteres especiales o numeros (excepto espacios)
                for (int i = 33; i < 127; i++) {
                    switch (i) {
                    case 65: //Ignorar mayusculas
                        i = 91;
                        break;

                    case 97: //Ignorar minusculas
                        i = 123;
                        break;
                    }
                    if (CheckName.find(i) != wstring::npos) {
                        hasonlyletesp = false;
                    }
                }
                if (!hasonlyletesp) {
                    MessageBox(NULL, (LPCWSTR)L"El nombre del cliente debe usar solo letras y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
            }

            GetWindowText(ed_CLIalias, CLIaux->alias, sizeof(CLIaux->alias) / 2);
            if (!*borrar && wcslen(CLIaux->alias) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El alias del cliente debe de tener 5 a 15 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                *borrar = true;
            }
            if (!*borrar) {
                wstring CheckAlias;
                bool hasonlyletesp = true;
                CheckAlias.clear();
                CheckAlias.append(CLIaux->alias);
                //Busca si tiene caracteres especiales o numeros (excepto espacios)
                for (int i = 33; i < 127; i++) {
                    switch (i) {
                    case 65: //Ignorar mayusculas
                        i = 91;
                        break;

                    case 97: //Ignorar minusculas
                        i = 123;
                        break;
                    }
                    if (CheckAlias.find(i) != wstring::npos) {
                        hasonlyletesp = false;
                    }
                }
                if (!hasonlyletesp) {
                    MessageBox(NULL, (LPCWSTR)L"El alias del cliente debe usar solo letras y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
            }

            GetWindowText(ed_CLItelef, CLIaux->telefono, sizeof(CLIaux->telefono) / 2);
            if (!*borrar && wcslen(CLIaux->telefono) != 10) {
                MessageBox(NULL, (LPCWSTR)L"El telefono del cliente debe de tener 10 numeros", (LPCWSTR)L"Error", MB_ICONERROR);
                *borrar = true;
            }

            GetWindowText(ed_CLImail, CLIaux->Email, sizeof(CLIaux->Email) / 2);
            if (!*borrar && wcslen(CLIaux->Email) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El e-mail del cliente debe de tener de 5 a 25 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                *borrar = true;
            }
            if (!*borrar) {
                if (ValidateEmail(CLIaux->Email) == false) {
                    MessageBox(NULL, (LPCWSTR)L"Correo invalido", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
            }
            if (*borrar) {
                cerrar = false;
                CLIENTE* temp = CLIaux;
                if (temp == CLIog) {
                    CLIog = nullptr;
                    CLIaux = nullptr;
                }
                else {
                    CLIENTE* temp2 = CLIog;
                    while (temp2->sig != CLIaux) {
                        temp2 = temp2->sig;
                    }
                    temp2->sig = nullptr;
                    temp2 = nullptr;
                    CLIaux = nullptr;
                    delete temp2;
                }
                delete temp;
            }
            delete borrar;

            //Si un cliente se registro correctamente, se cierra la pantalla de registro
            if (cerrar) {
                SendDlgItemMessage(hwnd, ID_FechaCli, MCM_GETCURSEL, 0, (LPARAM)&Registro);
                CLIaux->dia = Registro.wDay;
                CLIaux->mes = Registro.wMonth;
                CLIaux->año = Registro.wYear;
                wcscpy_s(CLIaux->comercio, SesIn->comercio);
                NumClientes++;
                EndDialog(hwnd, ID_RegCliWnd);
            }
            break;
        }
        break;
    }
    
    return FALSE;
}

//Proc de pantalla de listado de clientes
BOOL CALLBACK ListarClientes(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        wchar_t seleccion[16];
        ListaCli = GetDlgItem(hwnd, ID_ListaClientes);
        ListaInfoCli = GetDlgItem(hwnd, ID_ListaInfoCli);
        FechaRegCli = GetDlgItem(hwnd, ID_FechaRegCli);
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
                SendMessage(ListaCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
            }
            CLIaux = CLIaux->sig;
        }
        if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
            SendMessage(ListaCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_ListClientsWnd);
            break;

        case ID_ActCli: {
            if (CLIog == nullptr) {
                MessageBox(NULL, L"No se pudo encontrar el cliente", L"Error", MB_ICONWARNING);
                break;
            }
            int indice = SendMessage((HWND)ListaCli, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaCli, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            CLIaux = CLIog;
            //Si hay mas de un usuario y no es el ultimo
            while (CLIaux->sig != nullptr) {
                if (wcscmp(seleccion, CLIaux->nombre) == 0 && wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
                    break;
                }
                else {
                    CLIaux = CLIaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, CLIaux->nombre) != 0 || wcscmp(SesIn->comercio, CLIaux->comercio) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el cliente", L"Error", MB_ICONWARNING);
                break;
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_ActCliWnd), hwnd, (DLGPROC)ActCli);
        }
        break;

        case ID_Eliminar: {
            if (!SesIn->isAdmin) {
                MessageBox(NULL, L"No cuentas con los permisos para realizar esta acción", L"Error", MB_ICONWARNING);
                break;
            }
            if (CLIog == nullptr) {
                MessageBox(NULL, L"No se pudo encontrar el cliente", L"Error", MB_ICONWARNING);
                break;
            }
            int indice = SendMessage((HWND)ListaCli, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaCli, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            CLIaux = CLIog;
            borrarog = false;

            //Si hay mas de un cliente y no es el ultimo
            while (CLIaux->sig != nullptr) {
                if (wcscmp(seleccion, CLIaux->nombre) == 0 && wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
                    break;
                }
                else {
                    CLIaux = CLIaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, CLIaux->nombre) != 0 || wcscmp(SesIn->comercio, CLIaux->comercio) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el cliente", L"Error", MB_ICONWARNING);
                break;
            }
            //Si es primero
            if (CLIaux == CLIog) {
                borrarog = true;
            }

            IndiceUniversal = indice;

            DialogBox(NULL, MAKEINTRESOURCE(ID_DeleteClientWnd), hwnd, (DLGPROC)BorrarCli);
        }
        break;

        case ID_ListaClientes:
            switch (HIWORD(wParam)) {
            case LBN_SELCHANGE:
                SendMessage(ListaInfoCli, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                int indice = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                ZeroMemory(seleccion, sizeof(seleccion));
                SendMessage((HWND)lParam, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
                CLIaux = CLIog;
                while (CLIaux->sig != nullptr) {
                    if (wcscmp(seleccion, CLIaux->nombre) == 0 && wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
                        break;
                    }
                    else {
                        CLIaux = CLIaux->sig;
                    }
                }
                if (wcscmp(seleccion, CLIaux->nombre) != 0 || wcscmp(SesIn->comercio, CLIaux->comercio) != 0) {
                    SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"No se pudo encontrar la información de este cliente");
                    break;
                }

                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Alias: ");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->alias);
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"E-mail: ");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->Email);
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Telefono: ");
                SendMessage(ListaInfoCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->telefono);
                Registro.wDay = CLIaux->dia;
                Registro.wMonth = CLIaux->mes;
                Registro.wYear = CLIaux->año;
                SendMessage(FechaRegCli, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
                break;
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla para actualizar clientes
BOOL CALLBACK ActCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        FechaCli = GetDlgItem(hwnd, ID_FechaCli);
        ed_CLIalias = GetDlgItem(hwnd, ID_EDCliAlias);
        ed_CLImail = GetDlgItem(hwnd, ID_EDCliMail);
        ed_CLItelef = GetDlgItem(hwnd, ID_EDCliTelef);
        Registro.wDay = CLIaux->dia;
        Registro.wMonth = CLIaux->mes;
        Registro.wYear = CLIaux->año;
        SendMessage(ed_CLIalias, EM_SETLIMITTEXT, (WPARAM)15, (LPARAM)0);
        SendMessage(ed_CLImail, EM_SETLIMITTEXT, (WPARAM)25, (LPARAM)0);
        SendMessage(ed_CLItelef, EM_SETLIMITTEXT, (WPARAM)10, (LPARAM)0);
        SetDlgItemText(hwnd, ID_EDCliNombre, CLIaux->nombre);
        SetDlgItemText(hwnd, ID_EDCliAlias, CLIaux->alias);
        SetDlgItemText(hwnd, ID_EDCliTelef, CLIaux->telefono);
        SetDlgItemText(hwnd, ID_EDCliMail, CLIaux->Email);
        SetDlgItemText(hwnd, ID_EDCom, CLIaux->comercio);
        SendMessage(FechaCli, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_ActCliWnd);
            break;

        case ID_Guardar: {
            wchar_t al[16] = L"";
            GetWindowText(ed_CLIalias, al, sizeof(al) / 2);
            if (wcslen(al) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El alias del cliente debe de tener 5 a 15 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            wstring CheckAlias;
            bool hasonlyletesp = true;
            CheckAlias.clear();
            CheckAlias.append(al);
            //Busca si tiene caracteres especiales o numeros (excepto espacios)
            for (int i = 33; i < 127; i++) {
                switch (i) {
                case 65: //Ignorar mayusculas
                    i = 91;
                    break;

                case 97: //Ignorar minusculas
                    i = 123;
                    break;
                }
                if (CheckAlias.find(i) != wstring::npos) {
                    hasonlyletesp = false;
                }
            }
            if (!hasonlyletesp) {
                MessageBox(NULL, (LPCWSTR)L"El alias del cliente debe usar solo letras y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            wchar_t mail[26] = L"";
            GetWindowText(ed_CLImail, mail, sizeof(mail) / 2);
            if (wcslen(mail) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El e-mail del cliente debe de tener de 5 a 25 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            if (ValidateEmail(mail) == false) {
                MessageBox(NULL, (LPCWSTR)L"Correo invalido", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            wchar_t telefono[11] = L"";
            GetWindowText(ed_CLItelef, telefono, sizeof(telefono) / 2);
            if (wcslen(telefono) != 10) {
                MessageBox(NULL, (LPCWSTR)L"El telefono del cliente debe de tener 10 numeros", (LPCWSTR)L"Error", MB_ICONERROR);
                break;
            }
            GetWindowText(ed_CLIalias, CLIaux->alias, sizeof(CLIaux->alias) / 2);
            GetWindowText(ed_CLImail, CLIaux->Email, sizeof(CLIaux->Email) / 2);
            GetWindowText(ed_CLItelef, CLIaux->telefono, sizeof(CLIaux->telefono) / 2);
            EndDialog(hwnd, ID_ActCliWnd);
        }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de registro de promociones
BOOL CALLBACK RegistrarPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        ed_PROname = GetDlgItem(hwnd, ID_EDProNombre);
        ed_PROmonto = GetDlgItem(hwnd, ID_EDProMonto);
        ed_PROporcentaje = GetDlgItem(hwnd, ID_EDProPrj);
        ed_PROstatus = GetDlgItem(hwnd, ID_ActivePro);
        FechaPRO = GetDlgItem(hwnd, ID_FechaPro);
        SendMessage(ed_PROname, EM_SETLIMITTEXT, (WPARAM)50, (LPARAM)0);
        SendMessage(ed_PROmonto, EM_SETLIMITTEXT, (WPARAM)7, (LPARAM)0);
        SendMessage(ed_PROporcentaje, EM_SETLIMITTEXT, (WPARAM)6, (LPARAM)0);
        SetDlgItemText(hwnd, ID_EDCom, SesIn->comercio);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_RegProWnd);
            break;

        case ID_Guardar:
            if (PROog == nullptr) {
                PROog = new PROMOCION;
                PROog->sig = nullptr;
                PROaux = PROog;
                ZeroMemory(PROaux->nombre, sizeof(PROaux->nombre));
            }
            else {
                PROaux = PROog;
                while (PROaux->sig != nullptr) {
                    PROaux = PROaux->sig;
                }
                PROaux->sig = new PROMOCION;
                PROaux = PROaux->sig;
                PROaux->sig = nullptr;
                ZeroMemory(PROaux->nombre, sizeof(PROaux->nombre));
            }
            bool cerrar = true;
            bool* borrar = new bool;
            *borrar = false;
            GetWindowText(ed_PROname, PROaux->nombre, sizeof(PROaux->nombre) / 2);
            if (wcslen(PROaux->nombre) < 5) {
                MessageBox(NULL, (LPCWSTR)L"El nombre de la promoción debe de tener 5 a 50 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
                *borrar = true;
            }
            if (!*borrar) {
                wstring CheckName;
                bool hasonlyletesp = true;
                CheckName.clear();
                CheckName.append(PROaux->nombre);
                //Busca si tiene caracteres especiales o numeros (excepto espacios)
                for (int i = 33; i < 127; i++) {
                    switch (i) {
                    case 65: //Ignorar mayusculas
                        i = 91;
                        break;

                    case 97: //Ignorar minusculas
                        i = 123;
                        break;
                    }
                    if (CheckName.find(i) != wstring::npos) {
                        hasonlyletesp = false;
                    }
                }
                if (!hasonlyletesp) {
                    MessageBox(NULL, (LPCWSTR)L"El nombre de la promoción debe usar solo letras y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
            }
            PROaux->monto = GetDlgItemInt(hwnd, ID_EDProMonto, NULL, TRUE);
            if (!*borrar) {
                if (PROaux->monto <= 100) {
                    MessageBox(NULL, (LPCWSTR)L"El monto de la promoción debe ser mayor a 100", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
            }
            if (!*borrar) {
                wchar_t Porcentaje[11] = L"";
                GetWindowText(ed_PROporcentaje, Porcentaje, sizeof(Porcentaje) / 2);
                wstring CheckPor;
                bool hasonlynumpoint = true;
                int pointcounter = 0;
                CheckPor.clear();
                CheckPor.append(Porcentaje);
                //Busca si tiene caracteres especiales o letras
                for (int i = 32; i < 127; i++) {
                    switch (i) {
                    case 46: //Ignorar punto
                        i = 47;
                        break;

                    case 48: //Ignorar numeros
                        i = 58;
                        break;
                    }
                    if (CheckPor.find(i) != wstring::npos) {
                        hasonlynumpoint = false;
                    }
                }
                for (int i = 0; i < wcslen(Porcentaje); i++) {
                    if (Porcentaje[i] == '.') {
                        pointcounter++;
                    }
                }
                if (!hasonlynumpoint || pointcounter > 1) {
                    MessageBox(NULL, (LPCWSTR)L"El porcentaje de la promoción debe ser solo numeros y un punto", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
                else {
                    PROaux->porcentaje = wcstod(Porcentaje, NULL);
                    if (PROaux->porcentaje <= 0 || PROaux->porcentaje >= 100) {
                        MessageBox(NULL, (LPCWSTR)L"El porcentaje de la promoción debe ser mayor a 0 y menor a 100", (LPCWSTR)L"Error", MB_ICONERROR);
                        *borrar = true;
                    }
                }
            }
            if (*borrar) {
                cerrar = false;
                PROMOCION* temp = PROaux;
                if (temp == PROog) {
                    PROog = nullptr;
                    PROaux = nullptr;
                }
                else {
                    PROMOCION* temp2 = PROog;
                    while (temp2->sig != PROaux) {
                        temp2 = temp2->sig;
                    }
                    temp2->sig = nullptr;
                    temp2 = nullptr;
                    PROaux = nullptr;
                    delete temp2;
                }
                delete temp;
            }
            delete borrar;
            if (cerrar) {
                if (IsDlgButtonChecked(hwnd, ID_ActivePro) != 0) {
                    PROaux->activa = true;
                    NumPromocionesAct++;
                }
                else {
                    PROaux->activa = false;
                    NumPromocionesInact++;
                }
                SendDlgItemMessage(hwnd, ID_FechaPro, MCM_GETCURSEL, 0, (LPARAM)&Registro);
                PROaux->dia = Registro.wDay;
                PROaux->mes = Registro.wMonth;
                PROaux->año = Registro.wYear;
                wcscpy_s(PROaux->comercio, SesIn->comercio);
                EndDialog(hwnd, ID_RegProWnd);
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de listado de promociones
BOOL CALLBACK ListarPromociones(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        wchar_t seleccion[51];
        ListaPro = GetDlgItem(hwnd, ID_ListaPromociones);
        ListaInfoPro = GetDlgItem(hwnd, ID_ListaInfoPro);
        FechaRegPro = GetDlgItem(hwnd, ID_FechaRegPro);
        PROaux = PROog;
        while (PROaux->sig != nullptr) {
            if (wcscmp(PROaux->comercio, SesIn->comercio) == 0) {
                SendMessage(ListaPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)PROaux->nombre);
            }
            PROaux = PROaux->sig;
        }
        if (wcscmp(PROaux->comercio, SesIn->comercio) == 0) {
            SendMessage(ListaPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)PROaux->nombre);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_ListPromotionsWnd);
            break;

        case ID_ActPro: {
            if (PROog == nullptr) {
                MessageBox(NULL, L"No se pudo encontrar la promoción", L"Error", MB_ICONWARNING);
                break;
            }
            int indice = SendMessage((HWND)ListaPro, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaPro, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            PROaux = PROog;
            //Si hay mas de un usuario y no es el ultimo
            while (PROaux->sig != nullptr) {
                if (wcscmp(seleccion, PROaux->nombre) == 0 && wcscmp(SesIn->comercio, PROaux->comercio) == 0) {
                    break;
                }
                else {
                    PROaux = PROaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, PROaux->nombre) != 0 || wcscmp(SesIn->comercio, PROaux->comercio) != 0) {
                MessageBox(NULL, L"No se pudo encontrar la promoción", L"Error", MB_ICONWARNING);
                break;
            }
            DialogBox(NULL, MAKEINTRESOURCE(ID_ActProWnd), hwnd, (DLGPROC)ActPro);
        }
        break;

        case ID_Eliminar: {
            if (!SesIn->isAdmin) {
                MessageBox(NULL, L"No cuentas con los permisos para realizar esta acción", L"Error", MB_ICONWARNING);
                break;
            }
            if (PROog == nullptr) {
                MessageBox(NULL, L"No se pudo encontrar la promocion", L"Error", MB_ICONWARNING);
                break;
            }
            int indice = SendMessage((HWND)ListaPro, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaPro, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            PROaux = PROog;
            borrarog = false;

            //Si hay mas de un cliente y no es el ultimo
            while (PROaux->sig != nullptr) {
                if (wcscmp(seleccion, PROaux->nombre) == 0 && wcscmp(SesIn->comercio, PROaux->comercio) == 0) {
                    break;
                }
                else {
                    PROaux = PROaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, PROaux->nombre) != 0 || wcscmp(SesIn->comercio, PROaux->comercio) != 0) {
                MessageBox(NULL, L"No se pudo encontrar la promoción", L"Error", MB_ICONWARNING);
                break;
            }
            //Si es primero
            if (PROaux == PROog) {
                borrarog = true;
            }

            IndiceUniversal = indice;

            DialogBox(NULL, MAKEINTRESOURCE(ID_DeletePromotionWnd), hwnd, (DLGPROC)BorrarPro);
        }
        break;

        case ID_ListaPromociones:
            switch (HIWORD(wParam)) {
            case LBN_SELCHANGE:
                SendMessage(ListaInfoPro, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                int indice = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                ZeroMemory(seleccion, sizeof(seleccion));
                SendMessage((HWND)lParam, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
                PROaux = PROog;
                while (PROaux->sig != nullptr) {
                    if (wcscmp(seleccion, PROaux->nombre) == 0 && wcscmp(SesIn->comercio, PROaux->comercio) == 0) {
                        break;
                    }
                    else {
                        PROaux = PROaux->sig;
                    }
                }
                if (wcscmp(seleccion, PROaux->nombre) != 0 || wcscmp(SesIn->comercio, PROaux->comercio) != 0) {
                    SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"No se pudo encontrar la información de esta promocion");
                    break;
                }
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)PROaux->nombre);
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Monto para que la promoción aplique: ");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(PROaux->monto).c_str());
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Porcentaje de descuento: ");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(PROaux->porcentaje).append(L"%").c_str());
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Estatus: ");
                if (PROaux->activa) {
                    SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Activa");
                }
                else {
                    SendMessage(ListaInfoPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Inactiva");
                }
                Registro.wDay = PROaux->dia;
                Registro.wMonth = PROaux->mes;
                Registro.wYear = PROaux->año;
                SendMessage(FechaRegPro, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
                break;
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla para activar promociones
BOOL CALLBACK ActPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND FechaPro = GetDlgItem(hwnd, ID_FechaPro);
        HWND CheckBox = GetDlgItem(hwnd, ID_ActivePro);
        wchar_t monto[10];
        wchar_t porcentaje[3];
        _itow_s(PROaux->monto, monto, sizeof(monto) / 2, 10);
        _itow_s(PROaux->porcentaje, porcentaje, sizeof(porcentaje) / 2, 10);
        SetDlgItemText(hwnd, ID_EDProNombre, PROaux->nombre);
        SetDlgItemText(hwnd, ID_EDProMonto, monto);
        SetDlgItemText(hwnd, ID_EDProPrj, porcentaje);
        SetDlgItemText(hwnd, ID_EDCom, SesIn->comercio);
        Registro.wDay = PROaux->dia;
        Registro.wMonth = PROaux->mes;
        Registro.wYear = PROaux->año;
        if (PROaux->activa) {
            SendMessage(CheckBox, BM_SETCHECK, BST_CHECKED, 0);
            NumPromocionesAct--;
        }
        else {
            NumPromocionesInact--;
        }
        SendMessage(FechaPro, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
    }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Guardar:
            if (IsDlgButtonChecked(hwnd, ID_ActivePro) != 0) {
                PROaux->activa = true;
                NumPromocionesAct++;
            }
            else {
                PROaux->activa = false;
                NumPromocionesInact++;
            }
            EndDialog(hwnd, ID_ActProWnd);
            break;

        case ID_Regresar:
            if (PROaux->activa) {
                NumPromocionesAct++;
            }
            else {
                NumPromocionesInact++;
            }
            EndDialog(hwnd, ID_ActProWnd);
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de registro de consumos
BOOL CALLBACK RegistrarCon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        wchar_t seleccion[51];
        ListaCli = GetDlgItem(hwnd, ID_ListaConCli);
        ed_CONmonto = GetDlgItem(hwnd, ID_EDConMonto);
        SendMessage(ed_CONmonto, EM_SETLIMITTEXT, (WPARAM)7, (LPARAM)0);
        SetDlgItemText(hwnd, ID_EDCom, SesIn->comercio);
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
                SendMessage(ListaCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
            }
            CLIaux = CLIaux->sig;
        }
        if (wcscmp(CLIaux->comercio, SesIn->comercio) == 0) {
            SendMessage(ListaCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_RegConWnd);
            break;

        case ID_Guardar: {
            if (CONog == nullptr) {
                CONog = new CONSUMO;
                CONog->sig = nullptr;
                CONaux = CONog;
                ZeroMemory(CONaux->cliente, sizeof(CONaux->cliente));
            }
            else {
                CONaux = CONog;
                while (CONaux->sig != nullptr) {
                    CONaux = CONaux->sig;
                }
                CONaux->sig = new CONSUMO;
                CONaux = CONaux->sig;
                CONaux->sig = nullptr;
                ZeroMemory(CONaux->cliente, sizeof(CONaux->cliente));
            }
            int indice = SendMessage((HWND)ListaCli, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            ZeroMemory(seleccion, sizeof(seleccion));
            SendMessage((HWND)ListaCli, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
            bool* borrar = new bool;
            *borrar = false;
            CLIaux = CLIog;

            //Si hay mas de un cliente y no es el ultimo
            while (CLIaux->sig != nullptr) {
                if (wcscmp(seleccion, CLIaux->nombre) == 0 && wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
                    break;
                }
                else {
                    CLIaux = CLIaux->sig;
                }
            }
            //Si no es el unico, o el ultimo
            if (wcscmp(seleccion, CLIaux->nombre) != 0 || wcscmp(SesIn->comercio, CLIaux->comercio) != 0) {
                MessageBox(NULL, L"No se pudo encontrar el cliente", L"Error", MB_ICONWARNING);
                *borrar = true;
            }
            if (!*borrar) {
                wchar_t Monto[11] = L"";
                GetWindowText(ed_CONmonto, Monto, sizeof(Monto) / 2);
                wstring CheckMon;
                bool hasonlynumpoint = true;
                int pointcounter = 0;
                CheckMon.clear();
                CheckMon.append(Monto);
                //Busca si tiene caracteres especiales o letras
                for (int i = 32; i < 127; i++) {
                    switch (i) {
                    case 46: //Ignorar punto
                        i = 47;
                        break;

                    case 48: //Ignorar numeros
                        i = 58;
                        break;
                    }
                    if (CheckMon.find(i) != wstring::npos) {
                        hasonlynumpoint = false;
                    }
                }
                for (int i = 0; i < wcslen(Monto); i++) {
                    if (Monto[i] == '.') {
                        pointcounter++;
                    }
                }
                if (!hasonlynumpoint || pointcounter > 1) {
                    MessageBox(NULL, (LPCWSTR)L"El monto del consumo debe ser solo numeros y un punto", (LPCWSTR)L"Error", MB_ICONERROR);
                    *borrar = true;
                }
                else {
                    CONaux->monto = wcstod(Monto, NULL);
                    if (CONaux->monto <= 0) {
                        MessageBox(NULL, (LPCWSTR)L"El monto del consumo debe ser mayor a 0", (LPCWSTR)L"Error", MB_ICONERROR);
                        *borrar = true;
                    }
                }
            }
            if (*borrar) {
                CONSUMO* temp = CONaux;
                if (temp == CONog) {
                    CONog = nullptr;
                    CONaux = nullptr;
                }
                else {
                    CONSUMO* temp2 = CONog;
                    while (temp2->sig != CONaux) {
                        temp2 = temp2->sig;
                    }
                    temp2->sig = nullptr;
                    temp2 = nullptr;
                    CONaux = nullptr;
                    delete temp2;
                }
                delete temp;
            }
            else {
                CONaux->promocion = b;
                CONaux->subtotal = c;
                CONaux->total = d;
                SendDlgItemMessage(hwnd, ID_FechaCon, MCM_GETCURSEL, 0, (LPARAM)&Registro);
                CONaux->dia = Registro.wDay;
                CONaux->mes = Registro.wMonth;
                CONaux->año = Registro.wYear;
                wcscpy_s(CONaux->comercio, SesIn->comercio);
                wcscpy_s(CONaux->cliente, CLIaux->nombre);
                NumConsumos++;
                NumConsumosMonto = NumConsumosMonto + CONaux->total;
                EndDialog(hwnd, ID_RegConWnd);
            }
            delete borrar;
        }
            break;

        case ID_EDConMonto:
            switch (HIWORD(wParam)) {
            case EN_CHANGE: {
                a = 0, b = 0, c = 0, d = 0;
                bool ignore = false;
                wchar_t Monto[11] = L"";
                GetWindowText(ed_CONmonto, Monto, sizeof(Monto) / 2);
                wstring CheckMon;
                bool hasonlynumpoint = true;
                int pointcounter = 0;
                CheckMon.clear();
                CheckMon.append(Monto);
                //Busca si tiene caracteres especiales o letras
                for (int i = 32; i < 127; i++) {
                    switch (i) {
                    case 46: //Ignorar punto
                        i = 47;
                        break;

                    case 48: //Ignorar numeros
                        i = 58;
                        break;
                    }
                    if (CheckMon.find(i) != wstring::npos) {
                        hasonlynumpoint = false;
                    }
                }
                for (int i = 0; i < wcslen(Monto); i++) {
                    if (Monto[i] == '.') {
                        pointcounter++;
                    }
                }
                if (!hasonlynumpoint || pointcounter > 1) {
                    ignore = true;
                }
                else {
                    a = wcstod(Monto, NULL);
                    if (a <= 0) {
                        ignore = true;
                    }
                }
                if (!ignore) {
                    if (PROog != nullptr) {
                        PROaux = PROog;
                        while (PROaux->sig != nullptr) {
                            if (wcscmp(PROaux->comercio, SesIn->comercio) == 0 && a == PROaux->monto && PROaux->activa) {
                                b = PROaux->porcentaje;
                                break;
                            }
                            PROaux = PROaux->sig;
                        }
                        if (wcscmp(PROaux->comercio, SesIn->comercio) == 0 && a == PROaux->monto && PROaux->activa) {
                            b = PROaux->porcentaje;
                        }
                    }
                    SetDlgItemText(hwnd, ID_EDConDesc, to_wstring(b).c_str());
                    c = a * (b / 100);
                    c = a - c;
                    SetDlgItemText(hwnd, ID_EDConSub, to_wstring(c).c_str());
                    d = c * 0.16;
                    d = c + d;
                    SetDlgItemText(hwnd, ID_EDConTotal, to_wstring(d).c_str());
                }
            }
                break;
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de listado de consumos
BOOL CALLBACK ListarConsumos(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: 
        ListaCon = GetDlgItem(hwnd, ID_ListaConsumos);
        ListaInfoCon = GetDlgItem(hwnd, ID_ListaInfoCon);
        FechaRegCon = GetDlgItem(hwnd, ID_FechaRegCon);
        CONaux = CONog;
        wchar_t day[3], month[3], year[5], seleccion[51+13];
        
        //fulldate.substr(fulldate.length() - 13);
        while (CONaux->sig != nullptr) {
            if (wcscmp(CONaux->comercio, SesIn->comercio) == 0) {
                _itow_s(CONaux->dia, day, sizeof(day) / 2, 10);
                _itow_s(CONaux->mes, month, sizeof(month) / 2, 10);
                _itow_s(CONaux->año, year, sizeof(year) / 2, 10);
                fulldate.clear();
                fulldate.append(day);
                fulldate.append(L"/");
                fulldate.append(month);
                fulldate.append(L"/");
                fulldate.append(year);
                fulldate.append(L" - ");
                fulldate.append(CONaux->cliente);
                SendMessage(ListaCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)fulldate.c_str());
            }
            CONaux = CONaux->sig;
        }
        if (wcscmp(CONaux->comercio, SesIn->comercio) == 0) {
            _itow_s(CONaux->dia, day, sizeof(day) / 2, 10);
            _itow_s(CONaux->mes, month, sizeof(month) / 2, 10);
            _itow_s(CONaux->año, year, sizeof(year) / 2, 10);
            fulldate.clear();
            fulldate.append(day);
            fulldate.append(L"/");
            fulldate.append(month);
            fulldate.append(L"/");
            fulldate.append(year);
            fulldate.append(L" - ");
            fulldate.append(CONaux->cliente);
            SendMessage(ListaCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)fulldate.c_str());
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_ListTicketsWnd);
            break;

        case ID_ListaConsumos:
            switch (HIWORD(wParam)) {
            case LBN_SELCHANGE:
                SendMessage(ListaInfoCon, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                int indice = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                ZeroMemory(seleccion, sizeof(seleccion));
                SendMessage((HWND)lParam, (UINT)LB_GETTEXT, (WPARAM)indice, (LPARAM)seleccion);
                SelCon.clear();
                SelCon.append(seleccion);
                CONaux = CONog;
                while (CONaux->sig != nullptr) {
                    _itow_s(CONaux->dia, day, sizeof(day) / 2, 10);
                    _itow_s(CONaux->mes, month, sizeof(month) / 2, 10);
                    _itow_s(CONaux->año, year, sizeof(year) / 2, 10);
                    fulldate.clear();
                    fulldate.append(day);
                    fulldate.append(L"/");
                    fulldate.append(month);
                    fulldate.append(L"/");
                    fulldate.append(year);
                    fulldate.append(L" - ");
                    fulldate.append(CONaux->cliente);
                    if (SelCon.compare(fulldate) == 0 && wcscmp(SesIn->comercio, CONaux->comercio) == 0) {
                        break;
                    }
                    else {
                        CONaux = CONaux->sig;
                    }
                }
                _itow_s(CONaux->dia, day, sizeof(day) / 2, 10);
                _itow_s(CONaux->mes, month, sizeof(month) / 2, 10);
                _itow_s(CONaux->año, year, sizeof(year) / 2, 10);
                fulldate.clear();
                fulldate.append(day);
                fulldate.append(L"/");
                fulldate.append(month);
                fulldate.append(L"/");
                fulldate.append(year);
                fulldate.append(L" - ");
                fulldate.append(CONaux->cliente);
                if (SelCon.compare(fulldate) != 0 || wcscmp(SesIn->comercio, CONaux->comercio) != 0) {
                    SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"No se pudo encontrar la información de este consumo");
                    break;
                }
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Cliente: ");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)CONaux->cliente);
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Monto: ");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(CONaux->monto).c_str());
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                if (CONaux->promocion != 0) {
                    SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Promoción: ");
                    SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(CONaux->promocion).append(L"%").c_str());
                    SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                }
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Subtotal: ");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(CONaux->subtotal).c_str());
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Total: ");
                SendMessage(ListaInfoCon, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(CONaux->total).c_str());
                Registro.wDay = CONaux->dia;
                Registro.wMonth = CONaux->mes;
                Registro.wYear = CONaux->año;
                SendMessage(FechaRegCon, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
                break;
            }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de eliminación de usuarios
BOOL CALLBACK BorrarUs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND ListaBorrarUs = GetDlgItem(hwnd, ID_ListaBorrarUs);
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Usuario: ");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->user);
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Contraseña: ");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->password);
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->nombre);
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Alias: ");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)USaux->alias);
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Tipo de usuario:");
        if (USaux->isAdmin) {
            SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Administrador");
        }
        else {
            SendMessage(ListaBorrarUs, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Cajero");
        }
    }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_DeleteUserWnd);
            break;

        case ID_Eliminar:
            if (borrarComercio) {
                DeleteCom(USaux->comercio);
            }
            USUARIO* temp = USaux;
            USaux = USog;
            //Si solo hay un usuario
            if (borrarog && USog->sig == nullptr) {
                USog = nullptr;
                USaux = nullptr;
            }
            //Si es el primero y hay más usuarios
            else if (borrarog && USog->sig != nullptr) {
                USog = USog->sig;
                USaux = nullptr;
            }
            else {
                while (USaux->sig != nullptr) {
                    if (USaux->sig == temp) {
                        break;
                    }
                    else {
                        USaux = USaux->sig;
                    }
                }
                USaux->sig = temp->sig;
            }
            delete temp;
            SendMessage((HWND)ListaUs, (UINT)LB_DELETESTRING, (WPARAM)IndiceUniversal, (LPARAM)0);
            SendMessage((HWND)ListaInfoUs, (UINT)LB_RESETCONTENT, (WPARAM)IndiceUniversal, (LPARAM)0);
            EndDialog(hwnd, ID_DeleteUserWnd);
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de eliminación de clientes
BOOL CALLBACK BorrarCli(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND ListaBorrarCli = GetDlgItem(hwnd, ID_ListaBorrarCli);
        HWND FechaDelCli = GetDlgItem(hwnd, ID_FechaDelCli);
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->nombre);
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Alias: ");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->alias);
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"E-mail: ");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->Email);
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Telefono: ");
        SendMessage(ListaBorrarCli, LB_ADDSTRING, (WPARAM)0, (LPARAM)CLIaux->telefono);
        Registro.wDay = CLIaux->dia;
        Registro.wMonth = CLIaux->mes;
        Registro.wYear = CLIaux->año;
        SendMessage(FechaDelCli, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
    }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_DeleteClientWnd);
            break;
        case ID_Eliminar: {
            CLIENTE* temp = CLIaux;
            CLIaux = CLIog;
            //Si solo hay un cliente
            if (borrarog && CLIog->sig == nullptr) {
                CLIog = nullptr;
                CLIaux = nullptr;
            }
            //Si es el primero y hay más clientes
            else if (borrarog && CLIog->sig != nullptr) {
                CLIog = CLIog->sig;
                CLIaux = nullptr;
            }
            else {
                while (CLIaux->sig != nullptr) {
                    if (CLIaux->sig == temp) {
                        break;
                    }
                    else {
                        CLIaux = CLIaux->sig;
                    }
                }
                CLIaux->sig = temp->sig;
            }
            delete temp;
            NumClientes--;
            SendMessage((HWND)ListaCli, (UINT)LB_DELETESTRING, (WPARAM)IndiceUniversal, (LPARAM)0);
            SendMessage((HWND)ListaInfoCli, (UINT)LB_RESETCONTENT, (WPARAM)IndiceUniversal, (LPARAM)0);
            EndDialog(hwnd, ID_DeleteClientWnd);
        }
            break;
        }
        break;
    }
    return FALSE;
}

//Proc de pantalla de eliminación de promociones
BOOL CALLBACK BorrarPro(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND ListaBorrarPro = GetDlgItem(hwnd, ID_ListaBorrarPro);
        HWND FechaDelPro = GetDlgItem(hwnd, ID_FechaDelPro);
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Nombre: ");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)PROaux->nombre);
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Monto para que la promoción aplique: ");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(PROaux->monto).c_str());
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Porcentaje de descuento: ");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)to_wstring(PROaux->porcentaje).append(L"%").c_str());
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"\n");
        SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Estatus: ");
        if (PROaux->activa) {
            SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Activa");
        }
        else {
            SendMessage(ListaBorrarPro, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Inactiva");
        }
        Registro.wDay = PROaux->dia;
        Registro.wMonth = PROaux->mes;
        Registro.wYear = PROaux->año;
        SendMessage(FechaDelPro, MCM_SETCURSEL, (WPARAM)0, (LPARAM)&Registro);
    }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_Regresar:
            EndDialog(hwnd, ID_DeletePromotionWnd);
            break;
        case ID_Eliminar: {
            if (PROaux->activa) {
                NumPromocionesAct--;
            }
            else {
                NumPromocionesInact--;
            }
            PROMOCION* temp = PROaux;
            PROaux = PROog;
            //Si solo hay un cliente
            if (borrarog && PROog->sig == nullptr) {
                PROog = nullptr;
                PROaux = nullptr;
            }
            //Si es el primero y hay más clientes
            else if (borrarog && PROog->sig != nullptr) {
                PROog = PROog->sig;
                PROaux = nullptr;
            }
            else {
                while (PROaux->sig != nullptr) {
                    if (PROaux->sig == temp) {
                        break;
                    }
                    else {
                        PROaux = PROaux->sig;
                    }
                }
                PROaux->sig = temp->sig;
            }
            delete temp;
            SendMessage((HWND)ListaPro, (UINT)LB_DELETESTRING, (WPARAM)IndiceUniversal, (LPARAM)0);
            SendMessage((HWND)ListaInfoPro, (UINT)LB_RESETCONTENT, (WPARAM)IndiceUniversal, (LPARAM)0);
            EndDialog(hwnd, ID_DeletePromotionWnd);
        }
            break;
        }
        break;
    }
    return FALSE;
}

//Inicia gdiplus y se pinta la pfp seleccionada
//Si no hay ninguna seleccionada, se usa una por default
void OnPaint(HDC hdc, int x, int y, TCHAR path[MAX_PATH+1]) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    Image* DefaultPFP = new Image(DfPfp);
    Image* PFP_bitmap = new Bitmap(path);
    Graphics graphics(hdc);
    if (wcslen(path)!=0) {
        graphics.DrawImage(PFP_bitmap, x, y, 100, 100);
    }
    else {
        graphics.DrawImage(DefaultPFP, x, y, 100, 100);
    }
    delete DefaultPFP;
    delete PFP_bitmap;
}

//Se elije una imagen local para foto de perfil (pfp)
void UploadImg(HWND Father) {
    OPENFILENAME PfpImg;
    ZeroMemory(ImagePath, sizeof(ImagePath));
    ZeroMemory(&PfpImg, sizeof(OPENFILENAME));
    PfpImg.lStructSize = sizeof(PfpImg);
    PfpImg.hwndOwner = Father;
    PfpImg.lpstrFile = ImagePath;
    PfpImg.nMaxFile = sizeof(ImagePath);
    PfpImg.lpstrFilter = TEXT("JPG\0*.jpg\0PNG\0*.png\0BITMAP\0*.bmp\0ICON\0*.ico\0");
    PfpImg.nFilterIndex = 1;
    PfpImg.lpstrFileTitle = NULL;
    PfpImg.nMaxFileTitle = 0;
    PfpImg.lpstrInitialDir = NULL;
    PfpImg.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&PfpImg)) {
        InvalidateRect(Father, NULL, TRUE);
        UpdateWindow(Father);
    }
    else {
        MessageBox(NULL, TEXT("La imagen no pudo ser abierta"), TEXT("Error"), MB_ICONSTOP);
    }
}

//Se guarda la información en un archivo
void SaveInfo(HWND Father) {
    OPENFILENAME SaveFile;
    TCHAR FilePath[MAX_PATH + 1];
    ZeroMemory(FilePath, sizeof(FilePath));
    ZeroMemory(&SaveFile, sizeof(OPENFILENAME));
    SaveFile.lStructSize = sizeof(SaveFile);
    SaveFile.hwndOwner = Father;
    SaveFile.lpstrFile = FilePath;
    SaveFile.nMaxFile = sizeof(FilePath);
    SaveFile.lpstrFilter = TEXT("Todos los archivos\0*.*\0BIN\0*.bin\0");
    SaveFile.lpstrDefExt = TEXT("bin");
    SaveFile.nFilterIndex = 1;
    SaveFile.lpstrFileTitle = NULL;
    SaveFile.nMaxFileTitle = 0;
    SaveFile.lpstrInitialDir = NULL;
    SaveFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetSaveFileName(&SaveFile)) {
        WriteFile(SaveFile.lpstrFile);
    }
    else {
        MessageBox(NULL, TEXT("No se pudo guardar el archivo"), TEXT("Error"), MB_ICONERROR);
    }
}

//Se escribe toda la información en el archivo antes nombrado
void WriteFile(LPWSTR Path) {
    char filename[256];
    ZeroMemory(filename, sizeof(filename));
    wcstombs_s(NULL, filename, Path, 256);

    FILE* NewFile;
    fopen_s(&NewFile, filename, "w");
    int num = 0;
    if (USog != nullptr) {
        USaux = USog;
        while (USaux->sig != nullptr) {
            num++;
            USaux = USaux->sig;
        }
        num++;
        fwrite(&num, sizeof(int), 1, NewFile);
        USaux = USog;
        while (USaux->sig != nullptr) {
            if (wcslen(USaux->PFPpath) == 0) {
                wcscpy_s(USaux->PFPpath, L"DefaultPfp.jpg");
            }
            if (wcslen(USaux->nombre) == 0) {
                wcscpy_s(USaux->nombre, L"Nombre");
            }
            if (wcslen(USaux->alias) == 0) {
                wcscpy_s(USaux->alias, L"Alias");
            }
            fwrite(reinterpret_cast<char*>(USaux), sizeof(USUARIO), 1, NewFile);
            USaux = USaux->sig;
        }
        if (wcslen(USaux->PFPpath) == 0) {
            wcscpy_s(USaux->PFPpath, L"DefaultPfp.jpg");
        }
        if (wcslen(USaux->nombre) == 0) {
            wcscpy_s(USaux->nombre, L"Nombre");
        }
        if (wcslen(USaux->alias) == 0) {
            wcscpy_s(USaux->alias, L"Alias");
        }
        fwrite(reinterpret_cast<char*>(USaux), sizeof(USUARIO), 1, NewFile);
    }
    num = 0;
    if (CLIog != nullptr) {
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            num++;
            CLIaux = CLIaux->sig;
        }
        num++;
        fwrite(&num, sizeof(int), 1, NewFile);
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            fwrite(reinterpret_cast<char*>(CLIaux),sizeof(CLIENTE), 1, NewFile);
            //fwrite(CLIaux->nombre, sizeof(CLIaux->nombre), 1, NewFile);
            //fwrite(CLIaux->alias, sizeof(CLIaux->alias), 1, NewFile);
            //fwrite(CLIaux->telefono, sizeof(CLIaux->telefono), 1, NewFile);
            //fwrite(CLIaux->Email, sizeof(CLIaux->Email), 1, NewFile);
            //fwrite(CLIaux->comercio, sizeof(CLIaux->comercio), 1, NewFile);
            //fwrite(&CLIaux->dia, sizeof(int), 1, NewFile);
            //fwrite(&CLIaux->mes, sizeof(int), 1, NewFile);
            //fwrite(&CLIaux->año, sizeof(int), 1, NewFile);
            CLIaux = CLIaux->sig;
        }
        fwrite(reinterpret_cast<char*>(CLIaux), sizeof(CLIENTE), 1, NewFile);
        //fwrite(CLIaux->nombre, sizeof(CLIaux->nombre), 1, NewFile);
        //fwrite(CLIaux->alias, sizeof(CLIaux->alias), 1, NewFile);
        //fwrite(CLIaux->telefono, sizeof(CLIaux->telefono), 1, NewFile);
        //fwrite(CLIaux->Email, sizeof(CLIaux->Email), 1, NewFile);
        //fwrite(CLIaux->comercio, sizeof(CLIaux->comercio), 1, NewFile);
        //fwrite(&CLIaux->dia, sizeof(int), 1, NewFile);
        //fwrite(&CLIaux->mes, sizeof(int), 1, NewFile);
        //fwrite(&CLIaux->año, sizeof(int), 1, NewFile);
    }
    num = 0;
    if (PROog != nullptr) {
        PROaux = PROog;
        while (PROaux->sig != nullptr) {
            num++;
            PROaux = PROaux->sig;
        }
        num++;
        fwrite(&num, sizeof(int), 1, NewFile);
        PROaux = PROog;
        while (PROaux->sig != nullptr) {
            fwrite(reinterpret_cast<char*>(PROaux), sizeof(PROMOCION), 1, NewFile);
            PROaux = PROaux->sig;
        }
        fwrite(reinterpret_cast<char*>(PROaux), sizeof(PROMOCION), 1, NewFile);
    }
    num = 0;
    if (CONog != nullptr) {
        CONaux = CONog;
        while (CONaux->sig != nullptr) {
            num++;
            CONaux = CONaux->sig;
        }
        num++;
        fwrite(&num, sizeof(int), 1, NewFile);
        CONaux = CONog;
        while (CONaux->sig != nullptr) {
            fwrite(reinterpret_cast<char*>(CONaux), sizeof(CONSUMO), 1, NewFile);
            CONaux =CONaux->sig;
        }
        fwrite(reinterpret_cast<char*>(CONaux), sizeof(CONSUMO), 1, NewFile);
    }
    fclose(NewFile);
}

//Se carga el archivo de usuarios
void LoadInfo(HWND Father) {
    OPENFILENAME LoadFile;
    TCHAR FilePath[MAX_PATH + 1];
    ZeroMemory(FilePath, sizeof(FilePath));
    ZeroMemory(&LoadFile, sizeof(OPENFILENAME));
    LoadFile.lStructSize = sizeof(LoadFile);
    LoadFile.hwndOwner = Father;
    LoadFile.lpstrFile = FilePath;
    LoadFile.nMaxFile = sizeof(FilePath);
    LoadFile.lpstrFilter = TEXT("BIN\0*.bin\0");
    LoadFile.nFilterIndex = 1;
    LoadFile.lpstrFileTitle = NULL;
    LoadFile.nMaxFileTitle = 0;
    LoadFile.lpstrInitialDir = NULL;
    LoadFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&LoadFile)) {
        LoadInfoFile(LoadFile.lpstrFile);
    }
    else {
        MessageBox(NULL, TEXT("El archivo no se pudo abrir"), TEXT("Error"), MB_ICONSTOP);
    }
}

//Se cargan usuarios del archivo antes nombrado
void LoadInfoFile(LPWSTR Path) {
    char filename[256];
    ZeroMemory(filename, sizeof(filename));
    wcstombs_s(NULL, filename, Path, 256);

    FILE* File;
    fopen_s(&File, filename, "r");

    int num = 0;
    USUARIO* tempUS = new USUARIO;
    fread(&num, sizeof(int), 1, File);
    if (num != 0) {
        USog = new USUARIO;
        USaux = USog;
        for (int i = 0; i < num; i++) {
            //ZeroMemory(temp, sizeof(temp));
            fread(reinterpret_cast<char*>(tempUS), sizeof(USUARIO), 1, File);
            if (_tcslen(tempUS->PFPpath) != 0) {
                _tcscpy_s(USaux->PFPpath, (sizeof(tempUS->PFPpath)) / 2, tempUS->PFPpath);
            }
            wcscpy_s(USaux->user, (sizeof(tempUS->user)) / 2, tempUS->user);
            wcscpy_s(USaux->password, (sizeof(tempUS->password) + 1) / 2, tempUS->password);
            wcscpy_s(USaux->nombre, (sizeof(tempUS->nombre)) / 2, tempUS->nombre);
            wcscpy_s(USaux->alias, (sizeof(tempUS->alias)) / 2, tempUS->alias);
            wcscpy_s(USaux->comercio, (sizeof(tempUS->comercio)) / 2, tempUS->comercio);
            USaux->isAdmin = tempUS->isAdmin;
            USaux->isOwner = tempUS->isOwner;
            USaux->PrimVez = tempUS->PrimVez;
            if (i + 1 == num) {
                USaux->sig = nullptr;
            }
            else {
                USaux->sig = new USUARIO;
                USaux = USaux->sig;
            }
        }
    }
    delete tempUS;

    num = 0;
    CLIENTE* tempCLI = new CLIENTE;
    fread(&num, sizeof(int), 1, File);
    if (num != 0) {
        CLIog = new CLIENTE;
        CLIaux = CLIog;
        for (int i = 0; i < num; i++) {
            ZeroMemory(tempCLI, sizeof(tempCLI));
            fread(reinterpret_cast<char*>(tempCLI), sizeof(CLIENTE), 1, File);
            wcscpy_s(CLIaux->nombre, (sizeof(tempCLI->nombre)) / 2, tempCLI->nombre);
            wcscpy_s(CLIaux->alias, (sizeof(tempCLI->alias)) / 2, tempCLI->alias);
            wcscpy_s(CLIaux->comercio, (sizeof(tempCLI->comercio)) / 2, tempCLI->comercio);
            wcscpy_s(CLIaux->telefono, (sizeof(tempCLI->telefono)) / 2, tempCLI->telefono);
            wcscpy_s(CLIaux->Email, (sizeof(tempCLI->Email)) / 2, tempCLI->Email);
            CLIaux->dia = tempCLI->dia;
            CLIaux->mes = tempCLI->mes;
            CLIaux->año = tempCLI->año;

            if (i + 1 == num) {
                CLIaux->sig = nullptr;
            }
            else {
                CLIaux->sig = new CLIENTE;
                CLIaux = CLIaux->sig;
            }
        }
    }
    delete tempCLI;

    num = 0;
    PROMOCION* tempPRO = new PROMOCION;
    fread(&num, sizeof(int), 1, File);
    if (num != 0) {
        PROog = new PROMOCION;
        PROaux = PROog;
        for (int i = 0; i < num; i++) {
            
            fread(reinterpret_cast<char*>(tempPRO), sizeof(PROMOCION), 1, File);
            wcscpy_s(PROaux->nombre, (sizeof(tempPRO->nombre)) / 2, tempPRO->nombre);
            wcscpy_s(PROaux->comercio, (sizeof(tempPRO->comercio)) / 2, tempPRO->comercio);
            PROaux->monto = tempPRO->monto;
            PROaux->porcentaje = tempPRO->porcentaje;
            PROaux->activa = tempPRO->activa;
            PROaux->dia = tempPRO->dia;
            PROaux->mes = tempPRO->mes;
            PROaux->año = tempPRO->año;
            if (i + 1 == num) {
                PROaux->sig = nullptr;
            }
            else {
                PROaux->sig = new PROMOCION;
                PROaux = PROaux->sig;
            }
        }
    }
    delete tempPRO;
    
    num = 0;
    CONSUMO* tempCON = new CONSUMO;
    fread(&num, sizeof(int), 1, File);
    if (num != 0) {
        CONog = new CONSUMO;
        CONaux = CONog;
        for (int i = 0; i < num; i++) {
            //ZeroMemory(temp, sizeof(temp));
            fread(reinterpret_cast<char*>(tempCON), sizeof(CONSUMO), 1, File);
            wcscpy_s(CONaux->cliente, (sizeof(tempCON->cliente)) / 2, tempCON->cliente);
            wcscpy_s(CONaux->comercio, (sizeof(tempCON->comercio)) / 2, tempCON->comercio);
            CONaux->monto = tempCON->monto;
            CONaux->promocion = tempCON->promocion;
            CONaux->subtotal = tempCON->subtotal;
            CONaux->total = tempCON->total;
            CONaux->dia = tempCON->dia;
            CONaux->mes = tempCON->mes;
            CONaux->año = tempCON->año;
            if (i + 1 == num) {
                CONaux->sig = nullptr;
            }
            else {
                CONaux->sig = new CONSUMO;
                CONaux = CONaux->sig;
            }
        }
    }
    delete tempCON;
    fclose(File);
}

//Antes de cargar un archivo, se borra todo lo previamente introducido (si es que hay)
void DeleteEverything() {
    if (USog != nullptr) {
        USaux = USog;
        while (USaux->sig != nullptr) {
            USUARIO* morido = USaux;
            USaux = USaux->sig;
            USog = USog->sig;
            delete morido;
        }
        USUARIO* morido = USaux;
        USog = nullptr;
        USaux = nullptr;
        delete morido;
    }
    if (CLIog != nullptr) {
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            CLIENTE* morido = CLIaux;
            CLIaux = CLIaux->sig;
            CLIog = CLIog->sig;
            delete morido;
        }
        CLIENTE* morido = CLIaux;
        CLIog = nullptr;
        CLIaux = nullptr;
        delete morido;
    }
    if (PROog != nullptr) {
        PROaux = PROog;
        while (PROaux->sig != nullptr) {
            PROMOCION* morido = PROaux;
            PROaux = PROaux->sig;
            PROog = PROog->sig;
            delete morido;
        }
        PROMOCION* morido = PROaux;
        PROog = nullptr;
        PROaux = nullptr;
        delete morido;
    }
    if (CONog != nullptr) {
        CONaux = CONog;
        while (CONaux->sig != nullptr) {
            CONSUMO* morido = CONaux;
            CONaux = CONaux->sig;
            CONog = CONog->sig;
            delete morido;
        }
        CONSUMO* morido = CONaux;
        CONog = nullptr;
        CONaux = nullptr;
        delete morido;
    }
}

//Borra toda la información de un comercio
void DeleteCom(const wchar_t Comercio[16]) {
    if (USog != nullptr) {
        USUARIO* maintemp = USog;
        while (maintemp->sig != nullptr) {
            if (!maintemp->isOwner && wcscmp(Comercio, maintemp->comercio) == 0) {
                USUARIO* temp = maintemp, *temp2 = USog;
                while (temp2->sig != nullptr) {
                    if (temp2->sig == temp) {
                        break;
                    }
                    else {
                        temp2 = temp2->sig;
                    }
                }
                temp2->sig = temp->sig;
                maintemp = USog;
                delete temp;
                temp2 = nullptr;
                delete temp2;
            }
            maintemp = maintemp->sig;
        }
        if (!maintemp->isOwner && wcscmp(Comercio, maintemp->comercio) == 0) {
            USUARIO* temp = maintemp, * temp2 = USog;
            while (temp2->sig != nullptr) {
                if (temp2->sig == temp) {
                    break;
                }
                else {
                    temp2 = temp2->sig;
                }
            }
            temp2->sig = temp->sig;
            delete temp;
            temp2 = nullptr;
            delete temp2;
        }
        maintemp = nullptr;
        delete maintemp;
    }
    if (CLIog != nullptr) {
        CLIaux = nullptr;
        CLIENTE* maintemp = CLIog;
        while (maintemp->sig != nullptr) {
            if (wcscmp(Comercio, maintemp->comercio) == 0) {
                CLIENTE* temp = maintemp, * temp2 = CLIog;
                if (temp == temp2) {
                    CLIog = CLIog->sig;
                }
                else {
                    while (temp2->sig != nullptr) {
                        if (temp2->sig == temp) {
                            break;
                        }
                        else {
                            temp2 = temp2->sig;
                        }
                    }
                    temp2->sig = temp->sig;
                }
                maintemp = CLIog;
                temp2 = nullptr;
                delete temp;
                delete temp2;
                if (CLIog->sig == nullptr) {
                    break;
                }
            }
            maintemp = maintemp->sig;
        }
        if (wcscmp(Comercio, maintemp->comercio) == 0) {
            CLIENTE* temp = maintemp, * temp2 = CLIog;
            if (temp == temp2) {
                CLIog = CLIog->sig;
            }
            else {
                while (temp2->sig != nullptr) {
                    if (temp2->sig == temp) {
                        break;
                    }
                    else {
                        temp2 = temp2->sig;
                    }
                }
                temp2->sig = temp->sig;
            }
            temp2 = nullptr;
            delete temp;
            delete temp2;
        }
        maintemp = nullptr;
        delete maintemp;
    }
    if (PROog != nullptr) {
        PROaux = nullptr;
        PROMOCION* maintemp = PROog;
        while (maintemp->sig != nullptr) {
            if (wcscmp(Comercio, maintemp->comercio) == 0) {
                PROMOCION* temp = maintemp, * temp2 = PROog;
                if (temp == temp2) {
                    PROog = PROog->sig;
                }
                else {
                    while (temp2->sig != nullptr) {
                        if (temp2->sig == temp) {
                            break;
                        }
                        else {
                            temp2 = temp2->sig;
                        }
                    }
                    temp2->sig = temp->sig;
                }
                maintemp = PROog;
                temp2 = nullptr;
                delete temp;
                delete temp2;
                if (PROog->sig == nullptr) {
                    break;
                }
            }
            maintemp = maintemp->sig;
        }
        if (wcscmp(Comercio, maintemp->comercio) == 0) {
            PROMOCION* temp = maintemp, * temp2 = PROog;
            if (temp == temp2) {
                PROog = PROog->sig;
            }
            else {
                while (temp2->sig != nullptr) {
                    if (temp2->sig == temp) {
                        break;
                    }
                    else {
                        temp2 = temp2->sig;
                    }
                }
                temp2->sig = temp->sig;
            }
            temp2 = nullptr;
            delete temp;
            delete temp2;
        }
        maintemp = nullptr;
        delete maintemp;
    }
    if (CONog != nullptr) {
        CONaux = nullptr;
        CONSUMO* maintemp = CONog;
        while (maintemp->sig != nullptr) {
            if (wcscmp(Comercio, maintemp->comercio) == 0) {
                CONSUMO* temp = maintemp, * temp2 = CONog;
                if (temp == temp2) {
                    CONog = CONog->sig;
                }
                else {
                    while (temp2->sig != nullptr) {
                        if (temp2->sig == temp) {
                            break;
                        }
                        else {
                            temp2 = temp2->sig;
                        }
                    }
                    temp2->sig = temp->sig;
                }
                maintemp = CONog;
                temp2 = nullptr;
                delete temp;
                delete temp2;
                if (CONog->sig == nullptr) {
                    break;
                }
            }
            maintemp = maintemp->sig;
        }
        if (wcscmp(Comercio, maintemp->comercio) == 0) {
            CONSUMO* temp = maintemp, * temp2 = CONog;
            if (temp == temp2) {
                CONog = CONog->sig;
            }
            else {
                while (temp2->sig != nullptr) {
                    if (temp2->sig == temp) {
                        break;
                    }
                    else {
                        temp2 = temp2->sig;
                    }
                }
                temp2->sig = temp->sig;
            }
            temp2 = nullptr;
            delete temp;
            delete temp2;
        }
        maintemp = nullptr;
        delete maintemp;
    }
}

//Funcion de registro de nuevo usuario
//Regresa true si se registro correctamente, regresa false si no
bool RegistrarUs(bool CheckCom) {
    if (USog == nullptr) {
        USog = new USUARIO;
        USog->sig = nullptr;
        USaux = USog;
        ZeroMemory(USaux->PFPpath, sizeof(USaux->PFPpath));
        ZeroMemory(USaux->alias, sizeof(USaux->alias));
        ZeroMemory(USaux->comercio, sizeof(USaux->comercio));
        ZeroMemory(USaux->nombre, sizeof(USaux->nombre));
        ZeroMemory(USaux->password, sizeof(USaux->password));
        ZeroMemory(USaux->user, sizeof(USaux->user));
    }
    else {
        USaux = USog;
        while (USaux->sig != nullptr) {
            USaux = USaux->sig;
        }
        USaux->sig = new USUARIO;
        USaux = USaux->sig;
        USaux->sig = nullptr;
        ZeroMemory(USaux->PFPpath, sizeof(USaux->PFPpath));
        ZeroMemory(USaux->alias, sizeof(USaux->alias));
        ZeroMemory(USaux->comercio, sizeof(USaux->comercio));
        ZeroMemory(USaux->nombre, sizeof(USaux->nombre));
        ZeroMemory(USaux->password, sizeof(USaux->password));
        ZeroMemory(USaux->user, sizeof(USaux->user));
    }
    bool cerrar = true;
    bool* borrar = new bool;
    *borrar = false;
    GetWindowText(ed_pass2, USaux->password, sizeof(USaux->password) / 2);
    wchar_t confirm[11];
    ZeroMemory(confirm, sizeof(confirm));
    GetWindowText(ed_confirm, confirm, sizeof(confirm) / 2);

    //Confirma si lo introducido en "Confirmar contraseña" es la misma contraseña
    //Si se quiere revisar el comercio, se checa si ya existe, 
    //sino, se copia el del comercio actual automaticamente
    //Finalmente se checa si el nombre de usuario ya esta registrado
    if (wcscmp(confirm, USaux->password) == 0) {
        wchar_t us[16] = L"";
        if (CheckCom) {
            GetWindowText(ed_UScom, USaux->comercio, sizeof(USaux->comercio) / 2);
            if (MismoComUs(USaux) == false) {
                GetWindowText(ed_user2, us, sizeof(us) / 2);
                if (wcslen(us) == 0) {
                    MessageBox(NULL, (LPCWSTR)L"El nombre de usuario no puede estar vacio", (LPCWSTR)L"Error", MB_ICONWARNING);
                    *borrar = true;
                }
                if (UserExists(us, false) == true & !*borrar) {
                    MessageBox(NULL, (LPCWSTR)L"Este usuario ya esta registrado", (LPCWSTR)L"Error", MB_ICONWARNING);
                    *borrar = true;
                }
                else {
                    GetWindowText(ed_user2, USaux->user, sizeof(USaux->user) / 2);
                    USaux->isAdmin = true;
                }
            }
            else {
                MessageBox(NULL, (LPCWSTR)L"Este comercio ya esta registrado, por favor introduce otro", (LPCWSTR)L"Error", MB_ICONWARNING);
                *borrar = true;
            }
        }
        else {
            GetWindowText(ed_user2, us, sizeof(us) / 2);
            if (wcslen(us) == 0) {
                MessageBox(NULL, (LPCWSTR)L"El nombre de usuario no puede estar vacio", (LPCWSTR)L"Error", MB_ICONWARNING);
                *borrar = true;
            }
            if (UserExists(us, false) == true && !*borrar) {
                MessageBox(NULL, (LPCWSTR)L"Este usuario ya esta registrado", (LPCWSTR)L"Error", MB_ICONWARNING);
                *borrar = true;
            }
            else {
                GetWindowText(ed_user2, USaux->user, sizeof(USaux->user) / 2);
                wcscpy_s(USaux->comercio, SesIn->comercio);
            }
        }
    }
    else {
        MessageBox(NULL, (LPCWSTR)L"La contraseña no se confirmo correctamente", (LPCWSTR)L"Error", MB_ICONERROR);
        *borrar = true;
    }

    //Procesos de validación (Perdon por la masiva cantidad de ifs xd, queria 
    //que solo apareciera una advertencia a la vez, en vez de que se envien multiples)

    //Validación de usuarios (Solo letras, no puede estar vacio, maximo 15)
    if (!*borrar && wcslen(USaux->user) == 0) {
        MessageBox(NULL, (LPCWSTR)L"El nombre de usuario no puede estar en blanco", (LPCWSTR)L"Error", MB_ICONERROR);
        *borrar = true;
    }
    if (!*borrar) {
        wstring CheckUsername;
        bool hasonlylet = true;
        CheckUsername.clear();
        CheckUsername.append(USaux->user);
        //Busca si tiene caracteres especiales o numeros
        for (int i = 32; i < 127; i++) {
            switch (i) {
            case 65: //Ignorar mayusculas
                i = 91;
                break;

            case 97: //Ignorar minusculas
                i = 123;
                break;
            }
            if (CheckUsername.find(i) != wstring::npos) {
                hasonlylet = false;
            }
        }
        if (!hasonlylet) {
            MessageBox(NULL, (LPCWSTR)L"El nombre de usuario debe usar solo letras", (LPCWSTR)L"Error", MB_ICONERROR);
            *borrar = true;
        }
    }

    //Validación de comercios (Caracteres alfanumericos y/o espacios, 5 a 15 caracteres)
    if (!*borrar && wcslen(USaux->comercio) < 5) {
        MessageBox(NULL, (LPCWSTR)L"El nombre de comercio debe tener entre 5 a 15 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
        *borrar = true;
    }
    if (!*borrar) {
        wstring CheckComercio;
        bool hasonlyalfesp = true; // (has only alfanumeric/space)
        CheckComercio.clear();
        CheckComercio.append(USaux->comercio);
        for (int i = 33; i < 127; i++) {
            switch (i) {
            case 48: //Ignorar numeros
                i = 58;
                break;

            case 65: //Ignorar mayusculas
                i = 91;
                break;

            case 97: //Ignorar minusculas
                i = 123;
                break;
            }
            if (CheckComercio.find(i) != wstring::npos) {
                hasonlyalfesp = false;
            }
        }
        if (!hasonlyalfesp) {
            MessageBox(NULL, (LPCWSTR)L"El nombre del comercio debe usar solo caracteres alfanumericos y/o espacios", (LPCWSTR)L"Error", MB_ICONERROR);
            *borrar = true;
        }
    }

    //Validación de contraseñas (Minimo 1 letra, 1 caracter especial y 1 numero, 3 a 10 caracteres)
    if (!*borrar && wcslen(USaux->password) < 3) {
        MessageBox(NULL, (LPCWSTR)L"La contraseña debe de tener 3 a 10 caracteres", (LPCWSTR)L"Error", MB_ICONERROR);
        *borrar = true;
    }
    if (!*borrar && ValidatePass(USaux->password) == false) {
        MessageBox(NULL, (LPCWSTR)L"La contraseña es invalida", (LPCWSTR)L"Error", MB_ICONERROR);
        *borrar = true;
    }

    //Si la información del usuario no es valida, se borra
    if (*borrar) {
        cerrar = false;
        USUARIO* temp = USaux;
        if (temp == USog) {
            USog = nullptr;
            USaux = nullptr;
        }
        else {
            USUARIO* temp2 = USog;
            while (temp2->sig != USaux) {
                temp2 = temp2->sig;
            }
            temp2->sig = nullptr;
            temp2 = nullptr;
            USaux = nullptr;
            delete temp2;
        }
        delete temp;
    }
    delete borrar;

    //Si un usuario se registro correctamente, se cierra la pantalla de registro
    if (cerrar) {
        return true;
    }
    else {
        return false;
    }
}

//Confirma si un comercio ya existe
bool MismoComUs(USUARIO* info) {
    USUARIO* temp = new USUARIO;
    temp = USog;
    bool MismoComercio = false;
    if (info == temp) {
        MismoComercio = false;
    }
    else {
        while (temp->sig != nullptr) {
            if (wcscmp(temp->comercio, info->comercio) == 0 && info != temp) {
                MismoComercio = true;
                break;
            }
            else {
                temp = temp->sig;
            }
        }
        if (wcscmp(temp->comercio, info->comercio) == 0 && info != temp) {
            MismoComercio = true;
        }
    }
    temp = nullptr;
    delete temp;
    if (MismoComercio) {
        return true;
    }
    else {
        return false;
    }
}

//Confirma si un usuario ya existe, si RewriteAux es verdadero se asignara un usuario a USaux
bool UserExists(const wchar_t CompareUs[16], bool RewriteAux) {
    USUARIO* temp;
    bool existe = false;
    temp = USog;
    while (temp->sig != nullptr) {
        if (wcscmp(CompareUs, temp->user) == 0) {
            existe = true;
            break;
        }
        temp = temp->sig;
    }
    if (wcscmp(CompareUs, temp->user) == 0) {
        existe = true;
    }
    if (RewriteAux) {
        USaux = temp;
    }
    temp = nullptr;
    delete temp;
    if (existe) {
        return true;
    }
    else {
        return false;
    }
}

//Confirma si una contraseña es valida
bool ValidatePass(const wchar_t Password[11]) {
    wstring CheckPass;
    bool hasspec = false, hasnum = false, haslet = false;
    CheckPass.clear();
    CheckPass.append(Password);
    for (int i = 32; i < 177; i++) {
        switch (i) {
        case 48:
            i = 58;
            break;

        case 65:
            i = 91;
            break;

        case 97:
            i = 123;
            break;
        }
        if (CheckPass.find(i) != wstring::npos) {
            hasspec = true;
        }
    }
    for (int i = 48; i < 58; i++) {
        if (CheckPass.find(i) != wstring::npos) {
            hasnum = true;
        }
    }
    for (int i = 65; i < 123; i++) {
        if (i == 91) {
            i = 97;
        }
        if (CheckPass.find(i) != wstring::npos) {
            haslet = true;
        }
    }
    if (haslet && hasnum && hasspec) {
        return true;
    }
    else {
        return false;
    }
}

//Confirma si un email es valido
bool ValidateEmail(const wchar_t Mail[26]) {
    wstring CheckMail;
    bool valid = true;
    CheckMail.clear();
    CheckMail.append(Mail);
    if (Mail[0] == L'.' || CheckMail.find(L"@.") != wstring::npos || CheckMail.find(L'.') == wstring::npos || CheckMail.find(L' ') != wstring::npos) {
        valid = false;
    }
    int NumArr = 0;
    for (int i = 0; i < wcslen(Mail); i++) {
        if (Mail[i] == L'@') {
            NumArr++;
        }
    }
    if (NumArr != 1) {
        valid = false;
    }
    if (CheckMail.find(L".com") == wstring::npos && CheckMail.find(L".edu") == wstring::npos && CheckMail.find(L".org") == wstring::npos && CheckMail.find(L".gob") == wstring::npos && CheckMail.find(L".net") == wstring::npos) {
        valid = false;
    }
    //Busca si tiene caracteres especiales o numeros
    for (int i = 32; i < 127; i++) {
        switch (i) {
        case 65: //Ignorar mayusculas
            i = 91;
            break;

        case 97: //Ignorar minusculas
            i = 123;
            break;
        }
        if (CheckMail.back() == i) {
            valid = false;
        }
    }
    if (valid) {
        return true;
    }
    else {
        return false;
    }
}

//Cuenta ciertos parametros del comercio para que aparezcan en la pantalla principal
void CountAll() {
    NumClientes = 0, NumPromocionesAct = 0, NumPromocionesInact = 0, NumConsumos = 0, NumConsumosMonto = 0;
    if (CLIog != nullptr) {
        CLIaux = CLIog;
        while (CLIaux->sig != nullptr) {
            if (wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
                NumClientes++;
            }
            CLIaux = CLIaux->sig;
        }
        if (wcscmp(SesIn->comercio, CLIaux->comercio) == 0) {
            NumClientes++;
        }
    }
    if (PROog != nullptr) {
        PROaux = PROog;
        while (PROaux->sig != nullptr) {
            if (wcscmp(SesIn->comercio, PROaux->comercio) == 0) {
                if (PROaux->activa) {
                    NumPromocionesAct++;
                }
                else {
                    NumPromocionesInact++;
                }
            }
            PROaux = PROaux->sig;
        }
        if (wcscmp(SesIn->comercio, PROaux->comercio) == 0) {
            if (PROaux->activa) {
                NumPromocionesAct++;
            }
            else {
                NumPromocionesInact++;
            }
        }
    }
    if (CONog != nullptr) {
        CONaux = CONog;
        while (CONaux->sig != nullptr) {
            if (wcscmp(SesIn->comercio, CONaux->comercio) == 0) {
                NumConsumos++;
                NumConsumosMonto = NumConsumosMonto + CONaux->total;
            }
            CONaux = CONaux->sig;
        }
        if (wcscmp(SesIn->comercio, CONaux->comercio) == 0) {
            NumConsumos++;
            NumConsumosMonto = NumConsumosMonto + CONaux->total;
        }
    }
}