#include <iostream>
#include <stdio.h>

#include "ipTest.cpp"
#include "portTest.cpp"

using namespace std;

int run_all_tests()
{
    int errors = 0;

    errors += run_ip_tests();
    errors += run_port_tests();

    return errors;
}
