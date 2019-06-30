#pragma once


#define DEMUX_EXPORTS __declspec(dllexport)
  
extern "C"
{

	DEMUX_EXPORTS bool DEMUX_GetDuration(const char *fileName, double *duration);

}
