/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description: header file
 */

//#include <utils/Thread.h>
//#include <utils/Vector.h>

#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

//using namespace android;
#if !defined(_C_MSG_QUEUE_H)
#define _C_MSG_QUEUE_H

class CMessage {
public:
    CMessage():mDelayMs(0), mWhenMs(0), mType(0), mpData(nullptr) {}
    ~CMessage() {}

    std::chrono::milliseconds mDelayMs; // delay times, milliseconds
    std::chrono::milliseconds mWhenMs; // when, the msg will handle
    int mType;
    void *mpData;
    unsigned char mpPara[5120];
};

class CMsgQueueThread {
public:
    CMsgQueueThread() :mExitFlag(false) {}
    ~CMsgQueueThread() { stop(); }

    int startMsgQueue();
    void sendMsg(CMessage &msg);
    void removeMsg(CMessage &msg);
    void clearMsg();
    void stop();

private:
    bool  threadLoop();
    std::chrono::milliseconds getNowMs();//get system time , MS
    virtual void handleMessage(CMessage &msg) = 0;

    std::vector<CMessage> m_v_msg;
    std::condition_variable mGetMsgCondition;
    std::mutex mLockQueue;
    std::atomic<bool> mExitFlag;
    std::thread mThread;

};

/*class CHandler
{
    public:
    CHandler(CMsgQueueThread& msgQueue);
    ~CHandler();
    void sendMsg();
    void removeMsg();
    private:
    virtual void handleMessage(CMessage &msg);
};*/

#endif
