/***************************************************************************
*                       Bit Stream File Class Implementation
*
*   File    : bitfile.cpp
*   Purpose : This file implements a simple class of I/O methods for files
*             that contain data in sizes that aren't integral bytes.  An
*             attempt was made to make the methods in this class analogous
*             to the methods provided to manipulate file streams.  The
*             methods contained in this class were created with compression
*             algorithms in mind, but may be suited to other applications.
*   Author  : Michael Dipperstein
*   Date    : July 20, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: bitfile.cpp,v 1.11 2009/07/23 03:57:56 michael Exp $
*   $Log: bitfile.cpp,v $
*   Revision 1.11  2009/07/23 03:57:56  michael
*   Zero out MSBs in value returned by GetBits when not retuning EOF.
*
*   Revision 1.10  2008/09/15 04:12:53  michael
*   Removed dead code.
*
*   Revision 1.9  2008/01/27 06:04:54  michael
*   Added  ByteAlign() and FlushOutput() methods.
*
*   Revision 1.8  2007/08/26 21:41:36  michael
*   All methods that don't modify the calling object have been made const to
*   increase functionality of const bit_array_c.
*
*   Changes required for LGPL v3.
*
*   Revision 1.7  2007/02/06 06:25:02  michael
*   Trim trailing spaces.
*
*   Revision 1.6  2006/06/03 18:57:59  michael
*   Corrected error discovered by anonymous in the destructor of writing
*   objects.  Underlying output stream was not being deleted.
*
*   Used spell checker to correct comments.
*
*   Revision 1.5  2006/02/10 04:30:47  michael
*   Applied fix for error discovered by Peter Husemann
*   <peter.husemann (at) cebitec (dot) uni-bielefeld (dot) de>.
*   When GetBit() reads a 0xFF byte, it would mistake it for EOF.
*
*   Revision 1.4  2005/12/10 05:20:01  michael
*   Added methods to get/put bits from/to integer types.
*
*   Revision 1.3  2005/06/23 04:39:06  michael
*   Convert from DOS end of line to Unix end of line
*
*   Revision 1.2  2005/06/23 04:33:07  michael
*   Prevent GetBits/PutBits from accessing an extra byte when given an
*   integral number of bytes.
*
*   Revision 1.1.1.1  2004/08/04 13:45:38  michael
*   bitfile class
*
*
****************************************************************************
*
* Bitfile: Bit Stream File I/O Class
* Copyright (C) 2004-2007 by Michael Dipperstein (mdipper@cs.ucsb.edu)
*
* This file is part of the bit file library.
*
* The bit file library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The bit file library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include "pch.h"
#include "bitfile.h"

using namespace std;

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/* union used to test for endianess */
typedef union
{
    unsigned long word;
    unsigned char bytes[sizeof(unsigned long)];
} endian_test_t;
 
 
bool bit_file_c::CreatePIDFile(int pid, const char *fileName)
{
	if (m_pidToSave[pid] != -1)
		return false;

	m_pidToSave[pid] = pid;
	m_OutStream[pid] = new ofstream(fileName, ios::out | ios::binary);

	if (!m_OutStream[pid]->good())
	{
		delete m_OutStream[pid];
		m_OutStream[pid] = NULL;
		return false;
	}
	return true;
}
 
/***************************************************************************
*   Method     : ~bit_file_c - destructor
*   Description: This is the bit_file_c destructor.  It closes and frees
*                any open file streams.  The bit buffer will be flushed
*                prior to closing an output stream.
*   Parameters : None
*   Effects    : Closes and frees open file streams.
*   Returned   : None
***************************************************************************/
bit_file_c::~bit_file_c(void)
{
	if (pFileBuffer != NULL && m_externalBuffer == false)
	{
		delete pFileBuffer;
		pFileBuffer = NULL;
	}
    if (m_InStream != NULL)
    {		
        delete m_InStream;
    }

	for (int i = 0; i < MAX_FILES; i++)
	{
		if (m_OutStream[i] != NULL)
		{	
			m_OutStream[i]->close();
			delete m_OutStream[i];
			m_pidToSave[i] = -1;
		}
	}
}

