/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description: c++ file
 */


#define LOG_MODULE_TAG "TV"
#define LOG_CLASS_TAG "CMessage"

#include "CMsgQueue.h"
#include <CTvLog.h>
#include <algorithm>

std::chrono::milliseconds CMsgQueueThread::getNowMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void CMsgQueueThread::sendMsg(CMessage &msg)
{
    std::lock_guard<std::mutex> lock(mLockQueue);
    msg.mWhenMs = getNowMs() + msg.mDelayMs;
    auto it = std::find_if(m_v_msg.begin(), m_v_msg.end(), [&msg](const CMessage &m) { return m.mWhenMs > msg.mWhenMs; });
    m_v_msg.insert(it, msg);
    CMessage msg1 = m_v_msg.front();
    LOGD("sendmsg now = %lld msg[0] when = %lld\n", getNowMs().count(), msg1.mWhenMs.count());
    mGetMsgCondition.notify_one();
}

void CMsgQueueThread::removeMsg(CMessage &msg)
{
    std::lock_guard<std::mutex> lock(mLockQueue);
    size_t beforeSize = m_v_msg.size();
    m_v_msg.erase(std::remove_if(m_v_msg.begin(), m_v_msg.end(), [&msg](const CMessage &m) { return m.mType == msg.mType; }), m_v_msg.end());
    //some msg removeed
    if (beforeSize > m_v_msg.size())
        mGetMsgCondition.notify_one();
}

void CMsgQueueThread::clearMsg()
{
    std::lock_guard<std::mutex> lock(mLockQueue);
    m_v_msg.clear();
}

void CMsgQueueThread::stop() {
    mExitFlag = true;
    mGetMsgCondition.notify_one();
    if (mThread.joinable()) {
        mThread.join();
    }
}

int CMsgQueueThread::startMsgQueue()
{
    std::thread threadObj([this](){ this->threadLoop(); });
    threadObj.detach(); // detach the thread
    return 0;
}

bool CMsgQueueThread::threadLoop()
{
    while (!mExitFlag) { //requietexit() or requietexitWait() not call
        std::unique_lock<std::mutex> lock(mLockQueue);
        mGetMsgCondition.wait(lock, [this](){ return !m_v_msg.empty(); });
        if (mExitFlag) {
            break;
        }
        auto now = getNowMs();
        while (!m_v_msg.empty() && m_v_msg.front().mWhenMs <= now) {
            auto msg = std::move(m_v_msg.front());
            m_v_msg.erase(m_v_msg.begin());
            lock.unlock();
            handleMessage(msg);
            lock.lock();
        }
    }
    return false;
}
