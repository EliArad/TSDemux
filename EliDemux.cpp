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
	t.Start("d:\\truck.ts");

}

 