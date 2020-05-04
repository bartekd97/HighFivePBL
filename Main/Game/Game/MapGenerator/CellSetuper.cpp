#include "CellSetuper.h"


void CellSetuper::Setup(GameObject cell)
{
	// TODO: finish it all here
	structuresConfig.mainStatuePrefab->Instantiate(cell, { 0,0,0 }, { 0,25,0 });
}
