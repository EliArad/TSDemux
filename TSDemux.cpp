#include "pch.h"
#include "TSDemux.h"

 
#pragma comment(lib,"ws2_32.lib") //Winsock Library

static const uint32_t static_crc_table[256] = {
		0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
		0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
		0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
		0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
		0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
		0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
		0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
		0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
		0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
		0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
		0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
		0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
		0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
		0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
		0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
		0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
		0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
		0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
		0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
		0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
		0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
		0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
		0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
		0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
		0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
		0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
		0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
		0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
		0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
		0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
		0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
		0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
		0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
		0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
		0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
		0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
		0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
		0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
		0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
		0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
		0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
		0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
		0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
		0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
		0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
		0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
		0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
		0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
		0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
		0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
		0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
		0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
		0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
		0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
		0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
		0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
		0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
		0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
		0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
		0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
		0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
		0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
		0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
		0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


TSDemux::TSDemux() : m_streaming(false) , 
					 m_server(-1),
					 m_loop(false)
{

	m_tsp.ts.packetNumber = 0;
	m_showPCR = false;	

	
	m_stream_types[0x01] = "MPEG2VIDEO|ISO/IEC 11172-2 Video";	// v
	m_stream_types[0x02] = "MPEG2VIDEO|ISO/IEC 13818-2 Video";	// v
	m_stream_types[0x03] = "MP3|ISO/IEC 11172-3 Audio";
	m_stream_types[0x04] = "MP3|ISO/IEC 13818-3 Audio";
	m_stream_types[0x05] = "ISO/IEC 13818-1 PRIVATE SECTION";
	m_stream_types[0x06] = "ISO/IEC 13818-1 PES";
	m_stream_types[0x07] = "ISO/IEC 13522 MHEG";
	m_stream_types[0x08] = "ISO/IEC 13818-1 Annex A DSM-CC";
	m_stream_types[0x09] = "ITU-T Rec.H.222.1";
	m_stream_types[0x0a] = "ISO/IEC 13818-6 type A";
	m_stream_types[0x0b] = "ISO/IEC 13818-6 type B";
	m_stream_types[0x0c] = "ISO/IEC 13818-6 type C";
	m_stream_types[0x0d] = "ISO/IEC 13818-6 type D";
	m_stream_types[0x0e] = "ISO/IEC 13818-1 AUXILIARY";
	m_stream_types[0x0f] = "AAC"; // AAC|ISO/IEC 13818-7 Audio with ADTS transport syntax
	m_stream_types[0x10] = "MPEG4|ISO/IEC 14496-2 Visual";			// v
	m_stream_types[0x11] = "LATM|ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
	m_stream_types[0x12] = "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
	m_stream_types[0x13] = "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections";
	m_stream_types[0x14] = "ISO/IEC 13818-6 Synchronized Download Protocol";
	m_stream_types[0x1b] = "H264";			// v
	m_stream_types[0x20] = "H264";			// v
	m_stream_types[0x24] = "HEVC";			// v
	m_stream_types[0x42] = "CAVS";			// v
	m_stream_types[0xd1] = "DIRAC";			// v
	m_stream_types[0xea] = "VC1";			// v

	m_stream_types[0x80] = "PCM_BLURAY";
	m_stream_types[0x81] = "AC3|DOLBY_AC3_AUDIO";
	m_stream_types[0x82] = "DTS";
	m_stream_types[0x83] = "TRUEHD";
	m_stream_types[0x84] = "EAC3";
	m_stream_types[0x85] = "DTS";
	m_stream_types[0x86] = "DTS";
	m_stream_types[0x8a] = "DTS";
	m_stream_types[0xa1] = "EAC3";
	m_stream_types[0xa2] = "DTS";
	m_stream_types[0x90] = "HDMV_PGS_SUBTITLE";
	
	m_streams.resize(0x2000, 0);


}


uint32_t TSDemux::crc32(const uint8_t* data, size_t len)
{
	uint32_t crc = 0xffffffff;
	for (int i = 0; i < len; i++)
		crc = (crc << 8) ^ static_crc_table[(crc >> 24) ^ (data[i])];
	return crc;
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


void TSDemux::smoothing_buffer_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		reserved 2 bslbf
		sb_leak_rate 22 uimsbf
		reserved 2 bslbf
		sb_size 22 uimsbf
		*/
}


void TSDemux::private_data_indicator_descriptor() 
{
	/*
	descriptor_tag 38 uimsbf
		descriptor_length 38 uimsbf
		private_data_indicator 32 uimsbf
		*/
}


void TSDemux::maximum_bitrate_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		reserved 2 bslbf
		maximum_bitrate 22 uimsbf
		*/
}

