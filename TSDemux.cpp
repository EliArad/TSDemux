#include "pch.h"
#include "TSDemux.h"



TSDemux::TSDemux()
{

	m_tsp.ts.packetNumber = 0;
}


TSDemux::~TSDemux()
{
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

	bf.Open(fileName, BF_READ);

}

void TSDemux::adaptation_field() 
{
	int N = 0;
	bf.GetBits(&m_tsp.Adp.adaptation_field_length, 8);
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
			printf("PCR = %f\n", m_tsp.Adp.PCR);
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
	if (m_tsp.ts.adaptation_field_control == 1 || m_tsp.ts.adaptation_field_control == 3)
	{
		for (int i = 0; i < N; i++) 
		{
			//data_byte 8 bslbf
		}
	}
	int bc = bf.GetBitCount();
	N = 188 -  bc / 8;
	uint8_t temp;
	for (int i = 0 ; i < N ; i++)
	{
		if (bf.GetChar(&temp) == -1)
		{
			printf("EOF\n");
			return -1;
		}
	}
	return 1;
		  
}
void TSDemux::Start(const char *fileName)
{
	  
	ReadInput(fileName);

	do 
	{
		if (transport_packet() == 0)
			break;
  		 
	} while (true);
	 
}

