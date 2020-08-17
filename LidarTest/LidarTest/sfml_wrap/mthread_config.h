#pragma once
#ifndef _MTHREAD_CONFIG_H_
#define _MTHREAD_CONFIG_H_


#include <mutex>
#include "..\helpers\spinlock.h"

//typedef SpinLock LockType;
using LockType  = std::recursive_mutex;
using LockGuard = std::lock_guard<LockType>;

using RendererLockType = recursive_spinlock;

#endif //_MTHREAD_CONFIG_H_
