/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* pcm submodule                                                                   */
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
#include "umdev.h"
#include <config.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sound/asound.h>
#include <linux/types.h>

#include <assert.h>
#include <alloca.h>

//#include "snd.h"
#include "snd_transfer_functions.h"
#include "pcm_aux.h"

static int timestamp=0;

struct snd_buffer *sb;

static int pcm_lseek(char type, dev_t device, loff_t start, loff_t stop, struct dev_info *di)
{
	/* protocol >= 2.0.10 should'nt get here */
	printf("[umpcm] lseek");
	return 0;
}

static int pcm_init(char type, dev_t device, char *path, unsigned long flags, char *args, struct umdev *devhandle)
{
	printf("[umpcm] sound module initialized\n");

	arguments_t *argums=loadArgums(args);
	#ifdef UMAUDIO_DEBUG
	printf("cardno %i\nversion %i.%i.%i\n",argums->cardno, argums->pversion[0], argums->pversion[1], argums->pversion[2]);
	#endif
	umdev_setprivatedata(devhandle, argums);

	int (*pcm_method_init)(void * arg)=NULL;
	void *pcm_method_arg=NULL;

	#ifdef UM_SND_FILE

//	struct snd_file *sf=malloc(sizeof(struct snd_file));
	sb=malloc(sizeof(struct snd_file));
	((struct snd_file *)sb)->playback=1-argums->playback;

	pcm_method_init=snd_file_init;
	asprintf(&(((struct snd_file *)sb)->path), "%s", /*"/tmp/outBuffer"*/argums->outfile);
	pcm_method_arg=sb;
	#endif

	if(pcm_method_init!=NULL)
		pcm_method_init(pcm_method_arg);

	return 0;
}

static int pcm_fini(char type, dev_t device, char *path, unsigned long flags, char *args, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] mixer unloaded\n");
	#endif
	return 0;
}

static int pcm_open(char type, dev_t device, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] mixer opened loopback\n");
	#endif
	timestamp=0;
	return 0;
}

static int pcm_read(char type, dev_t device, char *buf, size_t len, loff_t pos, struct dev_info *di)
{
	/* protocol >= 2.0.10 should'nt get here */
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] read %d bytes %d off\n", len, pos);
	#endif
	/*we don't use read, because mmap should fail*/
	/*TODO, we need to modify this to make alsa's mmap work*/
	return 0;
}

static int pcm_write(char type, dev_t device, char *buf, size_t len, loff_t pos, struct dev_info *di)
{
	/* protocol >= 2.0.10 should'nt get here */
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] write\n");
	#endif
	return 0;
}

static int pcm_release(char type, dev_t device, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] mixer released\n");
	#endif
	return 0;
}

static int pcm_ioctl(char type, dev_t device, int req, void *arg, struct dev_info *di)
{
	arguments_t *arguments=umdev_getprivatedata(di->devhandle);
	int *protocol=arguments->pversion;
	struct snd_pcm_hw_params *spam;

	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] received ioctl ");
	#endif

	switch(req){
		case SNDRV_PCM_IOCTL_PVERSION:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_PVERSION\n");
			#endif
			*(int *)arg=SNDRV_PROTOCOL_VERSION(protocol[0],protocol[1],protocol[2]);
			return 0;
		}
		case SNDRV_PCM_IOCTL_INFO:
		{
			struct snd_pcm_info *spi=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNRDV_PCM_IOCTL_INFO\n");
			#endif
			spi->device=0;
			spi->subdevice=0;
			spi->stream=1;
			/* XXX segfault on following struct fields
			 * UMVIEW bug?
			 */
			/* spi->card=0; */
			return 0;
		}
		case SNDRV_PCM_IOCTL_CHANNEL_INFO:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_CHANNEL_INFO\n");
			#endif
			struct snd_pcm_channel_info *spci=arg;
			spci->channel=0;
			return 0;
		}
		case SNDRV_PCM_IOCTL_TTSTAMP:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_TTSTAMP\n");
			#endif
			/* TODO implement */
			timestamp=*(int *)arg;
			return 0;
		}
		case SNDRV_PCM_IOCTL_HW_REFINE:
		{
			struct snd_pcm_hw_params *spham=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_HW_REFINE\n");
			#endif
			spham->intervals[SNDRV_PCM_HW_PARAM_CHANNELS-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].min=1;
			return virtual_hw_refine(arguments->conf, /*substream,*/ spham);
		}
		case SNDRV_PCM_IOCTL_HW_PARAMS:
		{
			struct snd_pcm_hw_params *spham=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_HW_PARAMS\n");
			#endif
			return virtual_hw_params(arguments->conf, /*substream,*/ spham);
		}
		case SNDRV_PCM_IOCTL_SW_PARAMS:
		{
			struct snd_pcm_sw_params *spsam=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_SW_PARAMS\n");
			#endif
			return virtual_sw_params(/*substream,*/ spsam);
		}
		case SNDRV_PCM_IOCTL_PREPARE:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_PREPARE\n");
			#endif
			return virtual_prepare(sb);
		}
		case SNDRV_PCM_IOCTL_WRITEI_FRAMES:
		{
			struct snd_xferi *xf=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_WRITEI_FRAMES\n");
			#endif
			return virtual_writei(xf);
		}
		case SNDRV_PCM_IOCTL_READI_FRAMES:
		{
			struct snd_xferi *xf=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_READI_FRAMES\n");
			#endif
			return virtual_readi(xf);
		}
		case SNDRV_PCM_IOCTL_START:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_START\n");
			#endif
			return 0;
		}
		case SNDRV_PCM_IOCTL_SYNC_PTR:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_SYNC_PTR\n");
			#endif
			struct snd_pcm_sync_ptr *s=arg;
			return 0;
		}
		case SNDRV_PCM_IOCTL_DROP:
		{
			/* not implemented */
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_DROP\n");
			#endif
			return -ENOSYS;
		}
		case SNDRV_PCM_IOCTL_HW_FREE:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_PCM_IOCTL_HW_FREE\n");
			#endif
			return -ENOSYS;
		}
		default:
		{
			#ifdef UMAUDIO_DEBUG
			printf("req: 0x%x\n", req);
			#endif
			return -ENOSYS;
		}
	}
	return 0;
}

