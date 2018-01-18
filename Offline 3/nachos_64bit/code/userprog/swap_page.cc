//
// Created by rafid on 1/18/18.
//

#include "swap_page.h"

SwapPage::SwapPage(int vpn, char *page) {
    this->vpn = vpn;
    this->page = page;
}

char *SwapPage::getPage() {
    return page;
}

void SwapPage::setPage(char *page) {
    SwapPage::page = page;
}

int SwapPage::getVpn() {
    return vpn;
}

void SwapPage::setVpn(int vpn) {
    SwapPage::vpn = vpn;
}
