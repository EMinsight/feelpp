h=0.1;
Point(1) = {0.0000000000000000e+00,-5.0000000000000000e-01,0.0000000000000000e+00,h};
Point(2) = {6.0000000000000000e+00,-5.0000000000000000e-01,0.0000000000000000e+00,h};
Point(3) = {6.0000000000000000e+00,5.0000000000000000e-01,0.0000000000000000e+00,h};
Point(4) = {0.0000000000000000e+00,5.0000000000000000e-01,0.0000000000000000e+00,h};
Line(1) = { 1,2};
Line(2) = { 2,3};
Line(3) = { 3,4};
Line(4) = { 4,1};
Line Loop(1) = {1,2,3,4};
Plane Surface(1) = {1};

Physical Point("solid-fixed") = {1,2,3,4};
Physical Line("fluid-inlet") = {4};
Physical Line("fsi-wall") = {1,3};
Physical Line("fluid-outlet") = {2};
Physical Surface("Fluid") = {1};
