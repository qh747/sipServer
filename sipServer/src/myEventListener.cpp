#include <Util/NoticeCenter.h>
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "event/myEventListener.h"
using namespace MY_COMMON;
using toolkit::NoticeCenter;

namespace MY_EVENT {

MyEventTagPtr                           MyMediaReqEvListener::EvTagPtr = nullptr;
MyMediaReqEvListener::MyMediaReqEvCbPtr MyMediaReqEvListener::EvCbPtr  = nullptr;

MyStatus_t MyMediaReqEvListener::AddListener(MyMediaReqEvCb cb)
{
    if (nullptr != EvCbPtr) {
        LOG(WARNING) << "MyMediaReqEvListener add listener failed, listener exists.";
        return MyStatus_t::FAILED;
    }

    EvCbPtr = std::make_shared<MyMediaReqEvCb>(cb);
    NoticeCenter::Instance().addListener(&EvTagPtr, MY_EVENT::kMyMediaRequestEvent, [](MY_MEDIA_REQ_EV_ARGS){
        MyMediaReqEvListener::Notify(deviceId, reqInfo, status, respInfo);
    });

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyMediaReqEvListener::DelListener()
{
    if (nullptr == EvCbPtr) {
        LOG(WARNING) << "MyMediaReqEvListener del listener failed, listener not exists.";
        return MyStatus_t::FAILED;
    }

    NoticeCenter::Instance().delListener(&EvTagPtr, MY_EVENT::kMyMediaRequestEvent);
    EvCbPtr.reset();

    return MyStatus_t::SUCCESS;
}

MyStatus_t MyMediaReqEvListener::Notify(MY_MEDIA_REQ_EV_ARGS)
{
    if (nullptr == EvCbPtr) {
        status = MyStatus_t::FAILED;
        respInfo = "media request event has no listener.";

        LOG(WARNING) << "MyMediaReqEvListener notify failed, listener not exists.";
        return MyStatus_t::FAILED;
    }

    (*EvCbPtr)(deviceId, reqInfo, status, respInfo);
    return (MyStatus_t::SUCCESS == status ? MyStatus_t::SUCCESS : MyStatus_t::FAILED);
}

}; // namespace MY_EVENT