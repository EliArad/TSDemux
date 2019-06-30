#pragma once
#include <stdint.h>

//https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf

#define fast_forward 0
#define slow_motion  1 
#define freeze_frame 2
#define slow_reverse 4
#define fast_reverse 3

#define PCR_TIME_BASE				27000000
#define TS_SIZE						188
#define TS_HEADER_SIZE				4
#define TS_HEADER_SIZE_AF			6
#define TS_HEADER_SIZE_PCR			12
#define PSI_HEADER_SIZE				3
#define PSI_HEADER_SIZE_SYNTAX1		8
#define PSI_CRC_SIZE				4
#define PAT_HEADER_SIZE				PSI_HEADER_SIZE_SYNTAX1
#define PAT_PROGRAM_SIZE			4
#define PMT_HEADER_SIZE			    (PSI_HEADER_SIZE_SYNTAX1 + 4)
#define PMT_ES_SIZE					5
#define PES_HEADER_SIZE				6
#define PES_HEADER_OPTIONAL_SIZE	3

enum STREAM_TYPE
{
	STREAM_TYPE_UNKNOWN = 0,
	STREAM_TYPE_VIDEO_MPEG1,
	STREAM_TYPE_VIDEO_MPEG2,
	STREAM_TYPE_AUDIO_MPEG1,
	STREAM_TYPE_AUDIO_MPEG2,
	STREAM_TYPE_AUDIO_AAC,
	STREAM_TYPE_AUDIO_AAC_ADTS,
	STREAM_TYPE_AUDIO_AAC_LATM,
	STREAM_TYPE_VIDEO_H264,
	STREAM_TYPE_VIDEO_HEVC,
	STREAM_TYPE_AUDIO_AC3,
	STREAM_TYPE_AUDIO_EAC3,
	STREAM_TYPE_DVB_TELETEXT,
	STREAM_TYPE_DVB_SUBTITLE,
	STREAM_TYPE_VIDEO_MPEG4,
	STREAM_TYPE_VIDEO_VC1,
	STREAM_TYPE_AUDIO_LPCM,
	STREAM_TYPE_AUDIO_DTS,
	STREAM_TYPE_PRIVATE_DATA,
	STREAM_TYPE_KLV=21,
};

enum {
	unkown_type = 0x00,
	video_mpeg1 = 0x01,
	video_mpeg2 = 0x02,
	audio_mpeg1 = 0x03,
	audio_mpeg2 = 0x04,
	private_section = 0x05,
	private_data = 0x06,
	iso_13818_1_pes = 0x07,
	iso_13522_mheg = 0x08,
	itu_t_rec_h_222_1 = 0x09,
	iso_13818_6_type_a = 0x0a,
	iso_13818_6_type_b = 0x0b,
	iso_13818_6_type_c = 0x0c,
	iso_13818_6_type_d = 0x0d,
	iso_13818_1_auxiliary = 0x0e,
	audio_aac = 0x0f,
	video_mpeg4 = 0x10,
	audio_aac_latm = 0x11,
	iso_14496_1_pes = 0x12,
	iso_14496_1_sections = 0x13,
	iso_13818_6_SDP = 0x14,
	metadata = 0x15,
	video_h264 = 0x1b,
	video_hevc = 0x24,
	video_cavs = 0x42,
	video_dirac = 0xd1,
	video_vc1 = 0xea,
	audio_pcm_bluray = 0x80,
	audio_ac3 = 0x81,
	audio_dts_0 = 0x82,
	audio_truehd = 0x83,
	audio_eac3_0 = 0x84,
	audio_dts_1 = 0x85,
	audio_dts_2 = 0x86,
	audio_eac3_1 = 0x87,
	audio_eac3_2 = 0xa1,
	audio_dts_3 = 0xa2,
	hdmv_pgs_subtitle = 0x90,
	audio_dts_4 = 0x8a,
};

enum {
	av_picture_type_none = 0,	///< Undefined
	av_picture_type_i,			///< Intra
	av_picture_type_p,			///< Predicted
	av_picture_type_b,			///< Bi-dir predicted
	av_picture_type_s,			///< S(GMC)-VOP MPEG4
	av_picture_type_si,			///< Switching Intra
	av_picture_type_sp,			///< Switching Predicted
	av_picture_type_bi,			///< BI type
};

