#pragma once
#include <vector>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <list>
#include <mutex>
#include <queue>

class Sorter
{
public:
	Sorter(const std::string& inputFilename, const std::string& ouputFilename, const unsigned& maxMemory, const unsigned& threadCount);
	void Sort();
	

private:
	unsigned getElementRemain(std::ifstream& in);
	void sortFirst(std::ifstream& in);
	void sortSecond();
	int getMinValueAmongPlus(const std::vector<int>& vector);

	//Оказалось через cond_var медленнее
//	void writer(std::ofstream& out, std::vector<unsigned>& buf, unsigned& currIndex);


	const std::string _inputFilename, _outputFilename, _tempDirectory;
	const unsigned _maxMemory; //В мегабайтах
	const unsigned _threadCount;
	const long _bufElementCount; //Количество элементов в буфере
	std::atomic_int _counterFiles;
	std::mutex _mutex;

//  	std::condition_variable _cond_variable;
//  	bool _is_done = false;
// 	bool _is_notified = false;
};


