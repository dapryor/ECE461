#ifndef PROFILE_H
#define PROFILE_H

#define RET_ADX_OFFSET (0x18)
#define CUR_FRAME_SIZE (8)  // 0 if var is initialized as first auto var 

#define DISABLE_PROFILING { profiling_enabled--;}
#define ENABLE_PROFILING { profiling_enabled++;}

extern volatile char profiling_enabled;

extern void Init_Profiling(void);

extern void Disable_Profiling(void);
extern void Enable_Profiling(void);
extern void Sort_Profile(void);

extern void Print_Results(void);

#endif
