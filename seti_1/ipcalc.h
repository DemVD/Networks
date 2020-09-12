#ifndef IPCALC_H
#define IPCALC_H

#include <qdebug.h>
#include <vector>
#include <QString>
#include <math.h>


typedef uint8_t byte_t;

using namespace std;


class IPClass{
private:
    vector<byte_t> IP; // Пустой вектор типа byte_t
    byte_t Mask; // Маска
    vector<byte_t> MaskVectorBits; // Маска в битовом виде
    vector<byte_t> SubNetID; // Идентификатор подсети
    vector<byte_t> BroadCast; // adress for broadcast
    vector<byte_t> MinIPAdress; // Минимальное значение адреса в подсети
    vector<byte_t> MaxIPAdress; // Максимальное значение адреса в подсети
    unsigned UserInputHosts;
    unsigned AvailableSubnets; // свободные подсети с соответствующей маской
    unsigned AvailableSubnetsTAG; // вторичное значение
    unsigned AvailableHosts; // количество свободных адресов
public:
    IPClass(); // констр-р по умолч.
    // set ip data methods
    void setIP(const byte_t b0=0, const byte_t b1=0, // установить IP по 4м пер.
               const byte_t b2=0, const byte_t b3=0);
    void setIP(const vector<QString> vecQStr);
    void calcMaskVectorBits(byte_t b0); // calculate mask
    void calcIPData();

    // more set ip data methods
    bool compareIPs(const IPClass &IPVar) const; // checks if IPvar is greater Or equal to the inside IP
    void setMask(byte_t m);
    void setSubNetID(vector<byte_t> MaskVector);
    void setAvailableHosts(byte_t m);
    void setMinIPAdress(vector<byte_t> SNID);
    void setMaxIPAdress(vector<byte_t> BC);

    // conversion methods
    vector<vector<byte_t>> getVectOfIPNetsInRange() const;
    vector<int> convVecByteToVecInt(const vector<byte_t> vecOfByte) const;
    QString convVecToQStr(const vector<byte_t> vecOfByte) const;

    // get methods
    unsigned getUserInputHosts() const;
    unsigned getAvailableSubnets() const;
    unsigned getAvailableSubnetsTAG() const;
    unsigned getAvailableHosts() const;

    // get QStr methods
    QString getQStrIP() const; // QString
    QString getQStrIPAndMask() const; // QString + маска
    QString getQStrMaskVectorBits() const;
    QString getQStrSubNetID() const;
    QString getQStrBroadCast() const;
    QString getQStrMinIPAdress() const;
    QString getQStrMaxIPAdress() const;
    QString getQStrAvailableHosts() const;
    QString getQStrAvailableSubnets() const;
    QString getQStrAvailableSubnetsTAG() const;

    // calculation and logic methods
    vector<IPClass> produceAdressForHosts();
};

#endif // IPCALC_H