/***************************************************************************
*   Constructor
***************************************************************************/
bit_file_c::bit_file_c()
{
	m_externalBufferPacketsSize = 0;
	m_externalBuffer = false;
	m_InStream = nullptr;
	pFileBuffer = nullptr;
	for (int i = 0; i < MAX_FILES; i++)
	{
		m_pidToSave[i] = -1;
		m_OutStream[i] = NULL;
	}
}
void bit_file_c::Open(const char *fileName)
{
    /* make sure file isn't already open */
    if (m_InStream != NULL)
    {
        throw("Error: File Already Open");
    }
	m_fileReadIndex = 0;
	m_InStream = new ifstream(fileName, ios::in | ios::binary);
	m_InStream->seekg(0, ios::end);
	ifstream::pos_type pos = m_InStream->tellg();
	m_InStream->seekg(0, ios::beg);
	pFileBuffer = new uint8_t[(uint32_t)pos];
	m_InStream->read((char *)pFileBuffer, (uint32_t)pos);
	
	if (!m_InStream->good())
	{
		delete m_InStream;
		m_InStream = NULL;
		throw("Error: Unable To Open File");
	}
	 
	m_InStream->close();
	m_BitBuffer = 0;
	m_BitCount = 0;
	m_filePos = pos;
    /* make sure we opened a file */
    if (m_InStream == NULL)
    {
        throw("Error: Unable To Open File");
    }
}

void bit_file_c::SetBuffer(uint8_t *p, uint32_t size)
{	 
	m_externalBuffer = true;
	pFileBuffer = p;
	m_filePos = size;
	m_BitBuffer = 0;
	m_BitCount = 0;
  
}
 
void bit_file_c::SetExternalBuffer(int packets)
{
	m_externalBuffer = true;
	if (pFileBuffer == nullptr)
		pFileBuffer = new uint8_t[packets * 188];
	m_externalBufferPacketsSize = packets;
	m_BitBuffer = 0;
	m_BitCount = 0;
}

/***************************************************************************
*   Method     : Close
*   Description: This method closes and frees any open file streams.  The
*                bit buffer will be flushed prior to closing an output
*                stream.  All member variables are re-initialized.
*   Parameters : None
*   Effects    : Closes and frees open file streams.  Resets member
*                variables.
*   Returned   : None
***************************************************************************/
void bit_file_c::Close(void)
{
    if (m_InStream != NULL)
    {
        m_InStream->close();
        delete m_InStream;

        m_InStream = NULL;
        m_BitBuffer = 0;
        m_BitCount = 0;
        m_Mode = BF_NO_MODE;
    }

	for (int i = 0; i < MAX_FILES; i++)
	{
		if (m_OutStream[i] != NULL)
		{
			m_OutStream[i]->close();
			delete m_OutStream[i];
			m_OutStream[i] = NULL;
			m_BitBuffer = 0;
			m_BitCount = 0;
		}
	}
} 
 
bool bit_file_c::MoveAhead(uint8_t size)
{
	if (m_BitCount > 0 && m_BitCount < 8)
		return false;

	m_fileReadIndex += size;
	IncBitCounter(8 * size);
	return true;

}

void bit_file_c::GetFilePtrIndex(uint32_t *size, uint32_t *fileStartPointer , uint32_t *fileCurPointer)
{
	*size = m_fileReadIndex - m_filePtrStart;
	*fileStartPointer = m_filePtrStart;
	*fileCurPointer = m_fileReadIndex;
}
void bit_file_c::GetFileData(uint32_t fileStartPointer, uint32_t size, char *buffer)
{
	memcpy(buffer, pFileBuffer + fileStartPointer, size);
}

