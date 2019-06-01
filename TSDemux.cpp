#include "pch.h"
#include "TSDemux.h"
#include <chrono>
#include <stdio.h>
#include <sys\timeb.h>
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library


TSDemux::TSDemux() : m_streaming(false) , m_server(-1)
{

	m_tsp.ts.packetNumber = 0;
	m_showPCR = false;	
}

void TSDemux::Streaming(bool streamIt, char *ipAddress, int port)
{
	m_streaming = streamIt;

	if (streamIt == true)
	{
		strcpy(m_serverIpAddress, ipAddress);
		InitUDPServer(m_serverIpAddress, port);
	}
}

void TSDemux::PrintConfig(bool showPCR)
{
	m_showPCR = showPCR;
}

TSDemux::~TSDemux()
{
	if (m_server != -1)
	{
		closesocket(m_server);
		m_server = -1;
	}
}

bool TSDemux::CreatePIDFile(int pid, const char *fileName)
{
	return bf.CreatePIDFile(pid, fileName);
}
void TSDemux::ReadInput(const char *fileName)
{
	 
#if 0 
	ifstream inFile;
	size_t size = 0;
	inFile.open(fileName, ios::in | ios::binary | ios::ate);

	int readSize = 1000;
	if (inFile.is_open()) {

		uint8_t* oData = 0;
		size = inFile.tellg();
		inFile.seekg(0, ios::beg);

		oData = new uint8_t[readSize];
		inFile.read((char *)oData, readSize);
		printf("0x%x\n", oData[0]);
		for (int i = 0; i < readSize; i++)
		{
			bb.write_byte(oData[i]);
		}		
	}
	  

	//std::cout << bb.read_bit(0) << std::endl;      // Prints out 1 (aka true)
	//std::cout << bb.read_bit(1) << std::endl;      // Prints out 0 (aka false)
	//std::cout << bb.read_bits(2, 2) << std::endl;  // Prints out 2
	//std::cout << bb.read_bits(4, 4) << std::endl;  // Prints out 10
	//std::cout << bb.read_byte(1) << std::endl;     // Prints out 100
	//std::cout << bb.read_bits(8, 8) << std::endl;  // Prints out 100
	//std::cout << bb.read_byte(0) << std::endl;     // 1010 1010, so it prints out 170
	//std::cout << bb.read_bytes(0, 2) << std::endl; // 1010 1010 0110 0100, so it prints out 43620

#endif 

	bf.Open(fileName);

}

