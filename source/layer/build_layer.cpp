//
// Created by fss on 22-11-15.
//
#include "build_layer.hpp"
#include "convolution.hpp"
#include "parser/runtime_ir.hpp"

namespace kuiper_infer {

static ParameterAttrParseStatus BuildConvLayer(const std::shared_ptr<RuntimeOperator> &op,
                                               std::shared_ptr<Layer> conv_layer) {
  CHECK(op != nullptr);
  const std::map<std::string, RuntimeParameter *> params = op->params;

  if (params.find("in_channels") == params.end()) {
    LOG(ERROR) << "Can not find the in channel parameter";
    return ParameterAttrParseStatus::kParameterFailedInChannel;
  }

  const auto &in_channel = dynamic_cast<RuntimeParameterInt *>(params.at("in_channels"));
  if (!in_channel) {
    LOG(ERROR) << "Can not find the in channel parameter";
    return ParameterAttrParseStatus::kParameterFailedInChannel;
  }

  if (params.find("out_channels") == params.end()) {
    LOG(ERROR) << "Can not find the out channel parameter";
    return ParameterAttrParseStatus::kParameterFailedOutChannel;
  }

  const auto &out_channel = dynamic_cast<RuntimeParameterInt *>(params.at("out_channels"));
  if (!out_channel) {
    LOG(ERROR) << "Can not find the out channel parameter";
    return ParameterAttrParseStatus::kParameterFailedOutChannel;
  }

  if (params.find("padding") == params.end()) {
    LOG(ERROR) << "Can not find the padding parameter";
    return ParameterAttrParseStatus::kParameterFailedPadding;
  }

  const auto &padding = dynamic_cast<RuntimeParameterIntArray *>(params.at("padding"));
  if (!padding) {
    LOG(ERROR) << "Can not find the padding parameter";
    return ParameterAttrParseStatus::kParameterFailedPadding;
  }

  if (params.find("bias") == params.end()) {
    LOG(ERROR) << "Can not find the bias parameter";
    return ParameterAttrParseStatus::kParameterFailedUseBias;
  }
  const auto &use_bias = dynamic_cast<RuntimeParameterBool *>(params.at("bias"));
  if (!use_bias) {
    LOG(ERROR) << "Can not find the bias parameter";
    return ParameterAttrParseStatus::kParameterFailedUseBias;
  }

  if (params.find("stride") == params.end()) {
    LOG(ERROR) << "Can not find the stride parameter";
    return ParameterAttrParseStatus::kParameterFailedStride;
  }
  const auto &stride = dynamic_cast<RuntimeParameterIntArray *>(params.at("stride"));
  if (!stride) {
    LOG(ERROR) << "Can not find the stride parameter";
    return ParameterAttrParseStatus::kParameterFailedStride;
  }

  if (params.find("kernel_size") == params.end()) {
    return ParameterAttrParseStatus::kParameterFailedKernel;
  }
  const auto &kernel = dynamic_cast<RuntimeParameterIntArray *>(params.at("kernel_size"));
  if (!kernel) {
    return ParameterAttrParseStatus::kParameterFailedKernel;
  }

  const uint32_t dims = 2;
  const std::vector<int> &kernels = kernel->value;
  const std::vector<int> &paddings = padding->value;
  const std::vector<int> &strides = stride->value;
  if (kernels.size() != dims || paddings.size() != dims || stride->value.size() != dims) {
    return ParameterAttrParseStatus::kParameterFailedParamsSize;
  }

  conv_layer = std::make_shared<ConvolutionLayer>(out_channel->value, in_channel->value,
                                                  kernels.at(0), kernels.at(1),
                                                  paddings.at(0), strides.at(0),
                                                  use_bias->value);

  // load weights
  const std::map<std::string, std::shared_ptr<RuntimeAttribute>> &attrs = op->attribute;
  const auto &bias = attrs.at("bias");
  const std::vector<int> &bias_shape = bias->shape;
  if (bias_shape.empty() || bias_shape.at(0) != out_channel->value) {
    LOG(ERROR) << "Bias shape is wrong";
    return ParameterAttrParseStatus::kParameterFailedBiasSize;
  }

  std::vector<double> bias_values = bias->get<double>();
  conv_layer->set_bias(bias_values);

  const auto &weight = attrs.at("weight");
  const std::vector<int> &weight_shape = weight->shape;
  if (weight_shape.empty()) {
    LOG(ERROR) << "Weight shape is empty";
    return ParameterAttrParseStatus::kParameterFailedWeightSize;
  }

  std::vector<double> weight_values = weight->get<double>();
  conv_layer->set_weights(weight_values);

  return ParameterAttrParseStatus::kParameterSuccess;
}

std::shared_ptr<Layer> BuildLayer(const std::string &layer_type, const std::shared_ptr<RuntimeOperator> &op) {
  CHECK(op != nullptr);
  if (layer_type == "nn.Conv2d") {
    std::shared_ptr<Layer> conv_layer;
    const ParameterAttrParseStatus result = BuildConvLayer(op, conv_layer);
    LOG_IF(FATAL, result != ParameterAttrParseStatus::kParameterSuccess)
            << "Build convolution layer failed,error code: " << int(result);
    return conv_layer;
  }
}
}