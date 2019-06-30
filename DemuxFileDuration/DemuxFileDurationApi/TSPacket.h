#pragma once
#include <stdint.h>

//https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf

#define fast_forward 0
#define slow_motion  1 
#define freeze_frame 2
#define slow_reverse 4
#define fast_reverse 3

typedef enum PES_CODES_
{
	program_stream_map = 0xBC,
	private_stream_1 = 0xBD,
	padding_stream = 0xBE,
	private_stream_2 = 0xBF,
	/*
	110x xxxx ISO / IEC 13818 - 3 or ISO / IEC 11172 - 3 or ISO / IEC 13818 - 7 or ISO / IEC
	14496 - 3 audio stream number x xxxx
	1110 xxxx ITU - T Rec.H.262 | ISO / IEC 13818 - 2 or ISO / IEC 11172 - 2 or ISO / IEC
	14496 - 2 video stream number xxxx
	*/
	ECM_stream = 0xF0,
	EMM_stream = 0xF1,
	program_stream_directory = 0xFF,	 
	DSMCC_stream = 0xF2, // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A or ISO/IEC 13818-6_DSMCC_stream 
	ITU_T_Rec_H_222_1_type_E_Stream = 0xE8
} PES_CODES;
 

typedef struct _PESPacket
{
	uint32_t packet_start_code_prefix;
	uint8_t	 stream_id;
	uint16_t PES_packet_length;
	uint8_t	 PES_scrambling_control;
	uint8_t	 PES_priority;
	uint8_t	 data_alignment_indicator;
	uint8_t	 copyright;
	uint8_t	 original_or_copy;
	uint8_t	 PTS_DTS_flags;
	uint8_t	 ESCR_flag;
	uint8_t	 ES_rate_flag;
	uint8_t	 DSM_trick_mode_flag;
	uint8_t	 additional_copy_info_flag;
	uint8_t	 PES_CRC_flag;
	uint8_t	 PES_extension_flag;
	uint8_t	 PES_header_data_length;
	uint32_t PTS2;
	uint32_t marker_bit;
	uint32_t PTS1;
	uint32_t PTS0;
	uint32_t DTS2;
	uint32_t DTS1;
	uint32_t DTS0;
	uint32_t reserved2;
	uint32_t ESCR_base2;
	uint32_t reserved;
	uint32_t ESCR_base1;			
	uint32_t ESCR_base0;
	uint32_t ESCR_extension;
	uint32_t ES_rate;
	uint8_t trick_mode_control;	
	uint8_t intra_slice_refresh;
	uint8_t frequency_truncation;
	uint8_t rep_cntrl;
	uint8_t field_id;
	uint8_t reserved3;
	uint8_t reserved5;
	uint8_t additional_copy_info;			
	uint16_t previous_PES_packet_CRC;
	uint8_t PES_private_data_flag;
	uint8_t pack_header_field_flag;
	uint8_t program_packet_sequence_counter_flag;
	uint8_t P_STD_buffer_flag;
	uint8_t reserved33;
	uint8_t PES_extension_flag_2;

	uint8_t PES_private_data[128];
	uint8_t pack_field_length;		
	uint8_t program_packet_sequence_counter;
	uint8_t MPEG1_MPEG2_identifier;
	uint8_t original_stuff_length;
	uint8_t P_STD_buffer_scale;
	uint16_t P_STD_buffer_size;
	 
	uint8_t PES_extension_field_length;
	uint8_t stuffing_byte;
	uint8_t PES_packet_data_byte;
	uint8_t padding_byte;
		


} PESPacket;

#define MAX_PMTs 10
typedef struct PMT_
{
	uint8_t table_id;
	uint8_t section_syntax_indicator;
	uint8_t reserved;
	uint16_t section_length;
	uint16_t program_number;
	uint8_t version_number;
	uint8_t current_next_indicator;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t PCR_PID;
	uint8_t program_info_length;
	uint8_t stream_type[MAX_PMTs];
	uint16_t elementary_PID[MAX_PMTs];
	uint16_t ES_info_length[MAX_PMTs];
	uint32_t CRC_32;

} PMT;

typedef struct PAT_
{
	uint8_t table_id;
	uint8_t section_syntax_indicator;
	uint8_t reserved;
	uint16_t section_length;
	uint16_t transport_stream_id;
	uint8_t version_number;
	uint8_t current_next_indicator;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t program_number[100];
	uint8_t	reserved3;
	uint16_t network_PID[100];
	uint16_t program_map_PID[100];
	uint32_t CRC_32;
} PAT;

typedef struct _TSPacket
{
	PESPacket pesPacket;

	struct TS
	{
		uint64_t packetNumber;
		uint8_t  sync_byte;
		uint8_t  transport_error_indicator;
		uint8_t  payload_unit_start_indicator;
		uint8_t  transport_priority;
		uint16_t  PID;
		uint8_t  transport_scrambling_control;
		uint8_t  adaptation_field_control;
		uint8_t  continuity_counter;
	} ts;

	struct Adaption
	{
		uint8_t adaptation_field_length;
		uint8_t discontinuity_indicator;
		uint8_t random_access_indicator;
		uint8_t elementary_stream_priority_indicator;
		uint8_t PCR_flag;
		uint8_t OPCR_flag;
		uint8_t splicing_point_flag;
		uint8_t transport_private_data_flag;
		uint8_t adaptation_field_extension_flag;
		uint64_t program_clock_reference_base;
		double PCR;
		uint8_t reserved;
		uint16_t program_clock_reference_extension;
		uint8_t splice_countdown;
		uint8_t transport_private_data_length;
		uint8_t adaptation_field_extension_length;
		uint64_t original_program_clock_reference_base;
		uint16_t original_program_clock_reference_extension;
		uint8_t ltw_flag;
		uint8_t piecewise_rate_flag;
		uint8_t seamless_splice_flag;
		uint32_t piecewise_rate;
		uint8_t ltw_valid_flag;
		uint16_t ltw_offset;
		uint8_t marker_bit;
		uint8_t private_data_byte[1000];
		uint8_t splice_type;
		uint8_t stuffing_byte;


	} Adp;

	PAT  pat;
	PMT pmt;


} TSPacket;
