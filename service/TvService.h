/*
 * Copyright (c) 2014 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description: header file
 */

#ifndef TV_SERVICE_H
#define TV_SERVICE_H

#include <binder/Binder.h>
#include <binder/Parcel.h>
#include "CTv.h"
#include "tvcmd.h"
#include <map>

using namespace android;

class TvService: public CTv::TvIObserver, public BBinder{
public:
    TvService();
    ~TvService();
    static TvService *GetInstance();

    enum command {
        CMD_START = IBinder::FIRST_CALL_TRANSACTION,
        CMD_TV_ACTION = IBinder::FIRST_CALL_TRANSACTION + 1,
        CMD_SET_TV_CB = IBinder::FIRST_CALL_TRANSACTION + 2,
        CMD_CLR_TV_CB = IBinder::FIRST_CALL_TRANSACTION + 3,
        EVT_SRC_CT_CB = IBinder::FIRST_CALL_TRANSACTION + 4,
        EVT_SIG_DT_CB = IBinder::FIRST_CALL_TRANSACTION + 5,
        DATA_SET_ACTION = IBinder::FIRST_CALL_TRANSACTION + 6,
        DATA_GET_ACTION = IBinder::FIRST_CALL_TRANSACTION + 7,
        CMD_DATA_REQ  = IBinder::FIRST_CALL_TRANSACTION + 8,
        EVT_SIG_DV_ALLM = IBinder::FIRST_CALL_TRANSACTION + 9,
        CMT_GET_ALLM_INFO = IBinder::FIRST_CALL_TRANSACTION + 10,
        EVT_SIG_VRR_CB = IBinder::FIRST_CALL_TRANSACTION + 11,

    };

private:
    virtual void onTvEvent(CTvEvent &event);
    int SendSignalForSignalDetectEvent(CTvEvent &event);
    int SendSignalForSourceConnectEvent(CTvEvent &event);
    int SendSignalForDvAllmEvent(CTvEvent &event);
    int SendSignalForVrrEvent(CTvEvent &event);
    int ParserTvCommand(const char *commandData);
    int ParserTvDataCommand(const char *commandBuf, unsigned char *dataBuf);
    int HandleDataRequest(const char* cmdtype, const char* cmd, tvcmd_e subcmd, tv_source_input_t source, char* data, size_t length);

    int SetTvServiceCallBack(sp<IBinder> callBack);
    int RemoveTvServiceCallBack(int callBackId);
    CTv *mpTv;
    std::map<int, sp<IBinder>> mTvServiceCallBack;
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data, Parcel* reply,
                                uint32_t flags = 0);
};
#endif
