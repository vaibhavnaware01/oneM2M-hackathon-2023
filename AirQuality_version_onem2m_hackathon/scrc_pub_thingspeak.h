
#ifndef SCRC_PUB_THINGSPEAK_H_
#define SCRC_PUB_THINGSPEAK_H_

#include "scrc_constants.h"
#include "scrc_node.h"

void pub_setup_thingspeak();
int pub_thingspeak(const struct sensors_data *ptr_buf,
		const short int latest_buf_idx, const short int hist_buf,
		bool pub_hist);

#endif /* SCRC_PUB_THINGSPEAK_H_ */
