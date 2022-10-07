/**
 * \file src/skeleton/centerline_includes.h
 * \brief This file implements useful code to include centerlines functionality.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_INCLUDES_H_
#define SRC_SKELETON_CENTERLINE_INCLUDES_H_

#include <armadillo>
#include <string>
#include <utility>
#include <tuple>
#include <vector>
#include <cstdint>
#include <memory>

#include "src/skeleton/centerline/centerline_calculator.h"
#include "src/skeleton/centerline/vertex.h"
#include "src/skeleton/centerline/speed_graph_builder.h"
#include "src/skeleton/centerline/memory_graph_builder.h"
#include "src/skeleton/centerline/weight_calculator.h"
#include "src/skeleton/centerline/vertex_neighbour_calculator.h"
#include "src/skeleton/centerline/centerline_centerpoint_discoverer.h"
#include "src/skeleton/centerline/maximal_clusters_discoverer.h"

#include "src/skeleton/graph/speed_graph.h"
#include "src/skeleton/graph/memory_graph.h"
#include "src/skeleton/graph/dijkstra.h"
#include "src/skeleton/graph/annotated_vertex_helper.h"

#include "src/skeleton/contours/contour_calculator.h"
#include "src/skeleton/paths/quadratic_path_calculator.h"

#include "src/skeleton/heaps/handle_based_binary_heap.h"

#include "src/skeleton/utils/annotated_image_json_exporter.h"
#include "src/skeleton/utils/annotated_image_binary_exporter.h"
#include "src/skeleton/neighbours/twenty_six_neighbour_calculator.h"
#include "src/skeleton/images/annotated_image.h"
#include "src/skeleton/images/integer_image.h"
#include "src/skeleton/images/ternary_image.h"
#include "src/skeleton/images/arma_3d_binary_image_builder.h"
#include "src/skeleton/skeletonizer_by_ift.h"
#include "src/skeleton/centerline/gradient.h"

namespace skeleton {

struct Settings {
  std::string folder;
  uint8_t flavor;
};

/**
 * \brief Main class to compute centerlines from rock samples.
 */
class CenterlineManager {
 private:
  using gradient_point_type = skeleton::Point<3, double>;
  using Image3D = TernaryImage<Voxel>;
  using AnnotatedImage3D = AnnotatedImage<Voxel, Annotation<Voxel>>;
  using VoxelBuilder = PointBuilder;
  using VoxelNeighbourCalculator = TwentySixNeighbourCalculator<Voxel,
                                                                VoxelBuilder>;
  using ContourCalculator = skeleton::ContourCalculator<Image3D,
                                                        AnnotatedImage3D,
                                                        VoxelNeighbourCalculator>;

  using AnnotationType = typename AnnotatedImage3D::annotation_type;
  using PathCalculator = QuadraticPathCalculator<AnnotationType, Voxel>;
  using IntegerImage3D = IntegerImage<Voxel, int32_t>;
  using SkeletonImage = IntegerImage3D;
  using Skeletonizer3D = SkeletonizerByIFT<Image3D,
                                           AnnotatedImage3D,
                                           VoxelNeighbourCalculator,
                                           ContourCalculator,
                                           PathCalculator,
                                           IntegerImage3D,
                                           SkeletonImage>;
  using Gradient3D = Gradient<Image3D,
                              AnnotatedImage3D,
                              VoxelNeighbourCalculator,
                              VoxelBuilder,
                              gradient_point_type>;

  using RealDistanceType = typename PathCalculator::real_distance_type;

  using DistanceType = typename PathCalculator::real_distance_type;

  using PointAndDistance = Vertex<Voxel, DistanceType>;

  using AnnotatedVertexType = AnnotatedVertexHelper<PointAndDistance,
                                                    RealDistanceType>;

  using WeightCalculatorType = WeightCalculator<PointAndDistance,
                                                PathCalculator>;

  using GraphNeighbourCalculatorType = VertexNeighbourCalculator<PointAndDistance,
                                                                 VoxelNeighbourCalculator,
                                                                 AnnotatedImage3D>;

  using KeyIndexConverter = Converter<AnnotatedVertexType, Image3D>;

  using SpeedGraphType = SpeedGraph<AnnotatedVertexType,
                                    GraphNeighbourCalculatorType,
                                    KeyIndexConverter>;
  using MemoryGraphType = MemoryGraph<AnnotatedVertexType,
                                      GraphNeighbourCalculatorType>;
  using MemoryClustersDiscovererType = MaximalClustersDiscoverer<MemoryGraphType>;
  using SpeedClustersDiscovererType = MaximalClustersDiscoverer<SpeedGraphType>;

