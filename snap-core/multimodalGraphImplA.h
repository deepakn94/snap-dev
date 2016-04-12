//#//////////////////////////////////////////////
/// Multimodal graph: Implementation A
/// Single graph with all nodes [with global ids], each node labelled
/// with appropriate attribute

class TMultimodalGraphImplA;

/// Pointer to a multimodal graph
typedef TPt<TMultimodalGraphImplA> PMultimodalGraphImplA;
typedef TIntNNet::TNodeI TNodeI;
typedef TIntNNet::TEdgeI TEdgeI;

//#//////////////////////////////////////////////
/// Multimodal graph. ##TMultimodalGraphImplA::Class
class TMultimodalGraphImplA {
public:
  TCRef CRef;
private:
  TInt MxNId, NEdges;
  // Encapsulate a network under the hood
  TIntNNet Network;
public:
  TMultimodalGraphImplA() : CRef(), MxNId(0), NEdges(0), Network() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TMultimodalGraphImplA(const int& Nodes, const int& Edges) : CRef(), MxNId(0), NEdges(0) { Reserve(Nodes, Edges); }
  TMultimodalGraphImplA(const TMultimodalGraphImplA& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), Network(Graph.Network) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TMultimodalGraphImplA(TSIn& SIn) : MxNId(SIn), NEdges(SIn), Network(Network) { }
  virtual ~TMultimodalGraphImplA() { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); Network.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PMultimodalGraphImplA Graph = TMultimodalGraphImplAGraph::New().
  static PMultimodalGraphImplA New() { return new TMultimodalGraphImplA(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TMultimodalGraphImplA::New
  static PMultimodalGraphImplA New(const int& Nodes, const int& Edges) { return new TMultimodalGraphImplA(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PMultimodalGraphImplA Load(TSIn& SIn) { return PMultimodalGraphImplA(new TMultimodalGraphImplA(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const { return Network.HasFlag(Flag); }
  TMultimodalGraphImplA& operator = (const TMultimodalGraphImplA& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; Network=Graph.Network; } return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return Network.GetNodes(); }
  /// Adds a node of ID NId and ModeId MId to the multimodal graph.
  int AddNode(const TPair<TInt,TInt>& NId);
  /// Deletes node of ID NId from the graph. ##TMultimodalGraphImplA::DelNode
  void DelNode(const TPair<TInt,TInt>& NId) { Network.DelNode(NId.GetVal2()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const TPair<TInt,TInt>& NId) const { return Network.IsNode(NId.GetVal2()); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return Network.BegNI(); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return Network.EndNI(); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const TPair<TInt,TInt>& NId) const { return Network.GetNI(NId.GetVal2()); }
  /// Returns an ID that is larger than any node ID in the graph.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const { return NEdges; }
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TMultimodalGraphImplA::AddEdge
  int AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TMultimodalGraphImplA::DelEdge
  void DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const { return Network.IsEdge(SrcNId.GetVal2(), DstNId.GetVal2()); }
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); if (GetNodes() != 0 && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { EI++; } return EI; }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. ##TMultimodalGraphImplA::GetEI
  TEdgeI GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const { return Network.GetEI(SrcNId.GetVal2(), DstNId.GetVal2()); }

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return Network.GetRndNId(Rnd); }
  /// Returns an interator referring to a random node in the graph. Pass in a ModeId of -1
  /// because we know ModeId isn't used in Node lookups.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(TPair<TInt,TInt>(-1, GetRndNId(Rnd))); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const { Network.GetNIdV(NIdV); }

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0; NEdges=0; Network.Clr(); }
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { Network.Reserve(Nodes, Edges); }
  /// Defragments the graph. ##TMultimodalGraphImplA::Defrag
  void Defrag(const bool& OnlyNodeLinks=false) { Network.Defrag(OnlyNodeLinks); }
  /// Checks the graph data structure for internal consistency. ##TMultimodalGraphImplA::IsOk
  bool IsOk(const bool& ThrowExcept=true) const { return Network.IsOk(ThrowExcept); }
  /// Returns a small graph on 10 nodes and 10 edges with 2 modes. ##TMultimodalGraphImplA::GetSmallGraph
  static PMultimodalGraphImplA GetSmallGraph();
};
