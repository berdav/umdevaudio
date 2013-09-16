/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* pcm aux functions                                                               */
/* Copyright (C) 2013  Davide Berardi                                              */
/*                                                                                 */
/* This program is free software; you can redistribute it and/or                   */
/* modify it under the terms of the GNU General Public License                     */
/* as published by the Free Software Foundation; either version 2                  */
/* of the License, or (at your option) any later version.                          */
/*                                                                                 */
/* This program is distributed in the hope that it will be useful,                 */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   */
/* GNU General Public License for more details.                                    */
/*                                                                                 */
/* You should have received a copy of the GNU General Public License               */
/* along with this program; if not, write to the Free Software                     */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */
/***********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sound/asound.h>
#include <time.h>
#include <stdint.h>

#include "snd_transfer_functions.h"

#define hw_is_mask(x) 	(x	>=	SNDRV_PCM_HW_PARAM_FIRST_MASK && x		<=	SNDRV_PCM_HW_PARAM_LAST_MASK)
#define hw_is_interval(x) 	(x	>=	SNDRV_PCM_HW_PARAM_FIRST_INTERVAL && x		<=	SNDRV_PCM_HW_PARAM_LAST_INTERVAL)

struct snd_buffer *sb;

/* Prepare the virtual sound card */
int virtual_prepare(struct snd_buffer *s)
{
	sb=s;
	return 0;
}

/* Set an hw interval */
void setInterval(struct snd_pcm_hw_params *params, int paramName, int min, int max, int openmin, int openmax, int integer, int empty)
{
	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].min=min;
	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].max=max;

	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].openmax=openmax;
	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].openmin=openmin;
	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].integer=integer;
	params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].empty=empty;

	params->cmask|=(1<<paramName);

}

