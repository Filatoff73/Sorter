#include "Sorter.h"
#include <fstream>
#include <algorithm>
#include <functional>
//#include <filesystem>
#include <map>
#include <iostream>
#include <thread>
#include <string>


Sorter::Sorter(const std::string& inputFilename, const std::string& ouputFilename, const unsigned& maxMemory, const unsigned& threadCount):
	_inputFilename(inputFilename),
	_outputFilename(ouputFilename),
	_tempDirectory("."),
	_maxMemory(maxMemory>1?maxMemory-1:maxMemory), //резервируем 1мб на создание временных переменных и тп
	_threadCount(threadCount),
	_bufElementCount(_maxMemory *1000000/ sizeof(unsigned)/ threadCount),
	_counterFiles(-1)
{
}


void Sorter::Sort()
{
	std::ifstream in(_inputFilename, std::ios_base::in | std::ios_base::binary);
	if (!in) {
		throw std::runtime_error("Error opening file");
	}
// 	std::experimental::filesystem::remove_all(_tempDirectory);
// 	std::experimental::filesystem::create_directory(_tempDirectory);
	
	std::vector<std::thread> thrVector;
	for (unsigned i = 0; i < _threadCount; i++)
		thrVector.push_back(std::thread(&Sorter::sortFirst,this,std::ref(in)));

	for (unsigned i = 0; i < _threadCount; i++)
		thrVector[i].join();
	
	in.close();

	sortSecond();

}

unsigned Sorter::getElementRemain(std::ifstream& in)
{
	auto begin = in.tellg();
	in.seekg(0, std::ios::end);
	auto end = in.tellg();
	in.seekg(begin);

	return (end - begin)/sizeof(unsigned);
}

void Sorter::sortFirst(std::ifstream& in)
{
	unsigned remainElem = 0;
	while (true)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		remainElem = getElementRemain(in);
		if (remainElem == 0)
			break;

		unsigned size = _bufElementCount > remainElem ? remainElem : _bufElementCount;
		std::vector<unsigned> buf(size);
		in.read((char*)&buf[0], sizeof(buf[0])*size);
		lock.unlock();

		std::sort(buf.begin(), buf.end());

		_counterFiles++;
		std::ofstream out(_tempDirectory + "/out_" + std::to_string(_counterFiles) + ".bin", std::ios::out | std::ios::binary);
		if (!out) {
			throw std::runtime_error("Error output file!");
		}
		out.write((char*)&buf[0], sizeof(buf[0])*size);
		out.close();
		
	}
}

void Sorter::sortSecond()
{
	std::ofstream out(_outputFilename, std::ios::out | std::ios::binary);
	if (!out) {
		throw std::runtime_error("Error output file!");
	}
	std::vector<std::ifstream> files;
	std::vector<int> values(_counterFiles+1);
	for (unsigned i = 0; i <= _counterFiles; ++i)
	{
		files.push_back(std::ifstream(_tempDirectory + "/out_" + std::to_string(i) + ".bin", std::ios::out | std::ios::binary));
		if (!files[i]) {
			throw std::runtime_error("Error opening file");
		}
		unsigned readedValue;
		files[i].read((char*)&readedValue, sizeof(readedValue));
		if (files[i])
			values[i] = readedValue;
	}

	unsigned maxBufCount = _maxMemory * 1000000 / sizeof(unsigned)/2;
	std::vector<unsigned> buf(maxBufCount+1);
	unsigned bufIndex = 0;

//	std::thread t(&Sorter::writer, this, std::ref(out), std::ref(buf), std::ref(bufIndex));
	while (true)
	{
		int minValue = getMinValueAmongPlus(values);
		if(minValue<0)
			break;
		for (unsigned i = 0; i < values.size(); i++)
		{
			if (values[i] == minValue)
			{
// 				std::unique_lock<std::mutex> locker(_mutex);				
				buf[bufIndex++] = minValue;
				if (bufIndex == maxBufCount)
				{
					out.write((char*)&buf[0], sizeof(buf[0])*bufIndex);
					bufIndex = 0;

// 					_is_notified = true;
// 					_cond_variable.notify_one();
				}
//				locker.unlock();
				
				values[i] = -1;
				if (!files[i])
					continue;
				unsigned readedValue;
				files[i].read((char*)&readedValue, sizeof(readedValue));
				if (files[i])
					values[i] = readedValue;
			}
		}

	}
	if (bufIndex>0)
	{
// 		_is_notified = true;
// 		_cond_variable.notify_one();

		out.write((char*)&buf[0], sizeof(buf[0])*bufIndex);
		bufIndex = 0;
	}

//  	_is_done = true;
// 	    _is_notified = true;
//  	_cond_variable.notify_one();
//  	t.join();
	out.close();

}

int Sorter::getMinValueAmongPlus(const std::vector<int>& vector)
{
	if (vector.size() == 0)
		throw std::runtime_error("Vector size is null!");
	int minValue = vector[0];
	for (unsigned i = 1; i < vector.size(); i++)
	{
		if (vector[i] > 0 && (minValue > vector[i] || minValue<0))
			minValue = vector[i];
	}
	return minValue;

}

// void Sorter::writer(std::ofstream& out, std::vector<unsigned>& buf, unsigned& currIndex)
// {
// 	while (!_is_done)
// 	{
// 		std::unique_lock<std::mutex> locker(_mutex);
// 		_cond_variable.wait(locker, [&]() {return _is_notified; });
// 		if(currIndex>0)
// 			out.write((char*)&buf[0], sizeof(buf[0])*currIndex);
// 		currIndex = 0;
// 		_is_notified = false;
// 
// 	}
// }



