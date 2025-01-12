//! -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*- vim:fenc=utf-8:ft=cpp:et:sw=4:ts=4:sts=4
//!
//! This file is part of the Feel++ library
//!
//! This library is free software; you can redistribute it and/or
//! modify it under the terms of the GNU Lesser General Public
//! License as published by the Free Software Foundation; either
//! version 2.1 of the License, or (at your option) any later version.
//!
//! This library is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//! Lesser General Public License for more details.
//!
//! You should have received a copy of the GNU Lesser General Public
//! License along with this library; if not, write to the Free Software
//! Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//!
//! @file
//! @author Christophe Prud'homme <christophe.prudhomme@feelpp.org>
//! @date 23 Jul 2021
//! @copyright 2017 Feel++ Consortium
//!
#include <feel/feelpython/pybind11/pybind11.h>

#include <feel/feeldiscr/mesh.hpp>
#include <feel/feeldiscr/meshstructured.hpp>
#include <feel/feelmesh/filters.hpp>
#include <feel/feelvf/vf.hpp>
#include <mpi4py/mpi4py.h>
#include <feel/feelpython/pybind11/eigen.h>
#include <feel/feelcore/environment.hpp>
           namespace py = pybind11;

template <typename MeshT>
void defIntegrate( py::module& m )
{
    using namespace Feel;
    using mesh_t = MeshT;
    using mesh_ptr_t = std::shared_ptr<mesh_t>;
    constexpr int Dim = MeshT::nDim;
    constexpr int Order = MeshT::nOrder;
    constexpr int RealDim = MeshT::nRealDim;

    m.def(
        "integrate", []( Range<mesh_t,MESH_ELEMENTS> const& r, std::string const& e, int quad_order )
        { return integrate( _range = r, _expr = expr( e ), _quad = quad_order ).evaluate(); },
        py::arg( "range" ), py::arg( "expr" ) = "1", py::arg( "quad" ) = 1, "compute the integral of the expression over the range of elements" );
    m.def(
        "integrate", []( Range<mesh_ptr_t,MESH_ELEMENTS> const& r, std::string const& e, int quad_order )
        { return integrate( _range = r, _expr = expr( e ), _quad = quad_order ).evaluate(); },
        py::arg( "range" ), py::arg( "expr" ) = "1", py::arg( "quad" ) = 1, "compute the integral of the expression over the range of elements" );
    if constexpr ( mesh_t::nDim > 1 )
    {
        m.def(
            "integrate", []( Range<mesh_t,MESH_FACES> const& r, std::string const& e, int quad_order )
            { return integrate( _range = r, _expr = expr( e ), _quad = quad_order ).evaluate(); },
            py::arg( "range" ), py::arg( "expr" ) = "1", py::arg( "quad" ) = 1, "compute the integral of the expression over the range of facets" );
        m.def(
            "integrate", []( Range<mesh_ptr_t,MESH_FACES> const& r, std::string const& e, int quad_order )
            { return integrate( _range = r, _expr = expr( e ), _quad = quad_order ).evaluate(); },
            py::arg( "range" ), py::arg( "expr" ) = "1", py::arg( "quad" ) = 1, "compute the integral of the expression over the range of facets" );
    }
#if 0
    if constexpr ( mesh_t::nDim == 3 )
    {
        m.def(
            "Integrate", []( Range<mesh_t,MESH_EDGES> const& r, std::string const& e, int quad_order )
            { return integrate( _range = r, _expr = expr( e ), _quad = quad_order ).evaluate(); },
            py::arg( "range" ), py::arg( "expr" ) = "1", py::arg( "quad" ) = 1, "compute the integral of the expression over the range of edges" );
    }
#endif
}

PYBIND11_MODULE( _integrate, m )
{
    using namespace Feel;

    if ( import_mpi4py() < 0 ) return;

    // 1D
    defIntegrate<Mesh<Simplex<1, 1, 1>>>( m );
    // 2D
    defIntegrate<Mesh<Simplex<2, 1, 2>>>( m );
    // 3D
    defIntegrate<Mesh<Simplex<3, 1, 3>>>( m );
}
