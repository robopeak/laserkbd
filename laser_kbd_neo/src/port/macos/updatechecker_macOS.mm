/*
 *  Laser Keyboard
 *  Client Update Checker
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "port/common/updatechecker.h"
#include "port/common/productid.h"
#include <pthread.h>


#include "json/json.h"


@interface URLConnectionDelegate : NSObject
{
    BOOL isRequestReceived;
    NSMutableData * recvData;
    NSURLRequest *theRequest;
    NSURLConnection *theConnection;
}

- (BOOL)isRequestReceivced;

- (void)startConnection:(const char *)URLAddr;

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;

- (void)connection:(NSURLConnection *)connection
didFailWithError:(NSError *)error;

- (void)connectionDidFinishLoading:(NSURLConnection *)connection;

- (size_t)retrieveData:(char *)buffer withBufferSize:(size_t)size;

@end



@implementation URLConnectionDelegate

- (id)init {
    [super init];
    isRequestReceived = NO;
    recvData = [[NSMutableData data] retain];
    theRequest = nil;
    return self;
}


- (BOOL)isRequestReceivced {
    return isRequestReceived;
}

- (void)deadlloc {
    
    [recvData release];
    [super dealloc];
}


- (void)startConnection:(const char *)URLAddr {
    NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];
    theRequest = [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:URLAddr]]
                                  cachePolicy:NSURLRequestReloadIgnoringCacheData
                              timeoutInterval:60.0];


    theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
    [localpool drain];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    [recvData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    [recvData appendData:data];
}

- (void)connection:(NSURLConnection *)connection
didFailWithError:(NSError *)error {
    [connection release];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    isRequestReceived = YES;
    [connection release];
    

}

- (size_t)retrieveData:(char *)buffer withBufferSize:(size_t)size {
    if (isRequestReceived == YES) {
        size_t copysize = std::min((size_t)[recvData length], size);
        memcpy(buffer, [recvData mutableBytes], copysize);
        buffer[(copysize == size)?(copysize-1):copysize] = 0;
        return copysize;
    } else {
        return 0;
    }
}


@end

class UpdateChecker_MacOS : public UpdateChecker
{
public:

    UpdateChecker_MacOS()
        : UpdateChecker() 
        , is_working(false)
    {
        _urldelegate = [[URLConnectionDelegate alloc] init];
    }

    virtual ~UpdateChecker_MacOS() {

        is_working = false;
        [_urldelegate release];
    }

    virtual void sendQueryRequest( const char * vendortype, const char * productString,  const char * versionString){
        if (is_working) return ;

        if (!GetPlatformIdentification(_platofomr_id)) return ;
        _vendortype = vendortype;
        _productString = productString;
        _versionString = versionString;

        is_working = true;

        _working_thrd();

    }

    virtual bool getResponseUrl(std::string & buffer){
        if (!isResponseReady()) return false;
        if (![_urldelegate retrieveData:responseBuffer withBufferSize:sizeof(responseBuffer)]) {
            return false;
        }
        
        printf("Retrieved Buffer=%s\n", responseBuffer);
        // parse the response to json
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(std::string(responseBuffer),root)) {
            return false;
        }

        std::string result = root.get("result", "no_update_available").asString();
        if (result.compare("no_update_available")==0) {
            return false;
        }
        else {
            buffer = result;
            return true;
        }
    }

    virtual bool isResponseReady() {
        return [_urldelegate isRequestReceivced];
    }
protected:

    char responseBuffer[2048];
    size_t responsePos;


    void _working_thrd() {
        // compose the requst
        char query_urlbuffer[1024];
        std::string urlString;
        
        sprintf(query_urlbuffer, "?device=%s&model=%s&submodel=%02x%02x%02x%02x%02x%02x&platform=%s&software_version=%s&firmware_version=%s&format=json"
                , _productString.c_str(), _vendortype.c_str()
                , (int)_platofomr_id.id_buf[0]
                , (int)_platofomr_id.id_buf[1]
                , (int)_platofomr_id.id_buf[2]
                , (int)_platofomr_id.id_buf[3]
                , (int)_platofomr_id.id_buf[4]
                , (int)_platofomr_id.id_buf[5]
                , GetPlatformName()
                , _versionString.c_str()
                , "0.0");
        
        
        urlString += UPDATE_POST_URL;
        urlString += query_urlbuffer;
        
        [_urldelegate startConnection:urlString.c_str()];

    }

    bool      is_working;

    std::string _vendortype;
    std::string _productString;
    std::string _versionString;
    platform_id_t _platofomr_id;
    
    
    URLConnectionDelegate * _urldelegate;
};

static UpdateChecker_MacOS g_instance;

UpdateChecker * UpdateChecker::GetInstance()
{
    return &g_instance;
}
    
