//
// Created by rafid on 1/18/18.
//

#ifndef SWAPPAGE_H
#define SWAPPAGE_H


#include "../machine/machine.h"

class SwapPage {
    char *page;
    int vpn; //vpn = virtual page no

public:
    SwapPage(int vpn, char *page);

    char *getPage();

    void setPage(char *page);

    int getVpn();

    void setVpn(int vpn);
};


#endif //SWAPPAGE_H
