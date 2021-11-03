#include <raptordsp/digital/rds/rds_decode.h>
#include "rds_constants.h"
#include <cstdint>

raptor_rds_decode::raptor_rds_decode() {
    bit_counter = 0;
    lastseen_offset_counter = 0;
    reg = 0;
    block_bit_counter = 0;
    wrong_blocks_counter = 0;
    blocks_counter = 0;
    group_good_blocks_counter = 0;
    log = false;
    debug = false;
    presync = false;
    good_block = false;
    group_assembly_started = false;
    lastseen_offset = false;
    block_number = false;
}

void raptor_rds_decode::enter_no_sync() {
    presync = false;
    d_state = NO_SYNC;
}

void raptor_rds_decode::enter_sync(unsigned int sync_block_number) {
    wrong_blocks_counter   = 0;
    blocks_counter         = 0;
    block_bit_counter      = 0;
    block_number           = (sync_block_number + 1) % 4;
    group_assembly_started = false;
    d_state                = SYNC;
}

/* see Annex B, page 64 of the standard */
unsigned int raptor_rds_decode::calc_syndrome(unsigned long message, unsigned char mlen) {
    unsigned long reg = 0;
    unsigned int i;
    const unsigned long poly = 0x5B9;
    const unsigned char plen = 10;

    for (i = mlen; i > 0; i--)  {
        reg = (reg << 1) | ((message >> (i-1)) & 0x01);
        if (reg & (1 << plen)) reg = reg ^ poly;
    }
    for (i = plen; i > 0; i--) {
        reg = reg << 1;
        if (reg & (1<<plen)) reg = reg ^ poly;
    }
    return (reg & ((1<<plen)-1));	// select the bottom plen bits of reg
}

/*static char* makebits(uint64_t data) {
    char* tmp = (char*)malloc(100);
    int o = 0;
    for(int i = 0; i < 64; i++) {
        if (i % 16 == 0 && i != 0)
            tmp[o++] = ' ';
        else if (i % 8 == 0 && i != 0)
            tmp[o++] = '-';
        tmp[o++] = ((data >> i) & 1) ? '1' : '0';
    }
    tmp[o] = 0;
    return tmp;
}

static void testlog(raptorrds_packet packet, int testNo) {
    uint64_t pi = packet.frame & 0xFFFF;
    unsigned int group = (packet.frame >> 16) & 15;
    char c1 = packet.read_integer(48, 8);//(char)((packet.frame >> 48) & 255);
    char c2 = packet.read_integer(56, 8);//(char)((packet.frame >> 56) & 255);
    __android_log_print(ANDROID_LOG_DEBUG, "RaptorRdsTest", "TEST %d: %llx - %s - [%llx ~ %x] - [%d ~ %d] - [%c%c ~ %c%c]", testNo, packet.frame, makebits(packet.frame), pi, packet.get_pi_code(), group, packet.get_group_type(), c1, c2, c2, c1);
}*/

static uint64_t decode_group(unsigned int *group) {
    uint64_t frame = 0;
    for(int i = 0; i<4; i++)
    {
        uint64_t inValue = (uint64_t)group[i] & 0xFFFF;
        uint64_t outValue = 0;
        int bitIn = 0;
        int bitOut = 15;
        for (int j = 0; j < 16; j++)
            outValue |= ((inValue >> bitIn++) & 1) << bitOut--;
        frame |= outValue << (16 * i);
        //frame |= (ulong)group[i] << (16 * i);
    }
    return frame;
}

