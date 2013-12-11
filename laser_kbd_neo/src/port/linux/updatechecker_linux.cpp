/*                                                                              
 * Copyright (C) 2013 Deepin, Inc.                                                 
 *               2013 Leslie Zhai                                                  
 *                                                                              
 * Author:     Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 * Maintainer: Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 *                                                                              
 * This program is free software: you can redistribute it and/or modify         
 * it under the terms of the GNU General Public License as published by         
 * the Free Software Foundation, either version 3 of the License, or            
 * any later version.                                                           
 *                                                                              
 * This program is distributed in the hope that it will be useful,              
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 * GNU General Public License for more details.                                 
 *                                                                              
 * You should have received a copy of the GNU General Public License            
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.        
 */

#include <pthread.h>                                                            
#include <curl/curl.h>                                                          
#include <json/json.h>                                                          
#include <string.h>

#include "common.h"
#include "port/common/updatechecker.h"
#include "port/common/productid.h"

class UpdateChecker_Linux : public UpdateChecker
{
public:

    UpdateChecker_Linux()
        : UpdateChecker() 
        , working_thrd(NULL)
        , is_working(false)
    {
    }

    virtual ~UpdateChecker_Linux() 
    {   
        if (working_thrd) 
        { 
            pthread_cancel(working_thrd);
        }

        is_working = false;
    }

    virtual void sendQueryRequest(const char* vendortype, 
                                  const char* productString, 
                                  const char* versionString) 
    {
        if (is_working) 
            return ;

        if (!GetPlatformIdentification(_platofomr_id)) 
            return ;

        _vendortype = vendortype;
        _productString = productString;
        _versionString = versionString;

        is_working = true;

        pthread_create(&working_thrd, NULL, s_working_poc, this);
    }

    virtual bool getResponseUrl(std::string & buffer) 
    {
        if (!_isResponseReady) 
            return false;

        // parse the response to json
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(std::string(responseBuffer),root)) 
            return false;

        std::string result = root.get("Result", "no_update_available").asString();
        if (result.compare("no_update_available") == 0) 
            return false;
        else 
        {
            buffer = result;
            return true;
        }
    }


protected:
    char responseBuffer[2048];
    size_t responsePos;

    static size_t curl_on_write_data(void* buffer, 
                                     size_t size, 
                                     size_t nmemb, 
                                     void* userp)
    {
        UpdateChecker_Linux* This = reinterpret_cast<UpdateChecker_Linux*>(userp);

        int remaining_cnt = sizeof(This->responseBuffer) - This->responsePos;
        if (remaining_cnt < 0) 
            return 0;

        size_t actualCopySize = std::min((size_t)remaining_cnt, size * nmemb);
        if (actualCopySize == 0) 
            return 0;

        memcpy(This->responseBuffer+This->responsePos, buffer, actualCopySize);
        This->responsePos += actualCopySize;

        return actualCopySize;
    }

    void _working_thrd() 
    {
        // compose the requst
        curl_global_init(CURL_GLOBAL_ALL);
        do 
        {
            CURL* easyhandle = curl_easy_init();

            if (!easyhandle) 
                break;
            
            char query_urlbuffer[1024] = {0};
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

            curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(easyhandle, CURLOPT_URL, urlString.c_str());
            curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, &curl_on_write_data);
            curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
            curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, 1);

            responsePos = 0;

            CURLcode result = curl_easy_perform(easyhandle);

            if (responsePos && result == CURLE_OK) 
            {
                responseBuffer[responsePos] = 0;
                _isResponseReady = true;
            }

            curl_easy_cleanup(easyhandle);

        } while(0);
    final:
        is_working = false;
    }

    static void* s_working_poc(void * data) 
    {
        UpdateChecker_Linux* This = reinterpret_cast<UpdateChecker_Linux*>(data);
        This->_working_thrd();

        return NULL;
    }

    pthread_t working_thrd;
    bool      is_working;
    std::string _vendortype;
    std::string _productString;
    std::string _versionString;
    platform_id_t _platofomr_id;

};

static UpdateChecker_Linux g_instance;

UpdateChecker * UpdateChecker::GetInstance()
{
    return &g_instance;
}