void TSDemux::adaptation_field() 
{
	int N = 0;

	bf.GetBits(&m_tsp.Adp.adaptation_field_length, 8);
	bf.ResetBitCount(0);
	if (m_tsp.Adp.adaptation_field_length > 0)
	{
		m_tsp.Adp.discontinuity_indicator = bf.GetBit();
		m_tsp.Adp.random_access_indicator = bf.GetBit();
		m_tsp.Adp.elementary_stream_priority_indicator = bf.GetBit();
		m_tsp.Adp.PCR_flag = bf.GetBit();
		m_tsp.Adp.OPCR_flag = bf.GetBit();
		m_tsp.Adp.splicing_point_flag = bf.GetBit();
		m_tsp.Adp.transport_private_data_flag = bf.GetBit();
		m_tsp.Adp.adaptation_field_extension_flag = bf.GetBit();
		if (m_tsp.Adp.PCR_flag == 1) 
		{
			bf.GetBits(&m_tsp.Adp.program_clock_reference_base, 33);
			bf.GetBits(&m_tsp.Adp.reserved, 6);
			bf.GetBits(&m_tsp.Adp.program_clock_reference_extension, 9);
			m_tsp.Adp.PCR = ((double)m_tsp.Adp.program_clock_reference_base * 300.0 + m_tsp.Adp.program_clock_reference_extension) / 27000.0;
			if (m_showPCR == true) printf("PCR = %f\n", m_tsp.Adp.PCR);
		}
		if (m_tsp.Adp.OPCR_flag == 1) 
		{
			bf.GetBits(&m_tsp.Adp.original_program_clock_reference_base, 33);
			bf.GetBits(&m_tsp.Adp.reserved, 6);
			bf.GetBits(&m_tsp.Adp.original_program_clock_reference_extension, 9);
		}
		if (m_tsp.Adp.splicing_point_flag == 1) 
		{
			bf.GetBits(&m_tsp.Adp.splice_countdown, 8);
		}
		if (m_tsp.Adp.transport_private_data_flag == 1) 
		{
			bf.GetBits(&m_tsp.Adp.transport_private_data_length ,8);
			for (int i = 0; i < m_tsp.Adp.transport_private_data_length; i++)
			{
				bf.GetBits(&m_tsp.Adp.private_data_byte[i], 8);
			}
		}
		if (m_tsp.Adp.adaptation_field_extension_flag == 1)
		{
			bf.GetBits(&m_tsp.Adp.adaptation_field_extension_length, 8);
			m_tsp.Adp.ltw_flag = bf.GetBit();
			m_tsp.Adp.piecewise_rate_flag= bf.GetBit();
			m_tsp.Adp.seamless_splice_flag = bf.GetBit();
			bf.GetBits(&m_tsp.Adp.reserved, 5);
			if (m_tsp.Adp.ltw_flag == 1) 
			{
				m_tsp.Adp.ltw_valid_flag = bf.GetBit();
				bf.GetBits(&m_tsp.Adp.ltw_offset, 15);
			}
			if (m_tsp.Adp.piecewise_rate_flag == 1) 
			{
				bf.GetBits(&m_tsp.Adp.reserved, 2);
				bf.GetBits(&m_tsp.Adp.piecewise_rate, 22);
			}
			if (m_tsp.Adp.seamless_splice_flag == 1)
			{
				bf.GetBits(&m_tsp.Adp.splice_type, 4);
				uint32_t DTS_next_AU_32_30;
				bf.GetBits(&DTS_next_AU_32_30, 3);
				m_tsp.Adp.marker_bit = bf.GetBit();
				uint32_t DTS_next_AU_29_15; 
				bf.GetBits(&DTS_next_AU_29_15, 15);
				m_tsp.Adp.marker_bit = bf.GetBit();
				uint32_t DTS_next_AU_0_14;
				bf.GetBits(&DTS_next_AU_0_14, 15);
				m_tsp.Adp.marker_bit = bf.GetBit();
			}
			for (int i = 0; i < N; i++) {
				bf.GetBits(&m_tsp.Adp.reserved, 8);
			}
		}
		N = m_tsp.Adp.adaptation_field_length - bf.GetBitCounter(0) / 8;
		for (int i = 0; i < N; i++)
		{
			bf.GetBits(&m_tsp.Adp.stuffing_byte, 8);
		}
	}
}
 
int TSDemux::transport_packet()
{
	int N = 0;
	bf.ResetBitCount();
	m_tsp.ts.packetNumber++;
	if (bf.GetChar(&m_tsp.ts.sync_byte) == -1)
	{
		printf("EOF\n");
		return 0;
	}
	if (m_tsp.ts.sync_byte != 0x47)
	{
		printf("Error in sync byte at packet %d\n" , m_tsp.ts.packetNumber);
		return 0;
	}
	m_tsp.ts.transport_error_indicator = bf.GetBit();
	m_tsp.ts.payload_unit_start_indicator = bf.GetBit();
	m_tsp.ts.transport_priority = bf.GetBit();
	bf.GetBits(&m_tsp.ts.PID , 13);	
	bf.GetBits(&m_tsp.ts.transport_scrambling_control ,2);
	bf.GetBits(&m_tsp.ts.adaptation_field_control , 2);
	bf.GetBits(&m_tsp.ts.continuity_counter ,4);
	if (m_tsp.ts.adaptation_field_control == 2 || m_tsp.ts.adaptation_field_control == 3)
	{
		adaptation_field();
	}
	int bc = bf.GetBitCount();
	N = 188 - bc / 8;
	uint8_t temp;
	uint8_t buffer[188];
	if (m_tsp.ts.adaptation_field_control == 1 || m_tsp.ts.adaptation_field_control == 3)
	{
		if (bf.GetTSBuffer(buffer, N) == false)
		{
			printf("EOF\n");
			return -1;
		}
		if (bf.m_pidToSave[m_tsp.ts.PID] != -1)
		{
			bf.m_OutStream[m_tsp.ts.PID]->write((char*)&buffer, N);
		}		
	} 
	else
	{
		bf.MoveAhead(N);
		#if 0 
		for (int i = 0; i < N; i++)
		{
			if (bf.GetChar(&temp) == -1)
			{
				printf("EOF\n");
				return -1;
			}
		}
		#endif 
	}
	return 1;
		  
}

