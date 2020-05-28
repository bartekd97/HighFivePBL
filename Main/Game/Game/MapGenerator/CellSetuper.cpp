#include "CellSetuper.h"


void CellSetuper::Setup()
{
	// spawn monument only on normal cell
	if (type == CellSetuper::Type::NORMAL)
		structuresConfig.mainStatuePrefab->Instantiate(cell, { 0,0,0 }, { 0,25,0 });
}
