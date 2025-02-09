# Operating Systems - Task Orchestration System 

## Description
The Task Orchestration System is a project developed for the **Operating Systems** course. The system provides a service for scheduling and executing user-submitted tasks efficiently. Users submit tasks with estimated execution times, and the system manages task execution, status tracking, and logging using efficient scheduling policies.

### 🎯 Purpose:
The primary objective of this project is to apply **Operating Systems** concepts, including:
- Process creation and management
- Inter-process communication using named pipes (FIFOs)
- Scheduling policies for task execution
- Logging and persistence of executed tasks
- System performance optimization

### 🚀 Key Features:
- **Task Submission**: Users submit tasks with an estimated execution time and program to run.
- **Scheduling Policies**: Tasks are scheduled based on a chosen policy (e.g., FCFS, SJF) to optimize execution time.
- **Task Status Tracking**: Users can check running, queued, and completed tasks.
- **Parallel Processing**: Multiple tasks can be executed simultaneously based on server configuration.
- **Output Logging**: Task outputs (stdout & stderr) are saved to files.
- **Inter-Process Communication**: Client-server communication via named pipes (FIFOs).
- **Performance Evaluation**: Implemented tests to analyze scheduling efficiency.

## 📚 Learning Outcomes
- **Process and Thread Management**: Gained experience with process creation and handling in Linux.
- **IPC Mechanisms**: Used named pipes for communication between processes.
- **Scheduling Algorithms**: Implemented different task scheduling approaches and evaluated their performance.
- **Performance Analysis**: Measured and optimized execution time and parallel processing efficiency.

## 🚧 Areas for Improvement
- **Process Management**: The implementation used slightly more forks than necessary, impacting efficiency.
- **Active Waiting**: Some areas of the system relied on active waiting, which could be optimized for better CPU utilization.

## 👨‍💻 Contributors
- **Alex Araújo Sá** - [Alex Sá](https://github.com/alexaraujosa)
- **Paulo Alexandre Rodrigues Ferreira** - [Paulo](https://github.com/pauloarf)
- **Rafael Santos Fernandes** - [DarkenLM](https://github.com/DarkenLM)

## 🛠️ Technologies Used
- **Programming Language**: C
- **Inter-Process Communication**: Named pipes (FIFOs)
- **Development Tools**: GCC, GDB, Valgrind
- **Performance Measurement**: gettimeofday() for execution time tracking
- **Data Handling**: File-based logging of completed tasks
