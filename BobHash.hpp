#pragma once 

#ifndef BOB_HASH_H
#define BOB_HASH_H

#include <stdio.h>

typedef unsigned int uint;

class BOBHash
{

public:

	BOBHash();
	~BOBHash();
	BOBHash(uint primeNum);
	void initialize(uint primeNum);
	uint run(const char * str, uint len);

private:

	uint primeNum;

};

#endif // !BOB_HASH_H
