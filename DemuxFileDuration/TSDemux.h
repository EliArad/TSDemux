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

	bool GetDuration(const char *fileName, double &duration);
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
	void Streaming(bool streamIt, char *strAddr, int port, bool server);
	void Loop(bool l)
	{
		m_loop = l;
	}

private:
	void ReadInput(const char *fileName);
	int transport_packet();
	void adaptation_field();	
	void StreamingWaitPCR();
	bool InitUDPServer(char *strAddr, int port, bool server);
	bool SendData(const char *buf, int size);
	bool SendDataTo(const char *buf, int size);
	void ServerNagtation();
	bool PES_packet();
	void pack_header();
	void program_association_section();
	void TS_program_map_section();

private:
	double m_startPCR;
	bool m_loop;
	bool m_receiveFrom;
	bool m_serverRunning;
	char m_serverIpAddress[100];
	double lastPcr;
	bool m_streaming;
	bool m_sendAsServer;
	bit_file_c bf;
	shared_ptr<thread> pthread;
	shared_ptr<thread> pserverThread;	
	bool m_tsworker;
	TSPacket  m_tsp;
	PESPacket pesPacket;
	bool m_showPCR;
	SOCKET m_server;
	CFifo fifo;
};

