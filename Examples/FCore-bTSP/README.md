## FCore-bTSP

Bi-objective TSP

See: Thibaut Lust, Jacques Teghem. Two-phase Pareto local search for the biobjective traveling salesman problem (2010).
https://link.springer.com/content/pdf/10.1007/s10732-009-9103-9.pdf


---

Finding available packages with bazel.

local components:
bazel query 'attr(visibility, "//visibility:public", ...)'

remote OptFrame packages:
bazel query @OptFrame//...
bazel query 'attr(visibility, "//visibility:public", "@OptFrame//...")'


list label hdrs:
bazel query 'labels(hdrs, ...)'
