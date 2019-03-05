/*
 * Central Processing Unit (CPU)
 *
 * Copyright (C) 2019-2020 Udo Steinberg, BedRock Systems, Inc.
 *
 * This file is part of the NOVA microhypervisor.
 *
 * NOVA is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOVA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 */

#pragma once

#include "compiler.hpp"
#include "memory.hpp"
#include "spinlock.hpp"
#include "types.hpp"

class Cpu
{
    public:
        static unsigned id              CPULOCAL;
        static unsigned hazard          CPULOCAL;
        static bool     bsp             CPULOCAL;
        static uint32   affinity        CPULOCAL;
        static unsigned boot_cpu;
        static unsigned online;
        static Spinlock boot_lock       asm ("__boot_lock");

        ALWAYS_INLINE
        static inline auto remote_affinity (unsigned cpu)
        {
            return __atomic_load_n (reinterpret_cast<decltype (affinity) *>(reinterpret_cast<uintptr_t>(&affinity) - CPU_LOCAL_DATA + CPU_GLOBL_DATA + cpu * PAGE_SIZE), __ATOMIC_RELAXED);
        }

        static inline void halt()
        {
            asm volatile ("wfi; msr daifclr, #0xf; msr daifset, #0xf" : : : "memory");
        }

        static inline void preempt_disable()
        {
            asm volatile ("msr daifset, #0xf" : : : "memory");
        }

        static inline void preempt_enable()
        {
            asm volatile ("msr daifclr, #0xf" : : : "memory");
        }

        static void init (unsigned, unsigned);
};