void bit_file_c::SaveFilePointerStart()
{
	m_filePtrStart = m_fileReadIndex;
}

/***************************************************************************
*   Method     : GetChar
*   Description: This method returns the next byte from the input stream.
*   Parameters : None
*   Effects    : Reads next byte from file and updates buffer accordingly.
*   Returned   : EOF if a whole byte cannot be obtained.  Otherwise,
*                the character read.
***************************************************************************/
int bit_file_c::GetChar(uint8_t *returnValue)
{
    int tmp;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}
	
    *returnValue = pFileBuffer[m_fileReadIndex++];

    if (m_BitCount == 0)
    {
		IncBitCounter(8);
        /* we can just get byte from file */
        return 1;
    }

    /* we have some buffered bits to return too */
    tmp = (*returnValue >> m_BitCount);
    tmp |= m_BitBuffer << (8 - m_BitCount);

    /* put remaining in buffer. count shouldn't change. */
    m_BitBuffer = (char)*returnValue;

    *returnValue = tmp & 0xFF;
	IncBitCounter(8);
    return 1;

}

bool bit_file_c::GetTSBuffer(uint8_t *buffer, int size)
{
	int tmp;

	if (m_BitCount != 0)
		return false;

	if (m_externalBuffer == false)
	{
		uint8_t temp;
		for (int i = 0; i < size; i++)
		{
			GetChar(&temp);
			buffer[i] = temp;
		}
		return true;
	}

	memcpy(buffer , pFileBuffer + m_fileReadIndex , size);
	m_fileReadIndex += size;	 
	return 1;
}

/***************************************************************************
*   Method     : PutChar
*   Description: This method writes the byte passed as a parameter to the
*                output stream.
*   Parameters : c - the character to be written
*   Effects    : Writes a byte to the file and updates buffer accordingly.
*   Returned   : On success, the character written, otherwise EOF.
***************************************************************************/
int bit_file_c::PutChar(const int c)
{
    int tmp;

    if (m_OutBitStream == NULL)
    {
        return EOF;
    }

    if (m_BitCount == 0)
    {
        /* we can just put byte from file */
		m_OutBitStream->put(c);
        return c;
    }

    /* figure out what to write */
    tmp = (c & 0xFF) >> m_BitCount;
    tmp = tmp | ((m_BitBuffer) << (8 - m_BitCount));

	m_OutBitStream->put((char)tmp);    /* check for error */

    /* put remaining in buffer. count shouldn't change. */
    m_BitBuffer = (char)c;

    return tmp;
}

/***************************************************************************
*   Method     : GetBit
*   Description: This method returns the next bit from the input stream.
*                The bit value returned is the msb in the bit buffer.
*   Parameters : None
*   Effects    : Reads next bit from bit buffer.  If the buffer is empty,
*                a new byte will be read from the input stream.
*   Returned   : 0 if bit == 0, 1 if bit == 1, and EOF if operation fails.
***************************************************************************/
int bit_file_c::GetBit(void)
{
    int returnValue;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}

    if (m_BitCount == 0)
    {
        /* buffer is empty, read another character */
		returnValue = pFileBuffer[m_fileReadIndex++];
        m_BitCount = 8;
        m_BitBuffer = returnValue;        
    }

    /* bit to return is msb in buffer */
    m_BitCount--;
    returnValue = m_BitBuffer >> m_BitCount;
	IncBitCounter(1);
    return (returnValue & 0x01);
}

int bit_file_c::_GetBit(void)
{
	int returnValue;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}

	if (m_BitCount == 0)
	{
		/* buffer is empty, read another character */
		returnValue = pFileBuffer[m_fileReadIndex++];
		m_BitCount = 8;
		m_BitBuffer = returnValue;		
	}

	/* bit to return is msb in buffer */
	m_BitCount--;
	returnValue = m_BitBuffer >> m_BitCount;
	return (returnValue & 0x01);
}

