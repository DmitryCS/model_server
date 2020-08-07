//*****************************************************************************
// Copyright 2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************
#pragma once

#include <map>
#include <string>
#include <utility>

#include <grpcpp/server_context.h>

#include "tensorflow_serving/util/net_http/server/public/response_code_enum.h"

namespace ovms {

namespace net_http = tensorflow::serving::net_http;

enum class StatusCode {
    OK, /*!< Success */

    PATH_INVALID, /*!< The provided path is invalid or doesn't exists */
    FILE_INVALID, /*!< File not found or cannot open */
    NETWORK_NOT_LOADED,
    JSON_INVALID,             /*!< The file/content is not valid json */
    JSON_SERIALIZATION_ERROR, /*!< Data serialization to json format failed */
    MODELINSTANCE_NOT_FOUND,
    SHAPE_WRONG_FORMAT,                /*!< The provided shape param is in wrong format */
    PLUGIN_CONFIG_WRONG_FORMAT,        /*!< Plugin config is in wrong format */
    MODEL_VERSION_POLICY_WRONG_FORMAT, /*!< Model version policy is in wrong format */
    GRPC_CHANNEL_ARG_WRONG_FORMAT,
    NO_MODEL_VERSION_AVAILABLE, /*!< No model version found in path */
    RESHAPE_ERROR,              /*!< Impossible to perform reshape */
    RESHAPE_REQUIRED,           /*!< Model instance needs to be reloaded with new shape */
    BATCHSIZE_CHANGE_REQUIRED,  /*!< Model instance needs to be reloaded with new batch size */
    AMBIGUOUS_SHAPE_PARAM,  /*!< Fixed no named shape param defined for model with multiple inputs */

    // Model management
    MODEL_MISSING,                    /*!< Model with such name and/or version does not exist */
    MODEL_NAME_MISSING,               /*!< Model with requested name is not found */
    MODEL_VERSION_MISSING,            /*!< Model with requested version is not found */
    MODEL_VERSION_NOT_LOADED_ANYMORE, /*!< Model with requested version is retired */
    MODEL_VERSION_NOT_LOADED_YET,     /*!< Model with requested version is not loaded yet */

    // Predict request validation
    INVALID_NO_OF_INPUTS,           /*!< Invalid number of inputs */
    INVALID_MISSING_INPUT,          /*!< Missing one or more of inputs */
    INVALID_NO_OF_SHAPE_DIMENSIONS, /*!< Invalid number of shape dimensions */
    INVALID_BATCH_SIZE,             /*!< Input batch size other than required */
    INVALID_SHAPE,                  /*!< Invalid shape dimension number or dimension value */
    INVALID_PRECISION,              /*!< Invalid precision */
    INVALID_VALUE_COUNT,            /*!< Invalid value count error status for uint16 and half float data types */
    INVALID_CONTENT_SIZE,           /*!< Invalid content size error status for types using tensor_content() */

    // Deserialization
    OV_UNSUPPORTED_DESERIALIZATION_PRECISION, /*!< Unsupported deserialization precision, theoretically should never be returned since ModelInstance::validation checks against network precision */
    OV_INTERNAL_DESERIALIZATION_ERROR,        /*!< Error occured during deserialization */

    // Inference
    OV_INTERNAL_INFERENCE_ERROR, /*!< Error occured during inference */

    // Serialization
    OV_UNSUPPORTED_SERIALIZATION_PRECISION, /*!< Unsupported serializaton precision */
    OV_INTERNAL_SERIALIZATION_ERROR,        /*!< Error occurred during serialization */

    // GetModelStatus
    INVALID_SIGNATURE_DEF, /*!< Requested signature is not supported */

    // Common request validation errors
    MODEL_SPEC_MISSING, /*!< Request lacks model_spec */

    INTERNAL_ERROR,

    UNKNOWN_ERROR,

    NOT_IMPLEMENTED,

    // S3
    S3_BUCKET_NOT_FOUND, /*!< S3 Bucket not found  */
    S3_METADATA_FAIL,
    S3_FAILED_LIST_OBJECTS,
    S3_FAILED_GET_TIME,
    S3_INVALID_ACCESS,
    S3_FILE_NOT_FOUND,
    S3_FILE_INVALID,
    S3_FAILED_GET_OBJECT,

