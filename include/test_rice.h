#ifndef TEST_RICE_H
#define TEST_RICE_H

#include "./rice.h"

void test_variables();
void test_initBitStream();
void test_writeBit();
void test_encode();
void test_exportCDE1();
void test_exportCDE2();
void test_exportCDE3();

void test_readBit();
void test_importSP();
void test_importIdx();
void test_decode();
void test_setStream();
void test_searchCDE();
void test_searchPrecompCDE();

void test_rice();
void test_ceri();

#endif