/* Return the hw interval (internal to the structure) */
struct snd_interval getConf(struct snd_interval *conf, int val)
{
	return conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

/* Get an hw interval */
struct snd_interval getInterval(struct snd_pcm_hw_params *params, int paramName)
{
	return params->intervals[paramName-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

/* Set an hw mask*/
void setMask(struct snd_pcm_hw_params *params, int paramName, int *values)
{
	int i;
	/* we need all the masks? for now we use only the first 2 (access mode) */
	for(i=0;i<2/*MASKMAXSIZE*/;i++)
	{
		params->masks[paramName-SNDRV_PCM_HW_PARAM_FIRST_MASK].bits[i]=values[i];
	}
}

/* Refine the stream */
int virtual_hw_refine(struct snd_interval *conf,/*struct snd_pcm_substream *substream,*/ struct snd_pcm_hw_params *params)
{

#ifdef UMAUDIO_DEBUG
{
	int i=0;
	for(i=SNDRV_PCM_HW_PARAM_FIRST_MASK;i<SNDRV_PCM_HW_PARAM_LAST_MASK;i++){
		if(params->rmask & (1 << i)){
			if(hw_is_mask(i)){
				printf("[debug] detected a mask request with id: %d\n", i);
			}
		}
	}

	for(i=SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;i<=SNDRV_PCM_HW_PARAM_LAST_INTERVAL;i++){
		if(params->rmask & (1 << i)){
			if(hw_is_interval(i)){
				printf("[debug] detected an interval request with value: %d\n", i);
				switch(i){
					case SNDRV_PCM_HW_PARAM_SAMPLE_BITS:
						printf("SNDRV_PCM_HW_PARAM_SAMPLE_BITS\n");
						break;
					case SNDRV_PCM_HW_PARAM_FRAME_BITS:
						printf("SNDRV_PCM_HW_PARAM_FRAME_BITS\n");
						break;
					case SNDRV_PCM_HW_PARAM_CHANNELS:
						printf("SNDRV_PCM_HW_PARAM_CHANNELS\n");
						break;
					case SNDRV_PCM_HW_PARAM_RATE:
						printf("SNDRV_PCM_HW_PARAM_RATE\n");
						break;
					case SNDRV_PCM_HW_PARAM_PERIOD_TIME:
						printf("SNDRV_PCM_HW_PARAM_PERIOD_TIME\n");
						break;
					case SNDRV_PCM_HW_PARAM_PERIOD_SIZE:
						printf("SNDRV_PCM_HW_PARAM_PERIOD_SIZE\n");
						break;
					case SNDRV_PCM_HW_PARAM_PERIOD_BYTES:
						printf("SNDRV_PCM_HW_PARAM_PERIOD_BYTES\n");
						break;
					case SNDRV_PCM_HW_PARAM_PERIODS:
						printf("SNDRV_PCM_HW_PARAM_PERIODS\n");
						break;
					case SNDRV_PCM_HW_PARAM_BUFFER_TIME:
						printf("SNDRV_PCM_HW_PARAM_BUFFER_TIME\n");
						break;
					case SNDRV_PCM_HW_PARAM_BUFFER_SIZE:
						printf("SNDRV_PCM_HW_PARAM_BUFFER_SIZE\n");
						break;
					case SNDRV_PCM_HW_PARAM_BUFFER_BYTES:
						printf("SNDRV_PCM_HW_PARAM_BUFFER_BYTES\n");
						break;
					case SNDRV_PCM_HW_PARAM_TICK_TIME:
						printf("SNDRV_PCM_HW_PARAM_TICK_TIME\n");
						break;
					default:
						printf("not recognized interval\n");
						break;
				}
			}
		}
	}
}
#endif

	/* This program don't use infos */
	params->info= 0x0;

	/* Some magic numbers, access MMAP and support to it */
	uint32_t values[((SNDRV_MASK_MAX+31)/32)]={/*9,*/0};
	//values[0]=9; /* MMAP SUPPORT / readi, writei */
	values[0]|=SNDRV_PCM_ACCESS_RW_INTERLEAVED;
	setMask(params, SNDRV_PCM_HW_PARAM_FIRST_MASK, values);

	values[0]=1028;
	setMask(params, SNDRV_PCM_HW_PARAM_FIRST_MASK+1, values);

	values[0]=1;
	setMask(params, SNDRV_PCM_HW_PARAM_FIRST_MASK+2, values);

#ifdef UMAUDIO_DEBUG
{
	printf("getintervals\n");
	struct snd_interval si=getInterval(params,SNDRV_PCM_HW_PARAM_CHANNELS);
	printf("CHANNELS: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
	printf("SAMPLE_BITS: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_FRAME_BITS);
	printf("FRAME_BITS: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_RATE);
	printf("RATE: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);

	printf("PERIOD_BYTES: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_PERIOD_TIME);
	printf("PERIOD_TIME: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_BUFFER_TIME);
	printf("BUFFER_TIME: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
	printf("BUFFER_BYTES: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
	printf("BUFFER_SIZE: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
	si=getInterval(params,SNDRV_PCM_HW_PARAM_TICK_TIME);
	printf("TICK_TIME: %d %d %d %d %d %d\n", si.min, si.max, si.openmin, si.openmax, si.integer, si.empty);
}
#endif

	/* Can't use a for cycle here because the params could not be sequentials */
	struct snd_interval so=getConf(conf, SNDRV_PCM_HW_PARAM_CHANNELS);
	#ifdef UMAUDIO_DEBUG
	printf("loading CHANNELS with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_CHANNELS, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_RATE);
	#ifdef UMAUDIO_DEBUG
	printf("loading RATE with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_RATE, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
	#ifdef UMAUDIO_DEBUG
	printf("loading SAMPLE_BITS with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_FRAME_BITS);
	#ifdef UMAUDIO_DEBUG
	printf("loading FRAME_BITS with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_FRAME_BITS, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
	#ifdef UMAUDIO_DEBUG
	printf("loading PERIOD_SIZE with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_TIME);
	#ifdef UMAUDIO_DEBUG
	printf("loading PERIOD_TIME with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_BYTES);
	#ifdef UMAUDIO_DEBUG
	printf("loading PERIOD_BYTES with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_TIME);
	#ifdef UMAUDIO_DEBUG
	printf("loading BUFFER_TIME with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	
	so=getConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
	#ifdef UMAUDIO_DEBUG
	printf("loading BUFFER_BYTES with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_BYTES, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);

	so=getConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
	#ifdef UMAUDIO_DEBUG
	printf("loading BUFFER_SIZE with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
	/* my host soundcard values in case of some problems */
	/*
	setInterval(params, SNDRV_PCM_HW_PARAM_CHANNELS, 1, 2, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, 16, 32, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_FRAME_BITS, 32, 64, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_RATE, 8000, 48000, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, 16, 8192, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, 333, 1024000, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 128, 65536, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, 666, 2048000, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 128, 65536, 0, 0, 1, 0);
	setInterval(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, 32, 16384, 0, 0, 1, 0);
	*/
	/*we need that?*/
/*
	so=getConf(conf, SNDRV_PCM_HW_PARAM_TICK_TIME);
	#ifdef UMAUDIO_DEBUG
	printf("loading TICK_TIME with params %u %u %u %u %u %u\n", so.min, so.max, so.openmin, so.openmax, so.integer, so.empty );
	#endif
	setInterval(params, SNDRV_PCM_HW_PARAM_TICK_TIME, so.min, so.max, so.openmin, so.openmax, so.integer, so.empty);
*/
/*	setInterval(params, SNDRV_PCM_HW_PARAM_TICK_TIME, 0, 0, 0, 1, 0,0); */

	return 0;
}

/* Get out the structure */
int virtual_hw_params(struct snd_interval *conf, struct snd_pcm_hw_params *params)
{
	return virtual_hw_refine(conf, params);
}

/* Virtual Params, fixed,
 * TODO set that in the conf_file?
 */
int virtual_sw_params(struct snd_pcm_sw_params *params)
{

	params->tstamp_mode = 0;//SND_PCM_TSTAMP_NONE;
	params->period_step = 1;
	params->sleep_min = 0;
	params->avail_min = 4096;//pcm->period_size;
	params->xfer_align = 1;
	params->start_threshold = 1;
	params->stop_threshold = 4096;//pcm->buffer_size;
	params->silence_threshold = 0;
	params->silence_size = 0;

	return 0;
}

/* Magic number to align mplayer time counter */
#define SLEEPTIME_AVAL_CAP 20202
#define SLEEPTIME_AVAL_PLA 20000
/* Realign time */
int sleep_realign(uint32_t frames, int playcap)
{
	struct timespec t;
	t.tv_sec=0;
	t.tv_nsec=frames * (playcap?SLEEPTIME_AVAL_PLA:SLEEPTIME_AVAL_CAP);

	nanosleep(&t, NULL);
	return 0;
}

/* Writei */
int virtual_writei(struct snd_xferi *xf)
{
	struct dummy_frame *buf=malloc(sizeof(struct dummy_frame) * xf->frames);
	int (*executer)(struct dummy_frame* buf, struct snd_xferi *xf, void *argm)=NULL;
	void *arg=NULL;

	#ifdef UMAUDIO_DEBUG
	printf("received writei with xf->frames: %u\n", xf->frames);
	#endif


	/* copying buffer from the user */
	um_mod_umoven(xf->buf, sizeof(struct dummy_frame) * xf->frames, buf);

	/* load module */

#ifdef UM_SND_FILE
	executer=snd_filewrite;
	arg=(void *)sb;
#endif

	if(executer==NULL)
		return -1;
	executer(buf, xf, arg);

	sleep_realign(xf->frames, 1);

	return 0;
}

/* Readi */
int virtual_readi(struct snd_xferi *xf)
{
	struct dummy_frame *buf=malloc(sizeof(struct dummy_frame) * xf->frames);
	
	int (*executer)(struct dummy_frame* buf, struct snd_xferi *xf, void *argm)=NULL;
	void *arg=NULL;

#ifdef UMAUDIO_DEBUG
	printf("received readi with xf->frames: %u\n", xf->frames);
#endif

 /* sound processor dispatcher */
#ifdef UM_SND_FILE
	executer=snd_fileget;
	arg=(void *)sb;
#endif

	if(executer==NULL)
		return -1;
	executer(buf, xf, arg);

	um_mod_ustoren(xf->buf, sizeof(struct dummy_frame) * xf->frames, buf);
	free(buf);

	sleep_realign(xf->frames, 0);
	return 0;
}
