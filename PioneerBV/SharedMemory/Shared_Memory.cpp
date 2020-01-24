// Shared_Memory.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "Shared_Memory.h"


//Default-Konstruktor
Shared_Memory::Shared_Memory()
{
	float_anzahl = 10;
}

//Initialisierungsfunktion für den Speicher
void Shared_Memory::SM_Init()
{
	//Reserviert Speicher mit best. Eigenschaften unter einem eindeutigen Namen
	this->map = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(float) * this->float_anzahl,(LPCTSTR)"PioneerBV");//
	
	if(this->map == NULL)
	{
		cout << "Shared Memory nicht erhalten" << endl;
		return;
	}
	
	//Sollte der Speicher bereits vorher erzeugt worden sein,...
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//wird das Mapping mit dem Namen "PioneerBV" geoeffnet
		cout << "Shared Memory bereits vorhanden...wird geoeffnet" << endl;
		this->map = OpenFileMapping(FILE_MAP_WRITE,FALSE,(LPCTSTR)"PioneerBV");
		this->mutex = CreateMutex(NULL,FALSE,(LPCTSTR)"PioneerBV");
	}

	cout << "Shared Memory reserviert..." << endl;

	//uebertragen des Mappings in den Prozessinternen Speicherbereich, sodass darauf 
	//zugegriffen werden kann
	this->com = (LPSTR)MapViewOfFile(map,FILE_MAP_WRITE,0,0,sizeof(float) * float_anzahl);
	
	if (this->com == NULL)
	{
		cout << "String nicht erhalten!" << endl;
		return;
	}
	cout << "Shared Object erzeugt ..." << endl;
}

//liefert einen Zeiger auf die Speicherzelle mit best. Index
float* Shared_Memory::SM_GetFloat(int index)
{
	if (index >= this->float_anzahl) 
		index = 0;

	if (index < 0)
		index = 0;
	
	return ((float*)this->com) + index;
}

//setzt eine Speicherzelle auf einen best. Wert
void Shared_Memory::SM_SetFloat(int index, float wert)
{
	if (index >= this->float_anzahl)
		index = 0;

	if (index < 0)
		index = 0;

	((float*)this->com)[index] = wert;
}

//Entladen des Speichers
void Shared_Memory::SM_Close()
{
	UnmapViewOfFile(this->com);
    CloseHandle(this->map);
}

//liefert die Anzahl der verfügbaren Speicherzellen
int Shared_Memory::SM_Getfloat_anzahl()
{
	return this->float_anzahl;
}