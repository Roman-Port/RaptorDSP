#pragma once

enum win_type {
    WIN_NONE = -1,       //!< don't use a window
    WIN_HAMMING = 0,     //!< Hamming window; max attenuation 53 dB
    WIN_HANN = 1,        //!< Hann window; max attenuation 44 dB
    WIN_HANNING = 1,     //!< alias to WIN_HANN
    WIN_BLACKMAN = 2,    //!< Blackman window; max attenuation 74 dB
    WIN_RECTANGULAR = 3, //!< Basic rectangular window; max attenuation 21 dB
    WIN_KAISER = 4, //!< Kaiser window; max attenuation see max_attenuation
    WIN_BLACKMAN_hARRIS = 5, //!< Blackman-harris window; max attenuation 92 dB
    WIN_BLACKMAN_HARRIS =
    5,            //!< alias to WIN_BLACKMAN_hARRIS for capitalization consistency
    WIN_BARTLETT = 6, //!< Barlett (triangular) window; max attenuation 26 dB
    WIN_FLATTOP = 7,  //!< flat top window; useful in FFTs; max attenuation 93 dB
    WIN_NUTTALL = 8,  //!< Nuttall window; max attenuation 114 dB
    WIN_BLACKMAN_NUTTALL = 8, //!< Nuttall window; max attenuation 114 dB
    WIN_NUTTALL_CFD =
    9, //!< Nuttall continuous-first-derivative window; max attenuation 112 dB
    WIN_WELCH = 10,  //!< Welch window; max attenuation 31 dB
    WIN_PARZEN = 11, //!< Parzen window; max attenuation 56 dB
    WIN_EXPONENTIAL =
    12, //!< Exponential window; max attenuation see max_attenuation
    WIN_RIEMANN = 13, //!< Riemann window; max attenuation 39 dB
    WIN_GAUSSIAN =
    14,         //!< Gaussian window; max attenuation see max_attenuation
    WIN_TUKEY = 15, //!< Tukey window; max attenuation see max_attenuation
};

float* raptor_window_build(win_type type, int ntaps, double param, const bool normalize);