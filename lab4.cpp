#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>

using namespace std;

struct Process {
    int id;
    int priority;
    double arrivalTime;
    double burstTime;
    double startTime;
    double finishTime;
    double turnAroundTime;
    double remainingBurst;
    bool isFinished;
};

bool compareArrivalTime(const Process& a, const Process& b) {
    return a.arrivalTime < b.arrivalTime;
}

bool compareId(const Process& a, const Process& b) {
    return a.id < b.id;
}

bool comparePriority(const Process& a, const Process& b) {
    if (a.priority == b.priority) {
        return a.arrivalTime < b.arrivalTime;
    }
    return a.priority > b.priority;
}

void inputProcesses(Process proc[], int count) {
    for (int i = 0; i < count; i++) {
        cout << "Enter arrival time for Process " << i + 1 << ": ";
        cin >> proc[i].arrivalTime;
        cout << "Enter burst time for Process " << i + 1 << ": ";
        cin >> proc[i].burstTime;
        cout << "Enter priority for Process " << i + 1 << ": ";
        cin >> proc[i].priority;

        proc[i].id = i + 1;
        proc[i].remainingBurst = proc[i].burstTime;
        proc[i].isFinished = false;
        cout << endl;
    }
}

void displayResults(Process proc[], int count, const string& schedulingType) {
    double totalTurnAroundTime = 0;
    cout << schedulingType << " Scheduling Results\n";
    cout << "#ID\tST\tFT\tTAT\n";
    for (int i = 0; i < count; i++) {
        totalTurnAroundTime += proc[i].turnAroundTime;
        cout << proc[i].id << "\t" << proc[i].startTime << "\t" << proc[i].finishTime << "\t" << proc[i].turnAroundTime << endl;
    }
    cout << "Average Turnaround Time = " << totalTurnAroundTime / count << endl;
}

void FCFS(Process proc[], int count) {
    sort(proc, proc + count, compareArrivalTime);

    for (int i = 0; i < count; i++) {
        proc[i].startTime = (i == 0) ? proc[i].arrivalTime : max(proc[i - 1].finishTime, proc[i].arrivalTime);
        proc[i].finishTime = proc[i].startTime + proc[i].burstTime;
        proc[i].turnAroundTime = proc[i].finishTime - proc[i].arrivalTime;
    }

    sort(proc, proc + count, compareId);
    displayResults(proc, count, "First Come First Serve (FCFS)");
}

void ShortestJobFirst(Process proc[], int count) {
    double currentTime = 0;
    int completedCount = 0;
    vector<bool> isCompleted(count, false);
    vector<int> orderOfExecution;

    while (completedCount != count) {
        int selectedIdx = -1;
        double shortestBurstTime = 1e6;

        for (int i = 0; i < count; i++) {
            if (proc[i].arrivalTime <= currentTime && !isCompleted[i]) {
                if (proc[i].burstTime < shortestBurstTime) {
                    shortestBurstTime = proc[i].burstTime;
                    selectedIdx = i;
                }
            }
        }

        if (selectedIdx != -1) {
            proc[selectedIdx].startTime = currentTime;
            proc[selectedIdx].finishTime = proc[selectedIdx].startTime + proc[selectedIdx].burstTime;
            proc[selectedIdx].turnAroundTime = proc[selectedIdx].finishTime - proc[selectedIdx].arrivalTime;
            isCompleted[selectedIdx] = true;
            orderOfExecution.push_back(proc[selectedIdx].id);

            completedCount++;
            currentTime = proc[selectedIdx].finishTime;
        } else {
            currentTime++;
        }
    }

    displayResults(proc, count, "Shortest Job First (SJF)");
}

void PreemptivePriorityScheduling(Process proc[], int count) {
    double currentTime = 0;
    int completedCount = 0;
    vector<bool> isCompleted(count, false);
    vector<int> orderOfExecution;

    while (completedCount != count) {
        int selectedIdx = -1;
        double highestPriority = -1;

        for (int i = 0; i < count; i++) {
            if (proc[i].arrivalTime <= currentTime && !isCompleted[i]) {
                if (proc[i].priority > highestPriority) {
                    highestPriority = proc[i].priority;
                    selectedIdx = i;
                }
            }
        }

        if (selectedIdx != -1) {
            if (proc[selectedIdx].remainingBurst == proc[selectedIdx].burstTime) {
                proc[selectedIdx].startTime = currentTime;
            }

            proc[selectedIdx].remainingBurst -= 1;
            currentTime++;

            if (proc[selectedIdx].remainingBurst == 0) {
                proc[selectedIdx].finishTime = currentTime;
                proc[selectedIdx].turnAroundTime = proc[selectedIdx].finishTime - proc[selectedIdx].arrivalTime;
                isCompleted[selectedIdx] = true;
                orderOfExecution.push_back(proc[selectedIdx].id);

                completedCount++;
            }
        } else {
            currentTime++;
        }
    }

    displayResults(proc, count, "Preemptive Priority Scheduling");
}

void RoundRobin(Process proc[], int count, int timeQuantum) {
    double systemTime = 0;
    double localQuantum;
    int systemQt = 2;
    int completedJobs = 0;

    while (completedJobs < count) {
        for (int i = 0; i < count; i++) {
            if (proc[i].arrivalTime <= systemTime && !proc[i].isFinished) {
                localQuantum = timeQuantum + proc[i].priority / 2.0;

                if (proc[i].remainingBurst == proc[i].burstTime) {
                    proc[i].startTime = systemTime;
                }

                if (localQuantum > proc[i].remainingBurst) {
                    proc[i].finishTime = systemTime + proc[i].remainingBurst;
                    systemTime += proc[i].remainingBurst;
                    proc[i].turnAroundTime = proc[i].finishTime - proc[i].arrivalTime;
                    proc[i].isFinished = true;
                    completedJobs++;
                } else {
                    proc[i].finishTime = systemTime + localQuantum;
                    systemTime += localQuantum;
                    proc[i].remainingBurst -= localQuantum;
                }
            }
        }
        systemTime++;
    }

    displayResults(proc, count, "Round Robin Scheduling");
}

int main() {
    int numProcesses;
    cout << setprecision(2) << fixed;

    cout << "Enter the number of processes: ";
    cin >> numProcesses;

    Process processes[100];
    inputProcesses(processes, numProcesses);

    Process processesFCFS[100], processesSJF[100], processesPriority[100], processesRR[100];
    copy(processes, processes + numProcesses, processesFCFS);
    copy(processes, processes + numProcesses, processesSJF);
    copy(processes, processes + numProcesses, processesPriority);
    copy(processes, processes + numProcesses, processesRR);

    FCFS(processesFCFS, numProcesses);
    cout << endl;

    ShortestJobFirst(processesSJF, numProcesses);
    cout << endl;

    PreemptivePriorityScheduling(processesPriority, numProcesses);
    cout << endl;

    int timeQuantum;
    cout << "Enter time quantum for Round Robin: ";
    cin >> timeQuantum;

    RoundRobin(processesRR, numProcesses, timeQuantum);

    return 0;
}
