#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<string>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<iterator>
#include<iomanip>

// Contents of a process 
struct Process{
    public:  
        int pid;
        int arr_time;
        int burst_time;
        int priority;
        int turnaround_time;
        int wait_time;
        int remaining_time;
        int start_time;
        int end_time;
};

// Function Declarations
void FCFS(std::vector<Process> tobesorted);
void SJFP(std::vector<Process> processes);
void Priority(std::vector<Process> processes);
void moveProcess(std::vector<Process> &from, std::vector<Process> &to, int index);

// Sorting algorithm used for FCFS
bool sortProcessByArrival (const Process &first, const Process &second){
    if(first.arr_time == second.arr_time){
        return first.pid < second.pid;
    }
    return first.arr_time < second.arr_time;
}

//Sorting algorithm used for SJFP
bool sortProcessByShortestJob (const Process &first, const Process &second){
    if(first.remaining_time == second.remaining_time){
        if(first.burst_time == second.burst_time){
            return first.pid < second.pid;
        }
        return first.burst_time < second.burst_time;
    }
    return first.remaining_time < second.remaining_time;
}
//Sorting algorithm used for Priority
bool sortProcessByPriority (const Process &first, const Process &second){
    if(first.priority == second.priority){
        if(first.arr_time == second.arr_time){

                return first.pid < second.pid;
        }
        return first.arr_time < second.arr_time;
    }
    return first.priority < second.priority;
}

using namespace std;
int main(int argc, char *argv[]){
    if(argc != 2){
        cout << "Incorrect Arguements" << '\n';
        return -1;
    }

    vector<Process> processes;
    string line;

    ifstream input_file(argv[1]);

    if(input_file.is_open()){
        while(getline(input_file, line)){
            istringstream stream(line);

            int index = 0;
            Process p;

            while(stream){
                string value;
                if(!getline(stream, value, ',')) break;
                switch (index){
                    case 0:
                        p.pid = stoi(value);
                        break;
                    case 1:
                        p.arr_time = stoi(value);
                        break;
                    case 2:
                        p.burst_time = stoi(value);
                        break;
                    case 3:
                        p.priority = stoi(value);
                        break;
                }
                index++;
            }
            p.turnaround_time = 0;
            p.wait_time = 0;
            p.start_time = -1;
            p.end_time = 0;
            p.remaining_time = p.burst_time;
            processes.push_back(p);
        }
    }

    FCFS(processes);
    SJFP(processes);
    Priority(processes);
    return 0;
}


void FCFS(vector<Process> tobesorted){
    cout << "--- FCFS ---"<<'\n';

    sort(tobesorted.begin(), tobesorted.end(),sortProcessByArrival);

    int currentTime = 0, totalturnaround_time = 0, totalwait_time = 0;
    int numProcesses = tobesorted.size();

    for(Process p: tobesorted){
        if(p.arr_time > currentTime){
            currentTime = p.arr_time;
        }
        p.start_time = currentTime;
        p.end_time = p.start_time + p.burst_time;
        p.turnaround_time = p.end_time - p.arr_time;
        p.wait_time = p.turnaround_time - p.burst_time;

        totalturnaround_time += p.turnaround_time;
        totalwait_time += p.wait_time;

        currentTime += p.burst_time;
    }

    float avgturntime = static_cast<float>(totalturnaround_time)/static_cast<float>(numProcesses);
    float avgwaittime = static_cast<float>(totalwait_time)/static_cast<float>(numProcesses);
    float throughput = static_cast<float>(numProcesses)/static_cast<float>(currentTime);

    cout << fixed << setprecision(3);
    cout << "Average Turnaround Time: " << avgturntime << '\n';
    cout << "Average Waiting Time: " << avgwaittime << '\n';
    cout << "Throughput: " << throughput << '\n';

}
void moveProcess(vector<Process> &from, vector<Process> &to, int index){
    to.push_back(from.at(index));
    from.erase(from.begin() + index);
}
void Priority(vector<Process> processes){
    cout << "--- Priority ---"<<'\n';
    sort(processes.begin(), processes.end(), sortProcessByArrival);

    vector<Process> ready;
    vector<Process> current;
    vector<Process> completed;

    int currentTime = 0;
    size_t totalturnaround_time = 0, totalwait_time = 0, numProcesses = processes.size();

    //while(!processes.empty() || !ready.empty()){
    while(completed.size() != numProcesses){
        for(int i = 0; i < static_cast<int>(processes.size()); i++){
            if(processes.at(i).arr_time <= currentTime){
                moveProcess(processes, ready, i);
            }
        }

        sort(ready.begin(), ready.end(), sortProcessByPriority);



        if(!ready.empty()){
            if(current.empty()){
                moveProcess(ready, current, 0);
            }else if(ready[0].priority < current[0].priority){
                moveProcess(current, ready, 0);
                moveProcess(ready, current, 0);
            }   
        }

        if(!current.empty()){

            current.front().remaining_time--;

            if(current.front().remaining_time == 0 ){

                current.front().end_time = currentTime+1;
                    
                current.front().turnaround_time = current.front().end_time - current.front().arr_time;
                current.front().wait_time = current.front().turnaround_time - current.front().burst_time;
                    
                moveProcess(current, completed, 0);
                //cout << "[Removing from ready] Ready size: " << ready.size() << "   Processes size: " << processes.size()<< '\n';               
            }
        }
        currentTime++;
    }
    
    for(Process p: completed){
        //cout << "Wait Time: " << p.wait_time << "   Turn around time: " << p.turnaround_time << '\n';

        totalturnaround_time += p.turnaround_time;
        totalwait_time += p.wait_time;
       
        //cout << " turntime:  " << static_cast<float>(totalturnaround_time)/static_cast<float>(numProcesses)<<"  Waittime:" <<static_cast<float>(totalwait_time)/static_cast<float>(numProcesses)<< '\n';
    }
    //cout << "Total time: " << currentTime << "   # of Processses: " << completed.size() << '\n';

    float avgturntime = static_cast<float>(totalturnaround_time)/static_cast<float>(numProcesses);
    float avgwaittime = static_cast<float>(totalwait_time)/static_cast<float>(numProcesses);
    float throughput = static_cast<float>(numProcesses)/static_cast<float>(currentTime);

    cout << fixed << setprecision(3);
    cout << "Average Turnaround Time: " << avgturntime << '\n';
    cout << "Average Waiting Time: " << avgwaittime << '\n';
    cout << "Throughput: " << throughput << '\n';
}

