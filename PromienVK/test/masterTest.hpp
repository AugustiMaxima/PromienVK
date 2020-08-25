#ifndef MASTER_TEST_H
#define MASTER_TEST_H
//Includes all the test cases

int confTest();

int dictTest();

int allocTest(bool stress = false);

int masterTest(bool stress = false);

#endif