int pcm_ioctl_params(char type, dev_t device, int req, void *arg, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umpcm] received ioctl params ");
	#endif
	switch(req){
		case SNDRV_PCM_IOCTL_PVERSION:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_PVERSION\n");
			#endif
			return _IOR(0,0,sizeof(int));
		case SNDRV_PCM_IOCTL_INFO:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_INFO\n");
			#endif
			return _IOR(0,0,struct snd_pcm_info);
		case SNDRV_PCM_IOCTL_HW_REFINE:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_HW_REFINE\n");
			#endif
			return _IOWR(0,0,struct snd_pcm_hw_params);
#if 0
		/* unsupported old ioctl */
		case SND_PCM_IOCTL_HW_REFINE_OLD:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_HW_REFINE_OLD\n");
			#endif
			printf("OLD audio protocol (< 2.0.2) not supported\n");
			return 0;
		case SND_PCM_IOCTL_HW_PARAMS_OLD:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_HW_PARAMS_OLD\n");
			#endif
			printf("OLD audio protocol (< 2.0.2) not supported\n");
			return 0;
#endif
		case SNDRV_PCM_IOCTL_HW_PARAMS:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_HW_PARAMS\n");
			#endif
			return _IOWR(0,0,struct snd_pcm_hw_params);
		case SNDRV_PCM_IOCTL_SW_PARAMS:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_SW_PARAMS\n");
			#endif
			return _IOWR(0,0,struct snd_pcm_sw_params);
		case SNDRV_PCM_IOCTL_SYNC_PTR:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_SYNC_PTR\n");
			#endif
			return _IOWR(0,0,struct snd_pcm_sync_ptr);
		case SNDRV_PCM_IOCTL_CHANNEL_INFO:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_CHANNEL_INFO\n");
			#endif
			return _IOR(0, 0, struct snd_pcm_channel_info);
		case SNDRV_PCM_IOCTL_DROP:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_DROP\n");
			#endif
			return 0;
		case SNDRV_PCM_IOCTL_HW_FREE:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_HW_FREE\n");
			#endif
			return 0;
		case SNDRV_PCM_IOCTL_PREPARE:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_PREPARE\n");
			#endif
			return 0;
		case SNDRV_PCM_IOCTL_TTSTAMP:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_TTSTAMP\n");
			#endif
			return _IOW(0, 0, int);
		case SNDRV_PCM_IOCTL_WRITEI_FRAMES:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_WRITEI_FRAMES\n");
			#endif
			return _IOW(0, 0, struct snd_xferi);
		case SNDRV_PCM_IOCTL_READI_FRAMES:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_READI_FRAMES\n");
			#endif
			/* XXX little buglike thing, we need that as a _IOR but if we set to return that macro value, the core give us wrong values
			p.e. xferi->frames = 16 when it should be 4096*/
			return _IOW(0, 0, struct snd_xferi);
		case SNDRV_PCM_IOCTL_START:
			#ifdef UMAUDIO_DEBUG
			printf("IOCTL_START\n");
			#endif
			return 0;
		default:
			#ifdef UMAUDIO_DEBUG
			printf("req: 0x%x\n", req);
			#endif
			return 0;
	}
}

struct umdev_operations umdev_ops={
	.init=pcm_init,
	.fini=pcm_fini,
	.open=pcm_open,
	.read=pcm_read,
	.write=pcm_write,
	.release=pcm_release,
	.ioctl=pcm_ioctl,
	.ioctlparms=pcm_ioctl_params,
	.lseek=pcm_lseek,
};
