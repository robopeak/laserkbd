/*
 *  Laser Keyboard
 *  Utilities
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include <string.h>

#include "common.h"

std::string trim( const char * src, char ch)
{
    int startpos = 0;
    int endpos = strlen(src) -1;

    while( startpos<=endpos && src[startpos] == ch) ++startpos;
    if ( startpos>endpos) return "";

    while( endpos>=startpos && src[endpos] == ch) --endpos;

    return std::string( src+startpos, endpos-startpos+1);
}

int split( const char * src, std::vector<std::string> & dest, char ch)
{
    int item_count = 0;
    int last_offset = 0,current_pos, src_len;
   
    dest.clear();

    src_len = strlen( src);
    if (src_len == 0) return 0;
   
    current_pos = 0;
    do
    {   
        if ( src[current_pos] == ch || current_pos==src_len)
        {
            if ( current_pos == last_offset) //null string
            {
                //dest.push_back( "" );
            }else
            {
                std::string ans = trim(std::string( src+last_offset, current_pos - last_offset).c_str());
                if (ans.size())
                    dest.push_back(ans);
            }
            item_count++;
            last_offset = current_pos+1;
        }
    }
    while( (++current_pos)<=src_len);


    return dest.size();
}
