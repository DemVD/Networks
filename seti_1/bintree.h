#ifndef BINTREE_H
#define BINTREE_H
#include <ipcalc.h>


using namespace std;

class BinTree{
private:
    IPClass ipData;
    BinTree* left;
    BinTree* right;
    bool empty;
public:
    BinTree();
    void setIPData(const IPClass IPD);
    void setIPData(const vector<int> ipVec);
    IPClass getIPData() const;
    void insertLeaf(const BinTree *root, const IPClass IP);
    void setLeft(BinTree *r);
    void setRight(BinTree* r);
    void setEmptyFlagTRUE();
    void setEmptyFlagFALSE();
    bool checkEmptyFlag() const;
};


#endif // BINTREE_H
