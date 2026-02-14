#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>

struct PSIMetrics {
    double avg10 = 0.0;
    double avg60 = 0.0;
    double avg300 = 0.0;
    unsigned long long total = 0;
};

PSIMetrics parse_psi_file(const std::string& filepath) {
    PSIMetrics metrics;
    std::ifstream file(filepath);
    std::string line;

    if (file.is_open() && std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        ss >> token; // Skip prefix "some" or "full"

        while (ss >> token) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = token.substr(0, eq_pos);
                double val = std::stod(token.substr(eq_pos + 1));
                
                if (key == "avg10") metrics.avg10 = val;
                else if (key == "avg60") metrics.avg60 = val;
                else if (key == "avg300") metrics.avg300 = val;
                else if (key == "total") metrics.total = static_cast<unsigned long long>(val);
            }
        }
    }
    return metrics;
}

void monitor_psi(double threshold_avg10) {
    const std::vector<std::pair<std::string, std::string>> resources = {
        {"CPU", "/proc/pressure/cpu"},
        {"Memory", "/proc/pressure/memory"},
        {"I/O", "/proc/pressure/io"}
    };

    std::cout << "[*] Starting C++ PSI Stall Monitor..." << std::endl;

    while (true) {
        for (const auto& [name, path] : resources) {
            PSIMetrics m = parse_psi_file(path);
            if (m.avg10 > threshold_avg10) {
                std::cout << "[ALERT] High " << name << " Stall! avg10=" << m.avg10 << "%" << std::endl;
            } else {
                std::cout << "[" << name << "] avg10: " << m.avg10 << "% | total: " << m.total << "us" << std::endl;
            }
        }
        sleep(2);
    }
}

int main() {
    monitor_psi(5.0);
    return 0;
}