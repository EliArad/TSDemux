#pragma once
#include <stdint.h>

typedef struct _TSPacket
{
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


} TSPacket;
