//// License: Apache 2.0. See LICENSE file in root directory.
//// Copyright(c) 2020 Intel Corporation. All Rights Reserved.

#include "calibration.h"
#include "debug.h"
#include "utils.h"


using namespace librealsense::algo::depth_to_rgb_calibration;


calib::calib( rs2_intrinsics_double const & intrin, rs2_extrinsics_double const & extrin )
{
    auto const & r = extrin.rotation;
    auto const & t = extrin.translation;
    auto const & c = intrin.coeffs;

    height = intrin.height;
    width = intrin.width;
    rot = { r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8] };
    trans = { t[0], t[1], t[2] };
    k_mat = { intrin.fx, intrin.fy, intrin.ppx, intrin.ppy };
    coeffs[0] = c[0];
    coeffs[1] = c[1];
    coeffs[2] = c[2];
    coeffs[3] = c[3];
    coeffs[4] = c[4];
    model = intrin.model;
}

calib::calib( rs2_intrinsics const & intrin, rs2_extrinsics const & extrin )
{
    auto const & r = extrin.rotation;
    auto const & t = extrin.translation;
    auto const & c = intrin.coeffs;

    height = intrin.height;
    width = intrin.width;
    rot = { r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8] };
    trans = { t[0], t[1], t[2] };
    k_mat = { intrin.fx, intrin.fy, intrin.ppx, intrin.ppy };
    coeffs[0] = c[0];
    coeffs[1] = c[1];
    coeffs[2] = c[2];
    coeffs[3] = c[3];
    coeffs[4] = c[4];
    model = intrin.model;
}

rs2_intrinsics_double calib::get_intrinsics() const
{
    return {
        width, height,
        k_mat,
        model, coeffs };
}

rs2_extrinsics_double calib::get_extrinsics() const
{
    auto & r = rot.rot;
    auto & t = trans;
    return {
        { r[0] , r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8] },
        { t.t1, t.t2, t.t3 }
    };
}

p_matrix const calib::calc_p_mat() const
{
    auto r = rot.rot;
    auto t = trans;
    auto fx = k_mat.fx;
    auto fy = k_mat.fy;
    auto ppx = k_mat.ppx;
    auto ppy = k_mat.ppy;
    p_matrix p_mat = { fx* r[0] + ppx * r[6], fx* r[1] + ppx * r[7], fx* r[2] + ppx * r[8], fx* t.t1 + ppx * t.t3,
              fy* r[3] + ppy * r[6], fy* r[4] + ppy * r[7], fy* r[5] + ppy * r[8], fy* t.t2 + ppy * t.t3,
              r[6]                 , r[7]                 , r[8]                 , t.t3 };

    return p_mat;
}

void calib::copy_coefs( calib & obj ) const
{
    obj.width = this->width;
    obj.height = this->height;

    obj.coeffs[0] = this->coeffs[0];
    obj.coeffs[1] = this->coeffs[1];
    obj.coeffs[2] = this->coeffs[2];
    obj.coeffs[3] = this->coeffs[3];
    obj.coeffs[4] = this->coeffs[4];

    obj.model = this->model;
}

calib calib::operator*( double step_size ) const
{
    calib res;
    res.k_mat.fx = this->k_mat.fx * step_size;
    res.k_mat.fy = this->k_mat.fy * step_size;
    res.k_mat.ppx = this->k_mat.ppx * step_size;
    res.k_mat.ppy = this->k_mat.ppy *step_size;

    for (auto i = 0; i < 9; i++)
        res.rot.rot[i] = this->rot.rot[i] * step_size;

    res.trans.t1 = this->trans.t1 *step_size;
    res.trans.t2 = this->trans.t2 * step_size;
    res.trans.t3 = this->trans.t3 *step_size;

    copy_coefs( res );

    return res;
}

calib calib::operator/( double factor ) const
{
    return (*this)*(1.f / factor);
}

