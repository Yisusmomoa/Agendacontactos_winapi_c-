#include <windows.h>
#include <CommDlg.h>   //Permite abrir archivos
#include <fstream>     
#include <string>
#include<iostream>
#include<conio.h>
#include <stdlib.h>
#include"resource.h"
#include<ctype.h>// de aquí viene la funcion isalpha
HWND ghDlg;
HINSTANCE H_DIALOG;
using namespace std;
struct datospersona
{
	char primernombre[20];
	char segundonombre[20];
	char apellidopaterno[20];
	char apellidomaterno[20];
};
struct datosdireccion
{
	char calle[_MAX_PATH];
	char numero[5];
	char CP[10];
	char colonia[30];
	char municipio[30];
	char estado[30];
	char pais[20];
	char entrecalle1[_MAX_PATH];
};
struct contacto
{
	int id = 0; 
	char numerotelefonico[30];
	char email[_MAX_PATH];
	char nombreamayus[90]; //Función strupr()
	char imagen1[_MAX_PATH];
	char imagen2[_MAX_PATH];
	bool repetido = false;
	datospersona nombre;
	datosdireccion direccion;
	contacto*ant, *sig;
};
contacto*ini = 0, *nuevo, *auxC = 0, *auxC2=0;
char  full[4];
long longitudtelefono, longitudemail, longitudprimernombre;
bool sitiene12 = false, sitienea = false, sitienecom = false, busca = false, encontrado = false, sigimg, StieneNumeros = false, SitienenumerosCP=false, SitienenumerosDireccion=false;
string cut, aux, cortanombre;
contacto apoyo;
CHAR ArchContactos[_MAX_PATH] = "C:\\Users\\hp\\source\\repos\\ProyPrograDelta\\ProyPrograDelta\\Contactos.bin";
char Archcolores[_MAX_PATH] = "C:\\Users\\hp\\source\\repos\\ProyPrograDelta\\ProyPrograDelta\\colores.txt";
char direimgaen[_MAX_PATH] = "C:\\Users\\hp\\source\\repos\\ProyPrograDelta\\ProyPrograDelta\\3x3x5crazy2.bmp"; 
int wmId, wmEvent;
PAINTSTRUCT ps;
HDC hdc;
RECT clientrect;
RECT textrect;
HRGN bgRgn;
HBRUSH hBrush;
HPEN hpen;
int R = 255, G = 255, B = 255;
//FUNCIONES DE LAS CALLBACK(VENTANAS)
BOOL CALLBACK call_Dialogo1(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //tu pantalla principal
BOOL CALLBACK call_Dialogo2(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //tu pantalla agregar
BOOL CALLBACK call_Dialogo3(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //ventana busqueda
BOOL CALLBACK call_Dialogo4(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //pantalla modificar
BOOL CALLBACK call_Dialogo5(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //pantalla carrusel
BOOL CALLBACK call_Dialogo6(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam); //pantalla manual
//ARCHIVOS E IMAGENES
void GetFile(HWND Dialog, int ID, int Tipo)
{
	char Filtro[MAX_PATH] = "";
	strcat(Filtro, "Todos\\0*.*\\0Archivos ");
	//strcat(Filtro, Tipo);
	strcat(Filtro, "\\0*.");
	//strcat(Filtro, Tipo);
	strcat(Filtro, "\\0");
	LPCSTR filtro;
	if (Tipo == 1)
		filtro = "Todos\0*.*\0Imagenes BMP\0*.bmp\0Imagenes jpg\0*.jpg";
	//filtro = Filtro;
	char szFile[MAX_PATH];  // buffer for file name
	OPENFILENAME ofn;       // common dialog box structure
	HANDLE hf;              // file handle
							// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Dialog;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	/*ofn.lpstrFilter = filtro;*/ //SI QUITAS EL COMENTARIO EN ESTA LINEA DEJA DE JALAR Y NOSE PORQUE
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn) == TRUE)
		SetWindowText(GetDlgItem(Dialog, ID), ofn.lpstrFile);
	else
		MessageBox(0, "No eligió archivo", "Aviso", MB_OK | MB_ICONINFORMATION);
}
void llenabmp(HWND foto, char*filtro, int ancho, int alto) {
	static HBITMAP bmp1, bmp2;
	bmp1 = (HBITMAP)SendMessage(foto, STM_GETIMAGE, IMAGE_BITMAP, 0);
	bmp2 = (HBITMAP)LoadImage(NULL, filtro, IMAGE_BITMAP, ancho, alto, LR_LOADFROMFILE);
	SendMessage(foto, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
}
void vaciabmp(HWND foto, char *filtro, int ID, int tipo) {
	static HBITMAP bmp1, bmp2;
	bmp1 = (HBITMAP)SendMessage(foto, STM_SETIMAGE, 0, 0);
}
void escribecontactos() {
	ofstream FileContactos(ArchContactos, ios::trunc | ios::binary);
	if (FileContactos.is_open())
	{
		auxC = ini;
		while (auxC != NULL)
		{
			FileContactos.write((char*)auxC, sizeof(contacto));
			auxC = auxC->sig;
		}
		FileContactos.close();
	}
}
void Leecontacots() {
	ifstream Filecontactos;
	Filecontactos.open(ArchContactos, ios::binary);
	if (Filecontactos.is_open())
	{
		nuevo = 0;
		nuevo = new contacto;
		Filecontactos.read((char*)nuevo, sizeof(contacto));
		nuevo->ant = 0;
		nuevo->sig = 0;
		while (!Filecontactos.eof())
		{
			if (ini == 0)
			{
				ini = nuevo;
			}
			else
			{
				auxC = ini;
				while (auxC->sig != 0)
				{
					auxC = auxC->sig;
				}
				auxC->sig = nuevo;
				nuevo->ant = auxC;
				nuevo->sig = 0;
			}
			nuevo = new contacto;
			Filecontactos.read((char*)nuevo, sizeof(contacto));
		}
		Filecontactos.close();
	}
}
//VALIDACIONES, BUSQUEDA, ELIMINACION DE LA LISTA LIGADA
bool Buscacontacto(char buscalo[]) {//Recibe como parametro el nombre a buscar
									//Aqui se usan las tres legendarias lineas
	auxC = ini; //El puntero auxiliar(auxC) apunta al mismo lugar donde apunta ini osea al apunta al inicio de la lista
	while (auxC != NULL) // se abre el ciclo que se repetira hasta el ultimo nodo
	{
		if (!strcmp(auxC->nombre.primernombre, buscalo))//se compara el nombre del nodo actual con el que se mando por parametro
		{
			return true; //si lo encuentra regresara un verdadero y se saldra de esta función
			//break;
		}
		auxC = auxC->sig;//Hace que el puntero se mueva al nodo sig  
	}
	return false;// Si no lo encuentra regresara un falso
}
contacto*buscame(int i) {
	auxC = ini; 
	while (auxC!=0)
	{
		if (auxC->id==i)
		{
			return auxC; 
		}
		auxC = auxC->sig; 
	}
}
contacto*buscame2(char bs[]) {
	auxC = ini;
	while (auxC!=NULL)
	{
		if (strcmp(auxC->nombreamayus, bs)==0)
		{
			return auxC; 
		}
		auxC = auxC->sig; 
	}
}
void borracontacto(contacto *auxC) {
	if (auxC->sig == NULL && auxC->ant == NULL)//si solo hay un nodo
	{
		ini = NULL;
		delete auxC;
	}
	else
	{
		if (auxC->ant == NULL)//si es el primero de la lista
		{
			ini = auxC->sig;
			auxC->sig->ant = NULL;
			delete auxC;
		}
		else
		{
			if (auxC->sig == NULL)// si es el ultimo nodo de la lista
			{
				/*fin = auxC->ant;
				auxC->ant->sig = NULL;
				auxC->ant = NULL;*/
				auxC = auxC->ant; 
				auxC->sig->ant = 0; 
				delete auxC->sig;
				auxC->sig = 0;
			}
			else//si esta en medio de dos nodos
			{
				auxC->ant->sig = auxC->sig;
				auxC->sig->ant = auxC->ant;
				delete auxC;
			}
		}
	}
}
bool Buscarepetidoemail(char emailabuscar[]) {
	//es exactamente lo mismo que la de buscacontacto
	auxC = ini;
	while (auxC != NULL)
	{
		if (!strcmp(auxC->email, emailabuscar))
		{
			return true;
			//break;
		}
		auxC = auxC->sig;
	}
	return false;
}
bool tiene12(long auxlongitudnumero, HWND dlg) {//recibe como parametros una variable tipo long que almacena la longitud del telefono y la variable hwnd de la ventana agregar
	if (auxlongitudnumero < 8)//Si la longitud del telefno es maenor que 8 regresara un falso
	{
		MessageBox(dlg, "¡Pon un Numero Telefonico mayor a 8 digitos!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false;
	}
	else if (auxlongitudnumero > 12)//si la longitud del telefono es mayor que 12 regresara un falso
	{
		MessageBox(dlg, "¡Pon un Numero Telefonico menor a 12 digitos!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
bool tieneaycom(int auxlongitudemail, char auxemail[], HWND dlg){//recibe como parametros la longitud del correo, un arreglo que contiene el correo y la hwnd de la ventana agregar
	for (size_t i = 0; i < auxlongitudemail; i++)//se abre un for que se repetira una cantidad N de veces, donde N es: la longitud del correo
	{
		if (auxemail[i]=='@')//se compara cada caracter del arreglo con un @
		{
			sitienea = true; //Si entra aquí es que si hay un caracter que sea @ 
		}
	}
	cut = auxemail; //cut es un string y se iguala al arreglo char que contiene el correo
	cut = cut.substr(auxlongitudemail - 4, 4); //se sustraen los ultimos 4 digitos de la cadena para saber si es .com 
	if (cut==".com")//se compara la cadena cut que ahora contiene lo que se sustrajo de la misma
	{
		sitienecom = true; //si entra aquí es que si cuenta con .com
	}
	if (!sitienea||!sitienecom)//si entra aqui es porque una de las dos cosas NO se cumplio. Sea el @ o el .com
	{
		MessageBox(dlg, "¡Olvidaste poner @ o .com en tu email !", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false; //regresara un falso y pondra el mensaje
	}
	return true; 
}
bool tienenumeros(char auxnumero[], HWND dlg) {//se envia como parametro el arreglo del numero y la hwnd de la ventana de agregar
	for (size_t i = 0; i < auxnumero[i]; i++) //se va recorriendo el arreglo caracter por caracter
	{
		if (isalpha(auxnumero[i]))//se compara si el caracter actual es una letra
		{
			StieneNumeros = false;//si entra aquí es porque el carcater es una letra y se sale del ciclo
			break;
		}
		//de lo contrario continuara recorriendolo
		StieneNumeros = true;
	}
	if (!StieneNumeros)//si entra aquí es porque encontro una letra 
	{
		MessageBox(dlg, "¡Por favor pon solo Numeros!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false;
	}
		return true;//sino, es porque NO hay letras
}
bool tienenumerosCP(char auxCP[], HWND dlg) {//exactamente lo mismo
	for (size_t i = 0; i < auxCP[i]; i++)
	{
		if (isalpha(auxCP[i]))
		{
			SitienenumerosCP = false; 
			break; 
		}
		SitienenumerosCP = true; 
	}
	if (!SitienenumerosCP)
	{
		MessageBox(dlg, "¡Por favor pon  un CP valido!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false;
	}
		return true;
}
bool tienenumerosdireccion(char auxdire[], HWND dlg) {//exactamente lo mismo
	for (size_t i = 0; i < auxdire[i]; i++)
	{
		if (isalpha(auxdire[i]))
		{
			SitienenumerosDireccion = false;
			break;
		}
		SitienenumerosDireccion = true; 
	}
	if (!SitienenumerosDireccion)
	{
		MessageBox(dlg, "¡Por favor pon  un Numero de dirección valido!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
		return false;
	}
		return true;
}
//ORDENAMIENTO
void swap(contacto *a, contacto*b)
{
	contacto*t=new contacto; 
	strcpy(t->nombre.primernombre, a->nombre.primernombre); 
	strcpy(t->nombre.segundonombre, a->nombre.segundonombre);
	strcpy(t->nombre.apellidopaterno, a->nombre.apellidopaterno);
	strcpy(t->nombre.apellidomaterno, a->nombre.apellidomaterno);
	strcpy(t->email, a->email);
	strcpy(t->numerotelefonico, a->numerotelefonico);
	strcpy(t->nombreamayus, a->nombreamayus);
	strcpy(t->imagen1, a->imagen1);
	strcpy(t->imagen2, a->imagen2);
	strcpy(t->direccion.calle, a->direccion.calle);
	strcpy(t->direccion.colonia, a->direccion.colonia);
	strcpy(t->direccion.CP, a->direccion.CP);
	strcpy(t->direccion.entrecalle1, a->direccion.entrecalle1);
	strcpy(t->direccion.estado, a->direccion.estado);
	strcpy(t->direccion.municipio, a->direccion.municipio);
	strcpy(t->direccion.numero, a->direccion.numero);
	strcpy(t->direccion.pais, a->direccion.pais);

	strcpy(a->nombre.primernombre, b->nombre.primernombre);
	strcpy(a->nombre.segundonombre, b->nombre.segundonombre);
	strcpy(a->nombre.apellidopaterno, b->nombre.apellidopaterno);
	strcpy(a->nombre.apellidomaterno, b->nombre.apellidomaterno);
	strcpy(a->email, b->email);
	strcpy(a->numerotelefonico, b->numerotelefonico);
	strcpy(a->nombreamayus, b->nombreamayus);
	strcpy(a->imagen1, b->imagen1);
	strcpy(a->imagen2, b->imagen2);
	strcpy(a->direccion.calle, b->direccion.calle);
	strcpy(a->direccion.colonia, b->direccion.colonia);
	strcpy(a->direccion.CP, b->direccion.CP);
	strcpy(a->direccion.entrecalle1, b->direccion.entrecalle1);
	strcpy(a->direccion.estado, b->direccion.estado);
	strcpy(a->direccion.municipio, b->direccion.municipio);
	strcpy(a->direccion.numero, b->direccion.numero);
	strcpy(a->direccion.pais, b->direccion.pais);

	strcpy(b->nombre.primernombre, t->nombre.primernombre);
	strcpy(b->nombre.segundonombre, t->nombre.segundonombre);
	strcpy(b->nombre.apellidopaterno, t->nombre.apellidopaterno);
	strcpy(b->nombre.apellidomaterno, t->nombre.apellidomaterno);
	strcpy(b->email, t->email);
	strcpy(b->numerotelefonico, t->numerotelefonico);
	strcpy(b->nombreamayus, t->nombreamayus);
	strcpy(b->imagen1, t->imagen1);
	strcpy(b->imagen2, t->imagen2);
	strcpy(b->direccion.calle, t->direccion.calle);
	strcpy(b->direccion.colonia, t->direccion.colonia);
	strcpy(b->direccion.CP, t->direccion.CP);
	strcpy(b->direccion.entrecalle1, t->direccion.entrecalle1);
	strcpy(b->direccion.estado, t->direccion.estado);
	strcpy(b->direccion.municipio, t->direccion.municipio);
	strcpy(b->direccion.numero, t->direccion.numero);
	strcpy(b->direccion.pais, t->direccion.pais);
		/*char t[30];
	strcpy(t, a);
	strcpy(a, b);
	strcpy(b, t);*/
}
contacto* partition(contacto *l, contacto *h)
{
	// set pivot as h element 
	char x[30];
	strcpy(x, h->nombre.primernombre);
									  // similar to i = l-1 for array implementation 
	contacto *i = l->ant;
	// Similar to "for (int j = l; j <= h- 1; j++)" 
	for (contacto *j = l; j != h; j = j->sig)
	{
		if (strcmp(x, j->nombre.primernombre)>0)
		{
			// Similar to i++ for array 
			i = (i == NULL) ? l : i->sig;
			swap(i, j);
		}
	}
	i = (i == NULL) ? l : i->sig; // Similar to i++ 
	swap(i, h);
	return i;
}
struct contacto *lastNode(contacto *root)
{
	while (root->sig != NULL)
		root = root->sig;
	return root;
}
/* A recursive implementation of quicksort for linked list */
void _quickSort(struct contacto* l, struct contacto *h)
{
	if (h != NULL && l != h && l != h->sig)
	{
		struct contacto *p = partition(l, h);
		_quickSort(l, p->ant);
		_quickSort(p->sig, h);
	}
}
void quickSort(struct contacto *head)
{
	// Find last node 
	struct contacto *h = lastNode(head);
	// Call the recursive QuickSort 
	_quickSort(head, h);
}
void llenacombocolores(HWND obj, UINT mensaje, char archivo[_MAX_PATH]) {
	static char colores[_MAX_PATH]; 
	ifstream archicolores; 
	archicolores.open(archivo); 
	if (archicolores.is_open())
	{
		archicolores.getline(colores, 20); 
		while (!archicolores.eof())
		{
			SendMessage(obj, mensaje, 0, (LPARAM)colores);
			archicolores.getline(colores, 20);
		}
	}
	archicolores.close(); 
}
void buscacolor(char busquedacolor[]) {
	if (strcmp(busquedacolor, "Rojo")==0)
	{
		R = 255; G = 0; B = 0; 
	}
	else if (strcmp(busquedacolor, "Amarillo") == 0) {
		R = 250; G = 200; B = 0; 
	}
	else if (strcmp(busquedacolor, "Negro") == 0)
	{
		R = 0; G = 0; B = 0; 
	}
	else if (strcmp(busquedacolor, "Naranja") == 0)
	{
		R = 250; G = 120; B = 10; 
	}
	else if (strcmp(busquedacolor, "Rosa") == 0)
	{
		R = 250; G = 10; B = 150; 
	}
	else if (strcmp(busquedacolor, "Verde") == 0)
	{
		R = 10; G = 150; B = 10; 
	}
	else if (strcmp(busquedacolor, "Morado") == 0)
	{
		R = 100; G = 10; B = 100;
	}
	else if (strcmp(busquedacolor, "Azul") == 0)
	{
		R = 10; G = 10; B = 100;
	}
	else if (strcmp(busquedacolor, "Gris") == 0)
	{
		R = 50; G = 50; B = 65;
	}
	else if (strcmp(busquedacolor, "Celeste") == 0)
	{
		R = 25; G = 100; B = 255;
	}
}
void cambiaprimeraletraamayus(int longitudnombre, char nombre[]) {
	char PrimerLetra[20], Restonombre[20];
	cortanombre = nombre; //El string cortanombre lo igualo a lo que se capturo en el edit control
	cortanombre = cortanombre.substr(longitudprimernombre - longitudprimernombre, 1);//Saco la primera letra del nombre
	strcpy(PrimerLetra, cortanombre.c_str()); //la copio a una variable de tipo char, porque no puedo pasarla a mayusculas siendo de tipo string
	strupr(PrimerLetra); //la primera letra la paso a mayusculas
	cortanombre = nombre; //vuelvo a igualar el string cortanombre al nombre que se capturo
	cortanombre = cortanombre.substr(longitudprimernombre + 1 - longitudprimernombre, longitudprimernombre); //Saco el resto del nombre sin tomar la primera letra para concatenarlo y así hacer que el nombre siempre sea mayusculas
	strcpy(Restonombre, cortanombre.c_str()); //copio el resto del nombre a la variable char "restonombre"
	strcat(PrimerLetra, Restonombre); //Concateno el resto del nombre a la primera letra ya en mayusculas
	strcpy(apoyo.nombre.primernombre, PrimerLetra); //Lo copio a la variable de la estructura apoyo con la cual lo copiare al nodo de la lista
}
void agregacontactos(contacto apoyo, char dire1[], char dire2[]) {
	nuevo = new contacto; //Se reserva espacio de memoria de la estructura
	nuevo->ant = 0;
	nuevo->sig = 0;
	strcpy(nuevo->numerotelefonico, apoyo.numerotelefonico); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->email, apoyo.email); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->nombre.primernombre, apoyo.nombre.primernombre); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->nombre.segundonombre, apoyo.nombre.segundonombre);//se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->nombre.apellidopaterno, apoyo.nombre.apellidopaterno);//se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->nombre.apellidomaterno, apoyo.nombre.apellidomaterno);//se copia lo que se capturo en una estructura de apoyo al nodo
	//Tomamos cada campo(de los nombres) que se capturo, y lo ordenamos en una nueva variable que los contendra a todos
	sprintf(nuevo->nombreamayus, "%s%s%s%s", nuevo->nombre.primernombre, nuevo->nombre.segundonombre, nuevo->nombre.apellidopaterno, nuevo->nombre.apellidomaterno);
	strupr(nuevo->nombreamayus);//Hace que el Todo el nombre se haga mayusculas
	strcpy(nuevo->direccion.calle, apoyo.direccion.calle); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.colonia, apoyo.direccion.colonia); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.entrecalle1, apoyo.direccion.entrecalle1); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.estado, apoyo.direccion.estado); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.municipio, apoyo.direccion.municipio); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.pais, apoyo.direccion.pais); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.CP, apoyo.direccion.CP); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->direccion.numero, apoyo.direccion.numero); //se copia lo que se capturo en una estructura de apoyo al nodo
	strcpy(nuevo->imagen1, dire1); //se copia la dirección de la primera imagen
	strcpy(nuevo->imagen2, dire2); //se copia la dirección de la segunda imagen 

}
int WINAPI WinMain(HINSTANCE h1, HINSTANCE h2, LPSTR lPSTR, int cmd) {
	Leecontacots(); 
	ghDlg = CreateDialog(h1, MAKEINTRESOURCE(IDD_DIALOG1), 0, call_Dialogo1);
	ShowWindow(ghDlg, cmd);
	MSG msg;
	HDC hdc;
	//donde es?
	//en la indepe
	//en donde exactamente
	//tepeyac 1010
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (ghDlg == 0 || !IsDialogMessage(ghDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
BOOL CALLBACK call_Dialogo1(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) {
	static HWND img1, img2, img3; static HBITMAP bmp; 
	static HWND nombre, numero, email; 
	static HWND listacontactos, combocolores=0; 
	UINT indice, indicecombo; //se usa para saber cual elemento se eligio en el listbox
	char auxnombre[_MAX_PATH], auxcolor[_MAX_PATH]; 
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		//se obitenen las variables HWND de cada objeto de la ventana
		nombre = GetDlgItem(Dlg, IDC_EDITnombre);
		numero = GetDlgItem(Dlg, IDC_EDITnumero);
		email = GetDlgItem(Dlg, IDC_EDITemail);
		listacontactos = GetDlgItem(Dlg, IDC_LISTcontactos);
		combocolores = GetDlgItem(Dlg, IDC_COMBO1); 
		img1 = GetDlgItem(Dlg, IDC_ImgPP);
		img2 = GetDlgItem(Dlg, IDC_ImgPP);
		img3 = GetDlgItem(Dlg, IDC_Imagenboton); 
		llenacombocolores(combocolores, CB_ADDSTRING, Archcolores);
		/*Flstudio programa de musica para videojuegos
		Hay alguna diferencia a la hora de desarrollar entre las plataformas?*/
		llenabmp(img3, direimgaen, 50, 50);
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_Imagenboton: {
			MessageBox(Dlg, ":V", "owo", MB_ICONEXCLAMATION); 
			return true; 
		}
		case IDC_COMBO1: {
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK: {
				indicecombo = SendMessage(combocolores, CB_GETCURSEL, 0, 0);
				SendMessage(combocolores, CB_GETLBTEXT, (WPARAM)indicecombo, (LPARAM)auxcolor);
				buscacolor(auxcolor);
				EndDialog(Dlg, false);
				DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG1), Dlg, call_Dialogo1);
				hBrush = CreateSolidBrush(RGB(R, G, B));
				FillRgn(hdc, bgRgn, hBrush);
				return true; 
			}
			}
			return true;
		}
		case ID_CONTACTOS_AGREGAR: {
			DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG2), Dlg, call_Dialogo2);
			return true; 
		}
		case ID_Ayuda: {
			DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG6), Dlg, call_Dialogo6);
			return true; 
		}
		case ID_CONTACTOS_BUSCAR: {
			DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG3), Dlg, call_Dialogo3);
			return true; 
		}
		case ID_VERCONTACTOS: {//Tres legendarias lineas
			auxC = ini;//el puntero auxiliar apunta al mismo nodo que apunta el puntero inicial
			SendMessage(listacontactos, LB_RESETCONTENT, 0, (LPARAM)" "); //se "vacia" el listbox
			quickSort(auxC); //Se ordena la lista ligada
			while (auxC != NULL)//se abre un ciclo que se repetira hasta que el puntero auxc apunte a NULL
			{
				SendMessage(listacontactos, LB_ADDSTRING, 0, (LPARAM)auxC->nombre.primernombre);//se va poniendo el primer nombre de cada nodo de manera ordenada
				auxC = auxC->sig;//se avanza al siguiente nodo
			}
			return true; 
		}
		case ID_Carrusel: {
			if (ini!=0)//se pregunta si Ya hay al menos UN nodo
			{
				DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG5), Dlg, call_Dialogo5);
			}
			else
			{
				MessageBox(Dlg, "Primero agrega contactos!", "MESSAGE", MB_OK + MB_ICONWARNING);
			}
			return true; 
		}
		case IDC_LISTcontactos: {
			switch (HIWORD(wParam))
			{
			case LBN_DBLCLK: {
				indice = SendMessage(listacontactos, LB_GETCURSEL, 0, 0);//se obtiene cual elemento selecciono
				auxC = buscame(indice); 
				llenabmp(img1, auxC->imagen1, 80, 80);//pone la primera imagen del contacto
				SetWindowText(nombre, auxC->nombreamayus);//pone su nombre en mayusculas
				SetWindowText(numero, auxC->numerotelefonico);//pone su numero telefonico
				SetWindowText(email, auxC->email);// pone su correo
				return true;
			}
			}
			return true;
		}
		case IDC_imgant: {
			img1 = GetDlgItem(Dlg, IDC_ImgPP); //se obtiene la HWND del picturebox 
			llenabmp(img1, auxC->imagen2, 100, 100); //se pone la segunda imagen del contacto
			return true; 
		}
		case IDC_imgsig: {
			img2 = GetDlgItem(Dlg, IDC_ImgPP); //se obtiene la HWND del picturebox 
			llenabmp(img2, auxC->imagen1, 100, 100); //se pone la primera imagen del contacto
			return true; 
		}
		}
		return true;
	}
	case WM_CLOSE: {
		escribecontactos(); //se escribe toda la lista en un archivo binario
		PostQuitMessage(0);
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}
BOOL CALLBACK call_Dialogo2(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) {//VENTANA DE AREGAR CONTACTOS
	static HWND NumeroTele, PrimerNomb, SegundoNomb, APaterno, AMaterno, Email, Dcalle, Dcolonia, DEntrecalles, DMunicipio, DEstado, Dpais, CP, DNumero; 
	static HWND Dimg1, Dimg2; 
	static HWND Hbmp, Hbmp2; 
	static char direccion1[_MAX_PATH], direccion2[_MAX_PATH]; 
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		//se inicializan las variables booleanas en false
		sigimg = false; 
		//se obtienen las variables HWND de cada objeto de la ventana/dialog/callback
		NumeroTele = GetDlgItem(Dlg, IDC_EDITnumero); 
		PrimerNomb = GetDlgItem(Dlg, IDC_EDITPnombre); 
		SegundoNomb = GetDlgItem(Dlg, IDC_EDITSnombre); 
		APaterno = GetDlgItem(Dlg, IDC_EDITApaterno); 
		AMaterno = GetDlgItem(Dlg, IDC_EDITAmaterno);
		Email = GetDlgItem(Dlg, IDC_EDITemail); 
		Dcalle = GetDlgItem(Dlg, IDC_EDITDcalle); 
		Dcolonia = GetDlgItem(Dlg, IDC_EDITDcolonia); 
		DEntrecalles = GetDlgItem(Dlg, IDC_EDITDentre); 
		DMunicipio = GetDlgItem(Dlg, IDC_EDITDmuni); 
		DEstado = GetDlgItem(Dlg, IDC_EDITDestado); 
		Dpais = GetDlgItem(Dlg, IDC_EDITDpais); 
		CP = GetDlgItem(Dlg, IDC_EDITDCP); 
		DNumero = GetDlgItem(Dlg, IDC_EDITDnumero); 
		Dimg1 = GetDlgItem(Dlg, IDC_EDITdireccionimagen); 
		Dimg2 = GetDlgItem(Dlg, IDC_EDITdireccionimagen2); 
		Hbmp = GetDlgItem(Dlg, IDC_imagen); 
		Hbmp2 = GetDlgItem(Dlg, IDC_Imagen2); 
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_BUTTONAgregar: {
			//Se toma lo que estaba en las cajas de texto(editcontrol)
			GetWindowText(NumeroTele, apoyo.numerotelefonico, 30);
			GetWindowText(PrimerNomb, apoyo.nombre.primernombre, 20);
			GetWindowText(SegundoNomb, apoyo.nombre.segundonombre, 20);
			GetWindowText(APaterno, apoyo.nombre.apellidopaterno, 20);
			GetWindowText(AMaterno, apoyo.nombre.apellidomaterno, 20);
			GetWindowText(Email, apoyo.email, _MAX_PATH);
			GetWindowText(Dcalle, apoyo.direccion.calle, _MAX_PATH);
			GetWindowText(Dcolonia, apoyo.direccion.colonia, 30);
			GetWindowText(DEntrecalles, apoyo.direccion.entrecalle1, _MAX_PATH);
			GetWindowText(DMunicipio, apoyo.direccion.municipio, 30);
			GetWindowText(DEstado, apoyo.direccion.estado, 30);
			GetWindowText(Dpais, apoyo.direccion.pais, 20);
			GetWindowText(CP, apoyo.direccion.CP, 10);
			GetWindowText(DNumero, apoyo.direccion.numero, 5);
			//hasta aquí es donde se toma lo que tienen los editcontrol
			longitudtelefono = strlen(apoyo.numerotelefonico); //se calcula la longitud del telefono que se capturo
			longitudemail = strlen(apoyo.email); //se calcula la longitud del email
			strcat(apoyo.nombre.primernombre, " ");// lo que se capturo en el nombre se le concatena un espacio
			strcat(apoyo.nombre.segundonombre, " ");// lo que se capturo en el segundo nombre se le concatena un espacio
			strcat(apoyo.nombre.apellidopaterno, " ");//lo que se capturo en el apellido paterno se le concatena un espacio
			strcat(apoyo.nombre.apellidomaterno, " ");//lo que se capturo en el apellido materno se le concatena un espacio
			//Esta parte es para que el la letra del primer nombre siempre sea mayuscula
			longitudprimernombre = strlen(apoyo.nombre.primernombre); //Saco la longitud del Primer nombre
			cambiaprimeraletraamayus(longitudprimernombre, apoyo.nombre.primernombre); 
			if (tiene12(longitudtelefono, Dlg) && tieneaycom(longitudemail, apoyo.email, Dlg) && tienenumeros(apoyo.numerotelefonico, Dlg)&&tienenumerosCP(apoyo.direccion.CP, Dlg)&&tienenumerosdireccion(apoyo.direccion.numero, Dlg))
				{
					//SI Y SOLO SI CADA UNA DE ESAS FUNCIONES QUE ESTAN DENTRO DEL IF RETORNAN UN VERDADERO ENTRARARA AQUÍ
				agregacontactos(apoyo, direccion1, direccion2);
					vaciabmp(Hbmp, 0, 0, 0);//se hace la limpieza de pantalla
					vaciabmp(Hbmp2, 0, 0, 0);//se hace la limpieza de pantalla
					if (ini==NULL)//pregunta si la lista esta vacia 
					{
						ini = nuevo; //el puntero ini ahora apunta al nodo que esta apuntando el puntero ini
						MessageBox(Dlg, "¡Contacto agregado !", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
						//se hace la limpieza de pantalla
						SetWindowText(NumeroTele, "");
						SetWindowText(PrimerNomb, "");
						SetWindowText(SegundoNomb, "");
						SetWindowText(APaterno, "");
						SetWindowText(AMaterno, "");
						SetWindowText(Email, "");
						SetWindowText(Dcalle, "");
						SetWindowText(Dcolonia, "");
						SetWindowText(DEntrecalles, "");
						SetWindowText(DMunicipio, "");
						SetWindowText(DEstado, "");
						SetWindowText(Dpais, "");
						SetWindowText(CP, "");
						SetWindowText(DNumero, "");
						//se hace la limpieza de pantalla
					}
					else
					{		 
						nuevo->repetido= Buscarepetidoemail(nuevo->email);//se manda el email del nuevo nodo que se quiere agregar y se comprueba que no haya otro igual
						if (!nuevo->repetido)//si NO hay un nodo que tenga el mismo email entra
						{
						auxC = ini;//Tres legendarias lineas
						while (auxC->sig != NULL)
						{
							auxC = auxC->sig;
						}//conecciones entre nodos
						auxC->sig = nuevo;
						nuevo->ant = auxC;
						nuevo->sig = NULL;
						nuevo->id = auxC->id + 1; 
						MessageBox(Dlg, "¡Contacto agregado !", "MESSAGE", MB_OK );//limpieza de pantallas
						SetWindowText(NumeroTele, "");
						SetWindowText(PrimerNomb, "");
						SetWindowText(SegundoNomb, "");
						SetWindowText(APaterno, "");
						SetWindowText(AMaterno, "");
						SetWindowText(Email, "");
						SetWindowText(Dcalle, "");
						SetWindowText(Dcolonia, "");
						SetWindowText(DEntrecalles, "");
						SetWindowText(DMunicipio, "");
						SetWindowText(DEstado, "");
						SetWindowText(Dpais, "");
						SetWindowText(CP, "");
						SetWindowText(DNumero, "");
						}
						else//si hay otro que tenga el mismo email, entrara a este else y borrara el puntero nuevo
						{
							delete nuevo;
							MessageBox(Dlg, "Ya existe ese correo!", "MESSAGE", MB_OK + MB_ICONWARNING);
						}
					}
				}
			return true; 
		}
		case ID_EXAMINAR: {
			if (!sigimg)
			{
				GetFile(Dlg, IDC_EDITdireccionimagen, 1);
				GetWindowText(Dimg1, direccion1, _MAX_PATH);
				llenabmp(Hbmp, direccion1, 100, 100);
				sigimg = true; 
			}
			else
			{
				GetFile(Dlg, IDC_EDITdireccionimagen2, 1);
				GetWindowText(Dimg2, direccion2, _MAX_PATH);
				llenabmp(Hbmp2, direccion2, 100, 100);
			}
			return true; 
		}
		}
		return true;
	}
	case WM_CLOSE: {
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}
BOOL CALLBACK call_Dialogo3(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) {//VENTANA DE BUSCAR
	static HWND editbusqueda, busqueda; 
	static HWND img, img1; 
	static HWND PnumeroT, PPrimernombre, Psegundonombre, PApellidopaterno, PapellidoMaterno, Pemail, PCP, PColonia, Pnumero, PCalle, Pentrecalles, Pmuni, Ppais, Pestado; 
	static HWND PNCOMPLETO = 0, lista = 0, combito = 0; 
	char auxbusqueda[_MAX_PATH], Seleccióncombo[20]; 
	UINT indice; 
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		//se obtienen las variables HWND de cada objeto de de la pantalla
		busqueda = GetDlgItem(Dlg, IDC_EDITbusquedaPnombre);
		PnumeroT = GetDlgItem(Dlg, IDC_STATICBnumero);
		PPrimernombre = GetDlgItem(Dlg, IDC_STATICPnombreB);
		Psegundonombre = GetDlgItem(Dlg, IDC_STATICBsegundoN);
		PApellidopaterno = GetDlgItem(Dlg, IDC_STATICBApaterno);
		PapellidoMaterno = GetDlgItem(Dlg, IDC_STATICBAmaterno);
		Pemail = GetDlgItem(Dlg, IDC_STATICBemail);
		PCalle = GetDlgItem(Dlg, IDC_STATICBCalle);
		PColonia = GetDlgItem(Dlg, IDC_STATICBcolonia);
		Pnumero = GetDlgItem(Dlg, IDC_STATICBnumeroD);
		Pentrecalles = GetDlgItem(Dlg, IDC_STATICBentrecalles);
		Pmuni = GetDlgItem(Dlg, IDC_STATICBmuni);
		Ppais = GetDlgItem(Dlg, IDC_STATICBpais);
		Pestado = GetDlgItem(Dlg, IDC_STATICBestado);
		PCP = GetDlgItem(Dlg, IDC_STATICCP);
		PNCOMPLETO = GetDlgItem(Dlg, IDC_STATICBNCO);
		lista = GetDlgItem(Dlg, IDC_LIST1);
		img = GetDlgItem(Dlg, IDC_Imagenbusqueda);
		combito = GetDlgItem(Dlg, IDC_COMBO1);
		SendMessage(combito, CB_ADDSTRING, 0, (LPARAM)"nombre");
		SendMessage(combito, CB_ADDSTRING, 0, (LPARAM)"email");
		SendMessage(lista, LB_RESETCONTENT, 0, (LPARAM)" ");
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_BUTTONbuscar: {
			SendMessage(lista, LB_RESETCONTENT, 0, (LPARAM)" ");
			GetWindowText(combito, Seleccióncombo, 20); 
			GetWindowText(busqueda, auxbusqueda, _MAX_PATH);//se toma lo que esta en la caja de texto 
			if (strcmp(auxbusqueda, " ")==0)//si lo que se tomo del edit control esta vacio entrara
			{
				MessageBox(Dlg, "¡Faltan datos por capturar!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
			}
			else if(strcmp(Seleccióncombo, "nombre")==0)
			{
				strcat(auxbusqueda, " "); //se concatena un espacio en el char de busqueda porque el primer nombre de cada nodo tiene un espacio al final
				auxC = ini;
				while (auxC != 0)
				{
					if (strcmp(auxC->nombre.primernombre, auxbusqueda)==0)
					{
						SendMessage(lista, LB_ADDSTRING, 0, (LPARAM)auxC->nombreamayus); 
					}
					auxC = auxC->sig; 
				}
			}
			else if(strcmp(Seleccióncombo, "email")==0)
			{
				auxC = ini;
				while (auxC != 0)
				{
					if (strcmp(auxC->email, auxbusqueda) == 0)
					{
						SendMessage(lista, LB_ADDSTRING, 0, (LPARAM)auxC->email);
					}
					auxC = auxC->sig;
				}
			}
			else
			{
				MessageBox(Dlg, "¡Selecciona una opción de busqueda!", "MESSAGE", MB_OK + MB_ICONEXCLAMATION);
			}
			return true; 
		}
		case IDC_LIST1: {
			switch (HIWORD(wParam))
			{
			case LBN_DBLCLK: {
				indice = SendMessage(lista, LB_GETCURSEL, 0, 0);
				SendMessage(lista, LB_GETTEXT, indice, (LPARAM)auxbusqueda);
				//calle galileo 125 cortijo del rio
				//Buscacontacto(auxbusqueda); 
				auxC = buscame2(auxbusqueda);
				llenabmp(img, auxC->imagen1, 70, 70); 
					SetWindowText(PnumeroT, auxC->numerotelefonico); 
					SetWindowText(PPrimernombre, auxC->nombre.primernombre); 
					SetWindowText(Psegundonombre, auxC->nombre.segundonombre);
					SetWindowText(PApellidopaterno, auxC->nombre.apellidopaterno);
					SetWindowText(PapellidoMaterno, auxC->nombre.apellidomaterno);
					SetWindowText(Pemail, auxC->email);
					SetWindowText(PCalle, auxC->direccion.calle);
					SetWindowText(PColonia, auxC->direccion.colonia);
					SetWindowText(Pnumero, auxC->direccion.numero);
					SetWindowText(Pentrecalles, auxC->direccion.entrecalle1);
					SetWindowText(Pmuni, auxC->direccion.municipio);
					SetWindowText(Ppais, auxC->direccion.pais);
					SetWindowText(Pestado, auxC->direccion.estado);
					SetWindowText(PCP, auxC->direccion.CP);
					SetWindowText(PNCOMPLETO, auxC->nombreamayus);
					SetWindowText(busqueda, ""); 
				return true; 
			}
			}
			return true;
		}
		case IDC_Modificar: {
				DialogBox(H_DIALOG, MAKEINTRESOURCE(IDD_DIALOG4), Dlg, call_Dialogo4);
			return true; 
		}
		case IDC_Eliminar: {
			borracontacto(auxC); // se manda el puntero que esta apuntado al nodo que se acaba de buscar y se borra
			MessageBox(Dlg, "¡Contacto Eliminado!", "MESSAGE", MB_OK+ MB_ICONEXCLAMATION);
			EndDialog(Dlg, false);
			return true;
		}
		case IDC_imgant: {
			llenabmp(img, auxC->imagen1, 100, 100);//se muestra la imagen 1
			return true;
		}
		case IDC_imgsig: {
			img1 = GetDlgItem(Dlg, IDC_Imagenbusqueda); 
			llenabmp(img1, auxC->imagen2, 100, 100); //se muestra la imagen 2
			return true;
		}
		}
		return true;
	}
	case WM_CLOSE: {
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}
BOOL CALLBACK call_Dialogo4(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) {//VENTANA MODIFICAR
	string aux1 ; 
	long longitudemail1=0; 
	static HWND NumeroTele, PrimerNomb, SegundoNomb, APaterno, AMaterno, Email, Dcalle, Dcolonia, DEntrecalles, DMunicipio, DEstado, Dpais, CP, DNumero;
	static HWND Dimg1, Dimg2;
	static HWND Hbmp, Hbmp2; char direccion1[_MAX_PATH], direccion2[_MAX_PATH]; 
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		sitiene12 = false;
		sitienea = false;
		sitienecom = false;
		StieneNumeros = false;
		NumeroTele = GetDlgItem(Dlg, IDC_EDITMnumero); 
		PrimerNomb = GetDlgItem(Dlg, IDC_EDITEPnombre); 
		SegundoNomb = GetDlgItem(Dlg, IDC_EDITESegundoN); 
		APaterno = GetDlgItem(Dlg, IDC_EDITEApaterno); 
		AMaterno = GetDlgItem(Dlg, IDC_EDITEAMaterno); 
		Email = GetDlgItem(Dlg, IDC_EDITEEmail);
		Dcalle = GetDlgItem(Dlg, IDC_EDITECalle);
		Dcolonia = GetDlgItem(Dlg, IDC_EDITEcolonia);
		DEntrecalles = GetDlgItem(Dlg, IDC_EDITEEntre);
		DMunicipio = GetDlgItem(Dlg, IDC_EDITEmuni);
		DEstado = GetDlgItem(Dlg, IDC_EDITEEstado);
		Dpais = GetDlgItem(Dlg, IDC_EDITEpais);
		CP = GetDlgItem(Dlg, IDC_EDITECP);
		DNumero = GetDlgItem(Dlg, IDC_EDITENu);
		Hbmp = GetDlgItem(Dlg, IDC_imagen); 
		Hbmp2 = GetDlgItem(Dlg, IDC_Imagen2); 
		Dimg1 = GetDlgItem(Dlg, IDC_EDITDireccionMod);
		Dimg2 = GetDlgItem(Dlg, IDC_EDITDireccionMod2);
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_BUTTONModi: {
			GetWindowText(NumeroTele, apoyo.numerotelefonico, 30);
			GetWindowText(PrimerNomb, apoyo.nombre.primernombre, 20);
			GetWindowText(SegundoNomb, apoyo.nombre.segundonombre, 20);
			GetWindowText(APaterno, apoyo.nombre.apellidopaterno, 20);
			GetWindowText(AMaterno, apoyo.nombre.apellidomaterno, 20);
			GetWindowText(Email, apoyo.email, _MAX_PATH);
			GetWindowText(Dcalle, apoyo.direccion.calle, _MAX_PATH);
			GetWindowText(Dcolonia, apoyo.direccion.colonia, 30);
			GetWindowText(DEntrecalles, apoyo.direccion.entrecalle1, _MAX_PATH);
			GetWindowText(DMunicipio, apoyo.direccion.municipio, 30);
			GetWindowText(DEstado, apoyo.direccion.estado, 30);
			GetWindowText(Dpais, apoyo.direccion.pais, 20);
			GetWindowText(CP, apoyo.direccion.CP, 10);
			GetWindowText(DNumero, apoyo.direccion.numero, 5);
				longitudtelefono = strlen(apoyo.numerotelefonico);
				longitudemail1 = strlen(apoyo.email);
				if (strcmp(apoyo.numerotelefonico, "")==0)
				{
					sitiene12 = true; 
					StieneNumeros = true; 
				}
				else
				{
					sitiene12 = tiene12(longitudtelefono, Dlg); 
					StieneNumeros = tienenumeros(apoyo.numerotelefonico, Dlg); 
					if (sitiene12&&StieneNumeros)
					{
						strcpy(auxC->numerotelefonico, apoyo.numerotelefonico);
					}
				}
				if (!strcmp(apoyo.email, ""))
				{
					sitienea = true; 
					sitienecom = true; 
				}
				else
				{
					if (tieneaycom(longitudemail1, apoyo.email, Dlg));
					{
						strcpy(auxC->email, apoyo.email);
						sitienea = true, sitienecom = true; 
					}
				}		
					if (sitiene12&&sitienea&&sitienecom&&StieneNumeros)
					{
							if (!strcmp(apoyo.nombre.primernombre, ""))
							{
							}
							else
							{
								longitudprimernombre = strlen(apoyo.nombre.primernombre); 
								cambiaprimeraletraamayus(longitudprimernombre, apoyo.nombre.primernombre); 
								strcpy(auxC->nombre.primernombre, apoyo.nombre.primernombre);
								strcat(auxC->nombre.primernombre, " "); 
							}
							if (!strcmp(apoyo.nombre.segundonombre, ""))
							{
							}
							else
							{
								strcpy(auxC->nombre.segundonombre, apoyo.nombre.segundonombre);
								strcat(auxC->nombre.segundonombre, " ");
							}
							if (!strcmp(apoyo.nombre.apellidopaterno, ""))
							{
							}
							else
							{
								strcpy(auxC->nombre.apellidopaterno, apoyo.nombre.apellidopaterno);
								strcat(auxC->nombre.apellidopaterno, " ");
							}
							if (!strcmp(apoyo.nombre.apellidomaterno, ""))
							{
							}
							else
							{
								strcpy(auxC->nombre.apellidomaterno, apoyo.nombre.apellidomaterno);
								strcat(auxC->nombre.apellidomaterno, " ");
							}
							sprintf(auxC->nombreamayus, "%s%s%s%s", auxC->nombre.primernombre, auxC->nombre.segundonombre, auxC->nombre.apellidopaterno, auxC->nombre.apellidomaterno);
							strupr(auxC->nombreamayus); 
							if (!strcmp(apoyo.direccion.calle, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.calle, apoyo.direccion.calle);
							}
							if (!strcmp(apoyo.direccion.colonia, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.colonia, apoyo.direccion.colonia);
							}
							if (!strcmp(apoyo.direccion.entrecalle1, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.entrecalle1, apoyo.direccion.entrecalle1);
							}
							if (!strcmp(apoyo.direccion.estado, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.estado, apoyo.direccion.estado);
							}
							if (!strcmp(apoyo.direccion.municipio, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.municipio, apoyo.direccion.municipio);
							}
							if (!strcmp(apoyo.direccion.pais, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.pais, apoyo.direccion.pais);
							}
							if (!strcmp(apoyo.direccion.CP, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.CP, apoyo.direccion.CP);
							}
							if (!strcmp(apoyo.direccion.numero, ""))
							{
							}
							else
							{
								strcpy(auxC->direccion.numero, apoyo.direccion.numero);
							}
							MessageBox(Dlg, "Contacto Modificado!", "MESSAGE", MB_OK);
							EndDialog(Dlg, false);
					}
			return true; 
		}
		case ID_EXAMINAR: {
			if (!sigimg)
			{
				GetFile(Dlg, IDC_EDITdireccionimagen, 1);
				GetWindowText(Dimg1, direccion1, _MAX_PATH);
				llenabmp(Hbmp, direccion1, 100, 100);
				sigimg = true;
			}
			else
			{
				GetFile(Dlg, IDC_EDITdireccionimagen2, 1);
				GetWindowText(Dimg2, direccion2, _MAX_PATH);
				llenabmp(Hbmp2, direccion2, 100, 100);
			}
			return true; 
		}
		}
		return true;
	}
	case WM_CLOSE: {
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}
BOOL CALLBACK call_Dialogo5(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) { //CARRUSEL
	static HWND img1, img2; 
	static HWND numeroT, Pnombre, Snombre, APnombre, AMnombre, email, calle, numero, CP, Pais, Estado, colonia, entrecalles, municipio, NCompleto; 
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		numeroT = GetDlgItem(Dlg, IDC_STATICBnumero);
		Pnombre = GetDlgItem(Dlg, IDC_STATICPnombreB);
	    Snombre = GetDlgItem(Dlg, IDC_STATICBsegundoN);
		APnombre = GetDlgItem(Dlg, IDC_STATICBApaterno);
		AMnombre = GetDlgItem(Dlg, IDC_STATICBAmaterno);
		email = GetDlgItem(Dlg, IDC_STATICBemail);
		calle = GetDlgItem(Dlg, IDC_STATICBCalle);
		numero = GetDlgItem(Dlg, IDC_STATICBnumeroD);
		CP = GetDlgItem(Dlg, IDC_STATICBcp);
		Pais = GetDlgItem(Dlg, IDC_STATICBpais2);
		Estado = GetDlgItem(Dlg, IDC_STATICBestado2);
		colonia = GetDlgItem(Dlg, IDC_STATICBcolonia);
		entrecalles = GetDlgItem(Dlg, IDC_STATICBentrecalles);
		municipio = GetDlgItem(Dlg, IDC_STATICBmuni2);
		img1 = GetDlgItem(Dlg, IDC_Imagenbusqueda);
		NCompleto = GetDlgItem(Dlg, IDC_STATICBNCO); 
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_BUTTONAnt: {
			if (!auxC==0)
			{
				llenabmp(img1, auxC->imagen1, 80, 80);
				SetWindowText(numeroT, auxC->numerotelefonico);
				SetWindowText(Pnombre, auxC->nombre.primernombre);
				SetWindowText(Snombre, auxC->nombre.segundonombre);
				SetWindowText(APnombre, auxC->nombre.apellidopaterno);
				SetWindowText(AMnombre, auxC->nombre.apellidomaterno);
				SetWindowText(email, auxC->email);
				SetWindowText(calle, auxC->direccion.calle);
				SetWindowText(colonia, auxC->direccion.colonia);
				SetWindowText(numero, auxC->direccion.numero);
				SetWindowText(entrecalles, auxC->direccion.entrecalle1);
				SetWindowText(municipio, auxC->direccion.municipio);
				SetWindowText(Pais, auxC->direccion.pais);
				SetWindowText(Estado, auxC->direccion.estado);
				SetWindowText(CP, auxC->direccion.CP);
				SetWindowText(NCompleto, auxC->nombreamayus);
				auxC = auxC->ant;
			}
			else
			{
				auxC = lastNode(ini); 
			}
			return true; 
		}
		case IDC_BUTTONSig: {
			if (!auxC == 0)
			{
				llenabmp(img1, auxC->imagen1, 80, 80);
				SetWindowText(numeroT, auxC->numerotelefonico);
				SetWindowText(Pnombre, auxC->nombre.primernombre);
				SetWindowText(Snombre, auxC->nombre.segundonombre);
				SetWindowText(APnombre, auxC->nombre.apellidopaterno);
				SetWindowText(AMnombre, auxC->nombre.apellidomaterno);
				SetWindowText(email, auxC->email);
				SetWindowText(calle, auxC->direccion.calle);
				SetWindowText(colonia, auxC->direccion.colonia);
				SetWindowText(numero, auxC->direccion.numero);
				SetWindowText(entrecalles, auxC->direccion.entrecalle1);
				SetWindowText(municipio, auxC->direccion.municipio);
				SetWindowText(Pais, auxC->direccion.pais);
				SetWindowText(Estado, auxC->direccion.estado);
				SetWindowText(CP, auxC->direccion.CP);
				SetWindowText(NCompleto, auxC->nombreamayus);
				auxC = auxC->sig; 
			}
			else
			{
				auxC = ini;
			}
			return true; 
		}
		case IDC_imgant: {
			img2 = GetDlgItem(Dlg, IDC_Imagenbusqueda);
			llenabmp(img2, auxC->imagen2, 80, 80);
			return true; 
		}
		case IDC_imgsig: {
			img1 = GetDlgItem(Dlg, IDC_Imagenbusqueda);
			llenabmp(img1, auxC->imagen1, 80, 80);
			return true; 
		}
		}
		return true;
	}
	case WM_CLOSE: {
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}
BOOL CALLBACK call_Dialogo6(HWND Dlg, UINT MSG, WPARAM wParam, LPARAM lParam) {//MANUAL
	switch (MSG)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Dlg, &ps);
		GetClientRect(Dlg, &clientrect);
		bgRgn = CreateRectRgnIndirect(&clientrect);
		hBrush = CreateSolidBrush(RGB(R, G, B));
		FillRgn(hdc, bgRgn, hBrush);
		return true;
	}
	case WM_INITDIALOG: {
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{

		}
		return true;
	}
	case WM_CLOSE: {
		EndDialog(Dlg, false);
		return true;
	}
	case WM_DESTROY: {
		EndDialog(Dlg, false);
		return true;
	}
	}
	return false;
}