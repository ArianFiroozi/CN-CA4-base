#include <iostream>
#include <stdio.h>

#include "ip.h"

using namespace std;

bool mask_calculated_correctly()
{
    Mask mask;
    mask.strToMask("0.0.2.10");
    if (mask.addrToNum() != 522)
    {
        cerr << "mask returns wrong number!" << endl;
        return false;
    }
    return true;
}

bool mask_to_int_correct()
{
    IPv4 myIP("255.224.0.0", "192.168.1.12");
    if (myIP.maskToInt() != 11)
    {
        cerr << "mask convert to wrong number!" << endl;
        return false;
    }
    return true;
}

bool net_address_is_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    if (myIP.netAddr().toStr().toStdString()!="192.168.1.0" ||
        myIP.broadcastAddr().toStr().toStdString()!="192.168.1.255")
    {
        cerr << "wrong broadcast or net address returned!" << endl;
        return false;
    }

    return true;
}

bool ip_to_str_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    if (myIP.getIPStr().toStdString()!="192.168.1.12/24")
    {
        cerr << "wrong ip to string format: " << myIP.getIPStr().toStdString() << endl;
        return false;
    }

    return true;
}

bool ip_comparision_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    IPv4 otherIP1("255.255.255.0", "192.168.1.94");
    IPv4 otherIP2("255.255.255.0", "192.168.10.12");
    if (!myIP.includes(&otherIP1) || myIP.includes(&otherIP2))
    {
        cerr << "incorrect ip comparision!" << endl;
        return false;
    }

    return true;
}

int run_ip_tests()
{
    int errors = 0;

    errors += (mask_calculated_correctly()==true)?0:1;
    errors += (mask_to_int_correct()==true)?0:1;
    errors += (net_address_is_correct()==true)?0:1;
    errors += (ip_to_str_correct()==true)?0:1;
    errors += (ip_comparision_correct()==true)?0:1;

    return errors;
}
