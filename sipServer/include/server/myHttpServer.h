#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "common/myTypeDef.h"
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace MY_SERVER {

/**
 * http服务类
 */
class MyHttpServer : public std::enable_shared_from_this<MyHttpServer>
{
public:
    typedef MyHttpServer*                    Ptr;
    typedef std::shared_ptr<MyHttpServer>    SmtPtr;
    typedef std::weak_ptr<MyHttpServer>      SmtWkPtr;
    typedef std::shared_ptr<tcp::acceptor>   AcceptSmtPtr;

public:         
    MyHttpServer();          
    ~MyHttpServer();   
    
public:
    /**
     * @brief                               http服务事件线程回调函数
     * @return                              回调结果，0-success，-1-failed
     * @param arg                           回调参数
     */
    static MY_COMMON::MyStatus_t            ServThdFunc(MY_COMMON::MyFuncCbParamPtr arg);

public:         
    /**         
     * @brief                               初始化http服务
     * @return                              初始化结果，0-success，-1-failed
     * @param addrCfg                       http服务地址配置
     */             
    MY_COMMON::MyStatus_t                   init(const MY_COMMON::MyHttpServAddrCfg_dt& addrCfg);

    /**
     * @brief                               启动http服务
     * @return                              启动结果，0-success，-1-failed
     */             
    MY_COMMON::MyStatus_t                   run();

    /**     
     * @brief                               关闭http服务
     * @return                              初始化结果，0-success，-1-failed
     */         
    MY_COMMON::MyStatus_t                   shutdown();

public:
    /**
     * @brief                               处理http get请求
     * @return                              处理结果，0-success，-1-failed
     * @param url                           请求url
     * @param statusCode                    http响应状态码
     * @param respBody                      http响应信息
     */
    MY_COMMON::MyStatus_t                   onRecvHttpGetReq(const std::string& url, http::status& statusCode, std::string& respBody);

    /**
     * @brief                               处理http post请求
     * @return                              处理结果，0-success，-1-failed
     * @param url                           请求url
     * @param reqBody                       http请求信息
     * @param statusCode                    http响应状态码
     * @param respBody                      http响应信息
     */
    MY_COMMON::MyStatus_t                   onRecvHttpPostReq(const std::string& url, const std::string& reqBody,
                                                              http::status& statusCode, std::string& respBody);

public:     
    /**     
     * @brief                               http服务状态
     * @return                              获取结果，0-success，-1-failed
     * @param status                        http服务状态
     */         
    MY_COMMON::MyStatus_t                   getState(MY_COMMON::MyStatus_t& status) const;

    /**
     * @brief                               获取http服务地址配置
     * @return                              获取结果，0-success，-1-failed
     * @param cfg                           http服务地址配置
     */
    MY_COMMON::MyStatus_t                   getServAddrCfg(MY_COMMON::MyHttpServAddrCfg_dt& cfg);

private:
    //                                      启动状态 
    MY_COMMON::MyAtomicStatus               m_status; 
    
    //                                      http服务多线程互斥量
    boost::shared_mutex                     m_rwMutex;

    //                                      http服务地址配置
    MY_COMMON::MyHttpServAddrCfg_dt         m_servAddrCfg;

    AcceptSmtPtr                            m_acceptPtr;
};

}; // namespace MY_SERVER