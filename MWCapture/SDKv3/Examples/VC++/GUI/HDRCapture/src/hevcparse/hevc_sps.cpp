#include <string.h>
#include <stdlib.h>
#include "hevc_sps.h"
#include "hevc_bitstream.h"

int g_n_common_inf_preset_flag = 1;

void HevcParseProfileTierLevel(int max_sub_layers_minus1,
			       ST_HEVC_BITSTREAMER *pstBs,
			       ST_HEVC_SPS_TIER_LEVEL *pstPtl)
{
	int i, j;
	pstPtl->general_profile_space = HevcGetBits(pstBs, 2);
	pstPtl->general_tier_flag = HevcGetBits(pstBs, 1);
	pstPtl->general_profile_idc = HevcGetBits(pstBs, 5);

	for (i = 0; i < 32; i++) {
		pstPtl->general_profile_compatibility_flag[i] =
			HevcGetBits(pstBs, 1);
	}

	pstPtl->general_progressive_source_flag = HevcGetBits(pstBs, 1);
	pstPtl->general_interlaced_source_flag = HevcGetBits(pstBs, 1);
	pstPtl->general_non_packed_constraint_flag = HevcGetBits(pstBs, 1);
	pstPtl->general_frame_only_constraint_flag = HevcGetBits(pstBs, 1);
	HevcGetBits(pstBs, 32);
	HevcGetBits(pstBs, 12);
	pstPtl->general_level_idc = HevcGetBits(pstBs, 8);

	pstPtl->sub_layer_profile_present_flag.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_level_present_flag.resize(max_sub_layers_minus1);

	for (i = 0; i < max_sub_layers_minus1; i++) {
		pstPtl->sub_layer_profile_present_flag[i] =
			HevcGetBits(pstBs, 1);
		pstPtl->sub_layer_level_present_flag[i] = HevcGetBits(pstBs, 1);
	}

	if (max_sub_layers_minus1 > 0) {
		for (i = max_sub_layers_minus1; i < 8; i++) {
			HevcGetBits(pstBs, 2);
		}
	}

	pstPtl->sub_layer_profile_space.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_tier_flag.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_profile_idc.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_profile_compatibility_flag.resize(
		max_sub_layers_minus1);
	pstPtl->sub_layer_progressive_source_flag.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_interlaced_source_flag.resize(max_sub_layers_minus1);
	pstPtl->sub_layer_non_packed_constraint_flag.resize(
		max_sub_layers_minus1);
	pstPtl->sub_layer_frame_only_constraint_flag.resize(
		max_sub_layers_minus1);
	pstPtl->sub_layer_level_idc.resize(max_sub_layers_minus1);

	for (i = 0; i < max_sub_layers_minus1; i++) {
		if (pstPtl->sub_layer_profile_present_flag[i]) {
			pstPtl->sub_layer_profile_space[i] =
				HevcGetBits(pstBs, 2);
			pstPtl->sub_layer_tier_flag[i] = HevcGetBits(pstBs, 1);
			pstPtl->sub_layer_profile_idc[i] =
				HevcGetBits(pstBs, 5);
			pstPtl->sub_layer_profile_compatibility_flag[i].resize(
				32);

			for (j = 0; j < 32; j++) {
				pstPtl->sub_layer_profile_compatibility_flag[i]
									    [j] =
					HevcGetBits(pstBs, 1);
			}
			pstPtl->sub_layer_progressive_source_flag[i] =
				HevcGetBits(pstBs, 1);
			pstPtl->sub_layer_interlaced_source_flag[i] =
				HevcGetBits(pstBs, 1);
			pstPtl->sub_layer_non_packed_constraint_flag[i] =
				HevcGetBits(pstBs, 1);
			pstPtl->sub_layer_frame_only_constraint_flag[i] =
				HevcGetBits(pstBs, 1);
			HevcGetBits(pstBs, 32);
			HevcGetBits(pstBs, 12);
		}

		if (pstPtl->sub_layer_level_present_flag[i]) {
			pstPtl->sub_layer_level_idc[i] = HevcGetBits(pstBs, 8);
		} else {
			pstPtl->sub_layer_level_idc[i] = 1;
		}
	}
}
void HevcParseScalingListData(ST_HEVC_BITSTREAMER *pstBs,
			      ST_HEVC_SCALING_LIST_DATA *pstScld)
{
	int iSizeId, iMatrixId;

	pstScld->scaling_list_pred_mode_flag.resize(4);
	pstScld->scaling_list_pred_matrix_id_delta.resize(4);
	pstScld->scaling_list_dc_coef_minus8.resize(2);
	pstScld->scaling_list_delta_coef.resize(4);

	for (iSizeId = 0; iSizeId < 4; iSizeId++) {
		if (iSizeId == 3) {
			pstScld->scaling_list_pred_mode_flag[iSizeId].resize(2);
			pstScld->scaling_list_pred_matrix_id_delta[iSizeId]
				.resize(2);
			pstScld->scaling_list_dc_coef_minus8[iSizeId - 2].resize(
				2);
			pstScld->scaling_list_delta_coef[iSizeId].resize(2);
		} else {
			pstScld->scaling_list_pred_mode_flag[iSizeId].resize(6);
			pstScld->scaling_list_pred_matrix_id_delta[iSizeId]
				.resize(6);
			pstScld->scaling_list_delta_coef[iSizeId].resize(6);
			if (iSizeId >= 2) {
				pstScld->scaling_list_dc_coef_minus8[iSizeId - 2]
					.resize(6);
			}
		}

		for (iMatrixId = 0; iMatrixId < ((iSizeId == 3) ? 2 : 6);
		     iMatrixId++) {
			pstScld->scaling_list_pred_mode_flag[iSizeId][iMatrixId] =
				HevcGetBits(pstBs, 1);
			if (!pstScld->scaling_list_pred_mode_flag[iSizeId]
								 [iMatrixId]) {
				pstScld->scaling_list_pred_matrix_id_delta
					[iSizeId][iMatrixId] =
					HevcGetGolombU(pstBs);
			} else {
				int i;
				int iCoefNum = 1 << (4 + (iSizeId << 1));
				if (iCoefNum > 64) {
					iCoefNum = 64;
				}
				if (iSizeId > 1) {
					pstScld->scaling_list_dc_coef_minus8
						[iSizeId - 2][iMatrixId] =
						HevcGetGolombS(pstBs);
				}
				pstScld
					->scaling_list_delta_coef[iSizeId]
								 [iMatrixId]
					.resize(iCoefNum);
				for (i = 0; i < iCoefNum; i++) {
					pstScld->scaling_list_delta_coef
						[iSizeId][iMatrixId][i] =
						HevcGetGolombS(pstBs);
				}
			}
		}
	}
}

