
#ifndef SCRC_WIFI_H_
#define SCRC_WIFI_H_

void nw_setup();
bool nw_start(unsigned long timeout, int retry_delay);
void nw_stop();

bool nw_is_connected();

#endif /* SCRC_WIFI_H_ */
