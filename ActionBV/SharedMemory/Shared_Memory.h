#ifndef _SHARED_MEMORY_H
#define _SHARED_MEMORY_H


#include "windows.h"
#include <string>
#include <iostream>		


using namespace std;

class Shared_Memory
{

public:
	//(De-)Konstruktor
	Shared_Memory();
	~Shared_Memory();
	//Initialisierungsfunktion für den Speicher
	void SM_Init();
	//Entladen des Speichers
	void SM_Close();
	//liefert einen Zeiger auf die Speicherzelle mit best. Index
	float* SM_GetFloat(int index);
	//setzt eine Speicherzelle auf einen best. Wert
	void SM_SetFloat(int index, float wert);
	//liefert die Anzahl der verfügbaren Speicherzellen
	int SM_Getfloat_anzahl();
	
private:
	
	HANDLE map;			// Shared Memory
	HANDLE mutex;		// Synchronisationsobjekt
	int float_anzahl;	//Anzahl der zu erstellenden Float-Zellen
	LPSTR com;			//Zeiger auf den gemappten Specher
};

#endif