struct STREAM_INFO
{
	char                  language[4];
	int                   composition_id;
	int                   ancillary_id;
	int                   fps_scale;
	int                   fps_rate;
	int                   height;
	int                   width;
	float                 aspect;
	int                   channels;
	int                   sample_rate;
	int                   block_align;
	int                   bit_rate;
	int                   bits_per_sample;
	bool                  interlaced;
};

typedef struct IBP_DESCRIPOT
{

	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint8_t closed_gop_flag;
	uint8_t identical_gop_flag;
	uint8_t max_gop_length;

} IBP_DESCRIPOT;


typedef struct STD_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint8_t reserved;
	uint8_t leak_valid_flag;

} STD_DESCRIPTOR;


typedef struct MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint8_t bound_valid_flag;
	uint8_t LTW_offset_lower_bound;
	uint8_t LTW_offset_upper_bound;

} MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR;


typedef struct SYSTEM_CLOCK_DESCRIPTOR_
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint8_t external_clock_reference_indicator;
	uint8_t reserved1;
	uint8_t clock_accuracy_integer;
	uint8_t clock_accuracy_exponent;
	uint8_t reserved2;

}  SYSTEM_CLOCK_DESCRIPTOR;


typedef struct ISO_639_LANGUAGE_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;

	uint32_t ISO_639_language_code;
	uint8_t audio_type;

} ISO_639_LANGUAGE_DESCRIPTOR;


typedef struct CA_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint16_t CA_system_ID;
	uint8_t reserved;
	uint16_t CA_PID;
	uint8_t private_data_byte;

} CA_DESCRIPTOR;


typedef struct VIDEO_WINDOW_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint16_t horizontal_offset;
	uint16_t vertical_offset;
	uint8_t window_priority;

} VIDEO_WINDOW_DESCRIPTOR;


typedef struct TARGET_BACKGROUND_GRID_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint16_t horizontal_size;
	uint16_t vertical_size;
	uint8_t aspect_ratio_information;

} TARGET_BACKGROUND_GRID_DESCRIPTOR;


typedef struct DATA_STREAM_ALIGNMENT_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint8_t alignment_type;
} DATA_STREAM_ALIGNMENT_DESCRIPTOR;

typedef struct REGISTRATION_DESCRIPTOR
{
	uint8_t descriptor_tag;
	uint8_t descriptor_length;
	uint32_t format_identifier;
	uint8_t additional_identification_info;

} REGISTRATION_DESCRIPTOR;


typedef struct  HIERARCHY_DESCRIPTOR
{
	uint8_t  descriptor_tag;
	uint8_t  descriptor_length;
	uint8_t  reserved;
	uint8_t  hierarchy_type;
	uint8_t  reserved1;
	uint8_t  hierarchy_layer_index;
	uint8_t  reserved2;
	uint8_t  hierarchy_embedded_layer_index;
	uint8_t  reserved3;
	uint8_t  hierarchy_channel;

} HIERARCHY_DESCRIPTOR;



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
	uint64_t PTS;
	uint64_t DTS;
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
	uint8_t stream_type;
	uint16_t elementary_PID;
	uint16_t ES_info_length;
	uint32_t CRC_32;

	uint8_t profile_and_level_indication;
	uint8_t descriptor_tag;
	uint8_t descriptor_length ;
	uint8_t multiple_frame_rate_flag;
	uint8_t frame_rate_code;
	uint8_t MPEG_1_only_flag;
	uint8_t constrained_parameter_flag;
	uint8_t	still_picture_flag;
	uint8_t	chroma_format;
	uint8_t	frame_rate_extension_flag;

	uint8_t free_format_flag;
	uint8_t ID;
	uint8_t layer;
	uint8_t variable_rate_audio_indicator;
	 

	IBP_DESCRIPOT ibpDesc;
	MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR multiplexBufferDesc;
	SYSTEM_CLOCK_DESCRIPTOR systemClockDesc;
	ISO_639_LANGUAGE_DESCRIPTOR ISO639Desc;
	CA_DESCRIPTOR caDesc;
	VIDEO_WINDOW_DESCRIPTOR videoWindowDesc;
	TARGET_BACKGROUND_GRID_DESCRIPTOR targetBackgroundGridDesc;
	 

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
