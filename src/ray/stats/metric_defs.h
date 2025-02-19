// Copyright 2017 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ray/stats/metric.h"

namespace ray {

namespace stats {

/// The definitions of metrics that you can use everywhere.
///
/// There are 4 types of metric:
///   Histogram: Histogram distribution of metric points.
///   Gauge: Keeps the last recorded value, drops everything before.
///   Count: The count of the number of metric points.
///   Sum: A sum up of the metric points.
///
/// You can follow these examples to define your metrics.

/// NOTE: When adding a new metric, add the metric name to the _METRICS list in
/// python/ray/tests/test_metrics_agent.py to ensure that its existence is tested.

/// Event stats
DECLARE_stats(operation_count);
DECLARE_stats(operation_run_time_ms);
DECLARE_stats(operation_queue_time_ms);
DECLARE_stats(operation_active_count);

/// GRPC server
DECLARE_stats(grpc_server_req_process_time_ms);
DECLARE_stats(grpc_server_req_new);
DECLARE_stats(grpc_server_req_handling);
DECLARE_stats(grpc_server_req_finished);

/// GCS Resource Manager
DECLARE_stats(new_resource_creation_latency_ms);

/// Placement Group
DECLARE_stats(placement_group_creation_latency_ms);
DECLARE_stats(placement_group_scheduling_latency_ms);
DECLARE_stats(pending_placement_group);
DECLARE_stats(registered_placement_group);
DECLARE_stats(infeasible_placement_group);

/// The below items are legacy implementation of metrics.
/// TODO(sang): Use DEFINE_stats instead.

///
/// Common
///
/// RPC
static Histogram GcsLatency("gcs_latency",
                            "The latency of a GCS (by default Redis) operation.", "us",
                            {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000},
                            {CustomKey});

///
/// Raylet Metrics
///

/// Raylet Resource Manager
static Gauge LocalAvailableResource("local_available_resource",
                                    "The available resources on this node.", "",
                                    {ResourceNameKey});

static Gauge LocalTotalResource("local_total_resource",
                                "The total resources on this node.", "",
                                {ResourceNameKey});

/// Object Manager.
static Gauge ObjectStoreAvailableMemory(
    "object_store_available_memory",
    "Amount of memory currently available in the object store.", "bytes");

static Gauge ObjectStoreUsedMemory(
    "object_store_used_memory",
    "Amount of memory currently occupied in the object store.", "bytes");

static Gauge ObjectStoreFallbackMemory(
    "object_store_fallback_memory",
    "Amount of memory in fallback allocations in the filesystem.", "bytes");

static Gauge ObjectStoreLocalObjects("object_store_num_local_objects",
                                     "Number of objects currently in the object store.",
                                     "objects");

static Gauge ObjectManagerPullRequests("object_manager_num_pull_requests",
                                       "Number of active pull requests for objects.",
                                       "requests");

/// Object Directory.
static Gauge ObjectDirectoryLocationSubscriptions(
    "object_directory_subscriptions",
    "Number of object location subscriptions. If this is high, the raylet is attempting "
    "to pull a lot of objects.",
    "subscriptions");

static Gauge ObjectDirectoryLocationUpdates(
    "object_directory_updates",
    "Number of object location updates per second., If this is high, the raylet is "
    "attempting to pull a lot of objects and/or the locations for objects are frequently "
    "changing (e.g. due to many object copies or evictions).",
    "updates");

static Gauge ObjectDirectoryLocationLookups(
    "object_directory_lookups",
    "Number of object location lookups per second. If this is high, the raylet is "
    "waiting on a lot of objects.",
    "lookups");

static Gauge ObjectDirectoryAddedLocations(
    "object_directory_added_locations",
    "Number of object locations added per second., If this is high, a lot of objects "
    "have been added on this node.",
    "additions");

static Gauge ObjectDirectoryRemovedLocations(
    "object_directory_removed_locations",
    "Number of object locations removed per second. If this is high, a lot of objects "
    "have been removed from this node.",
    "removals");

/// Node Manager
static Histogram HeartbeatReportMs(
    "heartbeat_report_ms",
    "Heartbeat report time in raylet. If this value is high, that means there's a high "
    "system load. It is possible that this node will be killed because of missing "
    "heartbeats.",
    "ms", {100, 200, 400, 800, 1600, 3200, 6400, 15000, 30000});

/// Worker Pool
static Histogram ProcessStartupTimeMs("process_startup_time_ms",
                                      "Time to start up a worker process.", "ms",
                                      {1, 10, 100, 1000, 10000});

static Sum NumWorkersStarted(
    "internal_num_processes_started",
    "The total number of worker processes the worker pool has created.", "processes");

/// Scheduler
static Sum NumReceivedTasks(
    "internal_num_received_tasks",
    "The cumulative number of lease requeusts that this raylet has received.", "tasks");

static Sum NumDispatchedTasks(
    "internal_num_dispatched_tasks",
    "The cumulative number of lease requeusts that this raylet has granted.", "tasks");

static Sum NumSpilledTasks("internal_num_spilled_tasks",
                           "The cumulative number of lease requeusts that this raylet "
                           "has spilled to other raylets.",
                           "tasks");

static Gauge NumInfeasibleTasks(
    "internal_num_infeasible_tasks",
    "The number of tasks in the scheduler that are in the 'infeasible' state.", "tasks");

static Gauge NumInfeasibleSchedulingClasses(
    "internal_num_infeasible_scheduling_classes",
    "The number of unique scheduling classes that are infeasible.", "tasks");

/// Local Object Manager (Spilling)
static Gauge SpillingBandwidthMB("object_spilling_bandwidth_mb",
                                 "Bandwidth of object spilling.", "MB");

static Gauge RestoringBandwidthMB("object_restoration_bandwidth_mb",
                                  "Bandwidth of object restoration.", "MB");

///
/// GCS Server Metrics
///

/// Workers
static Count UnintentionalWorkerFailures(
    "unintentional_worker_failures_total",
    "Number of worker failures that are not intentional. For example, worker failures "
    "due to system related errors.",
    "");

/// Nodes
static Count NodeFailureTotal(
    "node_failure_total", "Number of node failures that have happened in the cluster.",
    "");

/// Actors
static Gauge PendingActors("pending_actors", "Number of pending actors in GCS server.",
                           "actors");

/// Resources
static Histogram OutboundHeartbeatSizeKB("outbound_heartbeat_size_kb",
                                         "Outbound heartbeat payload size", "kb",
                                         {10, 50, 100, 1000, 10000, 100000});

static Histogram GcsUpdateResourceUsageTime(
    "gcs_update_resource_usage_time", "The average RTT of a UpdateResourceUsage RPC.",
    "ms", {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000}, {CustomKey});

/// Testing
static Gauge LiveActors("live_actors", "Number of live actors.", "actors");
static Gauge RestartingActors("restarting_actors", "Number of restarting actors.",
                              "actors");

}  // namespace stats

}  // namespace ray
