// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2020 Intel Corporation. All Rights Reserved.

#pragma once

#include "calibration-types.h"


namespace librealsense {
namespace algo {
namespace depth_to_rgb_calibration {

    void ndgrid_my(const double vec1[5], const double vec2[5], double yScalingGrid[25], double xScalingGrid[25]); //auto generated code
    void inv(const double x[9], double y[9]); //auto generated code
    void transpose(const double x[9], double y[9]);
    void rotate_180(const uint8_t* A, uint8_t* B, uint32_t w, uint32_t h); //auto generated code
    std::vector< double > interp1( const std::vector< double > & ind,
                                   const std::vector< double > & vals,
                                   const std::vector< double > & intrp );
    double get_norma(const std::vector<double3>& vec);
    double rad_to_deg(double rad); 
    double deg_to_rad(double deg);
    void direct_inv_2x2(const double A[4], const double B[2], double C[2]);
    void direct_inv_6x6(const double A[36], const double B[6], double C[6]);

    double3x3 cholesky3x3( double3x3 const & mat );

}
}
}

