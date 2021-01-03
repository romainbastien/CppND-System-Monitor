#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
    Processor();
    float Utilization();

 private:
    long total_;
    long active_;
    long idle_;

    void SetValues(long total, long active, long idle);
};

#endif