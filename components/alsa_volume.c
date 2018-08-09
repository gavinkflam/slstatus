/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../util.h"

/* Derived from work of RushPL licensed under WTFPL.
 * https://stackoverflow.com/a/7661176/2621216 */
const char *
alsa_vol_perc(const char *mixer_name)
{
	snd_mixer_t* handle;
	snd_mixer_elem_t* elem;
	snd_mixer_selem_id_t* sid;

	long vol, minv, maxv;

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, mixer_name);

	if ((snd_mixer_open(&handle, 0)) < 0)
		return NULL;

	if ((snd_mixer_attach(handle, "default")) < 0) {
		snd_mixer_close(handle);
		return NULL;
	}

	if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		snd_mixer_close(handle);
		return NULL;
	}

	if ((snd_mixer_load(handle)) < 0) {
		snd_mixer_close(handle);
		return NULL;
	}

	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		snd_mixer_close(handle);
		return NULL;
	}

	if (snd_mixer_selem_get_playback_volume(elem, 0, &vol) < 0) {
		snd_mixer_close(handle);
		return NULL;
	}

	snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);
  snd_mixer_close(handle);

	return bprintf("%d", (vol - minv) * 100 / (maxv - minv));
}