void HevcParseShortTermRefPicSet(int iRpsIdx, int num_short_term_ref_pic_sets,
				 ST_HEVC_SPS *pstSps,
				 ST_HEVC_BITSTREAMER *pstBs)
{
	unsigned int i;
	ST_HEVC_SHORT_TERM_REF_PIC_SET *pstNowRps =
		&(pstSps->short_term_ref_pic_set[iRpsIdx]);

	pstNowRps->inter_ref_pic_set_prediction_flag = 0;
	pstNowRps->delta_idx_minus1 = 0;
	if (iRpsIdx) {
		pstNowRps->inter_ref_pic_set_prediction_flag =
			HevcGetBits(pstBs, 1);
	}

	if (pstNowRps->inter_ref_pic_set_prediction_flag) {
		int iRefRpsIdx;
		unsigned int iNumDeltaPocs = 0;
		ST_HEVC_SHORT_TERM_REF_PIC_SET *pstRefRps;
		if (iRpsIdx == num_short_term_ref_pic_sets) {
			pstNowRps->delta_idx_minus1 = HevcGetGolombU(pstBs);
		}

		pstNowRps->delta_rps_sign = HevcGetBits(pstBs, 1);
		pstNowRps->abs_delta_rps_minus1 = HevcGetGolombU(pstBs);

		iRefRpsIdx = iRpsIdx - (pstNowRps->delta_idx_minus1 + 1);
		pstRefRps = &(pstSps->short_term_ref_pic_set[iRefRpsIdx]);
		if (pstRefRps->inter_ref_pic_set_prediction_flag) {
			for (i = 0; i < pstRefRps->used_by_curr_pic_flag.size();
			     i++) {
				if (pstRefRps->used_by_curr_pic_flag[i] ||
				    pstRefRps->use_delta_flag[i]) {
					iNumDeltaPocs++;
				}
			}
		} else {
			iNumDeltaPocs = pstRefRps->num_negative_pics +
					pstRefRps->num_positive_pics;
		}

		pstNowRps->used_by_curr_pic_flag.resize(iNumDeltaPocs + 1);
		pstNowRps->use_delta_flag.resize(iNumDeltaPocs + 1, 1);

		for (i = 0; i <= iNumDeltaPocs; i++) {
			pstNowRps->used_by_curr_pic_flag[i] =
				HevcGetBits(pstBs, 1);
			if (!pstNowRps->used_by_curr_pic_flag[i]) {
				pstNowRps->use_delta_flag[i] =
					HevcGetBits(pstBs, 1);
			}
		}
	} else {
		pstNowRps->num_negative_pics = HevcGetGolombU(pstBs);
		pstNowRps->num_positive_pics = HevcGetGolombU(pstBs);

		if (pstNowRps->num_negative_pics >
		    pstSps->sps_max_dec_pic_buffering_minus1
			    [pstSps->sps_max_sub_layers_minus1]) {
			printf("ShortTermRefPicSet: num_negative_pics > sps_max_dec_pic_buffering_minus1");
			return;
		}

		if (pstNowRps->num_positive_pics >
		    pstSps->sps_max_dec_pic_buffering_minus1
			    [pstSps->sps_max_sub_layers_minus1]) {
			printf("ShortTermRefPicSet: num_positive_pics > sps_max_dec_pic_buffering_minus1");
			return;
		}

		pstNowRps->delta_poc_s0_minus1.resize(
			pstNowRps->num_negative_pics);
		pstNowRps->used_by_curr_pic_s0_flag.resize(
			pstNowRps->num_negative_pics);

		for (i = 0; i < pstNowRps->num_negative_pics; i++) {
			pstNowRps->delta_poc_s0_minus1[i] =
				HevcGetGolombU(pstBs);
			pstNowRps->used_by_curr_pic_s0_flag[i] =
				HevcGetBits(pstBs, 1);
		}

		pstNowRps->delta_poc_s1_minus1.resize(
			pstNowRps->num_positive_pics);
		pstNowRps->used_by_curr_pic_s1_flag.resize(
			pstNowRps->num_positive_pics);
		for (i = 0; i < pstNowRps->num_positive_pics; i++) {
			pstNowRps->delta_poc_s1_minus1[i] =
				HevcGetGolombU(pstBs);
			pstNowRps->used_by_curr_pic_s1_flag[i] =
				HevcGetBits(pstBs, 1);
		}
	}
}
void HevcParseSubLayerHrdParameters(int sub_pic_hrd_params_present_flag,
				    unsigned int CpbCnt,
				    ST_HEVC_BITSTREAMER *pstBs,
				    ST_HEVC_SUBLAYER_HRD_PARAMETERS *pstSlhrd)
{
	unsigned int i;
	pstSlhrd->bit_rate_value_minus1.resize(CpbCnt + 1);
	pstSlhrd->cpb_size_value_minus1.resize(CpbCnt + 1);
	pstSlhrd->cpb_size_du_value_minus1.resize(CpbCnt + 1);
	pstSlhrd->bit_rate_du_value_minus1.resize(CpbCnt + 1);
	pstSlhrd->cbr_flag.resize(CpbCnt + 1);

	for (i = 0; i <= CpbCnt; i++) {
		pstSlhrd->bit_rate_value_minus1[i] = HevcGetGolombU(pstBs);
		pstSlhrd->cpb_size_value_minus1[i] = HevcGetGolombU(pstBs);
		if (sub_pic_hrd_params_present_flag) {
			pstSlhrd->cpb_size_du_value_minus1[i] =
				HevcGetGolombU(pstBs);
			pstSlhrd->bit_rate_du_value_minus1[i] =
				HevcGetGolombU(pstBs);
		}
		pstSlhrd->cbr_flag[i] = HevcGetBits(pstBs, 1);
	}
}

void HevcParseHrdParameters(int commonInfPresentFlag, int maxNumSubLayersMinus1,
			    ST_HEVC_BITSTREAMER *pstBs,
			    ST_HEVC_HRD_PARAMETERS *pstHrdp)
{
	int i;
	pstHrdp->nal_hrd_parameters_present_flag = 0;
	pstHrdp->vcl_hrd_parameters_present_flag = 0;
	pstHrdp->sub_pic_hrd_params_present_flag = 0;
	pstHrdp->sub_pic_cpb_params_in_pic_timing_sei_flag = 0;
	if (commonInfPresentFlag) {
		pstHrdp->nal_hrd_parameters_present_flag =
			HevcGetBits(pstBs, 1);
		pstHrdp->vcl_hrd_parameters_present_flag =
			HevcGetBits(pstBs, 1);

		if (pstHrdp->nal_hrd_parameters_present_flag ||
		    pstHrdp->vcl_hrd_parameters_present_flag) {
			pstHrdp->sub_pic_hrd_params_present_flag =
				HevcGetBits(pstBs, 1);
			if (pstHrdp->sub_pic_hrd_params_present_flag) {
				pstHrdp->tick_divisor_minus2 =
					HevcGetBits(pstBs, 8);
				pstHrdp->du_cpb_removal_delay_increment_length_minus1 =
					HevcGetBits(pstBs, 5);
				pstHrdp->sub_pic_cpb_params_in_pic_timing_sei_flag =
					HevcGetBits(pstBs, 1);
				pstHrdp->dpb_output_delay_du_length_minus1 =
					HevcGetBits(pstBs, 5);
			}
			pstHrdp->bit_rate_scale = HevcGetBits(pstBs, 4);
			pstHrdp->cpb_size_scale = HevcGetBits(pstBs, 4);

			if (pstHrdp->sub_pic_hrd_params_present_flag) {
				pstHrdp->cpb_size_du_scale =
					HevcGetBits(pstBs, 4);
			}

			pstHrdp->initial_cpb_removal_delay_length_minus1 =
				HevcGetBits(pstBs, 5);
			pstHrdp->au_cpb_removal_delay_length_minus1 =
				HevcGetBits(pstBs, 5);
			pstHrdp->dpb_output_delay_length_minus1 =
				HevcGetBits(pstBs, 5);
		}
	}

	pstHrdp->fixed_pic_rate_general_flag.resize(maxNumSubLayersMinus1 + 1);
	pstHrdp->fixed_pic_rate_within_cvs_flag.resize(maxNumSubLayersMinus1 +
						       1);
	pstHrdp->elemental_duration_in_tc_minus1.resize(maxNumSubLayersMinus1 +
							1);
	pstHrdp->low_delay_hrd_flag.resize(maxNumSubLayersMinus1 + 1, 0);
	pstHrdp->cpb_cnt_minus1.resize(maxNumSubLayersMinus1 + 1, 0);

	if (pstHrdp->nal_hrd_parameters_present_flag) {
		pstHrdp->nal_sub_layer_hrd_parameters.resize(
			maxNumSubLayersMinus1 + 1);
	}

	if (pstHrdp->vcl_hrd_parameters_present_flag) {
		pstHrdp->vcl_sub_layer_hrd_parameters.resize(
			maxNumSubLayersMinus1 + 1);
	}

	for (i = 0; i <= maxNumSubLayersMinus1; i++) {
		pstHrdp->fixed_pic_rate_general_flag[i] = HevcGetBits(pstBs, 1);

		if (pstHrdp->fixed_pic_rate_general_flag[i]) {
			pstHrdp->fixed_pic_rate_within_cvs_flag[i] = 1;
		}

		if (!pstHrdp->fixed_pic_rate_general_flag[i]) {
			pstHrdp->fixed_pic_rate_within_cvs_flag[i] =
				HevcGetBits(pstBs, 1);
		}

		if (pstHrdp->fixed_pic_rate_within_cvs_flag[i]) {
			pstHrdp->elemental_duration_in_tc_minus1[i] =
				HevcGetGolombU(pstBs);
		} else {
			pstHrdp->low_delay_hrd_flag[i] = HevcGetBits(pstBs, 1);
		}

		if (!pstHrdp->low_delay_hrd_flag[i]) {
			pstHrdp->cpb_cnt_minus1[i] = HevcGetGolombU(pstBs);
		}

		if (pstHrdp->nal_hrd_parameters_present_flag) {
			HevcParseSubLayerHrdParameters(
				pstHrdp->sub_pic_hrd_params_present_flag,
				pstHrdp->cpb_cnt_minus1[i], pstBs,
				&(pstHrdp->nal_sub_layer_hrd_parameters[i]));
		}

		if (pstHrdp->vcl_hrd_parameters_present_flag) {
			HevcParseSubLayerHrdParameters(
				pstHrdp->sub_pic_hrd_params_present_flag,
				pstHrdp->cpb_cnt_minus1[i], pstBs,
				&(pstHrdp->vcl_sub_layer_hrd_parameters[i]));
		}
	}
}