void TSDemux::copyright_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
	descriptor_length 8 uimsbf
	copyright_identifier 32 uimsbf
	for (i = 0; i < N; i++) {
	additional_copyright_info 8 bslbf
	}
	*/
}


void TSDemux::ibp_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		closed_gop_flag 1 uimsbf
		identical_gop_flag 1 uimsbf
		max_gop - length 14 uimsbf
		*/
}


void TSDemux::STD_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		reserved 7 bslbf
		leak_valid_flag 1 bslbf
		*/
}


void TSDemux::multiplex_buffer_utilization_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		bound_valid_flag 1 bslbf
		LTW_offset_lower_bound 15 uimsbf
		reserved 1 bslbf
		LTW_offset_upper_bound 14 uimsbf
		*/
}


void TSDemux::system_clock_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		external_clock_reference_indicator 1 bslbf
		reserved 1 bslbf
		clock_accuracy_integer 6 uimsbf
		clock_accuracy_exponent 3 uimsbf
		reserved 5 bslbf
		*/
}


void TSDemux::ISO_639_language_descriptor() 
{	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		for (i = 0; i < N; i++) {
			ISO_639_language_code 24 bslbf
				audio_type 8 bslbf
		}
		*/
}

void TSDemux::CA_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		CA_system_ID 16 uimsbf
		reserved 3 bslbf
		CA_PID 13 uimsbf
		for (i = 0; i < N; i++) {
			private_data_byte 8 uimsbf
		}
		*/
}

void TSDemux::video_window_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		horizontal_offset 14 uimsbf
		vertical_offset 14 uimsbf
		window_priority 4 uimsbf
		*/
}

void TSDemux::target_background_grid_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
	descriptor_length 8 uimsbf
	horizontal_size 14 uimsbf
	vertical_size 14 uimsbf
	aspect_ratio_information 4 uimsbf
	*/
}


void TSDemux::data_stream_alignment_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		alignment_type 8 uimsbf
	*/
}

void TSDemux::registration_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		format_identifier 32 uimsbf
		for (i = 0; i < N; i++) {
			additional_identification_info 8 bslbf
		}
	*/
}


void TSDemux::hierarchy_descriptor() 
{
	/*
	descriptor_tag 8 uimsbf
		descriptor_length 8 uimsbf
		reserved 4 bslbf
		hierarchy_type 4 uimsbf
		reserved 2 bslbf
		hierarchy_layer_index 6 uimsbf
		reserved 2 bslbf
		hierarchy_embedded_layer_index 6 uimsbf
		reserved 2 bslbf
		hierarchy_channel 6 uimsbf
		*/
}

