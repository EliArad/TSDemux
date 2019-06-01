#pragma once
#include "bitfile.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <fstream>
#include "TSPacket.h"
#include "fifo.h"
#include <thread>
#include <memory>
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
	void Process();
	bool PushData(uint8_t *buffer, uint32_t size);
	void PrintConfig(bool showPCR);
	void InitTSWorker(int packets, uint32_t fifoSize);
	void WaitWorker();
	void StopWorker();
	void Streaming(bool streamIt, char *strAddr, int port);
	 

private:
	void ReadInput(const char *fileName);
	int transport_packet();
	void adaptation_field();	
	void StreamingWaitPCR();
	bool InitUDPServer(char *strAddr, int port);



private:
	char m_serverIpAddress[100];
	double lastPcr;
	bool m_streaming;
	bit_file_c bf;
	shared_ptr<thread> pthread;
	bool m_tsworker;
	TSPacket  m_tsp;
	bool m_showPCR;
	SOCKET m_server;
	CFifo fifo;
};

