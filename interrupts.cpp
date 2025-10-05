#include "interrupts.hpp"

// This code was written in Visual Studios, and we forgot to push the code during the creation. 
// We have both equally participated in the creation of this code. 

int main(int argc, char** argv) {


   // Reads the vector and the decive  table
   auto [vectors, delays] = parse_args(argc, argv);


   // Opens teh trace files then stores one line and the excution String stroes the whole sim
   std::ifstream input_file(argv[1]);
   std::string trace;
   std::string execution;


   // My variables for the time
   int current_time = 0;       // current time in milli
   int context_save_time = 10; // time to save after the cpu has a interupt
   int isr_activity_time = 40; // time for the isr to excute


   // Reading each line from the trace file
   while (std::getline(input_file, trace)) {
       // Split the line into: "activity" = anevent (CPU, SYSCALL, END_IO) and "number" = CPU time


       auto [activity, number] = parse_trace(trace);


       if (activity == "CPU") {
           // CPU burst: just advance time by the duration
           execution += std::to_string(current_time) + ", " +
                        std::to_string(number) + ", CPU burst\n";
           current_time += number;
       }
       else if (activity == "SYSCALL") {
           // System call that then  triggers an interrupt


           // Step 1: Switch to kernel mode, save context, find ISR address
           auto [intr_log, new_time] = intr_boilerplate(current_time, number,context_save_time, vectors);
           execution += intr_log;
           current_time = new_time;


           // Step 2: Execute ISR for the device
           execution += std::to_string(current_time) + ", " + std::to_string(isr_activity_time) + ", execute ISR body for device " + std::to_string(number) + "\n";
           current_time += isr_activity_time;


           // Step 3: Return from interrupt
           execution += std::to_string(current_time) + ", 1, IRET\n";
           current_time++;
       }
       else if (activity == "END_IO") {
           // when the device is finished it causes an interrupt


           // Step 1: Same as before
           auto [intr_log, new_time] = intr_boilerplate(current_time, number,
                                                         context_save_time, vectors);
           execution += intr_log;
           current_time = new_time;


           // Step 2: again
           int device_delay = delays.at(number);
           execution += std::to_string(current_time) + ", " + std::to_string(device_delay) + ", end of I/O device " + std::to_string(number) + "\n";
           current_time += device_delay;


           // Step 3: Return from interrupt
           execution += std::to_string(current_time) + ", 1, IRET\n";
           current_time++;
       }
       else {
           // if the trace file is wrong
           std::cerr << "Unknown activity: " << activity << "\n";
       }
   }


   // Close trace file
   input_file.close();


   // Writes complete simulation log to executio
   write_output(execution);


   return 0;
}