void TSDemux::audio_stream_descriptor() 
{
	bf.GetBits(&m_tsp.pmt.descriptor_tag, 8);
	bf.GetBits(&m_tsp.pmt.descriptor_length, 8);
	bf.GetBits(&m_tsp.pmt.free_format_flag, 1);
	bf.GetBits(&m_tsp.pmt.ID, 1);
	bf.GetBits(&m_tsp.pmt.layer, 2);
	bf.GetBits(&m_tsp.pmt.variable_rate_audio_indicator, 1);
	bf.GetBits(&m_tsp.pmt.reserved ,3);
}
void TSDemux::video_stream_descriptor()
{

	bf.GetBits(&m_tsp.pmt.descriptor_tag, 8);
	bf.GetBits(&m_tsp.pmt.descriptor_length, 8);
	bf.GetBits(&m_tsp.pmt.multiple_frame_rate_flag, 1);
	bf.GetBits(&m_tsp.pmt.frame_rate_code, 4);
	bf.GetBits(&m_tsp.pmt.MPEG_1_only_flag, 1);
	bf.GetBits(&m_tsp.pmt.constrained_parameter_flag, 1);
	bf.GetBits(&m_tsp.pmt.still_picture_flag, 1);
	if (m_tsp.pmt.MPEG_1_only_flag == 0)
	{
		bf.GetBits(&m_tsp.pmt.profile_and_level_indication, 8);
		bf.GetBits(&m_tsp.pmt.chroma_format, 2);
		bf.GetBits(&m_tsp.pmt.frame_rate_extension_flag, 1);
		bf.GetBits(&m_tsp.pmt.reserved, 5);
	}

}

