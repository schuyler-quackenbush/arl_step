#ifndef __sysFiles_H
#define __sysFiles_H

#include "defineValues.h"

/* information about component files of signal */
typedef struct {
    AUDIO_SAMP *samples[MAX_CHN];
} SysFiles;

#endif /* __sysFiles_H */
