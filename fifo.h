#pragma once
#include <stdint.h>
#include <Windows.h>


class CFifo
{

public: 
	CFifo();
	~CFifo();
	void Create(int size);
	bool Push(const uint8_t *data, uint32_t size);
	bool Pop(uint8_t *data, uint32_t size);	
	bool Pop(uint8_t *data);
	bool PopTS(uint8_t *data, int maxPacketSize, int *packets);
private:
	uint32_t GetFifoFreeSize();
	uint32_t GetFifoSize();
private:
	mutable CRITICAL_SECTION cs;
	uint32_t m_fifoSize;
	uint32_t m_wr;
	uint32_t m_rd;
	uint8_t *m_fifoBuffer;
	uint32_t m_fifoFullnessSize;

};