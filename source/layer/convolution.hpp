//
// Created by fss on 22-11-13.
//

#ifndef KUIPER_COURSE_SOURCE_LAYER_CONVOLUTION_HPP_
#define KUIPER_COURSE_SOURCE_LAYER_CONVOLUTION_HPP_
#include "param_layer.hpp"

namespace kuiper_infer {
class ConvolutionLayer : public ParamLayer {
 public:
  explicit ConvolutionLayer();

  explicit ConvolutionLayer(const std::vector<std::shared_ptr<Blob>> &weights,
                            const std::vector<std::shared_ptr<Blob>> &bias,
                            uint32_t padding, uint32_t stride, bool use_bias = true);

  explicit ConvolutionLayer(uint32_t output_channel, uint32_t in_channel, uint32_t kernel_h,
                            uint32_t kernel_w, uint32_t padding, uint32_t stride, bool use_bias = true);

  InferStatus Forward(const std::vector<std::shared_ptr<Blob>> &inputs,
                      std::vector<std::shared_ptr<Blob>> &outputs) override;


 private:
  bool use_bias_ = false;
  uint32_t padding_ = 0;
  uint32_t stride_ = 1;
};
}

#endif //KUIPER_COURSE_SOURCE_LAYER_CONVOLUTION_HPP_