bool bit_file_c::CheckBits(uint32_t *value , int num)
{
 
	if (m_InStream == NULL)
	{
		return false;
	}

	if (m_BitCount == 0)
	{
		/* buffer is empty, read another character */
		uint32_t A = pFileBuffer[m_fileReadIndex];
		uint32_t B = pFileBuffer[m_fileReadIndex + 1];
		uint32_t C = pFileBuffer[m_fileReadIndex + 2];		 
		*value = (A << 16) | (B << 8) | C;
		return true;
	}
	throw ("not supported yet");
	return true;
}

int bit_file_c::CheckBit(void)
{
	int returnValue;

	if (m_InStream == NULL)
	{
		return EOF;
	}

	if (m_BitCount == 0)
	{
		/* buffer is empty, read another character */
		returnValue = pFileBuffer[m_fileReadIndex++];
		m_BitCount = 7;
		m_BitBuffer = returnValue;		
	}

	/* bit to return is msb in buffer */
	//m_BitCount--;
	returnValue = m_BitBuffer >> m_BitCount;

	return (returnValue & 0x01);
}

int bit_file_c::CheckByte(void)
{
	int returnValue;

	if (m_InStream == NULL)
	{
		return EOF;
	}

	if (m_BitCount == 0)
	{
		/* buffer is empty, read another character */
		returnValue = pFileBuffer[m_fileReadIndex++];
		m_BitCount = 8;
		m_BitBuffer = returnValue;

	}

	/* bit to return is msb in buffer */
	//m_BitCount--;
	returnValue = m_BitBuffer;
	return returnValue;
}

/***************************************************************************
*   Method     : PutBit
*   Description: This method writes the bit passed as a parameter to the
*                output stream.
*   Parameters : c - the bit value to be written
*   Effects    : Writes a bit to the bit buffer.  If the buffer has a byte,
*                the buffer is written to the output stream and cleared.
*   Returned   : On success, the bit value written, otherwise EOF.
***************************************************************************/
int bit_file_c::PutBit(const int c)
{
    int returnValue = c;

    if (m_OutStream == NULL)
    {
        return EOF;
    }

    m_BitCount++;
    m_BitBuffer <<= 1;

    if (c != 0)
    {
        m_BitBuffer |= 1;
    }

    /* write bit buffer if we have 8 bits */
    if (m_BitCount == 8)
    {
		m_OutBitStream->put(m_BitBuffer);    /* check for error */

        /* reset buffer */
        m_BitCount = 0;
        m_BitBuffer = 0;
    }

    return returnValue;
}

/***************************************************************************
*   Method     : GetBits
*   Description: This method reads the specified number of bits from the
*                input stream and writes them to the requested memory
*                location (msb to lsb).
*   Parameters : bits - address to store bits read
*                count - number of bits to read
*   Effects    : Reads bits from the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.
*   Returned   : EOF for failure, otherwise the number of bits read.  If
*                an EOF is reached before all the bits are read, bits
*                will contain every bit through the last complete byte.
***************************************************************************/
int bit_file_c::GetBits(uint16_t *bits, const unsigned int count)
{
     
    int remaining, returnValue;
	*bits = 0;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}
 
    remaining = count;

	while (remaining > 0)
	{

		if (m_BitCount == 0)
		{			
			/* buffer is empty, read another character */
			returnValue = pFileBuffer[m_fileReadIndex++];
			if (count == 8)
			{
				*bits = returnValue;
				IncBitCounter(count);
				return count;
			}
			if (count == 16)
			{
				uint32_t returnValue1 = pFileBuffer[m_fileReadIndex++];
				*bits = (returnValue << 8) | returnValue1;
				IncBitCounter(count);
				return count;
			}
			m_BitCount = 8;
			m_BitBuffer = returnValue;			
		}
		
		returnValue = this->_GetBit();
		*bits |= returnValue;
		remaining--;
		if (remaining > 0)
			*bits = *bits << 1;				 
	}
	IncBitCounter(count);
    return count;
}