calib calib::operator+( const calib & c ) const
{
    calib res;
    res.k_mat.fx = this->k_mat.fx + c.k_mat.fx;
    res.k_mat.fy = this->k_mat.fy + c.k_mat.fy;
    res.k_mat.ppx = this->k_mat.ppx + c.k_mat.ppx;
    res.k_mat.ppy = this->k_mat.ppy + c.k_mat.ppy;

    for (auto i = 0; i < 9; i++)
        res.rot.rot[i] = this->rot.rot[i] + c.rot.rot[i];

    res.trans.t1 = this->trans.t1 + c.trans.t1;
    res.trans.t2 = this->trans.t2 + c.trans.t2;
    res.trans.t3 = this->trans.t3 + c.trans.t3;

    copy_coefs( res );

    return res;
}

calib calib::operator-( const calib & c ) const
{
    calib res;
    res.k_mat.fx = this->k_mat.fx - c.k_mat.fx;
    res.k_mat.fy = this->k_mat.fy - c.k_mat.fy;
    res.k_mat.ppx = this->k_mat.ppx - c.k_mat.ppx;
    res.k_mat.ppy = this->k_mat.ppy - c.k_mat.ppy;

    for (auto i = 0; i < 9; i++)
        res.rot.rot[i] = this->rot.rot[i] - c.rot.rot[i];

    res.trans.t1 = this->trans.t1 - c.trans.t1;
    res.trans.t2 = this->trans.t2 - c.trans.t2;
    res.trans.t3 = this->trans.t3 - c.trans.t3;

    copy_coefs( res );

    return res;
}

calib calib::operator/( const calib & c ) const
{
    calib res;
    res.k_mat.fx = this->k_mat.fx / c.k_mat.fx;
    res.k_mat.fy = this->k_mat.fy / c.k_mat.fy;
    res.k_mat.ppx = this->k_mat.ppx / c.k_mat.ppx;
    res.k_mat.ppy = this->k_mat.ppy / c.k_mat.ppy;

    for (auto i = 0; i < 9; i++)
        res.rot.rot[i] = this->rot.rot[i] / c.rot.rot[i];


    res.trans.t1 = this->trans.t1 / c.trans.t1;
    res.trans.t2 = this->trans.t2 / c.trans.t2;
    res.trans.t3 = this->trans.t3 / c.trans.t3;

    copy_coefs( res );

    return res;
}


p_matrix p_matrix::operator*(double step_size) const
{
    p_matrix res;

    for (auto i = 0; i < 12; i++)
        res.vals[i] = vals[i] * step_size;

    return res;
}

p_matrix p_matrix::operator/(double factor) const
{
    return (*this)*(1.f / factor);
}

p_matrix p_matrix::operator+(const p_matrix & c) const
{
    p_matrix res;
    for (auto i = 0; i < 12; i++)
        res.vals[i] = vals[i] + c.vals[i];

    return res;
}

p_matrix p_matrix::operator-(const p_matrix & c) const
{
    p_matrix res;
    for (auto i = 0; i < 12; i++)
        res.vals[i] = vals[i] - c.vals[i];

    return res;
}

p_matrix p_matrix::operator*(const p_matrix & c) const
{
    p_matrix res;
    for (auto i = 0; i < 12; i++)
        res.vals[i] = vals[i] * c.vals[i];

    return res;
}

p_matrix p_matrix::operator/(const p_matrix & c) const
{
    p_matrix res;
    for (auto i = 0; i < 12; i++)
        res.vals[i] = c.vals[i] == 0 ? 0 : vals[i] / c.vals[i];

    return res;
}

double p_matrix::get_norma() const
{
    double grads_norm = 0;

    for (auto i = 0; i < 12; i++)
        grads_norm += vals[i] * vals[i];

    grads_norm = sqrt(grads_norm);

    return grads_norm;
}

void svd( const double in[12], double out[3] );  // in svp.cpp
static inline bool rtIsNaN( double value ) { return (value != value); }
static inline bool rtIsInf( double value )
{
    return ( value == std::numeric_limits< double >::infinity()
             || value == -std::numeric_limits< double >::infinity() );
}

