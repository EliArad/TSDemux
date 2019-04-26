#pragma once
#include "bitfile.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <fstream>
#include "TSPacket.h"

using namespace std;

#define SYNC_BYTE 0x47

 

class TSDemux
{
public:
	TSDemux();
	~TSDemux();

	void Start(const char *fileName);
	void Start(uint8_t *buffer, uint32_t size);
	bool CreatePIDFile(int pid, const char *fileName);
	void SetBuffer(uint8_t *p, uint32_t size);
	void Process(uint8_t *buffer, uint32_t size);
	void PrintConfig(bool showPCR);
  
private:
	void ReadInput(const char *fileName);
	int transport_packet();
	void adaptation_field();	


private:
	bit_file_c bf;

	TSPacket  m_tsp;
	bool m_showPCR;
};

