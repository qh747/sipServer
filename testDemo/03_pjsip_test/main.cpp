#include <pjsua2.hpp>
#include <iostream>

// Subclass to extend the Account and get notifications etc.
class MyAccount : public pj::Account
{
public:
    void onRegState(pj::OnRegStateParam &prm) override {
        pj::AccountInfo info = this->getInfo();
        std::cout << (info.regIsActive? "*** Register:" : "*** Unregister:") << " code=" << prm.code << std::endl;
    }
};

int main()
{
    pj::Endpoint ep;

    // create library
    ep.libCreate();

    // Initialize endpoint
    pj::EpConfig ep_cfg;
    ep.libInit(ep_cfg);

    // Create SIP transport. Error handling sample is shown
    pj::TransportConfig tcfg;
    tcfg.port = 5060;

    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    } 
    catch (pj::Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library (worker threads etc)
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Configure an AccountConfig
    pj::AccountConfig acfg;
    acfg.idUri = "sip:test@sip.pjsip.org";
    acfg.regConfig.registrarUri = "sip:sip.pjsip.org";

    pj::AuthCredInfo cred("digest", "*", "test", 0, "secret");
    acfg.sipConfig.authCreds.push_back(cred);

    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(acfg);

    // Here we don't have anything else to do..
    pj_thread_sleep(10000);

    // Delete the account. This will unregister from server
    delete acc;

    // This will implicitly shutdown the library
    return 0;
}