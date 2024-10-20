#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

class Job {
public:
    string processId;
    int size;
    int status; // 0 = Waiting, 1 = Running
    string partID; // ID of the partition assigned

    Job(string id, int s) : processId(id), size(s), status(0), partID("") {}
};

class Partition {
public:
    string partId;
    int size;
    bool isAvailable;
    string jobId;

    Partition(string id, int s) : partId(id), size(s), isAvailable(true), jobId("") {}
};

void printJobAllocations(const vector<Job>& jobs, const vector<Partition>& partitions) {
    cout << left << setw(10) << "Job ID" 
         << left << setw(15) << "Partition ID" 
         << left << setw(10) << "Waste" 
         << left << setw(10) << "Status" << endl;
    
    int totalWaste = 0;

    for (const auto& job : jobs) {
        string partitionId = job.partID.empty() ? "None" : job.partID;
        int waste = 0;

        if (job.status == 1) { // If the job is running
            for (const auto& partition : partitions) {
                if (partition.jobId == job.processId) {
                    waste = partition.size - job.size; // Calculate waste
                    totalWaste += waste;
                    break;
                }
            }
        }

        cout << left << setw(10) << job.processId 
             << left << setw(15) << partitionId 
             << left << setw(10) << waste 
             << left << setw(10) << (job.status == 1 ? "Run" : "Wait") << endl;
    }
    
    cout << "Total Waste: " << totalWaste << endl << endl;
}

void firstFit(vector<Job>& jobs, vector<Partition>& partitions) {
    for (auto& job : jobs) {
        for (auto& partition : partitions) {
            if (job.size <= partition.size && partition.isAvailable) {
                partition.isAvailable = false;
                partition.jobId = job.processId;
                job.partID = partition.partId;
                job.status = 1; // Running
                break; // Move to next job
            }
        }
    }
}

void nextFit(vector<Job>& jobs, vector<Partition>& partitions) {
    static int lastIndex = 0;
    for (auto& job : jobs) {
        int startIndex = lastIndex;
        do {
            if (job.size <= partitions[lastIndex].size && partitions[lastIndex].isAvailable) {
                partitions[lastIndex].isAvailable = false;
                partitions[lastIndex].jobId = job.processId;
                job.partID = partitions[lastIndex].partId;
                job.status = 1; // Running
                lastIndex = (lastIndex + 1) % partitions.size(); // Move to next partition
                break; // Move to next job
            }
            lastIndex = (lastIndex + 1) % partitions.size();
        } while (lastIndex != startIndex);
    }
}

void bestFit(vector<Job>& jobs, vector<Partition>& partitions) {
    for (auto& job : jobs) {
        int bestIndex = -1;
        int bestWaste = numeric_limits<int>::max();
        
        for (int j = 0; j < partitions.size(); j++) {
            if (job.size <= partitions[j].size && partitions[j].isAvailable) {
                int waste = partitions[j].size - job.size;
                if (waste < bestWaste) {
                    bestWaste = waste;
                    bestIndex = j;
                }
            }
        }
        
        if (bestIndex != -1) {
            partitions[bestIndex].isAvailable = false;
            partitions[bestIndex].jobId = job.processId;
            job.partID = partitions[bestIndex].partId;
            job.status = 1; // Running
        }
    }
}

void worstFit(vector<Job>& jobs, vector<Partition>& partitions) {
    for (auto& job : jobs) {
        int worstIndex = -1;
        int worstWaste = -1;

        for (int j = 0; j < partitions.size(); j++) {
            if (job.size <= partitions[j].size && partitions[j].isAvailable) {
                int waste = partitions[j].size - job.size;
                if (waste > worstWaste) {
                    worstWaste = waste;
                    worstIndex = j;
                }
            }
        }
        
        if (worstIndex != -1) {
            partitions[worstIndex].isAvailable = false;
            partitions[worstIndex].jobId = job.processId;
            job.partID = partitions[worstIndex].partId;
            job.status = 1; // Running
        }
    }
}

int main() {
    int numPartitions, numJobs;
    
    cout << "Enter number of partitions: ";
    cin >> numPartitions;
    cout << "Enter size of each partition:\n";
    
    vector<Partition> partitions;
    for (int i = 0; i < numPartitions; i++) {
        int size;
        cout << "Partition " << i + 1 << ": ";
        cin >> size;
        partitions.emplace_back(to_string(i + 1), size);
    }

    cout << "Enter number of jobs: ";
    cin >> numJobs;

    vector<Job> jobs;
    for (int i = 0; i < numJobs; i++) {
        int size;
        cout << "Process " << i + 1 << " size: ";
        cin >> size;
        jobs.emplace_back("P" + to_string(i + 1), size);
    }

    // Apply each algorithm
    cout << "\n=== First-Fit ===\n";
    firstFit(jobs, partitions);
    printJobAllocations(jobs, partitions);

    // Reset partitions and jobs for next algorithm
    for (auto& partition : partitions) {
        partition.isAvailable = true;
        partition.jobId = "";
    }
    for (auto& job : jobs) {
        job.status = 0;
        job.partID = "";
    }

    cout << "\n=== Next-Fit ===\n";
    nextFit(jobs, partitions);
    printJobAllocations(jobs, partitions);

    // Reset partitions and jobs for next algorithm
    for (auto& partition : partitions) {
        partition.isAvailable = true;
        partition.jobId = "";
    }
    for (auto& job : jobs) {
        job.status = 0;
        job.partID = "";
    }

    cout << "\n=== Best-Fit ===\n";
    bestFit(jobs, partitions);
    printJobAllocations(jobs, partitions);

    // Reset partitions and jobs for next algorithm
    for (auto& partition : partitions) {
        partition.isAvailable = true;
        partition.jobId = "";
    }
    for (auto& job : jobs) {
        job.status = 0;
        job.partID = "";
    }

    cout << "\n=== Worst-Fit ===\n";
    worstFit(jobs, partitions);
    printJobAllocations(jobs, partitions);

    return 0;
}
