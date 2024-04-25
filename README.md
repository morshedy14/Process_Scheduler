# CPU Scheduler Simulation

This project simulates a CPU scheduler for a computer system with a single CPU and infinite memory. The scheduler determines the order of execution for scheduled processes and decides which process will run according to a specified scheduling algorithm. It plays a crucial role in an operating system, impacting memory and time usage.

## System Components
The project consists of four main components:

### Process Generator
The process generator is responsible for:
- Reading the input files and asking the user for the scheduling algorithm and its parameters.
- Initiating and creating the scheduler and clock processes.
- Creating a data structure for processes and providing it with its parameters.
- Sending information to the scheduler at the appropriate time (when a process arrives).
- Clearing IPC resources at the end.

### Clock
The clock component emulates an integer time clock for synchronization among the other components. This module is pre-built and acts as the timekeeper for the scheduler and process generator.

### Scheduler
The scheduler is the core component, responsible for tracking processes and their states (Running, Ready, Blocked). It decides which process will run based on the selected scheduling algorithm. This component has the following key functionalities:
- Implementation of three scheduling algorithms: Round Robin (RR), Short Job First (SJF), and Shortest Remaining Time Next (SRTN).
- Starting new processes (forking and assigning parameters).
- Switching between processes according to the scheduling algorithm.
- Maintaining a Process Control Block (PCB) for each process, tracking its state, execution time, remaining time, etc.
- Deleting data for completed processes when notified.
- Generating performance reports with metrics such as CPU utilization, average weighted turnaround time, and standard deviation for average weighted turnaround time.
- Creating two output files: `Scheduler.log` and `Scheduler.perf`.

### Process
The process component simulates a CPU-bound process that interacts with the scheduler. When it finishes, it notifies the scheduler on termination, allowing the scheduler to manage the process's lifecycle.

## Key Features
- Support for multiple scheduling algorithms (RR, SJF, SRTN).
- Comprehensive tracking of process states through PCBs.
- Ability to generate detailed performance reports.
- Synchronization through an integer time clock.

## Input/Output
- Input files: Describes processes and their parameters for simulation.
- Output files: `Scheduler.log` and `Scheduler.perf` containing detailed information on scheduler behavior and performance.

## Project Goals
This project aims to provide a comprehensive simulation of a CPU scheduler, demonstrating various scheduling algorithms and their impact on process management and system performance.
