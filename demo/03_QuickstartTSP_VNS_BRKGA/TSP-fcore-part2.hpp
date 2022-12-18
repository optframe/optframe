
// TSP problem context and data reads
class ProblemContext {
 public:
  int n{0};          // number of clients
  Matrix<int> dist;  // distance matrix (Euclidean)
  // load data from Scanner
  void load(Scanner& scanner) {
    n = *scanner.nextInt();    // reads number of clients
    dist = Matrix<int>(n, n);  // initializes n x n matrix
    //
    vector<double> xvalues(n);
    vector<double> yvalues(n);
    //
    for (int i = 0; i < n; i++) {
      scanner.next();
      xvalues[i] = *scanner.nextDouble();  // reads x
      yvalues[i] = *scanner.nextDouble();  // reads y
    }
    // calculate distance values, for every client pair (i,j)
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        dist(i, j) = (int)::round(distance(xvalues.at(i), yvalues.at(i),
                                           xvalues.at(j), yvalues.at(j)));
  }
  // euclidean distance (double as return)
  double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
};
// Create TSP Problem Context
// ProblemContext pTSP;
