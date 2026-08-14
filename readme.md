# Linux Container Resource Controller & PSI Monitor

A low-level C++ systems utility and monitoring service designed to enforce resource constraints on Linux containers using **cgroups v2** and monitor real-time resource pressure using Linux **Pressure Stall Information (PSI)** metrics.

---

## Technical Highlights

* **cgroups v2 Resource Controller:** Interface directly with `/sys/fs/cgroup/` using standard POSIX system calls (`open`, `write`) to apply hard limits on CPU quotas (`cpu.max`), memory limits (`memory.max`), and I/O bandwidth (`io.max`).


* **PSI Stall Monitor Daemon:** Low-overhead daemon that parses `/proc/pressure/cpu`, `/proc/pressure/memory`, and `/proc/pressure/io` in real time to capture stall metrics (`avg10`, `total`) and issue alert triggers during resource contention.


* **Reproducible Stress Workloads:** Docker Compose setup integrated with `stress-ng` to trigger throttling, out-of-memory (OOM) kills, and I/O latency spikes under controlled conditions.



---

## Repository Structure

```text
.
├── cgroup_controller.cpp  # POSIX C++ interface for cgroups v2 control
├── psi_monitor.cpp        # Real-time daemon parsing /proc/pressure metrics
├── docker-compose.yml     # Stress testing workloads (stress-ng)
└── README.md

```

---

## System Requirements

* **OS:** Linux Kernel 4.20+ (with unified `cgroups v2` hierarchy and `PSI` support enabled)
* **Compiler:** `g++` (C++17 standard or higher)
* **Container Runtime:** Docker Engine with Docker Compose v2

---

## Build Instructions

Compile the C++ source binaries using `g++`:

```bash
# Build cgroup controller binary
g++ -O2 -Wall cgroup_controller.cpp -o cgroup_controller

# Build PSI monitor daemon
g++ -O2 -Wall psi_monitor.cpp -o psi_monitor

```

---

## Usage & Demonstration

### 1. Enforcing Cgroup Limits

Execute the compiled controller with `sudo` permissions to apply resource restrictions to a specific cgroup:

```bash
sudo ./cgroup_controller <cgroup_name>

```

*Sets standard thresholds: CPU restricted to 50% single-core capacity (`50000 100000`) and RAM limited to 128 MB (`134217728` bytes).*

### 2. Running PSI Pressure Monitor

Start the monitoring daemon to poll `/proc/pressure/` file streams every 2 seconds:

```bash
./psi_monitor

```

Monitors rolling average stall durations (`avg10`) and outputs alert warnings whenever resource stall percentages exceed configured thresholds.

### 3. Simulating Container Stress Workloads

In a separate shell, launch the container workloads to verify limits and monitor output:

```bash
docker compose up

```

This triggers three distinct workloads:

* **`cpu_stress`:** Runs multi-threaded loops against single-core cgroup quotas to induce CPU throttling.


* **`memory_stress`:** Attempts to allocate 300 MB against a 128 MB container memory boundary to trigger OOM events.


* **`io_stress`:** Generates heavy sequential write operations to trigger I/O stall latency.
