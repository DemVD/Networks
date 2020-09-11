#ifndef LOG_H
#define LOG_H

#include <condition_variable>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <list>
#include <fstream>
#include <string>
#include <time.h>


using namespace std;

namespace Logger{

class Log{
public:
    Log() = default;
    Log(const Log&) = delete;
    Log(Log&&) = default;

    ~Log(){
        {
            lock_guard<mutex> lock{ mLogMutex };
            mClosing = true;
        }

        mNotificationVariable.notify_one();
        mThread.join();
    }

    // Something to log normal values
    template<class T>
    Log& operator <<(const T& value){
        return write(move(value));
    }

    // overloads for IO manipulators
    // they manipulate the output of stream
    Log& operator <<(ios& (*pf)(ios&)){
        return write(move(pf));
    }

    Log& operator <<(ios_base& (*pf)(ios_base&)){
        return write(move(pf));
    }

    Log& operator <<(ostream& (*pf)(ostream&)){
        return write(move(pf));
    }

    Log& operator =(const Log&) = delete;
    Log& operator =(Log&&) = default;

private:
    // sync string buffer
    class LogBuf : public stringbuf{
    public:
        explicit LogBuf(Log& log) : stringbuf{}, mLog{ log } {

        }

        virtual ~LogBuf() = default;

    protected:
        virtual int sync() override{
            mLog.handleSync();
            return stringbuf::sync();
        }

    private:
        Log& mLog;
    };

    // output stream implementation
    class LogStream : public ostream{
    public:
        explicit LogStream(Log& log) : ostream{ &mBuf }, mBuf{ log } {

        }

        virtual ~LogStream() = default;

        // method that will ret the data of longbuffer, clear it
        string getStringAndClear(){
            const auto strVal = mBuf.str();
            mBuf.str({});

            return strVal;
        }

    private:
        LogBuf mBuf;
    };

    // we need to store a thread id associated with each val
    struct LogEntry{
        thread::id mThreadId;
        function<void()> mFunctor;
    };

    // log file path
    const char *filePath = "C:/Users/Vladimir/documents/";

    // date and time at first start
    string startDate = currentDate();
    string startTime = currentTime();

    // list of functors, we can efficiently remove elements
    // from an arbitrary position
    list<LogEntry> mLogQueue;

    // iterator, points to our current element
    decltype(mLogQueue)::iterator mCurrentLog = end(mLogQueue);

    // we also need info for which thread were currently logging
    thread::id mCurrentThreadId;

    // to guard this list and use condition variable
    mutex mLogMutex;

    // condition variable
    condition_variable mNotificationVariable;

    // a flag to notify the thread if we're shutting down
    bool mClosing = false;
    // a flag to inform logging thread that a flush occured
    bool mFlushed = false;

    // for use to output values, replaces cout in write method
    LogStream mStream{ *this };

    // the thread itself
    thread mThread{ &Log::serialize, this };

    // method to handle sync commands and flush
    void handleSync(){
        mFlushed = true;
    }

    // logging function
    void serialize(){
        while (true){
            // with condition variables we need the unique lock
            unique_lock<mutex> lock{ mLogMutex };
            // wait untill closing, or there's something to log
            mNotificationVariable.wait(lock, [=] {
                return mClosing || mCurrentLog != end(mLogQueue);
                });

            //closing handling
            if (mClosing){
                while (!mLogQueue.empty()){
                    if (mCurrentLog == end(mLogQueue)){
                        mCurrentLog = begin(mLogQueue);
                        mCurrentThreadId = mCurrentLog->mThreadId;
                    }

                    // we go trough all the list and output everything
                    while (mCurrentLog != end(mLogQueue)){
                        if (mCurrentThreadId == mCurrentLog->mThreadId){
                            mCurrentLog->mFunctor();
                            mCurrentLog = mLogQueue.erase(mCurrentLog);
                        }
                        else{// if current log belongs to thread another than our
                             // currently logged forethread - we skip this element
                            ++mCurrentLog;
                        }
                    }
                }

                flush();
                break;
            }

            // flush method called here, similar to above in close
            do { // we know that there's at least one entry at this point

                // functor call
                mCurrentLog->mFunctor();

                if (mFlushed){ // if flushed flag is set (when a sync is encountered)
                    flush(); // flush the output

                    mLogQueue.erase(mCurrentLog); // erase curr log elem
                    mCurrentLog = begin(mLogQueue); // put curr log as first in queue

                    // if at this point curr log points to the end of the queue (queue empty)
                    if (mCurrentLog == end(mLogQueue)){ // reset currThreadId to an enpty one
                        mCurrentThreadId = {};
                        break;
                    }

                    // otherwise set currThreadId to currently logged thread
                    mCurrentThreadId = mCurrentLog->mThreadId;
                }
                else{ // if we have not encountered a flush (sync)
                    // then we move curr log pointer to the next log entry for currently locked thread
                    mCurrentLog = find_if(mLogQueue.erase(mCurrentLog), end(mLogQueue), [=](const auto& entry) {

                        // ret true if currThreadId == the expected thread Id
                        return mCurrentThreadId == entry.mThreadId;
                        });
                }
              // check if we have anything to log, and if we're still working in the same thread
            } while (mCurrentLog != end(mLogQueue) && mCurrentThreadId == mCurrentLog->mThreadId);
        }
    }

    // method to put something in queue
    template<class T>
    Log& write(T value){
        // get the id of curr thread
        const auto id = this_thread::get_id();

        // inserting data into queue - need to protect it
        lock_guard<mutex> lock{ mLogMutex };
        mLogQueue.emplace_back(LogEntry{ id,[=, value = move(value)] {
            mStream << value;
        } });

        // if currThreadId (when the log starts)
        if (mCurrentThreadId == thread::id{}){
            // assign the initial values to internal state
            mCurrentLog = begin(mLogQueue);
            mCurrentThreadId = mCurrentLog->mThreadId;
            // notify thread that something happened
            mNotificationVariable.notify_one();
        }
        // if currently logged thread == current thread
        else if (mCurrentThreadId == id){
            // check if current log points to end of list
            if (mCurrentLog == end(mLogQueue))
                // then point it to the -1 element
                mCurrentLog = prev(end(mLogQueue));

            // notify thread that we have a new element in queue
            mNotificationVariable.notify_one();
        }

        return *this;
    }

    // a method that will output the data
    void flush(){
        string str = mStream.getStringAndClear();
        cout << str;
        save(str);
        mFlushed = false;
    }

    // save to file
    void save(string &data){
        string bufStr = "log "+startDate+" "+startTime[0]+
                startTime[1]+"-"+startTime[3]+startTime[4]+
                ".txt";
        ofstream file(bufStr, std::ios_base::app);
        if(file.is_open() && bufStr != "" && data != ""){
            bufStr = currentTime()+"  |  ";
            file<<bufStr;
            file << data;
            file.close();
        }
    }

    // Get current date
    // Format YYYY-MM-DD
    const string currentDate(){
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

        return buf;
    }

    // Get current time
    // Format HH:mm:ss
    const string currentTime(){
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%X", &tstruct);

        return buf;
    }
};
}

#endif // LOG_H
