#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);                        // Constructor declaration
  int Pid();                               // Other member function declarations...
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
};

#endif
