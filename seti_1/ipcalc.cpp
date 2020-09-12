#include "ipcalc.h"

IPClass::IPClass(){
    IP = {0,0,0,0}; // Резервируем 4 элемента типа byte_t for IP
    MaskVectorBits = {0,0,0,0}; // Резервируем 4 элемента типа byte_t for MASK
    SubNetID = {0,0,0,0}; // Резервируем 4 элемента типа byte_t for subnet id
    BroadCast = {0,0,0,0};
    MinIPAdress = {0,0,0,0};
    MaxIPAdress = {0,0,0,0};
}

void IPClass::setIP(const byte_t b0, const byte_t b1,
                    const byte_t b2, const byte_t b3){
    IP[0] = b0;
    IP[1] = b1;
    IP[2] = b2;
    IP[3] = b3;
}

void IPClass::setIP(const vector<QString> vecQStr){
    if(vecQStr.size() >= 4){ // first 4 octets
        vector<byte_t> IPTEMP(4);
        for(unsigned i=0; i<4;i++){ // assign to each octet its val, or 0 if range is incorrect
            if(vecQStr[i].toInt() >= 0 && vecQStr[i].toInt() <= 255){
                IPTEMP[i] = vecQStr[i].toInt();
            }
            else{
                IPTEMP[i] = 0;
                qDebug()<<"ERROR: in ipcalc.cpp IP Octet "+QString::number(i)+" value "+vecQStr[i]+
                          " is not in range 0-255.";
            }
        }
        IP = IPTEMP;
    }
    if(vecQStr.size() >= 5){ // mask in vector exists
        int temp = vecQStr[4].toInt();
        if(temp >= 0 && temp <= 32){ // check mask size
            Mask =  temp;
            calcMaskVectorBits(Mask);
        }
        else{
            Mask = 0;
            qDebug()<<"ERROR: in ipcalc.cpp IP MASK value "+vecQStr[4]+" is not in range 0-32.";
        }
    }
    if(vecQStr.size() >= 6){ // HOSTS included
        int num = vecQStr[5].toInt();
        UserInputHosts = num;
    }
}

void IPClass::calcMaskVectorBits(byte_t maska){
    vector<byte_t> MaskVectorBitsTEMP = {0,0,0,0};
    unsigned l,r;
    l = maska/8; // Цел.ч. от дел. - кол-во полных байтов
    r = maska%8; // Ост. ч. - биты в оставшемся байте
    for(unsigned i=0; i<l; i++){ // заполняем октеты полностью
        MaskVectorBitsTEMP[i] = 255; // full fill mask bits
    }
    if(l < 4){ // switch case for last octet (half-filled)
        switch (r){
        case 1: // 1/8 is filled
            MaskVectorBitsTEMP[l] = 128; // sum = 128 for mask
            break;
        case 2: // 2/8 etc..
            MaskVectorBitsTEMP[l] = 192; // sum = 192 for mask
            break;
        case 3:
            MaskVectorBitsTEMP[l] = 224;
            break;
        case 4:
            MaskVectorBitsTEMP[l] = 240;
            break;
        case 5:
            MaskVectorBitsTEMP[l] = 248;
            break;
        case 6:
            MaskVectorBitsTEMP[l] = 252;
            break;
        case 7:
            MaskVectorBitsTEMP[l] = 254;
            break;
        }
        MaskVectorBits = MaskVectorBitsTEMP;
    }
    else{ // MASK == 32
        MaskVectorBits = MaskVectorBitsTEMP;
    }
}