void HevcParseVuiParameters(int sps_max_sub_layers_minus1,
			    ST_HEVC_BITSTREAMER *pstBs,
			    ST_HEVC_VUI_PARAMETERS *pstVui)
{
	pstVui->aspect_ratio_idc = 0;
	pstVui->sar_width = 0;
	pstVui->sar_height = 0;
	pstVui->aspect_ratio_info_present_flag = HevcGetBits(pstBs, 1);

	if (pstVui->aspect_ratio_info_present_flag) {
		pstVui->aspect_ratio_idc = HevcGetBits(pstBs, 8);
		if (pstVui->aspect_ratio_idc == 255) { //EXTENDED_SAR
			pstVui->sar_width = HevcGetBits(pstBs, 16);
			pstVui->sar_height = HevcGetBits(pstBs, 16);
		}
	}

	pstVui->overscan_info_present_flag = HevcGetBits(pstBs, 1);
	if (pstVui->overscan_info_present_flag) {
		pstVui->overscan_appropriate_flag = HevcGetBits(pstBs, 1);
	}
	pstVui->video_format = 5;
	pstVui->video_full_range_flag = 0;
	pstVui->colour_primaries = 2;
	pstVui->transfer_characteristics = 2;
	pstVui->matrix_coeffs = 2;
	pstVui->video_signal_type_present_flag = HevcGetBits(pstBs, 1);

	if (pstVui->video_signal_type_present_flag) {
		pstVui->video_format = HevcGetBits(pstBs, 3);
		pstVui->video_full_range_flag = HevcGetBits(pstBs, 1);
		pstVui->colour_description_present_flag = HevcGetBits(pstBs, 1);

		if (pstVui->colour_description_present_flag) {
			pstVui->colour_primaries = HevcGetBits(pstBs, 8);
			pstVui->transfer_characteristics =
				HevcGetBits(pstBs, 8);
			pstVui->matrix_coeffs = HevcGetBits(pstBs, 8);
		}
	}
	pstVui->chroma_sample_loc_type_top_field = 0;
	pstVui->chroma_sample_loc_type_bottom_field = 0;

	pstVui->chroma_loc_info_present_flag = HevcGetBits(pstBs, 1);
	if (pstVui->chroma_loc_info_present_flag) {
		pstVui->chroma_sample_loc_type_top_field =
			HevcGetGolombU(pstBs);
		pstVui->chroma_sample_loc_type_bottom_field =
			HevcGetGolombU(pstBs);
	}
	pstVui->neutral_chroma_indication_flag = HevcGetBits(pstBs, 1);
	pstVui->field_seq_flag = HevcGetBits(pstBs, 1);
	pstVui->frame_field_info_present_flag = HevcGetBits(pstBs, 1);
	pstVui->default_display_window_flag = HevcGetBits(pstBs, 1);

	pstVui->def_disp_win_left_offset = 0;
	pstVui->def_disp_win_right_offset = 0;
	pstVui->def_disp_win_right_offset = 0;
	pstVui->def_disp_win_bottom_offset = 0;

	if (pstVui->default_display_window_flag) {
		pstVui->def_disp_win_left_offset = HevcGetGolombU(pstBs);
		pstVui->def_disp_win_right_offset = HevcGetGolombU(pstBs);
		pstVui->def_disp_win_top_offset = HevcGetGolombU(pstBs);
		pstVui->def_disp_win_bottom_offset = HevcGetGolombU(pstBs);
	}

	pstVui->vui_timing_info_present_flag = HevcGetBits(pstBs, 1);

	if (pstVui->vui_timing_info_present_flag) {
		pstVui->vui_num_units_in_tick = HevcGetBits(pstBs, 32);
		pstVui->vui_time_scale = HevcGetBits(pstBs, 32);
		pstVui->vui_poc_proportional_to_timing_flag =
			HevcGetBits(pstBs, 1);

		if (pstVui->vui_poc_proportional_to_timing_flag) {
			pstVui->vui_num_ticks_poc_diff_one_minus1 =
				HevcGetGolombU(pstBs);
		}
		pstVui->vui_hrd_parameters_present_flag = HevcGetBits(pstBs, 1);
		if (pstVui->vui_hrd_parameters_present_flag) {
			HevcParseHrdParameters(g_n_common_inf_preset_flag,
					       sps_max_sub_layers_minus1, pstBs,
					       &(pstVui->hrd_parameters));
		}
	}
	pstVui->bitstream_restriction_flag = HevcGetBits(pstBs, 1);

	if (pstVui->bitstream_restriction_flag) {
		pstVui->tiles_fixed_structure_flag = HevcGetBits(pstBs, 1);
		pstVui->motion_vectors_over_pic_boundaries_flag =
			HevcGetBits(pstBs, 1);
		pstVui->restricted_ref_pic_lists_flag = HevcGetBits(pstBs, 1);
		pstVui->min_spatial_segmentation_idc = HevcGetGolombU(pstBs);
		pstVui->max_bytes_per_pic_denom = HevcGetGolombU(pstBs);
		pstVui->max_bits_per_min_cu_denom = HevcGetGolombU(pstBs);
		pstVui->log2_max_mv_length_horizontal = HevcGetGolombU(pstBs);
		pstVui->log2_max_mv_length_vertical = HevcGetGolombU(pstBs);
	}
}

