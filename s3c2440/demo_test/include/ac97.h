#ifndef __ac97_H__
#define __ac97_H__

#ifdef __cplusplus
extern "C" 
{
#endif

void AC97_Test(void);

void PCMout_Test_AC97(void);
void PCMin_Test_AC97(void);
void MICin_Test_AC97(void);
void Powerdown_Test_AC97(void);
void Reset_Test_AC97(void);

#ifdef __cplusplus
}
#endif

#endif    //__ac97_H__
