#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "common/myDataDef.h"
#include "utils/myBaseHelper.h"

namespace MY_SERVER { class MySipServer; };

namespace MY_ENVIR {

/**
 * 服务管理类
 */
class MySystemServManage : public MY_UTILS::MyNonConstructableAndNonCopyable
{
public:
    typedef std::shared_ptr<MY_SERVER::MySipServer> SipServSmtPtr;
    typedef std::weak_ptr<MY_SERVER::MySipServer>   SipServSmtWkPtr;

public:
    /**
     * 服务管理对象类
     */
    class MySystemServManageObject;

public:         
    /**         
     * @brief                                       初始化
     * @return                                      MyStatus_t状态码
     */                                             

    static MY_COMMON::MyStatus_t                    Init();
    /**                                             
     * @brief                                       运行
     * @return                                      MyStatus_t状态码
     */                                             

    static MY_COMMON::MyStatus_t                    Run();

    /**                                             
     * @brief                                       关闭
     * @return                                      MyStatus_t状态码
     */                                             
    static MY_COMMON::MyStatus_t                    Shutdown();

public:
    /**
     * @brief                                       获取SipServer对象
     * @return                                      std::weak_ptr<MY_SERVER::MySipServer>
     * @param servId                                SipServer ID
     */
    static SipServSmtWkPtr                          GetSipServer(const std::string& servId);
};

}; // namespace MY_ENVIR








