#pragma once
#include <memory>
#include <functional>
#include "common/myTypeDef.h"
#include "utils/myBaseHelper.h"
#include "event/myEvent.h"

namespace MY_EVENT {

/**
 * 媒体流请求事件监听类
 */
class MyMediaReqEvListener : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    typedef std::function<void(MY_MEDIA_REQ_EV_ARGS)> MyMediaReqEvCb;
    typedef std::shared_ptr<MyMediaReqEvCb>           MyMediaReqEvCbPtr;

public:
    /**
     * @brief                       添加事件监听
     * @return                      添加结果
     * @param cb                    事件回调函数
     */
    static MY_COMMON::MyStatus_t    AddListener(const MyMediaReqEvCb& cb);

    /**
     * @brief                       删除事件监听
     * @return                      删除结果
     */
    static MY_COMMON::MyStatus_t    DelListener();

    /**
     * @brief                       事件通知
     * @return                      通知结果
     * @param args                  事件参数
     */
    static MY_COMMON::MyStatus_t    Notify(MY_MEDIA_REQ_EV_ARGS);

private:
    static MY_COMMON::MyEventTagPtr EvTagPtr;
    static MyMediaReqEvCbPtr        EvCbPtr;
};

}; // namespace MY_EVENT