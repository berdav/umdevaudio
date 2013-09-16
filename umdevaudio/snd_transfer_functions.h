/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* functions for sound processing header file                                      */
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
#include "snd.h"

#ifndef UM_SND_FILE
	#define UM_SND_FILE
#endif

#ifdef UM_SND_FILE
/*read/write from files*/
struct snd_file{
	char *path;	/* RW filename			*/
	int seeker;	/* R  seek pointer	*/
	int playback:1;
};

int snd_file_init(void *arg);

int snd_fileget(struct dummy_frame *buf, struct snd_xferi *xf, void *argm);

int snd_filewrite(struct dummy_frame *buf, struct snd_xferi *xf, void *argm);
#endif
