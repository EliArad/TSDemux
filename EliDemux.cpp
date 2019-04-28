// EliDemux.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "TSDemux.h"

// the ts pdf https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf
std::ifstream *m_InStream;
ifstream::pos_type pos;
TSDemux t;
uint8_t *pFileBuffer; 

void Process()
{
	int i = 0;
	int chunkSize = 188;
	while (pos > 0)
	{
		t.PushData(pFileBuffer + i, chunkSize);
		i += chunkSize;
		pos -= chunkSize;
	}
}

int main()
{
    std::cout << "Hello World!\n"; 

	
	t.CreatePIDFile(258, "c:\\klv.bin");
	t.CreatePIDFile(256, "c:\\video.h264");
	//t.Start("d:\\truck.ts");
	
	   
	

	m_InStream = new ifstream("d:\\truck.ts", ios::in | ios::binary);
	m_InStream->seekg(0, ios::end);
	pos = m_InStream->tellg();
	m_InStream->seekg(0, ios::beg);
	pFileBuffer = new uint8_t[(uint32_t)pos];
	m_InStream->read((char *)pFileBuffer, (uint32_t)pos);
	m_InStream->close();
	  
	//t.Start(pFileBuffer, (uint32_t)pos);
	t.PrintConfig(true);

	t.InitTSWorker(1, 188 * 2);
	
	thread t1(Process);
	t.WaitWorker();
	Sleep(14000);
	t.StopWorker();

	delete pFileBuffer;
	 
}



 