  using SpeedGraphBuilderType = SpeedGraphBuilder<SpeedGraphType,
                                                  AnnotatedImage3D,
                                                  Image3D,
                                                  GraphNeighbourCalculatorType>;

  using MemoryGraphBuilderType = MemoryGraphBuilder<MemoryGraphType,
                                                    AnnotatedImage3D,
                                                    GraphNeighbourCalculatorType>;

  using SpeedDijkstraAlgorithm = Dijkstra<SpeedGraphType,
                                          SpeedGraphType,
                                          PathCalculator,
                                          WeightCalculatorType,
                                          Gradient3D,
                                          SpeedClustersDiscovererType,
                                          HandleBasedBinaryHeap>;

  using MemoryDijkstraAlgorithm = Dijkstra<MemoryGraphType,
                                           MemoryGraphType,
                                           PathCalculator,
                                           WeightCalculatorType,
                                           Gradient3D,
                                           MemoryClustersDiscovererType,
                                           HandleBasedBinaryHeap>;

  using EndpointsDiscovererType = CenterpointDiscoverer<AnnotatedImage3D,
                                                        VoxelNeighbourCalculator,
                                                        PointAndDistance,
                                                        Image3D>;

  using CenterlineCalculatorType = CenterlineCalculator<SpeedDijkstraAlgorithm,
                                                        Skeletonizer3D,
                                                        EndpointsDiscovererType,
                                                        Gradient3D,
                                                        SpeedClustersDiscovererType>;

  using MemoryCenterlineCalculatorType = CenterlineCalculator<MemoryDijkstraAlgorithm,
                                                              Skeletonizer3D,
                                                              EndpointsDiscovererType,
                                                              Gradient3D,
                                                              MemoryClustersDiscovererType>;

  using CenterlineSetType = CenterlineSet<SpeedGraphType, PathCalculator>;

  template<class TFactory>
  typename TFactory::set_type
  ComputeCenterlines(const arma::Cube<uint8_t> &image,
                     const Skeletonizer3D &skeletonizer,
                     const TFactory &factory) const {
    auto graph_builder = factory.GetNewGraphBuilder();
    auto graph = graph_builder.Build(skeletonizer.GetAnnotations(),
                                     skeletonizer.GetImage());
    using size_type = typename TFactory::size_type;
    std::array<size_type, 3> sizes = {{static_cast<size_type>(image.n_rows),
                                       static_cast<size_type>(image.n_cols),
                                       static_cast<size_type>(image.n_slices)}};
    auto calculator = factory.GetNewCalculator(sizes);
    auto centerlines = calculator.ComputeCenterlines(graph,
                                                     skeletonizer.GetImage(),
                                                     skeletonizer.GetAnnotations());
    return centerlines;
  }

  std::unique_ptr<Skeletonizer3D> PrepareImage(const arma::Cube<uint8_t> &image) const {
    Image3D::array sizes = {{static_cast<Image3D::size>(image.n_rows),
                             static_cast<Image3D::size>(image.n_cols),
                             static_cast<Image3D::size>(image.n_slices)}};
    auto image_ptr = std::make_unique<Image3D>(image, sizes);
    auto ptr = std::make_unique<Skeletonizer3D>(std::move(image_ptr));
    ptr->ComputeSkeleton();
    return ptr;
  }

  template<class TSet>
  std::tuple<IntegerImage3D, IntegerImage3D, IntegerImage3D> FillImage(
      const TSet &centerlines) const {
    IntegerImage3D centerlines_image;
    IntegerImage3D centerlines_dist_image;
    IntegerImage3D centerlines_merged_image;
    for (const auto &centerline : centerlines) {
      for (std::size_t index = 0; index != centerline.GetNumPoints(); ++index) {
        const auto &node = centerline[index];
        auto pt_and_prop = node.Point();
        if (!centerlines_image.HasAnnotation(pt_and_prop.Point())) {
          centerlines_dist_image.AddPointAnnotation(pt_and_prop.Point(),
                                                    pt_and_prop.PropertyValue());

          if (index == 0 || index == centerline.GetNumPoints() - 1)
            centerlines_image.AddPointAnnotation(pt_and_prop.Point(), 2);
          else
            centerlines_image.AddPointAnnotation(pt_and_prop.Point(), 1);
          centerlines_merged_image
              .AddPointAnnotation(pt_and_prop.Point(),
                                  pt_and_prop.PropertyValue());
        }
      }
    }
    return std::make_tuple(centerlines_image,
                           centerlines_dist_image,
                           centerlines_merged_image);
  }