void IPClass::calcIPData(){
    vector<byte_t> tempIpVec = IP;
    vector<vector<byte_t>> subNetsVec;
    unsigned Tb = 8; // total bits
    unsigned l = Mask/8; // Цел.ч. от дел. - кол-во полных байтов
    unsigned n = Mask%8; // Ост. ч. - биты в оставшемся байте
    // SET n to n-1 value if wrong... ARGHHH
    unsigned m1 = Tb - n; // bits left for host
    unsigned m2 = Tb; // same, but for n=0 cases
    unsigned subnetsNum1 = pow(2,n); // ammount of subnets
    unsigned subnetsNum2 = 0; // same, but for n=0 cases
    unsigned Delta1 = pow(2,m1); // last bit value
    unsigned Delta2 = pow(2,m2); // same, but for when n=0 (m=8)
    unsigned Hosts1 = Delta1 -2; // hosts per subnet
    unsigned Hosts2 = Delta2 -2; // same, but for when n=0
    switch (l) {
    case 0: // mask = X.0.0.0
        subNetsVec = getVectOfIPNetsInRange();
        for(unsigned i=0;i<(subNetsVec.size()-1);i+=2){
            if(IP[0] >= subNetsVec[i][0] && IP[0] <= subNetsVec[i+1][0]){
                SubNetID = subNetsVec[i];
                BroadCast = subNetsVec[i+1];
                MinIPAdress = subNetsVec[i];
                MinIPAdress[3]++;
                MaxIPAdress = subNetsVec[i+1];
                MaxIPAdress[3]--;
                AvailableSubnets = subnetsNum1;
                AvailableSubnetsTAG = pow(2,(24+n));
                setAvailableHosts(Mask);
                break;
            }
        }
        break;
    case 1: // mask = 255.X.0.0
        subNetsVec = getVectOfIPNetsInRange();
        for(unsigned i=0;i<(subNetsVec.size()-1);i+=2){
            if(IP[1] >= subNetsVec[i][1] && IP[1] <= subNetsVec[i+1][1]){
                SubNetID = subNetsVec[i];
                BroadCast = subNetsVec[i+1];
                MinIPAdress = subNetsVec[i];
                MinIPAdress[3]++;
                MaxIPAdress = subNetsVec[i+1];
                MaxIPAdress[3]--;
                AvailableSubnets = subnetsNum1;
                AvailableSubnetsTAG = pow(2,(16+n));
                setAvailableHosts(Mask);
                break;
            }
        }
        break;
    case 2: // mask = 255.255.X.0
        subNetsVec = getVectOfIPNetsInRange();
        for(unsigned i=0;i<(subNetsVec.size()-1);i+=2){
            if(IP[2] >= subNetsVec[i][2] && IP[2] <= subNetsVec[i+1][2]){
                SubNetID = subNetsVec[i];
                BroadCast = subNetsVec[i+1];
                MinIPAdress = subNetsVec[i];
                MinIPAdress[3]++;
                MaxIPAdress = subNetsVec[i+1];
                MaxIPAdress[3]--;
                AvailableSubnets = subnetsNum1;
                AvailableSubnetsTAG = pow(2,(8+n));
                setAvailableHosts(Mask);
                break;
            }
        }
        break;
    case 3: // mask = 255.255.255.X
        subNetsVec = getVectOfIPNetsInRange();
        for(unsigned i=0;i<(subNetsVec.size()-1);i+=2){
            if(IP[3] >= subNetsVec[i][3] && IP[3] <= subNetsVec[i+1][3]){
                SubNetID = subNetsVec[i];
                BroadCast = subNetsVec[i+1];
                MinIPAdress = subNetsVec[i];
                MinIPAdress[3] = MinIPAdress[3]+1;
                MaxIPAdress = subNetsVec[i+1];
                MaxIPAdress[3] = MaxIPAdress[3]-1;
                AvailableSubnets = subnetsNum1;
                AvailableSubnetsTAG = subnetsNum1;
                AvailableHosts = Hosts1;
                break;
            }
        }
        break;
    case 4: // mask = 255.255.255.255
        SubNetID = IP;
        BroadCast = IP;
        MinIPAdress = IP;
        MaxIPAdress = IP;
        AvailableSubnets = 1;
        AvailableSubnetsTAG = 1;
        AvailableHosts = 1;
        break;
    }
}

vector<vector<byte_t>> IPClass::getVectOfIPNetsInRange() const{
    unsigned Tb = 8; // total bits
    unsigned l = Mask/8; // Цел.ч. от дел. - кол-во полных байтов
    unsigned n = Mask%8; // Ост. ч. - биты в оставшемся байте
    unsigned subnetsNum1 = pow(2,n); // ammount of subnets
    unsigned m1 = Tb - n; // bits left for host
    unsigned Delta1 = pow(2,m1); // last bit value
    vector<byte_t> tempIpVec = IP;
    vector<vector<byte_t>> subNetsVec;
    switch (l) {
    case 0: // mask = X.0.0.0
        for(unsigned i=0;i<subnetsNum1;i++){
            tempIpVec[0] = i*Delta1;
            tempIpVec[1] = 0;
            tempIpVec[2] = 0;
            tempIpVec[3] = 0;
            subNetsVec.push_back(tempIpVec);
            tempIpVec[0] = (i+1)*Delta1-1;
            tempIpVec[1] = 255;
            tempIpVec[2] = 255;
            tempIpVec[3] = 255;
            subNetsVec.push_back(tempIpVec);
        }
        break;
    case 1: // mask = 255.X.0.0
        for(unsigned i=0;i<subnetsNum1;i++){
            tempIpVec[1] = i*Delta1;
            tempIpVec[2] = 0;
            tempIpVec[3] = 0;
            subNetsVec.push_back(tempIpVec);
            tempIpVec[1] = (i+1)*Delta1-1;
            tempIpVec[2] = 255;
            tempIpVec[3] = 255;
            subNetsVec.push_back(tempIpVec);
        }
        break;
    case 2: // mask = 255.255.X.0
        for(unsigned i=0;i<subnetsNum1;i++){
            tempIpVec[2] = i*Delta1;
            tempIpVec[3] = 0;
            subNetsVec.push_back(tempIpVec);
            tempIpVec[2] = (i+1)*Delta1-1;
            tempIpVec[3] = 255;
            subNetsVec.push_back(tempIpVec);
        }
        break;
    case 3: // mask = 255.255.255.X
        for(unsigned i=0;i<subnetsNum1;i++){
            tempIpVec[3] = i*Delta1;
            subNetsVec.push_back(tempIpVec);
            tempIpVec[3] = (i+1)*Delta1-1;
            subNetsVec.push_back(tempIpVec);
        }
        break;
    case 4: // mask = 255.255.255.255
        break;
    }
    return subNetsVec;
}

