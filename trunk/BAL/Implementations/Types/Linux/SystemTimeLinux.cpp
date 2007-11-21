/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com 
 * Copyright (C) 2007 Pleyo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "DeprecatedString.h"
#include "PlatformString.h"
#include "BTLogHelper.h"
#include "SystemTime.h"
#include <time.h>
#include <sys/time.h>


namespace WebCore
{

/**
 * Return the current system time in seconds, using the classic POSIX epoch of January 1, 1970.
 * Like time(0) from <time.h>, except with a wider range of values and higher precision.
 */
double currentTime()
{
  struct timeval aTimeval;
  
  gettimeofday( &aTimeval, 0 );
  return (double)aTimeval.tv_sec + (double)(aTimeval.tv_usec / 1000000.0);
}

#ifdef __OWB__
double timeFromString(String dateString)
{
    double day, daysTilFirstOfMonth = 0, hours, year, minutes, seconds;
    String month, cutDate, tmpLeft, tmpRight;
    double cookieTime = 0;
    
    /*
     * There are several ways to format the date and each web site uses a different one :-/
     * This fonction tries to analyse the string to obtain the proper format.
     * When this is done, we extract the values and convert them into a number of seconds since 1/1/1970 0h00m00s
     */
    
    tmpRight = dateString.right(3);
    if (tmpRight.contains("GMT", false))
        cutDate = dateString.right(24);
    else
        cutDate = dateString.right(20);
    
    // we get the numeric values directly
    
    tmpLeft = cutDate.left(2);
    if (tmpLeft.contains(",",false)) {
        day = cutDate.substring(2,2).toDouble();
        month = month = cutDate.substring(5,3);
        year = cutDate.substring(9,2).toDouble();
        year = year + 2000;
    }
    else {
        day = cutDate.substring(0,2).toDouble();
        month = cutDate.substring(3,3);
        year = cutDate.substring(7,4).toDouble();
    }
    
    // GMT time
    hours = cutDate.substring(12,2).toDouble();
    if (dateString.endsWith("GMT",false));
        hours = hours + 2.0; //FIXME : this would only work in france
        
    minutes = cutDate.substring(15,4).toDouble();
    seconds = cutDate.substring(18,4).toDouble();
    
    /* 
     * We need to handle the month
     * This is the number of days elapsed from the begining of the year to first of the dateString month
     */
    if (month == "Jan")
        daysTilFirstOfMonth = 0;
    else if (month == "Feb")
        daysTilFirstOfMonth = 31;
    else if (month == "Mar")
        daysTilFirstOfMonth = 59;
    else if (month == "Apr")
        daysTilFirstOfMonth = 90;
    else if (month == "May")
        daysTilFirstOfMonth = 120;
    else if (month == "Jun")
        daysTilFirstOfMonth = 151;
    else if (month == "Jul")
        daysTilFirstOfMonth = 181;
    else if (month == "Aug")
        daysTilFirstOfMonth = 212;
    else if (month == "Sep")
        daysTilFirstOfMonth = 242;
    else if (month == "Oct")
        daysTilFirstOfMonth = 273;
    else if (month == "Nov")
        daysTilFirstOfMonth = 303;
    else if (month == "Dec")
        daysTilFirstOfMonth = 334;
    
    // handle the bissextil years
    if (year/4==int(year/4) && daysTilFirstOfMonth > 31) 
        daysTilFirstOfMonth++;
    
    DBGML(MODULE_FACILITIES, LEVEL_INFO, "Original : %s\nDATE FROM STRING\nDay = %lf\nMonth = %lf\nYear = %lf\nHours = %lf\nMin = %lf\nSec = %lf\n ",dateString.deprecatedString().ascii(), day, daysTilFirstOfMonth, year, hours, minutes, seconds);
    
    // now we can calculate the number of seconds since the January 1970 the 1st at 00:00:00
    cookieTime = (year - 1970)*(60*60*24*365);
    cookieTime += daysTilFirstOfMonth * (60*60*24);
    cookieTime += ((day - 1)+ int((year-1970)/4)) * (60*60*24);
    cookieTime += hours * (60*60);
    cookieTime += minutes * 60;
    cookieTime += seconds;
    
    return cookieTime;
}
#endif

/**
 * Return the number of seconds since a user event has been generated
 */
float userIdleTime()
{
    return 0;
}
}
