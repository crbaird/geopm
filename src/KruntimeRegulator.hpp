/*
 * Copyright (c) 2015, 2016, 2017, 2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef KRUNTIMEREGULATOR_HPP_INCLUDE
#define KRUNTIMEREGULATOR_HPP_INCLUDE

#include <vector>
#include <string>

#include "geopm_time.h"
#include "geopm_message.h"

namespace geopm
{
    class IKruntimeRegulator
    {
        public:
            IKruntimeRegulator() = default;
            virtual ~IKruntimeRegulator() = default;
            /// @brief Called when the region is entered on a
            ///        particular rank.
            /// @param [in] rank The rank that entered the region.
            /// @param [in] entry_time The time the entry was
            ///        recorded.
            virtual void record_entry(int rank, struct geopm_time_s entry_time) = 0;
            /// @brief Called when the region is exited on a
            ///        particular rank.
            /// @param [in] rank The rank that entered the region.
            /// @param [in] entry_time The time the exit was
            ///        recorded.
            virtual void record_exit(int rank, struct geopm_time_s exit_time) = 0;
            /// @brief Returns the runtime measured for each rank the
            ///        last time it entered and exited the region.  If
            ///        a rank has not entered and exited the region,
            ///        the runtime will be 0.
            /// @return Last runtime for each rank.
            virtual std::vector<double> per_rank_last_runtime(void) const = 0;
            /// @brief Returns the total accumulated runtime for each
            ///        rank that has entered and exited the region at
            ///        least once.
            /// @return Total runtime for each rank.
            virtual std::vector<double> per_rank_total_runtime(void) const = 0;
            /// @brief Returns the number of times each rank has
            ///        entered and exited the region.
            /// @return Count of entries and exits for each rank.
            virtual std::vector<double> per_rank_count(void) const = 0;
        protected:
            static const struct geopm_time_s M_TIME_ZERO;
    };

    class KruntimeRegulator : public IKruntimeRegulator
    {
        public:
            KruntimeRegulator() = delete;
            KruntimeRegulator(const KruntimeRegulator &other) = default;
            KruntimeRegulator(int num_rank);
            virtual ~KruntimeRegulator() = default;
            void record_entry(int rank, struct geopm_time_s entry_time) override;
            void record_exit(int rank, struct geopm_time_s exit_time) override;
            std::vector<double> per_rank_last_runtime(void) const override;
            std::vector<double> per_rank_total_runtime(void) const override;
            std::vector<double> per_rank_count(void) const override;
        protected:
            enum m_num_rank_signal_e {
                M_NUM_RANK_SIGNAL = 2,
            };
            struct m_log_s {
                struct geopm_time_s enter_time;
                double last_runtime;
                double total_runtime;
                size_t count;
            };
            int m_num_rank;
            std::vector<struct m_log_s> m_rank_log;
    };
}

#endif