// checks if IPVar is greater OR equal to the class IP
bool IPClass::compareIPs(const IPClass &IPVar) const{
    if(IP[0] != IPVar.IP[0]){
        if(IP[0] > IPVar.IP[0]) return false;
        else return true;
    }
    else if(IP[1] != IPVar.IP[1]){
        if(IP[1] > IPVar.IP[1]) return false;
        else return true;
    }
    else if(IP[2] != IPVar.IP[2]){
        if(IP[2] > IPVar.IP[2]) return false;
        else return true;
    }
    else if(IP[3] != IPVar.IP[3]){
        if(IP[3] > IPVar.IP[3]) return false;
        else return true;
    }
    else{
        return true;
    }
}

void IPClass::setSubNetID(vector<byte_t> MaskVector){
    vector<byte_t> SubNetIDTEMP(4);
    SubNetIDTEMP[0] = IP[0] & MaskVector[0];
    SubNetIDTEMP[1] = IP[1] & MaskVector[1];
    SubNetIDTEMP[2] = IP[2] & MaskVector[2];
    SubNetIDTEMP[3] = IP[3] & MaskVector[3];
    SubNetID = SubNetIDTEMP;
}

void IPClass::setAvailableHosts(byte_t m){
    unsigned lim = 32 - m;
    unsigned cnt = 0;
    for(unsigned i=0;i<lim;i++){
        cnt += pow(2,i);
    }
    cnt--;
    AvailableHosts = cnt;
}

void IPClass::setMinIPAdress(vector<byte_t> SNID){
    vector<byte_t> tempVec = SNID;
    tempVec[3] = tempVec[3]+1;
    MinIPAdress = tempVec;
}

void IPClass::setMaxIPAdress(vector<byte_t> BC){
    vector<byte_t> tempVec = BC;
    tempVec[3] = tempVec[3]-1;
    MaxIPAdress = tempVec;
}

vector<int> IPClass::convVecByteToVecInt(const vector<byte_t> vecOfByte) const{
    vector<int> vecOfInts = {int(vecOfByte[0]), int(vecOfByte[1]),
                             int(vecOfByte[2]), int(vecOfByte[3])};
    return vecOfInts;
}

QString IPClass::convVecToQStr(const vector<byte_t> vecOfByte) const{
    vector<int> vecOfInts = convVecByteToVecInt(vecOfByte);
    QString QStrLine = (QString::number(vecOfInts[0])+"."+
                        QString::number(vecOfInts[1])+"."+
                        QString::number(vecOfInts[2])+"."+
                        QString::number(vecOfInts[3]));
    return QStrLine;
}

QString IPClass::getQStrIP() const{
    QString QStrIP = convVecToQStr(IP);
    return QStrIP;
}

QString IPClass::getQStrIPAndMask() const{
    QString QStrIPM = getQStrIP();
    QStrIPM += "/"+QString::number(int(Mask));
    return QStrIPM;
}

QString IPClass::getQStrMaskVectorBits() const{
    QString QStrMaskVec = convVecToQStr(MaskVectorBits);
    return QStrMaskVec;
}

QString IPClass::getQStrSubNetID() const{
    QString QStrSubNetID = convVecToQStr(SubNetID);
    return QStrSubNetID;
}

QString IPClass::getQStrBroadCast() const{
    QString QStrBroadCast = convVecToQStr(BroadCast);
    return QStrBroadCast;
}

QString IPClass::getQStrMinIPAdress() const{
    QString QStrMinIPAdr = convVecToQStr(MinIPAdress);
    return QStrMinIPAdr;
}

QString IPClass::getQStrMaxIPAdress() const{
    QString QStrMaxIPAdr = convVecToQStr(MaxIPAdress);
    return QStrMaxIPAdr;
}

QString IPClass::getQStrAvailableHosts() const{
    QString QStrAvailableHosts = QString::number(AvailableHosts);
    return QStrAvailableHosts;
}

QString IPClass::getQStrAvailableSubnets() const{
    QString QStrAvailableSubnets = QString::number(AvailableSubnets);
    return QStrAvailableSubnets;
}

QString IPClass::getQStrAvailableSubnetsTAG() const{
    QString QStrAvailableSubnetsTAG = QString::number(AvailableSubnetsTAG);
    return QStrAvailableSubnetsTAG;
}
