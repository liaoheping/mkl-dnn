#ifndef MKL_DNN_H
#define MKL_DNN_H

#include "mkl_dnn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initializes a \param tensor_desc by given \param ndims_batch, \param
 * ndims_channels, \param ndims_spatial, and array \param dims */
mkl_dnn_status_t mkl_dnn_tensor_desc_init(mkl_dnn_tensor_desc_t *tensor_desc,
		uint32_t ndims_batch, uint32_t ndims_channels, uint32_t ndims_spatial,
		const mkl_dnn_dims_t dims);

/** Initializes a \param memory_desc memory descriptor by given \param tensor
 * and \param format. \param format is allowed to be memory_format_any */
mkl_dnn_status_t mkl_dnn_memory_desc_init(mkl_dnn_memory_desc_t *memory_desc,
        const mkl_dnn_tensor_desc_t *tensor, mkl_dnn_memory_format_t format);

/** Initializes a \param memory_primtive_desc memory primitive descriptor by
 * given \param memory_desc and \param engine. Note that \param memory_desc
 * cannot be uncertain, i.e. initialized with memory_format_any */
mkl_dnn_status_t mkl_dnn_memory_primitive_desc_init(
        mkl_dnn_memory_primitive_desc_t *memory_primitive_desc,
        const mkl_dnn_memory_desc_t *memory_desc,
		const_mkl_dnn_engine_t engine);

/** Compares two memory primitive descriptors. \return 1 if the memory
 * primitives descriptors are the same, \return 0 otherwise. Use this function
 * in order to identify whether reorder between two memory primitives is
 * required */
int mkl_dnn_memory_primitive_desc_equal(const mkl_dnn_memory_primitive_desc_t *lhs,
        const mkl_dnn_memory_primitive_desc_t *rhs);

/** Creates a \param memory by given \param memory_primitive_desc and \param
 * data_ptr. If \param data_ptr is NULL \param memory would be initialized with
 * internally allocated memory. primitive_destroy() will takes care on
 * allocated memory then */
mkl_dnn_status_t mkl_dnn_memory_create(mkl_dnn_primitive_t *memory,
        const mkl_dnn_memory_primitive_desc_t *memory_primitive_desc,
        const void *data_ptr);

/** For given \param memory primitive fills in coresponding \param
 * memory_primitive_desc */
mkl_dnn_status_t mkl_dnn_memory_get_primitive_desc(
		const_mkl_dnn_primitive_t memory,
        mkl_dnn_memory_primitive_desc_t *memory_primitive_desc);

/** Initializes a \param reorder_primitive_desc by given \param input and
 * \param output memory primitive descriptors */
mkl_dnn_status_t mkl_dnn_reorder_primitive_desc_init(
        mkl_dnn_reorder_primitive_desc_t *reorder_primitive_desc,
        const mkl_dnn_memory_primitive_desc_t *input,
        const mkl_dnn_memory_primitive_desc_t *output);

/** Initializes a \param convolution_desc by given \param prop_kind, \param
 * alg_kind, memory descriptors, \param strides, \param padding and \param
 * padding_kind. Please note that memory descriptors may be initialized with
 * format_kind_any format_kind. In this case convolution_primitive_desc_init()
 * will choose the best convolution possible in terms of performance */
mkl_dnn_status_t mkl_dnn_convolution_desc_init(
		mkl_dnn_convolution_desc_t *convolution_desc,
        mkl_dnn_prop_kind_t prop_kind, mkl_dnn_alg_kind_t alg_kind,
        const mkl_dnn_memory_desc_t *input_desc,
		const mkl_dnn_memory_desc_t *weights_desc,
        const mkl_dnn_memory_desc_t *bias_desc,
		const mkl_dnn_memory_desc_t *output_desc,
        const mkl_dnn_dims_t strides, const mkl_dnn_nd_pos_t padding,
        mkl_dnn_padding_kind_t padding_kind);

/** Initializes a \param convolution_primitve_desc by given \param
 * convolution_desc and \param engine */
mkl_dnn_status_t mkl_dnn_convolution_primitive_desc_init(
        mkl_dnn_convolution_primitive_desc_t *convolution_primitive_desc,
        const mkl_dnn_convolution_desc_t *convolution_desc,
		const_mkl_dnn_engine_t engine);

/* TODO: add convolution_forward_primitive_desc_init for given
 * mkl_dnn_memory_primitive_desc_t input, weights, ... so that user has more
 * flexibility */

/* XXX: think on this: either add "forward" in function name or put all inputs
 * and outputs in-to arrays, otherwise it is unclear how to create bwd filt */
mkl_dnn_status_t mkl_dnn_convolution_create(mkl_dnn_primitive_t *convolution,
        const mkl_dnn_convolution_primitive_desc_t *convolution_primitive_desc,
        const mkl_dnn_primitive_at_t input, const mkl_dnn_primitive_t weights,
        const mkl_dnn_primitive_at_t bias, mkl_dnn_primitive_t output);

/** Creates a \param primitive by given \param primitive descriptor and array
 * of \param inputs and \param outputs */
mkl_dnn_status_t mkl_dnn_primitive_create(mkl_dnn_primitive_t *primitive,
        const_mkl_dnn_primitive_desc_t primitive_desc,
        const mkl_dnn_primitive_at_t *inputs,
		const_mkl_dnn_primitive_t *outputs);

/** Deletes \param primitive. Also deallocates internally allocated memory if
 * primitive represents memory, previously created via
 * memory_create(&primitive, ..., NULL) */
mkl_dnn_status_t mkl_dnn_primitive_destroy(mkl_dnn_primitive_t primitive);

/** Creates a dnn_primitive_at structure out of a \param primitive and a \param
 * output_index. Note that this function only fills in the data structure
 * without checking whether parameters are correct. The actual error checking
 * is done when the resulting dnn_primitive_at structure is passed to a
 * primitive creation function. */
mkl_dnn_primitive_at_t mkl_dnn_primitive_at(
		const_mkl_dnn_primitive_t primitive,
        size_t output_index);

/** Returns the number of engines of a particular \param kind */
size_t mkl_dnn_engine_get_count(mkl_dnn_engine_kind_t kind);

/** Creates an \param engine of particular \param kind and \param index */
mkl_dnn_status_t mkl_dnn_engine_create(mkl_dnn_engine_t *engine,
		mkl_dnn_engine_kind_t kind, size_t index);

/** Destroys an \param engine */
mkl_dnn_status_t mkl_dnn_engine_destroy(mkl_dnn_engine_t engine);

/** Creates an execution \param stream */
mkl_dnn_status_t mkl_dnn_stream_create(mkl_dnn_stream_t *stream);

/** Submits \param n \param primitives to execution \param stream.  All or none
 * of the primitives must be lazy. In case of an error, may return offending
 * \param error_primitive if it is not NULL. */
mkl_dnn_status_t mkl_dnn_stream_submit(mkl_dnn_stream_t stream, size_t n,
        mkl_dnn_primitive_t primitives[],
		mkl_dnn_primitive_t *error_primitive);

/** Waits for all primitives in the execution \param stream to finish. Returns
 * immediately with a status if \param block is 0. In case of error, may return
 * offending \param error_primitive if it is not NULL. */
mkl_dnn_status_t mkl_dnn_stream_wait(mkl_dnn_stream_t stream, int block,
        mkl_dnn_primitive_t *error_primitive);

/** Destroys an execution \param stream */
mkl_dnn_status_t mkl_dnn_stream_destroy(mkl_dnn_stream_t stream);

#ifdef __cplusplus
}
#endif

#endif