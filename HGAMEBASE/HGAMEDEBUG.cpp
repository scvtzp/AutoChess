#include "HGAMEDEBUG.h"

HGAMEDEBUG::HGAMEDEBUGSTART HGAMEDEBUG::Starter = HGAMEDEBUG::HGAMEDEBUGSTART();
bool HGAMEDEBUG::IsLeckCheck = false;

FILE* HGAMEDEBUG::stream;
int HGAMEDEBUG::m_FrameCount = 0;

bool HGAMEDEBUG::bCon = false;