import numpy as np
import matplotlib.pyplot as plt

A = np.genfromtxt("matrix.txt",  autostrip=True)

# fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
# X =np.arange(60)
# Y =np.arange(60)
# X, Y = np.meshgrid(X, Y)
# Z = 


plt.imshow(A, cmap='hot', interpolation='nearest')
plt.show()

from mpl_toolkits.mplot3d import Axes3D

def surface_plot (matrix, **kwargs):
    # acquire the cartesian coordinate matrices from the matrix
    # x is cols, y is rows
    (x, y) = np.meshgrid(np.arange(matrix.shape[0]), np.arange(matrix.shape[1]))
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(x, y, matrix, **kwargs)
    return (fig, ax, surf)

# m.shape must be (10,10)
m = np.fromfunction(lambda x, y: np.sin(np.sqrt(x**2 + y**2)), (60, 60))

(fig, ax, surf) = surface_plot(A, cmap=plt.cm.coolwarm)

fig.colorbar(surf)

ax.set_xlabel('X (cols)')
ax.set_ylabel('Y (rows)')
ax.set_zlabel('Z (values)')

plt.show()