/***********************************************************************************/
/* UmDevAudio umview module for audio processing                                   */
/* pcm_aux functions header file                                                   */
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
#include <string.h>
#include <sound/asound.h>
#include <time.h>

#ifndef SNDRV_MASK_BITS
	#define SNDRV_MASK_BITS	64
#endif
#ifndef SNDRV_MASK_SIZE
	#define SNDRV_MASK_SIZE	(SNDRV_MASK_BITS/32)
#endif
#ifndef MASK_OFS
	#define MASK_OFS(i)	((i) >> 5)
#endif
#ifndef MASK_BIT
	#define MASK_BIT(i) (1U << ((i)&31))
#endif
#define UM_MASKSET 0xff

#define MASKMAXSIZE ((SNDRV_MASK_MAX+31)/32)
#define snd_pcm_access_t int

int virtual_prepare(struct snd_buffer *s);
void setInterval(struct snd_pcm_hw_params *params, int paramName, int min, int max, int openmin, int openmax, int integer, int empty);
struct snd_interval getInterval(struct snd_pcm_hw_params *params, int paramName);
void setMask(struct snd_pcm_hw_params *params, int paramName, int *values);
int virtual_hw_refine(struct snd_interval *conf,/*struct snd_pcm_substream *substream,*/ struct snd_pcm_hw_params *params);
int virtual_hw_params(struct snd_interval *conf, struct snd_pcm_hw_params *params);
int virtual_sw_params(struct snd_pcm_sw_params *params);
int sleep_realign(uint32_t frames, int playcap);
int virtual_writei(struct snd_xferi *xf);
int virtual_readi(struct snd_xferi *xf);
