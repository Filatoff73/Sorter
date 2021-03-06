// Sorter.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <iostream>
#include "Sorter.h"
#include <string>



//размер в мегабайтах
void generateFile(const std::string& filename, const int& size)
{
	std::ofstream out(filename, std::ios::out | std::ios::binary);
	if (!out) {
		throw std::runtime_error("Error output file!");
	}

	srand(static_cast<unsigned int>(time(0)));
	for (int j = 0; j < 6; j++)
    for (int i = 0; i < size*1000000/sizeof(unsigned); i++)
	{
		unsigned randomVal = rand()+1;
		out.write((char*)&randomVal, sizeof(randomVal));
	}
	out.close();
}

void printFile(const std::string& filename)
{
	std::ifstream in(filename, std::ios_base::in | std::ios_base::binary);
	if (!in) {
		throw std::runtime_error("Error opening file");
	}

	while (true)
	{
		unsigned readedValue;
		in.read((char*)&readedValue, sizeof(readedValue));		
		if (!in)
			break;
		std::cout << readedValue << "\n";
	}
}

int main()
{
	try
	{
//		generateFile("input", 10);
//		printFile("input");

 		Sorter sorter("input.bin", "output.bin", 120, 4);
 		//auto start = clock();
		sorter.Sort();
		//auto end = clock();
 		//std::cout << "\n\n" << (double)(end - start) / CLOCKS_PER_SEC << "\n\n";

//		printFile("output");
	}
	catch (const std::runtime_error & ex)
	{
		std::cout << ex.what();
	}

	//_getch();

    return 0;
}

