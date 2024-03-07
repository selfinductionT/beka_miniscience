'''
u = x^3 + y^3 <=> Laplacian(u) = 6*x + 6*y

On the boundary of unit sircle u = (x + y)(x^2 - xy + y^2) =
= (x + y)(1 - xy)

'''

from dolfin import *

from mshr import *
domain = Circle(Point(0, 0), 1)
mesh = generate_mesh(domain, 64)
V = FunctionSpace(mesh, "Lagrange", 1)

def boundary(x):
    ro = sqrt(x[0]**2 + x[1]**2)
    return ro < 1.0 + DOLFIN_EPS or ro > 1.0 - DOLFIN_EPS

# Define boundary condition
u0 = Expression('(x[0] + x[1]) * (1 - x[0]*x[1])', degree=3)
bc = DirichletBC(V, u0, boundary)

# Define variational problem
u = TrialFunction(V)
v = TestFunction(V)
f = Expression('6*x[0] + 6*x[1]', degree=3)
a = inner(grad(u), grad(v))*dx
L = f*v*dx

# Compute solution
u = Function(V)
solve(a == L, u, bc)

# Save solution in VTK format
file = File("poisson/poisson.pvd")
file << u

# Plot solution
import matplotlib.pyplot as plt
plot(u)
plt.show()