struct timeb start;
void TakeTimeStart()
{ 
	int i = 0;
	ftime(&start);
}

int diffTime()
{
	struct timeb end;
	int diff;
	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));

	//printf("\nOperation took %u milliseconds\n", diff);
	return diff;
}

void TSDemux::StreamingWaitPCR()
{
	if (lastPcr == 0)
	{
		lastPcr = m_tsp.Adp.PCR;
		return;
	}

	double pcrDiff = m_tsp.Adp.PCR - lastPcr;
	if (pcrDiff > 0)
	{
		int diff = diffTime();
		while (diff < pcrDiff)
		{
			Sleep(1);
			diff = diffTime();
			//printf(".");
		}
		//printf("\n");
		TakeTimeStart();


	}
	lastPcr = m_tsp.Adp.PCR;
}
void ShowCurrentTime()
{
	time_t mytime = time(NULL);
	char * time_str = ctime(&mytime);
	time_str[strlen(time_str) - 1] = '\0';
	printf("Current Time : %s\n", time_str);
}
void TSDemux::Start(const char *fileName)
{
	lastPcr = 0;
	ReadInput(fileName);
	TakeTimeStart();
	ShowCurrentTime();
	

	do 
	{
		if (transport_packet() == 0)
		{
			ShowCurrentTime();
			break;
		}

		if (m_streaming)
		{
			StreamingWaitPCR();			
		}  		 
	} while (true);
	 
}

void TSDemux::Start(uint8_t *buffer, uint32_t size)
{
	bf.SetBuffer(buffer, size);
 
	do
	{
		if (transport_packet() == 0)
			break;

	} while (true);

}

void TSDemux::SetBuffer(uint8_t *buffer, uint32_t size)
{
	bf.SetBuffer(buffer, size);
}

void TSDemux::InitTSWorker(int packets, uint32_t fifoSize)
{
	bf.SetExternalBuffer(packets);
	fifo.Create(fifoSize);
	m_tsworker = true;
	pthread = std::make_shared<std::thread>(std::bind(&TSDemux::Process, this));

}

bool TSDemux::PushData(uint8_t *buffer, uint32_t size)
{
	return fifo.Push(buffer, size);
}
void TSDemux::WaitWorker()
{
	if (pthread != nullptr)
		pthread->join();
}
void TSDemux::StopWorker()
{
	m_tsworker = false;
	if (pthread != nullptr)
		pthread->join();
	pthread = nullptr;

}
void TSDemux::Process()
{ 	
	
	int packets;
	int maxPacketSize = bf.GetPacketSize();
	 
	while (m_tsworker)
	{
		if (fifo.PopTS(bf.GetBuffer(), maxPacketSize, &packets) == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		for (int i = 0; i < packets; i++)
		{
			if (transport_packet() == 0)
			{
				return;
			}
		}
	}
}

bool TSDemux::InitUDPServer(char *strAddr, int port)
{

	
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	 
	WSADATA wsa;

	slen = sizeof(si_other);

 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}
 
	//Create a socket
	if ((m_server = ::socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		return false;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;	
	DWORD ip = inet_addr(strAddr);
	server.sin_addr.s_addr = ip;
	server.sin_port = htons(port);

	//Bind
	if (::bind(m_server, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

