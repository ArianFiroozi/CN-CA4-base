#include <iostream>
#include <stdio.h>
#include <QVector>

#include "./headers/ip.h"

using namespace std;

QString mask_calculated_correctly()
{
    Mask mask;
    mask.strToMask("0.0.2.10");
    if (mask.addrToNum() != 522)
        return "mask returns wrong number!";

    return "";
}

QString mask_to_int_correct()
{
    IPv4 myIP("255.224.0.0", "192.168.1.12");
    if (myIP.maskToInt() != 11)
        return "mask convert to wrong number!";

    return "";
}

QString net_address_is_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    if (myIP.netAddr().toStr().toStdString()!="192.168.1.0" ||
        myIP.broadcastAddr().toStr().toStdString()!="192.168.1.255")
        return "wrong broadcast or net address returned!";

    return "";
}

QString ip_to_str_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    if (myIP.getIPStr().toStdString()!="192.168.1.12/24")
        return "wrong ip to string format!";

    return "";
}

QString ip_comparision_correct()
{
    IPv4 myIP("255.255.255.0", "192.168.1.12");
    IPv4 otherIP1("255.255.255.0", "192.168.1.94");
    IPv4 otherIP2("255.255.255.0", "192.168.10.12");
    if (!myIP.includes(otherIP1) || myIP.includes(otherIP2))
        return "incorrect ip comparision!";

    return "";
}

QVector<QString> run_ip_tests()
{
    QVector<QString> errors;

    errors += mask_calculated_correctly();
    errors += mask_to_int_correct();
    errors += net_address_is_correct();
    errors += ip_to_str_correct();
    errors += ip_comparision_correct();

    return errors;
}
