// EliDemux.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "TSDemux.h"

// the ts pdf https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf

int main()
{
    std::cout << "Hello World!\n"; 

	TSDemux t;
	t.CreatePIDFile(258, "c:\\klv.bin");
	t.CreatePIDFile(256, "c:\\video.h264");
	//t.Start("d:\\truck.ts");
	
	std::ifstream *m_InStream;      
	

	m_InStream = new ifstream("d:\\truck.ts", ios::in | ios::binary);
	m_InStream->seekg(0, ios::end);
	ifstream::pos_type pos = m_InStream->tellg();
	m_InStream->seekg(0, ios::beg);
	uint8_t *pFileBuffer = new uint8_t[(uint32_t)pos];
	m_InStream->read((char *)pFileBuffer, (uint32_t)pos);
	m_InStream->close();

	t.Start(pFileBuffer, (uint32_t)pos);

	delete pFileBuffer;



}

 