void raptor_rds_decode::process (const unsigned char* in, int count, raptor_rds_decode_callback callback, void* callbackCtx)
{
    int i=0,j;
    unsigned long bit_distance, block_distance;
    unsigned int block_calculated_crc, block_received_crc, checkword,dataword;
    unsigned int reg_syndrome;
    unsigned char offset_char('x');  // x = error while decoding the word offset

/* the synchronization process is described in Annex C, page 66 of the standard */
    while (i < count) {
        reg=(reg<<1)|in[i];		// reg contains the last 26 rds bits
        switch (d_state) {
            case NO_SYNC:
                reg_syndrome = calc_syndrome(reg,26);
                for (j=0;j<5;j++) {
                    if (reg_syndrome==syndrome[j]) {
                        if (!presync) {
                            lastseen_offset=j;
                            lastseen_offset_counter=bit_counter;
                            presync=true;
                        }
                        else {
                            bit_distance=bit_counter-lastseen_offset_counter;
                            if (offset_pos[lastseen_offset]>=offset_pos[j])
                                block_distance=offset_pos[j]+4-offset_pos[lastseen_offset];
                            else
                                block_distance=offset_pos[j]-offset_pos[lastseen_offset];
                            if ((block_distance*26)!=bit_distance) presync=false;
                            else {
                                //lout << "@@@@@ Sync State Detected" << std::endl;
                                enter_sync(j);
                            }
                        }
                        break; //syndrome found, no more cycles
                    }
                }
                break;
            case SYNC:
/* wait until 26 bits enter the buffer */
                if (block_bit_counter<25) block_bit_counter++;
                else {
                    good_block=false;
                    dataword=(reg>>10) & 0xffff;
                    block_calculated_crc=calc_syndrome(dataword,16);
                    checkword=reg & 0x3ff;
/* manage special case of C or C' offset word */
                    if (block_number==2) {
                        block_received_crc=checkword^offset_word[block_number];
                        if (block_received_crc==block_calculated_crc) {
                            good_block=true;
                            offset_char = 'C';
                        } else {
                            block_received_crc=checkword^offset_word[4];
                            if (block_received_crc==block_calculated_crc) {
                                good_block=true;
                                offset_char = 'c';  // C' (C-Tag)
                            } else {
                                wrong_blocks_counter++;
                                good_block=false;
                            }
                        }
                    }
                    else {
                        block_received_crc=checkword^offset_word[block_number];
                        if (block_received_crc==block_calculated_crc) {
                            good_block=true;
                            if (block_number==0) offset_char = 'A';
                            else if (block_number==1) offset_char = 'B';
                            else if (block_number==3) offset_char = 'D';
                        } else {
                            wrong_blocks_counter++;
                            good_block=false;
                        }
                    }
/* done checking CRC */
                    if (block_number==0 && good_block) {
                        group_assembly_started=true;
                        group_good_blocks_counter=1;
                    }
                    if (group_assembly_started) {
                        if (!good_block) group_assembly_started=false;
                        else {
                            group[block_number]=dataword;
                            offset_chars[block_number] = offset_char;
                            group_good_blocks_counter++;
                        }
                        if (group_good_blocks_counter==5) {
                            uint64_t frame = decode_group(group);
                            callback(frame, callbackCtx);
                        }
                    }
                    block_bit_counter=0;
                    block_number=(block_number+1) % 4;
                    blocks_counter++;
/* 1187.5 bps / 104 bits = 11.4 groups/sec, or 45.7 blocks/sec */
                    if (blocks_counter==50) {
                        if (wrong_blocks_counter>35) {
                            /*lout << "@@@@@ Lost Sync (Got " << wrong_blocks_counter
                                 << " bad blocks on " << blocks_counter
                                 << " total)" << std::endl;*/
                            enter_no_sync();
                        } else {
                            /*lout << "@@@@@ Still Sync-ed (Got " << wrong_blocks_counter
                                 << " bad blocks on " << blocks_counter
                                 << " total)" << std::endl;*/
                        }
                        blocks_counter=0;
                        wrong_blocks_counter=0;
                    }
                }
                break;
            default:
                d_state=NO_SYNC;
                break;
        }
        i++;
        bit_counter++;
    }
}

bool raptor_rds_decode::is_synced() {
    return d_state == SYNC;
}