#include <QCoreApplication>
#include "log.h"


using namespace Logger;

int main()
{
    Log log;

    std::thread t1{[&] {
        log << "t1 1" << "A" << std::endl << "t1 2" << "B" << std::endl;
    }};

    log
        << "abc"
        << 42
        << std::endl;

    std::thread t2{[&] {
        log << "t2 1" << "C" << std::endl << "t2 2" << "D" << std::endl;
    }};

    t1.join();
    t2.join();

    return 0;
}
