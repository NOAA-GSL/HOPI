

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RBFInterpolator


def func(xyz):
    #return 3.0 + 4.3*xyz[:,0] - 5.1*xyz[:,1] - 2.2*xyz[:,2]
    return 1.0 + np.sum(xyz, axis=1)*np.exp(-6*np.sum(xyz**2, axis=1))

# Random locations between 0-1 with shape(NPts, NDim)
source_xyz = np.random.rand(10000, 3)
target_xyz = np.random.rand(10000, 3)

# Generate Function Values at Samples
source_f = func(source_xyz)
target_f = func(target_xyz)

# Jack the whole thing by repeating source locations
#source_xyz = np.vstack((source_xyz,source_xyz))
#source_f   = np.hstack((source_f,source_f+0.1))

#unique_sources = np.unique(source_xyz, axis=0, return_index=True)
#print(unique_sources)

# Interpolate to Targets
rbf = RBFInterpolator(source_xyz, source_f, kernel='thin_plate_spline', neighbors=100, degree=1)
interp_f = rbf(target_xyz)

# Error
error_f = np.abs(interp_f - target_f)/target_f
print('Max Error = %12.3e'%(np.max(error_f)))
print('Std Error = %12.3e'%(np.std(error_f)))