double p_matrix::matrix_norm() const
{
    // Code generated by Matlab Coder!
    double absx;
    double n = 0;
    double m[12];
    m[0] = vals[0];
    m[1] = vals[4];
    m[2] = vals[8];
    m[3] = vals[1];
    m[4] = vals[5];
    m[5] = vals[9];
    m[6] = vals[2];
    m[7] = vals[6];
    m[8] = vals[10];
    m[9] = vals[3];
    m[10] = vals[7];
    m[11] = vals[11];
    for( int j = 0; j < 4; j++ )
    {
        absx = std::abs( m[3 * j] );
        if( rtIsNaN( absx ) || ( absx > n ) )
            n = absx;

        absx = std::abs( m[1 + 3 * j] );
        if( rtIsNaN( absx ) || ( absx > n ) )
            n = absx;

        absx = std::abs( m[2 + 3 * j] );
        if( rtIsNaN( absx ) || ( absx > n ) )
            n = absx;
    }
    if( ( ! rtIsInf( n ) ) && ( ! rtIsNaN( n ) ) )
    {
        double dv0[3];
        svd( m, dv0 );
        n = dv0[0];
    }
    return n;
}

double p_matrix::sum() const
{
    double res = 0;
    for( auto i = 0; i < 12; i++ )
        res += vals[i];
    return res;
}

p_matrix p_matrix::normalize( double const norma ) const
{
    p_matrix res;
    for( auto i = 0; i < 12; i++ )
        res.vals[i] = vals[i] / norma;
    return res;
}

krt p_matrix::decompose() const
{
    //%firstThreeCols = P(:,1:3);% This is Krgb*R
    double3x3 first_three_cols = { vals[0], vals[1], vals[2],
                                   vals[4], vals[5], vals[6],
                                   vals[8], vals[9], vals[10] };

    //%KSquare = firstThreeCols*firstThreeCols';% This is Krgb*R*R'*Krgb' = Krgb*Krgb'
    auto k_square = first_three_cols * first_three_cols.transpose();
    //%KSquareInv = inv(KSquare); % Returns a matrix that is equal to: inv(Krgb')*inv(Krgb)
    std::vector<double> inv_k_square_vac( 9, 0 );
    inv( k_square.to_vector().data(), inv_k_square_vac.data() );

    double3x3 inv_k_square = { inv_k_square_vac[0], inv_k_square_vac[1], inv_k_square_vac[2],
                               inv_k_square_vac[3], inv_k_square_vac[4], inv_k_square_vac[5],
                               inv_k_square_vac[6], inv_k_square_vac[7], inv_k_square_vac[8] };

    //%KInv = cholesky3x3(KSquareInv)';% Cholsky decomposition 3 by 3. returns a lower triangular matrix 3x3. Equal to inv(Krgb')
    auto k_inv = cholesky3x3( inv_k_square ).transpose();
    //%K = inv(KInv);
    std::vector<double> k_vac( 9, 0 );
    inv( k_inv.to_vector().data(), k_vac.data() );
    //%K = K/K(end);
    for( auto i = 0; i < k_vac.size(); i++ )
        k_vac[i] /= k_vac[k_vac.size() - 1];

    //%t = KInv*P(:,4);
    auto t = k_inv * double3{ vals[3], vals[7], vals[11] };

    //%R = KInv*firstThreeCols;
    auto r = (k_inv * first_three_cols).to_vector();

    krt calibration;
    for( auto i = 0; i < r.size(); i++ )
        calibration.rot.rot[i] = r[i];

    calibration.trans = { t.x, t.y, t.z };

    calibration.k_mat.fx = k_vac[0];
    calibration.k_mat.fy = k_vac[4];
    calibration.k_mat.ppx = k_vac[2];
    calibration.k_mat.ppy = k_vac[5];

    return calibration;
}

calib librealsense::algo::depth_to_rgb_calibration::decompose( p_matrix const & in_mat,
                                                               calib const & in_calibration )
{
    auto krt = in_mat.decompose();

    calib calibration;
    calibration.rot = krt.rot;
    calibration.trans = krt.trans;
    calibration.k_mat = krt.k_mat;
    in_calibration.copy_coefs( calibration );

    return calibration;
}
