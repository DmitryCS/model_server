//*****************************************************************************
// Copyright 2021 Intel Corporation
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

#include <fstream>

#include "../binaryutils.hpp"
#include "gtest/gtest.h"
#include "opencv2/opencv.hpp"

using namespace ovms;

namespace {

class BinaryUtilsTest : public ::testing::Test {};

TEST_F(BinaryUtilsTest, tensorWithNonMatchingBatchsize) {
    tensorflow::TensorProto stringVal;
    stringVal.add_string_val("dummy");
    InferenceEngine::Blob::Ptr blob;
    auto tensorInfo = std::make_shared<TensorInfo>();
    tensorInfo->setShape({5, 1, 1, 1});
    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);
    EXPECT_EQ(status, ovms::StatusCode::UNSUPPORTED_LAYOUT);
}

TEST_F(BinaryUtilsTest, positive_rgb) {
    uint8_t rgb_expected_blob[] = {0x24, 0x1b, 0xed};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/rgb.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::U8, shape_t{1, 3, 1, 1}, InferenceEngine::Layout::NCHW);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);

    ASSERT_EQ(blob->size(), 3);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();
    EXPECT_EQ(std::equal(ptr, ptr + blob->size(), rgb_expected_blob), true);
}

TEST_F(BinaryUtilsTest, positive_grayscale) {
    uint8_t grayscale_expected_blob[] = {0x00};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/grayscale.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::U8, shape_t{1, 1, 1, 1}, InferenceEngine::Layout::NCHW);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);
    ASSERT_EQ(blob->size(), 1);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();
    EXPECT_EQ(std::equal(ptr, ptr + blob->size(), grayscale_expected_blob), true);
}

TEST_F(BinaryUtilsTest, positive_batch_size_2) {
    uint8_t rgb_batchsize_2_blob[] = {0x24, 0x1b, 0xed, 0x24, 0x1b, 0xed};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/rgb.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::U8, shape_t{2, 3, 1, 1}, InferenceEngine::Layout::NCHW);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);

    ASSERT_EQ(blob->size(), 6);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();
    EXPECT_EQ(std::equal(ptr, ptr + blob->size(), rgb_batchsize_2_blob), true);
}

TEST_F(BinaryUtilsTest, positive_precision_changed) {
    uint8_t rgb_precision_changed_expected_blob[] = {0x24, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0xed, 0x00, 0x00, 0x00};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/rgb.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::I32, shape_t{1, 3, 1, 1}, InferenceEngine::Layout::NCHW);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);

    ASSERT_EQ(blob->size(), 3);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();
    int I32_size = 4;
    EXPECT_EQ(std::equal(ptr, ptr + blob->size() * I32_size, rgb_precision_changed_expected_blob), true);
}

TEST_F(BinaryUtilsTest, positive_nhwc_layout) {
    uint8_t rgb_expected_blob[] = {0x24, 0x1b, 0xed};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/rgb.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::U8, shape_t{1, 1, 1, 3}, InferenceEngine::Layout::NHWC);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);

    ASSERT_EQ(blob->size(), 3);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();

    EXPECT_EQ(std::equal(ptr, ptr + blob->size(), rgb_expected_blob), true);
}

TEST_F(BinaryUtilsTest, positive_resizing) {
    uint8_t rgb_expected_blob[] = {0x24, 0x24, 0x24, 0x24, 0x1b, 0x1b, 0x1b, 0x1b, 0xed, 0xed, 0xed, 0xed};

    std::ifstream DataFile;
    DataFile.open("/ovms/src/test/binaryutils/rgb.jpg", std::ios::binary);
    DataFile.seekg(0, std::ios::end);
    size_t filesize = DataFile.tellg();
    DataFile.seekg(0);
    std::unique_ptr<char[]> image_bytes(new char[filesize]);
    DataFile.read(image_bytes.get(), filesize);

    tensorflow::TensorProto stringVal;
    InferenceEngine::Blob::Ptr blob;
    stringVal.set_dtype(tensorflow::DataType::DT_STRING);
    stringVal.add_string_val(image_bytes.get(), filesize);

    std::shared_ptr<TensorInfo> tensorInfo = std::make_shared<TensorInfo>("", InferenceEngine::Precision::U8, shape_t{1, 3, 2, 2}, InferenceEngine::Layout::NCHW);

    auto status = convertStringValToBlob(stringVal, &blob, tensorInfo);

    ASSERT_EQ(blob->size(), 12);
    ASSERT_EQ(status, ovms::StatusCode::OK);
    uint8_t* ptr = blob->buffer();
    EXPECT_EQ(std::equal(ptr, ptr + blob->size(), rgb_expected_blob), true);
}
}  // namespace