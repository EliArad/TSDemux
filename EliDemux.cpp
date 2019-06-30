// EliDemux.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "TSDemux.h"

// the ts pdf https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf
std::ifstream *m_InStream;
std::ifstream::pos_type pos;
TSDemux t;
uint8_t *pFileBuffer; 

void Process()
{
	int i = 0;
	int chunkSize = 400;
	while (pos > 0)
	{
		if (t.PushData(pFileBuffer + i, chunkSize) == false)
		{
			Sleep(1);
			continue;
		}
		i += chunkSize;
		pos -= chunkSize;
		Sleep(0);
	}
}

int main()
{
    std::cout << "Hello World!\n"; 

	
	t.CreatePIDFile(258, "c:\\klv.bin");
	//t.CreatePIDFile(256, "c:\\video.h264");
	t.PrintConfig(true);
	//t.Streaming(true, (char *)"10.0.0.10", 6000, false);
	t.Loop(true);

#if 1 // working from file 
	t.Start("d:\\truck.ts");
	//t.Start("D:\\tempBauotechDVRStorage\\2019_06_29_10_55_41\\2019_06_29_10_55_41\\2019_06_29_10_55_41_dvrsink_00000.ts");
	//t.Start("d:\\long_clock.ts");
#endif 

#if 0  // working from external buffer 
	

	m_InStream = new ifstream("d:\\truck.ts", ios::in | ios::binary);
	m_InStream->seekg(0, ios::end);
	pos = m_InStream->tellg();
	m_InStream->seekg(0, ios::beg);
	pFileBuffer = new uint8_t[(uint32_t)pos];
	m_InStream->read((char *)pFileBuffer, (uint32_t)pos);
	m_InStream->close();
	  
	//t.Start(pFileBuffer, (uint32_t)pos);  another option to work from complete external buffer 
	t.PrintConfig(true);


	// work from worker thread where the buffer is filled into a fifo
	t.InitTSWorker(100, 188 * 1010);
	
	thread t1(Process);
	t.WaitWorker();
	Sleep(24000);
	t.StopWorker();

	delete pFileBuffer;
#endif 
	 
}



 