void SJFP(vector<Process> processes){
    cout << "--- SJFP ---"<<'\n';
    sort(processes.begin(), processes.end(), sortProcessByArrival);

    vector<Process> ready;
    vector<Process> current;
    vector<Process> completed;

    int currentTime = 0;
    size_t totalturnaround_time = 0, totalwait_time = 0, numProcesses = processes.size();

    //while(!processes.empty() || !ready.empty()){
    while(completed.size() != numProcesses){
        for(int i = 0; i < static_cast<int>(processes.size()); i++){
            if(processes.at(i).arr_time <= currentTime){
                moveProcess(processes, ready, i);
            }
        }

        sort(ready.begin(), ready.end(), sortProcessByShortestJob);



        if(!ready.empty()){
            if(current.empty()){
                moveProcess(ready, current, 0);
            }else if(ready[0].remaining_time < current[0].remaining_time){
                moveProcess(current, ready, 0);
                moveProcess(ready, current, 0);
            }   
        }

        if(!current.empty()){

            current.front().remaining_time--;

            if(current.front().remaining_time == 0 ){

                current.front().end_time = currentTime+1;
                    
                current.front().turnaround_time = current.front().end_time - current.front().arr_time;
                current.front().wait_time = current.front().turnaround_time - current.front().burst_time;
                    
                moveProcess(current, completed, 0);
                //cout << "[Removing from ready] Ready size: " << ready.size() << "   Processes size: " << processes.size()<< '\n';               
            }
        }
        currentTime++;
    }
    
    for(Process p: completed){
        //cout << "Wait Time: " << p.wait_time << "   Turn around time: " << p.turnaround_time << '\n';

        totalturnaround_time += p.turnaround_time;
        totalwait_time += p.wait_time;
       
        //cout << " turntime:  " << static_cast<float>(totalturnaround_time)/static_cast<float>(numProcesses)<<"  Waittime:" <<static_cast<float>(totalwait_time)/static_cast<float>(numProcesses)<< '\n';
    }
    //cout << "Total time: " << currentTime << "   # of Processses: " << completed.size() << '\n';

    float avgturntime = static_cast<float>(totalturnaround_time)/static_cast<float>(numProcesses);
    float avgwaittime = static_cast<float>(totalwait_time)/static_cast<float>(numProcesses);
    float throughput = static_cast<float>(numProcesses)/static_cast<float>(currentTime);

    cout << fixed << setprecision(3);
    cout << "Average Turnaround Time: " << avgturntime << '\n';
    cout << "Average Waiting Time: " << avgwaittime << '\n';
    cout << "Throughput: " << throughput << '\n';
}