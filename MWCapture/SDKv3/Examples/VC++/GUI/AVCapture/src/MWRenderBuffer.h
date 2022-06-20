#ifndef MWRENDERBUFFER_H
#define MWRENDERBUFFER_H

#include "stdint.h"
#include "windows.h"

typedef struct mw_buffer_s {
	uint8_t *m_p_buf;
	uint64_t m_u64_timestamp;
	uint32_t m_u32_size_buf;
	uint32_t m_u32_size_data;
} mw_buffer_t;

class CMWRenderBuffer {
public:
	CMWRenderBuffer();
	~CMWRenderBuffer();

public:
	bool alloc_buf(uint32_t t_u32_size);
	void free_buf();

	// get buf-free to fill
	mw_buffer_t *get_buf_to_fill();
	void put_buf_filled();

	mw_buffer_t *get_buf_render(uint32_t &t_id_render);

	mw_buffer_t *get_buf_encode(uint32_t &t_id_encode);

	uint32_t get_buf_num();
	mw_buffer_t *get_buf_by_id(uint32_t t_u32_id);

	void clear_data();
	bool is_clear();

	void reset_encode_id();

protected:
	uint32_t m_u32_id_cur_buf;
	uint32_t m_u32_size_cur_buf;

	mw_buffer_t *m_pa_buf;
	uint32_t m_u32_num_buf;

	uint32_t m_u32_id_write;
	uint32_t m_u32_id_read_render;
	uint32_t m_u32_id_read_encode;
	uint32_t m_u32_dur_render;
	uint32_t m_u32_dur_encode;

	bool m_b_clear;
	CRITICAL_SECTION m_cs_lock;
};

#endif