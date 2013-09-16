/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* control submodule                                                               */
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

#include "snd.h"

/* static (hw) kernel variables */
static int subdev=0;
static int subscribed=0;
static int **mixer;

static int audio_init(char type, dev_t device, char *path, unsigned long flags, char *args, struct umdev *devhandle)
{
	printf("[umctl] sound: ctl module initialized\n");

	arguments_t *argums=loadArgums(args);
	#ifdef UMAUDIO_DEBUG
	printf("[umctl] cardno %i\naudio protocol version %i.%i.%i\n",argums->cardno, argums->pversion[0], argums->pversion[1], argums->pversion[2]);
	#endif
	umdev_setprivatedata(devhandle, argums);
	/*mixer loader*/
	int i;
	mixer=calloc(argums->nctl, sizeof(int *));
	for(i=0;i<argums->nctl;i++){
		mixer[i]=calloc(2,sizeof(int));
	}
	return 0;
}

static int audio_fini(char type, dev_t device, char *path, unsigned long flags, char *args, struct dev_info *di)
{
	printf("[umctl] mixer unloaded\n");
	return 0;
}

static int audio_open(char type, dev_t device, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umctl] mixer opened loopback\n");
	#endif
	/*resetting static variables*/
	subdev=0;
	subscribed=0;
	return 0;
}
static int audio_read(char type, dev_t device, char *buf, size_t len, loff_t pos, struct dev_info *di)
{
	/* protocol 2.0.10 should'nt get here */
	#ifdef UMAUDIO_DEBUG
	printf("[umctl] read\n");
	#endif
	return 0;
}

static int audio_write(char type, dev_t device, char *buf, size_t len, loff_t pos, struct dev_info *di)
{
	/* protocol 2.0.10 should'nt get here */
	#ifdef UMAUDIO_DEBUG
	printf("[umctl] write\n");
	#endif
	return 0;
}

static int audio_release(char type, dev_t device, struct dev_info *di)
{
	#ifdef UMAUDIO_DEBUG
	printf("[umctl] mixer released\n");
	#endif
	return 0;
}

static int audio_ioctl(char type, dev_t device, int req, void *arg, struct dev_info *di)
{
	arguments_t *arguments=umdev_getprivatedata(di->devhandle);
	int cardno=arguments->cardno;
	int *protocol=arguments->pversion;
	int nctl=arguments->nctl;

	#ifdef UMAUDIO_DEBUG
	printf("[umctl] received ioctl ");
	#endif
	switch(req){
		case SNDRV_CTL_IOCTL_CARD_INFO:
		{
			struct snd_ctl_card_info *ci=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_CARD_INFO\n");
			#endif
			ci->card=cardno;
			ci->pad=0;
			strcpy(ci->id, "Virtual-Sound");
			strcpy(ci->driver, "umAudio Driver");
			strcpy(ci->name, "umView Virtual Sound Device");
			strcpy(ci->longname, "umView Virtual audio Device alsa compatible");
			strcpy(ci->reserved_,"");
			strcpy(ci->mixername,"umAudio Virtual mixer");
			strcpy(ci->components,"");
			return 0;
		}
		case SNDRV_CTL_IOCTL_PVERSION:
		{
			/* return the protocol used by the audio device, we can set it from the cli */
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_PVERSION, my protocol is %i.%i.%i\n", protocol[0], protocol[1], protocol[2]);
			#endif
			setProtocol((int *)arg, protocol);
			return 0;
		}
		case SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE\n");
			#endif
			//test XXX set as the next device no (only one device)
			/* FIXME try with a static variable */
			*(int *)arg=subdev;
			if(subdev==0)
				subdev--;
			return 0;
		}
		case SNDRV_CTL_IOCTL_PCM_INFO:
		{
			struct snd_pcm_info *pcmi=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_PCM_INFO\n");
			#endif
			pcmi->card=cardno;
			pcmi->device=0;
			pcmi->subdevice=0;
			pcmi->subdevices_count=1;
			pcmi->subdevices_avail=1;
			pcmi->stream=1;
			strcpy(pcmi->id, "umAudio pcm");
			strcpy(pcmi->name, "umAudio pcm subdevice (full duplex)");
			strcpy(pcmi->subname, "umAudio pcm subdevice #0");
			return 0;
		}
		case SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE:
		{
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE\n");
			#endif
			/* TODO set is as a value from a rc file */
			*(int *)arg=0;
			return 0;
		}
		case SNDRV_CTL_IOCTL_ELEM_LIST:
		{
			struct snd_ctl_elem_list *sctl=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_ELEM_LIST\n");
			#endif
			if(sctl->space>0){
				/*request: set space-offset elements*/
				struct snd_ctl_elem_id *pids=calloc(sctl->space-sctl->offset, sizeof(struct snd_ctl_elem_id));
				int i;
				um_mod_umoven(sctl->pids, sizeof(struct snd_ctl_elem_id)*(sctl->space-sctl->offset), pids);
				for(i=sctl->offset;i<sctl->space;i++)
				{
					pids[i].device=2;
					pids[i].iface=2;
					pids[i].index=0;
					char *name=NULL;
					asprintf(&name, "Mixer %d", i);
					strcpy(pids[i].name, name);
					pids[i].numid=i+1;
					pids[i].subdevice=0;
				}
				/*setted elements (we can do it in some step or in a single step by setting all the values with the for above)*/
				sctl->used=i-sctl->offset;
				um_mod_ustoren(sctl->pids, sizeof(struct snd_ctl_elem_id)*sctl->used, pids);
				free(pids);
			}
			else{
				/*request: "how much control you have?"*/
				sctl->count=nctl;;
			}
			return 0;
		}
		case SNDRV_CTL_IOCTL_ELEM_INFO:
		{
			struct snd_ctl_elem_info *sinfo=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_ELEM_INFO\n");
			#endif
			sinfo->owner=-1;
			sinfo->type=SNDRV_CTL_ELEM_TYPE_INTEGER;
			/*number of values, for now, we only use stereo.*/
			sinfo->count=2;
			sinfo->value.integer.min=0;
			sinfo->value.integer.max=63;
			sinfo->value.integer.step=1;
			sinfo->access=SNDRV_CTL_ELEM_ACCESS_READWRITE;
			return 0;
		}
		case SNDRV_CTL_IOCTL_ELEM_READ:
		{
			struct snd_ctl_elem_value *svalue=arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_ELEM_READ\n");
			#endif
			/*TODO get id for mixer != 0*/
			svalue->value.integer.value[0]=mixer[svalue->id.index][0];
			svalue->value.integer.value[1]=mixer[svalue->id.index][1];
			return 0;
		}
		case SNDRV_CTL_IOCTL_ELEM_WRITE:
		{
			struct snd_ctl_elem_value *svalue=arg;
			mixer[svalue->id.index][0]=svalue->value.integer.value[0];
			mixer[svalue->id.index][1]=svalue->value.integer.value[1];
			return 0;
		}
		case SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS:
		{
			int a=*(int *)arg;
			#ifdef UMAUDIO_DEBUG
			printf("SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS\n");
			#endif
			if(a<0) a=subscribed;
			subscribed=(a?1:0);
			return 0;
		}
		default:
			#ifdef UMAUDIO_DEBUG
			printf("req 0x%x\n", req);
			#endif
			return -ENOSYS;
	}
	return 0;
}

