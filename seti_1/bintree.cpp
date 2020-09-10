#include "bintree.h"


BinTree::BinTree(){
    empty = true;
    left = nullptr;
    right = nullptr;
}

void BinTree::setIPData(const IPClass IPD){
    empty = false;
    ipData = IPD;
}

void BinTree::setIPData(const vector<int> ipVec){
    empty = false;
    ipData.setIP(ipVec[0],ipVec[1],ipVec[2],ipVec[3]);
}

IPClass BinTree::getIPData() const{
    return ipData;
}

void BinTree::insertLeaf(const BinTree* root, const IPClass IP){

}

void BinTree::setLeft(BinTree* r){
    left = r;
}

void BinTree::setRight(BinTree *r){
    right = r;
}

void BinTree::setEmptyFlagTRUE(){
    empty = true;
}

void BinTree::setEmptyFlagFALSE(){
    empty = false;
}

bool BinTree::checkEmptyFlag() const{
    return empty;
}
