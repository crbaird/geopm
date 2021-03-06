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

#ifndef AGENT_HPP_INCLUDE
#define AGENT_HPP_INCLUDE

#include <string>
#include <map>
#include <vector>

#include "PluginFactory.hpp"
#include "PlatformIO.hpp"

namespace geopm
{
    class Agent
    {
        public:
            Agent() = default;
            virtual ~Agent() = default;
            /// @brief Set the level where this Agent is active and push
            ///        signals/controls for that level.
            /// @param [in] level Level of the tree where this agent
            ///        is active.  Note that only agents at level zero
            ///        execute sample_platform() and adjust_platform().
            /// @param [in] fan_in Vector over level giving the the
            ///        number of Agents that report to each root Agent
            ///        operating at the level.
            /// @param [in] is_level_root True if the agent plays the
            ///        role of root of the level.  Only root agents
            ///        for a level execute ascend() and descend().
            virtual void init(int level, const std::vector<int> &fan_in, bool is_level_root) = 0;
            /// @brief Called by Kontroller to split policy for
            ///        children at next level down the tree.
            /// @param [in] in_policy Policy values from the parent.
            /// @param [out] out_policy Vector of policies to be sent
            ///        to each child.
            virtual bool descend(const std::vector<double> &in_policy,
                                 std::vector<std::vector<double> >&out_policy) = 0;
            /// @brief Aggregate signals from children for the next
            ///        level up the tree.
            /// @param [in] in_signal Vector of signal vectors, one
            ///        from each child.
            /// @param [out] out_signal Aggregated signal values to be
            ///        sent up to the parent.
            virtual bool ascend(const std::vector<std::vector<double> > &in_signal,
                                std::vector<double> &out_signal) = 0;
            /// @brief Adjust the platform settings based the policy
            ///        from above.
            /// @param [in] policy Settings for each control in the
            ///        policy.
            /// @return True if platform was adjusted, false otherwise.
            virtual bool adjust_platform(const std::vector<double> &policy) = 0;
            /// @brief Sample signals from the platform to be sent up
            ///        the tree.
            /// @param [out] sample Vector of samples, one per signal.
            virtual bool sample_platform(std::vector<double> &sample) = 0;
            /// @brief Called by Kontroller to wait for sample period
            ///        to elapse.  This controls the cadence of the
            ///        Kontroller main loop.
            virtual void wait(void) = 0;
            /// @brief Custom fields that will be added to the report
            ///        header when this agent is used.
            virtual std::vector<std::pair<std::string, std::string> > report_header(void) const = 0;
            /// @brief Custom fields for the node section of the report.
            virtual std::vector<std::pair<std::string, std::string> > report_node(void) const = 0;
            /// @brief Custom fields for each region in the report.
            virtual std::map<uint64_t, std::vector<std::pair<std::string, std::string> > > report_region(void) const = 0;
            /// @brief Column headers to be added to the trace.
            virtual std::vector<std::string> trace_names(void) const = 0;
            /// @brief Called by Kontroller to get latest values to be
            ///        added to the trace.
            virtual void trace_values(std::vector<double> &values) = 0;
            /// @brief Used to look up the number of values in the
            ///        policy vector sent down the tree for a specific
            ///        Agent.  This should be called with the
            ///        dictionary returned by
            ///        agent_factory().dictionary(agent_name) for the
            ///        Agent of interest.
            static int num_policy(const std::map<std::string, std::string> &dictionary);
            /// @brief Used to look up the number of values in the
            ///        sample vector sent up the tree for a specific
            ///        Agent.  This should be called with the
            ///        dictionary returned by
            ///        agent_factory().dictionary(agent_name) for the
            ///        Agent of interest.
            static int num_sample(const std::map<std::string, std::string> &dictionary);
            /// @brief Used to look up the names of values in the
            ///        policy vector sent down the tree for a specific
            ///        Agent.  This should be called with the
            ///        dictionary returned by
            ///        agent_factory().dictionary(agent_name) for the
            ///        Agent of interest.
            static std::vector<std::string> policy_names(const std::map<std::string, std::string> &dictionary);
            /// @brief Used to look up the names of values in the
            ///        sample vector sent up the tree for a specific
            ///        Agent.  This should be called with the
            ///        dictionary returned by
            ///        agent_factory().dictionary(agent_name) for the
            ///        Agent of interest.
            static std::vector<std::string> sample_names(const std::map<std::string, std::string> &dictionary);
            /// @brief Used to create a correctly-formatted dictionary
            ///        for an Agent at the time the Agent is
            ///        registered with the factory.  Concrete Agent
            ///        classes may provide policy_names() and
            ///        sample_names() methods to provide the vectors
            ///        to be passed to this method.
            static std::map<std::string, std::string> make_dictionary(const std::vector<std::string> &policy_names,
                                                                      const std::vector<std::string> &sample_names);
        private:
            static const std::string m_num_sample_string;
            static const std::string m_num_policy_string;
            static const std::string m_sample_prefix;
            static const std::string m_policy_prefix;
    };

    PluginFactory<Agent> &agent_factory(void);
}

#endif
