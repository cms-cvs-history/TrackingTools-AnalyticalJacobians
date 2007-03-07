#include "TrackingTools/AnalyticalJacobians/interface/JacobianCurvilinearToLocal.h"
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "TrackingTools/TrajectoryParametrization/interface/LocalTrajectoryParameters.h"
#include "MagneticField/Engine/interface/MagneticField.h"

JacobianCurvilinearToLocal::
JacobianCurvilinearToLocal(const Surface& surface, 
			   const LocalTrajectoryParameters& localParameters,
			   const MagneticField& magField) : theJacobian(5, 5, 0) {
 
  // Origin: TRSCSD
  GlobalVector tn = surface.toGlobal(localParameters.momentum()).unit();
  GlobalVector dj = surface.toGlobal(LocalVector(1., 0., 0.));
  GlobalVector dk = surface.toGlobal(LocalVector(0., 1., 0.));
  GlobalVector di = surface.toGlobal(LocalVector(0., 0., 1.));
  LocalVector tnl = surface.toLocal(tn);
  // rotate coordinates because of wrong coordinate system in orca
  LocalVector tvw(tnl.z(), tnl.x(), tnl.y());
  double cosl = tn.perp(); if (cosl < 1.e-30) cosl = 1.e-30;
  double cosl1 = 1./cosl;
  GlobalVector un(-tn.y()*cosl1, tn.x()*cosl1, 0.);
  GlobalVector vn(-tn.z()*un.y(), tn.z()*un.x(), cosl);
  double uj = un.dot(dj);
  double uk = un.dot(dk);
  double vj = vn.dot(dj);
  double vk = vn.dot(dk);
  double t1r = 1./tvw.x();
  double t2r = t1r*t1r;
  double t3r = t1r*t2r;
  theJacobian(1,1) = 1.;
  theJacobian(2,2) = -uk*t2r;
  theJacobian(2,3) = vk*cosl*t2r;
  theJacobian(3,2) = uj*t2r;
  theJacobian(3,3) = -vj*cosl*t2r;
  theJacobian(4,4) = vk*t1r;
  theJacobian(4,5) = -uk*t1r;
  theJacobian(5,4) = -vj*t1r;
  theJacobian(5,5) = uj*t1r;
  GlobalPoint  x = surface.toGlobal(localParameters.position());
  //  GlobalVector h = MagneticField::inInverseGeV(x);
  GlobalVector h  = magField.inTesla(x) * 2.99792458e-3;
  double q = -h.mag() * localParameters.signedInverseMomentum();
  double sinz =-un.dot(h.unit());
  double cosz = vn.dot(h.unit());
  double ui = un.dot(di);
  double vi = vn.dot(di);
  theJacobian(2,4) =-q*ui*(vk*cosz-uk*sinz)*t3r;
  theJacobian(2,5) =-q*vi*(vk*cosz-uk*sinz)*t3r;
  theJacobian(3,4) = q*ui*(vj*cosz-uj*sinz)*t3r;
  theJacobian(3,5) = q*vi*(vj*cosz-uj*sinz)*t3r;
  // end of TRSCSD
}

const AlgebraicMatrix& JacobianCurvilinearToLocal::jacobian() const {
  return theJacobian;
}
