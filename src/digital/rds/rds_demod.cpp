#include <raptordsp/digital/rds/rds_demod.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_rootraised.h>
#include <volk/volk.h>

#define RDS_SYMBOL_RATE 2375

void raptor_rds_demod::configure(float sampleRate) {
    //Set up rotator to 57 kHz
    float anglePerSec = 2.0f * M_PI * 57000 / sampleRate;
    rotatorInc.real(cos(anglePerSec));
    rotatorInc.imag(sin(anglePerSec));
    rotatorPhase = raptor_complex (1, 0);

    //Set up normal filter
    raptor_filter_builder_lowpass filterBuilder(sampleRate, 2800);
    filterBuilder.automatic_tap_count(300);
    fmFilter.configure(filterBuilder.build_taps_complex(), filterBuilder.calculate_decimation(&sampleRate));
    //__android_log_print(ANDROID_LOG_DEBUG, "NativeRaptorRdsDemod", "RDS sample rate %f, %d taps.", sampleRate, filterBuilder.ntaps);

    //Set up raised-root-cosine filter
    raptor_filter_builder_root_raised_cosine rootRaisedCosineFilterBuilder(sampleRate, RDS_SYMBOL_RATE, 1);
    rootRaisedCosineFilterBuilder.set_ntaps(100);
    rrcFilter.configure(rootRaisedCosineFilterBuilder.build_taps_complex(), 1);

    //Set up costas loop
    costasLoop.configure(sampleRate / RDS_SYMBOL_RATE, 2, false);

    //Configure clock recovery
    clockRecovery.configure(sampleRate / RDS_SYMBOL_RATE, 0.001f, 0.5f, 0.05f, 0.005f);

    //Reset slicer and differential decoder
    sliceIndex = 0;
    diffLastSample = 0;
}

int binary_slice_decimate(raptor_complex* ptrIn, unsigned char* ptrOut, int countIn, int* sliceIndex, int keep1InX) {
    int countOut = 0;
    for (int i = 0; i < countIn; i++) {
        //Skip
        if((*sliceIndex)++ != keep1InX)
            continue;
        (*sliceIndex) = 0;

        //Slice
        ptrOut[countOut] = ptrIn[i].real() >= 0 ? 1 : 0;
        countOut++;
    }
    return countOut;
}

int raptor_rds_demod::process(raptor_complex* ptr, unsigned char* output, int count) {
    //Rotate to center
    volk_32fc_s32fc_x2_rotator_32fc(ptr, ptr, rotatorInc, &rotatorPhase, count);

    //Filter to RDS
    count = fmFilter.process(ptr, ptr, count);
    //return count;

    //Apply raised-root-cosine filter
    count = rrcFilter.process(ptr, ptr, count);

    //Costas loop
    costasLoop.process(ptr, ptr, count);

    //Clock recovery MM
    count = clockRecovery.process(ptr, ptr, count);

    //Binary slice and also discard every other sample
    unsigned char* ptrBin = (unsigned char*)ptr;
    count = binary_slice_decimate(ptr, ptrBin, count, &sliceIndex, 1);

    //Apply differential decoding
    for (int i = 0; i < count; i++) {
        output[i] = (ptrBin[i] ^ diffLastSample) & 1;
        diffLastSample = ptrBin[i];
    }

    return count;
}