void HevcParseSPS(ST_HEVC_SPS *pstSps, unsigned char *pucStream,
		  unsigned int uiLen)
{
	ST_HEVC_BITSTREAMER stBs;
	unsigned int i;
	HevcBsInitRead(&stBs, pucStream, uiLen, true);
	pstSps->sps_video_parameter_set_id = HevcGetBits(&stBs, 4);
	pstSps->sps_max_sub_layers_minus1 = HevcGetBits(&stBs, 3);
	pstSps->sps_temporal_id_nesting_flag = HevcGetBits(&stBs, 1);
	HevcParseProfileTierLevel(pstSps->sps_max_sub_layers_minus1, &stBs,
				  &(pstSps->profile_tier_level));

	pstSps->sps_seq_parameter_set_id = HevcGetGolombU(&stBs);
	pstSps->chroma_format_idc = HevcGetGolombU(&stBs);

	if (pstSps->chroma_format_idc == 3) {
		pstSps->separate_colour_plane_flag = HevcGetBits(&stBs, 1);
	} else {
		pstSps->separate_colour_plane_flag = 0;
	}

	pstSps->pic_width_in_luma_samples = HevcGetGolombU(&stBs);
	pstSps->pic_height_in_luma_samples = HevcGetGolombU(&stBs);
	pstSps->conformance_window_flag = HevcGetBits(&stBs, 1);

	if (pstSps->conformance_window_flag) {
		pstSps->conf_win_left_offset = HevcGetGolombU(&stBs);
		pstSps->conf_win_right_offset = HevcGetGolombU(&stBs);
		pstSps->conf_win_top_offset = HevcGetGolombU(&stBs);
		pstSps->conf_win_bottom_offset = HevcGetGolombU(&stBs);
	}

	pstSps->bit_depth_luma_minus8 = HevcGetGolombU(&stBs);
	pstSps->bit_depth_chroma_minus8 = HevcGetGolombU(&stBs);
	pstSps->log2_max_pic_order_cnt_lsb_minus4 = HevcGetGolombU(&stBs);
	pstSps->sps_sub_layer_ordering_info_present_flag =
		HevcGetBits(&stBs, 1);

	pstSps->sps_max_dec_pic_buffering_minus1.resize(
		pstSps->sps_max_sub_layers_minus1 + 1, 0);
	pstSps->sps_max_num_reorder_pics.resize(
		pstSps->sps_max_sub_layers_minus1 + 1, 0);
	pstSps->sps_max_latency_increase_plus1.resize(
		pstSps->sps_max_sub_layers_minus1 + 1, 0);

	i = pstSps->sps_sub_layer_ordering_info_present_flag
		    ? 0
		    : pstSps->sps_max_sub_layers_minus1;
	for (; i <= pstSps->sps_max_sub_layers_minus1; i++) {
		pstSps->sps_max_dec_pic_buffering_minus1[i] =
			HevcGetGolombU(&stBs);
		pstSps->sps_max_num_reorder_pics[i] = HevcGetGolombU(&stBs);
		pstSps->sps_max_latency_increase_plus1[i] =
			HevcGetGolombU(&stBs);
	}

	pstSps->log2_min_luma_coding_block_size_minus3 = HevcGetGolombU(&stBs);
	pstSps->log2_diff_max_min_luma_coding_block_size =
		HevcGetGolombU(&stBs);
	pstSps->log2_min_transform_block_size_minus2 = HevcGetGolombU(&stBs);
	pstSps->log2_diff_max_min_transform_block_size = HevcGetGolombU(&stBs);
	pstSps->max_transform_hierarchy_depth_inter = HevcGetGolombU(&stBs);
	pstSps->max_transform_hierarchy_depth_intra = HevcGetGolombU(&stBs);

	pstSps->scaling_list_enabled_flag = HevcGetBits(&stBs, 1);
	if (pstSps->scaling_list_enabled_flag) {
		pstSps->sps_scaling_list_data_present_flag =
			HevcGetBits(&stBs, 1);
		if (pstSps->sps_scaling_list_data_present_flag) {
			HevcParseScalingListData(&stBs,
						 &(pstSps->scaling_list_data));
		}
	}

	pstSps->amp_enabled_flag = HevcGetBits(&stBs, 1);
	pstSps->sample_adaptive_offset_enabled_flag = HevcGetBits(&stBs, 1);
	pstSps->pcm_enabled_flag = HevcGetBits(&stBs, 1);

	if (pstSps->pcm_enabled_flag) {
		pstSps->pcm_sample_bit_depth_luma_minus1 =
			HevcGetBits(&stBs, 4);
		pstSps->pcm_sample_bit_depth_chroma_minus1 =
			HevcGetBits(&stBs, 4);
		pstSps->log2_min_pcm_luma_coding_block_size_minus3 =
			HevcGetGolombU(&stBs);
		pstSps->log2_diff_max_min_pcm_luma_coding_block_size =
			HevcGetGolombU(&stBs);
		pstSps->pcm_loop_filter_disabled_flag = HevcGetBits(&stBs, 1);
	}

	pstSps->num_short_term_ref_pic_sets = HevcGetGolombU(&stBs);

	pstSps->short_term_ref_pic_set.resize(
		pstSps->num_short_term_ref_pic_sets);
	for (i = 0; i < pstSps->num_short_term_ref_pic_sets; i++) {
		HevcParseShortTermRefPicSet(
			i, pstSps->num_short_term_ref_pic_sets, pstSps, &stBs);
	}
	pstSps->long_term_ref_pics_present_flag = HevcGetBits(&stBs, 1);
	if (pstSps->long_term_ref_pics_present_flag) {
		pstSps->num_long_term_ref_pics_sps = HevcGetGolombU(&stBs);
		pstSps->lt_ref_pic_poc_lsb_sps.resize(
			pstSps->num_long_term_ref_pics_sps);
		pstSps->used_by_curr_pic_lt_sps_flag.resize(
			pstSps->num_long_term_ref_pics_sps);

		for (i = 0; i < pstSps->num_long_term_ref_pics_sps; i++) {
			pstSps->lt_ref_pic_poc_lsb_sps[i] = HevcGetBits(
				&stBs,
				pstSps->log2_max_pic_order_cnt_lsb_minus4 + 4);
			pstSps->used_by_curr_pic_lt_sps_flag[i] =
				HevcGetBits(&stBs, 1);
		}
	}

	pstSps->sps_temporal_mvp_enabled_flag = HevcGetBits(&stBs, 1);
	pstSps->strong_intra_smoothing_enabled_flag = HevcGetBits(&stBs, 1);
	pstSps->vui_parameters_present_flag = HevcGetBits(&stBs, 1);

	if (pstSps->vui_parameters_present_flag) {
		HevcParseVuiParameters(pstSps->sps_max_sub_layers_minus1, &stBs,
				       &(pstSps->vui_parameters));
	}
	pstSps->sps_extension_flag = HevcGetBits(&stBs, 1);
}

