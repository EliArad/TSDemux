#include "pch.h"
#include "fifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <algorithm>

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
	m_fifoFullnessSize = 0;

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

	int fsize = GetFifoFreeSize();	
	if (size > fsize)
	{
		LeaveCriticalSection(&cs);
		return false;
	}

	if ((m_wr + size) <= m_fifoSize)
	{
		memcpy(m_fifoBuffer + m_wr, data, size);
		m_wr = (m_wr + size) % m_fifoSize;
		//printf("m_wr = %d\n", m_wr);
		m_fifoFullnessSize += size;
	}
	else 
	{
		int c = m_fifoSize - m_wr;
		memcpy(m_fifoBuffer + m_wr, data, c);
		m_wr = (m_wr + c) % m_fifoSize;
		size -= c;
		memcpy(m_fifoBuffer , data + c, size);
		m_wr = (m_wr + size) % m_fifoSize;
		m_fifoFullnessSize += size;
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

// pop all data

bool CFifo::Pop(uint8_t *data)
{
	EnterCriticalSection(&cs);

	int size = GetFifoSize();
	if (size == 0)
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

bool CFifo::PopTS(uint8_t *data, int maxPacketSize ,int *packets)
{
	EnterCriticalSection(&cs);

	int size = GetFifoSize();
	if (size == 0)
	{
		LeaveCriticalSection(&cs);
		return false;
	}
	
	*packets = size / 188;
	*packets = min(*packets, maxPacketSize);

	size = (*packets) * 188;
	if (size == 0)
	{
		LeaveCriticalSection(&cs);
		return false;
	}

	if ((m_rd + size) <= m_fifoSize)
	{
		memcpy(data, m_fifoBuffer + m_rd, size);
		m_rd = (m_rd + size) % m_fifoSize;
		//printf("m_rd = %d\n", m_rd);
		m_fifoFullnessSize -= size;
	}
	else
	{
		int c = m_fifoSize - m_rd;
		memcpy(data, m_fifoBuffer + m_rd, c);
		m_rd = (m_rd + c) % m_fifoSize;
		size -= c;
		memcpy(data + c, m_fifoBuffer, size);
		m_rd = (m_rd + size) % m_fifoSize;
		m_fifoFullnessSize -= size;
	}
	LeaveCriticalSection(&cs);
	return true;
}

uint32_t CFifo::GetFifoSize()
{
	return m_fifoFullnessSize;
	 
}

uint32_t CFifo::GetFifoFreeSize()
{
	return m_fifoSize - m_fifoFullnessSize;

	if (m_wr == m_rd)
	{
		return m_fifoSize;
	}
	if (m_wr > m_rd)
	{
		return m_fifoSize - (m_wr - m_rd);
	} 
	return ((m_fifoSize - m_rd) + m_wr);
}

