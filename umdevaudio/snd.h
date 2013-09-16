/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* global options                                                                  */
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
#define FIRST_CARD -1
#ifndef UMAUDIO
	#define UMAUDIO
#endif
#ifndef UMAUDIO_DEBUG
	#define UMAUDIO_DEBUG
#endif

#ifndef UM_SND_FILE
	#define UM_SND_FILE
#endif

/* net support still under development */
#ifndef UM_SND_NET
//	#define UM_SND_NET
#endif

#include <stdlib.h>
#define byte char
#define CTL_COUNT 20

/*enum input_arg{
	ARGS_CARDNO,
	ARGS_PVERMAJOR,
	ARGS_PVERMINOR,
	ARGS_PVERSUBMINOR,
	ARGS_NCTL,
	ARGS_PLACAP,
	ARGS_MAX
};*/
#define SEPARATOR ','

typedef struct {
	int cardno;
	int pversion[3];
	int nctl;
	int playback:2;	/* XXX --pedantic lend to a warning with a bitfield value of 1 on an assign (p.e. out->playback=1)
									 * so we need set that to :2 */
	char *outfile;
	char *conffile;
	struct snd_interval *conf;
} arguments_t;

/*fake structure to get the size of a frame*/
#define CHNO 2
#define BPCH 2
struct dummy_frame{
	char bytes[CHNO][BPCH];
};

static void bZero(void *p, int c);
arguments_t *loadArgums(char *args);

/*macro version*/
#define setProtocol(dst, protocol)\
	(*dst=SNDRV_PROTOCOL_VERSION(protocol[0], protocol[1], protocol[2]))
