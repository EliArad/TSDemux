// DemuxFileDurationApi.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DemuxFileDurationApi.h"

#include "TSDemux.h"
TSDemux tsDemux;

DEMUX_EXPORTS bool DEMUX_GetDuration(const char *fileName, double *duration)
{
	  
	return tsDemux.GetDuration(fileName, duration);

}