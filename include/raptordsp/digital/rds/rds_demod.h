#ifndef ANDROIDSDRTUNER_RDS_DEMOD_H
#define ANDROIDSDRTUNER_RDS_DEMOD_H

#include <raptordsp/defines.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <raptordsp/analog/costas_loop.h>
#include <raptordsp/digital/clock_recovery_mm.h>

class raptor_rds_demod {

public:
    void configure(float sampleRate);
    int process(raptor_complex* ptr, unsigned char* output, int count);

private:
    raptor_complex rotatorPhase;
    raptor_complex rotatorInc;

    raptor_filter_complex fmFilter;
    raptor_filter_complex rrcFilter;
    raptor_costas_loop costasLoop;
    raptor_clock_recovery_mm clockRecovery;
    int sliceIndex;
    unsigned char diffLastSample;

};

#endif //ANDROIDSDRTUNER_RDS_DEMOD_H