int bit_file_c::GetBits(uint8_t *bits, const unsigned int count)
{

	int remaining, returnValue;
	*bits = 0;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}

	remaining = count;

	while (remaining > 0)
	{

		if (m_BitCount == 0)
		{
			/* buffer is empty, read another character */
			returnValue = pFileBuffer[m_fileReadIndex++];
			if (count == 8)
			{
				*bits = returnValue;
				IncBitCounter(count);
				return count;
			}
			m_BitCount = 8;
			m_BitBuffer = returnValue;			
		}
		

		returnValue = this->_GetBit();
		*bits |= returnValue;
		remaining--;
		if (remaining > 0)
			*bits = *bits << 1;
	}
	IncBitCounter(count);
	return count;
}

int bit_file_c::GetBitCount()
{
	return m_totalBitCount;
}
int bit_file_c::GetBitCounter(int i)
{
	return m_BitCounter[i];
}

void bit_file_c::ResetBitCount()
{
	m_totalBitCount = 0;
}
void bit_file_c::ResetBitCount(int i)
{
	m_BitCounter[i] = 0;
}

int bit_file_c::GetBits(uint64_t *bits, const unsigned int count)
{

	int remaining, returnValue;
	*bits = 0;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}

	remaining = count;

	while (remaining > 0)
	{

		if (m_BitCount == 0)
		{
			/* buffer is empty, read another character */
			returnValue = pFileBuffer[m_fileReadIndex++];
			m_BitCount = 8;
			m_BitBuffer = returnValue;
		}

		returnValue = this->_GetBit();
		*bits |= returnValue;
		remaining--;
		if (remaining > 0)
			*bits = *bits << 1;
	}

	IncBitCounter(count);
	return count;
}

int bit_file_c::GetBits(uint32_t *bits, const unsigned int count)
{

	int remaining, returnValue;
	*bits = 0;

	if (m_externalBuffer == false)
	{
		if (m_fileReadIndex >= m_filePos)
			return EOF;
	}

	remaining = count;

	while (remaining > 0)
	{

		if (m_BitCount == 0)
		{
			/* buffer is empty, read another character */
			if (m_externalBuffer == false)
			{
				if (m_fileReadIndex >= m_filePos)
					return EOF;
			}
			returnValue = pFileBuffer[m_fileReadIndex++];
			m_BitCount = 8;
			m_BitBuffer = returnValue;			
		}

		returnValue = this->_GetBit();
		*bits |= returnValue;
		remaining--;
		if (remaining > 0)
			*bits = *bits << 1;
	}

	IncBitCounter(count);
	return count;
}

void bit_file_c::IncBitCounter(int count)
{
	m_totalBitCount += count;
	for (int i = 0; i < MAX_BIT_COUNTER; i++)
	{
		m_BitCounter[i] += count;
	}
}

/***************************************************************************
*   Method     : PutBits
*   Description: This method writes the specified number of bits from the
*                memory location passed as a parameter to the output
*                stream.   Bits are written msb to lsb.
*   Parameters : bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int bit_file_c::PutBits(void *bits, const unsigned int count)
{
    char *bytes, tmp;
    int offset, remaining, returnValue;

    if ((m_OutStream == NULL) || (bits == NULL))
    {
        return EOF;
    }

    bytes = (char *)bits;

    offset = 0;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = this->PutChar(bytes[offset]);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];
        while (remaining > 0)
        {
            returnValue = this->PutBit(tmp & 0x80);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}
 
/***************************************************************************
*   Method     : PutBitsInt
*   Description: This method provides a machine independent layer that
*                allows a single function call to write an arbitrary number
*                of bits from an integer type variable (short, int, long,
*                ...) to the file stream.
*   Parameters : bits - pointer to bits to write
*                count - number of bits to write
*                size - sizeof type containing "bits"
*   Effects    : Calls a method that writes bits to the bit buffer and
*                file stream.  The bit buffer will be modified as necessary.
*                the bits will be written to the file stream from least
*                significant byte to most significant byte.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.  An error is thrown if
*                the machine endianess is unknown.
***************************************************************************/
int bit_file_c::PutBitsInt(void *bits, const unsigned int count,
    const size_t size)
{
    int returnValue;

    if ((m_OutStream == NULL) || (bits == NULL))
    {
        return EOF;
    }

    if (m_endian == BF_LITTLE_ENDIAN)
    {
        returnValue = this->PutBitsLE(bits, count);
    }
    else if (m_endian == BF_BIG_ENDIAN)
    {
        returnValue = this->PutBitsBE(bits, count, size);
    }
    else
    {
        throw("Error: System Endianess Unknown");
    }

    return returnValue;
}

