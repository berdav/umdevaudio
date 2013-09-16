/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* conffile and line parsing                                                       */
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sound/asound.h>
#include "snd.h"

#define MAX_OPTION_LEN 256
#define MAX_VNUMBER_LEN 5
/* Max length of a conf line */
#define MAXCONF 2048

/* Simple function to set the a buffer to 0 */
/* XXX we can substitute it with bzero? */
static void bZero(void *p, int c)
{
#if 0
	bzero(p,c);
#else
	byte *x=p;
	int i;
	for(i=0;i<c;x[i]=0,i++)
		;
#endif
}

/* Get the next argument in the src list */
/* TODO it's space sensitive, we should make it space unsensitive? */
int getNextArg(char *src, char *dst)
{
	int i=0;
	for(i=0;src[i]!='\0' && src[i]!=','; i++) /* go to the next ',' */
		;
	if(i!=0)
	{
		strncpy(dst, src, i);
		dst[i]='\0';
	}
	return i;
}

/* get key/value from src */
int getKey(char *src, char *key, char *value)
{
	int i;
	bZero(key, MAX_OPTION_LEN);
	bZero(value, MAX_OPTION_LEN);
	for(i=0;src[i]!=':';i++)
		;
	if(i!=0)
	{
		/* loading the key:value in the right variables */
		strncpy(key, src, i);
		strcpy(value, src+i+1);
	}
	return 0;
}

/* Load version string in the array */
void loadVersion(int *out, char *src)
{
	int i,j,k;
	char major[MAX_VNUMBER_LEN];
	char minor[MAX_VNUMBER_LEN];
	char subminor[MAX_VNUMBER_LEN];

	for(i=0;src[i]!='.';i++)
		major[i]=src[i];
	i++;
	for(j=0;src[i+j]!='.';j++)
		minor[j]=src[i+j];
	j++;
	for(k=0;src[i+j+k]!='\0';k++)
		subminor[k]=src[i+j+k];
	out[0]=atoi(major);
	out[1]=atoi(minor);
	out[2]=atoi(subminor);
}

/* Options loader/dispatcher */
void setOption(char *key, char *value, arguments_t *arg)
{
	if(!strcmp(key, "card"))
	{
		arg->cardno=atoi(value);
	}
	else if(!strcmp(key, "version"))
	{
		loadVersion(arg->pversion, value);
	}
	else if(!strcmp(key, "controls"))
	{
		arg->nctl=atoi(value);
	}
	else if(!strcmp(key, "playcap"))
	{
		if(value[0]=='c')
			arg->playback=0;
		else if(value[0]=='p')
			arg->playback=1;
		else
			printf("not recognized play/cap flag %s\n", value);
	}
	else if(!strcmp(key, "file"))
	{
		if(arg->outfile!=NULL)
			free(arg->outfile);
		arg->outfile=calloc(strlen(value), sizeof(char));
		strcpy(arg->outfile, value);
	}
	else if(!strcmp(key, "configrc"))
	{
		arg->conffile=calloc(strlen(value), sizeof(char));
		strcpy(arg->conffile, value);
	}
	else
	{
		printf("unrecognized option %s\n", key, value);
	}
}

/* Load configuration inside the structure */
void setConf(struct snd_interval *conf, int val, unsigned int min, unsigned int max, unsigned int openmin, unsigned int openmax, unsigned int integer, unsigned int empty)
{
	conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].min=min;
	conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].max=max;
	conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].openmin=openmax;
	conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].integer=integer;
	conf[val-SNDRV_PCM_HW_PARAM_FIRST_INTERVAL].empty=empty;
}

/* Load configuration from file into the structure */
void loadConf(struct snd_interval *conf, char *path)
{
	FILE *f=fopen(path, "r");
	#ifdef UMAUDIO_DEBUG
	printf("loading configuration in file %s\n", path);
	#endif
	while(!feof(f))
	{
		char name[MAXCONF];
		unsigned int min, max, openmin, openmax, integer, empty;
		fgets(name, MAXCONF, f);
		if(!feof(f) && name[0]!='\n' && name[0] != '#' /* comments */ )
		{
			char realName[MAXCONF];
			sscanf(name, "%s %u %u %u %u %u %u %u", &realName, &min, &max, &openmin, &openmax, &integer, &empty);
			#ifdef UMAUDIO_DEBUG
			printf("loading line %s %u %u %u %u %u %u\n", realName, min, max, openmin, openmax, integer, empty);
			#endif
			if(!strcmp(realName, "CHAN"))
				setConf(conf, SNDRV_PCM_HW_PARAM_CHANNELS, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "SAMb"))
				setConf(conf, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "FRAb"))
				setConf(conf, SNDRV_PCM_HW_PARAM_FRAME_BITS, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "PARR"))
				setConf(conf, SNDRV_PCM_HW_PARAM_RATE, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "PERS"))
				setConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "PERT"))
				setConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_TIME, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "PERB"))
				setConf(conf, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "BUFT"))
				setConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_TIME, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "BUFB"))
				setConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_BYTES, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "BUFS"))
				setConf(conf, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, min, max, openmin, openmax, integer, empty);
			if(!strcmp(realName, "TICK"))
				setConf(conf, SNDRV_PCM_HW_PARAM_TICK_TIME, min, max, openmin, openmax, integer, empty);
		}
	}
}

/* Load the arguments into the arguments_t structure */
arguments_t *loadArgums(char *args)
{
	char *home;
	int index=1;
	char *src=args;
	char option[strlen(args)];

	arguments_t *out=malloc(sizeof(arguments_t));
	/* default values */
	out->cardno=FIRST_CARD;
	/* my current host machine protocol value */
	out->pversion[0]=2;
	out->pversion[1]=0;
	out->pversion[2]=10;

	out->nctl=1;

	/* playback mode */
	out->playback=1;

	if(strlen(args)<=0)
		/* return default values */
		return out;

	out->outfile=calloc(strlen("/tmp/outBuffer\0"), sizeof(char));
	strcpy(out->outfile, "/tmp/outBuffer\0");

	asprintf(&home,"%s/.umdevaudiorc", getenv("HOME"));
	out->conffile=calloc(strlen(home), sizeof(char));
	strcpy(out->conffile, home);


	while(index>0)
	{
		char key[MAX_OPTION_LEN];
		char value[MAX_OPTION_LEN];
		index=getNextArg(src, option);
		/* option is loaded with the key:value string */
		getKey(option, key, value);

		/* dispatch and applicate the option */
		setOption(key, value, out);

		/* go ahead */
		src+=index;
		if(src[0]!='\0')
			/* we get another option */
			src+=1;
		else
			/* break the cycle */
			index=-1;
	}
	out->conf=calloc(SNDRV_PCM_HW_PARAM_LAST_INTERVAL+1, sizeof(struct snd_interval));
	loadConf(out->conf, out->conffile);

	#ifdef UMAUDIO_DEBUG
	printf("using outfile : %s\n", out->outfile);
	#endif
	return out;
}