unsigned char *HevcAddHdrInfoToSps(ST_HEVC_SPS *pstSps,
				   unsigned int *puiStreamLen)
{
	unsigned int i;
	int uiLen;
	unsigned char aucTemp[100];
	unsigned char *pstStream;

	ST_HEVC_BITSTREAMER stBs;
	memset(aucTemp, 0, 100);
	HevcBsInitWrite(&stBs, aucTemp, 100);
	HevcWriteWord(&stBs, 4, pstSps->sps_video_parameter_set_id);
	HevcWriteWord(&stBs, 3, pstSps->sps_max_sub_layers_minus1);
	HevcWriteWord(&stBs, 1, pstSps->sps_temporal_id_nesting_flag);

	HevcWriteWord(&stBs, 2,
		      pstSps->profile_tier_level.general_profile_space);
	HevcWriteWord(&stBs, 1, pstSps->profile_tier_level.general_tier_flag);
	HevcWriteWord(&stBs, 5, pstSps->profile_tier_level.general_profile_idc);

	for (int i = 0; i < 32; i++) {
		HevcWriteWord(&stBs, 1,
			      pstSps->profile_tier_level
				      .general_profile_compatibility_flag[i]);
	}

	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_progressive_source_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_interlaced_source_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_non_packed_constraint_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_frame_only_constraint_flag);

	HevcWriteWord(&stBs, 44, 0);
	HevcWriteWord(&stBs, 8, pstSps->profile_tier_level.general_level_idc);
	HevcWriteUE(&stBs, pstSps->sps_seq_parameter_set_id);
	HevcWriteUE(&stBs, pstSps->chroma_format_idc);
	if (pstSps->chroma_format_idc == 3) {
		HevcWriteWord(&stBs, 1, pstSps->separate_colour_plane_flag);
	}

	HevcWriteUE(&stBs, pstSps->pic_width_in_luma_samples);
	HevcWriteUE(&stBs, pstSps->pic_height_in_luma_samples);

	HevcWriteWord(&stBs, 1, pstSps->conformance_window_flag);
	if (pstSps->conformance_window_flag) {
		HevcWriteUE(&stBs, pstSps->conf_win_left_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_right_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_top_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_bottom_offset);
	}

	HevcWriteUE(&stBs, pstSps->bit_depth_luma_minus8);
	HevcWriteUE(&stBs, pstSps->bit_depth_chroma_minus8);
	HevcWriteUE(&stBs, pstSps->log2_max_pic_order_cnt_lsb_minus4);

	HevcWriteWord(&stBs, 1,
		      pstSps->sps_sub_layer_ordering_info_present_flag);

	i = pstSps->sps_sub_layer_ordering_info_present_flag
		    ? 0
		    : pstSps->sps_max_sub_layers_minus1;
	for (; i <= pstSps->sps_max_sub_layers_minus1; i++) {
		HevcWriteUE(&stBs, pstSps->sps_max_dec_pic_buffering_minus1[i]);
		HevcWriteUE(&stBs, pstSps->sps_max_num_reorder_pics[i]);
		HevcWriteUE(&stBs, pstSps->sps_max_latency_increase_plus1[i]);
	}
	/*if (pstSps->sps_sub_layer_ordering_info_present_flag){
        HevcWriteUE(&stBs,  pstSps->sps_max_dec_pic_buffering_minus1[0]);
        HevcWriteUE(&stBs,  pstSps->sps_max_num_reorder_pics[0]);
        HevcWriteUE(&stBs,  pstSps->sps_max_latency_increase_plus1[0]);
    }*/

	HevcWriteUE(&stBs, pstSps->log2_min_luma_coding_block_size_minus3);
	HevcWriteUE(&stBs, pstSps->log2_diff_max_min_luma_coding_block_size);
	HevcWriteUE(&stBs, pstSps->log2_min_transform_block_size_minus2);
	HevcWriteUE(&stBs, pstSps->log2_diff_max_min_transform_block_size);
	HevcWriteUE(&stBs, pstSps->max_transform_hierarchy_depth_inter);
	HevcWriteUE(&stBs, pstSps->max_transform_hierarchy_depth_intra);

	HevcWriteWord(&stBs, 1, pstSps->scaling_list_enabled_flag);

	HevcWriteWord(&stBs, 1, pstSps->amp_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->sample_adaptive_offset_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->pcm_enabled_flag);
	if (pstSps->pcm_enabled_flag) {
		HevcWriteWord(&stBs, 4,
			      pstSps->pcm_sample_bit_depth_luma_minus1);
		HevcWriteWord(&stBs, 4,
			      pstSps->pcm_sample_bit_depth_chroma_minus1);
		HevcWriteUE(&stBs,
			    pstSps->log2_min_pcm_luma_coding_block_size_minus3);
		HevcWriteUE(
			&stBs,
			pstSps->log2_diff_max_min_pcm_luma_coding_block_size);
		HevcWriteWord(&stBs, 1, pstSps->pcm_loop_filter_disabled_flag);
	}

	HevcWriteUE(&stBs, pstSps->num_short_term_ref_pic_sets);
	if (pstSps->num_short_term_ref_pic_sets) {
		for (int j = 0; j < pstSps->num_short_term_ref_pic_sets; j++) {
			if (j) {
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.inter_ref_pic_set_prediction_flag);
			}

			//if (!pstSps->short_term_ref_pic_set[0].inter_ref_pic_set_prediction_flag)
			//{
			HevcWriteUE(&stBs, pstSps->short_term_ref_pic_set[j]
						   .num_negative_pics);
			HevcWriteUE(&stBs, pstSps->short_term_ref_pic_set[j]
						   .num_positive_pics);
			for (int i = 0; i < pstSps->short_term_ref_pic_set[j]
						    .num_negative_pics;
			     i++) {
				HevcWriteUE(&stBs,
					    pstSps->short_term_ref_pic_set[j]
						    .delta_poc_s0_minus1[i]);
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.used_by_curr_pic_s0_flag[i]);
			}

			//             for (int i = 0; i < pstSps->short_term_ref_pic_set[0].num_negative_pics; i++){
			//                 HevcWriteWord(&stBs, 1, pstSps->short_term_ref_pic_set[0].used_by_curr_pic_s0_flag[i]);
			//             }

			for (int i = 0; i < pstSps->short_term_ref_pic_set[j]
						    .num_positive_pics;
			     i++) {
				HevcWriteUE(&stBs,
					    pstSps->short_term_ref_pic_set[j]
						    .delta_poc_s1_minus1[i]);
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.used_by_curr_pic_s1_flag[i]);
			}

			//             for (int i = 0; i < pstSps->short_term_ref_pic_set[0].num_positive_pics; i++){
			//                 HevcWriteWord(&stBs, 1, pstSps->short_term_ref_pic_set[0].used_by_curr_pic_s1_flag[i]);
			//             }
			//}
		}
	}

	HevcWriteWord(&stBs, 1, pstSps->long_term_ref_pics_present_flag);
	if (pstSps->long_term_ref_pics_present_flag) {
		HevcWriteUE(&stBs, pstSps->num_long_term_ref_pics_sps);
	}

	HevcWriteWord(&stBs, 1, pstSps->sps_temporal_mvp_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->strong_intra_smoothing_enabled_flag);

	// start write vui
	HevcWriteWord(
		&stBs, 1,
		1); //HevcWriteWord(&stBs, 1, pstSps->vui_parameters_present_flag);

	/*if (pstSps->vui_parameters_present_flag)*/ {
		//         pstSps->vui_parameters.aspect_ratio_info_present_flag = 1;//fix
		//         pstSps->vui_parameters.aspect_ratio_idc = 1;//fix
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.aspect_ratio_info_present_flag);
		if (pstSps->vui_parameters.aspect_ratio_info_present_flag) {
			HevcWriteWord(&stBs, 8,
				      pstSps->vui_parameters.aspect_ratio_idc);
			if (pstSps->vui_parameters.aspect_ratio_idc == 255) {
				HevcWriteWord(&stBs, 16,
					      pstSps->vui_parameters.sar_width);
				HevcWriteWord(
					&stBs, 16,
					pstSps->vui_parameters.sar_height);
			}
		}

		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.overscan_info_present_flag);
		if (pstSps->vui_parameters.overscan_info_present_flag) {
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .overscan_appropriate_flag);
		}

		/*if (pstSps->vui_parameters.video_signal_type_present_flag == 0)*/ {
			HevcWriteWord(
				&stBs, 1,
				1); // set video_signal_type_present_flag = 1;
			HevcWriteWord(
				&stBs, 3,
				5); // set pstSps->vui_parameters.video_format
			HevcWriteWord(
				&stBs, 1,
				0); // set pstSps->vui_parameters.video_full_range_flag 0

			/* if (pstSps->vui_parameters.colour_description_present_flag == 0)*/
			{
				HevcWriteWord(
					&stBs, 1,
					1); // set pstSps->vui_parameters.colour_description_present_flag 1
				HevcWriteWord(
					&stBs, 8,
					2); // set pstSps->vui_parameters.colour_primaries 2
				HevcWriteWord(
					&stBs, 8,
					16); // set pstSps->vui_parameters.transfer_characteristics 16
				HevcWriteWord(
					&stBs, 8,
					2); // set pstSps->vui_parameters.matrix_coeffs 2
			}
		}

		/*if (pstSps->vui_parameters.chroma_loc_info_present_flag == 0)*/ {
			HevcWriteWord(
				&stBs, 1,
				1); // set pstSps->vui_parameters.chroma_loc_info_present_flag 1
			HevcWriteUE(
				&stBs,
				2); // set vui_parameters.chroma_sample_loc_type_top_field 0
			HevcWriteUE(
				&stBs,
				2); // set pstSps->vui_parameters.chroma_sample_loc_type_bottom_field 0
		}

		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.neutral_chroma_indication_flag);
		HevcWriteWord(&stBs, 1, pstSps->vui_parameters.field_seq_flag);
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.frame_field_info_present_flag);
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.default_display_window_flag);
		if (pstSps->vui_parameters.default_display_window_flag) {
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_left_offset);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_right_offset);
			HevcWriteUE(
				&stBs,
				pstSps->vui_parameters.def_disp_win_top_offset);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_bottom_offset);
		}

		//int aa = pstSps->vui_parameters.vui_timing_info_present_flag;
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.vui_timing_info_present_flag);
		if (pstSps->vui_parameters.vui_timing_info_present_flag) {
			HevcWriteWord(
				&stBs, 32,
				pstSps->vui_parameters.vui_num_units_in_tick);
			HevcWriteWord(&stBs, 32,
				      pstSps->vui_parameters.vui_time_scale);
			HevcWriteWord(
				&stBs, 1,
				pstSps->vui_parameters
					.vui_poc_proportional_to_timing_flag);
			if (pstSps->vui_parameters
				    .vui_poc_proportional_to_timing_flag) {
				HevcWriteUE(
					&stBs,
					pstSps->vui_parameters
						.vui_num_ticks_poc_diff_one_minus1);
			}
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .vui_hrd_parameters_present_flag);
		}

		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.bitstream_restriction_flag);
		if (pstSps->vui_parameters.bitstream_restriction_flag) {
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .tiles_fixed_structure_flag);
			HevcWriteWord(
				&stBs, 1,
				pstSps->vui_parameters
					.motion_vectors_over_pic_boundaries_flag);
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .restricted_ref_pic_lists_flag);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .min_spatial_segmentation_idc);
			HevcWriteUE(
				&stBs,
				pstSps->vui_parameters.max_bytes_per_pic_denom);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .max_bits_per_min_cu_denom);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .log2_max_mv_length_horizontal);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .log2_max_mv_length_vertical);
		}
	}

	HevcWriteWord(&stBs, 1, pstSps->sps_extension_flag);
	HevcWriteWord(&stBs, 1, 1);
	/*if (pstSps->sps_extension_flag)
    {
    HevcWriteWord(&stBs, 1, pstSps->sps_range_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_multilayer_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_3d_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_extension_5bits);
    }*/
	uiLen = (HevcGetBsBits(&stBs) + 7) / 8;
	pstStream = (unsigned char *)malloc(uiLen + 20);
	*puiStreamLen = uiLen + 20;
	HevcReplace(aucTemp, uiLen, pstStream, puiStreamLen);
	return pstStream;
}