/***************************************************************************
*   Method     : PutBitsLE   (Little Endian)
*   Description: This method writes the specified number of bits from the
*                memory location passed as a parameter to the file stream
*                Bits are written LSB to MSB, assuming little endian order.
*   Parameters : bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a little endian integer of length >= (count/8) + 1.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int bit_file_c::PutBitsLE(void *bits, const unsigned int count)
{
    unsigned char *bytes, tmp;
    int offset, remaining, returnValue;

    bytes = (unsigned char *)bits;
    offset = 0;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = this->PutChar(bytes[offset]);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];
        tmp <<= (8 - remaining);

        while (remaining > 0)
        {
            returnValue = this->PutBit(tmp & 0x80);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}

/***************************************************************************
*   Method     : PutBitsBE   (Big Endian)
*   Description: This method writes the specified number of bits from the
*                memory location passed as a parameter to the file stream
*                Bits are written LSB to MSB, assuming big endian order.
*   Parameters : bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a big endian integer of length size.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int bit_file_c::PutBitsBE(void *bits, const unsigned int count,
    const size_t size)
{
    unsigned char *bytes, tmp;
    int offset, remaining, returnValue;

    if (count > (size * 8))
    {
        /* too many bits to write */
        return EOF;
    }

    bytes = (unsigned char *)bits;
    offset = size - 1;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = this->PutChar(bytes[offset]);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset--;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];
        tmp <<= (8 - remaining);

        while (remaining > 0)
        {
            returnValue = this->PutBit(tmp & 0x80);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}

/***************************************************************************
*   Method     : eof
*   Description: This method indicates whether or not the open file stream
*                is at the end of file.
*   Parameters : None
*   Effects    : None
*   Returned   : Returns true if the opened file stream is at an EOF.
*                Otherwise false is returned.
***************************************************************************/
bool bit_file_c::eof(void)
{
    if (m_InStream != NULL)
    {
        return (m_InStream->eof());
    }
	 
    /* return false for no file */
    return false;
}

/***************************************************************************
*   Method     : good
*   Description: This method is analogous to good for file streams.
*   Parameters : None
*   Effects    : None
*   Returned   : Returns good for the opened file stream.  False is
*                returned if there is no open file stream.
***************************************************************************/
bool bit_file_c::good(int pid_index)
{
    if (m_InStream != NULL)
    {
        return (m_InStream->good());
    }

    if (m_OutStream[pid_index] != NULL)
    {
        return (m_OutStream[pid_index]->good());
    }

    /* return false for no file */
    return false;
}

/***************************************************************************
*   Method     : bad
*   Description: This method is analogous to bad for file streams.
*   Parameters : None
*   Effects    : None
*   Returned   : Returns bad for the opened file stream.  False is
*                returned if there is no open file stream.
***************************************************************************/
bool bit_file_c::bad(int pid_index)
{
    if (m_InStream != NULL)
    {
        return (m_InStream->bad());
    }

    if (m_OutStream != NULL)
    {
        return (m_OutStream[pid_index]->bad());
    }

    /* return false for no file */
    return false;
}
