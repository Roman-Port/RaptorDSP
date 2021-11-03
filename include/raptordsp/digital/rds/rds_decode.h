#ifndef ANDROIDSDRTUNER_RDS_DECODE_H
#define ANDROIDSDRTUNER_RDS_DECODE_H

#include <cstdint>

typedef void (*raptor_rds_decode_callback)(uint64_t frame, void* ctx);

class raptor_rds_decode {

public:
    raptor_rds_decode();
    void process(const unsigned char* in, int count, raptor_rds_decode_callback callback, void* callbackCtx);
    bool is_synced();

private:
    void enter_no_sync();
    void enter_sync(unsigned int);
    unsigned int calc_syndrome(unsigned long, unsigned char);

    unsigned long  bit_counter;
    unsigned long  lastseen_offset_counter, reg;
    unsigned int   block_bit_counter;
    unsigned int   wrong_blocks_counter;
    unsigned int   blocks_counter;
    unsigned int   group_good_blocks_counter;
    unsigned int   group[4];
    unsigned char  offset_chars[4];  // [ABCcDEx] (x=error)
    bool           log;
    bool           debug;
    bool           presync;
    bool           good_block;
    bool           group_assembly_started;
    unsigned char  lastseen_offset;
    unsigned char  block_number;
    enum { NO_SYNC, SYNC } d_state;

};

#endif //ANDROIDSDRTUNER_RDS_DECODE_H