    // GCS
    GCS_BUCKET_NOT_FOUND,
    GCS_METADATA_FAIL,
    GCS_FAILED_LIST_OBJECTS,
    GCS_FAILED_GET_TIME,
    GCS_INVALID_ACCESS,
    GCS_FILE_NOT_FOUND,
    GCS_FILE_INVALID,
    GCS_FAILED_GET_OBJECT,
    GCS_INCORRECT_REQUESTED_OBJECT_TYPE,

    // REST handler
    REST_NOT_FOUND,               /*!< Requested REST resource not found */
    REST_COULD_NOT_PARSE_VERSION, /*!< Could not parse model version in request */
    REST_MALFORMED_REQUEST,       /*!< Malformed REST request */

    // REST Parse
    REST_BODY_IS_NOT_AN_OBJECT,          /*!< REST body should be JSON object */
    REST_PREDICT_UNKNOWN_ORDER,          /*!< Could not detect order (row/column) */
    REST_INSTANCES_NOT_AN_ARRAY,         /*!< When parsing row order, instances must be an array */
    REST_NAMED_INSTANCE_NOT_AN_OBJECT,   /*!< When parsing named instance it needs to be an object */
    REST_INPUT_NOT_PREALLOCATED,         /*!< When parsing no named instance, exactly one input need to be preallocated */
    REST_NO_INSTANCES_FOUND,             /*!< Missing instances in row order */
    REST_INSTANCES_NOT_NAMED_OR_NONAMED, /*!< Unknown instance format, neither named or nonamed */
    REST_COULD_NOT_PARSE_INSTANCE,       /*!< Error while parsing instance content, not valid ndarray */
    REST_INSTANCES_BATCH_SIZE_DIFFER,    /*!< In row order 0-th dimension (batch size) must be equal for all inputs */
    REST_INPUTS_NOT_AN_OBJECT,           /*!< When parsing column order, inputs must be an object */
    REST_NO_INPUTS_FOUND,                /*!< Missing inputs in column order */
    REST_COULD_NOT_PARSE_INPUT,          /*!< Error while parsing input content, not valid ndarray */
    REST_PROTO_TO_STRING_ERROR,          /*!< Error while parsing ResponseProto to JSON string */
    REST_UNSUPPORTED_PRECISION,          /*!< Unsupported conversion from tensor_content to _val container */
    REST_SERIALIZE_TENSOR_CONTENT_INVALID_SIZE,

    PIPELINE_DEFINITION_ALREADY_EXIST,
    PIPELINE_MULTIPLE_ENTRY_NODES,
    PIPELINE_MULTIPLE_EXIT_NODES,
    PIPELINE_MISSING_ENTRY_OR_EXIT,
    PIPELINE_DEFINITION_NAME_MISSING,
    PIPELINE_NODE_NAME_DUPLICATE,
};

class Status {
    StatusCode code;

    static const std::map<const StatusCode, const std::pair<grpc::StatusCode, const std::string>> grpcMessages;

public:
    Status(StatusCode code = StatusCode::OK) :
        code(code) {}

    bool ok() const {
        return code == StatusCode::OK;
    }

    const StatusCode getCode() const {
        return this->code;
    }

    bool batchSizeChangeRequired() const {
        return code == StatusCode::BATCHSIZE_CHANGE_REQUIRED;
    }

    bool reshapeRequired() const {
        return code == StatusCode::RESHAPE_REQUIRED;
    }

    bool operator==(const Status& status) const {
        return this->code == status.code;
    }

    bool operator!=(const Status& status) const {
        return this->code != status.code;
    }

    const grpc::Status grpc() const {
        static const grpc::Status defaultStatus = grpc::Status(
            grpc::StatusCode::UNKNOWN,
            "Unknown error");

        auto it = grpcMessages.find(code);
        if (it != grpcMessages.end()) {
            return grpc::Status(it->second.first, it->second.second);
        } else {
            return defaultStatus;
        }
    }

    operator grpc::Status() const {
        return this->grpc();
    }

    const std::string& string() const {
        static const std::string defaultString = "Unknown error";

        auto it = grpcMessages.find(code);
        if (it != grpcMessages.end()) {
            return it->second.second;
        } else {
            return defaultString;
        }
    }

    operator const std::string&() const {
        return this->string();
    }

    const net_http::HTTPStatusCode http() const;
};

}  // namespace ovms