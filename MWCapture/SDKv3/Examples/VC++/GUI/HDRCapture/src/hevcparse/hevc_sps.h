#ifndef HEVC_SPS_H
#define HEVC_SPS_H
#include <stdio.h>
#include <vector>
using std::vector;
typedef struct st_HEVC_SPS_TIER_LEVEL {
	unsigned char general_profile_space;
	unsigned char general_tier_flag;
	unsigned char general_profile_idc;
	unsigned char general_profile_compatibility_flag[32];
	unsigned char general_progressive_source_flag;
	unsigned char general_interlaced_source_flag;
	unsigned char general_non_packed_constraint_flag;
	unsigned char general_frame_only_constraint_flag;
	unsigned char general_level_idc;
	vector<unsigned char> sub_layer_profile_present_flag;
	vector<unsigned char> sub_layer_level_present_flag;
	vector<unsigned char> sub_layer_profile_space;
	vector<unsigned char> sub_layer_tier_flag;
	vector<unsigned char> sub_layer_profile_idc;
	vector<vector<unsigned char>> sub_layer_profile_compatibility_flag;
	vector<unsigned char> sub_layer_progressive_source_flag;
	vector<unsigned char> sub_layer_interlaced_source_flag;
	vector<unsigned char> sub_layer_non_packed_constraint_flag;
	vector<unsigned char> sub_layer_frame_only_constraint_flag;
	vector<unsigned char> sub_layer_level_idc;
} ST_HEVC_SPS_TIER_LEVEL;

typedef struct st_HEVC_SCALING_LIST_DATA {
	vector<vector<unsigned char>> scaling_list_pred_mode_flag;
	vector<vector<unsigned int>> scaling_list_pred_matrix_id_delta;
	vector<vector<unsigned int>> scaling_list_dc_coef_minus8;
	vector<vector<vector<unsigned int>>> scaling_list_delta_coef;
} ST_HEVC_SCALING_LIST_DATA;

typedef struct st_HEVC_SHORT_TERM_REF_PIC_SET {
	unsigned char inter_ref_pic_set_prediction_flag;
	unsigned int delta_idx_minus1;
	unsigned char delta_rps_sign;
	unsigned int abs_delta_rps_minus1;
	vector<unsigned char> used_by_curr_pic_flag;
	vector<unsigned char> use_delta_flag;
	unsigned int num_negative_pics;
	unsigned int num_positive_pics;
	vector<unsigned int> delta_poc_s0_minus1;
	vector<unsigned char> used_by_curr_pic_s0_flag;
	vector<unsigned int> delta_poc_s1_minus1;
	vector<unsigned char> used_by_curr_pic_s1_flag;
} ST_HEVC_SHORT_TERM_REF_PIC_SET;
typedef struct st_HEVC_SUBLAYER_HRD_PARAMETERS {
	vector<unsigned int> bit_rate_value_minus1;
	vector<unsigned int> cpb_size_value_minus1;
	vector<unsigned int> cpb_size_du_value_minus1;
	vector<unsigned int> bit_rate_du_value_minus1;
	vector<unsigned char> cbr_flag;
} ST_HEVC_SUBLAYER_HRD_PARAMETERS;

typedef struct st_HEVC_HRD_PARAMETERS {
	unsigned char nal_hrd_parameters_present_flag;
	unsigned char vcl_hrd_parameters_present_flag;
	unsigned char sub_pic_hrd_params_present_flag;
	unsigned char tick_divisor_minus2;
	unsigned char du_cpb_removal_delay_increment_length_minus1;
	unsigned char sub_pic_cpb_params_in_pic_timing_sei_flag;
	unsigned char dpb_output_delay_du_length_minus1;
	unsigned char bit_rate_scale;
	unsigned char cpb_size_scale;
	unsigned char cpb_size_du_scale;
	unsigned char initial_cpb_removal_delay_length_minus1;
	unsigned char au_cpb_removal_delay_length_minus1;
	unsigned char dpb_output_delay_length_minus1;
	vector<unsigned char> fixed_pic_rate_general_flag;
	vector<unsigned char> fixed_pic_rate_within_cvs_flag;
	vector<unsigned int> elemental_duration_in_tc_minus1;
	vector<unsigned char> low_delay_hrd_flag;
	vector<unsigned int> cpb_cnt_minus1;
	vector<ST_HEVC_SUBLAYER_HRD_PARAMETERS> nal_sub_layer_hrd_parameters;
	vector<ST_HEVC_SUBLAYER_HRD_PARAMETERS> vcl_sub_layer_hrd_parameters;
} ST_HEVC_HRD_PARAMETERS;

