

namespace optframe {

// Weighted and support to lexicographic computation
// WLxEvaluation: depend on extended_arithmetics (+, - and *)
template <optframe::extended_arithmetics ObjType = evtype>
class WLxEvaluation final : public Component {
 protected:
  // ==== Objective Space type: pair<evtype, evtype> ====
  // objective function value (default = double)
  ObjType objFunction;
  // infeasibility measure value (default = double)
  ObjType infMeasure;
  // for lexicographic approaches, use these extra evaluation values
  vector<pair<ObjType, ObjType>> alternatives;

  // ==== Objective Space auxiliary information ====
  // LocalOptimum Status: mapping 'move.id()' to 'NeighborhoodStatus'
  // map<string, bool> localStatus; // TODO: REMOVE!
  // GlobalOptimumStatus (for exact methods only)
  enum GOS {
    gos_yes,
    gos_no,
    gos_unknown
  } gos;

  // LocalOptimumStatus
  enum LOS {
    los_yes,
    los_no,
    los_unknown
  };  // do not declare here (keep in ADS or R)

 public:
  // boolean field to indicate if Evaluation needs an update
  bool outdated;
  // boolean field to indicate if Evaluation value is an estimation (not exact)
  bool estimated;
  // constant to mutiply infeasibility weight
  ObjType weight;

  // ======================================
  // begin canonical part

  WLxEvaluation(const ObjType& obj, const ObjType& inf, const evtype& w = 1)
      : objFunction(obj), infMeasure(inf), weight(w) {
    gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  WLxEvaluation(const ObjType& obj)
      : objFunction(obj) {
    weight = 1;
    //infMeasure = optframe::get_numeric_zero<ObjType>();
    optframe::numeric_zero(infMeasure);

    gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  WLxEvaluation(const WLxEvaluation& e)
      : objFunction(e.objFunction), infMeasure(e.infMeasure), alternatives(e.alternatives), gos(e.gos), outdated(e.outdated), estimated(e.estimated), weight(e.weight) {
  }

  virtual ~WLxEvaluation() {
  }

  virtual WLxEvaluation& operator=(const WLxEvaluation& e) {
    if (&e == this)  // auto ref check
      return *this;

    objFunction = e.objFunction;
    infMeasure = e.infMeasure;
    outdated = e.outdated;
    estimated = e.estimated;
    weight = e.weight;
    alternatives = e.alternatives;
    gos = e.gos;

    return *this;
  }

  virtual WLxEvaluation& clone() const {
    return *new WLxEvaluation(*this);
  }

  // end canonical part
  // ======================================
  // begin Evaluation methods

  ObjType getObjFunction() const {
    return objFunction;
  }

  ObjType getInfMeasure() const {
    return infMeasure;
  }

  evtype getWeight() const {
    return weight;
  }

  const vector<pair<ObjType, ObjType>>& getAlternativeCosts() const {
    return alternatives;
  }

  void setObjFunction(ObjType obj) {
    objFunction = obj;
  }

  void setInfMeasure(ObjType inf) {
    infMeasure = inf;
  }

  void setWeight(const evtype& w) {
    weight = w;
  }

  void addAlternativeCost(const pair<ObjType, ObjType>& alternativeCost) {
    alternatives.push_back(alternativeCost);
  }

  void setAlternativeCosts(const vector<pair<ObjType, ObjType>>& alternativeCosts) {
    alternatives = alternativeCosts;
  }

  // -----------------
  // for local optimum
  // -----------------

  // TODO: remove! LOS management is now on NSSeq and NSSeqIterators
  /*
	bool getLocalOptimumStatus(string moveId)
	{
		return localStatus[moveId];
	}

	void setLocalOptimumStatus(string moveId, bool status)
	{
		localStatus[moveId] = status;
	}
	*/

  // ------------------
  // for global optimum
  // ------------------

  GOS getGlobalOptimumStatus() {
    return gos;
  }

  void setGlobalOptimumStatus(GOS status) {
    gos = status;
  }

  // evaluation = objFunction + weight*infMeasure
  // note that, if 'evtype' becomes complex, one must return a moveable copy, not reference of internal value
  ObjType evaluation() const {
    assert(!outdated);
    return objFunction + weight * infMeasure;
  }

  // ========= TAKEN FROM MoveCost =======

  // update Evaluation with costs
  virtual void update(Evaluation<ObjType>& evCost) {
    // this task was performed before by MoveCost... now unifying in Evaluation
    // how to do this?
    assert(false);
  }

  virtual WLxEvaluation<ObjType> diff(const WLxEvaluation<ObjType>& evFinal) {
    const WLxEvaluation<ObjType>& e = evFinal;  // pointing to legacy code

    // take my own information
    // -----------------------

    pair<ObjType, ObjType> e_begin = make_pair(this->getObjFunction(), this->getInfMeasure());
    // get original values for lexicographic part
    vector<pair<evtype, evtype>> alt_begin(this->getAlternativeCosts().size());
    for (unsigned i = 0; i < alt_begin.size(); i++) {
      alt_begin[i].first = this->getAlternativeCosts()[i].first;
      alt_begin[i].second = this->getAlternativeCosts()[i].second;
    }

    // compute cost difference
    // -----------------------
    WLxEvaluation<ObjType> mcost(e.getObjFunction() - e_begin.first, e.getInfMeasure() - e_begin.second, 1);  // no outdated or estimated

    // guarantee that alternative costs have same size
    assert(alt_begin.size() == e.getAlternativeCosts().size());

    // compute alternative costs
    for (unsigned i = 0; i < alt_begin.size(); i++)
      mcost.addAlternativeCost(make_pair(e.getAlternativeCosts()[i].first - alt_begin[i].first, e.getAlternativeCosts()[i].second - alt_begin[i].second));

    return WLxEvaluation<ObjType>(mcost);
  }

  virtual bool isEstimated() {
    return false;  // TODO: created good flag here??
  }

  // =========
  // finish MoveCost
  // =========

  // leave option to rewrite tolerance (or consider lexicographic values)
  virtual bool isFeasible() const {
    //return optframe::numeric_is_zero<ObjType>(infMeasure);
    // IMPORTANT: numeric_is_zero can come from anywhere!
    return optframe::numeric_is_zero(infMeasure);
    //return (EVALUATION_ABS(infMeasure) <= optframe::get_numeric_zero<ObjType>()); // deprecated
  }

  // ======================
  // Object specific part
  // ======================

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":WLxEvaluation";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  void print() const override {
    cout << toString() << endl;
  }

  std::string toString() const override {
    stringstream ss;
    ss << fixed;  // disable scientific notation
    ss << "WLxEvaluation function value = " << evaluation();
    ss << (isFeasible() ? " " : " (not feasible) ");
    ss << " w= " << weight << " ";
    ss << (outdated ? " OUTDATED " : " ");
    ss << (estimated ? " ESTIMATED " : " ");
    if (alternatives.size() > 0) {
      ss << " alternative costs: ";
      for (unsigned i = 0; i < alternatives.size(); i++)
        ss << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
    }
    // ss << endl;

    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os, const WLxEvaluation& me) {
    os << me.toString();
    return os;
  }
};

}  // namespace optframe