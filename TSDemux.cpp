#include "pch.h"
#include "TSDemux.h"
#include <chrono>
#include <stdio.h>
#include <sys\timeb.h>
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library


TSDemux::TSDemux() : m_streaming(false) , 
					 m_server(-1),
					 m_loop(false)
{

	m_tsp.ts.packetNumber = 0;
	m_showPCR = false;	
}

void TSDemux::Streaming(bool streamIt, char *ipAddress, int port, bool server)
{
	m_streaming = streamIt;
	if (streamIt == true)
	{		
		strcpy(m_serverIpAddress, ipAddress);
		InitUDPServer(m_serverIpAddress, port, server);
	
		
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

void TSDemux::TS_program_map_section() 
{
	int N = 0;
	int N1 = 0;
	int N2 = 0;
	uint8_t temp;
	bf.GetBits(&temp, 8);
	bf.ResetBitCount(9);
	bf.GetBits(&m_tsp.pmt.table_id, 8);
	bf.GetBits(&m_tsp.pmt.section_syntax_indicator, 1);
	bf.GetBits(&temp, 1); //'0' 1 bslbf
	bf.GetBits(&m_tsp.pmt.reserved, 2);
	bf.GetBits(&m_tsp.pmt.section_length, 12);
	bf.GetBits(&m_tsp.pmt.program_number, 16);
	bf.GetBits(&m_tsp.pmt.reserved, 2);
	bf.GetBits(&m_tsp.pmt.version_number, 5);
	bf.GetBits(&m_tsp.pmt.current_next_indicator, 1);
	bf.GetBits(&m_tsp.pmt.section_number, 8);
	bf.GetBits(&m_tsp.pmt.last_section_number, 8);
	bf.GetBits(&m_tsp.pmt.reserved, 3);
	bf.GetBits(&m_tsp.pmt.PCR_PID, 13);
	bf.GetBits(&m_tsp.pmt.reserved, 4);
	bf.GetBits(&m_tsp.pmt.program_info_length, 12);
	int x = bf.GetBitCounter(9);
	N = m_tsp.pmt.section_length - x / 8;
	for (int i = 0; i < N; i++) 
	{
		//descriptor()
	}
	for (int i = 0; i < N1; i++)
	{
		bf.GetBits(&m_tsp.pmt.stream_type[i], 8);
		bf.GetBits(&m_tsp.pmt.reserved, 3);
		bf.GetBits(&m_tsp.pmt.elementary_PID[i], 13);
		bf.GetBits(&m_tsp.pmt.reserved, 4);
		bf.GetBits(&m_tsp.pmt.ES_info_length[i], 12);
		for (int i = 0; i < N2; i++) 
		{
			//descriptor();
		}
	}
	bf.GetBits(&m_tsp.pmt.CRC_32, 32);
	
}

void TSDemux::program_association_section()
{
	//printf("PAT\n");
	int N = 0;
	uint8_t temp;	
	bf.GetBits(&temp, 8);	// I dont know why this byte is here!!!!
	bf.ResetBitCount(9);
	bf.GetBits(&m_tsp.pat.table_id, 8);
	bf.GetBits(&m_tsp.pat.section_syntax_indicator, 1);
	bf.GetBits(&temp, 1); //'0' 1 bslbf
	bf.GetBits(&m_tsp.pat.reserved, 2);
	bf.GetBits(&m_tsp.pat.section_length , 12);
	bf.GetBits(&m_tsp.pat.transport_stream_id, 16);	 
	bf.GetBits(&m_tsp.pat.reserved, 2);
	bf.GetBits(&m_tsp.pat.version_number, 5);
	bf.GetBits(&m_tsp.pat.current_next_indicator, 1);
	bf.GetBits(&m_tsp.pat.section_number , 8);
	bf.GetBits(&m_tsp.pat.last_section_number, 8);
	int x = bf.GetBitCounter(9) + 32;
	N = m_tsp.pat.section_length - x / 8;
	for (int i = 0; i < N; i++) 
	{
		bf.GetBits(&m_tsp.pat.program_number[i], 16);
		bf.GetBits(&m_tsp.pat.reserved, 3);
		if (m_tsp.pat.program_number[i] == 0) 
		{
			bf.GetBits(&m_tsp.pat.network_PID[i], 13);
		}
		else 
		{
			bf.GetBits(&m_tsp.pat.program_map_PID[i], 13);
			//printf("PMT PID %d\n" , m_tsp.pat.program_map_PID[i]);
		}		 
	}
	bf.GetBits(&m_tsp.pat.CRC_32, 32);

	N = (188 *8 - (bf.GetBitCounter(9) + 32 + 8)) / 8;
	for (int i = 0; i < N; i++)
		bf.GetBits(&temp, 8);

}

bool TSDemux::PES_packet()
{

	int N1 = 0;
	int N2 = 0;
	uint8_t temp;
	bf.CheckBits(&m_tsp.pesPacket.packet_start_code_prefix, 24);
	if (m_tsp.pesPacket.packet_start_code_prefix != 1)
		return false;
	bf.GetBits(&m_tsp.pesPacket.packet_start_code_prefix, 24);
	bf.GetBits(&m_tsp.pesPacket.stream_id, 8);
	bf.GetBits(&m_tsp.pesPacket.PES_packet_length, 16);
	if (m_tsp.pesPacket.stream_id != program_stream_map
		&& m_tsp.pesPacket.stream_id != padding_stream
		&& m_tsp.pesPacket.stream_id != private_stream_2
		&& m_tsp.pesPacket.stream_id != ECM_stream
		&& m_tsp.pesPacket.stream_id != EMM_stream
		&& m_tsp.pesPacket.stream_id != program_stream_directory
		&& m_tsp.pesPacket.stream_id != DSMCC_stream
		&& m_tsp.pesPacket.stream_id != ITU_T_Rec_H_222_1_type_E_Stream)
	{
		bf.GetBits(&temp, 2);
		if (temp != 0x2)
		{
			printf("error in pes packect - expected 0x2 here\n");
		}

		bf.GetBits(&m_tsp.pesPacket.PES_scrambling_control, 2);
		bf.GetBits(&m_tsp.pesPacket.PES_priority, 1);
		bf.GetBits(&m_tsp.pesPacket.data_alignment_indicator, 1);
		bf.GetBits(&m_tsp.pesPacket.copyright, 1);
		bf.GetBits(&m_tsp.pesPacket.original_or_copy, 1);
		bf.GetBits(&m_tsp.pesPacket.PTS_DTS_flags, 2);
		bf.GetBits(&m_tsp.pesPacket.ESCR_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.ES_rate_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.DSM_trick_mode_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.additional_copy_info_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.PES_CRC_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.PES_extension_flag, 1);
		bf.GetBits(&m_tsp.pesPacket.PES_header_data_length, 8);
		if (m_tsp.pesPacket.PTS_DTS_flags == 0x2)
		{
			bf.GetBits(&temp, 4);
			if (temp != 0x2)
			{
				printf("error in pes packect - expected 0x2 here\n");
			}

			bf.GetBits(&m_tsp.pesPacket.PTS2, 3);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.PTS1, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.PTS0, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
		}

		if (m_tsp.pesPacket.PTS_DTS_flags == 0x3)
		{
			bf.GetBits(&temp, 4);
			if (temp != 0x3)
			{
				printf("error in pes packect - expected 0x3 here\n");
			}

			bf.GetBits(&m_tsp.pesPacket.PTS2, 3);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.PTS1, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.PTS0, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&temp, 4);
			if (temp != 0x1)
			{
				printf("error in pes packect - expected 0x3 here\n");
			}

			bf.GetBits(&m_tsp.pesPacket.DTS2, 3);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.DTS1, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.DTS0, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
		}
		if (m_tsp.pesPacket.ESCR_flag == '1')
		{
			bf.GetBits(&m_tsp.pesPacket.reserved, 2);
			bf.GetBits(&m_tsp.pesPacket.ESCR_base2, 3);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.ESCR_base1, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.ESCR_base0, 15);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.ESCR_extension, 9);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
		}
		if (m_tsp.pesPacket.ES_rate_flag == 1)
		{
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.ES_rate, 22);
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
		}
		if (m_tsp.pesPacket.DSM_trick_mode_flag == 1) {
			bf.GetBits(&m_tsp.pesPacket.trick_mode_control, 3);
			if (m_tsp.pesPacket.trick_mode_control == fast_forward) {
				bf.GetBits(&m_tsp.pesPacket.field_id, 2);
				bf.GetBits(&m_tsp.pesPacket.intra_slice_refresh, 1);
				bf.GetBits(&m_tsp.pesPacket.frequency_truncation, 2);
			}
			else if (m_tsp.pesPacket.trick_mode_control == slow_motion) {
				bf.GetBits(&m_tsp.pesPacket.rep_cntrl, 5);
			}
			else if (m_tsp.pesPacket.trick_mode_control == freeze_frame) {
				bf.GetBits(&m_tsp.pesPacket.field_id, 2);
				bf.GetBits(&m_tsp.pesPacket.reserved, 3);
			}
			else if (m_tsp.pesPacket.trick_mode_control == fast_reverse)
			{
				bf.GetBits(&m_tsp.pesPacket.field_id, 2);
				bf.GetBits(&m_tsp.pesPacket.intra_slice_refresh, 1);
				bf.GetBits(&m_tsp.pesPacket.frequency_truncation, 2);
			}
			else if (m_tsp.pesPacket.trick_mode_control == slow_reverse)
			{
				bf.GetBits(&m_tsp.pesPacket.rep_cntrl, 5);
			}
			else
				bf.GetBits(&m_tsp.pesPacket.reserved, 5);
		}
		if (m_tsp.pesPacket.additional_copy_info_flag == 1)
		{
			bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
			bf.GetBits(&m_tsp.pesPacket.additional_copy_info, 7);
		}
		if (m_tsp.pesPacket.PES_CRC_flag == 1)
		{
			bf.GetBits(&m_tsp.pesPacket.previous_PES_packet_CRC, 16);
		}
		if (m_tsp.pesPacket.PES_extension_flag == 1)
		{
			bf.GetBits(&m_tsp.pesPacket.PES_private_data_flag, 1);
			bf.GetBits(&m_tsp.pesPacket.pack_header_field_flag, 1);
			bf.GetBits(&m_tsp.pesPacket.program_packet_sequence_counter_flag, 1);
			bf.GetBits(&m_tsp.pesPacket.P_STD_buffer_flag, 1);
			bf.GetBits(&m_tsp.pesPacket.reserved, 3);
			bf.GetBits(&m_tsp.pesPacket.PES_extension_flag_2, 1);
			if (m_tsp.pesPacket.PES_private_data_flag == 1)
			{
				throw ("not supported here");
			}
			if (m_tsp.pesPacket.pack_header_field_flag == '1')
			{
				bf.GetBits(&m_tsp.pesPacket.pack_field_length, 8);
				throw ("no supported");
				pack_header();
			}
			if (m_tsp.pesPacket.program_packet_sequence_counter_flag == 1) 
			{
				bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
				bf.GetBits(&m_tsp.pesPacket.program_packet_sequence_counter, 7);
				bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
				bf.GetBits(&m_tsp.pesPacket.MPEG1_MPEG2_identifier, 1);
				bf.GetBits(&m_tsp.pesPacket.original_stuff_length, 6);
			}
			if (m_tsp.pesPacket.P_STD_buffer_flag == 1)
			{
				bf.GetBits(&temp, 2);
				if (temp != 0x1)
				{
					printf("error here 414334");
				}
				bf.GetBits(&m_tsp.pesPacket.P_STD_buffer_scale, 1);
				bf.GetBits(&m_tsp.pesPacket.P_STD_buffer_size, 13);
			}
			if (m_tsp.pesPacket.PES_extension_flag_2 == 1) {
				bf.GetBits(&m_tsp.pesPacket.marker_bit, 1);
				bf.GetBits(&m_tsp.pesPacket.PES_extension_field_length, 7);
				for (int i = 0; i < m_tsp.pesPacket.PES_extension_field_length; i++)
				{
					bf.GetBits(&m_tsp.pesPacket.reserved, 8);
				}
			}
		}
		for (int i = 0; i < N1; i++) 
		{
			bf.GetBits(&m_tsp.pesPacket.stuffing_byte, 8);
		}
		for (int i = 0; i < N2; i++) 
		{
			bf.GetBits(&m_tsp.pesPacket.PES_packet_data_byte, 8);
		}
	}
	else if (m_tsp.pesPacket.stream_id == program_stream_map
		|| m_tsp.pesPacket.stream_id == private_stream_2
		|| m_tsp.pesPacket.stream_id == ECM_stream
		|| m_tsp.pesPacket.stream_id == EMM_stream
		|| m_tsp.pesPacket.stream_id == program_stream_directory
		|| m_tsp.pesPacket.stream_id == DSMCC_stream
		|| m_tsp.pesPacket.stream_id == ITU_T_Rec_H_222_1_type_E_Stream)
	{
		for (int i = 0; i < m_tsp.pesPacket.PES_packet_length; i++)
		{
			//PES_packet_data_byte 8 bslbf
		}
	}
	else if (m_tsp.pesPacket.stream_id == padding_stream)
	{
		for (int i = 0; i < m_tsp.pesPacket.PES_packet_length; i++)
		{
			bf.GetBits(&m_tsp.pesPacket.padding_byte, 8);
		}
	}
}