void hevc_add_hrd_sublayer_to_sps(ST_HEVC_SUBLAYER_HRD_PARAMETERS *pstSlhrd,
				  ST_HEVC_BITSTREAMER *t_p_st_bs,
				  int sub_pic_hrd_params_present_flag,
				  unsigned int CpbCnt);
void hevc_add_hrd_to_sps(ST_HEVC_SPS *t_p_stSps,
			 ST_HEVC_BITSTREAMER *t_p_st_bs);

unsigned char *hevc_add_hdr_to_sps(ST_HEVC_SPS *pstSps,
				   unsigned int *puiStreamLen)
{
	unsigned int i;
	int uiLen;
	unsigned char aucTemp[100];
	unsigned char *pstStream;

	ST_HEVC_BITSTREAMER stBs;
	memset(aucTemp, 0, 100);
	HevcBsInitWrite(&stBs, aucTemp, 100);
	HevcWriteWord(&stBs, 4, pstSps->sps_video_parameter_set_id);
	HevcWriteWord(&stBs, 3, pstSps->sps_max_sub_layers_minus1);
	HevcWriteWord(&stBs, 1, pstSps->sps_temporal_id_nesting_flag);

	HevcWriteWord(&stBs, 2,
		      pstSps->profile_tier_level.general_profile_space);
	HevcWriteWord(&stBs, 1, pstSps->profile_tier_level.general_tier_flag);
	HevcWriteWord(&stBs, 5, pstSps->profile_tier_level.general_profile_idc);

	for (int i = 0; i < 32; i++) {
		HevcWriteWord(&stBs, 1,
			      pstSps->profile_tier_level
				      .general_profile_compatibility_flag[i]);
	}

	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_progressive_source_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_interlaced_source_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_non_packed_constraint_flag);
	HevcWriteWord(
		&stBs, 1,
		pstSps->profile_tier_level.general_frame_only_constraint_flag);

	HevcWriteWord(&stBs, 44, 0);
	HevcWriteWord(&stBs, 8, pstSps->profile_tier_level.general_level_idc);
	HevcWriteUE(&stBs, pstSps->sps_seq_parameter_set_id);
	HevcWriteUE(&stBs, pstSps->chroma_format_idc);
	if (pstSps->chroma_format_idc == 3) {
		HevcWriteWord(&stBs, 1, pstSps->separate_colour_plane_flag);
	}

	HevcWriteUE(&stBs, pstSps->pic_width_in_luma_samples);
	HevcWriteUE(&stBs, pstSps->pic_height_in_luma_samples);

	HevcWriteWord(&stBs, 1, pstSps->conformance_window_flag);
	if (pstSps->conformance_window_flag) {
		HevcWriteUE(&stBs, pstSps->conf_win_left_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_right_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_top_offset);
		HevcWriteUE(&stBs, pstSps->conf_win_bottom_offset);
	}

	HevcWriteUE(&stBs, pstSps->bit_depth_luma_minus8);
	HevcWriteUE(&stBs, pstSps->bit_depth_chroma_minus8);
	HevcWriteUE(&stBs, pstSps->log2_max_pic_order_cnt_lsb_minus4);

	HevcWriteWord(&stBs, 1,
		      pstSps->sps_sub_layer_ordering_info_present_flag);

	i = pstSps->sps_sub_layer_ordering_info_present_flag
		    ? 0
		    : pstSps->sps_max_sub_layers_minus1;
	for (; i <= pstSps->sps_max_sub_layers_minus1; i++) {
		HevcWriteUE(&stBs, pstSps->sps_max_dec_pic_buffering_minus1[i]);
		HevcWriteUE(&stBs, pstSps->sps_max_num_reorder_pics[i]);
		HevcWriteUE(&stBs, pstSps->sps_max_latency_increase_plus1[i]);
	}
	/*if (pstSps->sps_sub_layer_ordering_info_present_flag){
        HevcWriteUE(&stBs,  pstSps->sps_max_dec_pic_buffering_minus1[0]);
        HevcWriteUE(&stBs,  pstSps->sps_max_num_reorder_pics[0]);
        HevcWriteUE(&stBs,  pstSps->sps_max_latency_increase_plus1[0]);
    }*/

	HevcWriteUE(&stBs, pstSps->log2_min_luma_coding_block_size_minus3);
	HevcWriteUE(&stBs, pstSps->log2_diff_max_min_luma_coding_block_size);
	HevcWriteUE(&stBs, pstSps->log2_min_transform_block_size_minus2);
	HevcWriteUE(&stBs, pstSps->log2_diff_max_min_transform_block_size);
	HevcWriteUE(&stBs, pstSps->max_transform_hierarchy_depth_inter);
	HevcWriteUE(&stBs, pstSps->max_transform_hierarchy_depth_intra);

	HevcWriteWord(&stBs, 1, pstSps->scaling_list_enabled_flag);

	HevcWriteWord(&stBs, 1, pstSps->amp_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->sample_adaptive_offset_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->pcm_enabled_flag);
	if (pstSps->pcm_enabled_flag) {
		HevcWriteWord(&stBs, 4,
			      pstSps->pcm_sample_bit_depth_luma_minus1);
		HevcWriteWord(&stBs, 4,
			      pstSps->pcm_sample_bit_depth_chroma_minus1);
		HevcWriteUE(&stBs,
			    pstSps->log2_min_pcm_luma_coding_block_size_minus3);
		HevcWriteUE(
			&stBs,
			pstSps->log2_diff_max_min_pcm_luma_coding_block_size);
		HevcWriteWord(&stBs, 1, pstSps->pcm_loop_filter_disabled_flag);
	}

	HevcWriteUE(&stBs, pstSps->num_short_term_ref_pic_sets);
	if (pstSps->num_short_term_ref_pic_sets) {
		for (int j = 0; j < pstSps->num_short_term_ref_pic_sets; j++) {
			if (j) {
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.inter_ref_pic_set_prediction_flag);
			}

			//if (!pstSps->short_term_ref_pic_set[0].inter_ref_pic_set_prediction_flag)
			//{
			HevcWriteUE(&stBs, pstSps->short_term_ref_pic_set[j]
						   .num_negative_pics);
			HevcWriteUE(&stBs, pstSps->short_term_ref_pic_set[j]
						   .num_positive_pics);
			for (int i = 0; i < pstSps->short_term_ref_pic_set[j]
						    .num_negative_pics;
			     i++) {
				HevcWriteUE(&stBs,
					    pstSps->short_term_ref_pic_set[j]
						    .delta_poc_s0_minus1[i]);
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.used_by_curr_pic_s0_flag[i]);
			}

			//             for (int i = 0; i < pstSps->short_term_ref_pic_set[0].num_negative_pics; i++){
			//                 HevcWriteWord(&stBs, 1, pstSps->short_term_ref_pic_set[0].used_by_curr_pic_s0_flag[i]);
			//             }

			for (int i = 0; i < pstSps->short_term_ref_pic_set[j]
						    .num_positive_pics;
			     i++) {
				HevcWriteUE(&stBs,
					    pstSps->short_term_ref_pic_set[j]
						    .delta_poc_s1_minus1[i]);
				HevcWriteWord(
					&stBs, 1,
					pstSps->short_term_ref_pic_set[j]
						.used_by_curr_pic_s1_flag[i]);
			}

			//             for (int i = 0; i < pstSps->short_term_ref_pic_set[0].num_positive_pics; i++){
			//                 HevcWriteWord(&stBs, 1, pstSps->short_term_ref_pic_set[0].used_by_curr_pic_s1_flag[i]);
			//             }
			//}
		}
	}

	HevcWriteWord(&stBs, 1, pstSps->long_term_ref_pics_present_flag);
	if (pstSps->long_term_ref_pics_present_flag) {
		HevcWriteUE(&stBs, pstSps->num_long_term_ref_pics_sps);
	}

	HevcWriteWord(&stBs, 1, pstSps->sps_temporal_mvp_enabled_flag);
	HevcWriteWord(&stBs, 1, pstSps->strong_intra_smoothing_enabled_flag);

	// start write vui
	// set vui parameters
	pstSps->vui_parameters_present_flag = 1;
	pstSps->vui_parameters.video_signal_type_present_flag = 1;
	pstSps->vui_parameters.video_format = 5;
	pstSps->vui_parameters.video_full_range_flag = 0;
	pstSps->vui_parameters.colour_description_present_flag = 1;
	pstSps->vui_parameters.colour_primaries = 2;
	pstSps->vui_parameters.transfer_characteristics = 16;
	pstSps->vui_parameters.matrix_coeffs = 2;
	pstSps->vui_parameters.chroma_loc_info_present_flag = 1;
	pstSps->vui_parameters.chroma_sample_loc_type_top_field = 0;    //2;
	pstSps->vui_parameters.chroma_sample_loc_type_bottom_field = 0; //2;

	HevcWriteWord(&stBs, 1, pstSps->vui_parameters_present_flag);
	if (pstSps->vui_parameters_present_flag) {

		// vui aspect ratio
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.aspect_ratio_info_present_flag);
		if (pstSps->vui_parameters.aspect_ratio_info_present_flag) {
			HevcWriteWord(&stBs, 8,
				      pstSps->vui_parameters.aspect_ratio_idc);
			if (pstSps->vui_parameters.aspect_ratio_idc == 255) {
				HevcWriteWord(&stBs, 16,
					      pstSps->vui_parameters.sar_width);
				HevcWriteWord(
					&stBs, 16,
					pstSps->vui_parameters.sar_height);
			}
		}

		//vui over scan
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.overscan_info_present_flag);
		if (pstSps->vui_parameters.overscan_info_present_flag) {
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .overscan_appropriate_flag);
		}

		// vui video signal type present flag
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.video_signal_type_present_flag);
		if (pstSps->vui_parameters.video_signal_type_present_flag) {
			HevcWriteWord(&stBs, 3,
				      pstSps->vui_parameters.video_format);
			HevcWriteWord(
				&stBs, 1,
				pstSps->vui_parameters.video_full_range_flag);

			// video colour description
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .colour_description_present_flag);
			if (pstSps->vui_parameters
				    .colour_description_present_flag) {
				HevcWriteWord(
					&stBs, 8,
					pstSps->vui_parameters.colour_primaries);
				HevcWriteWord(
					&stBs, 8,
					pstSps->vui_parameters
						.transfer_characteristics);
				HevcWriteWord(
					&stBs, 8,
					pstSps->vui_parameters.matrix_coeffs);
			}
		}

		// vui chroma loc
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.chroma_loc_info_present_flag);
		if (pstSps->vui_parameters.chroma_loc_info_present_flag) {
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .chroma_sample_loc_type_top_field);
			HevcWriteUE(
				&stBs,
				pstSps->vui_parameters
					.chroma_sample_loc_type_bottom_field);
		}

		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.neutral_chroma_indication_flag);
		HevcWriteWord(&stBs, 1, pstSps->vui_parameters.field_seq_flag);
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.frame_field_info_present_flag);

		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.default_display_window_flag);
		if (pstSps->vui_parameters.default_display_window_flag) {
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_left_offset);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_right_offset);
			HevcWriteUE(
				&stBs,
				pstSps->vui_parameters.def_disp_win_top_offset);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .def_disp_win_bottom_offset);
		}

		// vui timing info
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.vui_timing_info_present_flag);
		if (pstSps->vui_parameters.vui_timing_info_present_flag) {
			HevcWriteWord(
				&stBs, 32,
				pstSps->vui_parameters.vui_num_units_in_tick);
			HevcWriteWord(&stBs, 32,
				      pstSps->vui_parameters.vui_time_scale);

			// vui_poc_proportional_to_timing_flag
			HevcWriteWord(
				&stBs, 1,
				pstSps->vui_parameters
					.vui_poc_proportional_to_timing_flag);
			if (pstSps->vui_parameters
				    .vui_poc_proportional_to_timing_flag) {
				HevcWriteUE(
					&stBs,
					pstSps->vui_parameters
						.vui_num_ticks_poc_diff_one_minus1);
			}

			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .vui_hrd_parameters_present_flag);
			if (pstSps->vui_parameters
				    .vui_hrd_parameters_present_flag) {
				// write hrd
				hevc_add_hrd_to_sps(pstSps, &stBs);
			}
		}

		// vui restriction flag
		HevcWriteWord(
			&stBs, 1,
			pstSps->vui_parameters.bitstream_restriction_flag);
		if (pstSps->vui_parameters.bitstream_restriction_flag) {
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .tiles_fixed_structure_flag);
			HevcWriteWord(
				&stBs, 1,
				pstSps->vui_parameters
					.motion_vectors_over_pic_boundaries_flag);
			HevcWriteWord(&stBs, 1,
				      pstSps->vui_parameters
					      .restricted_ref_pic_lists_flag);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .min_spatial_segmentation_idc);
			HevcWriteUE(
				&stBs,
				pstSps->vui_parameters.max_bytes_per_pic_denom);
			HevcWriteUE(&stBs, pstSps->vui_parameters
						   .max_bits_per_min_cu_denom);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .log2_max_mv_length_horizontal);
			HevcWriteUE(&stBs,
				    pstSps->vui_parameters
					    .log2_max_mv_length_vertical);
		}
	}

	HevcWriteWord(&stBs, 1, pstSps->sps_extension_flag);

	HevcWriteWord(&stBs, 1, 1); // why?
	/*if (pstSps->sps_extension_flag)
    {
    HevcWriteWord(&stBs, 1, pstSps->sps_range_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_multilayer_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_3d_extension_flag);
    HevcWriteWord(&stBs, 1, pstSps->sps_extension_5bits);
    }*/
	uiLen = (HevcGetBsBits(&stBs) + 7) / 8;
	pstStream = (unsigned char *)malloc(uiLen + 20);
	*puiStreamLen = uiLen + 20;
	HevcReplace(aucTemp, uiLen, pstStream, puiStreamLen);
	return pstStream;
}

