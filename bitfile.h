/***************************************************************************
*                       Bit Stream File Class Header
*
*   File    : bitfile.h
*   Purpose : Provides definitions and prototypes for a simple class of I/O
*             methods for files that contain data in sizes that aren't
*             integral bytes.  An attempt was made to make the methods in
*             this class analogous to the methods provided to manipulate
*             file streams.  The methods contained in this class were
*             created with compression algorithms in mind, but may be
*             suited to other applications.
*   Author  : Michael Dipperstein
*   Date    : July 20, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: bitfile.h,v 1.6 2008/01/27 06:04:54 michael Exp $
*   $Log: bitfile.h,v $
*   Revision 1.6  2008/01/27 06:04:54  michael
*   Added  ByteAlign() and FlushOutput() methods.
*
*   Revision 1.5  2007/08/26 21:41:36  michael
*   All methods that don't modify the calling object have been made const
*   to increase functionality of const bit_array_c.
*
*   Changes required for LGPL v3.
*
*   Revision 1.4  2007/07/16 02:07:16  michael
*   Use -pedantic option when compiling.
*
*   Revision 1.3  2005/12/10 05:20:01  michael
*   Added methods to get/put bits from/to integer types.
*
*   Revision 1.2  2005/06/23 04:39:06  michael
*   Convert from DOS end of line to Unix end of line
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

#ifndef __BITFILE_H
#define __BITFILE_H

#include <iostream>
#include <fstream>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef enum
{
    BF_READ = 0,
    BF_WRITE = 1,
    BF_APPEND= 2,
    BF_NO_MODE
} BF_MODES;

typedef enum
{
    BF_UNKNOWN_ENDIAN,
    BF_LITTLE_ENDIAN,
    BF_BIG_ENDIAN
} endian_t;

class bit_file_c
{
	#define MAX_FILES 10000

    public:
        bit_file_c();
        virtual ~bit_file_c(void);
		int m_totalBitCount;
		int m_BitCounter[5];
        /* open/close bit file */
        void Open(const char *fileName);
        void Close(void);
		void IncBitCounter(int count);
		int GetBitCounter(int i);
        
        /* get/put character */
        int GetChar(uint8_t *returnValue);
        int PutChar(const int c);
		bool CreatePIDFile(int pid, const char *fileName);

        /* get/put single bit */
        int GetBit(void);
        int PutBit(const int c);
		int CheckBit(void);

		int GetBitCount();
		int CheckByte(void);
		void ResetBitCount();

        /* get/put number of bits */
        int GetBits(uint16_t *bits, const unsigned int count);
		int GetBits(uint8_t *bits, const unsigned int count);
		int GetBits(uint32_t *bits, const unsigned int count);
		int GetBits(uint64_t *bits, const unsigned int count);
        int PutBits(void *bits, const unsigned int count);
		int _GetBit(void);
        int PutBitsInt(void *bits, const unsigned int count,
            const size_t size);

        /* status */
        bool eof(void);
        bool good(int pid_index);
        bool bad(int pid_index);

		int m_pidToSave[MAX_FILES];
		std::ofstream *m_OutStream[MAX_FILES];    
		std::ofstream *m_OutBitStream;     
		void ResetBitCount(int i);

    private:
	
		char *pFileBuffer;
		uint32_t m_fileReadIndex;
		std::ifstream::pos_type m_filePos;
        std::ifstream *m_InStream;      /* input file stream pointer */
        endian_t m_endian;              /* endianess of architecture */
        char m_BitBuffer;               /* bits waiting to be read/written */
        unsigned char m_BitCount;       /* number of bits in bitBuffer */
        BF_MODES m_Mode;                /* open for read, write, or append */
		
        /* endianess aware methods used by GetBitsInt/PutBitsInt */
        
        int PutBitsLE(void *bits, const unsigned int count);
		 
        int PutBitsBE(void *bits, const unsigned int count,
            const size_t size);
		
};

#endif  /* ndef __BITFILE_H */
