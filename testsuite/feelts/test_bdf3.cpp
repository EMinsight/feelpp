/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t -*- vim:fenc=utf-8:ft=cpp:et:sw=4:ts=4:sts=4

  This file is part of the Feel library

  Author(s): Stephane Veys <stephane.veys@imag.fr>
             Christophe Prud'homme <christophe.prudhomme@feelpp.org>
       Date: 2013-12-28

  Copyright (C) 2011-2014 Feel++ Consortium

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//#define USE_BOOST_TEST 1
#if defined(USE_BOOST_TEST)
#define BOOST_TEST_MODULE test_bdf3
#include <testsuite/testsuite.hpp>
#endif

#include <feel/feelalg/backend.hpp>
#include <feel/feelts/bdf.hpp>
#include <feel/feeldiscr/pch.hpp>
#include <feel/feelfilters/loadmesh.hpp>
#include <feel/feelfilters/exporter.hpp>
#include <feel/feelvf/vf.hpp>

/** use Feel namespace */
using namespace Feel;
using Feel::project;

inline
AboutData
makeAbout()
{
    AboutData about( "test_bdf3" ,
                     "test_bdf3" ,
                     "0.2",
                     "nD(n=2,3) test bdf3",
                     Feel::AboutData::License_GPL,
                     "Copyright (c) 2015 Feel++ Consortium" );

    about.addAuthor( "Guillaume Dollé", "developer", "gdolle@unistra.fr", "" );
    return about;

}

inline
Feel::po::options_description
makeOptions()
{
    Feel::po::options_description opts( "Test Environment options" );
    opts.add_options()
        ( "N", Feel::po::value<int>()->default_value( 2 ), "Number of problems to solve successively" )
    ;
    return opts.add( Feel::feel_options() );
}

template<int Dim>
class Test:
    public Simget
{
public :

    void run()
    {
        using mesh_type = Mesh<Simplex<Dim,1>>;
        using element_type = typename Pch_type<mesh_type,1>::element_type;
        using timeset_type = typename Exporter<mesh_type>::timeset_type;
        using timeset_ptrtype = boost::shared_ptr<timeset_type>;

        auto mesh = loadMesh( _mesh=new Mesh<Simplex<Dim,1>> );
        const int N = ioption("N");
        const double c = -0.2;
        auto Xh = Pch<1>( mesh );
        auto u = Xh->element();
        auto v = Xh->element();

        std::vector<element_type> U;
        for( int i =0; i<N; i++ )
            U.push_back( Xh->element() );

        auto mybdf = bdf( _space=Xh, _name="mybdf" );

        //stiffness matrix
        auto a = form2( _test=Xh, _trial=Xh );
        auto at = form2( _test=Xh, _trial=Xh ); // Time dependent.
        auto e = exporter( _mesh=mesh, _name="test_bdf" );

        a = integrate( _range = elements( mesh ),
                       _expr = gradt( u )*trans( grad( v ) )
                               + c * mybdf->polyDerivCoefficient(0)*idt(u)*id(v) );

        auto l = form1(_test=Xh);
        auto lt = form1(_test=Xh); // Time dependent.

        at += on( _range=boundaryfaces(mesh),
                _rhs=l,
                _element=u,
                _expr=cst(0.) );

        // Initialize bdf unknowns.
        mybdf->initialize( u );
        for( const auto& ui : U)
            mybdf->initialize( ui );

        for( int i=0; i<N; i++ )
        {
            LOG(INFO) << "test bdf source: " << i;
            l = integrate( _range=elements( mesh ),
                           _expr=(i+1)*cst(100.)  );

            // Create timeset for each rhs.
            if( e->doExport() and ( i > 0 ) )
            {
                std::string prefix = ( boost::format( "rhs_%1%" ) % i ).str();
                auto timeset = boost::make_shared<timeset_type>( timeset_type( prefix ) );
                e->addTimeSet( timeset );
                e->setMesh( mesh, EXPORTER_GEOMETRY_STATIC);
                e->setPrefix( prefix );
            }

            for( mybdf->start();  mybdf->isFinished() == false; mybdf->next() )
            {
                at=a;
                lt=l;

                auto bdf_poly = mybdf->polyDeriv();

                lt += integrate( _range=elements(mesh),
                                 _expr=c*idv(bdf_poly)*id(v) );

                at += on( _range=boundaryfaces(mesh),
                          _rhs=lt,
                          _element=U[i],
                          _expr=cst(0.) );

                at.solve( _rhs=lt,
                          _solution=U[i] );

                LOG(INFO) << "test bdf export: " << i;
                //auto time = mybdf->time();
                auto time = mybdf->time();
                if( e->doExport() )
                {
                    e->step(time)->add( (boost::format("u%1%") % i ).str(), U[i] );
                }

                mybdf->shiftRight( U[i] );
                if( e->doExport() )
                {
                    e->save();
                }
            } // Bdf loop.a
        } // Source loop.
   } // Run.
};


#if defined(USE_BOOST_TEST)
FEELPP_ENVIRONMENT_WITH_OPTIONS( makeAbout(), feel_options() );
BOOST_AUTO_TEST_SUITE( bdf3 )

BOOST_AUTO_TEST_CASE( test_1 )
{
    Test<2> test;
    test.run();
}

BOOST_AUTO_TEST_SUITE_END()
#else
int main(int argc, char** argv )
{
    Feel::Environment env( _argc=argc, _argv=argv,
                           _desc=makeOptions(),
                           _about=makeAbout() );
    Test<2>  test;
    test.run();
}
#endif