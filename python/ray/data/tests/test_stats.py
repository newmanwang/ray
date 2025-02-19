import pytest
import re

import ray
from ray.tests.conftest import *  # noqa


def canonicalize(stats: str) -> str:
    # Time expressions.
    s1 = re.sub("[0-9\.]+(ms|us|s)", "T", stats)
    # Handle zero values specially so we can check for missing values.
    s2 = re.sub(" [0]+(\.[0]+)?", " Z", s1)
    # Other numerics.
    s3 = re.sub("[0-9]+(\.[0-9]+)?", "N", s2)
    return s3


def test_dataset_stats_basic(ray_start_regular_shared):
    ds = ray.data.range(1000, parallelism=10)
    ds = ds.map_batches(lambda x: x)
    ds = ds.map(lambda x: x)
    for batch in ds.iter_batches():
        pass
    stats = canonicalize(ds.stats())
    assert stats == """Stage Z read: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Stage N map_batches: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Stage N map: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T
"""


def test_dataset_pipeline_stats_basic(ray_start_regular_shared):
    ds = ray.data.range(1000, parallelism=10)
    ds = ds.map_batches(lambda x: x)
    pipe = ds.repeat(5)
    pipe = pipe.map(lambda x: x)
    for batch in pipe.iter_batches():
        pass
    stats = canonicalize(pipe.stats())
    assert stats == """== Pipeline Window N ==
Stage Z read: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Stage N map_batches: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Stage N map: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T

== Pipeline Window N ==
Stage Z read: [execution cached]
Stage N map_batches: [execution cached]
Stage N map: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T

== Pipeline Window N ==
Stage Z read: [execution cached]
Stage N map_batches: [execution cached]
Stage N map: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T

##### Overall Pipeline Time Breakdown #####
* Time stalled waiting for next dataset: T min, T max, T mean, T total
* Time in dataset iterator: T
* Time in user code: T
* Total time: T
"""


def test_dataset_pipeline_split_stats_basic(ray_start_regular_shared):
    ds = ray.data.range(1000, parallelism=10)
    pipe = ds.repeat(2)

    @ray.remote
    def consume(split):
        for batch in split.iter_batches():
            pass
        return split.stats()

    s0, s1 = pipe.split(2)
    stats = ray.get([consume.remote(s0), consume.remote(s1)])
    assert canonicalize(stats[0]) == """== Pipeline Window Z ==
Stage Z read: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T

== Pipeline Window N ==
Stage Z read: N/N blocks executed in T
* Remote wall time: T min, T max, T mean, T total
* Remote cpu time: T min, T max, T mean, T total
* Output num rows: N min, N max, N mean, N total
* Output size bytes: N min, N max, N mean, N total
* Tasks per node: N min, N max, N mean; N nodes used

Dataset iterator time breakdown:
* In ray.wait(): T
* In ray.get(): T
* In format_batch(): T
* In user code: T
* Total time: T

##### Overall Pipeline Time Breakdown #####
* Time stalled waiting for next dataset: T min, T max, T mean, T total
* Time in dataset iterator: T
* Time in user code: T
* Total time: T
"""


if __name__ == "__main__":
    import sys
    sys.exit(pytest.main(["-v", __file__]))
