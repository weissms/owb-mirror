/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file BIResourceHandleManager.h
 *
 * Interface of Transfer Job Manager
 */
#ifndef BIResourceHandleManager_H_
#define BIResourceHandleManager_H_

#include "ResourceHandleClient.h"

namespace BAL
{

class BIResourceHandle;

/**
* @brief the BIResourceHandleManager
*
* The transfer job manager. It manages transfer jobs. 
* Note that TransferJobManager is not the owner of the
* TransferJobs. Client remains the owner.
*
* @see BIResourceHandle, BIResourceHandleClient
*/

class BIResourceHandleManager
{
public:
     virtual ~BIResourceHandleManager() {};

    /**
     * add a transfer job. Transfer starts immediatly.
     * When transfer is completed (normal or error case), it is
     * removed from the manager's job list.
     **/
    virtual void add(BIResourceHandle *job) = 0;

    /**
     * Checks to see if this transfer job is in the active list 
     **/
    virtual bool contains(BIResourceHandle *job) = 0;

    /**
     * cancel a transfer job. It means job is stopped,
     * and removed from the manager's job list.
     **/
    virtual void cancel(BIResourceHandle *job) = 0;
};

}

#endif /* BIResourceHandleManager_H_ */
