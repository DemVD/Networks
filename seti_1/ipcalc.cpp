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

void IPClass::setIP(const vector<byte_t> vecByte){
    IP[0] = vecByte[0];
    IP[1] = vecByte[1];
    IP[2] = vecByte[2];
    IP[3] = vecByte[3];
}

void IPClass::setIP(const QString QStr){
    QStringList QStrL = QStr.split(".");
    QStringList lastOctet = QStrL[3].split("/");
    vector<QString> vecQStr = {QStrL[0], QStrL[1], QStrL[2], lastOctet[0]};
    setIP(vecQStr);
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

vector<vector<byte_t>> IPClass::getVectOfIPNetsInRange(bool getOnlyOnePair) const{
    unsigned Tb = 8; // total bits
    unsigned l = Mask/8; // Цел.ч. от дел. - кол-во полных байтов
    unsigned n = Mask%8; // Ост. ч. - биты в оставшемся байте
    unsigned subnetsNum1 = pow(2,n); // ammount of subnets
    unsigned m1 = Tb - n; // bits left for host
    unsigned Delta1 = pow(2,m1); // last bit value
    vector<byte_t> tempIpVec = IP;
    vector<vector<byte_t>> subNetsVec;

    if(getOnlyOnePair){
        subnetsNum1 = 1;
    }
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
        subNetsVec.push_back(tempIpVec);
        break;
    }
    return subNetsVec;
}

byte_t IPClass::getMask() const{
    return Mask;
}

void IPClass::setMask(byte_t m){
    Mask = m;
}

