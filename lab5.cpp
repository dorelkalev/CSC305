#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

void parentToChild(int write_fd) {
    const char* parentMessage = "Hello from Parent!";
    write(write_fd, parentMessage, strlen(parentMessage) + 1);
}

void childToParent(int write_fd, int start, int end) {
    int sum = 0;
    for (int i = start; i <= end; ++i) {
        sum += i;
    }
    write(write_fd, &sum, sizeof(sum));
}

void parentSumAndPrint(int read_fd, int start, int end) {
    int partialSum;
    read(read_fd, &partialSum, sizeof(partialSum));
    cout << "Child has computed the sum from " << start << " to " << end << ": " << partialSum << endl;
    int totalSum = partialSum;
    for (int i = 1; i < start; ++i) {
        totalSum += i;
    }
    cout << "The total sum from 1 to " << end << " is: " << totalSum << endl;
}

int main() {
    const int N = 20;
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1) {
        cerr << "Pipe 1 creation failed!" << endl;
        return 1;
    }
    if (pipe(pipe2) == -1) {
        cerr << "Pipe 2 creation failed!" << endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        char parentMessage[100];
        read(pipe1[0], parentMessage, sizeof(parentMessage));
        cout << "Child received message: " << parentMessage << endl;
        childToParent(pipe2[1], N / 2 + 1, N);
        close(pipe1[0]);
        close(pipe2[1]);

    } 
    else {
        close(pipe1[0]);
        close(pipe2[1]);
        parentToChild(pipe1[1]);
        int sum1 = 0;
        for (int i = 1; i <= N / 2; ++i) {
            sum1 += i;
        }
        cout << "Parent has computed the sum from 1 to " << N / 2 << ": " << sum1 << endl;
        parentSumAndPrint(pipe2[0], N / 2 + 1, N);
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }

    return 0;
}