void hevc_add_hrd_to_sps(ST_HEVC_SPS *t_p_stSps, ST_HEVC_BITSTREAMER *t_p_st_bs)
{
	if (g_n_common_inf_preset_flag) {
		HevcWriteWord(t_p_st_bs, 1,
			      t_p_stSps->vui_parameters.hrd_parameters
				      .nal_hrd_parameters_present_flag);
		HevcWriteWord(t_p_st_bs, 1,
			      t_p_stSps->vui_parameters.hrd_parameters
				      .vcl_hrd_parameters_present_flag);

		if (t_p_stSps->vui_parameters.hrd_parameters
			    .nal_hrd_parameters_present_flag ||
		    t_p_stSps->vui_parameters.hrd_parameters
			    .vcl_hrd_parameters_present_flag) {
			HevcWriteWord(t_p_st_bs, 1,
				      t_p_stSps->vui_parameters.hrd_parameters
					      .sub_pic_hrd_params_present_flag);
			if (t_p_stSps->vui_parameters.hrd_parameters
				    .sub_pic_hrd_params_present_flag) {
				HevcWriteWord(
					t_p_st_bs, 8,
					t_p_stSps->vui_parameters.hrd_parameters
						.tick_divisor_minus2);
				HevcWriteWord(
					t_p_st_bs, 5,
					t_p_stSps->vui_parameters.hrd_parameters
						.du_cpb_removal_delay_increment_length_minus1);
				HevcWriteWord(
					t_p_st_bs, 1,
					t_p_stSps->vui_parameters.hrd_parameters
						.sub_pic_cpb_params_in_pic_timing_sei_flag);
				HevcWriteWord(
					t_p_st_bs, 5,
					t_p_stSps->vui_parameters.hrd_parameters
						.dpb_output_delay_du_length_minus1);
			}

			HevcWriteWord(t_p_st_bs, 4,
				      t_p_stSps->vui_parameters.hrd_parameters
					      .bit_rate_scale);
			HevcWriteWord(t_p_st_bs, 4,
				      t_p_stSps->vui_parameters.hrd_parameters
					      .cpb_size_scale);

			if (t_p_stSps->vui_parameters.hrd_parameters
				    .sub_pic_hrd_params_present_flag) {
				HevcWriteWord(
					t_p_st_bs, 4,
					t_p_stSps->vui_parameters.hrd_parameters
						.cpb_size_du_scale);
			}

			HevcWriteWord(
				t_p_st_bs, 5,
				t_p_stSps->vui_parameters.hrd_parameters
					.initial_cpb_removal_delay_length_minus1);
			HevcWriteWord(
				t_p_st_bs, 5,
				t_p_stSps->vui_parameters.hrd_parameters
					.au_cpb_removal_delay_length_minus1);
			HevcWriteWord(t_p_st_bs, 5,
				      t_p_stSps->vui_parameters.hrd_parameters
					      .dpb_output_delay_length_minus1);
		}
	}

	for (int i = 0; i <= t_p_stSps->sps_max_sub_layers_minus1; i++) {
		HevcWriteWord(t_p_st_bs, 1,
			      t_p_stSps->vui_parameters.hrd_parameters
				      .fixed_pic_rate_general_flag[i]);
		if (t_p_stSps->vui_parameters.hrd_parameters
			    .fixed_pic_rate_general_flag[i]) {
			HevcWriteWord(
				t_p_st_bs, 1,
				t_p_stSps->vui_parameters.hrd_parameters
					.fixed_pic_rate_within_cvs_flag[i]);
		}

		if (t_p_stSps->vui_parameters.hrd_parameters
			    .fixed_pic_rate_within_cvs_flag[i]) {
			HevcWriteUE(
				t_p_st_bs,
				t_p_stSps->vui_parameters.hrd_parameters
					.elemental_duration_in_tc_minus1[i]);
		} else {
			HevcWriteWord(t_p_st_bs, 1,
				      t_p_stSps->vui_parameters.hrd_parameters
					      .low_delay_hrd_flag[i]);
		}

		if (!t_p_stSps->vui_parameters.hrd_parameters
			     .low_delay_hrd_flag[i]) {
			HevcWriteUE(t_p_st_bs,
				    t_p_stSps->vui_parameters.hrd_parameters
					    .cpb_cnt_minus1[i]);
		}

		if (t_p_stSps->vui_parameters.hrd_parameters
			    .nal_hrd_parameters_present_flag) {
			hevc_add_hrd_sublayer_to_sps(
				&(t_p_stSps->vui_parameters.hrd_parameters
					  .nal_sub_layer_hrd_parameters[i]),
				t_p_st_bs,
				t_p_stSps->vui_parameters.hrd_parameters
					.sub_pic_hrd_params_present_flag,
				t_p_stSps->vui_parameters.hrd_parameters
					.cpb_cnt_minus1[i]);
		}

		if (t_p_stSps->vui_parameters.hrd_parameters
			    .vcl_hrd_parameters_present_flag) {
			hevc_add_hrd_sublayer_to_sps(
				&(t_p_stSps->vui_parameters.hrd_parameters
					  .vcl_sub_layer_hrd_parameters[i]),
				t_p_st_bs,
				t_p_stSps->vui_parameters.hrd_parameters
					.sub_pic_hrd_params_present_flag,
				t_p_stSps->vui_parameters.hrd_parameters
					.cpb_cnt_minus1[i]);
		}
	}
}

