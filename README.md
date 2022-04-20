# Parallel and Distributed Programming

To run on cluster:

Seq:

`qrun 20c 1 pdp_serial serial_job.sh`

OpenMP:

`qrun 20c 1 pdp_serial serial_job.sh`

MPI:

`qrun 20c 3 pdp_long parallel_job.sh`
