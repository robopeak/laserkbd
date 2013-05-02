/*
 *  Laser Keyboard
 *  Client Update Checker
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once



class UpdateChecker 
{
public:
    static UpdateChecker * GetInstance();
    
    virtual ~UpdateChecker(){};

    virtual void sendQueryRequest( const char * vendortype, const char * productString,  const char * versionString) = 0;

    virtual bool getResponseUrl(std::string & buffer) = 0;

    virtual bool isResponseReady() { return _isResponseReady; }

protected:
    volatile bool _isResponseReady;

protected:
    UpdateChecker() : _isResponseReady(false) {};

};