typedef struct st_HEVC_VUI_PARAMETERS {
	unsigned char aspect_ratio_info_present_flag;
	unsigned char aspect_ratio_idc;
	unsigned short sar_width;
	unsigned short sar_height;
	unsigned char overscan_info_present_flag;
	unsigned char overscan_appropriate_flag;
	unsigned char video_signal_type_present_flag;
	unsigned char video_format;
	unsigned char video_full_range_flag;
	unsigned char colour_description_present_flag;
	unsigned char colour_primaries;
	unsigned char transfer_characteristics;
	unsigned char matrix_coeffs;
	unsigned char chroma_loc_info_present_flag;
	unsigned int chroma_sample_loc_type_top_field;
	unsigned int chroma_sample_loc_type_bottom_field;
	unsigned char neutral_chroma_indication_flag;
	unsigned char field_seq_flag;
	unsigned char frame_field_info_present_flag;
	unsigned char default_display_window_flag;
	unsigned int def_disp_win_left_offset;
	unsigned int def_disp_win_right_offset;
	unsigned int def_disp_win_top_offset;
	unsigned int def_disp_win_bottom_offset;
	unsigned char vui_timing_info_present_flag;
	unsigned int vui_num_units_in_tick;
	unsigned int vui_time_scale;
	unsigned char vui_poc_proportional_to_timing_flag;
	unsigned int vui_num_ticks_poc_diff_one_minus1;
	unsigned char vui_hrd_parameters_present_flag;
	ST_HEVC_HRD_PARAMETERS hrd_parameters;
	unsigned char bitstream_restriction_flag;
	unsigned char tiles_fixed_structure_flag;
	unsigned char motion_vectors_over_pic_boundaries_flag;
	unsigned char restricted_ref_pic_lists_flag;
	unsigned int min_spatial_segmentation_idc;
	unsigned int max_bytes_per_pic_denom;
	unsigned int max_bits_per_min_cu_denom;
	unsigned int log2_max_mv_length_horizontal;
	unsigned int log2_max_mv_length_vertical;
} ST_HEVC_VUI_PARAMETERS;
typedef struct st_HEVC_SPS {
	unsigned char sps_video_parameter_set_id;
	unsigned char sps_max_sub_layers_minus1;
	unsigned char sps_temporal_id_nesting_flag;
	ST_HEVC_SPS_TIER_LEVEL profile_tier_level;
	unsigned int sps_seq_parameter_set_id;
	unsigned int chroma_format_idc;
	unsigned char separate_colour_plane_flag;
	unsigned int pic_width_in_luma_samples;
	unsigned int pic_height_in_luma_samples;
	unsigned char conformance_window_flag;
	unsigned int conf_win_left_offset;
	unsigned int conf_win_right_offset;
	unsigned int conf_win_top_offset;
	unsigned int conf_win_bottom_offset;
	unsigned int bit_depth_luma_minus8;
	unsigned int bit_depth_chroma_minus8;
	unsigned int log2_max_pic_order_cnt_lsb_minus4;
	unsigned char sps_sub_layer_ordering_info_present_flag;
	vector<unsigned int> sps_max_dec_pic_buffering_minus1;
	vector<unsigned int> sps_max_num_reorder_pics;
	vector<unsigned int> sps_max_latency_increase_plus1;
	unsigned int log2_min_luma_coding_block_size_minus3;
	unsigned int log2_diff_max_min_luma_coding_block_size;
	unsigned int log2_min_transform_block_size_minus2;
	unsigned int log2_diff_max_min_transform_block_size;
	unsigned int max_transform_hierarchy_depth_inter;
	unsigned int max_transform_hierarchy_depth_intra;
	unsigned char scaling_list_enabled_flag;
	unsigned char sps_scaling_list_data_present_flag;
	ST_HEVC_SCALING_LIST_DATA scaling_list_data;
	unsigned char amp_enabled_flag;
	unsigned char sample_adaptive_offset_enabled_flag;
	unsigned char pcm_enabled_flag;
	unsigned char pcm_sample_bit_depth_luma_minus1;
	unsigned char pcm_sample_bit_depth_chroma_minus1;
	unsigned int log2_min_pcm_luma_coding_block_size_minus3;
	unsigned int log2_diff_max_min_pcm_luma_coding_block_size;
	unsigned char pcm_loop_filter_disabled_flag;
	unsigned int num_short_term_ref_pic_sets;
	vector<ST_HEVC_SHORT_TERM_REF_PIC_SET> short_term_ref_pic_set;
	unsigned char long_term_ref_pics_present_flag;
	unsigned int num_long_term_ref_pics_sps;
	vector<unsigned int> lt_ref_pic_poc_lsb_sps;
	vector<unsigned char> used_by_curr_pic_lt_sps_flag;
	unsigned char sps_temporal_mvp_enabled_flag;
	unsigned char strong_intra_smoothing_enabled_flag;
	unsigned char vui_parameters_present_flag;
	ST_HEVC_VUI_PARAMETERS vui_parameters;
	unsigned char sps_extension_flag;
} ST_HEVC_SPS;

void HevcParseSPS(ST_HEVC_SPS *pstSps, unsigned char *pucStream,
		  unsigned int uiLen);

unsigned char *HevcAddHdrInfoToSps(ST_HEVC_SPS *pstSps,
				   unsigned int *puiStreamLen);

unsigned char *hevc_add_hdr_to_sps(ST_HEVC_SPS *pstSps,
				   unsigned int *puiStreamLen);

#endif
