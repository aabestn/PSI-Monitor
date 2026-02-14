#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const std::string CGROUP_BASE = "/sys/fs/cgroup/";

bool write_cgroup_file(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[-] Failed to open: " << path << std::endl;
        return false;
    }
    file << value;
    std::cout << "[+] Updated " << path << " -> " << value << std::endl;
    return true;
}

void apply_limits(const std::string& group_name, const std::string& cpu_max, const std::string& mem_max) {
    std::string target_dir = CGROUP_BASE + group_name;
    
    // Create cgroup directory
    mkdir(target_dir.c_str(), 0755);

    // Set CPU quota (e.g., 50000 100000 = 50% of 1 CPU core)
    if (!cpu_max.empty()) {
        write_cgroup_file(target_dir + "/cpu.max", cpu_max);
    }
    
    // Set Memory Max Limit (e.g., 134217728 = 128MB)
    if (!mem_max.empty()) {
        write_cgroup_file(target_dir + "/memory.max", mem_max);
    }
}

int main(int argc, char* argv[]) {
    if (geteuid() != 0) {
        std::cerr << "[-] Must be run as root to modify cgroups." << std::endl;
        return 1;
    }

    std::string cgroup_name = (argc > 1) ? argv[1] : "demo_container";
    std::cout << "[*] Setting cgroup limits for: " << cgroup_name << std::endl;

    apply_limits(cgroup_name, "50000 100000", "134217728");
    return 0;
}