  void ExportCenterlines(const Settings &settings,
                         const JSONExporter<IntegerImage3D> &exporter,
                         const std::tuple<IntegerImage3D,
                                          IntegerImage3D,
                                          IntegerImage3D> &centerlines_images) const {
    exporter.ExportAnnotation(settings.folder, std::get<2>(centerlines_images));
  }

  template<class TStatistics>
  std::vector<std::string> ConvertStatisticToStrings(
      const TStatistics &statistic) const {
    std::vector<std::string> values;
    values.push_back(std::to_string(statistic.Size()));
    values.push_back(std::to_string(statistic.Tortuosity()));
    values.push_back(std::to_string(statistic.AveragePropertyValue()));
    return values;
  }

  template<class TSet>
  void ExportStatistics(const TSet &set,
                        const Settings &settings) const {
    std::vector<std::vector<std::string>> out_statistics;
    for (const auto &statistic : set.Statistics()) {
      auto values = ConvertStatisticToStrings(statistic);
      out_statistics.push_back(values);
    }
    FileUtils::WriteCsv(settings.folder + "/centerlines.stat", out_statistics);
  }

  template<class TSet>
  void SaveOutput(TSet set,
                  const Settings &settings) const {
    JSONExporter<IntegerImage3D> exporter;
    auto centerlines_images = FillImage(set);
    ExportCenterlines(settings, exporter, centerlines_images);
    ExportStatistics(set, settings);
  }

  void TestOutputLocations(const Settings &settings) const {
    FileUtils::AssertFolderExists(settings.folder);
  }

  template<class TFactory>
  void ExecuteAndSave(const arma::Cube<uint8_t> &image,
                      const std::unique_ptr<Skeletonizer3D> &skeletonizer_ptr,
                      const Settings &settings,
                      const TFactory &factory) const {
    auto centerlines = ComputeCenterlines(image, *skeletonizer_ptr, factory);
    SaveOutput(centerlines, settings);
  }

 public:
  /**
   * This class implements a factory design pattern for instantiating the
   * relevant classes for the speed optimized version of the code.
   */
  class SpeedFactory {
   public:
    using size_type = typename CenterlineCalculatorType::size_type;
    using set_type = typename CenterlineCalculatorType::set_type;

    SpeedGraphBuilderType GetNewGraphBuilder() const {
      return SpeedGraphBuilderType{};
    }

    CenterlineCalculatorType GetNewCalculator(
        const std::array<size_type, 3> &sizes) const {
      return CenterlineCalculatorType{sizes};
    }
  };

  /**
   * This class implements a factory design pattern for instantiating the
   * relevant classes for the memory optimized version of the code.
   */
  class MemoryFactory {
   public:
    using size_type = typename MemoryCenterlineCalculatorType::size_type;
    using set_type = typename MemoryCenterlineCalculatorType::set_type;

    MemoryGraphBuilderType GetNewGraphBuilder() const {
      return MemoryGraphBuilderType{};
    }

    MemoryCenterlineCalculatorType GetNewCalculator(
        const std::array<size_type, 3> &sizes) const {
      return MemoryCenterlineCalculatorType{sizes};
    }
  };

  /**
   * \brief Main method to compute centerlines. Centerlines are computed
   * using the Dijkstra algorithm for solving the single source minimum path
   * problem. Internally, each input / output pore has a centerpoint, which
   * is used as a source point for the Dijkstra's algorithm. Moreover, this
   * method employs a standard implementation of a binary heap as a priority
   * queue to the Dijkstra's algorithm.
   *
   * \param image An image represented by a matrix of integers. This method
   * assumes that pores are 0-valued.
   *
   * \param settings The configurations.
   */
  void ComputeCenterlines(const arma::Cube<uint8_t> &image,
                          const Settings &settings) const {
    TestOutputLocations(settings);
    auto skeletonizer_ptr = PrepareImage(image);

    if (settings.flavor == 0) {
      auto factory = SpeedFactory{};
      ExecuteAndSave(image, skeletonizer_ptr, settings, factory);
    } else {
      auto factory = MemoryFactory{};
      ExecuteAndSave(image, skeletonizer_ptr, settings, factory);
    }
  }
};  // end of class CenterlineManager

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_INCLUDES_H_
