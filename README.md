# ComputerNetworksProject01
Computer Networks Project 1 Repository

Group members:
- Ian Havenaar
- Simran Moolchandaney
- Jacob Schwartz

Assigned TAs:
- Alamin Mohammed
- Luke Siela

Due date: Friday, February 4th @ 10 PM

## Usage
- make (ran in ComputerNetworksProject01 directory): creates ./client/proj1client and ./server/proj1server executables
- ./project1client IP/hostname port file (ran under client subdirectory)
- ./proj1server port (ran under server subdirectory)
- make clean: removes executables 
        
  port has to be within 41000-41999 range and has to be the same for both commands

## Performance Metrics
<img width="888" alt="Screen Shot 2022-02-03 at 9 06 13 PM" src="https://user-images.githubusercontent.com/66736756/152460545-1dce8e4f-8738-4ede-9be6-e6853562af9f.png">
We can observe that for both small and large files, the avergae total transfer time (ms) is greater for Local Host hence the average speed (MB/s) is also greater.

## Extra Credit
- [X] Support the usage of a host name in addition to an IP address for the first argument of the client
- [X] Allow connections to arrive only from the 129.74.* address range, localhost (127.*), or the private network (192.168.*)
  