static int audio_ioctl_params(char type, dev_t device, int req, void *arg, struct dev_info *di){
	#ifdef UMAUDIO_DEBUG
	printf("[umaudio] received ioctl params ");
	#endif
	switch(req){
		case SNDRV_CTL_IOCTL_CARD_INFO:
			#ifdef UMAUDIO_DEBUG
			printf("for CARD_INFO\n");
			#endif
			return _IOR(0,0,struct snd_ctl_card_info);
		case SNDRV_CTL_IOCTL_PVERSION:
			#ifdef UMAUDIO_DEBUG
			printf("for PVERSION\n");
			#endif
			return _IOR(0,0,int);
			
		case SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE:
			#ifdef UMAUDIO_DEBUG
			printf("for PCM_NEXT_DEVICE\n");
			#endif
			return _IOR(0,0,int);
		case SNDRV_CTL_IOCTL_PCM_INFO:
			#ifdef UMAUDIO_DEBUG
			printf("for PCM_INFO\n");
			#endif
			return _IOWR(0,0,struct snd_pcm_info);
		case SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE:
			#ifdef UMAUDIO_DEBUG
			printf("for PCM_PREFER_SUBDEVICE\n");
			#endif
			return _IOW(0,0,int);
		case SNDRV_CTL_IOCTL_ELEM_LIST:
			#ifdef UMAUDIO_DEBUG
			printf("for ELEM_LIST\n");
			#endif
			return _IOWR(0,0,struct snd_ctl_elem_list);
		case SNDRV_CTL_IOCTL_ELEM_INFO:
			#ifdef UMAUDIO_DEBUG
			printf("for ELEM_INFO\n");
			#endif
			return _IOWR(0,0,struct snd_ctl_elem_info);
		case SNDRV_CTL_IOCTL_ELEM_READ:
			#ifdef UMAUDIO_DEBUG
			printf("for ELEM_READ\n");
			#endif
			return _IOWR(0,0,struct snd_ctl_elem_value);
		case SNDRV_CTL_IOCTL_ELEM_WRITE:
			#ifdef UMAUDIO_DEBUG
			printf("for ELEM_WRITE\n");
			#endif
			return _IOWR(0,0,struct snd_ctl_elem_value);
		case SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS:
			#ifdef UMAUDIO_DEBUG
			printf("for SUBSCRIBE_EVENTS\n");
			#endif
			return _IOWR(0,0,int);
		default: return 0;
	}
}
struct umdev_operations umdev_ops={
	.init=audio_init,
	.fini=audio_fini,
	.open=audio_open,
	.read=audio_read,
	.write=audio_write,
	.release=audio_release,
	.ioctl=audio_ioctl,
	.ioctlparms=audio_ioctl_params,
};