void TSDemux::pack_header()
{

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
		if (m_tsp.ts.PID == 0)
		{
			program_association_section();
			return 1;
		} 
		for (int i = 0; i < MAX_PMTs; i++)
		{
			if (m_tsp.ts.PID == m_tsp.pat.program_map_PID[i])
			{
				//TS_program_map_section();
				bf.MoveAhead(N);
				return 1;
			}
		}
	 
		PES_packet();
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
	char sendBuf[188 * 7];
	int  sendIndex = 0;

	do 
	{
		bf.SaveFilePointerStart();
		if (transport_packet() == 0)
		{
			ShowCurrentTime();
			if (m_loop == true)
			{
				bf.Close();
				bf.Open(fileName);
				continue;
			}
			else 
			{
				break;
			}
		}

		if (m_streaming)
		{
			StreamingWaitPCR();
			uint32_t size;
			uint32_t fileStartPointer, fileCurPointer;
			bf.GetFilePtrIndex(&size, &fileStartPointer, &fileCurPointer);
			bf.GetFileData(fileStartPointer, size, sendBuf + 188 * sendIndex);
			sendIndex++;
			if (sendIndex == 7)
			{
				if (m_sendAsServer == false)
					SendData(sendBuf, 1316);
				else 
					SendDataTo(sendBuf, 1316);
				sendIndex = 0;
			}
			//printf("%d\n", size);
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
#include <sys/types.h>
 

struct sockaddr src_addr;
int fromlen;

void TSDemux::ServerNagtation()
{
	m_serverRunning = true;
	char buf[110];
	
	while (m_serverRunning)
	{
		if (m_server == -1)
		{
			Sleep(1000);
			continue;
		}
		int res = recvfrom(m_server, buf, 10, 0 , &src_addr, &fromlen);
		if (res == -1)
		{
			int x = WSAGetLastError();
			printf("error in send %d\n", x);
			continue;
		}
		if (memcmp(buf , "Eli Arad", strlen("Eli Arad")) == 0)
			m_receiveFrom = true;
		memset(buf, 0, sizeof(buf));
	}

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

bool TSDemux::SendDataTo(const char *buf , int size)
{
	if (m_receiveFrom == false)
		return false;

	if (::sendto(m_server, buf, size, 0, &src_addr, fromlen) == SOCKET_ERROR)
	{
		int x = WSAGetLastError();
		printf("error in send %d\n", x);
		return false;
	}
	return true;

}

bool TSDemux::SendData(const char *buf, int size)
{
	if (::send(m_server, buf, size, 0) == SOCKET_ERROR)
	{
		int x = WSAGetLastError();
		printf("error in send %d\n", x);
		return false;
	}
	return true;

}

bool TSDemux::InitUDPServer(char *strAddr, int port, bool asServer)
{

	
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	 
	WSADATA wsa;
	m_sendAsServer = asServer;
	slen = sizeof(si_other);

 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}
 
	
	if ((m_server = ::socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		return false;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;	
	DWORD ip = inet_addr(strAddr);
	server.sin_addr.s_addr = ip;
	server.sin_port = htons(port);

	if (asServer == true)
	{
		//Bind
		if (::bind(m_server, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		{
			return false;
		}
		pserverThread = std::make_shared<std::thread>(std::bind(&TSDemux::ServerNagtation, this));
	}
	else
	{
		if (::connect(m_server, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		{
			return false;
		}
	}
	
	return true;
}

