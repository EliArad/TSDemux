#include "pch.h"
#include "fifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

CFifo::CFifo()
{
	m_wr = 0;
	m_rd = 0;

	m_fifoBuffer = NULL;

	if (!InitializeCriticalSectionAndSpinCount(&cs,
		0x00000400))
		return;

}
void CFifo::Create(int size)
{
	m_fifoBuffer = new uint8_t[size];
	m_fifoSize = size;

}
CFifo::~CFifo()
{
	if (m_fifoBuffer != NULL)
	{
		delete m_fifoBuffer;
	}

	DeleteCriticalSection(&cs);
}

bool CFifo::Push(const uint8_t *data, uint32_t size)
{
	// Request ownership of the critical section.
	EnterCriticalSection(&cs);

	while (GetFifoFreeSize() < size)
	{

	}
	if ((m_wr + size) < m_fifoSize)
	{
		memcpy(m_fifoBuffer + m_wr, data, size);
		m_wr += size;
	}
	else 
	{
		int c = m_fifoSize - m_wr;
		memcpy(m_fifoBuffer + m_wr, data, c);
		size -= c;
		memcpy(m_fifoBuffer , data + c, size);
		m_wr = c;
	}
	LeaveCriticalSection(&cs);
	return true;
}

bool CFifo::Pop(uint8_t *data, uint32_t size)
{
	EnterCriticalSection(&cs);

	if (GetFifoSize() < size)
	{
		LeaveCriticalSection(&cs);
		return false;
	}

	if ((m_rd + size) < m_fifoSize)
	{
		memcpy(data, m_fifoBuffer + m_rd, size);
		m_rd += size;
	}
	else
	{
		int c = m_fifoSize - m_rd;
		memcpy(data, m_fifoBuffer + m_rd, c);
		size -= c;
		memcpy(data + c, m_fifoBuffer, size);
		m_rd = c;
	}
	LeaveCriticalSection(&cs);
	return true;
}
uint32_t CFifo::GetFifoSize()
{
	if (m_wr == m_rd)
	{
		return 0;
	}
	if (m_wr > m_rd)
	{
		return m_wr - m_rd;
	}

	return (m_fifoSize - m_rd) + m_wr;
}

uint32_t CFifo::GetFifoFreeSize()
{
	if (m_wr == m_rd)
	{
		return m_fifoSize;
	}
	if (m_wr > m_rd)
	{
		return m_fifoSize - m_wr - m_rd;
	}

	return m_fifoSize - ((m_fifoSize - m_rd) + m_wr);
}