bool TSDemux::TS_program_map_section() 
{
	int N = 0;
	 
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

	int N1 = m_tsp.pmt.section_length - (9 + 4 + m_tsp.pmt.program_info_length);
	if (N1 >= TS_SIZE - TS_HEADER_SIZE || N1 < 0)
	{
		std::cerr << "parse N1 error, N1 = ";
		return false;
	}

	
	for (int n = 0; n < N1;)
	{
		bf.GetBits(&m_tsp.pmt.stream_type, 8);
		STREAM_TYPE streamType = get_stream_type(m_tsp.pmt.stream_type);
		if (streamType != STREAM_TYPE_UNKNOWN)
		{
			bf.GetBits(&m_tsp.pmt.reserved, 3);
			bf.GetBits(&m_tsp.pmt.elementary_PID, 13);
			bf.GetBits(&m_tsp.pmt.reserved, 4);
			bf.GetBits(&m_tsp.pmt.ES_info_length, 12);
			  
			n += (5 + m_tsp.pmt.ES_info_length);

		 
			m_streams[m_tsp.pmt.elementary_PID] = m_tsp.pmt.stream_type;
			if (m_tsp.pmt.stream_type == 0x1b || m_tsp.pmt.stream_type == 0x20)
				m_streams[m_tsp.pmt.elementary_PID] = video_h264;
			else if (m_tsp.pmt.stream_type == 0x24)
				m_streams[m_tsp.pmt.elementary_PID] = video_hevc;

			if (m_tsp.pmt.stream_type == 0x15)
			{
				m_klv_elementary_PIDs.set(m_tsp.pmt.elementary_PID);

			}
			else
			if (m_tsp.pmt.stream_type == 0x01 || m_tsp.pmt.stream_type == 0x02 ||
				m_tsp.pmt.stream_type == 0x1b || m_tsp.pmt.stream_type == 0x20 ||
				m_tsp.pmt.stream_type == 0x10 || m_tsp.pmt.stream_type == 0x24 ||
				m_tsp.pmt.stream_type == 0x42 || m_tsp.pmt.stream_type == 0xd1 ||
				m_tsp.pmt.stream_type == 0xea)
			{
				m_video_elementary_PIDs.set(m_tsp.pmt.elementary_PID);
			}
			else if (
				m_tsp.pmt.stream_type == 0x03 || m_tsp.pmt.stream_type == 0x04 ||
				m_tsp.pmt.stream_type == 0x0f || m_tsp.pmt.stream_type == 0x11 ||
				m_tsp.pmt.stream_type == 0x80 || m_tsp.pmt.stream_type == 0x81 ||
				m_tsp.pmt.stream_type == 0x82 || m_tsp.pmt.stream_type == 0x83 ||
				m_tsp.pmt.stream_type == 0x84 || m_tsp.pmt.stream_type == 0x85 ||
				m_tsp.pmt.stream_type == 0x86 || m_tsp.pmt.stream_type == 0x8a ||
				m_tsp.pmt.stream_type == 0xa1 || m_tsp.pmt.stream_type == 0xa2 ||
				m_tsp.pmt.stream_type == 0x90)
			{
				m_audio_elementary_PIDs.set(m_tsp.pmt.elementary_PID);
			}
			else
			{
				std::cerr << "parse mpegts, unexpected stream type, type = "
					<< (int)m_tsp.pmt.stream_type << std::endl;
			}
		}
		else
		{
			std::cerr << "parse stream type error, N1 = ";			 
			return false;
		}
	}
 
	// CRC32.
	bf.GetBits(&m_tsp.pmt.CRC_32, 32);

	int bc = bf.GetBitCount();
	 
	bf.MoveAhead(188 - (bc/8));
	/*
	auto crc = crc32((const uint8_t *)&m_tsp.pmt.CRC_32, 4);
	if (crc != m_tsp.pmt.CRC_32)
	{
		std::cerr << "parse PMT section crc32 error, crc = ";			 
	}
	*/
	
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


STREAM_TYPE TSDemux::get_stream_type(uint8_t pes_type)
{
	switch (pes_type)
	{
	case 0x01:
		return STREAM_TYPE_VIDEO_MPEG1;
	case 0x02:
		return STREAM_TYPE_VIDEO_MPEG2;
	case 0x03:
		return STREAM_TYPE_AUDIO_MPEG1;
	case 0x04:
		return STREAM_TYPE_AUDIO_MPEG2;
	case 0x06:
		return STREAM_TYPE_PRIVATE_DATA;
	case 0x0f:
	case 0x11:
		return STREAM_TYPE_AUDIO_AAC;
	case 0x10:
		return STREAM_TYPE_VIDEO_MPEG4;
	case 0x1b:
		return STREAM_TYPE_VIDEO_H264;
	case 0x15:
		return STREAM_TYPE_KLV;
	case 0x24:
		return STREAM_TYPE_VIDEO_HEVC;
	case 0xea:
		return STREAM_TYPE_VIDEO_VC1;
	case 0x80:
		return STREAM_TYPE_AUDIO_LPCM;
	case 0x81:
	case 0x83:
	case 0x84:
	case 0x87:
		return STREAM_TYPE_AUDIO_AC3;
	case 0x82:
	case 0x85:
	case 0x8a:
		return STREAM_TYPE_AUDIO_DTS;
	}
	return STREAM_TYPE_UNKNOWN;
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
			m_tsp.pesPacket.PTS = (m_tsp.pesPacket.PTS2 << 30) | (m_tsp.pesPacket.PTS1 << 15) | m_tsp.pesPacket.PTS0;
			m_tsp.pesPacket.PTS = m_tsp.pesPacket.PTS / 27000.0;
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
			m_tsp.pesPacket.PTS = (m_tsp.pesPacket.PTS2 << 30) | (m_tsp.pesPacket.PTS1 << 15 ) | m_tsp.pesPacket.PTS0;
			m_tsp.pesPacket.PTS = m_tsp.pesPacket.PTS / 27000.0;
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
			m_tsp.pesPacket.DTS = (m_tsp.pesPacket.DTS2 << 30) | (m_tsp.pesPacket.DTS1 << 15) | m_tsp.pesPacket.DTS0;
			m_tsp.pesPacket.DTS = m_tsp.pesPacket.DTS / 27000.0;
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
	else
	if (m_tsp.pesPacket.stream_id == program_stream_map
		|| m_tsp.pesPacket.stream_id == private_stream_2
		|| m_tsp.pesPacket.stream_id == ECM_stream
		|| m_tsp.pesPacket.stream_id == EMM_stream
		|| m_tsp.pesPacket.stream_id == program_stream_directory
		|| m_tsp.pesPacket.stream_id == DSMCC_stream
		|| m_tsp.pesPacket.stream_id == ITU_T_Rec_H_222_1_type_E_Stream
		|| (m_tsp.pesPacket.stream_id & 0xF0) == 0xE0)
	{
		for (int i = 0; i < m_tsp.pesPacket.PES_packet_length; i++)
		{
			//PES_packet_data_byte 8 bslbf
			bf.GetBits(&m_tsp.pesPacket.PES_packet_data_byte, 8);
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
				TS_program_map_section();
				//bf.MoveAhead(N);
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

