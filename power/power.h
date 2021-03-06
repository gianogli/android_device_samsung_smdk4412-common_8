/*
 * Copyright (C) 2016 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define GOVERNOR_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"

#define GOV_PERFORMANCE      "performance"
#define GOV_ONDEMAND         "ondemand"
#define GOV_DYNAMIC         "dynamic"
#define GOV_POWERSAVE        "conservative"

#define MS_TO_NS (1000000L)

enum {
    PROFILE_POWER_SAVE = 0,
    PROFILE_BALANCED,
    PROFILE_PERFORMANCE,
    PROFILE_MAX
};

typedef struct governor_settings {
    // freq values for core up/down
    int hotplug_freq_1_1;
    int hotplug_freq_2_0;
    // rq sizes for up/down
    int hotplug_rq_1_1;
    int hotplug_rq_2_0;
    // max/min freqs (-1 for default)
    int max_freq;
    int min_freq;

    // frequency above which power usage increase is significantly bigger than speed increase
    int power_optimal_freq;
    // maximum frequency that you want to consider as normal, long time operating frequency.
    int max_non_oc_freq;
    // oc_freq_boost_ms is to prevent some long running CPU intensive tasks from using too much battery
    int oc_freq_boost_ms;
    // cpu load must be lower than up_threshold sampling_down_factor times in a row to lower the frequency
    int sampling_down_factor;
    // frequency above current policy->min down from which a standby counter starts countdown
    // (see wiki - https://github.com/mkaluza/project-zen/wiki/Dynamic-governor)
    int standby_threshold_freq;
    // load at which to start scaling up
    int up_threshold;
    // higher down_differential == slower downscaling
    int down_differential;
    // min/max num of cpus to have online
    int min_cpu_lock;
    // wait sampling_rate * cpu_up_rate us before trying to upscale
    int cpu_up_rate;
    // wait sampling_rate * cpu_down_rate us before trying to downscale
    int cpu_down_rate;
    int sampling_rate; // in microseconds
    int io_is_busy;
    // boosting
    int input_boost_freq;
    int boost_mincpus;
    long interaction_boost_time;
    long launch_boost_time;
} power_profile;

static power_profile profiles[PROFILE_MAX] = {
    [PROFILE_POWER_SAVE] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .max_freq = 600000,
        .min_freq = -1,
        .power_optimal_freq = 500000,
        .max_non_oc_freq = 500000,
        .oc_freq_boost_ms = 1000,
        .sampling_down_factor = 1,
        .standby_threshold_freq = 200000,
        .up_threshold = 90,
        .down_differential = 5,
        .min_cpu_lock = 2,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 30000,
        .io_is_busy = 0,
        .input_boost_freq = 200000,
        .boost_mincpus = 0,
        .interaction_boost_time = 0,
        .launch_boost_time = 0,
    },
    [PROFILE_BALANCED] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .min_freq = 200000,
        .max_freq = -1,
        .power_optimal_freq = 600000,
        .max_non_oc_freq = 900000,
        .oc_freq_boost_ms = 2000,
        .sampling_down_factor = 2,
        .standby_threshold_freq = 100000,
        .up_threshold = 85,
        .down_differential = 5,
        .min_cpu_lock = 2,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 20000,
        .io_is_busy = 1,
        .input_boost_freq = 200000,
        .boost_mincpus = 0,
        .interaction_boost_time = 120 * (MS_TO_NS),
        .launch_boost_time = 2000 * (MS_TO_NS),
    },
    [PROFILE_PERFORMANCE] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .min_freq = 1400000,
        .max_freq = -1,
        .power_optimal_freq = 1400000,
        .max_non_oc_freq = 1400000,
        .oc_freq_boost_ms = 0,
        .sampling_down_factor = 3,
        .standby_threshold_freq = 100000,
        .up_threshold = 80,
        .down_differential = 5,
        .min_cpu_lock = 4,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 20000,
        .io_is_busy = 1,
        .input_boost_freq = 800000,
        .boost_mincpus = 2,
        .interaction_boost_time = 180 * (MS_TO_NS),
        .launch_boost_time = 2000 * (MS_TO_NS),
    },
};

// for non-interactive profiles we don't need to worry about
// boosting as it (should) only occur while the screen is on
static power_profile profiles_low_power[PROFILE_MAX] = {
    [PROFILE_POWER_SAVE] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .max_freq = 400000,
        .min_freq = -1,
        .up_threshold = 95,
        .down_differential = 5,
        .min_cpu_lock = 0,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 20000,
        .io_is_busy = 0,
        .input_boost_freq = 200000,
        .boost_mincpus = 0,
        .interaction_boost_time = 0,
        .launch_boost_time = 0,
    },
    [PROFILE_BALANCED] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .min_freq = 100000,
        .max_freq = -1,
        .up_threshold = 95,
        .down_differential = 5,
        .min_cpu_lock = 0,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 20000,
        .io_is_busy = 1,
        .input_boost_freq = 200000,
        .boost_mincpus = 0,
        .interaction_boost_time = 0,
        .launch_boost_time = 0,
    },
    [PROFILE_PERFORMANCE] = {
        .hotplug_freq_1_1 = 400000,
        .hotplug_freq_2_0 = 300000,
        .hotplug_rq_1_1 = 300,
        .hotplug_rq_2_0 = 250,
        .min_freq = 1400000,
        .max_freq = -1,
        .up_threshold = 95,
        .down_differential = 5,
        .min_cpu_lock = 2,
        .cpu_up_rate = 1,
        .cpu_down_rate = 1,
        .sampling_rate = 20000,
        .io_is_busy = 1,
        .input_boost_freq = 200000,
        .boost_mincpus = 2,
        .interaction_boost_time = 0,
        .launch_boost_time = 0,
    },
};


