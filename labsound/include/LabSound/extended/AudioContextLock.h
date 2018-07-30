// License: BSD 2 Clause
// Copyright (C) 2015+, The LabSound Authors. All rights reserved.

#pragma once

//#define DEBUG_LOCKS

#ifndef AUDIO_CONTEXT_LOCK_H
#define AUDIO_CONTEXT_LOCK_H

#include "LabSound/core/AudioContext.h"
#include "LabSound/extended/Logging.h"

#include <iostream>
#include <mutex>

//#define DEBUG_LOCKS

namespace lab
{

    class ContextGraphLock
    {
        AudioContext * m_context;

    public:
        
        ContextGraphLock(AudioContext * context, const std::string & lockSuitor)
        {
            if (context)
            {
                context->m_graphLock.lock();
                m_context = context;
                m_context->m_graphLocker = lockSuitor;
            }
#if defined(DEBUG_LOCKS)
            if (!m_context && context->m_graphLocker.size())
            {
                LOG("%s failed to acquire [GRAPH] lock. Currently held by: %s.", lockSuitor.c_str(), context->m_graphLocker.c_str());
            }
#endif
        }
        
        ~ContextGraphLock()
        {
            if (m_context)
            {
                //m_context->m_graphLocker.clear();
                m_context->m_graphLock.unlock();
            }

        }

        AudioContext * context() { return m_context; }
    };

    class ContextRenderLock
    {
        AudioContext * m_context;
        std::string m_prevSuitor;

    public:
        static thread_local bool acquired;

        ContextRenderLock(AudioContext * context, const std::string & lockSuitor)
        {
            size_t threadId = std::hash<std::thread::id>()(std::this_thread::get_id());
            m_context = nullptr;

            if (ContextRenderLock::acquired) {
                printf("tid=%d ContextRenderLock acquired=%d already locked\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                return;
            }
            if (context)
            {
                m_prevSuitor = context->m_renderLocker;
                printf("tid=%d %s acquiring [RENDER] lock from %s.\n", threadId, lockSuitor.c_str(), m_prevSuitor.c_str()); fflush(stdout);
                printf("tid=%d ContextRenderLock acquired=%d calling renderLock.lock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                context->m_renderLock.lock();
                ContextRenderLock::acquired = true;
                printf("tid=%d ContextRenderLock acquired=%d called renderLock.lock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                printf("tid=%d %s acquired [RENDER] lock from %s.\n", threadId, lockSuitor.c_str(), m_prevSuitor.c_str()); fflush(stdout);
                m_context = context;
                m_context->m_renderLocker = lockSuitor;
            }
        }

        ~ContextRenderLock()
        {
            size_t threadId = std::hash<std::thread::id>()(std::this_thread::get_id());
            if (m_context)
            {
                std::string suitor = m_context->m_renderLocker;
                printf("tid=%d ContextRenderLock unlocking [RENDER] lock from %s.\n", threadId, suitor.c_str()); fflush(stdout); fflush(stdout);
                printf("tid=%d ContextRenderLock acquired=%d calling renderLock.unlock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                m_context->m_renderLocker.clear();
                m_context->m_renderLock.unlock();
                ContextRenderLock::acquired = false;
                printf("tid=%d ContextRenderLock acquired=%d called renderLock.unlock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                printf("tid=%d ContextRenderLock lockinged [RENDER] lock from %s.\n", threadId, suitor.c_str()); fflush(stdout); fflush(stdout);
            }
        }

        AudioContext * context() { return m_context; }
    };

    class ContextRenderUnlock
    {
        AudioContext * m_context;
        std::string m_prevSuitor;

    public:

        ContextRenderUnlock(AudioContext * context)
        {
          size_t threadId = std::hash<std::thread::id>()(std::this_thread::get_id());
          m_context = nullptr;
            if (!ContextRenderLock::acquired) {
                printf("tid=%d ContextRenderUnlock acquired=%d already unlocked\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                return;
            }
            if (context)
            {
                m_prevSuitor = context->m_renderLocker;
                printf("tid=%d ContextRenderUnlock unlocking [RENDER] lock from %s.\n", threadId, m_prevSuitor.c_str()); fflush(stdout); fflush(stdout);
                printf("tid=%d ContextRenderUnlock acquired=%d calling renderLock.unlock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                context->m_renderLocker.clear();
                context->m_renderLock.unlock();
                printf("tid=%d ContextRenderUnlock acquired=%d called renderLock.unlock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                ContextRenderLock::acquired = false;
                m_context = context;
                printf("tid=%d ContextRenderUnlock unlocked [RENDER] lock from %s.\n", threadId, m_prevSuitor.c_str()); fflush(stdout); fflush(stdout);
            }
        }

        ~ContextRenderUnlock()
        {
            size_t threadId = std::hash<std::thread::id>()(std::this_thread::get_id());
            if (m_context)
            {
                std::string suitor = m_prevSuitor;
                printf("tid=%d ContextRenderUnlock relocking [RENDER] lock to %s.\n", threadId, suitor.c_str()); fflush(stdout); fflush(stdout);
                printf("tid=%d ContextRenderUnlock acquired=%d calling renderLock.lock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                m_context->m_renderLock.lock();
                m_context->m_renderLocker = m_prevSuitor;
                printf("tid=%d ContextRenderUnlock acquired=%d called renderLock.lock()\n", threadId, ContextRenderLock::acquired); fflush(stdout);
                ContextRenderLock::acquired = true;
                printf("tid=%d ContextRenderUnlock relocked [RENDER] lock to %s.\n", threadId, suitor.c_str()); fflush(stdout); fflush(stdout);
            }
        }

        AudioContext * context() { return m_context; }
    };

} // end namespace lab

#endif