void IPClass::setMask(QString s){
    if(s.size() >= 0 && s.size() <= 2){
        Mask = s.toUInt();
    }
    else{
        QStringList QStrL = s.split("/");
        setMask(QStrL[1]);
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

void IPClass::setUserInputHosts(unsigned inp){
    UserInputHosts = inp;
}

void IPClass::setUserInputHosts(QString inp){
    UserInputHosts = inp.toInt();
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

vector<byte_t> IPClass::getIP() const{
    return IP;
}

unsigned IPClass::getUserInputHosts() const{
    return UserInputHosts;
}

unsigned IPClass::getAvailableSubnets() const{
    return AvailableSubnets;
}

unsigned IPClass::getAvailableSubnetsTAG() const{
    return AvailableSubnetsTAG;
}

unsigned IPClass::getAvailableHosts() const{
    return AvailableHosts;
}

bool IPClass::getIsRightSon() const{
    return isRightSon;
}

vector<vector<QString>>* IPClass::getVecPairs_IpRfc(){
    return &vecPairs_IpRfc;
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

QString IPClass::getQStrUserInputHosts() const{
    QString QStrUserInputHosts = QString::number(UserInputHosts);
    return QStrUserInputHosts;
}

bool IPClass::operator>=(IPClass &ipVar){
    if(this->getIP()[0] < ipVar.getIP()[0]){
        return false;
    }
    else if(this->getIP()[1] < ipVar.getIP()[1]){
        return false;
    }
    else if(this->getIP()[2] < ipVar.getIP()[2]){
        return false;
    }
    else if(this->getIP()[3] < ipVar.getIP()[3]){
        return false;
    }
    else{
        return true;
    }
}

bool IPClass::operator<=(IPClass &ipVar){
    if(this->getIP()[0] > ipVar.getIP()[0]){
        return false;
    }
    else if(this->getIP()[1] > ipVar.getIP()[1]){
        return false;
    }
    else if(this->getIP()[2] > ipVar.getIP()[2]){
        return false;
    }
    else if(this->getIP()[3] > ipVar.getIP()[3]){
        return false;
    }
    else{
        return true;
    }
}

bool IPClass::operator==(IPClass &ipVar){
    if(this->getIP()[0] != ipVar.getIP()[0]){
        return false;
    }
    else if(this->getIP()[1] != ipVar.getIP()[1]){
        return false;
    }
    else if(this->getIP()[2] != ipVar.getIP()[2]){
        return false;
    }
    else if(this->getIP()[3] != ipVar.getIP()[3]){
        return false;
    }
    else{
        return true;
    }
}

void IPClass::produceAdressForHosts(){
    struct Pair{
        unsigned freeHostsNum; // for 8-1 (mask 255.255.255.x) 254, 126, 62, 30, 14, 6, 2
        unsigned bitValue; // bit val. to calc the final mask in the end.
    };

    unsigned l = Mask/8; // Цел.ч. от дел. - кол-во полных байтов
    unsigned maskOpos = 32 - Mask; // HOST BITS
    vector<Pair> vecPair;
    double freeHosts;
    double userInpHosts = UserInputHosts;
    Pair p;

    switch (l) {
    case 4: // mask = 255.255.255.255, maskOpos = 0
        setIP(0,0,0,0);
        setMask(32);
        calcIPData();
        break;
    default:
        for(unsigned maskBit=maskOpos;maskBit>1;maskBit--){
            freeHosts = pow(2,maskBit)-2; // 254, 126, 62...
            if(userInpHosts/freeHosts <= 1){ // as long as <=1 hosts can fit.
                p.bitValue = maskBit;
                p.freeHostsNum = freeHosts;
                vecPair.push_back(p);
            }
            else{
                break;
            }
        }
    }
    p = vecPair.back();
    setMask(32-p.bitValue);
}

vector<IPClass> IPClass::produceOneLevelBranch(){
    unsigned l = Mask/8; // Цел.ч. от дел. - кол-во полных байтов
    unsigned n = Mask%8; // Ост. ч. - биты в оставшемся байте
    unsigned subnetsNum1 = pow(2,n); // ammount of subnets
    subnetsNum1 *= 2;

    IPClass tempIP;
    tempIP.setIP(IP);
    tempIP.setMask(Mask+1);
    tempIP.calcIPData();
    vector<IPClass> vecPair;

    unsigned currHosts = 0;
    unsigned cnt = 0;

    switch (l) {
    case 0:{ // mask = X.0.0.0
        unsigned hostBits = 32 - (Mask+1) - 24;
        unsigned hostsAmm = pow(2,hostBits);
        while(currHosts < 255){
            cnt++;
            if(tempIP.SubNetID[0] >= currHosts && tempIP.SubNetID[0] < (currHosts+hostsAmm)){
                tempIP.setIP(currHosts,SubNetID[1],SubNetID[2], SubNetID[3]);
                tempIP.setMask(Mask+1);
                tempIP.calcIPData();
                break;
            }
            currHosts+=hostsAmm;
        }
        if(cnt == subnetsNum1){
            IPClass tempIP2;
            tempIP2.setIP((tempIP.getIP()[0] - hostsAmm), tempIP.getIP()[1], tempIP.getIP()[2], tempIP.getIP()[3]);
            tempIP2.setMask(Mask+1);
            tempIP2.calcIPData();
            vecPair.push_back(tempIP2);
            vecPair.push_back(tempIP);
            isRightSon = true;
        }
        else{
            vecPair.push_back(tempIP);
            tempIP.setIP((currHosts + hostsAmm),tempIP.getIP()[1],tempIP.getIP()[2], tempIP.getIP()[3]);
            tempIP.setMask(Mask+1);
            tempIP.calcIPData();
            vecPair.push_back(tempIP);
            isRightSon = false;
        }
        break;
    }
    case 1:{ // mask = 255.X.0.0
        unsigned hostBits = 32 - (Mask+1) - 16;
        unsigned hostsAmm = pow(2,hostBits);
        while(currHosts < 255){
            cnt++;
            if(tempIP.SubNetID[1] >= currHosts && tempIP.SubNetID[1] < (currHosts+hostsAmm)){
                tempIP.setIP(IP[0],currHosts,SubNetID[2], SubNetID[3]);
                tempIP.setMask(Mask+1);
                tempIP.calcIPData();
                break;
            }
            currHosts+=hostsAmm;
        }
        if(cnt == subnetsNum1){
            IPClass tempIP2;
            tempIP2.setIP(tempIP.getIP()[0], (tempIP.getIP()[1] - hostsAmm), tempIP.getIP()[2], tempIP.getIP()[3]);
            tempIP2.setMask(Mask+1);
            tempIP2.calcIPData();
            vecPair.push_back(tempIP2);
            vecPair.push_back(tempIP);
            isRightSon = true;
        }
        else{
            vecPair.push_back(tempIP);
            tempIP.setIP(tempIP.getIP()[0],(currHosts + hostsAmm),tempIP.getIP()[2], tempIP.getIP()[3]);
            tempIP.setMask(Mask+1);
            tempIP.calcIPData();
            vecPair.push_back(tempIP);
            isRightSon = false;
        }
        break;
    }
    case 2:{ // mask = 255.255.X.0
        unsigned hostBits = 32 - (Mask+1) - 8;
        unsigned hostsAmm = pow(2,hostBits);
        while(currHosts < 255){
            cnt++;
            if(tempIP.SubNetID[2] >= currHosts && tempIP.SubNetID[2] < (currHosts+hostsAmm)){
                tempIP.setIP(IP[0],IP[1], currHosts, SubNetID[3]);
                tempIP.setMask(Mask+1);
                tempIP.calcIPData();
                break;
            }
            currHosts+=hostsAmm;
        }
        if(cnt == subnetsNum1){
            IPClass tempIP2;
            tempIP2.setIP(tempIP.getIP()[0], tempIP.getIP()[1], (tempIP.getIP()[2] - hostsAmm), tempIP.getIP()[3]);
            tempIP2.setMask(Mask+1);
            tempIP2.calcIPData();
            vecPair.push_back(tempIP2);
            vecPair.push_back(tempIP);
            isRightSon = true;
        }
        else{
            vecPair.push_back(tempIP);
            tempIP.setIP(tempIP.getIP()[0],tempIP.getIP()[1],(currHosts + hostsAmm), tempIP.getIP()[3]);
            tempIP.setMask(Mask+1);
            tempIP.calcIPData();
            vecPair.push_back(tempIP);
            isRightSon = false;
        }
        break;
    }
    case 3:{ // mask = 255.255.255.X
        unsigned hostBits = 32 - (Mask+1);
        unsigned hostsAmm = pow(2,hostBits);
        while(currHosts < 255){
            cnt++;
            if(tempIP.SubNetID[3] >= currHosts && tempIP.SubNetID[3] < (currHosts+hostsAmm)){
                tempIP.setIP(IP[0],IP[1],IP[2], currHosts);
                tempIP.setMask(Mask+1);
                tempIP.calcIPData();
                break;
            }
            currHosts+=hostsAmm;
        }
        if(cnt == subnetsNum1){
            IPClass tempIP2;
            tempIP2.setIP(tempIP.getIP()[0], tempIP.getIP()[1], tempIP.getIP()[2], (tempIP.getIP()[3] - hostsAmm) );
            tempIP2.setMask(Mask+1);
            tempIP2.calcIPData();
            vecPair.push_back(tempIP2);
            vecPair.push_back(tempIP);
            isRightSon = true;
        }
        else{
            vecPair.push_back(tempIP);
            tempIP.setIP(tempIP.getIP()[0],tempIP.getIP()[1],tempIP.getIP()[2], (currHosts + hostsAmm) );
            tempIP.setMask(Mask+1);
            tempIP.calcIPData();
            vecPair.push_back(tempIP);
            isRightSon = false;
        }
        break;
    }
    case 4:{ // mask = 255.255.255.255
        break;
    }

    }

    return vecPair;
}

void IPClass::checkRFC(const IPClass ipVar){



}

void IPClass::setRFCVect(){
    // should be called once
    // {IpQStr, RfcDescrQStr}
    vecPairs_IpRfc.push_back({"192.0.2.0/24 192.0.2.255/24",
    "RFC 5737, (TEST-NET-1) reserved for documentation."});
    vecPairs_IpRfc.push_back({"198.51.100.0/24 198.51.100.255/24",
    "RFC 5737, (TEST-NET-2) reserved for documentation."});
    vecPairs_IpRfc.push_back({"203.0.113.0/24 203.0.113.255/24",
    "RFC 5737, (TEST-NET-3) reserved for documentation."});

    vecPairs_IpRfc.push_back({"10.0.0.0/8 10.255.255.255/8",
    "RFC 3330,1918, reserved by IANA for private internets."});
    vecPairs_IpRfc.push_back({"172.16.0.0/12 172.31.255.255/12",
    "RFC 3330,1918, reserved by IANA for private internets."});
    vecPairs_IpRfc.push_back({"192.168.0.0/16 192.168.255.255/16",
    "RFC 3330,1918, reserved by IANA for private internets."});

    vecPairs_IpRfc.push_back({"192.88.99.0/24 192.88.99.255/24",
    "RFC 3330,3068, allocated for use as 6to4 relay anycast addresses."});

    vecPairs_IpRfc.push_back({"169.254.0.0/16 169.254.255.255/16",
    "RFC 3927, reserved for Link-Local addressing."});

    vecPairs_IpRfc.push_back({"224.0.0.0/24 224.0.0.253/24",
    "RFC 1122, reserved for 'all-hosts' group."});

    // found in rfc 3330
    vecPairs_IpRfc.push_back({"0.0.0.0/8 0.255.255.255/8",
    "RFC 3330,1700, reserved by IANA for source hosts on 'this' network."});
    vecPairs_IpRfc.push_back({"127.0.0.0/8 127.255.255.255/8",
    "RFC 3330,1700, reserved for use as the Internet host loopback address."});
    vecPairs_IpRfc.push_back({"128.0.0.0/16 128.0.255.255/16",
    "RFC 3330, reserved by IANA as lowest of the former Class B addresses."});
    vecPairs_IpRfc.push_back({"191.255.0.0/16 191.255.255.255/16",
    "RFC 3330, reserved by IANA as highest to the former Class B addresses."});
    vecPairs_IpRfc.push_back({"192.0.0.0/24 192.0.0.255/24",
    "RFC 3330, reserved by IANA as lowest of the former Class C addresses."});
    vecPairs_IpRfc.push_back({"192.0.2.0/24 192.0.2.255/24",
    "RFC 3330, reserved by IANA as 'TEST-NET' - documentation and example code."});
    vecPairs_IpRfc.push_back({"198.18.0.0/15 198.19.255.255/15",
    "RFC 3330,2544, allocated for use in benchmark tests of network interconnect devices."});
    vecPairs_IpRfc.push_back({"223.255.255.0/24 223.255.255.255/24",
    "RFC 3330, reserved by IANA as highest of the former Class C addresses"});
    vecPairs_IpRfc.push_back({"224.0.0.0/4 239.255.255.255/4",
    "RFC 3330,3171, allocated for use in IPv4 multicast address assignments."});
    vecPairs_IpRfc.push_back({"240.0.0.0/4 255.255.255.255/4",
    "RFC 3330,1700, reserved for future use."});
}
