/**
 * \file test/src/skeleton/annotated_image_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_ANNOTATED_IMAGE_TEST_H_
#define TEST_SRC_SKELETON_ANNOTATED_IMAGE_TEST_H_

#include <gtest/gtest.h>

#include <cstdint>
#include <exception>
#include "src/skeleton/images/annotated_image.h"
#include "test/src/utils/test_utils.h"
#include "test/src/utils/geometry_utils.h"
#include "test/src/skeleton/commons.h"

namespace skeleton_test {

struct _Annotation {
  int id;

  bool operator ==(const _Annotation& other) const noexcept {
    return id == other.id;
  }
};

using annotated_image_test_t = skeleton::AnnotatedImage<_Pixel,
                                                        _Annotation,
                                                        geometry_utils::Pixel_Hasher,
                                                        geometry_utils::Pixel_Comparer>;

TEST(AnnotatedImage, HasAnnotation_Returns_True_For_Annotated_Points) {
  // arrange
  annotated_image_test_t annotations;
  const _Pixel first { 1, 1 };
  const _Pixel second { 2, 3 };
  const _Annotation first_annotation = { 1 };
  const _Annotation second_annotation = { 2 };
  annotations.AddPointAnnotation(first, first_annotation);
  annotations.AddPointAnnotation(second, second_annotation);

  // act
  auto result_first = annotations.HasAnnotation(first);
  auto result_second = annotations.HasAnnotation(second);

  // assert
  EXPECT_TRUE(result_first);
  EXPECT_TRUE(result_second);
}

TEST(AnnotatedImage, HasAnnotation_Returns_False_For_Non_Annotated_Points) {
  // arrange
  annotated_image_test_t annotations;
  const _Pixel first { 1, 1 };
  const _Pixel second { 2, 3 };

  // act
  auto result_first = annotations.HasAnnotation(first);
  auto result_second = annotations.HasAnnotation(second);

  // assert
  EXPECT_FALSE(result_first);
  EXPECT_FALSE(result_second);
}

TEST(AnnotatedImage, ReadAnnotation_Returns_Proper_Annotation) {
  // arrange
  annotated_image_test_t annotations;
  const _Pixel first { 1, 1 };
  const _Pixel second { 2, 3 };
  const _Annotation first_annotation = { 1 };
  const _Annotation second_annotation = { 2 };
  annotations.AddPointAnnotation(first, first_annotation);
  annotations.AddPointAnnotation(second, second_annotation);

  // act
  _Annotation result_first = annotations.ReadAnnotation(first);

  // assert
  EXPECT_EQ(first_annotation, result_first);
}

TEST(AnnotatedImage, ReadAnnotation_Throws_Exception_For_Unexistent_Annotation) {
  // arrange
  annotated_image_test_t annotations;

  EXPECT_ANY_THROW(annotations.ReadAnnotation(_Pixel { 1, 1 }));
}

TEST(AnnotatedImage, ModifyAnnotation_Modifies_Existing_Annotation) {
  // arrange
  annotated_image_test_t annotations;

  const _Pixel first { 1, 1 };
  const _Pixel second { 2, 3 };
  const _Annotation first_annotation = { 1 };
  const _Annotation second_annotation = { 2 };
  const _Annotation new_value = { 3 };
  annotations.AddPointAnnotation(first, first_annotation);
  annotations.AddPointAnnotation(second, second_annotation);

  // act
  annotations.ModifyAnnotation(first, new_value);

  // assert
  auto result_first = annotations.ReadAnnotation(first);
  EXPECT_EQ(new_value, result_first);

  auto result_second = annotations.ReadAnnotation(second);
  EXPECT_EQ(second_annotation, result_second);
}

TEST(AnnotatedImage, ModifyAnnotation_Throws_Exception_For_Unexisting_Annotation) {
  // arrange
  annotated_image_test_t annotations;
  const _Annotation new_value = { 3 };

  EXPECT_ANY_THROW(annotations.ModifyAnnotation(_Pixel { 1, 1 }, new_value));
}

TEST(AnnotatedImage, TryAddAnnotation_Returns_True_For_NonExisting_Annotation) {
  // arrange
  annotated_image_test_t annotations;

  const _Pixel first { 1, 1 };
  const _Annotation first_annotation = { 1 };

  // act
  auto result = annotations.TryAddAnnotation(first, first_annotation);

  // assert
  EXPECT_TRUE(result);
}

TEST(AnnotatedImage, TryAddAnnotation_Returns_False_For_Existing_Annotation) {
  // arrange
  annotated_image_test_t annotations;

  const _Pixel first { 1, 1 };
  const _Annotation first_annotation = { 1 };
  const _Annotation new_value = { 3 };
  annotations.AddPointAnnotation(first, first_annotation);

  // act
  auto result = annotations.TryAddAnnotation(first, new_value);

  // assert
  EXPECT_FALSE(result);
}

TEST(AnnotatedImage, AddPointAnnotation_Throws_For_Existing_Annotation) {
  // arrange
  annotated_image_test_t annotations;

  const _Pixel first { 1, 1 };
  const _Annotation first_annotation = { 1 };
  const _Annotation new_value = { 3 };
  annotations.AddPointAnnotation(first, first_annotation);

  EXPECT_ANY_THROW(annotations.AddPointAnnotation(first, new_value));
}

TEST(AnnotatedImage, Constructor_Creates_Object_With_Same_Size) {
  // arrange
  annotated_image_test_t annotations;
  const _Pixel first { 1, 1 };
  const _Annotation first_annotation = { 1 };
  annotations.AddPointAnnotation(first, first_annotation);

  // act
  annotated_image_test_t other_annotations(annotations);
  EXPECT_EQ(other_annotations.GetSize(), annotations.GetSize());
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_ANNOTATED_IMAGE_TEST_H_