void hevc_add_hrd_sublayer_to_sps(ST_HEVC_SUBLAYER_HRD_PARAMETERS *pstSlhrd,
				  ST_HEVC_BITSTREAMER *t_p_st_bs,
				  int sub_pic_hrd_params_present_flag,
				  unsigned int CpbCnt)
{
	for (int i = 0; i <= CpbCnt; i++) {
		HevcWriteUE(t_p_st_bs, pstSlhrd->bit_rate_value_minus1[i]);
		HevcWriteUE(t_p_st_bs, pstSlhrd->cpb_size_value_minus1[i]);
		if (sub_pic_hrd_params_present_flag) {
			HevcWriteUE(t_p_st_bs,
				    pstSlhrd->cpb_size_du_value_minus1[i]);
			HevcWriteUE(t_p_st_bs,
				    pstSlhrd->bit_rate_du_value_minus1[i]);
		}
		HevcWriteWord(t_p_st_bs, 1, pstSlhrd->cbr_flag[i]);
	}
}

#if 0
void main()
{
    ST_HEVC_SPS stSps;
    memset(&stSps, 0, sizeof(stSps));
    unsigned char *pucOut;
    unsigned int uiLen;
    FILE *fp = fopen("data", "rb");
    unsigned char buf[100];
    fread(buf, 1, 45, fp);
    HevcParseSPS(&stSps, buf+2, 43);
    pucOut = HevcAddHdrInfoToSps(&stSps, &uiLen);
    printf("%d\n",pucOut[0]);
}
#endif