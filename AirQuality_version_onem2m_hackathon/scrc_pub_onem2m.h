

#ifndef SCRC_PUB_ONEM2M_H_
#define SCRC_PUB_ONEM2M_H_

int pub_pri_buf_data(const struct sensors_data *ptr_buf,
		const short int latest_buf_idx, const short int hist_buf,
		bool pub_hist);

#endif /* SCRC_PUB_ONEM2M_H_ */
