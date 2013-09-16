/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* function for sound processing                                                   */
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
#include "snd_transfer_functions.h"


#ifdef UM_SND_FILE
/*read/write from files*/

int snd_file_init(void *arg)
{
	struct snd_file *sf=arg;
	/*we can do only a create call here?*/
	if(!sf->playback)
	{
		FILE *f=fopen(sf->path, "w+");
		fclose(f);
	}
	sf->seeker=-1;
}

int snd_fileget(struct dummy_frame *buf, struct snd_xferi *xf, void *argm)
{
	struct snd_file *arg=argm;

	FILE *f=fopen(arg->path, "r");
	if(!feof(f))
	{
		fseek(f, arg->seeker+1, SEEK_SET);
		arg->seeker+=xf->frames*sizeof(struct dummy_frame);
		fread(buf, sizeof(struct dummy_frame), xf->frames, f);
	}
	else
		memset(buf, 0, xf->frames *sizeof(struct dummy_frame));
	fclose(f);
}

int snd_filewrite(struct dummy_frame *buf, struct snd_xferi *xf, void *argm)
{
	struct snd_file *arg=argm;
	FILE *f=fopen(arg->path, "a");
	fwrite(buf, sizeof(struct dummy_frame), xf->frames, f);
	fclose(f);
}
#endif
