#pragma once
#include "bitfile.h"
#include <cstring>
#include <fstream>
#include "TSPacket.h"
#include <iostream>
#include "fifo.h"
#include <thread>
#include <memory>
#include <chrono>
#include <stdio.h>
#include <sys\timeb.h>
#include <map>
#include <vector>
#include <bitset>
#include <cstring>
#include <cinttypes>
#include <cstdint>
#include <climits>
#include <algorithm>



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
	bool TS_program_map_section();
	void video_stream_descriptor();
	void audio_stream_descriptor();



	void smoothing_buffer_descriptor();
	void maximum_bitrate_descriptor();
	void copyright_descriptor();
	void ibp_descriptor();
	void STD_descriptor();
	void multiplex_buffer_utilization_descriptor();
	void system_clock_descriptor();
	void ISO_639_language_descriptor();
	void CA_descriptor();
	void video_window_descriptor();
	void target_background_grid_descriptor();
	void data_stream_alignment_descriptor();
	void registration_descriptor();
	void private_data_indicator_descriptor();
	void hierarchy_descriptor();
	STREAM_TYPE get_stream_type(uint8_t pes_type);
	uint32_t crc32(const uint8_t* data, size_t len);

private:
	bool m_loop;
	bool m_receiveFrom;
	bool m_serverRunning;
	char m_serverIpAddress[100];
	double lastPcr;
	bool m_streaming;
	bool m_sendAsServer;
	bit_file_c bf;
	std::shared_ptr<std::thread> pthread;
	std::shared_ptr<std::thread> pserverThread;
	bool m_tsworker;
	TSPacket  m_tsp;
	PESPacket pesPacket;
	bool m_showPCR;
	SOCKET m_server;
	CFifo fifo;
	std::map<uint8_t, std::string> m_stream_types;
	std::vector<uint8_t> m_streams;
	std::bitset<0x2000> m_video_elementary_PIDs;
	std::bitset<0x2000> m_audio_elementary_PIDs;
	std::bitset<0x2000> m_klv_elementary_PIDs;
};

